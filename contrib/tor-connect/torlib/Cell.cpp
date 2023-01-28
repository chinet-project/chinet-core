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



#include "Cell.h"

Cell::Cell(u32 circuit_id, cell_command command) {
	memset(buffer, 0, sizeof(buffer));
	Util::Int32ToArrayBigEndian(buffer, circuit_id);
	buffer[POSITION_COMMAND] = command;	
	index = POSITION_PAYLOAD_SIZE;
}
Cell::Cell() {
	index = POSITION_PAYLOAD;
}

unsigned char Cell::GetCommand() {
	return buffer[POSITION_COMMAND];
}

void Cell::Append(u16 val) {
	Util::Int16ToArrayBigEndian(buffer + index, val);
	index += 2;
}

void Cell::Append(u32 val) {
	Util::Int32ToArrayBigEndian(buffer + index, val);
	index += 4;
}

void Cell::Append(unsigned char val) {
	buffer[index++] = val;
}

void Cell::Append(string& val) {
	memcpy(buffer + index, reinterpret_cast<unc*>(const_cast<char*>(val.c_str())), val.length());
	index += val.length();
}

void Cell::Append(unc* segment, size_t length) {
	memcpy(buffer + index, segment, length);
	index += length;
}

u32 Cell::ReadInt() {
	u32 val = Util::BigEndianArrayToInt(buffer + index);
	index += 4;
	return val;
}

unsigned char Cell::ReadByte() {
	return buffer[index++];
}

string Cell::ReadString() {
	unc len = buffer[index++];
	string val(reinterpret_cast<const char*>(buffer)+ index, static_cast<size_t>(len));
	index += static_cast<int>(len);
	return val;
}

unsigned char* Cell::GetPayload() {
	return buffer + POSITION_PAYLOAD_SIZE;
}

size_t Cell::GetPayloadSize() {
	return sizeof(buffer) - POSITION_PAYLOAD_SIZE;
}

unsigned char* Cell::GetBuffer() {
	return buffer;
}

size_t Cell::GetBufferSize() {
	if (IsVariableLen())
		return sizeof(buffer);
	else return CELL_SIZE;
}
size_t Cell::GetCurentIndex() {
	return index;
}

bool Cell::IsRelayCell() {
	return GetCommand() == cell_command::relay;
}

bool Cell::IsPaddingCell() {
	return GetCommand() == cell_command::padding;
}
bool Cell::IsVariableLen() {
	return GetCommand() == cell_command::versions || GetCommand() >= 128;
}
unc* Cell::GetPayloadPart(int begin, int end) {	
	size_t len = end-begin;	
	if (len <= 0|| (begin+len)>=CELL_SIZE) return nullptr;
	unc* segment = static_cast<unc*>(malloc(len));
	memcpy(segment, buffer + begin, len);
	return segment;
}
void Cell::SetCommand(u8 command) {
	buffer[POSITION_COMMAND] = command;
}
void Cell::SetId(u32 circuit_id) {
	
	uint8_t d[4] = { 0 };
	for (int i = 0; i < 4; ++i)
		d[i] = (reinterpret_cast<u8*>(&circuit_id))[3 - i];	
	memcpy(buffer,d, 4);
}
void Cell::SetPayloadSize(u16 payload_size)
{
	uint8_t d[2] = { 0 };
	for (int i = 0; i < 2; ++i)
		d[i] = (reinterpret_cast<u8*>(&payload_size))[1 - i];
	memcpy(buffer + POSITION_PAYLOAD_SIZE, d, 2);
}