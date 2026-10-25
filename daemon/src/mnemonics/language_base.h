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

 
 
 
 
 

#ifndef LANGUAGE_BASE_H
#define LANGUAGE_BASE_H

#include <vector>
#include <unordered_map>
#include <string>
#include <boost/algorithm/string.hpp>
#include "misc_log_ex.h"
#include "fnv1.h"
#include "common/utf8.h"

 
 
 
 
namespace Language
{
   
 
 
 
 
 
 
  template<typename T>
  inline T utf8prefix(const T &s, size_t count)
  {
    T prefix = "";
    size_t avail = s.size();
    const char *ptr = s.data();
    while (count-- && avail--)
    {
      prefix += *ptr++;
      while (avail && ((*ptr) & 0xc0) == 0x80)
      {
        prefix += *ptr++;
        --avail;
      }
    }
    return prefix;
  }

  struct WordHash
  {
    std::size_t operator()(const epee::wipeable_string &s) const
    {
      const epee::wipeable_string sc = tools::utf8canonical(s, [](wint_t c) -> wint_t { return std::towlower(c); });
      return epee::fnv::FNV1a(sc.data(), sc.size());
    }
  };

  struct WordEqual
  {
    bool operator()(const epee::wipeable_string &s0, const epee::wipeable_string &s1) const
    {
      const epee::wipeable_string s0c = tools::utf8canonical(s0, [](wint_t c) -> wint_t { return std::towlower(c); });
      const epee::wipeable_string s1c = tools::utf8canonical(s1, [](wint_t c) -> wint_t { return std::towlower(c); });
      return s0c == s1c;
    }
  };

   
 
 
 
 
  class Base
  {
  protected:
    enum {
      ALLOW_SHORT_WORDS = 1<<0,
      ALLOW_DUPLICATE_PREFIXES = 1<<1,
    };
    enum {
      NWORDS = 1626
    };
    std::vector<std::string> word_list;  
    std::unordered_map<epee::wipeable_string, uint32_t, WordHash, WordEqual> word_map;  
    std::unordered_map<epee::wipeable_string, uint32_t, WordHash, WordEqual> trimmed_word_map;  
    std::string language_name;  
    std::string english_language_name;  
    uint32_t unique_prefix_length;  
     
 
 
    void populate_maps(uint32_t flags = 0)
    {
      int ii;
      std::vector<std::string>::const_iterator it;
      if (word_list.size () != NWORDS)
        throw std::runtime_error("Wrong word list length for " + language_name);
      for (it = word_list.begin(), ii = 0; it != word_list.end(); it++, ii++)
      {
        word_map[*it] = ii;
        if ((*it).size() < unique_prefix_length)
        {
          if (flags & ALLOW_SHORT_WORDS)
            MINFO(language_name << " word '" << *it << "' is shorter than its prefix length, " << unique_prefix_length);
          else
            throw std::runtime_error("Too short word in " + language_name + " word list: " + *it);
        }
        epee::wipeable_string trimmed;
        if (it->length() > unique_prefix_length)
        {
          trimmed = utf8prefix(*it, unique_prefix_length);
        }
        else
        {
          trimmed = *it;
        }
        if (trimmed_word_map.find(trimmed) != trimmed_word_map.end())
        {
          if (flags & ALLOW_DUPLICATE_PREFIXES)
            MWARNING("Duplicate prefix in " << language_name << " word list: " << std::string(trimmed.data(), trimmed.size()));
          else
            throw std::runtime_error("Duplicate prefix in " + language_name + " word list: " + std::string(trimmed.data(), trimmed.size()));
        }
        trimmed_word_map[trimmed] = ii;
      }
    }
  public:
    Base(const char *language_name, const char *english_language_name, const std::vector<std::string> &words, uint32_t prefix_length):
      word_list(words),
      unique_prefix_length(prefix_length),
      language_name(language_name),
      english_language_name(english_language_name)
    {
    }
    virtual ~Base()
    {
    }
    void set_words(const char * const words[])
    {
      word_list.resize(NWORDS);
      for (size_t i = 0; i < NWORDS; ++i)
        word_list[i] = words[i];
    }
     
 
 
 
    const std::vector<std::string>& get_word_list() const
    {
      return word_list;
    }
     
 
 
 
    const std::unordered_map<epee::wipeable_string, uint32_t, WordHash, WordEqual>& get_word_map() const
    {
      return word_map;
    }
     
 
 
 
    const std::unordered_map<epee::wipeable_string, uint32_t, WordHash, WordEqual>& get_trimmed_word_map() const
    {
      return trimmed_word_map;
    }
     
 
 
 
    const std::string &get_language_name() const
    {
      return language_name;
    }
     
 
 
 
    const std::string &get_english_language_name() const
    {
      return english_language_name;
    }
     
 
 
 
    uint32_t get_unique_prefix_length() const
    {
      return unique_prefix_length;
    }
  };
}

#endif
