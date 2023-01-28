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



#pragma once
#include "Cell.h"
#include "Util.h"

#define BUFF_SIZE 1024

typedef std::function<void(const sys::error_code& error)> ConnectFunction;

class NetConnect
{
	tcp::resolver _resolver;
	ssl::stream<net::ip::tcp::socket> socket;
	net::streambuf request;
	net::streambuf response;

	void ResolvedComplete(ConnectFunction connectFunc, tcp::resolver::iterator endpoint_iter, const sys::error_code& err);
	bool VerifyCertificate(bool preverified, ssl::verify_context& ctx);
	
	void ConnectComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void Handshake(ConnectFunction connectFunc, const sys::error_code& err);

	void SentVersionComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadVersionComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadVersionContentComplete(int len, ConnectFunction connectFunc, const sys::error_code& err);
	
	void ReadHederComplete1(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadHederComplete2(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadCertificatesComplete1(int len, ConnectFunction connectFunc, const sys::error_code& err);
	void ReadCertificatesComplete2(int len, ConnectFunction connectFunc, const sys::error_code& err);

	void ReadHederNetInfoComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadNetInfoComplete(int len, ConnectFunction connectFunc, const sys::error_code& err);

	void WriteCellComplete(ConnectFunction connectFunc, const sys::error_code& err);
	void ReadHeaderComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err);
	void ReadCellComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err);
	void DummyFunc(const sys::error_code& error);

	tuple<u32, u8, u16> ParsingHeader();
	void HexDumpResponse();
public:	
	NetConnect(net::io_service& io_service, ssl::context& context);
	void Connect(const string& host, int port, ConnectFunction connectFunc);
	void SendVersion(ConnectFunction connectFunc);
	void Close();
	tcp::endpoint GetLocalEndpoint();
	long GetEndpointLong();
	void WriteCell(Cell& cell, ConnectFunction connectFunc);
	void ReadCell(shared_ptr<Cell> cell, ConnectFunction connectFunc);	
	void ShutDown();
	~NetConnect();
};

