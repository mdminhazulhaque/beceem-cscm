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
 * Description	:	This includes definitions or CDriverPacket class and
 *					member functions
 * Author		:
 * Reviewer		:
 *
 */


#include "DriverPacket.h"

void DriverPacket_Init(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	memset(&pDrvPkt->m_stPacket, 0, sizeof(pDrvPkt->m_stPacket));
	pDrvPkt->m_iPayloadLen = 0;
}



void DriverPacket_InitWithVal(PVOID pArg, USHORT usVcid, UCHAR ucStatus)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	memset(&pDrvPkt->m_stPacket, 0, sizeof(pDrvPkt->m_stPacket));
	pDrvPkt->m_stPacket.Leader.usVcid = usVcid;
	pDrvPkt->m_stPacket.Leader.ucStatus = ucStatus;
	pDrvPkt->m_iPayloadLen = 0;
}

void DriverPacket_Clear(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	memset(&pDrvPkt->m_stPacket, 0, sizeof(pDrvPkt->m_stPacket));
	pDrvPkt->m_iPayloadLen = 0;
}

void DriverPacket_SetVCID(PVOID pArg, USHORT usVcid)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	pDrvPkt->m_stPacket.Leader.usVcid = usVcid;
}

void DriverPacket_SetStatus(PVOID pArg, UCHAR ucStatus)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	pDrvPkt->m_stPacket.Leader.ucStatus = ucStatus;
}

void DriverPacket_SetData(PVOID pArg, UCHAR *pucData, UINT uiDataLenth)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	pDrvPkt->m_iPayloadLen = uiDataLenth;

	if(pDrvPkt->m_iPayloadLen > 0 && pDrvPkt->m_iPayloadLen < MAX_BUFFER_LENGTH)
		memcpy(pDrvPkt->m_stPacket.aucData, pucData, uiDataLenth);
	else
		pDrvPkt->m_iPayloadLen = 0;

	pDrvPkt->m_stPacket.Leader.usPayloadLength = Pad4(pDrvPkt->m_iPayloadLen);
}

void DriverPacket_AppendData(PVOID pArg, UCHAR *pucData, UINT uiDataLenth)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	if(pDrvPkt->m_iPayloadLen >= 0 && (pDrvPkt->m_iPayloadLen + uiDataLenth) < MAX_BUFFER_LENGTH)
	{
		memcpy(pDrvPkt->m_stPacket.aucData+pDrvPkt->m_iPayloadLen, pucData, uiDataLenth);
		pDrvPkt->m_iPayloadLen += uiDataLenth;
		pDrvPkt->m_stPacket.Leader.usPayloadLength = Pad4(pDrvPkt->m_iPayloadLen);
	}
}

void DriverPacket_AppendChar(PVOID pArg, UCHAR ucData)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return;

	DriverPacket_AppendData(pDrvPkt, &ucData, 1);
}

PPACKETTOSEND_ST DriverPacket_GetPacketToSend(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return NULL;

	return (PPACKETTOSEND_ST)&pDrvPkt->m_stPacket;
}

int DriverPacket_GetPacketToSendLength(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return ERROR_INVALID_ARGUMENT;

	return pDrvPkt->m_stPacket.Leader.usPayloadLength+sizeof(LEADER_ST);
}

PLEADER_ST DriverPacket_GetLeaderData(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return NULL;

	return (PLEADER_ST)&pDrvPkt->m_stPacket;
}

const UCHAR *DriverPacket_GetPacketData(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return NULL;

	return pDrvPkt->m_stPacket.aucData;
}

UINT DriverPacket_GetPacketDataLength(PVOID pArg)
{
	PDRIVERPACKET pDrvPkt = (PDRIVERPACKET)pArg;
	if(!pDrvPkt)
		return (UINT)ERROR_INVALID_ARGUMENT;

	return pDrvPkt->m_stPacket.Leader.usPayloadLength;
}
