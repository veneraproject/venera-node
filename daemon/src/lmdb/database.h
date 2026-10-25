// Copyright (c) 2025-2026 Venera Labs.
// Copyright (c) 2018-2022, The Monero Project

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

#include <atomic>
#include <cstddef>
#include <lmdb.h>
#include <memory>
#include <type_traits>

#include "common/expect.h"
#include "lmdb/error.h"
#include "lmdb/transaction.h"

namespace lmdb
{
     
    struct close_env
    {
        void operator()(MDB_env* ptr) const noexcept
        {
            if (ptr)
                mdb_env_close(ptr);
        }
    };

    using environment = std::unique_ptr<MDB_env, close_env>;

     
    expect<environment> open_environment(const char* path, MDB_dbi max_dbs) noexcept;

     
    struct context
    {
        std::atomic<std::size_t> active;
        std::atomic_flag lock;
    };

     
    class database
    {
        environment env;
        context ctx;

         
        MDB_env* handle() const noexcept { return env.get(); }

        expect<write_txn> do_create_txn(unsigned int flags) noexcept;

    public: 
        database(environment env);

        database(database&&) = delete;
        database(database const&) = delete;

        virtual ~database() noexcept;

        database& operator=(database&&) = delete;
        database& operator=(database const&) = delete;

         
 
 
 
        expect<void> resize() noexcept;

         
        expect<read_txn> create_read_txn(suspended_txn txn = nullptr) noexcept;

         
        expect<suspended_txn> reset_txn(read_txn txn) noexcept;

         
        expect<write_txn> create_write_txn() noexcept;

         
        expect<void> commit(write_txn txn) noexcept;

         
 
 

 
 

 
 
        template<typename F>
        typename std::result_of<F(MDB_txn&)>::type try_write(F f, unsigned attempts = 3)
        {
            for (unsigned i = 0; i < attempts; ++i)
            {
                expect<write_txn> txn = create_write_txn();
                if (!txn)
                    return txn.error();

                MONERO_PRECOND(*txn != nullptr);
                const auto wrote = f(*(*txn));
                if (wrote)
                {
                    MONERO_CHECK(commit(std::move(*txn)));
                    return wrote;
                }
                if (wrote != lmdb::error(MDB_MAP_FULL))
                    return wrote;

                txn->reset();
                MONERO_CHECK(this->resize());
            }
            return {lmdb::error(MDB_MAP_FULL)};
        }
    };
}  

