/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------------------------
// Utility.c contains miscellaneous utility functions.
// -------------------------------------------------------------------------

#if !defined(__APPLE__)
#include <malloc.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BeceemCscmCommon.h"
#include "Utility.h"
#ifdef WIN32
#include <share.h>
#endif

#ifdef DEBUG_MALLOC
static UINT32 g_u32HeapBlockID    = 0;
static UINT32 g_u32TotalHeapBytes = 0;
#endif

#define utility_wprintf b_printf

// -------------------------------------------------------------------------
//
// STRING BuildCommandLine(int argc, STRING argv[])
//
// Description: Builds a command from an array of args
//
// Input parameters:
//		argc   - Number of args
//	    argv[] - Wide string array
//
// Returns: Command as a dynamically allocated string, WHICH MUST BE FREED
//          BY THE CALLING FUNCTION.
// -------------------------------------------------------------------------

STRING BuildCommandLine(int argc, STRING argv[])
	{

	int n;
	UINT32 u32CmdLen, u32NxtChar, u32ArgLen;
	STRING bszCommand;
	BOOL   bNeedsQuotes;

	if (argc <= 0)
		return NULL;

	// Compute the command line length
	u32CmdLen = 0;
	for (n=0; n<argc; n++)
		{
		u32ArgLen = StringLen(argv[n]);

		u32CmdLen += u32ArgLen;
		if (b_strcspn(argv[n], DELIMITERS) < u32ArgLen)
			u32CmdLen += 2;		// for 2 quotation marks
		if (n != argc-1)
			u32CmdLen++;		// for space

		}

	// Allocate the buffer
	if ((bszCommand = WcmMalloc((u32CmdLen+1)*sizeof(B_CHAR))) == NULL)
		return NULL;

	// Assemble the command line
	u32NxtChar = 0;
	for (n=0; n<argc; n++)
		{

		u32ArgLen = StringLen(argv[n]);

		// If arg contains a space or tab, we need quotation marks
		bNeedsQuotes = b_strcspn(argv[n], DELIMITERS) < u32ArgLen;

		if (bNeedsQuotes)
			bszCommand[u32NxtChar++] = C('"');

		b_memcpy(bszCommand + u32NxtChar, argv[n], u32ArgLen);
		u32NxtChar += u32ArgLen;

		if (bNeedsQuotes)
			bszCommand[u32NxtChar++] = C('"');

		if (n != argc-1)
			bszCommand[u32NxtChar++] = C(' ');

		}

	bszCommand[u32NxtChar] = B_NUL;

	return bszCommand;

	} /* BuildComamndLine */

// -------------------------------------------------------------------------
// STRING StrPrintf(STRING bszBuffer, size_t size, const STRING bszFormat, ...)
//
// Description: Wprintf to a string buffer of a specified size
//
// Input parameter:
//		size      = Buffer size in characters
//	    bszFormat = Format
//      ...       = Other args
//
// Output parameter:
//      bszBuffer = output buffer
//
// Returns:
//		bszBuffer (copy of function arg)
// -------------------------------------------------------------------------

STRING StrPrintf(STRING bszBuffer, size_t size, const STRING bszFormat, ...)
	{
	va_list args;
	va_start(args, bszFormat);

	if (!bszBuffer)
		return NULL;

	b_vsnprintf(bszBuffer, size, bszFormat, args);

	return bszBuffer;

	} /* StrPrintf */

// -------------------------------------------------------------------------
// int inline StringLen(const STRING s)
//
// Description: Returns the length of a C string.
//
// Input parameter:
//		s  = C string 
//
// Returns:
//		length of s, if s is a valid pointer; otherwise, NULL
// -------------------------------------------------------------------------

int StringLen(const STRING s)
	{

	if (!s)
		return STRING_INVALID_STR_LEN;

	return (int) b_strlen(s);

	} /* StringLen */

// -------------------------------------------------------------------------
// char * StringToCharStar(STRING bszString)
//
// Description: Returns the char * equivalent of a wide char string,
//              in a buffer allocated on the heap. The calling program
//              must free the buffer after it is done with the string.
//
// Input parameter:
//		bszString = wide char string to be converted 
//
// Returns:
//		ASCII C string, or
//      NULL if malloc() fails
// -------------------------------------------------------------------------

