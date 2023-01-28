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

#include "NetConnect.h"


NetConnect::NetConnect(net::io_service& io_service, ssl::context& context)
	: _resolver(io_service), socket(io_service, context)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::created";
};


void NetConnect::Connect(const string& host, int port, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Connect host="<< host<< " port="<< port;
	tcp::endpoint ep(net::ip::address::from_string(host.c_str()), port);
	_resolver.async_resolve(ep,
		boost::bind(&NetConnect::ResolvedComplete, this, connectFunc, pl::iterator, pl::error));
}

void NetConnect::ResolvedComplete(ConnectFunction connectFunc, 
	tcp::resolver::iterator endpoint_iter, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ResolvedComplete";		
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ResolvedComplete  Resolve OK";
	socket.set_verify_mode(ssl::verify_none);
	//socket.set_verify_mode(ssl::verify_peer);
	//socket.set_verify_callback(
	//	boost::bind(&NetConnect::VerifyCertificate, this, _1, _2));	
	net::async_connect(socket.lowest_layer(), endpoint_iter,
		boost::bind(&NetConnect::ConnectComplete, this, connectFunc, pl::error));
}

void NetConnect::ConnectComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ConnectComplete";
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ConnectComplete Connect OK ";
	socket.async_handshake(ssl::stream_base::client,
		boost::bind(&NetConnect::Handshake, this, connectFunc, pl::error));
}

bool NetConnect::VerifyCertificate(bool preverified, ssl::verify_context& ctx)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::VerifyCertificate";
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::VerifyCertificate Verifying " << subject_name;
	//return preverified;
	return true;
}

void NetConnect::Handshake(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Handshake";
	TEST_ERR(err, connectFunc);
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Handshake  Handshake OK ";
	net::post(net::detail::bind_handler(connectFunc, err));
}

void NetConnect::SendVersion(ConnectFunction connectFunc)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::SendVersion";
	unsigned char versionBytes[] = { 0x00, 0x00, 0x07, 0x00, 0x02, 0x00, 0x04 };
	net::async_write(socket, net::buffer(versionBytes, 7),
		boost::bind(&NetConnect::SentVersionComplete, this, connectFunc, pl::error));
}

void NetConnect::SentVersionComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::SentVersionComplete";
	TEST_ERR(err, connectFunc);	
	net::async_read(socket, response,
		net::transfer_exactly(5),
		boost::bind(&NetConnect::ReadVersionComplete, this, connectFunc, pl::error));
}

tuple<u32, u8, u16> NetConnect::ParsingHeader()
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ParsingHeader";
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());

	u32 id = Util::BigEndianArrayToInt(header_data);
	u8 command = static_cast<u8>(header_data[POSITION_COMMAND]);
	BOOST_LOG_TRIVIAL(debug) << "ParsingHeader::cell_command=" << static_cast<unsigned int>(header_data[POSITION_COMMAND]);
	u16 len = Util::BigEndianArrayToShort(header_data + POSITION_PAYLOAD_SIZE);
	BOOST_LOG_TRIVIAL(debug) << "ParsingHeader::len=" << len;
	return make_tuple(id, command, len);
}

void NetConnect::ReadVersionComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadVersionComplete";
	TEST_ERR(err, connectFunc);	

	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());

	BOOST_LOG_TRIVIAL(debug) << "ReadVersionComplete::cell_command=" << static_cast<unsigned int>(header_data[2]);
	if (header_data[2] != cell_command::versions) {
		BOOST_LOG_TRIVIAL(error) << "Received strange version response cell.";		
		net::post(net::detail::bind_handler(connectFunc, net::error::bad_descriptor));
		return;
	}	
	u16 len = Util::BigEndianArrayToShort(header_data + 3);
	BOOST_LOG_TRIVIAL(debug) << "ReadVersionComplete::len=" << len;
	if (len > 24) {
		BOOST_LOG_TRIVIAL(error) << "Version response length is strangely long.";
		net::post(net::detail::bind_handler(connectFunc, net::error::bad_descriptor));
		return;
	}
	net::async_read(socket, response,
		net::transfer_exactly(len),
		//net::transfer_at_least(len), 
		boost::bind(&NetConnect::ReadVersionContentComplete, this, len, connectFunc, pl::error));
}

void NetConnect::ReadVersionContentComplete(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadVersionContentComplete";
	TEST_ERR(err, connectFunc);

	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_content(std::istreambuf_iterator<char>(response_stream), eos);
	unc* content_data = reinterpret_cast<unc*>(const_cast<char*>(v_content.c_str()));
	Util::HexDump(content_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Certificates 1";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederComplete1, this, connectFunc, pl::error));
}

void NetConnect::ReadHederComplete1(ConnectFunction connectFunc, const sys::error_code& err)
{	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_exactly(get<2>(header)),
		boost::bind(&NetConnect::ReadCertificatesComplete1,this, get<2>(header), connectFunc, pl::error));
}

