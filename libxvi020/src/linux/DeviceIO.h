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
 * Description	:	Device IO interface APIs for Linux.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef DEVICEIO_H
#define DEVICEIO_H

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


#include "Typedefs.h"
#include "ErrorDefs.h"
#include "DriverPacket.h"
#include "PrivateTypedefs.h"

typedef void (* PNOTIFYCALLBACK)(PVOID pContext, DWORD dwErrorCode, DWORD dwBytesTransfered);

typedef struct _IOCTL_BUFFER
{
	PVOID	pvInputBuffer;
	UINT	ulInputLength;
	PVOID	pvOutputBuffer;
	UINT	ulOutputLength;
}__attribute__((packed)) IOCTL_BUFFER, *PIOCTL_BUFFER;


typedef struct _DEVICEIO
{
	HANDLE m_hDevice;
	PNOTIFYCALLBACK m_pNotifyCallback;
	PVOID m_pNotifyContext;

}DEVICEIO, *PDEVICEIO;

void DeviceIO_Init(PVOID pArg);
void DeviceIO_Cleanup(PVOID pArg);
	
int DeviceIO_OpenHandle(PVOID pArg, const char *szDeviceName);

int DeviceIO_CloseHandle(PVOID pArg);

BOOL DeviceIO_IsValidHandle(PVOID pArg);

HANDLE DeviceIO_GetDeviceHandle(PVOID pArg);

int DeviceIO_Send(
	PVOID pArg,
	DWORD dwIoctl,
	PVOID pInputBuffer,
	DWORD dwInputBufferLength,
	PVOID pOutputBuffer,
	DWORD dwOutputBufferLength,
	LPDWORD lpByterReturned
	);
int DeviceIO_SendOnly(
	PVOID pArg,
	DWORD dwIoctl,
	PVOID pInputBuffer,
	DWORD dwInputBufferLength
	);

int DeviceIO_GetLinkState(PVOID pArg, PLINK_STATE pLinkState);
void DeviceIO_GetDeviceStatus(PVOID pArg);
int DeviceIO_SendIOCTL(PVOID pArg, DWORD dwIoctl);
int DeviceIO_SendDriverPkt(PVOID pArg, DWORD dwIoctl, PDRIVERPACKET pPkt);

void DeviceIO_RegisterNotifyCallback(PVOID pArg, PNOTIFYCALLBACK pNotifyCallback, PVOID pContext);

int DeviceIO_Receive(PVOID pArg, UCHAR *pucData, UINT uiDataLen);

BOOL DeviceIO_IsDeviceDettached(PVOID pArg);


#endif /* DEVICEIO_H */