char * StringToCharStar(STRING bszString)
	{

	size_t sizStrLen;
	char * szString;

	if (bszString == NULL)
		return NULL;

	sizStrLen = b_strlen(bszString);

	if ((szString = (char *) WcmMalloc((UINT32) 2*(sizStrLen + 1))) == NULL)
		return NULL;

	if (b_bcstombs(szString, bszString, 2*(sizStrLen + 1)) == BCSERROR)
		return NULL;

	return szString;

	} /* StringToCharStar */

// -------------------------------------------------------------------------
// UINT32 ArraySumUint32(UINT32 au32Array[], UINT32 u32Size)
//
//
// Description: Returns the sum of an array of UINT32
//
// -------------------------------------------------------------------------

UINT32 ArraySumUint32(UINT32 au32Array[], UINT32 u32Size)
	{
	UINT32 n, u32Sum = 0;

	if (au32Array == NULL)
		return u32Sum;

	for (n=0; n<u32Size; n++)
		u32Sum += au32Array[n];

	return u32Sum;

	} /* ArraySumUint32 */

// -------------------------------------------------------------------------
// UINT32 ArraySortUint32(UINT32 au32Array[], UINT32 u32Size)
//
// Description: Sorts the UINT32 array in place, removing any duplicates,
//              in ascending order. Uses an insertion sort.
//      
// Returns: The length of the sorted array, which will be <= u32Size.
//
// -------------------------------------------------------------------------

UINT32 ArraySortUint32(UINT32 au32Array[], UINT32 u32Size)
	{
	
	UINT32 u32LastSortedIndex, u32NextUnsortedIndex, u32NextUnsortedValue;
	int m, m1, m2;

	if (u32Size <= 1)
		return u32Size;

	u32LastSortedIndex = 0;
	for (u32NextUnsortedIndex = 1; u32NextUnsortedIndex < u32Size; u32NextUnsortedIndex++)
		{

		// Do a binary search to locate the insertion point
		u32NextUnsortedValue = au32Array[u32NextUnsortedIndex];
		m1 = 0;
		m2 = u32LastSortedIndex;
		while (m1 < m2)
			{
			m = (m1+m2)/2;
			if (u32NextUnsortedValue < au32Array[m])
				m2 = m - 1;
			else if (u32NextUnsortedValue > au32Array[m])
				m1 = m + 1;
			else
				{
				m1 = m;
				break;
				}
			}

		// Insert the value and drop any duplicate values
		if (u32NextUnsortedValue != au32Array[m1])
			{
			if (u32NextUnsortedValue > au32Array[m1])
				m1++;
			for (m = u32LastSortedIndex; m >= m1; m--)
				au32Array[m+1] = au32Array[m];
			au32Array[m1] = u32NextUnsortedValue;
			u32LastSortedIndex++;
			}
		}

	return u32LastSortedIndex+1;

	} /* ArraySortUint32 */

// -------------------------------------------------------------------------
// INT32 StringToUint8Array(STRING bszString, UINT8 *au8Data, UINT32 u32DataLen)
//
// Description: Returns a byte array equivalent of a wide char string, which
//              must consist of a string of hex bytes
//
// Input parameters:
//		bszString  = wide char string to be converted 
//		u32DataLen = length of output buffer, in bytes
//
// Output parameter:
//      au8Data    = Output data array
//
// Returns:
//		Actual length of data; -1 on error
// -------------------------------------------------------------------------

