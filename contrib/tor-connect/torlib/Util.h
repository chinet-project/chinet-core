/*-
 * Copyright (c) 2021, Chinet project, https://chinet.io
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of this program nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */




#ifndef __UTIL_H__
#define __UTIL_H__
 
#include <memory>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <locale>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <array>
#include <deque>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <iterator>
#include <numeric>
#include <time.h>
#include <sys/types.h>

#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/modes.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/format.hpp>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t limb;
typedef unsigned char unc;
typedef unsigned int uni;

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
namespace pl = net::placeholders;
namespace sys = boost::system;
namespace ssl = boost::asio::ssl;
namespace fs = boost::filesystem;
using timer = boost::asio::steady_timer;
using log_lv = boost::log::trivial::severity_level;
using sec = boost::posix_time::seconds;


using std::string;
using std::vector;
using std::shared_ptr;
using std::array;
using std::map;
using std::copy;
using std::reverse;
using std::make_tuple;
//using std::make_unique; 
using std::tuple;
using std::min;
using std::ifstream;
using std::get;
using std::stringstream;
using std::pair;
using std::unique_ptr;
using std::make_shared;

#define  TEST_ERR(err,connectFunc) if (err) {net::post(net::detail::bind_handler(connectFunc, err));return;}

class Util {
private:
 public:
  static void Int64ToArrayBigEndian(unc*a, u64 i);
  static void Int32ToArrayBigEndian(unc*a, u32 i);
  static void Int16ToArrayBigEndian(unc*a, u16 i);
  static uint16_t BigEndianArrayToShort(unc*buf);
  static uint32_t BigEndianArrayToInt(unc*buf);
  static void HexDump(unc* buf, size_t length);
  static void Base16Encode(char *dest, size_t destlen, const char *src, size_t srclen);
  static uint16_t GetRandomId();
  static uint32_t GetRandom();  
  static uint8_t GetRandom8();
  static bool IsBase64(unsigned char c);
  static string Base64Decode(string const& encoded_string);
};

#endif
