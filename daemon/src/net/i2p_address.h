// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2019-2022, The Monero Project
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

#include <boost/utility/string_ref.hpp>
#include <cstdint>
#include <string>

#include "common/expect.h"
#include "net/enums.h"
#include "net/error.h"

namespace epee
{
namespace serialization
{
    class portable_storage;
    struct section;
}
}

namespace net
{
     
    class i2p_address
    {
        char host_[61];  

         
        i2p_address(boost::string_ref host) noexcept;

    public:
         
        static constexpr std::size_t buffer_size() noexcept { return sizeof(host_); }

         
        static const char* unknown_str() noexcept;

         
        i2p_address() noexcept;

         
        static i2p_address unknown() noexcept { return i2p_address{}; }

         
 
 
 
 
        static expect<i2p_address> make(boost::string_ref address);

         
        bool _load(epee::serialization::portable_storage& src, epee::serialization::section* hparent);

         
        bool store(epee::serialization::portable_storage& dest, epee::serialization::section* hparent) const;

         

        i2p_address(const i2p_address& rhs) noexcept;
        ~i2p_address() = default;
        i2p_address& operator=(const i2p_address& rhs) noexcept;

         
        bool is_unknown() const noexcept;

        bool equal(const i2p_address& rhs) const noexcept;
        bool less(const i2p_address& rhs) const noexcept;

         
        bool is_same_host(const i2p_address& rhs) const noexcept;

         
        std::string str() const;

         
        const char* host_str() const noexcept { return host_; }

         
        std::uint16_t port() const noexcept { return 1; }

        static constexpr bool is_loopback() noexcept { return false; }
        static constexpr bool is_local() noexcept { return false; }

        static constexpr epee::net_utils::address_type get_type_id() noexcept
        {
            return epee::net_utils::address_type::i2p;
        }

        static constexpr epee::net_utils::zone get_zone() noexcept
        {
            return epee::net_utils::zone::i2p;
        }

         
        bool is_blockable() const noexcept { return !is_unknown(); }
    };

    inline bool operator==(const i2p_address& lhs, const i2p_address& rhs) noexcept
    {
        return lhs.equal(rhs);
    }

    inline bool operator!=(const i2p_address& lhs, const i2p_address& rhs) noexcept
    {
        return !lhs.equal(rhs);
    }

    inline bool operator<(const i2p_address& lhs, const i2p_address& rhs) noexcept
    {
        return lhs.less(rhs);
    }
}  