INT32 HexStringToUint8Array(STRING bszString, UINT8 *au8Data, UINT32 u32DataLen)
	{

	UINT32 m, n, u32NumBytes, u32NumChars;
	B_CHAR wc;
	BOOL bStore;
	UINT8 u8Digit;

	memset(au8Data, 0, u32DataLen);

	u32NumChars = (UINT32) b_strlen(bszString);
	u32NumBytes = u32NumChars / 2;
	if (u32NumChars % 2 != 0 || u32NumBytes > u32DataLen)
		return -1;

	n = 0;
	bStore = FALSE;
	for (m=0; m<u32NumChars; m++)
		{
		wc = bszString[m];

		if (wc >= C('0') && wc <= C('9'))
			u8Digit = (UINT8) (wc - '0');
		else if (wc >= C('A') && wc <= C('F'))
			u8Digit = (UINT8) (wc - C('A') + 10);
		else if (wc >= C('a') && wc <= C('f'))
			u8Digit = (UINT8) (wc - C('a') + 10);
		else
			return -1;

		if (bStore)
			{
			au8Data[n] = (au8Data[n] << 4) + u8Digit;
			n++;
			}
		else
			au8Data[n] = u8Digit;

		bStore = !bStore;

		}

	return n;

	} /* HexStringToUint8Array */

// -----------------------------------------------------------------------------------------
// BOOL ParseVersionString(STRING bszVersion, UINT32 au32VersionID[], UINT32 u32ArraySize, UINT32 *u32ActualSize)
//
// Description: Extracts version number components from a version number string.
// -----------------------------------------------------------------------------------------

BOOL ParseVersionString(STRING bszVersion, INT32 ai32VersionID[], UINT32 u32ArraySize, UINT32 *u32ActualSize)
	{

	UINT32 n;
	B_CHAR *wcp = bszVersion, *wdot, *wend;
	BOOL bDone = FALSE;

	if (!u32ActualSize || !bszVersion || !ai32VersionID)
		return FALSE;

	for (n=0; n<u32ArraySize && !bDone; n++)
		{
		if ((wdot = b_strchr(wcp, C('.'))) != NULL)
			*wdot = B_NUL;
		else 
			bDone = TRUE;

		ai32VersionID[n] = b_strtol(wcp, &wend, 10);
		if (*wend != B_NUL)
			return FALSE;
		*u32ActualSize = n + 1;
		wcp = wdot + 1;
		}

	return TRUE;

	} /* ParseVersionString */

#ifdef DEBUG_MALLOC
// -----------------------------------------------------------------------------------------
// void * WcmMalloc(UINT32 u32Size)
//
// Description: Dynamically allocates a block of memory, with an optional printed trace.
// -----------------------------------------------------------------------------------------

void * _WcmMalloc(UINT32 u32Size, char *szFile, UINT32 u32Line)
	{

	void * p;
	UINT32 *pu32Temp;

	p = malloc(u32Size + 2*sizeof(UINT32));

	if (p)
		{
		pu32Temp = (UINT32 *) p;
		pu32Temp[0] = g_u32HeapBlockID; // Save the block ID
		pu32Temp[1] = u32Size;		    // Save the block size
		g_u32TotalHeapBytes += u32Size;
		utility_wprintf(S("MEM %6d: Alloc %6d bytes (%6d heap bytes) at %p in ") B_NSF S(":%d\n"),
			g_u32HeapBlockID, u32Size, g_u32TotalHeapBytes, pu32Temp + 2, szFile, u32Line);
		g_u32HeapBlockID++;
		return (void *) (pu32Temp + 2);
		}
	else
		{
		utility_wprintf(S("MEM: Malloc returned NULL pointer in ") B_NSF S(":%d.\n"), szFile, u32Line);
		return p;
		}

	} /* WcmMalloc */

// -----------------------------------------------------------------------------------------
// void WcmFree(void *p)
//
// Description: Frees a dynamically allocated block, with an optional printed trace.
// -----------------------------------------------------------------------------------------

void _WcmFree(void *p, char *szFile, UINT32 u32Line)
	{
	if (p)
		{
		UINT32 *pu32Temp = ((UINT32 *) p) - 2;
		UINT32 u32BlockID = pu32Temp[0];
		UINT32 u32Size    = pu32Temp[1];
		g_u32TotalHeapBytes -= u32Size;
		utility_wprintf(S("MEM %6d: Free  %6d bytes (%6d heap bytes) at %p in ") B_NSF S(":%d\n"), 
			u32BlockID, u32Size, g_u32TotalHeapBytes, p, szFile, u32Line);
		free((void *) pu32Temp);
		}
	else
		utility_wprintf(S("MEM: Free called with NULL pointer in ") B_NSF S(":%d\n"), szFile, u32Line);

	} /* WcmFree */
