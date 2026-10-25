// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2021, The Monero Project
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

#include "ringct/rctTypes.h"

#include <vector>


namespace multisig {

namespace signing {

class CLSAG_context_t final {
private:
   
  bool initialized;
   
  rct::keyV c_params;
   
  std::size_t c_params_L_offset;
  std::size_t c_params_R_offset;
   
   
   
   
   
   
   
   
   
   
   
   
   
   
  rct::keyV b_params;
   
  std::size_t b_params_L_offset;
  std::size_t b_params_R_offset;
   
   
   
  rct::key mu_P;
  rct::key mu_C;
   
  std::size_t n;
   
  rct::geDsmp wH_l_precomp;
   
  std::vector<rct::geDsmp> W_precomp;
   
  std::vector<rct::geDsmp> H_precomp;
   
  rct::geDsmp G_precomp;
   
  std::size_t l;
   
  rct::keyV s;
   
  std::size_t num_alpha_components;
public:
  CLSAG_context_t() : initialized{false} {}

   
  bool init(
    const rct::keyV& P,
    const rct::keyV& C_nonzero,
    const rct::key& C_offset,
    const rct::key& message,
    const rct::key& I,
    const rct::key& D,
    const unsigned int l,
    const rct::keyV& s,
    const std::size_t num_alpha_components
  );

   
  bool combine_alpha_and_compute_challenge(
     
    const rct::keyV& total_alpha_G,
     
    const rct::keyV& total_alpha_H,
     
    const rct::keyV& alpha,
     
     
    rct::key& alpha_combined,
     
    rct::key& c_0,
     
    rct::key& c
  );

   
  bool get_mu(
    rct::key& mu_P,
    rct::key& mu_C
  ) const;
};

}  

}  