void NetConnect::ReadCertificatesComplete1(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCertificatesComplete1";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Certificates 2";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederComplete2, this, connectFunc, pl::error));
}

void NetConnect::ReadHederComplete2(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederComplete2";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_exactly(get<2>(header)),
		boost::bind(&NetConnect::ReadCertificatesComplete2, this, get<2>(header), connectFunc, pl::error));
}
void NetConnect::ReadCertificatesComplete2(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCertificatesComplete2";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, len);

	BOOST_LOG_TRIVIAL(debug) << "Read Net Info";
	net::async_read(socket, response,
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHederNetInfoComplete, this, connectFunc, pl::error));
}
void NetConnect::ReadHederNetInfoComplete(ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadHederNetInfoComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();
	net::async_read(socket, response,
		net::transfer_at_least(1),		
		boost::bind(&NetConnect::ReadNetInfoComplete, this, get<2>(header), connectFunc, pl::error));
}
void NetConnect::ReadNetInfoComplete(int len, ConnectFunction connectFunc, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadNetInfoComplet";
	TEST_ERR(err, connectFunc);
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string v_header(std::istreambuf_iterator<char>(response_stream), eos);
	unc* header_data = reinterpret_cast<unc*>(const_cast<char*>(v_header.c_str()));
	Util::HexDump(header_data, v_header.length());
	net::post(net::detail::bind_handler(connectFunc, err));	
}

void NetConnect::Close() {	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::Close";	
}

tcp::endpoint NetConnect::GetLocalEndpoint() {	
	return socket.lowest_layer().local_endpoint();
}
long NetConnect::GetEndpointLong() {
	return socket.lowest_layer().remote_endpoint().address().to_v4().to_ulong();
}

NetConnect::~NetConnect() {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::~NetConnect";
	Close();
}

void NetConnect::DummyFunc(const sys::error_code& error) {
	BOOST_LOG_TRIVIAL(debug) << "DummyFunc";
	if (error)
		BOOST_LOG_TRIVIAL(error) << error.message();
	else
		BOOST_LOG_TRIVIAL(debug) << "No error";
}

void NetConnect::WriteCell(Cell& cell, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::WriteCell";
	unc* buffer = cell.GetBuffer();
	size_t len = cell.GetBufferSize();
	Util::HexDump(buffer, len);
	net::async_write(socket, net::buffer(buffer, len),
		boost::bind(&NetConnect::WriteCellComplete, this, connectFunc, pl::error));
}

void NetConnect::WriteCellComplete(ConnectFunction connectFunc, const sys::error_code& err) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::WriteCellComplete";
	TEST_ERR(err, connectFunc);
	net::post(net::detail::bind_handler(connectFunc, err));
}

void NetConnect::ReadCell(shared_ptr<Cell> cell, ConnectFunction connectFunc) {
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCell";
	// Read heder
	net::async_read(socket, response, 
		net::transfer_exactly(POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadHeaderComplete, this, connectFunc, cell, pl::error));
}

void NetConnect::ReadHeaderComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete";
	TEST_ERR(err, connectFunc);
	tuple<u32, u8, u16> header = ParsingHeader();		
	cell->SetId(get<0>(header));
	cell->SetCommand(get<1>(header));
	cell->SetPayloadSize(get<2>(header));
	net::async_read(socket, response,
		net::transfer_exactly(CELL_SIZE- POSITION_PAYLOAD),
		boost::bind(&NetConnect::ReadCellComplete, this, connectFunc, cell, pl::error));
}
void NetConnect::ReadCellComplete(ConnectFunction connectFunc, shared_ptr<Cell> cell, const sys::error_code& err)
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete";
	TEST_ERR(err, connectFunc);	
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string data_str(std::istreambuf_iterator<char>(response_stream), eos);	
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ReadCellComplete len=" << data_str.length();
	cell->Append(data_str);	
	BOOST_LOG_TRIVIAL(debug) << "----------------------CELL Received-------------------";
	Util::HexDump(cell->GetBuffer(), cell->GetBufferSize());
	BOOST_LOG_TRIVIAL(debug) << "------------------------------------------------------";	
	
	net::post(net::detail::bind_handler(connectFunc, err));	
}
void NetConnect::HexDumpResponse()
{
	std::istream response_stream(&response);
	std::istreambuf_iterator<char> eos;
	std::string data_str(std::istreambuf_iterator<char>(response_stream), eos);
	unc* res_data = reinterpret_cast<unc*>(const_cast<char*>(data_str.c_str()));
	Util::HexDump(res_data, data_str.length());
}
void NetConnect::ShutDown()
{
	BOOST_LOG_TRIVIAL(debug) << "NetConnect::ShutDown";
	socket.shutdown(); 
}