#endif

// -------------------------------------------------------------------------
// void PressEnterToContinue(void)
//
// Description: Keeps console window from closing in Visual C++
// -------------------------------------------------------------------------

void PressEnterToContinue(void)
	{

#ifdef ENABLE_CONSOLE_PAUSE
	char c;
	utility_wprintf(S("Press Enter to continue ..."));
#ifdef MS_VISUAL_CPP
	scanf_s("%c", &c);
#else
	scanf("%c", &c);
#endif
#endif

	} /* PressEnterToContinue */

// -------------------------------------------------------------------------
// void DoesFileExist(STRING bszFilePath)
//
// Description: Determines whether or not a file exists and is readable
// -------------------------------------------------------------------------

BOOL DoesFileExist(STRING bszFilePath)
	{

	FILE *f;

	if ((f = BcsFopen(bszFilePath, S("rb"))) == NULL)
		return FALSE;

	fclose(f);

	return TRUE;

	} /* DoesFileExist */

// =========================================================================
// Wide char string functions
// -------------------------------------------------------------------------
// INT32 BcsToInt32(STRING bszString, BOOL *pbError)
//
// Description: Converts a string to a UINT32 value
// -------------------------------------------------------------------------

INT32 BcsToInt32 (const STRING bszString, BOOL *pbError)
	{

	INT32 i32Value;
	B_CHAR *wcpEnd = NULL;

	i32Value = b_strtol(bszString, &wcpEnd, 10);

	if (pbError != NULL && wcpEnd[0] != B_NUL)
		*pbError = TRUE;

	return i32Value;

	} /* BcsToInt32 */

// -------------------------------------------------------------------------
// STRING StrKHzToMHz(STRING bszBuffer, size_t size, const UINT32 u32ValueKHz)
//
// Description: Converts a kHz value to a string representation in MHz. Does
//              not use floating point operations.
// -------------------------------------------------------------------------
STRING StrKHzToMHz(STRING bszBuffer, size_t size, const UINT32 u32ValueKHz)
	{

	StrPrintf(bszBuffer, size, S("%lu.%03lu"), u32ValueKHz / 1000, u32ValueKHz % 1000);

	return bszBuffer;

	} /* StrKHzToMHz */

// -------------------------------------------------------------------------
// BOOL BcsCmpNoCase(STRING bszString1, STRING bszString2, UINT32 n) 
//
// Description: Determines if two strings are the same, ignoring case, upto nth position
// -------------------------------------------------------------------------

BOOL   BcsnCmpNoCase(STRING bszString1, STRING bszString2, UINT32 n)
	{
	
	B_CHAR *wcp1;
	B_CHAR *wcp2;

	if (bszString1 == NULL || bszString2 == NULL || n == 0)
		return FALSE;

	for (wcp1 = bszString1, wcp2 = bszString2;
		*wcp1 != B_NUL && *wcp2 != B_NUL && n > 0; 
		wcp1++, wcp2++, n--)
		{
		if (b_tolower(*wcp1) != b_tolower(*wcp2))
			return FALSE;
		}

	return (*wcp1 == B_NUL && *wcp2 == B_NUL) || n == 0;

	} /* BcsnCmpNoCase */

// -------------------------------------------------------------------------
// BOOL BcsCmpNoCase(STRING bszString1, STRING bszString2)
//
// Description: Determines if two strings are the same, ignoring case 
// -------------------------------------------------------------------------

BOOL   BcsCmpNoCase(STRING bszString1, STRING bszString2)
	{
	
	B_CHAR *wcp1;
	B_CHAR *wcp2;

	if (bszString1 == NULL || bszString2 == NULL)
		return FALSE;

	for (wcp1 = bszString1, wcp2 = bszString2;
		*wcp1 != B_NUL && *wcp2 != B_NUL; 
		wcp1++, wcp2++)
		{
		if (b_tolower(*wcp1) != b_tolower(*wcp2))
			return FALSE;
		}

	return *wcp1 == B_NUL && *wcp2 == B_NUL;

	} /* BcsCmpNoCase */

