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
#include "Util.h"
#include "Cell.h"
#include "RelayCell.h"

static constexpr uint8_t const_server[] = {
  'S', 'e', 'r', 'v', 'e', 'r'
};

static constexpr uint8_t const_protoid[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1'
};

static constexpr uint8_t const_t_mac[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'm', 'a', 'c'
};

static constexpr uint8_t const_t_key[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 't', 'r', 'a', 'c', 't'
};

static constexpr uint8_t const_t_verify[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'v', 'e', 'r', 'i', 'f', 'y'
};

static constexpr uint8_t const_m_expand[] = {
  'n', 't', 'o', 'r', '-', 'c', 'u', 'r', 'v', 'e', '2', '5', '5', '1', '9', '-', 's', 'h', 'a', '2', '5', '6', '-', '1',
  ':', 'k', 'e', 'y', '_', 'e', 'x', 'p', 'a', 'n', 'd'
};


#define TOTAL_KEY_MATERIAL (20*3+16*2)
#define DIGEST_LEN 20
#define KEY_LEN 128/8

#define KEY_SIZE 32

class OnionRouter
{
private:
	SHA_CTX forward_digest;
	SHA_CTX backward_digest;

	AES_KEY forward_key;
	AES_KEY backward_key;

	unc forward_iv[AES_BLOCK_SIZE];
	unc forward_ec[AES_BLOCK_SIZE];
	uni forward_num;

	unc back_iv[AES_BLOCK_SIZE];
	unc back_ec[AES_BLOCK_SIZE];
	uni back_num;
	
	int identity_key_len=0;
	RSA* GetKey(std::string current_key);

	pair<vector<u8>, vector<u8>> pair_key;
	pair<vector<u8>, vector<u8>> pair_key2;
	
	const u8 basepoint_9[32] = { 9 };

	void AesEncrypt(Cell& cell, AES_KEY* key, unc* iv, unc* ec, uni* num);	
	void SetDigest(RelayCell& cell);
	void VerifyDigest(RelayCell& cell);
	void CalculateDigest(SHA_CTX* digest, RelayCell& cell, unc* result);
	bool GeneratKeyMaterialUnc(unc* handshake_data, unc* other_public_key, unc* verify);	
public:
	OnionRouter();
	string nickname;
	string identity;
	string digest;
	string publication_date;
	string publication_time;
	string ip;
	int or_port;
	int dir_port;
	//
	// router_status_entry_r_item_count = 9
	//
	int item_count;

	string onion_key;
	string signing_key;
	string ntor_onion_key;
	string service_key; // for introduction point

	vector<u8> key_material;
	
	string GetBase16EncodedIdentity(string identity_key);
	bool SetOnionKey(string current_key);
	bool SetSignigKey(string current_key);
	bool SetNtorOnionKey(string current_key);
	bool SetServiceKey(string current_key);
	void GeneratPairKeys();
	u8* GetPublicKey();
	size_t GetPublicKeySize();
	bool GeneratKeyMaterial(shared_ptr<Cell> node);
	bool GeneratKeyMaterial(RelayCell& node);
	void SetKeyMaterial(unc* key_material);
	void Encrypt(RelayCell& cell, bool set_digest=true);
	void Decrypt(RelayCell& cell, bool verify_digest=true);
	
	string ConvertToString(char* a, int size);

};

