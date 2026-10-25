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

#pragma once

 
#define RANDOMX_ARGON_MEMORY       262144

 
#define RANDOMX_ARGON_ITERATIONS   3

 
#define RANDOMX_ARGON_LANES        1

 
#define RANDOMX_ARGON_SALT         "RandomX\x03"

 
#define RANDOMX_CACHE_ACCESSES     8

 
#define RANDOMX_SUPERSCALAR_LATENCY   170

 
#define RANDOMX_DATASET_BASE_SIZE  2147483648

 
#define RANDOMX_DATASET_EXTRA_SIZE 33554368

 
#define RANDOMX_PROGRAM_SIZE       256

 
#define RANDOMX_PROGRAM_ITERATIONS 2048

 
#define RANDOMX_PROGRAM_COUNT      8

 
#define RANDOMX_SCRATCHPAD_L3      2097152

 
#define RANDOMX_SCRATCHPAD_L2      262144

 
#define RANDOMX_SCRATCHPAD_L1      16384

 
#define RANDOMX_JUMP_BITS          8

 
#define RANDOMX_JUMP_OFFSET        8

 
 
 
 

 
#define RANDOMX_FREQ_IADD_RS       16
#define RANDOMX_FREQ_IADD_M         7
#define RANDOMX_FREQ_ISUB_R        16
#define RANDOMX_FREQ_ISUB_M         7
#define RANDOMX_FREQ_IMUL_R        16
#define RANDOMX_FREQ_IMUL_M         4
#define RANDOMX_FREQ_IMULH_R        4
#define RANDOMX_FREQ_IMULH_M        1
#define RANDOMX_FREQ_ISMULH_R       4
#define RANDOMX_FREQ_ISMULH_M       1
#define RANDOMX_FREQ_IMUL_RCP       8
#define RANDOMX_FREQ_INEG_R         2
#define RANDOMX_FREQ_IXOR_R        15
#define RANDOMX_FREQ_IXOR_M         5
#define RANDOMX_FREQ_IROR_R         8
#define RANDOMX_FREQ_IROL_R         2
#define RANDOMX_FREQ_ISWAP_R        4

 
#define RANDOMX_FREQ_FSWAP_R        4
#define RANDOMX_FREQ_FADD_R        16
#define RANDOMX_FREQ_FADD_M         5
#define RANDOMX_FREQ_FSUB_R        16
#define RANDOMX_FREQ_FSUB_M         5
#define RANDOMX_FREQ_FSCAL_R        6
#define RANDOMX_FREQ_FMUL_R        32
#define RANDOMX_FREQ_FDIV_M         4
#define RANDOMX_FREQ_FSQRT_R        6

 
#define RANDOMX_FREQ_CBRANCH       25
#define RANDOMX_FREQ_CFROUND        1

 
#define RANDOMX_FREQ_ISTORE        16

 
#define RANDOMX_FREQ_NOP            0
 
 
 
