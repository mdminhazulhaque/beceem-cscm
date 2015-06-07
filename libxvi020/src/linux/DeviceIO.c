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
 * Description	:	Device IO interface for Linux.
 * Author		:
 * Reviewer		:
 *
 */


#include "DeviceIO.h"
#ifdef LINUX
#include <linux/kdev_t.h>
#endif
#include <stdio.h>
#include "BeceemWiMAX.h"


void DeviceIO_Init(PVOID pArg)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return;

	pDevIO->m_hDevice = INVALID_HANDLE_VALUE; 
	pDevIO->m_pNotifyCallback = NULL;
	pDevIO->m_pNotifyContext = NULL;
}

void DeviceIO_Cleanup(PVOID pArg)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return;

	DeviceIO_CloseHandle(pDevIO);
}

BOOL DeviceIO_IsValidHandle(PVOID pArg)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return FALSE;

	return (pDevIO->m_hDevice != INVALID_HANDLE_VALUE);
}

int DeviceIO_OpenHandle(PVOID pArg, const char *szDeviceName)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

	if(!pDevIO)
		return ERROR_INVALID_ARGUMENT;

	if(!szDeviceName)
		return ERROR_INVALID_DEVICENAME;

	pDevIO->m_hDevice =(HANDLE)(UADDTYPE)open(szDeviceName, O_RDWR, mode);
   
	return DeviceIO_IsValidHandle(pDevIO) ? SUCCESS : ERROR_CREATEFILE_FAILED;
}

int DeviceIO_CloseHandle(PVOID pArg)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return ERROR_INVALID_ARGUMENT;

	if(DeviceIO_IsValidHandle(pDevIO))
	{
		close((UADDTYPE)pDevIO->m_hDevice);
		pDevIO->m_hDevice = INVALID_HANDLE_VALUE;
	}
	return SUCCESS;
}

HANDLE DeviceIO_GetDeviceHandle(PVOID pArg)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return INVALID_HANDLE_VALUE;

	return pDevIO->m_hDevice;
}

int DeviceIO_Send(
		PVOID pArg,
		DWORD dwIoctl,
		PVOID pInputBuffer,
		DWORD dwInputBufferLength,
		PVOID pOutputBuffer,
		DWORD dwOutputBufferLength,
		LPDWORD lpByterReturned
	)
{
	IOCTL_BUFFER ioctlBuff;
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	INT iRet = 0;
	
	if(!pDevIO)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(pDevIO))
		return ERROR_HANDLE_INVALID;

	memset(&ioctlBuff, 0, sizeof(IOCTL_BUFFER));
	ioctlBuff.pvInputBuffer = pInputBuffer;
	ioctlBuff.ulInputLength = dwInputBufferLength;

	ioctlBuff.pvOutputBuffer = pOutputBuffer;
	ioctlBuff.ulOutputLength = dwOutputBufferLength;

	iRet = ioctl((UADDTYPE)pDevIO->m_hDevice, dwIoctl,&ioctlBuff);

	if(iRet == 0)
		return SUCCESS;
	else
		return BeceemWiMAX_GetDeviceIoctlFailureReason();
}

int DeviceIO_SendOnly(
		PVOID pArg,
		DWORD dwIoctl,
		PVOID pInputBuffer,
		DWORD dwInputBufferLength
		)

{
	DWORD dwBytesRet = 0;
	return DeviceIO_Send(
			pArg,
			dwIoctl,
			pInputBuffer,
			dwInputBufferLength,
			NULL,
			0,
			&dwBytesRet
		);
}

int DeviceIO_SendIOCTL(PVOID pArg, DWORD dwIoctl)
{
	DWORD dwBytesRet = 0;
	return DeviceIO_Send(
			pArg,
			dwIoctl,
			NULL,
			0,
			NULL,
			0,
			&dwBytesRet
		);
}

int DeviceIO_SendDriverPkt(PVOID pArg, DWORD dwIoctl, PDRIVERPACKET pPkt)
{

	DWORD dwBytesRet = 0;
	if(!pPkt)
		return -1;

	return DeviceIO_Send(
			pArg,
			dwIoctl,
			DriverPacket_GetPacketToSend(pPkt),
			DriverPacket_GetPacketToSendLength(pPkt),
			NULL,
			0,
			&dwBytesRet
		);
}

