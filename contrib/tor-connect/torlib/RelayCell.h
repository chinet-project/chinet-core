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
#include "Cell.h"
#include "Util.h"

#define TOR_MAX_CELL_PAYLOAD_DATA 117

#define DIGEST_OFFSET 10
#define DIGEST_LENGTH 4
#define RELAY_PAYLOAD_OFFSET 14
#define RELAY_BYTES_LEN 2
#define HANDSHAKE_DATA 18
#define RELAY_TYPE_OFFSET 5
#define STREAM_ID_OFFSET 7

#define MAX_PAYLOAD_LENGTH 512 - 14

#define RELAY_END_TYPE 0x03

class RelayCell :
    public Cell
{
private:
    void AppendData(u16 streamId, cell_command relay_command, size_t length);
public:
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command);
    RelayCell(u16 streamId, cell_command command, cell_command relay_command);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc* data, int length);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, string& data, bool nullTerminatd);
    RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc payload);
    RelayCell(Cell& cell);
    void SetDigest(unc* digest);
    void GetDigest(unc* buf);
    unc* GetRelayPayload();
    int GetRelayPayloadLength();
    unc GetRelayType();
    bool IsRelayEnd();
    bool SetLengthRelayPayload(size_t size_data);
    u16 GetStreamId();
};

