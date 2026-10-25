/*
Copyright (c) 2018-2019, tevador <tevador@gmail.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the copyright holder nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Original code from Argon2 reference source code package used under CC0 Licence
 * https://github.com/P-H-C/phc-winner-argon2
 * Copyright 2015
 * Daniel Dinu, Dmitry Khovratovich, Jean-Philippe Aumasson, and Samuel Neves
*/

#ifndef ARGON2_CORE_H
#define ARGON2_CORE_H

#include <stdint.h>
#include "argon2.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CONST_CAST(x) (x)(uintptr_t)

  

enum argon2_core_constants {
	 
	ARGON2_BLOCK_SIZE = 1024,
	ARGON2_QWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 8,
	ARGON2_OWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 16,
	ARGON2_HWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 32,
	ARGON2_512BIT_WORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 64,

	 
 
 
	ARGON2_ADDRESSES_IN_BLOCK = 128,

	 
	ARGON2_PREHASH_DIGEST_LENGTH = 64,
	ARGON2_PREHASH_SEED_LENGTH = 72
};

 

 
 
 
 
 
typedef struct block_ { uint64_t v[ARGON2_QWORDS_IN_BLOCK]; } block;

 
 
 
 
 
 
typedef struct Argon2_instance_t {
	block *memory;           
	uint32_t version;
	uint32_t passes;         
	uint32_t memory_blocks;  
	uint32_t segment_length;
	uint32_t lane_length;
	uint32_t lanes;
	uint32_t threads;
	argon2_type type;
	int print_internals;  
	argon2_context *context_ptr;  
	randomx_argon2_impl *impl;
} argon2_instance_t;

 
 
 
 
typedef struct Argon2_position_t {
	uint32_t pass;
	uint32_t lane;
	uint8_t slice;
	uint32_t index;
} argon2_position_t;

 
typedef struct Argon2_thread_data {
	argon2_instance_t *instance_ptr;
	argon2_position_t pos;
} argon2_thread_data;

 

 
 
 
 
 
 
 
 
 
 
uint32_t randomx_argon2_index_alpha(const argon2_instance_t *instance,
	const argon2_position_t *position, uint32_t pseudo_rand,
	int same_lane);

 
 
 
 
 
 
 
int randomx_argon2_validate_inputs(const argon2_context *context);

 
 
 
 
 
 
 
 
 
 
int randomx_argon2_initialize(argon2_instance_t *instance, argon2_context *context);

 
 
 
 
 
 
int randomx_argon2_fill_memory_blocks(argon2_instance_t* instance);

#if defined(__cplusplus)
}
#endif

#endif
