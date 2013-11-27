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