void DeviceIO_RegisterNotifyCallback(PVOID pArg, PNOTIFYCALLBACK pNotifyCallback, PVOID pContext/*=NULL*/)
{
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return;

	pDevIO->m_pNotifyCallback = pNotifyCallback;
	pDevIO->m_pNotifyContext = pContext;
}

int DeviceIO_Receive(PVOID pArg, UCHAR *pucData, UINT uiDataLen)
{
	int iRet = 0;
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return ERROR_INVALID_ARGUMENT;

	if(pucData && uiDataLen > 0)
	{
		memset(pucData, 0, uiDataLen);
		iRet = read((UADDTYPE)pDevIO->m_hDevice, (VOID *)pucData, uiDataLen);
		if(pDevIO->m_pNotifyCallback)
			pDevIO->m_pNotifyCallback(pDevIO->m_pNotifyContext, 0, iRet);
	}
	return iRet;
}

BOOL DeviceIO_IsDeviceDettached(PVOID pArg)
{
    DWORD dwLastError = 0;/*GetLastError(); */
	PDEVICEIO pDevIO = (PDEVICEIO)pArg;
	if(!pDevIO)
		return FALSE;
	
	if(dwLastError == 31)
	{
		/* CloseHandle?? */
		return TRUE;
	}
	return FALSE;
}

int DeviceIO_GetLinkState(PVOID pArg, PLINK_STATE pLinkState)
{
	PDEVICEIO pDeviceIO = NULL;
	PDRIVERPACKET pDpkt = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	IOCTL_BUFFER ioctlBuff = {0};
	INT iRet = 0;

	if(!pWiMAX || !pLinkState)
		return ERROR_INVALID_ARGUMENT;

	pDeviceIO = (PDEVICEIO)&pWiMAX->m_Device;


    if(!DeviceIO_IsValidHandle(pDeviceIO))
    {
        return ERROR_HANDLE_INVALID;
    }


	memset(pLinkState, 0, sizeof(LINK_STATE));

	BECEEMCALLOC(pDpkt, PDRIVERPACKET, DRIVERPACKET, 1);

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, LINK_STATUS_REQ_PAYLOAD);


	ioctlBuff.pvInputBuffer = (PVOID)DriverPacket_GetLeaderData(pDpkt);
	ioctlBuff.ulInputLength = sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pDpkt);

	ioctlBuff.pvOutputBuffer = (PVOID)pLinkState;
	ioctlBuff.ulOutputLength = sizeof(LINK_STATE);

	iRet = ioctl((UADDTYPE)pDeviceIO->m_hDevice, IOCTL_BCM_GET_CURRENT_STATUS,&ioctlBuff);

	BECEEM_MEMFREE(pDpkt);
	
	if (iRet == 0)
		return SUCCESS;
	else
		return ERROR_DEVICEIOCONTROL_FAILED;
}

void DeviceIO_GetDeviceStatus(PVOID pArg)
{
	INT iRet = 0;
	IOCTL_BUFFER ioctlBuff = {0};
	DEVICE_DRIVER_INFO stDriverInfo = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDEVICEIO pDeviceIO = NULL;
	

	pDeviceIO = (PDEVICEIO)&pWiMAX->m_Device;
	
	ioctlBuff.pvInputBuffer = NULL;
	ioctlBuff.ulInputLength = 0;

	ioctlBuff.pvOutputBuffer = (PVOID)&stDriverInfo;
	ioctlBuff.ulOutputLength = sizeof(DEVICE_DRIVER_INFO);

	iRet = ioctl((UADDTYPE)pDeviceIO->m_hDevice, IOCTL_BCM_GET_DEVICE_DRIVER_INFO,&ioctlBuff);

	if(iRet)
		pWiMAX->u32DeviceStatus = DEV_UNKNOWN_DRIVER_STATE;		
	else
		pWiMAX->u32DeviceStatus = stDriverInfo.u32DeviceStatus;
	
}
