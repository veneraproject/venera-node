// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2014-2022, The Monero Project
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

 
 
 
 
 
 
 
 
 

#ifndef ELECTRUM_WORDS_H
#define ELECTRUM_WORDS_H

#include <string>
#include <cstdint>
#include "crypto/crypto.h"   

namespace epee {  class wipeable_string; }

 
 
 
 
 
namespace crypto
{
   
 
 
 
 
  namespace ElectrumWords
  {

    const int seed_length = 24;
    const std::string old_language_name = "EnglishOld";
     
 
 
 
 
 
 
 
 
    bool words_to_bytes(const epee::wipeable_string &words, epee::wipeable_string& dst, size_t len, bool duplicate,
      std::string &language_name);
     
 
 
 
 
 
 
    bool words_to_bytes(const epee::wipeable_string &words, crypto::secret_key& dst,
      std::string &language_name);

     
 
 
 
 
 
 
 
    bool bytes_to_words(const char *src, size_t len, epee::wipeable_string& words,
      const std::string &language_name);

     
 
 
 
 
 
 
    bool bytes_to_words(const crypto::secret_key& src, epee::wipeable_string& words,
      const std::string &language_name);

     
 
 
 
 
    void get_language_list(std::vector<std::string> &languages, bool english = false);

     
 
 
 
 
    bool get_is_old_style_seed(const epee::wipeable_string &seed);

     
 
 
 
 
    std::string get_english_name_for(const std::string &name);

    bool is_valid_language(const std::string &language);
  }
}

#endif
