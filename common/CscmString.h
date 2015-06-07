/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	November 2009                                       */
/* ---------------------------------------------------- */

#ifndef CSCM_STRING_H
#define CSCM_STRING_H

// ENABLE_WIDE_CHAR should be defined in Makefile or VS project file
// #define ENABLE_WIDE_CHAR

#ifdef _MSC_VER
#ifndef MS_VISUAL_CPP
#define MS_VISUAL_CPP
#endif
#endif

#ifdef ENABLE_WIDE_CHAR

#ifdef WIN32
#define b_main    wmain
#define b_argv    wargv
#else
#define b_main    main
#define b_argv    argv
#endif

#include <wchar.h>
#include <wctype.h>

typedef wchar_t   B_CHAR;
#define WIDEN(x)  L ## x
#define S(x)      WIDEN(x)
#define C(x)      WIDEN(x)
#define B_SF      S("%ls")
#define B_SF1(n)  S("%-") S(#n) S("ls")
#define B_SFNL    S("%ls\n")
#define B_NSF     S("%hs")
#define B_NSFNL   S("%hs\n")
#define b_fputs   fputws
#define b_fgets   fgetws
#define b_printf  wprintf
#define b_fprintf fwprintf
#define b_vprintf vwprintf
#define b_strlen  wcslen
#define b_strchr  wcschr
#define b_strspn  wcsspn
#define b_strtol  wcstol
#define b_strtod  wcstod
#define b_strpbrk wcspbrk
#define b_strcspn wcscspn
#define b_memcpy  wmemcpy
#define b_memset  wmemset
#define b_strcmp  wcscmp
#define b_strncmp wcsncmp
#define b_strstr  wcsstr
#define b_strcpy  wcscpy
#define b_tolower towlower
#ifdef MS_VISUAL_CPP
#define b_fopen   _wfopen_s
#define b_fsopen  _wfsopen
#define b_strtok  wcstok_s
#define b_vsnprintf(buf, len, fmt, args) _vsnwprintf_s(buf, len, _TRUNCATE, fmt, args)
#define b_snprintf_s _snwprintf_s
#define b_strncat(dst, src, len) wcsncat_s(dst, len, src, _TRUNCATE)
#define b_strncpy(dst, src, len) wcsncpy_s(dst, len, src, _TRUNCATE)
#else
#define b_strtok    wcstok
#define b_vsnprintf vswprintf
#define b_strncat   wcsncat
#define b_strncpy   wcsncpy
#endif

#else

#define b_main    main
#define b_argv    argv

#include <string.h>
#include <ctype.h>

typedef char      B_CHAR;
#define S(x)      x
#define C(x)      x
#define B_SF      S("%s")
#define B_SF1(n)  S("%-") S(#n) S("s")
#define B_SFNL    S("%s\n")
#define B_NSF     S("%s")
#define B_NSFNL   S("%s\n")
#define b_fputs   fputs
#define b_fgets   fgets
#define b_printf  printf
#define b_fprintf fprintf
#define b_vprintf vprintf
#define b_strlen  strlen
#define b_strchr  strchr
#define b_strspn  strspn
#define b_strtol  strtol
#define b_strtod  strtod
#define b_strpbrk strpbrk
#define b_strcspn strcspn
#define b_memcpy  memcpy
#define b_memset  memset
#define b_strcmp  strcmp
#define b_strncmp strncmp
#define b_strstr  strstr
#define b_strcpy  strcpy
#define b_tolower tolower
#ifdef MS_VISUAL_CPP
#define b_fopen   fopen_s
#define	b_fsopen  _fsopen
#define b_strtok  strtok_s
#define b_vsnprintf(buf, len, fmt, args) vsnprintf_s(buf, len, _TRUNCATE, fmt, args)
#define b_snprintf_s _snprintf_s
#define b_strncat(dst, src, len) strncat_s(dst, len, src, _TRUNCATE)
#define b_strncpy(dst, src, len) strncpy_s(dst, len, src, _TRUNCATE)
#else
#define b_strtok    strtok_r
#define b_vsnprintf vsnprintf
#define b_strncat   strncat
#define b_strncpy   strncpy
#endif

#endif

typedef B_CHAR         *STRING;

#define EMPTY_STRING   S("")
#define B_NUL C('\0')

#define BCHARS2BYTES(x) ((x) * sizeof(B_CHAR))
#define BYTES2BCHARS(x) ((x) / sizeof(B_CHAR))
#define BCHARSOF(x)	    (sizeof(x) / sizeof(B_CHAR))

#endif // CSCM_STRING_H
