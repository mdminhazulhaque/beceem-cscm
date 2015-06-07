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
 * Description	:	Device IO for Win32.
 * Author		:
 * Reviewer		:
 *
 */


#include "DeviceIO.h"
#include "BeceemWiMAX.h"
#pragma warning(disable : 4311 4312) /* To avoid nasty typecast warning*/
#include "globalobjects.h"

int DeviceIO_Init(PVOID pVoid)
{
	PDEVICEIO pDeviceIO;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pDeviceIO = (PDEVICEIO)pVoid;

	pDeviceIO->hDevice = INVALID_HANDLE_VALUE; 
	pDeviceIO->pNotifyCallback = NULL;
	pDeviceIO->pNotifyContext = NULL;

	return 0;
}

int DeviceIO_Cleanup(PVOID pVoid)
{
	PDEVICEIO pDeviceIO = (PDEVICEIO)pVoid;
    int iValue = 0;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

    iValue = DeviceIO_CloseHandle(pDeviceIO);

    return iValue;
}

BOOL DeviceIO_IsValidHandle(PVOID pVoid)
{
	PDEVICEIO pDeviceIO = (PDEVICEIO)pVoid;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	return (pDeviceIO->hDevice != INVALID_HANDLE_VALUE && pDeviceIO->hDevice != NULL) ? TRUE : FALSE;
}

int DeviceIO_OpenHandle(PVOID pVoid, const char *szDeviceName)
{
	PDEVICEIO pDeviceIO;
    int iRet;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	if(!szDeviceName)
		return ERROR_INVALID_DEVICENAME;


	pDeviceIO = (PDEVICEIO)pVoid;

    
    if(DeviceIO_IsValidHandle(pDeviceIO))
    {
    	return SUCCESS;
    }
        
	pDeviceIO->hDevice = CreateFile( 
										szDeviceName,
										GENERIC_READ | GENERIC_WRITE ,/*| FILE_READ_ATTRIBUTES | SYNCHRONIZE, */
										0,
										NULL,
										OPEN_EXISTING,
										FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
										NULL 
								  );

	    
	iRet = DeviceIO_IsValidHandle(pDeviceIO) ? SUCCESS : ERROR_CREATEFILE_FAILED;

    return iRet;
}

int DeviceIO_CloseHandle(PVOID pVoid)
{
	PDEVICEIO pDeviceIO;
    BOOL bClosed = TRUE;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)GpWiMAX;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pDeviceIO = (PDEVICEIO)pVoid;
	
	Mutex_Lock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
	if (GpWiMAX->m_DriverUnloadEvent.hEvent)
		CloseHandle(GpWiMAX->m_DriverUnloadEvent.hEvent);
	GpWiMAX->m_DriverUnloadEvent.hEvent = NULL;
	Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
	
	if(DeviceIO_IsValidHandle(pDeviceIO))
	{
		/* Close the handle */
        HANDLE hDevice =(void *)InterlockedExchangePointer((UINT32*)&pDeviceIO->hDevice,INVALID_HANDLE_VALUE);
        bClosed = CloseHandle(hDevice);
		pDeviceIO->hDevice = INVALID_HANDLE_VALUE;
	}

	return SUCCESS;
}

HANDLE DeviceIO_GetDeviceHandle(PVOID pVoid)
{
	PDEVICEIO pDeviceIO = NULL;
    HANDLE hHandle = INVALID_HANDLE_VALUE;
	if(!pVoid)
		return INVALID_HANDLE_VALUE;
    
    pDeviceIO = (PDEVICEIO)pVoid;
    hHandle = InterlockedCompareExchangePointer(&pDeviceIO->hDevice,INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE);
    return hHandle;
}

int DeviceIO_Send(
					PVOID pVoid,
					DWORD dwIoctl,
					const PVOID pInputBuffer,
					DWORD dwInputBufferLength,
					PVOID pOutputBuffer,
					DWORD dwOutputBufferLength,
					LPDWORD lpByterReturned
				)

