/* 
 * Copyright (c) 2014, Quarkslab
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

#include <leeloo/port.h>
#include <leeloo/port_list_intervals.h>
#include <iostream>

int compare_intervals(leeloo::port_list_intervals const& l, uint32_t const* const ref, size_t const ninter)
{
	if (l.intervals().size() != ninter) {
		std::cerr << "bad number of intervals" << std::endl;
		return 1;
	}
	int ret = 0;
	size_t i = 0;
	for (auto const& it: l.intervals()) {
		if ((it.lower() != ref[2*i]) ||
			(it.upper() != ref[2*i+1])) {
			std::cerr << "invalid interval at index " << i << std::endl;
			ret = 1;
		}
		i++;
	}
	return ret;
}

int main()
{
	int ret = 0;

	leeloo::port_list_intervals l;
	l.add(leeloo::port(80, leeloo::port::protocol_enum::TCP));
	l.add(leeloo::port(443, leeloo::port::protocol_enum::TCP));
	l.add(leeloo::port(53, leeloo::port::protocol_enum::UDP));
	l.add(leeloo::port(0xFFFF, leeloo::port::protocol_enum::UDP));
	l.add(leeloo::port(80, leeloo::port::protocol_enum::SCTP));
	l.aggregate();

	uint32_t intervals0[] = {
		0x01000050, 0x01000051,
		0x010001BB, 0x010001BC,
		0x02000035, 0x02000036,
		0x0200FFFF, 0x02010000,
		0x03000050, 0x03000051,
	};

	ret = compare_intervals(l, intervals0, sizeof(intervals0)/(2*sizeof(uint32_t)));
	l.aggregate();
	ret = compare_intervals(l, intervals0, sizeof(intervals0)/(2*sizeof(uint32_t)));

	l.clear();

	l.add(leeloo::port(80, leeloo::port::protocol_enum::TCP));
	l.add(leeloo::port(443, leeloo::port::protocol_enum::TCP));
	l.add(leeloo::port(53, leeloo::port::protocol_enum::UDP));
	l.add(leeloo::port(80, leeloo::port::protocol_enum::SCTP));
	l.add(0, 0xFFFF, leeloo::port::protocol_enum::TCP);
	l.add(0, 0xFFFF, leeloo::port::protocol_enum::UDP);
	l.add(0, 0xFFFF, leeloo::port::protocol_enum::SCTP);

	l.aggregate();

	uint32_t intervals1[] = {
		0x01000000, 0x01010000,
		0x02000000, 0x02010000,
		0x03000000, 0x03010000
	};

	ret = compare_intervals(l, intervals1, sizeof(intervals1)/(2*sizeof(uint32_t)));

	return ret;
}
