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

#include <lmdb.h>
#include <memory>

#include "lmdb/error.h"

 
#define MONERO_CURSOR(name)                                    \
    struct close_ ## name : ::lmdb::close_cursor {};           \
    using name = std::unique_ptr< MDB_cursor, close_ ## name >;

namespace lmdb
{
    struct abort_txn
    {
        void operator()(MDB_txn* ptr) const noexcept
        {
            if (ptr)
                mdb_txn_abort(ptr);
        }
    };

     
 
 
 
    struct release_read_txn
    { 
        void operator()(MDB_txn* ptr) const noexcept;
         
    };

     
 
 
 
    struct abort_write_txn
    {
        void operator()(MDB_txn* ptr) const noexcept
        {
            release_read_txn{}(ptr);
        }
    };

    struct close_cursor
    {
        void operator()(MDB_cursor* ptr) const noexcept
        {
            if (ptr)
                mdb_cursor_close(ptr);
        }
    };

    template<typename D>
    inline expect<std::unique_ptr<MDB_cursor, D>>
    open_cursor(MDB_txn& txn, MDB_dbi tbl) noexcept
    {
        MDB_cursor* cur = nullptr;
        MONERO_LMDB_CHECK(mdb_cursor_open(&txn, tbl, &cur));
        return std::unique_ptr<MDB_cursor, D>{cur};
    }

     

    using suspended_txn = std::unique_ptr<MDB_txn, abort_txn>;
    using read_txn = std::unique_ptr<MDB_txn, release_read_txn>;
    using write_txn = std::unique_ptr<MDB_txn, abort_write_txn>;
}  