{
	PDEVICEIO pDeviceIO = NULL;
	BOOL bRetStatus = FALSE;
	DWORD dwLastError = 0;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
    pDeviceIO = (PDEVICEIO)pVoid;

    if(!DeviceIO_IsValidHandle(pVoid))
    {
		return ERROR_HANDLE_INVALID;
    }

/*	DPRINT(DEBUG_MESSAGE, "Sending Data to Driver Start !!!");
	HexDump(pInputBuffer, dwInputBufferLength);
	DPRINT(DEBUG_MESSAGE, "Sending Data to Driver End !!!");
*/	

    bRetStatus =  DeviceIoControl(
						pDeviceIO->hDevice,
						dwIoctl,
						pInputBuffer,
						dwInputBufferLength,
						pOutputBuffer,
						dwOutputBufferLength,
						lpByterReturned,
						NULL
						);
    
    dwLastError = GetLastError();

   SetLastError(dwLastError);

	if(bRetStatus == 0)
	{	
		if(dwLastError == ERROR_DEVICE_NOT_CONNECTED)
		{
			DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,"Received ERROR_DEVICE_NOT_CONNECTED from driver.");
			Event_Set(&(GpWiMAX->m_DriverUnloadEvent));
			return ERROR_DEVICEIOCONTROL_FAILED;
		}
		else
			return BeceemWiMAX_GetDeviceIoctlFailureReason();
	}
	else
		return SUCCESS;
}

int DeviceIO_SendOnly(
						PVOID pVoid,
						DWORD dwIoctl,
						PVOID pInputBuffer,
						DWORD dwInputBufferLength
				   )

{
	DWORD dwBytesRet = 0;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	return DeviceIO_Send(
							pVoid,
							dwIoctl,
							pInputBuffer,
							dwInputBufferLength,
							NULL,
							0,
							&dwBytesRet
						);
}

int DeviceIO_SendIOCTL(PVOID pVoid, DWORD dwIoctl)

{
	DWORD dwBytesRet = 0;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	return DeviceIO_Send(
							pVoid,
							dwIoctl,
							NULL,
							0,
							NULL,
							0,
							&dwBytesRet
						);
}

int DeviceIO_SendDriverPkt(PVOID pVoid, DWORD dwIoctl, PDRIVERPACKET pPkt)
{
	if(!pVoid || !pPkt)
		return ERROR_INVALID_ARGUMENT;

		return DeviceIO_SendOnly(	
									pVoid,
									dwIoctl,
									(PVOID)DriverPacket_GetLeaderData(pPkt),
									sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pPkt)
								);
}

void DeviceIO_RegisterNotifyCallback(PVOID pVoid, PNOTIFYCALLBACK pNotifyCallback, PVOID pContext)
{
	PDEVICEIO pDeviceIO;
	if(!pVoid)
		return;

	pDeviceIO = (PDEVICEIO)pVoid;

	pDeviceIO->pNotifyCallback = pNotifyCallback;
	pDeviceIO->pNotifyContext = pContext;
}

int DeviceIO_Receive(PVOID pVoid, UCHAR *pucData, UINT uiDataLen)
{
	OVERLAPPED stOverlapped;
	PDEVICEIO pDeviceIO;
	BOOL bStatus;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	if(!pucData || uiDataLen == 0)
		return ERROR_INVALID_READBUFFER;

	pDeviceIO = (PDEVICEIO)pVoid;

	/*
		Overlapped Structure needs to be Intialized for calling ReadFileEx Function
	*/
	memset(&stOverlapped, 0, sizeof(stOverlapped));

	/*
		hEvent memmber of OVERLAPPED will be ignored by ReadFileEx
		We are using this to pass pDeviceIO
	*/

	bStatus = ReadFileEx(
							pDeviceIO->hDevice,
							pucData,
							uiDataLen,
							(LPOVERLAPPED)&stOverlapped,
							(LPOVERLAPPED_COMPLETION_ROUTINE)DeviceIO_NotificationRoutine
						);
	if(bStatus)
	{
		WaitForSingleObjectEx(GetCurrentThread(), INFINITE, TRUE);
	}
	else
	{
		return ERROR_READ_FAILED;
	}
	return SUCCESS;
}

BOOL DeviceIO_IsDeviceDettached()
{
	DWORD dwLastError = GetLastError();

	if(dwLastError == 31)
	{
		/* CloseHandle?? */
		return TRUE;
	}
	return FALSE;
}

/*
 Function:			NotificationRoutine
 Description:			It is a Callback Pending routinue whenever driver notifies 
							any event it will get called.
							and it will call registerd call back applicaiton routinue to notify application
 Input parameters:	parameter specific to call back routinue
 Return:				NONE
*/

VOID CALLBACK DeviceIO_NotificationRoutine(
												DWORD dwErrorCode,
												DWORD dwBytesTransfered,
												LPOVERLAPPED lpOverlapped
											)
{
	PDEVICEIO pDeviceIO = (PDEVICEIO)(lpOverlapped->hEvent);
	
	if(!pDeviceIO)
		return;

	pDeviceIO->pNotifyCallback(pDeviceIO->pNotifyContext, dwErrorCode, dwBytesTransfered);
}


