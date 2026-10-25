/** @file lmdb.h
 *	@brief Lightning memory-mapped database library
 *
 *	@mainpage	Lightning Memory-Mapped Database Manager (LMDB)
 *
 *	@section intro_sec Introduction
 *	LMDB is a Btree-based database management library modeled loosely on the
 *	BerkeleyDB API, but much simplified. The entire database is exposed
 *	in a memory map, and all data fetches return data directly
 *	from the mapped memory, so no malloc's or memcpy's occur during
 *	data fetches. As such, the library is extremely simple because it
 *	requires no page caching layer of its own, and it is extremely high
 *	performance and memory-efficient. It is also fully transactional with
 *	full ACID semantics, and when the memory map is read-only, the
 *	database integrity cannot be corrupted by stray pointer writes from
 *	application code.
 *
 *	The library is fully thread-aware and supports concurrent read/write
 *	access from multiple processes and threads. Data pages use a copy-on-
 *	write strategy so no active data pages are ever overwritten, which
 *	also provides resistance to corruption and eliminates the need of any
 *	special recovery procedures after a system crash. Writes are fully
 *	serialized; only one write transaction may be active at a time, which
 *	guarantees that writers can never deadlock. The database structure is
 *	multi-versioned so readers run with no locks; writers cannot block
 *	readers, and readers don't block writers.
 *
 *	Unlike other well-known database mechanisms which use either write-ahead
 *	transaction logs or append-only data writes, LMDB requires no maintenance
 *	during operation. Both write-ahead loggers and append-only databases
 *	require periodic checkpointing and/or compaction of their log or database
 *	files otherwise they grow without bound. LMDB tracks free pages within
 *	the database and re-uses them for new write operations, so the database
 *	size does not grow without bound in normal use.
 *
 *	The memory map can be used as a read-only or read-write map. It is
 *	read-only by default as this provides total immunity to corruption.
 *	Using read-write mode offers much higher write performance, but adds
 *	the possibility for stray application writes thru pointers to silently
 *	corrupt the database. Of course if your application code is known to
 *	be bug-free (...) then this is not an issue.
 *
 *	If this is your first time using a transactional embedded key/value
 *	store, you may find the \ref starting page to be helpful.
 *
 *	@section caveats_sec Caveats
 *	Troubleshooting the lock file, plus semaphores on BSD systems:
 *
 *	- A broken lockfile can cause sync issues.
 *	  Stale reader transactions left behind by an aborted program
 *	  cause further writes to grow the database quickly, and
 *	  stale locks can block further operation.
 *
 *	  Fix: Check for stale readers periodically, using the
 *	  #mdb_reader_check function or the \ref mdb_stat_1 "mdb_stat" tool.
 *	  Stale writers will be cleared automatically on most systems:
 *	  - Windows - automatic
 *	  - BSD, systems using SysV semaphores - automatic
 *	  - Linux, systems using POSIX mutexes with Robust option - automatic
 *	  Otherwise just make all programs using the database close it;
 *	  the lockfile is always reset on first open of the environment.
 *
 *	- On BSD systems or others configured with MDB_USE_SYSV_SEM or
 *	  MDB_USE_POSIX_SEM,
 *	  startup can fail due to semaphores owned by another userid.
 *
 *	  Fix: Open and close the database as the user which owns the
 *	  semaphores (likely last user) or as root, while no other
 *	  process is using the database.
 *
 *	Restrictions/caveats (in addition to those listed for some functions):
 *
 *	- Only the database owner should normally use the database on
 *	  BSD systems or when otherwise configured with MDB_USE_POSIX_SEM.
 *	  Multiple users can cause startup to fail later, as noted above.
 *
 *	- There is normally no pure read-only mode, since readers need write
 *	  access to locks and lock file. Exceptions: On read-only filesystems
 *	  or with the #MDB_NOLOCK flag described under #mdb_env_open().
 *
 *	- An LMDB configuration will often reserve considerable \b unused
 *	  memory address space and maybe file size for future growth.
 *	  This does not use actual memory or disk space, but users may need
 *	  to understand the difference so they won't be scared off.
 *
 *	- By default, in versions before 0.9.10, unused portions of the data
 *	  file might receive garbage data from memory freed by other code.
 *	  (This does not happen when using the #MDB_WRITEMAP flag.) As of
 *	  0.9.10 the default behavior is to initialize such memory before
 *	  writing to the data file. Since there may be a slight performance
 *	  cost due to this initialization, applications may disable it using
 *	  the #MDB_NOMEMINIT flag. Applications handling sensitive data
 *	  which must not be written should not use this flag. This flag is
 *	  irrelevant when using #MDB_WRITEMAP.
 *
 *	- A thread can only use one transaction at a time, plus any child
 *	  transactions.  Each transaction belongs to one thread.  See below.
 *	  The #MDB_NOTLS flag changes this for read-only transactions.
 *
 *	- Use an MDB_env* in the process which opened it, not after fork().
 *
 *	- Do not have open an LMDB database twice in the same process at
 *	  the same time.  Not even from a plain open() call - close()ing it
 *	  breaks fcntl() advisory locking.  (It is OK to reopen it after
 *	  fork() - exec*(), since the lockfile has FD_CLOEXEC set.)
 *
 *	- Avoid long-lived transactions.  Read transactions prevent
 *	  reuse of pages freed by newer write transactions, thus the
 *	  database can grow quickly.  Write transactions prevent
 *	  other write transactions, since writes are serialized.
 *
 *	- Avoid suspending a process with active transactions.  These
 *	  would then be "long-lived" as above.  Also read transactions
 *	  suspended when writers commit could sometimes see wrong data.
 *
 *	...when several processes can use a database concurrently:
 *
 *	- Avoid aborting a process with an active transaction.
 *	  The transaction becomes "long-lived" as above until a check
 *	  for stale readers is performed or the lockfile is reset,
 *	  since the process may not remove it from the lockfile.
 *
 *	  This does not apply to write transactions if the system clears
 *	  stale writers, see above.
 *
 *	- If you do that anyway, do a periodic check for stale readers. Or
 *	  close the environment once in a while, so the lockfile can get reset.
 *
 *	- Do not use LMDB databases on remote filesystems, even between
 *	  processes on the same host.  This breaks flock() on some OSes,
 *	  possibly memory map sync, and certainly sync between programs
 *	  on different hosts.
 *
 *	- Opening a database can fail if another process is opening or
 *	  closing it at exactly the same time.
 *
 *	@author	Howard Chu, Symas Corporation.
 *
 *	@copyright Copyright 2011-2019 Howard Chu, Symas Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 *
 *	@par Derived From:
 * This code is derived from btree.c written by Martin Hedenfalk.
 *
 * Copyright (c) 2009, 2010 Martin Hedenfalk <martin@bzero.se>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef _LMDB_H_
#define _LMDB_H_

#include <sys/types.h>
#include <inttypes.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

 
#ifdef _MSC_VER
typedef	int	mdb_mode_t;
#else
typedef	mode_t	mdb_mode_t;
#endif

#ifdef _WIN32
# define MDB_FMT_Z	"I"
#else
# define MDB_FMT_Z	"z"			 
#endif

#ifndef MDB_VL32
 
 
 
 
 
typedef size_t	mdb_size_t;
# define MDB_SIZE_MAX	SIZE_MAX	 
 
# define MDB_PRIy(t)	MDB_FMT_Z #t
 
# define MDB_SCNy(t)	MDB_FMT_Z #t
#else
typedef uint64_t	mdb_size_t;
# define MDB_SIZE_MAX	UINT64_MAX
# define MDB_PRIy(t)	PRI##t##64
# define MDB_SCNy(t)	SCN##t##64
# define mdb_env_create	mdb_env_create_vl32	 
#endif

 
 
 
 
#ifdef _WIN32
typedef	void *mdb_filehandle_t;
#else
typedef int mdb_filehandle_t;
#endif

 
 
 
 
 
 
 
 
#define MDB_VERSION_MAJOR	0
 
#define MDB_VERSION_MINOR	9
 
#define MDB_VERSION_PATCH	70

 
#define MDB_VERINT(a,b,c)	(((a) << 24) | ((b) << 16) | (c))

 
#define MDB_VERSION_FULL	\
	MDB_VERINT(MDB_VERSION_MAJOR,MDB_VERSION_MINOR,MDB_VERSION_PATCH)

 
#define MDB_VERSION_DATE	"December 19, 2015"

 
#define MDB_VERSTR(a,b,c,d)	"LMDB " #a "." #b "." #c ": (" d ")"

 
#define MDB_VERFOO(a,b,c,d)	MDB_VERSTR(a,b,c,d)

 
#define	MDB_VERSION_STRING	\
	MDB_VERFOO(MDB_VERSION_MAJOR,MDB_VERSION_MINOR,MDB_VERSION_PATCH,MDB_VERSION_DATE)
 

 
 
 
 
 
typedef struct MDB_env MDB_env;

 
 
 
 
 
typedef struct MDB_txn MDB_txn;

 
typedef unsigned int	MDB_dbi;

 
typedef struct MDB_cursor MDB_cursor;

 
 
 
 
 
 
 
 
 
 
 
typedef struct MDB_val {
	size_t		 mv_size;	 
	void		*mv_data;	 
} MDB_val;

 
typedef int  (MDB_cmp_func)(const MDB_val *a, const MDB_val *b);

 
 
 
 
 
 
 
 
 
 
 
 
 
 
typedef void (MDB_rel_func)(MDB_val *item, void *oldptr, void *newptr, void *relctx);

 
 
 
	 
#define MDB_FIXEDMAP	0x01
	 
#define MDB_NOSUBDIR	0x4000
	 
#define MDB_NOSYNC		0x10000
	 
#define MDB_RDONLY		0x20000
	 
#define MDB_NOMETASYNC		0x40000
	 
#define MDB_WRITEMAP		0x80000
	 
#define MDB_MAPASYNC		0x100000
	 
#define MDB_NOTLS		0x200000
	 
#define MDB_NOLOCK		0x400000
	 
#define MDB_NORDAHEAD	0x800000
	 
#define MDB_NOMEMINIT	0x1000000
	 
#define MDB_PREVSNAPSHOT	0x2000000
 

 
 
 
	 
#define MDB_REVERSEKEY	0x02
	 
#define MDB_DUPSORT		0x04
	 
 
 
#define MDB_INTEGERKEY	0x08
	 
#define MDB_DUPFIXED	0x10
	 
#define MDB_INTEGERDUP	0x20
	 
#define MDB_REVERSEDUP	0x40
	 
#define MDB_CREATE		0x40000
 

 
 
 
 
#define MDB_NOOVERWRITE	0x10
 
 
 
 
#define MDB_NODUPDATA	0x20
 
#define MDB_CURRENT	0x40
 
 
 
#define MDB_RESERVE	0x10000
 
#define MDB_APPEND	0x20000
 
#define MDB_APPENDDUP	0x40000
 
#define MDB_MULTIPLE	0x80000
 

 
 
 
 
 
 
#define MDB_CP_COMPACT	0x01
 

 
 
 
 
 
typedef enum MDB_cursor_op {
	MDB_FIRST,				 
	MDB_FIRST_DUP,			 
 
	MDB_GET_BOTH,			 
	MDB_GET_BOTH_RANGE,		 
	MDB_GET_CURRENT,		 
	MDB_GET_MULTIPLE,		 
 
 
	MDB_LAST,				 
	MDB_LAST_DUP,			 
 
	MDB_NEXT,				 
	MDB_NEXT_DUP,			 
 
	MDB_NEXT_MULTIPLE,		 
 
 
	MDB_NEXT_NODUP,			 
	MDB_PREV,				 
	MDB_PREV_DUP,			 
 
	MDB_PREV_NODUP,			 
	MDB_SET,				 
	MDB_SET_KEY,			 
	MDB_SET_RANGE,			 
	MDB_PREV_MULTIPLE		 
 
} MDB_cursor_op;

 
 
 
 
 
	 
#define MDB_SUCCESS	 0
	 
#define MDB_KEYEXIST	(-30799)
	 
#define MDB_NOTFOUND	(-30798)
	 
#define MDB_PAGE_NOTFOUND	(-30797)
	 
#define MDB_CORRUPTED	(-30796)
	 
#define MDB_PANIC		(-30795)
	 
#define MDB_VERSION_MISMATCH	(-30794)
	 
#define MDB_INVALID	(-30793)
	 
#define MDB_MAP_FULL	(-30792)
	 
#define MDB_DBS_FULL	(-30791)
	 
#define MDB_READERS_FULL	(-30790)
	 
#define MDB_TLS_FULL	(-30789)
	 
#define MDB_TXN_FULL	(-30788)
	 
#define MDB_CURSOR_FULL	(-30787)
	 
#define MDB_PAGE_FULL	(-30786)
	 
#define MDB_MAP_RESIZED	(-30785)
	 
 
 
 
 
 
 
 
#define MDB_INCOMPATIBLE	(-30784)
	 
#define MDB_BAD_RSLOT		(-30783)
	 
#define MDB_BAD_TXN			(-30782)
	 
#define MDB_BAD_VALSIZE		(-30781)
	 
#define MDB_BAD_DBI		(-30780)
	 
#define MDB_PROBLEM		(-30779)
	 
#define MDB_LAST_ERRCODE	MDB_PROBLEM
 

 
typedef struct MDB_stat {
	unsigned int	ms_psize;			 
 
	unsigned int	ms_depth;			 
	mdb_size_t		ms_branch_pages;	 
	mdb_size_t		ms_leaf_pages;		 
	mdb_size_t		ms_overflow_pages;	 
	mdb_size_t		ms_entries;			 
} MDB_stat;

 
typedef struct MDB_envinfo {
	void	*me_mapaddr;			 
	mdb_size_t	me_mapsize;				 
	mdb_size_t	me_last_pgno;			 
	mdb_size_t	me_last_txnid;			 
	unsigned int me_maxreaders;		 
	unsigned int me_numreaders;		 
} MDB_envinfo;

	 
 
 
 
 
 
 
char *mdb_version(int *major, int *minor, int *patch);

	 
 
 
 
 
 
 
 
 
 
char *mdb_strerror(int err);

	 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_create(MDB_env **env);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_open(MDB_env *env, const char *path, unsigned int flags, mdb_mode_t mode);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_copy(MDB_env *env, const char *path);

	 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_copyfd(MDB_env *env, mdb_filehandle_t fd);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_copy2(MDB_env *env, const char *path, unsigned int flags);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_copyfd2(MDB_env *env, mdb_filehandle_t fd, unsigned int flags);

	 
 
 
 
 
 
int  mdb_env_stat(MDB_env *env, MDB_stat *stat);

	 
 
 
 
 
 
int  mdb_env_info(MDB_env *env, MDB_envinfo *stat);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_sync(MDB_env *env, int force);

	 
 
 
 
 
 
 
 
void mdb_env_close(MDB_env *env);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_set_flags(MDB_env *env, unsigned int flags, int onoff);

	 
 
 
 
 
 
 
 
 
 
int  mdb_env_get_flags(MDB_env *env, unsigned int *flags);

	 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_get_path(MDB_env *env, const char **path);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_get_fd(MDB_env *env, mdb_filehandle_t *fd);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_set_mapsize(MDB_env *env, mdb_size_t size);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_set_maxreaders(MDB_env *env, unsigned int readers);

	 
 
 
 
 
 
 
 
 
 
int  mdb_env_get_maxreaders(MDB_env *env, unsigned int *readers);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_env_set_maxdbs(MDB_env *env, MDB_dbi dbs);

	 
 
 
 
 
 
 
int  mdb_env_get_maxkeysize(MDB_env *env);

	 
 
 
 
 
 
int  mdb_env_set_userctx(MDB_env *env, void *ctx);

	 
 
 
 
 
void *mdb_env_get_userctx(MDB_env *env);

	 
 
 
 
 
 
typedef void MDB_assert_func(MDB_env *env, const char *msg);

	 
 
 
 
 
 
 
int  mdb_env_set_assert(MDB_env *env, MDB_assert_func *func);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_txn_begin(MDB_env *env, MDB_txn *parent, unsigned int flags, MDB_txn **txn);

	 
 
 
 
MDB_env *mdb_txn_env(MDB_txn *txn);

	 
 
 
 
 
 
 
 
 
mdb_size_t mdb_txn_id(MDB_txn *txn);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_txn_commit(MDB_txn *txn);

	 
 
 
 
 
 
 
 
void mdb_txn_abort(MDB_txn *txn);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
void mdb_txn_reset(MDB_txn *txn);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_txn_renew(MDB_txn *txn);

 
#define mdb_open(txn,name,flags,dbi)	mdb_dbi_open(txn,name,flags,dbi)
 
#define mdb_close(env,dbi)				mdb_dbi_close(env,dbi)

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_dbi_open(MDB_txn *txn, const char *name, unsigned int flags, MDB_dbi *dbi);

	 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_stat(MDB_txn *txn, MDB_dbi dbi, MDB_stat *stat);

	 
 
 
 
 
 
 
int mdb_dbi_flags(MDB_txn *txn, MDB_dbi dbi, unsigned int *flags);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
void mdb_dbi_close(MDB_env *env, MDB_dbi dbi);

	 
 
 
 
 
 
 
 
 
int  mdb_drop(MDB_txn *txn, MDB_dbi dbi, int del);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_set_compare(MDB_txn *txn, MDB_dbi dbi, MDB_cmp_func *cmp);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_set_dupsort(MDB_txn *txn, MDB_dbi dbi, MDB_cmp_func *cmp);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_set_relfunc(MDB_txn *txn, MDB_dbi dbi, MDB_rel_func *rel);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_set_relctx(MDB_txn *txn, MDB_dbi dbi, void *ctx);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_get(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_put(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data,
			    unsigned int flags);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_del(MDB_txn *txn, MDB_dbi dbi, MDB_val *key, MDB_val *data);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_open(MDB_txn *txn, MDB_dbi dbi, MDB_cursor **cursor);

	 
 
 
 
 
 
void mdb_cursor_close(MDB_cursor *cursor);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_renew(MDB_txn *txn, MDB_cursor *cursor);

	 
 
 
 
MDB_txn *mdb_cursor_txn(MDB_cursor *cursor);

	 
 
 
 
MDB_dbi mdb_cursor_dbi(MDB_cursor *cursor);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_get(MDB_cursor *cursor, MDB_val *key, MDB_val *data,
			    MDB_cursor_op op);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_put(MDB_cursor *cursor, MDB_val *key, MDB_val *data,
				unsigned int flags);

	 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_del(MDB_cursor *cursor, unsigned int flags);

	 
 
 
 
 
 
 
 
 
 
 
 
int  mdb_cursor_count(MDB_cursor *cursor, mdb_size_t *countp);

	 
 
 
 
 
 
 
 
 
 
int  mdb_cmp(MDB_txn *txn, MDB_dbi dbi, const MDB_val *a, const MDB_val *b);

	 
 
 
 
 
 
 
 
 
 
int  mdb_dcmp(MDB_txn *txn, MDB_dbi dbi, const MDB_val *a, const MDB_val *b);

	 
 
 
 
 
 
typedef int (MDB_msg_func)(const char *msg, void *ctx);

	 
 
 
 
 
 
 
int	mdb_reader_list(MDB_env *env, MDB_msg_func *func, void *ctx);

	 
 
 
 
 
 
int	mdb_reader_check(MDB_env *env, int *dead);
 

#ifdef __cplusplus
}
#endif
 
 
 
 
 
 
 

#endif  
