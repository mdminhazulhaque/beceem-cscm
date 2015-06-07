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
 * Description	:	Debug print routines API for Win32.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_DEBUG_PRINT
#define BECEEM_DEBUG_PRINT

#pragma once

#include <windows.h>
#include <stdio.h>
#include "ErrorDefs.h"
#include "Typedefs.h"

#define DEBUG_MESSAGE	1
#define DEBUG_ERROR		2
#define DEBUG_WARNING	3


typedef struct _DEBUGPRINT
{
	FILE *pFptr;
	char	szFilePath[100];
}DEBUGPRINT, *PDEBUGPRINT;

int DebugPrint_Init(PVOID pVoid);
int DebugPrint_Cleanup(PVOID pVoid);
BOOL DebugPrint_InitializeData(PVOID pVoid);
VOID DebugPrint_AddLogHeader(PVOID pVoid);
VOID DebugPrint_AddLogFooter(PVOID pVoid);
BOOL DebugPrint_SetFile(PVOID pVoid, const char *szFilePath);
BOOL DebugPrint_Initialize(PVOID pVoid);
BOOL ConvertToUnicode(const char* pASCIIString,PUSHORT pUNICODEString, UINT *pUNICODEStringLength);
BOOL ConvertToASCII(const PUSHORT pUNICODEString, char* pASCIIString,UINT *pASCIIStringLength);
UINT GetUnicodeStringLength(PUSHORT pUNICODEString);
void DebugPrint_syslog(PCHAR pBufferToPrint);
BOOL SetFile(PVOID pArg, PCHAR szFilePath);
#endif /* BECEEM_DEBUG_PRINT */
