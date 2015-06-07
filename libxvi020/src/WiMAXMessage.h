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
 * Description	:	This includes CWiMAXMessage class, which implements
 *					WiMAX Message creation.
 * Author		:	Prashanth Gedde N
 * Reviewer		:
 *
 */


#ifndef WIMAXMESSAGE_H
#define WIMAXMESSAGE_H

#include <string.h>

#include "PrivateTypedefs.h"
#include "Typedefs.h"
#include "ErrorDefs.h"

typedef struct _WIMAXMESSAGE
{
    UINT uiMessageLen;
	UINT uiHeaderLen;
	WIMAX_MESSAGE stMessage;
	
}WIMAXMESSAGE, *PWIMAXMESSAGE;

int WiMAXMessage_Init(	PVOID pVoid,
						USHORT usType, 
						USHORT usSubType, 
						UCHAR *pucData, 
						UINT uiDataLenth
				 );
int WiMAXMessage_Cleanup(PVOID pVoid);

void WiMAXMessage_Clear(PVOID pVoid);
void WiMAXMessage_SetType(PVOID pVoid, USHORT usType);
void WiMAXMessage_SetSubtype(PVOID pVoid, USHORT usSubtype);
void WiMAXMessage_SetData(PVOID pVoid, UCHAR *pucData, UINT uiDataLenth);
void WiMAXMessage_AppendData(PVOID pVoid, UCHAR *pucData, UINT uiDataLenth);
const UCHAR *WiMAXMessage_GetData(PVOID pVoid);
UINT WiMAXMessage_GetDataLength(PVOID pVoid);
PWIMAX_MESSAGE WiMAXMessage_GetMessage(PVOID pVoid);
UINT WiMAXMessage_GetMessageLength(PVOID pVoid);


#endif /* WIMAXMESSAGE_H */
