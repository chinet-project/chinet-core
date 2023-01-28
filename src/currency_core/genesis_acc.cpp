// Copyright (c) 2014-2018 Chinet Project
// Copyright (c) 2014-2018 Chinet Project
// Copyright (c) 2014-2018 The Louisdor Project
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "genesis_acc.h"


namespace currency
{
#ifndef TESTNET
  const std::string ggenesis_tx_pub_key_str = "4aa722912742229a62eeb57a392d5f009c11cba9441be74299408184c9f2d0ea";
  const crypto::public_key ggenesis_tx_pub_key = epee::string_tools::parse_tpod_from_hex_string<crypto::public_key>(ggenesis_tx_pub_key_str);
  const genesis_tx_dictionary_entry ggenesis_dict[1] = {
	{7813170670029020568ULL,0}
  };
#else 
  const std::string ggenesis_tx_pub_key_str    = "4aa722912742229a62eeb57a392d5f009c11cba9441be74299408184c9f2d0ea";
  const crypto::public_key ggenesis_tx_pub_key = epee::string_tools::parse_tpod_from_hex_string<crypto::public_key>(ggenesis_tx_pub_key_str);
  const genesis_tx_dictionary_entry ggenesis_dict[1] = {
	{7813170670029020568ULL,0}
  };
#endif 
  


}