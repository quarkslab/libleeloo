/* 
 * Copyright (c) 2013, Quarkslab
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

#ifndef LEELO_IPS_PARSER_H
#define LEELO_IPS_PARSER_H

#include <cstdlib>
#include <cstdint>
#include <type_traits>

#include <leeloo/exports.h>

namespace leeloo {

class ip_list_intervals;

namespace ips_parser {

typedef void(*interval_callback)(uint32_t a, uint32_t b);

extern LEELOO_API uint32_t ipv4toi(const char* str, bool& valid, int min_dots = 3);
extern LEELOO_API uint32_t ipv4toi(const char* str, const size_t size, bool& valid, int min_dots = 3);

extern LEELOO_API bool parse_ips_add(ip_list_intervals& l, const char* str);
extern LEELOO_API bool parse_ips_remove(ip_list_intervals& l, const char* str);

template <bool exclude = false>
inline bool parse_ips(typename std::enable_if<exclude == true, ip_list_intervals&>::type l, const char* str)
{
	return parse_ips_remove(l, str);
}

template <bool exclude = false>
inline bool parse_ips(typename std::enable_if<exclude == false, ip_list_intervals&>::type l, const char* str)
{
	return parse_ips_add(l, str);
}

}

}

#endif
