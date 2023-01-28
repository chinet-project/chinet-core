/*
 * Copyright (c) 2021, Chinet project, https://chinet.io
 * Copyright (c) 2021, Mikhail Butolin, bml505@hotmail.com
 * Copyright 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * curve25519-donna: Curve25519 elliptic curve, public key function
 *
 * http://code.google.com/p/curve25519-donna/
 *
 * Adam Langley <agl@imperialviolet.org>
 *
 * Derived from public domain C code by Daniel J. Bernstein <djb@cr.yp.to>
 *
 * More information about curve25519 can be found here
 *   http://cr.yp.to/ecdh.html
 *
 * djb's sample implementation of curve25519 is written in a special assembly
 * language called qhasm and uses the floating point registers.
 *
 * This is, almost, a clean room reimplementation from the curve25519 paper. It
 * uses many of the tricks described therein. Only the crecip function is taken
 * from the sample implementation.
 *
 * Copyright(c) 2016 Petr Benes https://github.com/wbenny/mini-tor
 * 
 */




#pragma once
#include <cstdint>

typedef uint8_t u8;
typedef int32_t s32;
typedef int64_t limb;

class Curve25519
{
private:
	static void fsum(limb* output, const limb* in);
	static void fdifference(limb* output, const limb* in);
	static void fscalar_product(limb* output, const limb* in, const limb scalar);
	static void fproduct(limb* output, const limb* in2, const limb* in);
	static void freduce_degree(limb* output);
	static inline limb div_by_2_26(const limb v);
	static inline limb div_by_2_25(const limb v);
	static void freduce_coefficients(limb* output);
	static void fmul(limb* output, const limb* in, const limb* in2);
	static void fsquare_inner(limb* output, const limb* in);
	static void fsquare(limb* output, const limb* in);
	static void fexpand(limb* output, const u8* input);
	static s32 s32_eq(s32 a, s32 b);
	static s32 s32_gte(s32 a, s32 b);
	static void fcontract(u8* output, limb* input_limbs);
	static void fmonty(limb* x2, limb* z2,  /* output 2Q */
		limb* x3, limb* z3,  /* output Q + Q' */
		limb* x, limb* z,    /* input Q */
		limb* xprime, limb* zprime,  /* input Q' */
		const limb* qmqp /* input Q - Q' */);
	static void swap_conditional(limb a[19], limb b[19], limb iswap);
	static void cmult(limb* resultx, limb* resultz, const u8* n, const limb* q);
	static void crecip(limb* out, const limb* z);
public:
	static int curve25519_donna(uint8_t* mypublic, const uint8_t* secret, const uint8_t* basepoint);

};

