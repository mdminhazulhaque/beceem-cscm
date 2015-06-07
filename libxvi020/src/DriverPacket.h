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
 * Description	:	This includes declarations for CDriverPacket class
 * Author		:
 * Reviewer		:
 *
 */


#ifndef DRIVER_PACKET_H
#define DRIVER_PACKET_H

#include <string.h>
#include "ErrorDefs.h"
#include "Typedefs.h"

#define Pad4(usLen) (USHORT)(((usLen+3)/4)*4)


#pragma pack (push,1)

struct _LEADER_ST
{
	USHORT usVcid;
	USHORT usPayloadLength;
	UCHAR  ucStatus;
	UCHAR  ucUnused[3];
};
typedef struct _LEADER_ST LEADER_ST,*PLEADER_ST;

struct _PACKETTOSEND_ST
{
	LEADER_ST Leader;
	UCHAR  aucData[MAX_BUFFER_LENGTH];
};
typedef struct _PACKETTOSEND_ST PACKETTOSEND_ST,*PPACKETTOSEND_ST;

typedef struct _DRIVERPACKET
{
	PACKETTOSEND_ST m_stPacket;
	int	m_iPayloadLen;
}DRIVERPACKET, *PDRIVERPACKET;
#pragma pack(pop)
/*const int SIZE_DRIVERPACKET = sizeof(DRIVERPACKET);*/

void DriverPacket_Init(PVOID pArg);
void DriverPacket_InitWithVal(PVOID pArg, USHORT usVcid, UCHAR ucStatus);


void DriverPacket_Clear(PVOID pArg);
void DriverPacket_SetVCID(PVOID pArg, USHORT usVcid);
void DriverPacket_SetStatus(PVOID pArg, UCHAR ucStatus);
void DriverPacket_SetData(PVOID pArg, UCHAR *pucData, UINT uiDataLenth);
void DriverPacket_AppendData(PVOID pArg, UCHAR *pucData, UINT uiDataLenth);
void DriverPacket_AppendChar(PVOID pArg, UCHAR ucData);

PPACKETTOSEND_ST DriverPacket_GetPacketToSend(PVOID pArg);
int DriverPacket_GetPacketToSendLength(PVOID pArg);
PLEADER_ST DriverPacket_GetLeaderData(PVOID pArg);

const UCHAR *DriverPacket_GetPacketData(PVOID pArg);
UINT DriverPacket_GetPacketDataLength(PVOID pArg);


#endif /* DRIVER_PACKET_H */