// ----------------------------------------------------------------------------------
// BOOL BcsStrStrNoCase(STRING bszString, STRING bszSubString)
//
// Description: Determines if bszSubString is a substring of bszString, ignoring case 
// ----------------------------------------------------------------------------------

BOOL BcsStrStrNoCase(STRING bszString, STRING bszSubString)
	{
	
	B_CHAR *wcpString, *wcp1, *wcp2;

	if (bszString == NULL || bszSubString == NULL)
		return FALSE;

	for (wcpString = bszString; *wcpString != B_NUL; wcpString++)
		{
		wcp1 = wcpString;
		for (wcp1 = wcpString, wcp2 = bszSubString; 
			*wcp1 != B_NUL && *wcp2 != B_NUL;
			wcp1++, wcp2++)
			{
			if (b_tolower(*wcp1) != b_tolower(*wcp2))
				break;
			}
		if (*wcp2 == B_NUL)
			return TRUE;
		}

	return FALSE;

	} /* BcsCmpNoCase */

// -------------------------------------------------------------------------
// FILE *BcsFopen(STRING bszFilePath, STRING bszMode);
//
// Description: Opens a file.
// -------------------------------------------------------------------------

FILE *BcsFopen(STRING bszFilePath, STRING bszMode)
	{

	FILE *pf;
#ifdef MS_VISUAL_CPP
	b_fopen(&pf, bszFilePath, bszMode);
#else
	char szFilePath[256];
	char szMode[10];
	b_bcstombs(szFilePath, bszFilePath, sizeof(szFilePath));
	b_bcstombs(szMode, bszMode, sizeof(szMode));
	pf = fopen(szFilePath, szMode);
#endif

	return pf;

	} /* BcsFopen */

// -------------------------------------------------------------------------
// INT32 BcsOpenShare(String bszFilePath, INT32 i32Flags);
//
// Description: Opens a file, allowing other processes to read it.
// -------------------------------------------------------------------------

FILE *BcsFopenShare(STRING bszFilePath, STRING bszMode, BOOL bReadOnly)
	{

#ifdef WIN32
	FILE * pf;
	INT32 i32Access = bReadOnly ? _SH_DENYNO : _SH_DENYWR;
    pf = b_fsopen(bszFilePath, bszMode, i32Access); 
	return pf;
#else
	return BcsFopen(bszFilePath, bszMode);
#endif

	} /* BcsOpenShare */

#ifdef ENABLE_WIDE_CHAR

// -------------------------------------------------------------------------
// BOOL  WcsCreateWargv(STRING wargv[], int iSizeWargv, int argc, char *argv[])
//
// Description: Creates an array of command argument strings, converting from
//              char * to wchar_t * types.
// -------------------------------------------------------------------------

BOOL  WcsCreateWargv(STRING wargv[], int iSizeWargv, int argc, char *argv[])
	{

	int n, iLen;

	if (argc >= iSizeWargv)
		{
		utility_wprintf(S("ERROR: Too many command line parameters (Max = %d)\n"), iSizeWargv);
		return FALSE;
		}

	for (n=0; n<argc; n++)
		{

		iLen = (int) b_mbstobcs(NULL, argv[n], 1024);		

		if (iLen < 0 || iLen >= 1024)
			{
			utility_wprintf(S("ERROR: Invalid command line parameter '") B_NSF S("'\n"), argv[n]);
			return FALSE;
			}
		if ((wargv[n] = WcmMalloc((iLen+1) * sizeof(B_CHAR))) == NULL)
			{
			utility_wprintf(S("ERROR: Memory allocation failed for argv[%d]\n"), n);
			return FALSE;
			}
		b_mbstobcs(wargv[n], argv[n], iLen+1);

		}

	return TRUE;
	}

