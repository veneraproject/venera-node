// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2025-2026, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

 
#include "crypto/crypto.h"
#include "cryptonote_basic/account.h"
#include "cryptonote_basic/subaddress_index.h"
#include "cryptonote_config.h"
#include "wallet2.h"

 

 
#include <functional>
#include <boost/optional.hpp>
#include <unordered_map>
#include <vector>

namespace tools
{
namespace wallet
{
 
 
 
 
 
 
 
void check_consistent_ins_outs(const std::vector<wallet2::tx_construction_data> &txes);
void check_consistent_ins_outs(const std::vector<wallet2::pending_tx> &txes);
void sanity_check_tx_construction_data(const wallet2::tx_construction_data &tx);
 
void sanity_check_unsigned_tx_set(const std::vector<wallet2::tx_construction_data> &txes,
    const cryptonote::account_keys &account_keys,
    const std::unordered_map<crypto::public_key, cryptonote::subaddress_index> &subaddresses);
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
void sanity_check_pending_tx(const wallet2::pending_tx &ptx,
    const cryptonote::network_type nettype,
    const cryptonote::account_keys &account_keys,
    const std::unordered_map<crypto::public_key, cryptonote::subaddress_index> &subaddresses,
    const std::vector<wallet2::transfer_details> &transfers,
    const bool redacted,
    const boost::optional<std::function<const crypto::key_image(const size_t)>> &transfer_ki_resolver,
    const bool allow_read_only);
 
void sanity_check_pending_tx_set(const std::vector<wallet2::pending_tx> &ptxs,
    const cryptonote::network_type nettype,
    const cryptonote::account_keys &account_keys,
    const std::unordered_map<crypto::public_key, cryptonote::subaddress_index> &subaddresses,
    const std::vector<wallet2::transfer_details> &transfers,
    const bool redacted,
    const boost::optional<std::function<const crypto::key_image(const size_t)>> &transfer_ki_resolver,
    const bool allow_read_only);
}  
}  
