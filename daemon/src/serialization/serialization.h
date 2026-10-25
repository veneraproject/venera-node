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
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>
#include <string>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/mpl/bool.hpp>

 
 
 
 
template <class T>
struct is_blob_type { typedef boost::false_type type; };

 
 
 
 
 
 
 
 
template <class Archive, class T>
inline std::enable_if_t<is_blob_type<T>::type::value, bool> do_serialize(Archive &ar, T &v)
{
  ar.serialize_blob(&v, sizeof(v));
  return true;
}
template <class Archive, class T>
inline std::enable_if_t<boost::is_integral<T>::value, bool> do_serialize(Archive &ar, T &v)
{
  ar.serialize_int(v);
  return true;
}
template <class Archive, class T>
inline auto do_serialize(Archive &ar, T &v) -> decltype(v.member_do_serialize(ar), true)
{
  return v.member_do_serialize(ar);
}
template <class Archive>
inline bool do_serialize(Archive &ar, bool &v)
{
  ar.serialize_blob(&v, sizeof(v));
  return true;
}

 

 
 
 
 
#define BLOB_SERIALIZER(T)						\
  template<>								\
  struct is_blob_type<T> {						\
    typedef boost::true_type type;					\
  }

 
 
 
 
#define VARIANT_TAG(Archive, Type, Tag)					\
  template <bool W>							\
  struct variant_serialization_traits<Archive<W>, Type> {		\
    static inline typename Archive<W>::variant_tag_type get_tag() {	\
      return Tag;							\
    }									\
  }

 
 
 
 
 
 
#define BEGIN_SERIALIZE()						\
  template <bool W, template <bool> class Archive>			\
  bool member_do_serialize(Archive<W> &ar) {

 
 
 
 
 
#define BEGIN_SERIALIZE_OBJECT()					\
  template <bool W, template <bool> class Archive>			\
  bool member_do_serialize(Archive<W> &ar) {					\
    ar.begin_object();							\
    bool r = do_serialize_object(ar);					\
    ar.end_object();							\
    return r;								\
  }									\
  template <bool W, template <bool> class Archive>			\
  bool do_serialize_object(Archive<W> &ar){

 
 
 
#define PREPARE_CUSTOM_VECTOR_SERIALIZATION(size, vec, min_wire)	\
  do \
  { \
    if (!::serialization::detail::prepare_custom_vector_serialization(size, vec, min_wire, ar)) \
      return false; \
  } while (0);

 
 
 
#define END_SERIALIZE()				\
  return ar.good();				\
  }

 
 
 
 
#define FIELD_N(t, f)					\
  do {							\
    ar.tag(t);						\
    bool r = do_serialize(ar, f);			\
    if (!r || !ar.good()) return false;			\
  } while(0);

 
 
 
 
#define FIELD(f)					\
  do {							\
    ar.tag(#f);						\
    bool r = do_serialize(ar, f);			\
    if (!r || !ar.good()) return false;			\
  } while(0);

 
 
 
 
#define FIELDS(f)							\
  do {									\
    bool r = do_serialize(ar, f);					\
    if (!r || !ar.good()) return false;					\
  } while(0);

 
 
 
#define VARINT_FIELD(f)				\
  do {						\
    ar.tag(#f);					\
    ar.serialize_varint(f);			\
    if (!ar.good()) return false;		\
  } while(0);

 
 
 
 
#define VARINT_FIELD_N(t, f)			\
  do {						\
    ar.tag(t);					\
    ar.serialize_varint(f);			\
    if (!ar.good()) return false;		\
  } while(0);

 
 
#define MAGIC_FIELD(m)				\
  std::string magic = m;			\
  do {						\
    ar.tag("magic");				\
    ar.serialize_blob((void*)magic.data(), magic.size()); \
    if (!ar.good()) return false;		\
    if (magic != m) return false;		\
  } while(0);

 
 
#define VERSION_FIELD(v)			\
  uint32_t version = v;				\
  do {						\
    ar.tag("version");				\
    ar.serialize_varint(version);		\
    if (!ar.good()) return false;		\
  } while(0);

 
 
 
 
#define CONTAINER_FIELD_CAPPED(f, c)					\
  do {							\
    ar.tag(#f);						\
    bool r = do_serialize(ar, f, c);			\
    if (!r || !ar.good()) return false;			\
  } while(0);

namespace serialization {

   
 
 
 
 
  namespace detail
  {
     
 
 
 
    template <typename T, template<bool> class A>
    constexpr bool prepare_custom_vector_serialization(size_t, const std::vector<T>& vec, size_t, const A<true>&) noexcept
    {
      return true;
    }

    template <typename T, template<bool> class A>
    bool prepare_custom_vector_serialization(const size_t size, std::vector<T>& vec, const size_t min_size, const A<false>& ar)
    {
      if (size && ar.remaining_bytes() / size < min_size)
        return false;
      vec.resize(size);
      return true;
    }

     
 
 
 
    template<class Archive>
    bool do_check_stream_state(Archive& ar, boost::mpl::bool_<true>, bool noeof)
    {
      return ar.good();
    }
     
 
 
 
 
 
    template<class Archive>
    bool do_check_stream_state(Archive& ar, boost::mpl::bool_<false>, bool noeof)
    {
      bool result = false;
      if (ar.good())
	{
	  result = noeof || ar.eof();
	}
      return result;
    }
  }

   
 
 
 
  template<class Archive>
  bool check_stream_state(Archive& ar, bool noeof = false)
  {
    return detail::do_check_stream_state(ar, typename Archive::is_saving(), noeof);
  }

   
 
 
 
  template <class Archive, class T>
  inline bool serialize(Archive &ar, T &v)
  {
    bool r = do_serialize(ar, v);
    return r && check_stream_state(ar, false);
  }

   
 
 
 
  template <class Archive, class T>
  inline bool serialize_noeof(Archive &ar, T &v)
  {
    bool r = do_serialize(ar, v);
    return r && check_stream_state(ar, true);
  }
}
