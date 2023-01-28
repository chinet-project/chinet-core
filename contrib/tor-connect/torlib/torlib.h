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

#include "Parser.h"
#include "NetConnect.h"
#include "Util.h"
#include "Cell.h"
#include "tor_lib_iface.h"

enum class link_specifier_type : u8
{
	ipv4 = 0,
	ipv6 = 1,
	legacy_id = 2,
};


class TorLib: public tools::tor::t_tranport
{
private:	
	bool GetConsensus();
	bool GetKeysNode(int n_node);

  bool ConnectToNode(int n_node, int search_port = 0);

	//int connection_handle;
	vector<tuple<string, string, int, int>> DA;
	Parser parser;
	
	//string GetDataFromUrl(const string host, const int port, const string target);
	string GetDataFromUrlAsync(const string host, const int port, const string target);

	net::io_service io_service;
	shared_ptr<net::io_service::work> work;
	unique_ptr<net::deadline_timer> dtimer;
	unique_ptr<NetConnect> net_connect;
	bool operation_completed = false;
	bool error_last_operation = false;
	map<int, shared_ptr<OnionRouter>> onion_routers;

	bool SendNodeInfo(ConnectFunction connectFunc);
	void ReadNodeInfo(ConnectFunction connectFunc, const sys::error_code& err);
	void SendInfoComplete(ConnectFunction connectFunc, shared_ptr<Cell> node_info, const sys::error_code& err);	
	void LogErr(const sys::error_code& err);
	
	bool CreateNtor(int n_node, ConnectFunction connectFunc);
	void ReadCNtor(int n_node, ConnectFunction connectFunc, const sys::error_code& err);	
	void CreateNtorComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node_info, const sys::error_code& err);

	bool CreateExtendNtor(int n_node, ConnectFunction connectFunc);
	void ReadExtendNtor(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void CreateExtendNtorComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);

	bool CreateStream(int n_node, u16 id_stream, string host, int port, int timeout, ConnectFunction connectFunc);
	void ReadStreamNode(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void CreateStreamComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);

	string stream_host;
	int stream_port = 0;
	int n_stream = 0;
  int timeout_global = 0;
	vector<std::string> data_consensus;
	string data_result;
  uint64_t last_consensus_receive_time = 0;

	u32 circuit_id = 1;
  tools::tor::t_transport_state_notifier notifier_dummy;
  tools::tor::t_transport_state_notifier* pnotifier;

	bool SendData(string reqest/* , ConnectFunction connectFunc*/);
	void ReadStreamData(int n_node, ConnectFunction connectFunc, const sys::error_code& err);
	void ReadStreamOne(int n_node, ConnectFunction connectFunc);		
	void ReadStreamComplete(int n_node, ConnectFunction connectFunc, shared_ptr<Cell> node, const sys::error_code& err);	
	void OnTimeout(const sys::error_code& err);
public:
  TorLib();
	~TorLib();
  // ------------- t_tranport ------------- 
  virtual bool Init(log_lv log_level = boost::log::trivial::info);
  virtual int Connect(const string ip, const int port, const int timeout=0);
  virtual bool Close();
  virtual bool Send(const string& path);
  virtual bool Receive(string& buff, const int timeout=0);
  virtual void SetNotifier(tools::tor::t_transport_state_notifier* pnotifier);
  virtual void TransferExternalConsensus(std::vector<std::string>& local_consensus_data);
  virtual void WithdrawExternalConsensus(std::vector<std::string>& local_consensus_data);
};

