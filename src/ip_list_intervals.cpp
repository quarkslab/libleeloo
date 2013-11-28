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

#include <leeloo/ip_list_intervals.h>
#include <leeloo/ips_parser.h>

#include <cstdint>

template <bool exclude>
static bool __insert(leeloo::ip_list_intervals& obj, const char* str_interval)
{
	return leeloo::ips_parser::parse_ips<exclude>(obj, str_interval);
}

bool leeloo::ip_list_intervals::add(const char* str_interval)
{
	return __insert<false>(*this, str_interval);
}

bool leeloo::ip_list_intervals::remove(const char* str_interval)
{
	return __insert<true>(*this, str_interval);
}

bool leeloo::ip_list_intervals::contains(const char* ip_str) const
{
	bool valid = false;
	uint32_t ip = leeloo::ips_parser::ipv4toi(ip_str, strlen(ip_str), valid);
	if (!valid) {
		return false;
	}

	return contains(ip);
}
