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
#include "Util.h"

#define POSITION_COMMAND 4
#define POSITION_PAYLOAD_SIZE 5
#define POSITION_PAYLOAD 7
#define PAYLOAD_SIZE 509
#define CELL_SIZE 514


//
// 5.4.
//
// The payload of a RELAY_TRUNCATED or DESTROY cell contains a single octet,
// describing why the circuit is being closed or truncated.
//

enum cell_destroy_reason : uint8_t
{
	none = 0, // no reason given
	protocol = 1, // tor protocol violation
	internal = 2, // internal error
	requested = 3, // a client sent a TRUNCATE command
	hibernating = 4, // not currently operating; trying to save bandwidth
	resource_limit = 5, // out of memory, sockets, or circuit IDs
	connection_failed = 6, // unable to reach relay
	onion_router_identity = 7, // connected to relay, but its OR identity was not as expected
	onion_router_connection_closed = 8, // the OR connection that was carrying this circuit died
	finished = 9, // the circuit has expired for being dirty or old
	timeout = 10, // circuit construction took too long
	destroyed = 11, // the circuit was destroyed w/o client TRUNCATE
	no_such_service = 12, // request for unknown hidden service
};

enum cell_command : uint8_t
{
	//
	// cell commands.
	//
	padding = 0,
	create = 1,
	created = 2,
	relay = 3,
	destroy = 4,
	create_fast = 5,
	created_fast = 6,
	versions = 7,
	netinfo = 8,
	relay_early = 9,
	create2 = 10,
	created2 = 11,

	//
	// relay commands.
	//
	relay_begin = 1,
	relay_data = 2,
	relay_end = 3,
	relay_connected = 4,
	relay_sendme = 5,
	relay_extend = 6,
	relay_extended = 7,
	relay_truncate = 8,
	relay_truncated = 9,
	relay_drop = 10,
	relay_resolve = 11,
	relay_resolved = 12,
	relay_begin_dir = 13,
	relay_extend2 = 14,
	relay_extended2 = 15,

	//
	// rendezvous commands.
	//
	relay_command_establish_intro = 32,
	relay_command_establish_rendezvous = 33,
	relay_command_introduce1 = 34,
	relay_command_introduce2 = 35,
	relay_command_rendezvous1 = 36,
	relay_command_rendezvous2 = 37,
	relay_command_intro_established = 38,
	relay_command_rendezvous_established = 39,
	relay_command_introduce_ack = 40,

	//
	// variable-length cell commands.
	//
	vpadding = 128,
	certs = 129,
	auth_challenge = 130,
	authenticate = 131,
	authorize = 132,
};

class Cell {
protected:
	unc buffer[CELL_SIZE];
	size_t index;
public:

	Cell(u32 circuit_id, cell_command command);
	Cell();

	void Append(u16 val);
	void Append(u32 val);
	void Append(unc val);
	void Append(string& val);
	void Append(unc* segment, size_t length);

	u32 ReadInt();
	unc ReadByte();
	string ReadString();

	unc* GetBuffer();
	size_t GetBufferSize();

	unc* GetPayload();
	size_t GetPayloadSize();

	unc GetCommand();
	bool IsRelayCell();
	bool IsPaddingCell();
	bool IsVariableLen();
	unc* GetPayloadPart(int begin, int end);
	void SetCommand(u8 command);
	void SetId(u32 circuit_id);
	void SetPayloadSize(u16 payload_size);
  size_t GetCurentIndex();

	virtual ~Cell() {}
};

