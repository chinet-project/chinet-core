// Copyright (c) 2014-2018 Chinet Project
// Copyright (c) 2014-2018 The Louisdor Project
// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "checkpoints.h"
#include "misc_log_ex.h"

#define ADD_CHECKPOINT(h, hash)  CHECK_AND_ASSERT(checkpoints.add_checkpoint(h,  hash), false)

namespace currency
{

  inline bool create_checkpoints(currency::checkpoints& checkpoints)
  {
#ifdef TESTNET

#else
  ADD_CHECKPOINT(1,   "892a75fd73bb770b0be35c29306ca02a66e626e759371a3b79e6b070a467ab1f");
  ADD_CHECKPOINT(2000,   "486d38c239f424eff32e8be1017f4d2eb524a14e94c4e6b6ba2a628af8c67dbc");
  ADD_CHECKPOINT(7000,   "b35cf0a446cf112d8abf8247c99819183a22ad446a5dd6abcf0a745fce641c8a");
#endif

    return true;
  }

} // namespace currency 
