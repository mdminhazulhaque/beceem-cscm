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
 * Description	:	Baseband modem logger API.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_LOGGER
#define BECEEM_LOGGER

/* #pragma once */

#include "ErrorDefs.h"
#include "PrivateTypedefs.h"
#include "Messages.h"

#define EXTMEM_BASE_ADDR	0xBF600000
#define MAC_HOST_IF_OFFSET	0xAC00

typedef struct _WIMAXLOGGER
{
	UCHAR ucLogType;
	UCHAR ucElementSize;
	UCHAR ucStatus;			/* enabled/disabled */
	UCHAR ucUnused;

	UINT uiLogReg;
	UINT uiLogLenReg;
	UINT uiLogEnableReg;

	int iLogIndex;
	int iLogLen;

	UCHAR *pucLogData;
}WIMAXLOGGER, *PWIMAXLOGGER;

int WiMAXLogger_Init(PVOID pVoid);
int WiMAXLogger_Cleanup(PVOID pVoid);
int WiMAXLogger_InitParam(PVOID pVoid, int iLogType);
int WiMAXLogger_EnableLog(PVOID pVoid);
int WiMAXLogger_DisableLog(PVOID pVoid);
BOOL WiMAXLogger_GetLoggerStatus(PVOID pVoid);
UCHAR WiMAXLogger_GetLogType(PVOID pVoid);
UINT WiMAXLogger_GetLogElementSize(PVOID pVoid);
int WiMAXLogger_ReadLoggerData(PVOID pVoid);
int WiMAXLogger_GetLoggerData(PVOID pVoid, int *iPrevLoggerIndex, UCHAR *pucLoggerData, int iLoggerDataLen);


#endif /* BECEEM_LOGGER */
