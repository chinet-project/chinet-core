cmake_minimum_required (VERSION 3.8)

project("tor-connect")

SET(Boost_USE_STATIC_LIBS ON)           # link statically


if(MSVC)
  add_definitions("/D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x0600 /DWIN32_LEAN_AND_MEAN")
endif()


add_definitions("/DHTTP_ENABLE_GZIP")


file(GLOB_RECURSE TOR_LIB_FILES torlib/*.cpp torlib/*.h)    

add_library(tor-connect "tor-connect.cpp" "tor-connect.h" ${TOR_LIB_FILES} )

#target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

find_package(Boost 1.70 COMPONENTS log regex REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS}) 
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set(OPENSSL_USE_STATIC_LIBS TRUE) # link statically
find_package(OpenSSL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} Crypt32)
endif()

message(STATUS "SSL INCLUDE DIRS: ${OPENSSL_INCLUDE_DIR}")