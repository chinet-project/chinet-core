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


#include "RelayCell.h"

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, 0);
}

RelayCell::RelayCell(u16 streamId, cell_command command, cell_command relay_command)
    : Cell(0, command)
{
    AppendData(streamId, relay_command, 0);
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc* data, int length)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, length);
    Append(data, length);
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, string& data, bool null_term)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, null_term ? data.length() + 1 : data.length());
    Append(data);
    if (null_term) Append(static_cast<unc>('\0'));
}

RelayCell::RelayCell(u32 circuitId, u16 streamId, cell_command command, cell_command relay_command, unc payload)
    : Cell(circuitId, command)
{
    AppendData(streamId, relay_command, 1);
    Append(payload);
}

RelayCell::RelayCell(Cell& cell) :Cell()
{
    copy(cell.GetBuffer(), cell.GetBuffer() + GetBufferSize(), GetBuffer());
}

void RelayCell::AppendData(u16 streamId, cell_command relay_command, size_t length) {
    Append(static_cast<unc>(relay_command));                // relay command                    1 byte
    Append(static_cast<u16>(0));                            // recognized                       2 bytes
    Append(streamId);                                       // steram id                        2 bytes
    Append(static_cast<u32>(0));                            // digest placeholder (0 for now)   4 bytes
    Append(static_cast<u16>(0x77));                         // length                           2 bytes
    // Total 11 bytes
}

bool RelayCell::SetLengthRelayPayload(size_t size_data_arg) {
    if(size_data_arg>TOR_MAX_CELL_PAYLOAD_DATA)
    {
        BOOST_LOG_TRIVIAL(error) << "The length of the data is longer than the maximum length of the cell";
        return false;
    }    

    u16 size_data = static_cast<u16>(size_data_arg);
    unc len_payload[RELAY_BYTES_LEN];
    Util::Int16ToArrayBigEndian(len_payload, size_data);
    memcpy(GetBuffer() + RELAY_PAYLOAD_OFFSET, len_payload, RELAY_BYTES_LEN);
    return true;
}

void RelayCell::SetDigest(unc* digest) {
    copy(digest, digest+ DIGEST_LENGTH, GetBuffer() + DIGEST_OFFSET);    
}

void RelayCell::GetDigest(unc* buf) {
    copy(buf, buf + DIGEST_LENGTH, GetBuffer() + DIGEST_OFFSET);
}

unc* RelayCell::GetRelayPayload() {
    return GetBuffer() + RELAY_PAYLOAD_OFFSET + RELAY_BYTES_LEN;
}

int RelayCell::GetRelayPayloadLength() {
    return IsRelayEnd() ? -1 : static_cast<int>(Util::BigEndianArrayToShort(GetBuffer() + RELAY_PAYLOAD_OFFSET));
}

unc RelayCell::GetRelayType() {
    return GetBuffer()[RELAY_TYPE_OFFSET];
}

bool RelayCell::IsRelayEnd() {
    return GetRelayType() == RELAY_END_TYPE;
}

u16 RelayCell::GetStreamId() {
    return Util::BigEndianArrayToShort(GetBuffer() + STREAM_ID_OFFSET);
}


