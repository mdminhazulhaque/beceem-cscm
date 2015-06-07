/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// Define this to do a quick standalone test
// #define TEST_STRING _FUNCTIONS

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "BeceemAPI.h"
#include "Utility.h"

#define MAX_BLOCK_SIZE  (MAX_STR_RESULT_SIZE * sizeof(B_CHAR))

static STRING StringGetBuffer (BeceemAPI_t hAPI, UINT32 u32BcharCount)
	{
	STRING  bszBuffer;

	if (!hAPI)
		return NULL;

	if (u32BcharCount > MAX_STR_RESULT_SIZE)
		return NULL;

	if (hAPI->iStringNxtBufBchar + u32BcharCount >= MAX_STR_RESULT_SIZE)
		hAPI->iStringNxtBufBchar = 0;

	bszBuffer = &hAPI->bszBufferPool[hAPI->iStringNxtBufBchar];
	hAPI->iStringNxtBufBchar = hAPI->iStringNxtBufBchar + u32BcharCount;

	return bszBuffer;

	} /* StringGetBuffer */


STRING StringRepChar(BeceemAPI_t hAPI, B_CHAR wc, UINT32 u32Length)
	{

	STRING bszBuffer;
	
	if (!hAPI || u32Length == 0)
		return NULL;

	bszBuffer = StringGetBuffer(hAPI, u32Length + 1);
	b_memset(bszBuffer, wc, u32Length);
	bszBuffer[u32Length] = 0;

	return bszBuffer;

	} /* StringRepChar */

// -------------------------------------------------------------------------
// STRING StringPadLeft(const char *s, int iSize, char c)
//
// Description: Pads a string on the left to a specified length with a 
//       specified character.
//
// Note: Results are stored in a pool of static buffers. Results are
//       overwritten when the buffer pool is reused.
// -------------------------------------------------------------------------

STRING StringPadLeft(BeceemAPI_t hAPI, const STRING ws, int iSize, B_CHAR wc)
	{
	STRING bszBuffer;
	int iLen = StringLen(ws);
	
	if (!hAPI || iLen < 0)
		return NULL;

	if (iLen >= iSize || iSize >= MAX_STR_RESULT_SIZE)
		return ws;

	else
		{
		bszBuffer = StringGetBuffer(hAPI, iSize + 1);
		b_memset(bszBuffer, wc, iSize-iLen);
		b_memcpy(bszBuffer + iSize-iLen, ws, iLen);
		bszBuffer[iSize] = 0;

		return bszBuffer;
		}

	} /* StringPadLeft */

// -------------------------------------------------------------------------
// iSubstrLen <= means copy to end of string
// -------------------------------------------------------------------------

STRING StringSubString(BeceemAPI_t hAPI, const STRING ws, int iStart, int iSubstrLen)
	{
	STRING bszBuffer;
	int iLen = StringLen(ws);
	
	if (!hAPI || iLen < 0)
		return NULL;

	if (iStart < 0 || iStart >= iLen || iSubstrLen == 0)
		return S("");
	
	if (iSubstrLen < 0 || iStart + iSubstrLen >= iLen)
		iSubstrLen = iLen - iStart + 1;

	if (iSubstrLen >= MAX_STR_RESULT_SIZE)
		iSubstrLen = MAX_STR_RESULT_SIZE-1;

	bszBuffer = StringGetBuffer(hAPI, iSubstrLen + 1);
	b_memcpy(bszBuffer, ws+iStart, iSubstrLen);
	bszBuffer[iSubstrLen] = 0;

	return bszBuffer;

	} /* StringSubString */

STRING StringTrim(BeceemAPI_t hAPI, const STRING ws)
	{

	int iLen = StringLen(ws);
	int iStartPos, iEndPos, n;
	
	if (!hAPI || iLen < 0)
		return NULL;

	iEndPos   = iLen-1;

	// Strip leading spaces
	iStartPos = iLen;
	for (n = 0; n < iLen; n++)
		if (ws[n] != C(' '))
			{
			iStartPos = n;
			break;
			}

	if (iStartPos == iLen)
		return EMPTY_STRING;

	// Strip trailing spaces
	for (n=iLen-1; n>=0; n--)
		if (ws[n] != C(' '))
			{
			iEndPos = n;
			break;
			}

	return StringSubString(hAPI, ws, iStartPos, iEndPos-iStartPos+1);

	} /* StringTrim */

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

