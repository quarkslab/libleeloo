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

#ifndef __BENCH_H
#define __BENCH_H

#include <leeloo/helpers.h>

#define BENCH_START(var)\
	double __bench_start_##var = leeloo::get_current_timestamp();

#define BENCH_END_NODISP(var)\
	double __bench_end_##var = leeloo::get_current_timestamp();

#define BENCH_END(var, desc, nelts_start, size_elt_start, nelts_end, size_elt_end)\
	BENCH_END_NODISP(var);\
	{\
		double time = (__bench_end_##var-__bench_start_##var);\
		double size_in_mb = (double)((size_t)nelts_start*(size_t)size_elt_start)/(1024.0*1024.0);\
		double size_out_mb = (double)((size_t)nelts_end*(size_t)size_elt_end)/(1024.0*1024.0);\
		double bw_in = size_in_mb/time;\
		double bw_out = size_out_mb/time;\
		fprintf(stderr, "%s: in %0.5f ms. Input (#/size/BW): %lu/%0.5f MB/%0.5f MB/s | Output (#/size/BW): %lu/%0.5f MB/%0.5f MB/s\n",\
		       desc, time*1000.0, (size_t) nelts_start, size_in_mb, bw_in, (size_t) nelts_end, size_out_mb, bw_out);\
	}

#define BENCH_END_TIME(var)\
		((__bench_end_##var-__bench_start_##var))

#define BENCH_END_SAME_TYPE(var, desc, nelts_in, nelts_out, size_elt) BENCH_END(var, desc, nelts_in, size_elt, nelts_out, size_elt)
#define BENCH_END_TRANSFORM(var, desc, nelts, size_elt) BENCH_END_SAME_TYPE(var, desc, nelts, nelts, size_elt)

#ifdef __cplusplus
#define CHECK(v) __CHECK(v, __FILE__, __LINE__)
#define __CHECK(v,F,L)\
	if (!(v)) {\
		std::cerr << F << ":" << L << " :" << #v << " isn't valid." << std::endl;\
	}
#endif

#endif
