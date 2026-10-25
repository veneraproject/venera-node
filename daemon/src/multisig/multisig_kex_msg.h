// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2021-2022, The Monero Project
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

#include <cstdint>
#include <vector>


namespace multisig
{
   
   
   
   
   
   
   
   
   
   
   
  class multisig_kex_msg final
  {
   

   
  public:
     
    multisig_kex_msg() = default;

     
    multisig_kex_msg(const std::uint32_t round,
      const crypto::secret_key &signing_privkey,
      std::vector<crypto::public_key> msg_pubkeys,
      const crypto::secret_key &msg_privkey = crypto::null_skey);

     
    multisig_kex_msg(std::string msg);

     

   
    ~multisig_kex_msg() = default;

   

   
     
    const std::string& get_msg() const { return m_msg; }
     
    std::uint32_t get_round() const { return m_kex_round; }
     
    const std::vector<crypto::public_key>& get_msg_pubkeys() const { return m_msg_pubkeys; }
     
    const crypto::secret_key& get_msg_privkey() const { return m_msg_privkey; }
     
    const crypto::public_key& get_signing_pubkey() const { return m_signing_pubkey; }

  private:
     
    crypto::hash get_msg_to_sign() const;
     
    void construct_msg(const crypto::secret_key &signing_privkey);
     
    void parse_and_validate_msg();

   
  private:
     
    std::string m_msg;

     
    std::uint32_t m_kex_round;
     
    std::vector<crypto::public_key> m_msg_pubkeys;
     
    crypto::secret_key m_msg_privkey;
     
    crypto::public_key m_signing_pubkey;
  };
}  
