/* This is CMake-template for libmdbx's config.h
 ******************************************************************************/

/* *INDENT-OFF* */
/* clang-format off */

#define HAVE_VALGRIND_MEMCHECK_H
/* #undef HAS_RELAXED_CONSTEXPR */

/* #undef LTO_ENABLED */
/* #undef MDBX_USE_VALGRIND */
/* #undef ENABLE_GPROF */
/* #undef ENABLE_GCOV */
/* #undef ENABLE_ASAN */

/* Common */
#define MDBX_TXN_CHECKPID 0
#define MDBX_TXN_CHECKOWNER 1
#define MDBX_BUILD_SHARED_LIBRARY 0

/* Windows */
#define MDBX_CONFIG_MANUAL_TLS_CALLBACK 0
#define MDBX_AVOID_CRT 0

/* MacOS */
#define MDBX_OSX_SPEED_INSTEADOF_DURABILITY 0

/* POSIX */
#define MDBX_USE_ROBUST 0
#define MDBX_USE_OFDLOCKS 0
#define MDBX_DISABLE_GNU_SOURCE 0

/* Simulate "AUTO" values of tristate options */
/* #undef MDBX_TXN_CHECKPID_AUTO */
#ifdef MDBX_TXN_CHECKPID_AUTO
#undef MDBX_TXN_CHECKPID
#endif
/* #undef MDBX_USE_ROBUST_AUTO */
#ifdef MDBX_USE_ROBUST_AUTO
#undef MDBX_USE_ROBUST
#endif
/* #undef MDBX_USE_OFDLOCKS_AUTO */
#ifdef MDBX_USE_OFDLOCKS_AUTO
#undef MDBX_USE_OFDLOCKS
#endif

/* Build Info */
#define MDBX_BUILD_TIMESTAMP "2023-01-28T22:25:36Z"
#define MDBX_BUILD_TARGET "x86_64-linux-gnu-Linux"
#define MDBX_BUILD_CONFIG "Release"
#define MDBX_BUILD_COMPILER "cc (Ubuntu 5.5.0-12ubuntu1~16.04) 5.5.0 20171010"
#define MDBX_BUILD_FLAGS "-O3 -DNDEBUG -Ofast -DNDEBUG -Wno-unused-variable -flto -ffast-math -fvisibility=hidden"
#define MDBX_BUILD_SOURCERY 957e2b39d570c206069f958f5013b7c67c39e0cbe1f3fe599f3d0091afe61bd8_

/* *INDENT-ON* */
/* clang-format on */
