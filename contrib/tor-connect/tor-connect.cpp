/*-
 * Copyright (c) 2021, Chinet project, https://chinet.io
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
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


#include "torlib/torlib.h"

#include "epee/include/include_base_utils.h"
#include "epee/include/net/levin_client.h"
#include "epee/include/serialization/keyvalue_serialization.h"
#include "epee/include/storages/portable_storage_template_helper.h"
#include "torlib/tor_wrapper.h"

namespace tools
{
  typedef epee::levin::levin_client_impl2<tools::tor::tor_transport> levin_over_tor_client;
}

struct NOTIFY_NEW_TRANSACTIONS
{
  const static int ID = 1001;

  struct request
  {
    std::list<std::string>   txs;

    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE(txs)
    END_KV_SERIALIZE_MAP()
  };
};


int main()
{
  epee::log_space::get_set_log_detalisation_level(true, LOG_LEVEL_0);
  epee::log_space::get_set_need_thread_id(true, true);
  epee::log_space::log_singletone::add_logger(LOGGER_CONSOLE, NULL, NULL);


  while (true)
  {
    tools::levin_over_tor_client p2p_client;
    if (!p2p_client.connect("144.76.183.143", 1001, 5000))
    {
      LOG_PRINT_RED_L0("Failed to connect");
      continue;;
    }

    std::string buff1 = "123123123123sdfdcfs";
    std::string buff2 = "jindkwjdniwjenwjnwkjnkjnkjn";


    p2p_client.get_transport().send(buff1);
    p2p_client.get_transport().send(buff2);

    std::string buff_res;
    if (!p2p_client.get_transport().recv_n(buff_res, buff1.size() + buff2.size()))
    {
      LOG_ERROR("Failed to recv given amount of data back");
      p2p_client.disconnect();
      continue;
    }

    p2p_client.disconnect();
    LOG_PRINT_GREEN_L0("Success interactions done, closing connection.");
  }
  //epee::net_utils::levin_client2 p2p_client;

  return 1;




//   NOTIFY_NEW_TRANSACTIONS::request p2p_req = AUTO_VAL_INIT(p2p_req);
//   std::string blob = "somebinarywillbehere";
//   epee::serialization::store_t_to_binary(p2p_req, blob);
//   p2p_client.notify(NOTIFY_NEW_TRANSACTIONS::ID, blob);
//   p2p_client.disconnect();
//   return 0;
//     //setlocale(LC_CTYPE, "rus"); // set locale
//     TorLib tlb;
//     // track your own variable here
//     //log::trivial::severity_level log_level = log::trivial::info;
//     tlb.Init();
//     int err_connect = tlb.Connect("ifconfig.me", 80, 0);
//     if (err_connect == 0)
//     {
//         tlb.Send("/");
//         string buff;
//         tlb.Receive(buff);
//         std::cout << "Data received :" << buff;
//     }        
//     else std::cout << "Error connect =" << err_connect;
//     tlb.Close();
}
