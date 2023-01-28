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

#include "OnionRouter.h"
#include "Curve25519.h"

OnionRouter::OnionRouter() {    
    memset(forward_iv, 0, sizeof(forward_iv));
    memset(forward_ec, 0, sizeof(forward_ec));
    memset(back_iv, 0, sizeof(back_iv));
    memset(back_ec, 0, sizeof(back_ec));
    forward_num = 0;
    back_num = 0;
    SHA1_Init(&forward_digest);
    SHA1_Init(&backward_digest);
}

string OnionRouter::GetBase16EncodedIdentity(string identity_key) {    
    size_t encoded_len = identity_key.length() * 2 + 1;
    unique_ptr<char[]> encoded = std::unique_ptr<char[]>(new char[encoded_len]);
    Util::Base16Encode(encoded.get(), encoded_len, identity_key.c_str(), identity_key.length());
    string ret(encoded.get());
    return ret;
}

RSA* OnionRouter::GetKey(string str_key) {
    if (str_key.length() == 0) return NULL;
    RSA* key = RSA_new();
    BIO* publicKeyBio = BIO_new_mem_buf(str_key.c_str(), static_cast<int>(str_key.length()));
    if (publicKeyBio == NULL) return NULL;
    key = PEM_read_bio_RSAPublicKey(publicKeyBio, &key, NULL, NULL);
    if (key == NULL) return NULL;
    return key;
}

bool OnionRouter::SetOnionKey(string current_key) {    
    onion_key= Util::Base64Decode(current_key);
    if (onion_key.length()==0) return false;
    return true;
}
bool OnionRouter::SetSignigKey(string current_key) {
    signing_key = Util::Base64Decode(current_key);
    if (signing_key.length() == 0) return false;
    return true;
}
bool OnionRouter::SetNtorOnionKey(string current_key) {
    ntor_onion_key = Util::Base64Decode(current_key);
    if (ntor_onion_key.length() == 0) return false;
    return true;
}
bool OnionRouter::SetServiceKey(string current_key) {
    service_key = Util::Base64Decode(current_key);
    if (service_key.length() == 0) return false;
    return true;
}
void OnionRouter::GeneratPairKeys()
{
    vector<uint8_t> onion_skin_ntor;
    for (int i = 0; i < 32; i++)
    {
        pair_key.second.push_back(Util::GetRandom8());
        pair_key.first.push_back(0);
    }
    pair_key.second[0] &= 248;
    pair_key.second[31] &= 127;
    pair_key.second[31] |= 64;
    Curve25519::curve25519_donna(pair_key.first.data(), pair_key.second.data(), basepoint_9);
    BOOST_LOG_TRIVIAL(debug) << "------------- public key --------------";
    Util::HexDump(pair_key.first.data(), pair_key.first.size());
    BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "------------- private key -------------";
    Util::HexDump(pair_key.second.data(), pair_key.second.size());
    BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";
}

u8* OnionRouter::GetPublicKey()
{    
    return pair_key.first.data();
}

size_t OnionRouter::GetPublicKeySize()
{
    return pair_key.first.size();
}