// -------------------------------------------------------------------------
// BOOL  WcsDestroyWargv(int argc, STRING wargv[])
//
// Description: Cleans up the array created by WcsArgv
// -------------------------------------------------------------------------
BOOL  WcsDestroyWargv(int argc, STRING wargv[])
	{
	int i;

	if (wargv == NULL)
		return FALSE;

	for (i=0; i<argc; i++)
		WcmFree(wargv[i]);

	return TRUE;

	} /* WcsDestroyWargv */

#endif // #ifdef ENABLE_WIDE_CHAR

// -------------------------------------------------------------------------
// size_t b_mbstobcs(STRING bszDst, const char * wsSrc, size_t n)
//
// Description: Replacement for mbstowcs that works with both char and wchar_t
// -------------------------------------------------------------------------

size_t b_mbstobcs(STRING bszDst, const char *szSrc, size_t n)
	{

#ifdef MS_VISUAL_CPP
#ifdef ENABLE_WIDE_CHAR
	size_t sizNumConverted = 0;
	if (mbstowcs_s(&sizNumConverted, bszDst, n, szSrc, _TRUNCATE) != 0)
		return BCSERROR;
	else
		return sizNumConverted;
#else
	if (bszDst == NULL)
		return strlen(szSrc);
	else if (strncpy_s(bszDst, n, szSrc, _TRUNCATE) != 0)
		return BCSERROR;
	else
		return strlen(bszDst);
#endif

#else

#ifdef ENABLE_WIDE_CHAR
	return mbstowcs(bszDst, szSrc, n);
#else
	if (bszDst == NULL)
		return strlen(szSrc);
	else
		{
		strncpy(bszDst, szSrc, n);
		if (bszDst[n-1] == B_NUL)
			return strlen(bszDst);
		else
			return BCSERROR;
		}
#endif
	// Should never get here
	return BCSERROR;
#endif
	} /* b_mbstobcs */

// -------------------------------------------------------------------------
// size_t b_bcstombs(char *szDst, const STRING bszSrc, size_t n)
//
// Description: Replacement for wcstombs that works with both char and wchar_t
// -------------------------------------------------------------------------

size_t b_bcstombs(char *szDst, const STRING bszSrc, size_t n)
	{

#ifdef MS_VISUAL_CPP
#ifdef ENABLE_WIDE_CHAR
	size_t sizNumConverted = 0;
	if (wcstombs_s(&sizNumConverted, szDst, n, bszSrc, _TRUNCATE) != 0)
		return BCSERROR;
	else
		return sizNumConverted;
#else
	if (szDst == NULL)
		return strlen(bszSrc);
	else if (strncpy_s(szDst, n, bszSrc, _TRUNCATE) != 0)
		return BCSERROR;
	else
		return strlen(szDst);
#endif

#else

#ifdef ENABLE_WIDE_CHAR
	return wcstombs(szDst, bszSrc, n);
#else
	if (szDst == NULL)
		return strlen(bszSrc);
	else
		{
		strncpy(szDst, bszSrc, n);
		if (szDst[n-1] == B_NUL)
			return strlen(szDst);
		else
			return BCSERROR;
		}
#endif

#endif
	} /* b_bcstombs */

// -------------------------------------------------------------------------
// B_CHAR *b_bcsncpy(STRING *bszDst, const STRING bszSrc, size_t n);
//
// Description: Replacement for wcsncpy that works with both char and wchar_t
// -------------------------------------------------------------------------

B_CHAR *b_bcsncpy(STRING bszDst, const STRING bszSrc, size_t n)
	{

#ifdef MS_VISUAL_CPP
#ifdef ENABLE_WIDE_CHAR
	wcsncpy_s(bszDst, n, bszSrc, _TRUNCATE);
	return bszDst;
#else
	strncpy_s(bszDst, n, bszSrc, _TRUNCATE);
	return bszDst;
#endif

#else

#ifdef ENABLE_WIDE_CHAR
	return wcsncpy(bszDst, bszSrc, n);
#else
	return strncpy(bszDst, bszSrc, n);
#endif

#endif
	} /* b_bcsncpy */



