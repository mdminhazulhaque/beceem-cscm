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
 * Description	:	Baseband modem logger implementation.
 * Author		:
 * Reviewer		:
 *
 */


#include "Logger.h"
#include "BeceemWiMAX.h"

extern PVOID GpWiMAX;

int WiMAXLogger_Init(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	pWiMAXLogger->ucLogType = 0;
	pWiMAXLogger->ucElementSize = 0;
	pWiMAXLogger->ucStatus = 0;

	pWiMAXLogger->uiLogReg = 0;
	pWiMAXLogger->uiLogLenReg = 0;
	pWiMAXLogger->uiLogEnableReg = 0;

	pWiMAXLogger->iLogIndex = -1;
	pWiMAXLogger->iLogLen = 0;

	pWiMAXLogger->pucLogData = NULL;

	return 0;
}

int WiMAXLogger_Cleanup(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(pWiMAXLogger->pucLogData)
		free(pWiMAXLogger->pucLogData);

	return 0;
}

int WiMAXLogger_InitParam(PVOID pVoid, int iLogType)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(pWiMAXLogger->ucStatus)
		return ERROR_LOGGER_ENABLED; /*already enabled*/

	pWiMAXLogger->ucLogType =(UCHAR) iLogType;
	pWiMAXLogger->ucElementSize = 0;
	pWiMAXLogger->ucStatus = 0;

	pWiMAXLogger->uiLogReg = 0;
	pWiMAXLogger->uiLogLenReg = 0;
	pWiMAXLogger->uiLogEnableReg = 0;

	pWiMAXLogger->iLogIndex = -1;
	pWiMAXLogger->iLogLen = 0;

	pWiMAXLogger->pucLogData = NULL;

	switch(iLogType)
	{
	case SUBTYPE_IR_LOG_REQ	:
		pWiMAXLogger->ucElementSize = sizeof(RANGE_CH_TRACK);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET;
		break;

	case SUBTYPE_BR_LOG_REQ	:
		pWiMAXLogger->ucElementSize = sizeof(RANGE_CH_TRACK);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 12;
		break;

	case SUBTYPE_PR_LOG_REQ	:
		pWiMAXLogger->ucElementSize = sizeof(RANGE_CH_TRACK);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 24;
		break;

	case SUBTYPE_BWL_LOG_REQ :
		pWiMAXLogger->ucElementSize = sizeof(BW_LOG);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 36;
		break;

	case SUBTYPE_CL_LOG_REQ	:
		pWiMAXLogger->ucElementSize = sizeof(POWER_ADJ_TRACK);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 48;
		break;

	case SUBTYPE_OL_LOG_REQ	:
		pWiMAXLogger->ucElementSize = sizeof(POWER_ADJ_TRACK);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 60;
		break;

	case SUBTYPE_CID_LOG_REQ :
		pWiMAXLogger->ucElementSize = sizeof(CID_LOG);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 72;
		break;

	case SUBTYPE_CQICH_LOG_REQ :
		pWiMAXLogger->ucElementSize = sizeof(UINT32);
		pWiMAXLogger->uiLogReg = EXTMEM_BASE_ADDR + MAC_HOST_IF_OFFSET + 84;
		break;

	default:
		return ERROR_INVALID_LOGGER_TYPE;
	}

	pWiMAXLogger->uiLogLenReg = pWiMAXLogger->uiLogReg + 4;
	pWiMAXLogger->uiLogEnableReg = pWiMAXLogger->uiLogReg + 8;
	pWiMAXLogger->pucLogData = (UCHAR *)malloc(5000*pWiMAXLogger->ucElementSize);

	if(!pWiMAXLogger->pucLogData)
		return ERROR_MALLOC_FAILED;

	return WiMAXLogger_EnableLog(pWiMAXLogger);
}

int WiMAXLogger_EnableLog(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(pWiMAXLogger->ucStatus)
		return ERROR_LOGGER_ENABLED;

	pWiMAXLogger->ucStatus = 1;

	BeceemWiMAX_wrm(GpWiMAX, pWiMAXLogger->uiLogEnableReg, 1);

	return 0;
}

