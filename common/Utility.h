/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------------------------
// Header file for Utility.c, which contains various miscellaneous utility
// functions.
// -------------------------------------------------------------------------

#ifndef UTILITY_H
#define UTILITY_H

// Define the following to enable PressEnterToContinue
//#define ENABLE_CONSOLE_PAUSE 

// Define to get a debug trace for WcmMalloc and WcmFree
// #define DEBUG_MALLOC

#include "CscmTypes.h"
#include <stdio.h>

#define STRING_INVALID_STR_LEN -1

#define ONE_MEGA_BYTES  1048576
STRING BuildCommandLine(int argc, STRING argv[]);
void PressEnterToContinue(void);
int StringLen(const STRING ws);
STRING StrPrintf(STRING bszBuffer, size_t size, const STRING bszFormat, ...);
STRING StrKHzToMHz(STRING bszBuffer, size_t size, const UINT32 u32ValueKHz);
char * StringToCharStar(STRING bszString);
UINT32 ArraySumUint32(UINT32 au32Array[], UINT32 u32Size);
UINT32 ArraySortUint32(UINT32 au32Array[], UINT32 u32Size);
INT32 HexStringToUint8Array(STRING bszString, UINT8 *au8Data, UINT32 u32DataLen);
#ifdef DEBUG_MALLOC
#define WcmMalloc(x) _WcmMalloc(x, __FILE__, __LINE__)
#define WcmFree(x)   _WcmFree(x, __FILE__, __LINE__)
void * _WcmMalloc(UINT32 u32Size, char * szFile, UINT32 u32Line);
void _WcmFree(void *p, char * szFile, UINT32 u32Line);
#else
#define WcmMalloc(s)  malloc(s)
#define WcmFree(p)    if (p) free(p)
#endif
BOOL ParseVersionString(STRING bszVersion, INT32 ai32VersionID[], UINT32 u32ArraySize, UINT32 *u32ActualSize);

// Does a file exist?
BOOL DoesFileExist(STRING bszFilePath);

// Wide char string functions
INT32 BcsToInt32(const STRING bszString, BOOL *pbError);
BOOL   BcsnCmpNoCase(STRING bszString1, STRING bszString2, UINT32 n); 
BOOL  BcsCmpNoCase(STRING bszString1, STRING bszString2);
BOOL  BcsStrStrNoCase(STRING bszString, STRING bszSubString);
FILE *BcsFopen(STRING bszFilePath, STRING bszMode);
FILE *BcsFopenShare(STRING bszFilePath, STRING bszMode, BOOL bReadOnly);
#ifdef ENABLE_WIDE_CHAR
BOOL  WcsCreateWargv(STRING wargv[], int iSizeWargv, int argc, char *argv[]);
BOOL  WcsDestroyWargv(int argc, STRING wargv[]);
#endif

#define BCSERROR ((size_t) -1)
size_t b_mbstobcs(STRING bszDst, const char *szSrc, size_t n);
size_t b_bcstombs(char *szDst, const STRING bszSrc, size_t n);
B_CHAR *b_bcsncpy(STRING bszDst, const STRING bszSrc, size_t n);

#endif


