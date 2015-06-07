/*
 * (C) Copyright 2010. Beceem Communications Inc. All rights reserved.
 * 
 * Disclaimer :
 *
 * EXCEPT AS SET FORTH IN BECEEM'S TERMS AND CONDITIONS OF SALE, BECEEM
 * ASSUMES NO LIABILITY WHATSOEVER AND DISCLAIMS ANY EXPRESS, IMPLIED OR
 * STATUTORY WARRANTY RELATING TO ITS PRODUCTS INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL BECEEM BE LIABLE FOR
 * ANY DIRECT, INDIRECT, CONSEQUENTIAL, PUNITIVE, SPECIAL OR INCIDENTAL
 * DAMAGES (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
 * BUSINESS INTERRUPTION OR LOSS OF INFORMATION) ARISING OUT OF THE USE
 * OR INABILITY TO USE THIS DOCUMENT, EVEN IF BECEEM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES. 
 * 
 * Trademarks :
 *
 * Beceem, Beceem Communications, and Mobilizing Broadband are registered
 * trademarks of Beceem Communications Inc.  The Beceem logo is a
 * trademark of Beceem Communications Inc.
 *
 * Description	:	Debug print implementation for linux.
 * Author		:
 * Reviewer		:
 *
 */


#include "DebugPrint.h"
#include "SharedMemory.h"
#include "BeceemWiMAX.h"
#include<stddef.h>
#include<stdlib.h>
#include <time.h>

#include "globalobjects.h"
/* WCHAR Functions implementation */

 static size_t beceem_wcslen(const widechar_t *s)
 {
 	const widechar_t *s0 = s + 1;
 	while (*s++)
 		;
 	return (s - s0);
 }

static size_t beceem_wcstombs(char *s, const widechar_t *pwcs, size_t n)
{
        int count = 0;

        if (n != 0) {
                do {
                        if ((*s++ = (char) *pwcs++) == 0)
                                break;
                        count++;
                } while (--n != 0);
        }

        return count;
}

static size_t beceem_mbstowcs(widechar_t *pwcs, const char *s, size_t n)
{
        int count = 0;

        if (n != 0) {
                do {
                        if ((*pwcs++ = (widechar_t) *s++) == 0)
                                break;
                        count++;
                } while (--n != 0);
        }

        return count;
}



BOOL SetFile(PVOID pArg, char * szFilePath)
{
	PDEBUGPRINT pDPrint = (PDEBUGPRINT)pArg;
	if(!pDPrint)
		return FALSE;

	if(pDPrint->m_iFile != INVALID_FILE_DESCRIPTOR)
	{
		fflush (fdopen (pDPrint->m_iFile, "a+"));
		close(pDPrint->m_iFile);
		pDPrint->m_iFile = INVALID_FILE_DESCRIPTOR;
	}
	memset(pDPrint->m_szFilePath, 0, sizeof(pDPrint->m_szFilePath));

	if(szFilePath)
	{
		strcpy(pDPrint->m_szFilePath, szFilePath);
		pDPrint->m_iFile = open(pDPrint->m_szFilePath, O_CREAT|O_NONBLOCK|O_RDWR|O_APPEND, 0666);
		if (pDPrint->m_iFile == -1) {
			DPRINT_OTHER ("Failed to create / open file %s\n", pDPrint->m_szFilePath);
			return FALSE;
		}
	}
	else
		return FALSE;

    openlog (SYSLOG_PREFIX_STR, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
	return TRUE;
}


void DebugPrint_Init(PVOID pArg)
{
	PDEBUGPRINT pDPrint = (PDEBUGPRINT)pArg;
	if(!pDPrint)
		return;
	DebugPrint_Initialize(pDPrint);
}

BOOL DebugPrint_Initialize(PVOID pArg)
{
	PDEBUGPRINT pDPrint = (PDEBUGPRINT)pArg;
	if(!pDPrint)
		return FALSE;

	memset(pDPrint->m_szFilePath, 0, sizeof(pDPrint->m_szFilePath));
	pDPrint->m_iFile = INVALID_FILE_DESCRIPTOR;
	return TRUE;
}

void DebugPrint_Cleanup(PVOID pArg)
{
	PDEBUGPRINT pDPrint = (PDEBUGPRINT)pArg;
	if(!pDPrint)
		return;


	if(pDPrint->m_iFile != INVALID_FILE_DESCRIPTOR) 
        {

		close(pDPrint->m_iFile);
		pDPrint->m_iFile = INVALID_FILE_DESCRIPTOR;
	}

		closelog ();
}


BOOL ConvertToUnicode(const char* pASCIIString,PUSHORT pUNICODEString, UINT *pUNICODEStringLength)
{
	UINT uiASCIILength = (UINT)strlen(pASCIIString) + 2;
	INT iRet = 0;

	memset(pUNICODEString,0,uiASCIILength*2);

	iRet = (INT)beceem_mbstowcs((widechar_t*)pUNICODEString,pASCIIString,uiASCIILength);
	if(iRet != -1)
		{
			*pUNICODEStringLength = iRet;
			return TRUE;
		}
	*pUNICODEStringLength = 0;
	return FALSE;
}
BOOL ConvertToASCII(const PUSHORT pUNICODEString, char* pASCIIString,UINT *pASCIIStringLength)
{
	int nLen = (int)beceem_wcslen((widechar_t*)pUNICODEString)+1;
	INT iRet = 0;
	memset(pASCIIString,0,nLen);
	iRet =(INT)beceem_wcstombs(pASCIIString,(widechar_t*) pUNICODEString, nLen);
	if(iRet != -1)
		{
			*pASCIIStringLength = iRet;
			return TRUE;
		}
	*pASCIIStringLength = 0;
	return FALSE;
}
UINT GetUnicodeStringLength(PUSHORT pUNICODEString)
{
	return (UINT)beceem_wcslen((widechar_t*)pUNICODEString);
}
void DebugPrint_syslog(PCHAR pBufferToPrint)
{

    syslog (SYSLOG_LEVEL, "%s", pBufferToPrint);

}
