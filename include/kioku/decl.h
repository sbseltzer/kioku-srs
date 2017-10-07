#ifndef _KIOKU_DECL_H
#define _KIOKU_DECL_H

#ifdef _WIN32
#define kiokuAPI __declspec(dllexport)
#else
#define kiokuAPI extern
#endif

#define KIOKU_DIRSEP "/"

#endif /* _KIOKU_DECL_H */
