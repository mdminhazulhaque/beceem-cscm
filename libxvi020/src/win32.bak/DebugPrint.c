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
 * Description	:	Debug print routines for Win32.
 * Author		:
 * Reviewer		:
 *
 */


#include <time.h>
#include "DebugPrint.h"
#include "TCHAR.H"
#include "BeceemWiMAX.h"
int DebugPrint_Init(PVOID pVoid)
{
	PDEBUGPRINT pDebugPrint;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pDebugPrint = (PDEBUGPRINT) pVoid;
		
	DebugPrint_Initialize(pDebugPrint);
	
	return 0;
}
BOOL DebugPrint_Initialize(PVOID pVoid)
{
	PDEBUGPRINT pDebugPrint;

	if(!pVoid)
		return FALSE;

	pDebugPrint = (PDEBUGPRINT) pVoid;

	memset(pDebugPrint->szFilePath, 0, sizeof(pDebugPrint->szFilePath));

    pDebugPrint->pFptr = NULL;

	return TRUE;
}




int DebugPrint_Cleanup(PVOID pVoid)
{
	PDEBUGPRINT pDebugPrint;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pDebugPrint = (PDEBUGPRINT) pVoid;

	if(pDebugPrint->pFptr!= NULL)
	{
        fclose(pDebugPrint->pFptr);
        pDebugPrint->pFptr = NULL;
	}
	return 0;
}


BOOL ConvertToUnicode(const char* pASCIIString,PUSHORT pUNICODEString, UINT *pUNICODEStringLength)
{
	UINT uiASCIILength = (UINT)strlen(pASCIIString) + 2;
	INT iRet = 0;

	memset(pUNICODEString,0,uiASCIILength*2);

	iRet = (INT)mbstowcs(pUNICODEString,pASCIIString,uiASCIILength);
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
	int nLen = (int)wcslen(pUNICODEString)+1;
	INT iRet = 0;
	memset(pASCIIString,0,nLen);
	iRet =(INT)wcstombs(pASCIIString, pUNICODEString, nLen);
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
	return (UINT)wcslen(pUNICODEString);
}
void DebugPrint_syslog(PCHAR pBufferToPrint)
{
    OutputDebugString(pBufferToPrint);
}

BOOL SetFile(PVOID pArg, PCHAR szFilePath)
{
	PDEBUGPRINT pDPrint = (PDEBUGPRINT)pArg;
	
	if(!pDPrint)
		return FALSE;

	if(pDPrint->pFptr)
    {
        fclose(pDPrint->pFptr);
        pDPrint->pFptr = NULL;
    }
	memset(pDPrint->szFilePath, 0, sizeof(pDPrint->szFilePath));

	if(szFilePath)
	{
		errno_t err;

		strcpy(pDPrint->szFilePath, szFilePath);

		//pDPrint->pFptr = fopen(pDPrint->szFilePath,"a");
		err = fopen_s(&pDPrint->pFptr,pDPrint->szFilePath,"a");
        if(!pDPrint->pFptr)
            {
                DPRINT_OTHER ("Failed to create / open file %s error code \n", pDPrint->szFilePath);
            }
     }
    else
        return FALSE;
  
	return TRUE;
}


