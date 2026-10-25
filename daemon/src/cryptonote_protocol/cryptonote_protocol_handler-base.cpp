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

#include <string>
#include <vector>

#include <memory>

#include "syncobj.h"

#include "net/net_utils_base.h" 
#include "misc_log_ex.h" 
#include <boost/chrono.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 
#include "misc_language.h"
#include <algorithm>


#include "cryptonote_protocol_handler.h"
#include "net/network_throttle.hpp"

#include "cryptonote_core/cryptonote_core.h"  

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "net.cn"

 
 
 
 
 
 
 

namespace cryptonote {

class cryptonote_protocol_handler_base_pimpl {  
	public:

};

}  

 
 
 
 

namespace cryptonote { 

double cryptonote_protocol_handler_base::estimate_one_block_size() noexcept {  
 const double size_min = 500;  
  

 double avg=0;
 try {
 avg = get_avg_block_size( );
 } catch (...) { }
 avg = std::max( size_min , avg);
 return avg;
}

cryptonote_protocol_handler_base::cryptonote_protocol_handler_base() {
}

cryptonote_protocol_handler_base::~cryptonote_protocol_handler_base() {
}

void cryptonote_protocol_handler_base::handler_request_blocks_history(std::list<crypto::hash>& ids) {
}

void cryptonote_protocol_handler_base::handler_response_blocks_now(size_t packet_size) {
	using namespace epee::net_utils;
	double delay=0;  
	MDEBUG("Packet size: " << packet_size);
	do
	{  
		 
		 
 
 
 
		 
 
 
 

		{ 
	  	CRITICAL_REGION_LOCAL(	network_throttle_manager::m_lock_get_global_throttle_out );
			delay = network_throttle_manager::get_global_throttle_out().get_sleep_time_after_tick( packet_size );
		}

		
		delay *= 0.50;
		 
		if (delay > 0) {
			 
            		long int ms = (long int)(delay * 1000);
			MDEBUG("Sleeping for " << ms << " ms before packet_size="<<packet_size);  
			boost::this_thread::sleep(boost::posix_time::milliseconds( ms ) );  
		}
	} while(delay > 0);

 
	{
	  CRITICAL_REGION_LOCAL(	network_throttle_manager::m_lock_get_global_throttle_out );
		network_throttle_manager::get_global_throttle_out().handle_trafic_tcp( packet_size );  
		 
		 
	}
}

}  