int DeviceIO_GetLinkState(PVOID pArg, PLINK_STATE pLinkState)
{
	DWORD dwNumBytesReturned = 0;
	PDEVICEIO pDeviceIO = NULL;
	BOOL bRetStatus = FALSE;
	DRIVERPACKET pkt;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    DWORD dwLastError = 0;

	if(!pWiMAX || !pLinkState)
		return ERROR_INVALID_ARGUMENT;

	pDeviceIO = (PDEVICEIO)&pWiMAX->m_Device;

    if(!DeviceIO_IsValidHandle(pDeviceIO))
    {
        return ERROR_HANDLE_INVALID;
    }


	memset(pLinkState, 0, sizeof(LINK_STATE));

	DriverPacket_InitWithVal(&pkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(&pkt, LINK_STATUS_REQ_PAYLOAD);

	bRetStatus =  DeviceIoControl(
						pDeviceIO->hDevice,
						IOCTL_BCM_GET_CURRENT_STATUS,
						(PVOID)DriverPacket_GetLeaderData(&pkt),
						sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(&pkt),
						(PVOID)pLinkState,
						sizeof(LINK_STATE),
						&dwNumBytesReturned,
						NULL
						);
    dwLastError = GetLastError();
    SetLastError(dwLastError);

	if (bRetStatus)
		return SUCCESS;
	else
	{
		if(dwLastError == ERROR_DEVICE_NOT_CONNECTED)
		{
			DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,"Received ERROR_DEVICE_NOT_CONNECTED from driver.");
			Event_Set(&(pWiMAX->m_DriverUnloadEvent));
		}	
		return ERROR_DEVICEIOCONTROL_FAILED;
	}	
}

void DeviceIO_GetDeviceStatus(PVOID pArg)
{
	DEVICE_DRIVER_INFO stDriverInfo = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDEVICEIO pDeviceIO = NULL;
	BOOL bRetStatus = FALSE;
	DWORD dwNumBytesReturned = 0, dwLastError = 0;	

	pDeviceIO = (PDEVICEIO)&pWiMAX->m_Device;
	

	bRetStatus =  DeviceIoControl(
						pDeviceIO->hDevice,
						IOCTL_BCM_GET_DEVICE_DRIVER_INFO,
						NULL,
						0,
						(PVOID)&stDriverInfo,
						sizeof(DEVICE_DRIVER_INFO),
						&dwNumBytesReturned,
						NULL
						);
    dwLastError = GetLastError();
    SetLastError(dwLastError);

	if(bRetStatus)
		pWiMAX->u32DeviceStatus = stDriverInfo.u32DeviceStatus;	
	else
	{
		if(dwLastError == ERROR_DEVICE_NOT_CONNECTED)
		{
			DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,"Received ERROR_DEVICE_NOT_CONNECTED from driver.");
			Event_Set(&(pWiMAX->m_DriverUnloadEvent));
		}	
		pWiMAX->u32DeviceStatus = DEV_UNKNOWN_DRIVER_STATE;		
	}	
}

//windows only stuff.
BOOL DeviceIO_ReadOverlapped(PVOID pArg,
                            LPVOID lpBuffer,
                            DWORD nNumberOfBytesToRead,
                            LPOVERLAPPED lpOverlapped,
                            LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    PDEVICEIO pDeviceIO = NULL;
    BOOL bReturn = 0;
    DWORD dwLastError = 0;
	pDeviceIO = (PDEVICEIO)pArg;
    

    /* w/ everything going on, it seemed better to synchronize w/ the open close lock
       we didn't want someone to make a ReadFileEx on a bad handle, or a reassigned handle
     
       Note that the best option would be to use a read/write lock so multple IO's can be sent at once.
       another option would be reference counting and shared access to the ReadFileEx 
    */

    if(!DeviceIO_IsValidHandle(pDeviceIO))
    {
        SetLastError((DWORD)ERROR_HANDLE_INVALID);
        return FALSE;
    }

    SetLastError(0);
    bReturn = ReadFileEx(
        pDeviceIO->hDevice,
        lpBuffer,
        nNumberOfBytesToRead,
        lpOverlapped,
        lpCompletionRoutine);

    dwLastError = GetLastError();
    SetLastError(dwLastError);
    return bReturn;
}

