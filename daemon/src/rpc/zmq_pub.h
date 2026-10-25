// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2020-2022, The Monero Project

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

#include <array>
#include <boost/thread/mutex.hpp>
#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

#include "cryptonote_basic/fwd.h"
#include "net/zmq.h"
#include "span.h"
#include "cryptonote_basic/difficulty.h"

namespace cryptonote { namespace listener
{
 

 
 
 
 
 
 
class zmq_pub
{
   
 
 

    net::zmq::socket relay_;
    std::deque<std::vector<txpool_event>> txes_;
    std::array<std::size_t, 2> chain_subs_;
    std::array<std::size_t, 1> miner_subs_;
    std::array<std::size_t, 2> txpool_subs_;
    boost::mutex sync_;  

  public:
     
    static constexpr const char* relay_endpoint() noexcept { return "inproc://pub_relay"; }

    explicit zmq_pub(void* context);

    zmq_pub(const zmq_pub&) = delete;
    zmq_pub(zmq_pub&&) = delete;

    ~zmq_pub();

    zmq_pub& operator=(const zmq_pub&) = delete;
    zmq_pub& operator=(zmq_pub&&) = delete;

     
    bool sub_request(const boost::string_ref message);

     
 
    bool relay_to_pub(void* relay, void* pub);

     
 
 
    std::size_t send_chain_main(std::uint64_t height, epee::span<const cryptonote::block> blocks);

     
 
 
    std::size_t send_miner_data(uint8_t major_version, uint64_t height, const crypto::hash& prev_id, const crypto::hash& seed_hash, difficulty_type diff, uint64_t median_weight, uint64_t already_generated_coins, const std::vector<tx_block_template_backlog_entry>& tx_backlog);

     
 
 
    std::size_t send_txpool_add(std::vector<cryptonote::txpool_event> txes);

     
    struct chain_main
    {
      std::weak_ptr<zmq_pub> self_;
      void operator()(std::uint64_t height, epee::span<const cryptonote::block> blocks) const;
    };

     
    struct miner_data
    {
      std::weak_ptr<zmq_pub> self_;
      void operator()(uint8_t major_version, uint64_t height, const crypto::hash& prev_id, const crypto::hash& seed_hash, difficulty_type diff, uint64_t median_weight, uint64_t already_generated_coins, const std::vector<tx_block_template_backlog_entry>& tx_backlog) const;
    };

     
    struct txpool_add
    {
      std::weak_ptr<zmq_pub> self_;
      void operator()(std::vector<cryptonote::txpool_event> txes) const;
    };
  };
}}
