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
#pragma once

#include <vector>
#include <string>
#include <functional>
#include <boost/optional/optional_fwd.hpp>
#include <boost/utility/string_ref_fwd.hpp>

namespace tools
{

 
constexpr const int DNS_CLASS_IN  = 1;
constexpr const int DNS_TYPE_A    = 1;
constexpr const int DNS_TYPE_TXT  = 16;
constexpr const int DNS_TYPE_AAAA = 8;
constexpr const int DNS_TYPE_TLSA = 52;

struct DNSResolverData;

 
 
 
 
 
 
 
class DNSResolver
{
private:

   
 
 
 
 
  DNSResolver();

public:

   
 
 
  ~DNSResolver();

   
 
 
 
 
 
 
 
 
 
 
 
  std::vector<std::string> get_ipv4(const std::string& url, bool& dnssec_available, bool& dnssec_valid);

   
 
 
 
 
 
 
 
 
 
   std::vector<std::string> get_ipv6(const std::string& url, bool& dnssec_available, bool& dnssec_valid);

   
 
 
 
 
 
 
 
   
   std::vector<std::string> get_txt_record(const std::string& url, bool& dnssec_available, bool& dnssec_valid);

   
 
 
 
 
 
 
 
 
  std::vector<std::string> get_tlsa_tcp_record(boost::string_ref url, boost::string_ref port, bool& dnssec_available, bool& dnssec_valid);

   
 
 
 
 
 
 
 
 
 
  std::string get_dns_format_from_oa_address(const std::string& oa_addr);

   
 
 
 
 
  static DNSResolver& instance();

   
 
 
 
 
  static DNSResolver create();

private:

   
 
 
 
 
 
 
 
 
 
   
  std::vector<std::string> get_record(const std::string& url, int record_type, boost::optional<std::string> (*reader)(const char *,size_t), bool& dnssec_available, bool& dnssec_valid);

  DNSResolverData *m_data;
};  

namespace dns_utils
{

std::string address_from_txt_record(const std::string& s);
std::vector<std::string> addresses_from_url(const std::string& url, bool& dnssec_valid);

std::string get_account_address_as_str_from_url(const std::string& url, bool& dnssec_valid, std::function<std::string(const std::string&, const std::vector<std::string>&, bool)> confirm_dns);

bool load_txt_records_from_dns(std::vector<std::string> &records, const std::vector<std::string> &dns_urls);

std::vector<std::string> parse_dns_public(const char *s);

}   

}   
