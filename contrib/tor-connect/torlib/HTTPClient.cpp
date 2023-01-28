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



#include "HTTPClient.h"

// Start the asynchronous operation
void HTTPClient::RunClient(const string host, const int port, const string target, const int timeout, int version)
{
    error_operations = false;
    timeout_op = timeout;
    // Set up an HTTP GET request message
    req_.version(version);
    req_.method(http::verb::get);
    req_.target(target);
    req_.set(http::field::host, host);
    req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Look up the domain name
    resolver_.async_resolve(host, std::to_string(port),
        beast::bind_front_handler(&HTTPClient::OnResolve, shared_from_this()));
}

void HTTPClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnResolve");
    
    // Set a timeout on the operation
    if (timeout_op > 0) 
      stream_.expires_after(std::chrono::seconds(timeout_op));
    
    // Make the connection on the IP address we get from a lookup
    stream_.async_connect(results, 
        beast::bind_front_handler(&HTTPClient::OnConnect, shared_from_this()));
}

void HTTPClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnConnect");

    // Set a timeout on the operation
    if(timeout_op > 0) 
      stream_.expires_after(std::chrono::seconds(timeout_op));

    http::async_write(stream_, req_,
        beast::bind_front_handler(&HTTPClient::OnWrite, shared_from_this()));
}

void HTTPClient::OnWrite(beast::error_code ec, std::size_t /*bytes_transferred*/)
{
    if (ec) 
      return OnFail(ec, "HTTPClient::OnWrite");

    http::async_read(stream_, buffer_, res_,
        beast::bind_front_handler(&HTTPClient::OnRead, shared_from_this()));
}

void HTTPClient::OnRead(beast::error_code ec, std::size_t /*bytes_transferred*/)
{

    if (ec) 
      return OnFail(ec, "HTTPClient::OnRead");
    
    stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
    
    if (ec && ec != beast::errc::not_connected)
        return OnFail(ec, "HTTPClient shutdown");
}


void HTTPClient::OnFail(beast::error_code ec, char const* what)
{
    BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message(); 
    res_.clear();
    error_operations = true;
}

string HTTPClient::GetData()
{
    string ret_str = "";
    if (!error_operations)
        return std::move(res_.body().data());
    return ret_str;
}