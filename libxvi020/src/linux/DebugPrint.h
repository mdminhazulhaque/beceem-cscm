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
 * Description	:	Debug print header for linux.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_DEBUG_PRINT
#define BECEEM_DEBUG_PRINT


#include "Typedefs.h"
#include "BeceemWiMAX.h"
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#define DEBUG_MESSAGE	1
#define DEBUG_ERROR	2
#define DEBUG_WARNING	3


typedef  unsigned short widechar_t;

#define INVALID_FILE_DESCRIPTOR -1

typedef struct _DEBUGPRINT
{
	int	m_iFile;
	char	m_szFilePath[100];

}DEBUGPRINT, *PDEBUGPRINT;



void DebugPrint_Init(PVOID pArg);
void DebugPrint_Cleanup(PVOID pArg);
BOOL DebugPrint_Initialize(PVOID pArg );
BOOL SetFile(PVOID pArg, char * szFilePath);
BOOL ConvertToUnicode(const char* pASCIIString,PUSHORT pUNICODEString, UINT *pUNICODEStringLength);
BOOL ConvertToASCII(const PUSHORT pUNICODEString, char* pASCIIString,UINT *pASCIIStringLength);
UINT GetUnicodeStringLength(PUSHORT pUNICODEString);
BOOL GetShmemLogRec(void);
void DebugPrint_syslog(PCHAR pBufferToPrint);

#endif /* BECEEM_DEBUG_PRINT */

