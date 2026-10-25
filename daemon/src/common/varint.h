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
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include <limits>
#include <type_traits>
#include <utility>
#include <sstream>
#include <string>
 
 
 
 
 
 
 
 
 
 
 
 
 
 

namespace tools {

   
 
  enum {
     
    EVARINT_OVERFLOW = -1,
     
    EVARINT_REPRESENT = -2,
  };

   
 
  template<typename OutputIt, typename T>
   
  typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, void>::type 
  write_varint(OutputIt &&dest, T i) {
     
    while (i >= 0x80) {
      *dest = (static_cast<char>(i) & 0x7f) | 0x80; 
      ++dest;
      i >>= 7;			 
    }
     
    *dest = static_cast<char>(i);
    dest++;			 
  }

   
 
  template<typename T>
    std::string get_varint_data(const T& v)
    {
      std::stringstream ss;
      write_varint(std::ostreambuf_iterator<char>(ss), v);
      return ss.str();
    }
   
  
  template<int bits, typename InputIt, typename T>
    typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value && 0 <= bits && bits <= std::numeric_limits<T>::digits, int>::type
    read_varint(InputIt &&first, InputIt &&last, T &write) {
    int read = 0;
    write = 0;
    for (int shift = 0;; shift += 7) {
      if (first == last) {
	return read; 
      }
      unsigned char byte = *first;
      ++first;
      ++read;
      if (shift + 7 >= bits && byte >= 1 << (bits - shift)) {
	return EVARINT_OVERFLOW;
      }
      if (byte == 0 && shift != 0) {
	return EVARINT_REPRESENT;
      }

      write |= static_cast<T>(byte & 0x7f) << shift;  

       
      if ((byte & 0x80) == 0) {
	break;
      }
    }
    return read;
  }

   
 
 
  template<typename InputIt, typename T>
    int read_varint(InputIt &&first, InputIt &&last, T &i) {
    return read_varint<std::numeric_limits<T>::digits>(std::forward<InputIt>(first), std::forward<InputIt>(last), i);
  }
}
