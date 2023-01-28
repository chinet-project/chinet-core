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

#include <string>
#include <memory>
#include "tor_lib_iface.h"

namespace tools
{
  namespace tor
  {
    class tor_transport
    {
    public:

      tor_transport();
      bool connect(const std::string& ip, int port, int timeout, int recev_timeout, const std::string& bind_ip);
      bool is_connected();
      bool send(const void* data, size_t sz);
      bool send(const std::string& buff);
      bool recv_n(std::string& buff, size_t sz);
      bool disconnect();
      void set_notifier(tools::tor::t_transport_state_notifier* pn);

    private:
      std::shared_ptr<tools::tor::t_tranport> m_ptransport;
      bool m_is_connected;
      int m_recev_timeout;
      std::string m_intermediate_buff;
      tools::tor::t_transport_state_notifier* m_pn;
    };
  }
}

