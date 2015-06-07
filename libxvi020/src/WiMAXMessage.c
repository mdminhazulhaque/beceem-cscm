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
 * Description	:	Utility functions for message handling.
 * Author		:
 * Reviewer		:
 *
 */


#include "WiMAXMessage.h"

#include "ProxyEndianHandler.h"

int WiMAXMessage_Init(			PVOID pVoid,
								USHORT usType, 
								USHORT usSubType, 
								UCHAR *pucData, 
								UINT uiDataLenth
							)
{

	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return ERROR_INVALID_ARGUMENT;

	WiMAXMessage_Clear(pVoid);
	pWiMAXMessage->stMessage.usType = usType;
	pWiMAXMessage->stMessage.usSubType = usSubType;
	WiMAXMessage_SetData(pVoid, pucData, uiDataLenth);

	return 0;
}

int WiMAXMessage_Cleanup(PVOID pVoid)
{
	
	UNUSED_ARGUMENT(pVoid);

	return 0;
}

void WiMAXMessage_Clear(PVOID pVoid)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return;

	memset(&pWiMAXMessage->stMessage, 0, SIZE_WIMAX_MESSAGE);
	pWiMAXMessage->uiHeaderLen = SIZE_WIMAX_MESSAGE - MAX_VARIABLE_LENGTH;
	pWiMAXMessage->uiMessageLen = pWiMAXMessage->uiHeaderLen;

}

void WiMAXMessage_SetType(PVOID pVoid, USHORT usType)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return;

	pWiMAXMessage->stMessage.usType = usType;
	return;
}

void WiMAXMessage_SetSubtype(PVOID pVoid, USHORT usSubtype)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return;

	pWiMAXMessage->stMessage.usSubType = usSubtype;
	return;
}

void WiMAXMessage_SetData(PVOID pVoid, UCHAR *pucData, UINT uiDataLenth)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return ;

	if(!pucData || uiDataLenth <= 0 || uiDataLenth > MAX_VARIABLE_LENGTH)
		return;

	memcpy(pWiMAXMessage->stMessage.szData, pucData, uiDataLenth);

	CorrectEndiannessForProxy(&pWiMAXMessage->stMessage,uiDataLenth,1);
	
	pWiMAXMessage->uiMessageLen = uiDataLenth + pWiMAXMessage->uiHeaderLen;
}

void WiMAXMessage_AppendData(PVOID pVoid, UCHAR *pucData, UINT uiDataLenth)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return;

	if(!pucData || uiDataLenth <= 0 || uiDataLenth+(pWiMAXMessage->uiMessageLen-pWiMAXMessage->uiHeaderLen)  > MAX_VARIABLE_LENGTH)
		return;

	memcpy(pWiMAXMessage->stMessage.szData + (pWiMAXMessage->uiMessageLen - pWiMAXMessage->uiHeaderLen), pucData, uiDataLenth);
	pWiMAXMessage->uiMessageLen += uiDataLenth;

	return;
}

const UCHAR *WiMAXMessage_GetData(PVOID pVoid)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return NULL;

	return pWiMAXMessage->stMessage.szData;
}
UINT WiMAXMessage_GetDataLength(PVOID pVoid)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return (UINT)ERROR_INVALID_ARGUMENT;

	return pWiMAXMessage->uiMessageLen - pWiMAXMessage->uiHeaderLen;
}

PWIMAX_MESSAGE WiMAXMessage_GetMessage(PVOID pVoid)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return NULL;

	return &pWiMAXMessage->stMessage;
}

UINT WiMAXMessage_GetMessageLength(PVOID pVoid)
{
	PWIMAXMESSAGE pWiMAXMessage = (PWIMAXMESSAGE) pVoid;

	if(!pWiMAXMessage)
		return (UINT)ERROR_INVALID_ARGUMENT;

	return pWiMAXMessage->uiMessageLen;
}