STRING StringToScaledFixedPoint (BeceemAPI_t hAPI, const STRING ws, int iRightOfDecimal) 
	{

	STRING bszBuffer;
	int iLen = StringLen(ws);
	int iBufSize;
	
	if (!hAPI || iLen < 0)
		return NULL;

	if (iLen <= iRightOfDecimal)
		{
		iBufSize = iRightOfDecimal + 3;
		bszBuffer = StringGetBuffer(hAPI, iBufSize);
		return StrPrintf(bszBuffer, iBufSize, 
			S("0.") B_SF,  StringPadLeft(hAPI, ws, iRightOfDecimal, C('0')));
		}
	else 
		{
		int iLeftOfDecimal = iLen - iRightOfDecimal;
		iBufSize = iRightOfDecimal + 3;
		bszBuffer = StringGetBuffer(hAPI, iBufSize);
		return StrPrintf(bszBuffer, iBufSize, 
			B_SF S(".") B_SF, StringSubString(hAPI, ws, 0, iLeftOfDecimal), 
			StringSubString(hAPI, ws, iLeftOfDecimal, STRING_SUBSTR_TO_END));
		}

	} /* *StringToScaledFixedPoint */

// -------------------------------------------------------------------------
//
// void StringHexDump(BeceemAPI_t hAPI, BOOL bPrintAddress,
//					  UINT8 *au8Buffer, UINT32 u32Length)
//
// Description: Prints a hex dump to the specified stream
//
// Input parameters:
//		hAPI		    - Beceem API handle
//		au8Buffer		- Buffer to be printed
//	    u32ByteLength	- Number of bytes to be printed
//
// -------------------------------------------------------------------------

void StringHexDump(UINT32 u32DebugLevel, BeceemAPI_t hAPI, BOOL bPrintAddress, 
				   const UINT8 *au8Buffer, UINT32 u32ByteLength)
	{
	UINT32 n;
	for (n=0; n<u32ByteLength; n++)
		{
		if (n%32 == 0 && bPrintAddress)
			PrintDiag(u32DebugLevel, hAPI, S("\n%04X:"), n);
		PrintDiag(u32DebugLevel, hAPI, S(" %02X"), au8Buffer[n]);
		}
	PrintDiag(u32DebugLevel, hAPI, S("\n"));
	}

#ifdef TEST_STRING_FUNCTIONS

// -------------------------------------------------------------------------
// Standalone test function for this module
// -------------------------------------------------------------------------

#define test_wprintf b_printf

int main(void)
	{
	STRING bszTest;
	const STRING bszPadded;
	int iLen, n, iStart;

	test_wprintf(S("Testing string functions\n"));

	test_wprintf(S("\nTesting StringPadLeft\n"));
	bszTest = S("abc");
	bszPadded = StringPadLeft(bszTest, 20, C('0'));
	test_wprintf(S("Padding ") B_SF S(" left to len=20: ") B_SFNL, bszTest, bszPadded);

	test_wprintf(S("\nTesting StringSubString\n"));
	iLen = StringLen(bszPadded);
	for (n=0; n <= iLen+1; n++)
		test_wprintf(S("StringSubString(bszPadded,%d,-1) = \"") B_SF S("\"\n"), 
			n, StringSubString(bszPadded, n, STRING _SUBSTR_TO_END));
	iStart = 15;
	for (n=0; n <= iLen-iStart+4; n++)
		test_wprintf(S("StringSubString(bszPadded,%d,%d) = \"") B_SF S("\"\n"), 
			iStart, n, StringSubString(bszPadded, 15,n));

	test_wprintf(S("\nTesting StringToScaledFixedPoint\n"));
	bszTest = S("123456789");
	for (n=0; n <= StringLen(bszTest) + 3; n++)
		test_wprintf(S("StringToScaledFixedPoint(") B_SF S(", %d) = ") B_SFNL,
			bszTest, n, StringToScaledFixedPoint(bszTest, n));
	PressEnterToContinue();

	}
#endif
