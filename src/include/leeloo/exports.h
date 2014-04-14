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

#ifndef LEELOO_EXPORTS_H
#define LEELOO_EXPORTS_H

#if defined _WIN32 || defined __CYGWIN__
  #define LEELOO_HELPER_IMPORT __declspec(dllimport)
  #define LEELOO_HELPER_EXPORT __declspec(dllexport)
  #define LEELOO_HELPER_LOCAL
#else
  #define LEELOO_HELPER_IMPORT __attribute__ ((visibility ("default")))
  #define LEELOO_HELPER_EXPORT __attribute__ ((visibility ("default")))
  #define LEELOO_HELPER_LOCAL  __attribute__ ((visibility ("hidden")))
#endif

#ifdef leeloo_EXPORTS
  #define LEELOO_API LEELOO_HELPER_EXPORT
  #define LEELOO_LOCAL LEELOO_HELPER_LOCAL
  #define LEELOO_TEMPLATE_EXPIMP
#else
  #define LEELOO_API LEELOO_HELPER_IMPORT
  #define LEELOO_LOCAL LEELOO_HELPER_LOCAL
  #define LEELOO_TEMPLATE_EXPIMP extern
#endif

#include <leeloo/instances_exported.h>

#endif
