#ifndef _KIOKU_DECL_H
#define _KIOKU_DECL_H

/* Good resource: https://sourceforge.net/p/predef/wiki/Home/ */

#ifdef _WIN32
#define kiokuAPI __declspec(dllexport)
#else
#define kiokuAPI
#endif

#ifdef __cplusplus
#define kiokuAPI extern "C" kiokuAPI
#endif

#define KIOKU_DIRSEP "/"

#ifdef __APPLE__
  #define kiokuOS_APPLE
  #include "TargetConditionals.h"
  #ifdef TARGET_OS_IPHONE
    #define kiokuOS_IOS
    #define kiokuOS_MOBILE
  // iOS
  #elif TARGET_IPHONE_SIMULATOR
    #define kiokuOS_IOS
    #define kiokuOS_MOBILE
  // iOS Simulator
  #elif TARGET_OS_MAC
  // Other kinds of Mac OS
  #else
  // Unsupported platform
  #error Unsupported Apple OS
  #endif
#elif defined _WIN32 || defined _WIN64
#define kiokuOS_WINDOWS
#elif defined __linux__ || __gnu_linux__
#define kiokuOS_LINUX
#elif defined __unix__ || defined __unix
#define kiokuOS_UNIX
#elif defined __ANDROID__
#define kiokuOS_ANDROID
#define kiokuOS_MOBILE
#else
#error Unsupported OS
#endif

/* https://sourceforge.net/p/predef/wiki/Architectures/ */
#if /* 64-bit */                                                        \
  /* ARM */                                                             \
  defined __amd64__ || defined __x86_64__ || defined __x86_64 ||        \
  defined _MX64 || defined M_AMD64 ||                                   \
  /* ARM64 */                                                           \
  defined __aarch64__ ||                                                \
  /* Intel Itanium (IA-64) */                                           \
  defined __ia64__  || defined _M_IA64 || defined __itanium__
#define kiokuARCH_64BIT
#elif /* 32-bit */ \
  defined __i386__ || defined _M_IX86 || defined _X86_
#define kiokuARCH_32BIT
#else
  #error Unsupported architecture
#endif

#endif /* _KIOKU_DECL_H */

