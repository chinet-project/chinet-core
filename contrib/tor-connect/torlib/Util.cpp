/*-
 * Copyright (c) 2021, Chinet project, https://chinet.io
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright (c) 2009, Moxie Marlinspike
 * Copyright (c) 2008, John Millikin,  https://stackoverflow.com/a/180949/5566653
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

#include "Util.h"

static const string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

uint16_t Util::BigEndianArrayToShort(unc* buf) {
	return (uint16_t)((buf[0] & 0xff) << 8 | (buf[1] & 0xff));
}

uint32_t Util::BigEndianArrayToInt(unc* buf) {
	return (uint32_t)((buf[0] & 0xff) << 24 | (buf[1] & 0xff) << 16 |
		(buf[2] & 0xff) << 8 | (buf[3] & 0xff));
}

void Util::Int64ToArrayBigEndian(unc* a, u64 i) {
	a[0] = (i >> 56) & 0xFF;
	a[1] = (i >> 48) & 0xFF;
	a[2] = (i >> 40) & 0xFF;
	a[3] = (i >> 32) & 0xFF;
	a[4] = (i >> 24) & 0xFF;
	a[5] = (i >> 16) & 0xFF;
	a[6] = (i >> 8) & 0xFF;
	a[7] = i & 0xFF;
}


void Util::Int32ToArrayBigEndian(unc* a, u32 i) {
	a[0] = (i >> 24) & 0xFF;
	a[1] = (i >> 16) & 0xFF;
	a[2] = (i >> 8) & 0xFF;
	a[3] = i & 0xFF;
}

void Util::Int16ToArrayBigEndian(unc* a, u16 i) {
	a[0] = (i >> 8) & 0xff;
	a[1] = i & 0xff;
}
void Util::HexDump(unc* buffer, size_t length) {
	stringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.fill('0');

	for (size_t i = 0; i < length; ++i) {
		str << std::setw(2) << static_cast<unsigned short>(buffer[i]) << " ";
	}
	BOOST_LOG_TRIVIAL(debug) << str.str();
}

void Util::Base16Encode(char* dest, size_t destlen, const char* src, size_t srclen)
{
	const char* end;
	char* cp;

	assert(destlen >= srclen * 2 + 1);

	cp = dest;
	end = src + srclen;
	while (src < end) {
		*cp++ = "0123456789ABCDEF"[(*(const uint8_t*)src) >> 4];
		*cp++ = "0123456789ABCDEF"[(*(const uint8_t*)src) & 0xf];
		++src;
	}
	*cp = '\0';
}
uint16_t Util::GetRandomId() {
	unc id[2];
	RAND_bytes(id, sizeof(id));

	return BigEndianArrayToShort(id);
}

uint32_t Util::GetRandom() {
	unc bytes[4];
	RAND_bytes(bytes, sizeof(bytes));

	return BigEndianArrayToInt(bytes);
}
uint8_t Util::GetRandom8() {
	unc buf[1];
	RAND_bytes(buf, sizeof(buf));
	return buf[0] & 0xff;
}

bool Util::IsBase64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string Util::Base64Decode(string const& encoded_string) {
	size_t in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && IsBase64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}