int WiMAXLogger_DisableLog(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(!pWiMAXLogger->ucStatus)
		return ERROR_LOGGER_ENABLED;

	pWiMAXLogger->ucStatus = 0;
	BeceemWiMAX_wrm(GpWiMAX, pWiMAXLogger->uiLogEnableReg, 0);

	return 0;
}
BOOL WiMAXLogger_GetLoggerStatus(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return FALSE;

	return (pWiMAXLogger->ucStatus == 0 ?  FALSE : TRUE);
}
UCHAR WiMAXLogger_GetLogType(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return (UCHAR)ERROR_INVALID_ARGUMENT;

	return pWiMAXLogger->ucLogType;
}
UINT WiMAXLogger_GetLogElementSize(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	
	if(!pWiMAXLogger)
		return (UINT)ERROR_INVALID_ARGUMENT;

	return pWiMAXLogger->ucElementSize;
}
int WiMAXLogger_ReadLoggerData(PVOID pVoid)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	UINT uiLogLen = 0;
	UINT uiLogAddress = 0;
	UINT uiLogIndex = 0;

	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(!GpWiMAX || !pWiMAXLogger->pucLogData)
		return ERROR_INVALID_BUFFER;

	/*Read pointer where the log contents are present */
	BeceemWiMAX_rdm(GpWiMAX, pWiMAXLogger->uiLogReg, &uiLogAddress, sizeof(UINT32));
	
	/*
		At this 'uiLogAddress' there will be first a UINT32 
		which shows the current index of that log
	*/
	BeceemWiMAX_rdm(GpWiMAX, uiLogAddress, &uiLogIndex, sizeof(UINT32));
	if(uiLogIndex > 1000)
		return ERROR_INVALID_LOGGER_INDEX;

	pWiMAXLogger->iLogIndex = uiLogIndex;

	/*Read the Max numeber of logs as log Length */
	BeceemWiMAX_rdm(GpWiMAX, pWiMAXLogger->uiLogLenReg, &uiLogLen, sizeof(UINT32));
	if(uiLogLen > 5000)
		return ERROR_INVALID_LOGGER_INDEX;

	pWiMAXLogger->iLogLen = uiLogLen;

	/*Then read logger data */
	BeceemWiMAX_rdmBulk(GpWiMAX, uiLogAddress+4, pWiMAXLogger->pucLogData, pWiMAXLogger->iLogLen * pWiMAXLogger->ucElementSize);

	return SUCCESS;
}

int WiMAXLogger_GetLoggerData(PVOID pVoid, int *iPrevLoggerIndex, UCHAR *pucLoggerData, int iLoggerDataLen)
{
	PWIMAXLOGGER pWiMAXLogger = (PWIMAXLOGGER) pVoid;
	int j = 0;
	int i = 0;
	int iRet;

	if(!pWiMAXLogger)
		return ERROR_INVALID_ARGUMENT;

	if(!pucLoggerData || iLoggerDataLen <= 0)
		return ERROR_INVALID_BUFFER;
	
	iRet = WiMAXLogger_ReadLoggerData(pWiMAXLogger);
	if(iRet)
		return iRet;

	if(*iPrevLoggerIndex < 0 || pWiMAXLogger->iLogIndex < 0)
	{
		*iPrevLoggerIndex = pWiMAXLogger->iLogIndex;
		return ERROR_INVALID_LOGGER_INDEX;
	}
	memset(pucLoggerData, 0, iLoggerDataLen);

	if(*iPrevLoggerIndex <= pWiMAXLogger->iLogIndex)
	{
		for(i=*iPrevLoggerIndex; i<pWiMAXLogger->iLogIndex && i<pWiMAXLogger->iLogLen; i++, j++)
		{
			if((pWiMAXLogger->ucElementSize * j) > iLoggerDataLen)
			{
				j--;
				break;
			}
			memcpy(pucLoggerData+(pWiMAXLogger->ucElementSize*j), pWiMAXLogger->pucLogData+(pWiMAXLogger->ucElementSize*i), pWiMAXLogger->ucElementSize);
		}
	}
	else
	{
		for(i=*iPrevLoggerIndex; i<pWiMAXLogger->iLogLen; i++, j++)
		{
			if((pWiMAXLogger->ucElementSize * j) > iLoggerDataLen)
			{
				j--;
				break;
			}
			memcpy(pucLoggerData+(pWiMAXLogger->ucElementSize*j), pWiMAXLogger->pucLogData+(pWiMAXLogger->ucElementSize*i), pWiMAXLogger->ucElementSize);
		}
		for(i=0; i<pWiMAXLogger->iLogIndex; i++, j++)
		{
			if((pWiMAXLogger->ucElementSize * j) > iLoggerDataLen)
			{
				j--;
				break;
			}
			memcpy(pucLoggerData+(pWiMAXLogger->ucElementSize*j), pWiMAXLogger->pucLogData+(pWiMAXLogger->ucElementSize*i), pWiMAXLogger->ucElementSize);
		}
	}
	*iPrevLoggerIndex = pWiMAXLogger->iLogIndex;
	return j;
}
