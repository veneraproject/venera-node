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

#include <boost/uuid/uuid.hpp>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "span.h"

namespace net
{
namespace dandelionpp
{
     
    class connection_map
    {
         
        std::vector<boost::uuids::uuid> out_mapping_;  
        std::vector<std::pair<boost::uuids::uuid, std::size_t>> in_mapping_;  
        std::vector<std::size_t> usage_count_;

         
        connection_map(const connection_map&) = default;

    public:
        using value_type = boost::uuids::uuid;
        using size_type = std::vector<boost::uuids::uuid>::size_type;
        using difference_type = std::vector<boost::uuids::uuid>::difference_type;
        using reference = const boost::uuids::uuid&;
        using const_reference = reference;
        using iterator = std::vector<boost::uuids::uuid>::const_iterator;
        using const_iterator = iterator;

         
        explicit connection_map()
          : connection_map(std::vector<boost::uuids::uuid>{}, 0)
        {}

         
        explicit connection_map(std::vector<boost::uuids::uuid> out_connections, std::size_t stems);

        connection_map(connection_map&&) = default;
        ~connection_map() noexcept;
        connection_map& operator=(connection_map&&) = default;
        connection_map& operator=(const connection_map&) = delete;

         
        connection_map clone() const;

         
        const_iterator begin() const noexcept
        {
            return out_mapping_.begin();
        }

         
        const_iterator end() const noexcept
        {
            return out_mapping_.end();
        }

         
 
 

 
 
        bool update(std::vector<boost::uuids::uuid> current);

         
        std::size_t size() const noexcept;

         
        boost::uuids::uuid get_stem(const boost::uuids::uuid& source);
    };
}  
}  