bool OnionRouter::GeneratKeyMaterialUnc(unc* handshake_data, unc* other_public_key, unc* verify)
{
    BOOST_LOG_TRIVIAL(debug) << "-------------other_public_key-------------";
    Util::HexDump(other_public_key, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "-------------verify-----------------------";
    Util::HexDump(verify, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
    BOOST_LOG_TRIVIAL(debug) << "-------------ntor_onion_key---------------";
    const unc* ntor = reinterpret_cast<const unc*>(ntor_onion_key.c_str());
    Util::HexDump(const_cast<unc*>(ntor), KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";


    unc shared_key1[KEY_SIZE];
    Curve25519::curve25519_donna(&shared_key1[0], pair_key.second.data(), other_public_key);
    BOOST_LOG_TRIVIAL(debug) << "-------------shared_key1-------------";
    Util::HexDump(shared_key1, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";

    unc shared_key2[KEY_SIZE];
    Curve25519::curve25519_donna(&shared_key2[0], pair_key.second.data(),
        reinterpret_cast<const unc*>(ntor_onion_key.c_str()));

    BOOST_LOG_TRIVIAL(debug) << "-------------shared_key2-------------";
    Util::HexDump(shared_key2, KEY_SIZE);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";

    vector<unc> secret_input;
    copy(shared_key1, shared_key1 + KEY_SIZE, back_inserter(secret_input));
    copy(shared_key2, shared_key2 + KEY_SIZE, back_inserter(secret_input));
    copy(identity.begin(), identity.end(), back_inserter(secret_input));
    copy(ntor_onion_key.begin(), ntor_onion_key.end(), back_inserter(secret_input));
    copy(pair_key.first.begin(), pair_key.first.end(), back_inserter(secret_input));
    copy(other_public_key, other_public_key + KEY_SIZE, back_inserter(secret_input));
    copy(const_protoid, const_protoid + sizeof(const_protoid), back_inserter(secret_input));    

    BOOST_LOG_TRIVIAL(debug) << "-------------secret_input-------------";
    Util::HexDump(secret_input.data(), secret_input.size());
    BOOST_LOG_TRIVIAL(debug) << "--------------------------------------";

    unc* result = nullptr;
    uni resultlen = -1;
    unc* verify_comp = HMAC(EVP_sha256(), const_t_verify, sizeof(const_t_verify), secret_input.data(), secret_input.size(), result, &resultlen);

    vector<unc> auth_input;
    copy(verify_comp, verify_comp + KEY_SIZE, back_inserter(auth_input));
    copy(identity.begin(), identity.end(), back_inserter(auth_input));
    copy(ntor_onion_key.begin(), ntor_onion_key.end(), back_inserter(auth_input));
    copy(other_public_key, other_public_key + KEY_SIZE, back_inserter(auth_input));
    copy(pair_key.first.begin(), pair_key.first.end(), back_inserter(auth_input));
    copy(const_protoid, const_protoid + sizeof(const_protoid), back_inserter(auth_input));
    copy(const_server, const_server + sizeof(const_server), back_inserter(auth_input));
    result = nullptr;
    resultlen = -1;
    unc* verify_data = HMAC(EVP_sha256(), const_t_mac, sizeof(const_t_mac), auth_input.data(), auth_input.size(), result, &resultlen);

    if (memcmp(verify, verify_data, resultlen) == 0)
    {
        // create key material   
        result = nullptr;
        resultlen = -1;
        unc* ps_key = HMAC(EVP_sha256(), const_t_key, sizeof(const_t_key), secret_input.data(), secret_input.size(), result, &resultlen);
        vector<u8> pseudo_random_key;
        copy(ps_key, ps_key + resultlen, back_inserter(pseudo_random_key));

        BOOST_LOG_TRIVIAL(debug) << "-------------secret_input-------------";
        Util::HexDump(ps_key, resultlen);
        BOOST_LOG_TRIVIAL(debug) << "---------------------------------------";

        unc* result_block = nullptr;
        size_t bytes_remaining = TOTAL_KEY_MATERIAL;
        size_t bytes_processed=0;

        for (uint8_t i = 1; bytes_remaining > 0; i++)
        {
            vector<u8> message;
            if(result_block!=nullptr)
                copy(result_block, result_block + bytes_processed, back_inserter(message));
            copy(const_m_expand, const_m_expand+ sizeof(const_m_expand), back_inserter(message));
            message.push_back(i);
            result = nullptr;
            resultlen = -1;
            result_block = HMAC(EVP_sha256(), pseudo_random_key.data(), static_cast<int>(pseudo_random_key.size()), message.data(), message.size(), result, &resultlen);
            bytes_processed = min(static_cast<size_t>(resultlen), bytes_remaining);
            copy(result_block, result_block + bytes_processed, back_inserter(key_material));
            bytes_remaining -= bytes_processed;
        }
        BOOST_LOG_TRIVIAL(debug) << "-------------key_material-------------";
        Util::HexDump(key_material.data(), key_material.size());
        BOOST_LOG_TRIVIAL(debug) << "--------------------------------------";
    }
    else
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to generate key!";
        return false;
    }
    BOOST_LOG_TRIVIAL(debug) << "Material Key Generated!";

    return true;
}

bool OnionRouter::GeneratKeyMaterial(RelayCell &node)
{
    unc* handshake_data = node.GetPayloadPart(HANDSHAKE_DATA , CELL_SIZE - HANDSHAKE_DATA);
    unc* other_public_key = node.GetPayloadPart(HANDSHAKE_DATA, HANDSHAKE_DATA + KEY_SIZE);
    unc* verify = node.GetPayloadPart(HANDSHAKE_DATA + KEY_SIZE, HANDSHAKE_DATA + (KEY_SIZE * 2));

    BOOST_LOG_TRIVIAL(debug) << "-------------handshake_data-------------";
    Util::HexDump(handshake_data, CELL_SIZE - HANDSHAKE_DATA);
    BOOST_LOG_TRIVIAL(debug) << "----------------------------------------";

    bool ret= GeneratKeyMaterialUnc(handshake_data, other_public_key, verify);
    free(handshake_data);
    free(other_public_key);
    free(verify);
    return ret;
}

bool OnionRouter::GeneratKeyMaterial(shared_ptr<Cell> node)
{
    unc* handshake_data = node->GetPayloadPart(POSITION_PAYLOAD, CELL_SIZE - POSITION_PAYLOAD);
    unc* other_public_key = node->GetPayloadPart(POSITION_PAYLOAD, POSITION_PAYLOAD+KEY_SIZE);
    unc* verify = node->GetPayloadPart(POSITION_PAYLOAD+KEY_SIZE, POSITION_PAYLOAD + (KEY_SIZE * 2));
    bool ret = GeneratKeyMaterialUnc(handshake_data, other_public_key, verify);
    free(handshake_data);
    free(other_public_key);
    free(verify);
    return ret;
}
void OnionRouter::SetKeyMaterial(unc* key_material)
{
    BOOST_LOG_TRIVIAL(debug) << "-------------SetKeyMaterial-------------";
    Util::HexDump(key_material, TOTAL_KEY_MATERIAL);
    BOOST_LOG_TRIVIAL(debug) << "----------------------------------------";

    SHA1_Update(&forward_digest, key_material, DIGEST_LEN);
    SHA1_Update(&backward_digest, key_material + DIGEST_LEN, DIGEST_LEN);

    AES_set_encrypt_key(key_material + (DIGEST_LEN * 2), KEY_LEN * 8, &forward_key);
    AES_set_encrypt_key(key_material + (DIGEST_LEN * 2) + KEY_LEN, KEY_LEN * 8, &backward_key);

    BOOST_LOG_TRIVIAL(debug) << "-------------forward_key-------------";
    Util::HexDump(key_material + (DIGEST_LEN * 2), KEY_LEN);
    BOOST_LOG_TRIVIAL(debug) << "-------------------------------------";
}
void OnionRouter::AesEncrypt(Cell& cell, AES_KEY* key, unc* iv, unc* ec, uni* num)
{
    unsigned char buf[CELL_SIZE];
    unc* cell_payload = cell.GetPayload();
    size_t cell_payload_size = cell.GetPayloadSize();

    BOOST_LOG_TRIVIAL(debug) << "-------------Befor AesEncrypt-------------";
    Util::HexDump(cell_payload, cell_payload_size);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";

    CRYPTO_ctr128_encrypt(cell_payload, buf, cell_payload_size, key, iv, ec, num, reinterpret_cast<block128_f>(AES_encrypt));
    memcpy(cell_payload, buf, cell_payload_size);

    BOOST_LOG_TRIVIAL(debug) << "-------------After AesEncrypt-------------";
    Util::HexDump(cell_payload, cell_payload_size);
    BOOST_LOG_TRIVIAL(debug) << "------------------------------------------";
}

void OnionRouter::CalculateDigest(SHA_CTX* digest, RelayCell& cell, unc* result)
{
    unc* cell_payload = cell.GetPayload();
    size_t cell_payload_size = cell.GetPayloadSize();

    SHA1_Update(digest, cell_payload, cell_payload_size);
    SHA_CTX temp;
    memcpy(&temp, digest, sizeof(SHA_CTX));
    SHA1_Final(result, &temp);
}

void OnionRouter::SetDigest(RelayCell& cell) {
    unc buf[DIGEST_LEN];
    CalculateDigest(&forward_digest, cell, buf);
    cell.SetDigest(buf);
}

void OnionRouter::VerifyDigest(RelayCell& cell) {
    unc zero_digest[4];
    unc received_digest[4];
    unc calculated_digest[DIGEST_LEN];
    memset(zero_digest, 0, sizeof(zero_digest));
    cell.GetDigest(received_digest);
    cell.GetDigest(zero_digest);
    CalculateDigest(&backward_digest, cell, calculated_digest);
    if (memcmp(received_digest, calculated_digest, sizeof(received_digest)))
        BOOST_LOG_TRIVIAL(debug) << "Computed digest does not match server's...";
}

void OnionRouter::Encrypt(RelayCell& cell, bool set_digest) {
    if(set_digest) SetDigest(cell);
    AesEncrypt(cell, &forward_key, forward_iv, forward_ec, &forward_num);
}

void OnionRouter::Decrypt(RelayCell& cell, bool verify_digest) {
    AesEncrypt(cell, &backward_key, back_iv, back_ec, &back_num);
    if(verify_digest) VerifyDigest(cell);
}
