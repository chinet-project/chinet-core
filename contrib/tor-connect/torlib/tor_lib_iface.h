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

#pragma once 
#include <vector>
#include <boost/log/trivial.hpp>

namespace tools
{
  namespace tor
  {

    struct t_transport_state_notifier
    {
      virtual void notify_state_change(const std::string& state_code, const std::string& details = std::string()) {};
    };


    struct t_tranport
    {
      virtual bool Init(boost::log::trivial::severity_level log_level = boost::log::trivial::info) = 0;
      virtual int Connect(const std::string ip, const int port, const int timeout = 0) = 0;
      virtual bool Close() = 0;
      virtual bool Send(const std::string& buff) = 0;
      virtual bool Receive(std::string& buff, const int timeout = 0) = 0;
      virtual void SetNotifier(t_transport_state_notifier* pnotifier) = 0;
      virtual void TransferExternalConsensus(std::vector<std::string>& local_consensus_data) = 0;
      virtual void WithdrawExternalConsensus(std::vector<std::string>& local_consensus_data) = 0;

    };

  }
}

#define TOR_LIB_STATE_INITIALIZING                  "STATE_INITIALIZING"
#define TOR_LIB_STATE_DOWNLOADING_CONSENSUS         "STATE_DOWNLOADING_CONSENSUS"
#define TOR_LIB_STATE_MAKING_TUNNEL_A               "STATE_MAKING_TUNNEL_A"
#define TOR_LIB_STATE_MAKING_TUNNEL_B               "STATE_MAKING_TUNNEL_B"
#define TOR_LIB_STATE_CREATING_STREAM               "STATE_CREATING_STREAM"
#define TOR_LIB_STATE_FAILED                        "STATE_FAILED"
#define TOR_LIB_STATE_SUCCESS                       "STATE_SUCCESS"








