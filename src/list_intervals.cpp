/* 
 * Copyright (c) 2013-2014, Quarkslab
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither the name of Quarkslab nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <leeloo/interval.h>
#include <leeloo/list_intervals.h>
#include <leeloo/list_intervals_random.h>
#include <leeloo/uni.h>
#include <leeloo/random.h>

#include <random>

// Compile these headers so that specific template instantiation is done.

#ifdef LEELOO_INCLUDE_U8
template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint8_t>>;
#endif

#ifdef LEELOO_INCLUDE_U16
template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint16_t>>;
#endif

#ifdef LEELOO_INCLUDE_U32
template class LEELOO_API leeloo::list_intervals<leeloo::interval<uint32_t>>;
template class LEELOO_API leeloo::list_intervals_random<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>;
template void LEELOO_API leeloo::list_intervals_random<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>::init<leeloo::random<uint32_t, std::random_device>>(leeloo::list_intervals<leeloo::interval<uint32_t>> const&, leeloo::random<uint32_t, std::random_device>&&);
template class LEELOO_API leeloo::list_intervals_random_promise<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>;
template void LEELOO_API leeloo::list_intervals_random_promise<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>::init<leeloo::random<uint32_t, std::random_device>>(leeloo::list_intervals<leeloo::interval<uint32_t>> const&, leeloo::random<uint32_t, std::random_device>&&);

#ifdef LEELOO_BOOST_SERIALIZE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

template void LEELOO_API leeloo::list_intervals_random<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>::save_state<boost::archive::text_oarchive>(boost::archive::text_oarchive&);
template void LEELOO_API leeloo::list_intervals_random<leeloo::list_intervals<leeloo::interval<uint32_t>>, leeloo::uni, false>::restore_state<boost::archive::text_iarchive>(boost::archive::text_iarchive&, leeloo::list_intervals<leeloo::interval<uint32_t>> const&, leeloo::random<uint32_t, std::random_device>&&);
#endif

#endif
