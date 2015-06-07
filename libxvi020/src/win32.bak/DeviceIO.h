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
 * Description	:	Device IO APIs for Win32.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef DEVICEIO_H
#define DEVICEIO_H

#pragma once

#include <windows.h>
#include "ErrorDefs.h"
#include "PrivateTypedefs.h"
#include "DriverPacket.h"
#include "SyncObjects.h"

typedef void (* PNOTIFYCALLBACK)(PVOID pContext, DWORD dwErrorCode, DWORD dwBytesTransfered);

typedef struct _DEVICEIO
{
	HANDLE hDevice;
	PNOTIFYCALLBACK pNotifyCallback;
	PVOID pNotifyContext;
 }DEVICEIO, *PDEVICEIO;

int DeviceIO_Init(PVOID pVoid);
int DeviceIO_Cleanup(PVOID pVoid);	
VOID CALLBACK DeviceIO_NotificationRoutine(
													DWORD dwErrorCode,                /*  completion code */
													DWORD dwNumberOfBytesTransfered,  /*  number of bytes transferred */
													LPOVERLAPPED lpOverlapped         /*  I/O information buffer */
												);

int DeviceIO_OpenHandle(PVOID pVoid, const char *szDeviceName);
int DeviceIO_CloseHandle(PVOID pVoid);
BOOL DeviceIO_IsValidHandle(PVOID pVoid);
HANDLE DeviceIO_GetDeviceHandle(PVOID pVoid);
int DeviceIO_GetLinkState(PVOID pArg, PLINK_STATE pLinkState);
void DeviceIO_GetDeviceStatus(PVOID pArg);
int DeviceIO_Send(
					PVOID pVoid,
					DWORD dwIoctl,
					const PVOID pInputBuffer,
					DWORD dwInputBufferLength,
					PVOID pOutputBuffer,
					DWORD dwOutputBufferLength,
					LPDWORD lpByterReturned
				);
int DeviceIO_SendOnly(
						PVOID pVoid,
						DWORD dwIoctl,
						PVOID pInputBuffer,
						DWORD dwInputBufferLength
				);

int DeviceIO_SendIOCTL(PVOID pVoid, DWORD dwIoctl);
int DeviceIO_SendDriverPkt(PVOID pVoid, DWORD dwIoctl, PDRIVERPACKET pPkt);
void DeviceIO_RegisterNotifyCallback(PVOID pVoid, PNOTIFYCALLBACK pNotifyCallback, PVOID pContext);
int DeviceIO_Receive(PVOID pVoid, UCHAR *pucData, UINT uiDataLen);
BOOL DeviceIO_IsDeviceDettached();
BOOL DeviceIO_ReadOverlapped(PVOID pArg,
                            LPVOID lpBuffer,
                            DWORD nNumberOfBytesToRead,
                            LPOVERLAPPED lpOverlapped,
                            LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);


#endif /* DEVICEIO_H */
