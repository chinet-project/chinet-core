// Copyright (c) 2014-2018 Chinet Project
// Copyright (c) 2014-2018 The Louisdor Project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "genesis.h"

namespace currency
{
#ifndef TESTNET
	const genesis_tx_raw_data ggenesis_tx_raw = { {
	0xbc03000100000101,0xdc5f9ae7884b897c,0xe2f9ba2269b54e52,0x034c9438ca244683,0x00e5681894012ee5,0x42279122a74a1605,0x2d397ab5ee629a22,0x1b44a9cb119c005f,0xf2c98481409942e7,0x364345322813ead0,0x4433434632363946,0x3243373938303634,0x3344313545464543,0x4336364141373930,0x020b001542303030},
	{0x3f,0xb6,0x0e,0x0a,0x00,0x00}};
#else
	const genesis_tx_raw_data ggenesis_tx_raw = {{
	0xbc03000100000101,0xdc5f9ae7884b897c,0xe2f9ba2269b54e52,0x034c9438ca244683,0x00e5681894012ee5,0x42279122a74a1605,0x2d397ab5ee629a22,0x1b44a9cb119c005f,0xf2c98481409942e7,0x364345322813ead0,0x4433434632363946,0x3243373938303634,0x3344313545464543,0x4336364141373930,0x020b001542303030},
	{0x3f,0xb6,0x0e,0x0a,0x00,0x00}};
#endif
}
