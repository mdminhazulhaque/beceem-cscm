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
 * Description	:	OMA client interface for CAPI 1.2
 * Author		:
 * Reviewer		:
 *
 */


#include "WiMAXCommonAPI.h"
#include "socketipc.h"

#ifdef WIN32
#include "win32/Thread.h"
#else
#include "linux/Thread.h"
#endif

#include "BeceemWiMAX.h"
#include "OMADMInfo.h"
//
#include "globalobjects.h"
//

// ============================================================================================
// To init the CAPI sockect client 
// ============================================================================================
// --------------------------------------------------------------------------------
// 
// Description: .
//		This would launch the socket client that connects to Beceem_vDM server
// Input parameters:
//		full path of the running image
// Returns: none
//
// -------------------------------------------------------------------------------------

int CAPIOMAInterfaceInit(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	UNUSED_ARGUMENT(pArg);
	pDeviceData->socketCAPIOMAIPC =  BCM_INVALID_SOCKET;
	
	if(Thread_IsAlive(&pWiMAX->m_CAPIOMAThread))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Launching m_CAPIOMAThread thread failed !!!");
		return -1;
	}
	if(Thread_Run(&(pWiMAX->m_CAPIOMAThread),(PTHREADFUNC)CAPIClientOMAMonitorThread, GpWiMAX))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Launching m_CAPIOMAThread thread failed !!!");
		return -2;
	}
	return 0;
}



UINT CAPIClientOMAMonitorThread(PVOID pArg)
{
	bcm_socket_t socConnect;
	INT32 i32BytesReceived = 0,iErrorCode = 0;
	INT32 i32SendCount = 0;
	BCM_VDM_IPC_INTF stRecvBCM_VDM_IPC_INTF = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	UNUSED_ARGUMENT(pArg);

	if(!Socket_Init())
	{
		printf("%s: Socket_Init Failed\n",__FUNCTION__);
	}
	
start_again:
	socConnect = Socket_Create();

	if (socConnect == BCM_INVALID_SOCKET)
	DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "start_Client->Listen socket creation failed");

	while (!Socket_Connect(socConnect, DEFAULT_LISTEN_ADDRESS, DEFAULT_LISTEN_PORT) && !pWiMAX->m_bKillWimaxCommonAPI_CAPIOMAThread)
	{
	//	DPRINT(DEBUG_ERROR,"%s: Socket Connect failed\n",__FUNCTION__);
#ifdef WIN32
		Sleep(1000);
#else
		usleep(1000 * 1000);
#endif		
	}

	if(pWiMAX->m_bKillWimaxCommonAPI_CAPIOMAThread)
	{
		goto end;
	}
	
	stRecvBCM_VDM_IPC_INTF.eInterfaceType = CAPI_INTF;
	//sending interface type to server
	i32SendCount = Socket_Send(socConnect, (UINT8 *) &stRecvBCM_VDM_IPC_INTF, sizeof(BCM_VDM_IPC_INTF));
	if(sizeof(BCM_VDM_IPC_INTF) != i32SendCount)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "%s:Socket_Send failed.\n", __FUNCTION__);
		Socket_Close(socConnect,(UINT32*) &iErrorCode);
		goto start_again;
	}
	pDeviceData->socketCAPIOMAIPC = socConnect;
	while (pDeviceData->socketCAPIOMAIPC != BCM_INVALID_SOCKET && !pWiMAX->m_bKillWimaxCommonAPI_CAPIOMAThread)
	{
		i32BytesReceived = Socket_Receive(socConnect,(UINT8 *) &stRecvBCM_VDM_IPC_INTF, sizeof(BCM_VDM_IPC_INTF));	

		if (i32BytesReceived == BCM_SOCKET_RECEIVE_ERROR || 
			i32BytesReceived > sizeof(BCM_VDM_IPC_INTF))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "%s:Socket receive error.\n", __FUNCTION__);
			Socket_Close(socConnect,(UINT32*) &iErrorCode);
			goto start_again;
			
		}
		else if(i32BytesReceived == 0)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "%s:Connection closed by server\n",
			   __FUNCTION__);
			Socket_Close(socConnect,(UINT32*)&iErrorCode);
			goto start_again;
		}
		else if(stRecvBCM_VDM_IPC_INTF.uiMessageLength > 0)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			   "%s : Received from Server: %d %d %d %d %s\n", __FUNCTION__,
				stRecvBCM_VDM_IPC_INTF.eInterfaceType, stRecvBCM_VDM_IPC_INTF.uiCommand,
				stRecvBCM_VDM_IPC_INTF.iStatus, stRecvBCM_VDM_IPC_INTF.uiMessageLength,
				stRecvBCM_VDM_IPC_INTF.uchMessage);
		
		CAPI_OMA_DM_interfacehandler(&stRecvBCM_VDM_IPC_INTF);
		  memset(&stRecvBCM_VDM_IPC_INTF,0, sizeof(BCM_VDM_IPC_INTF));
		}
	}
end:	
	Socket_Close(socConnect,(UINT32*)&iErrorCode);
	
	return 0;
}

VOID CAPI_OMA_DM_interfacehandler(PBCM_VDM_IPC_INTF pstRecvBCM_VDM_IPC_INTF)
{
	PST_SESSION_STATE_INFO pSessionState = NULL;
	WIMAX_API_PROV_OPERATION provOperation;
	BOOL bIsValid = FALSE;

	memset(&provOperation,0,sizeof(WIMAX_API_PROV_OPERATION));

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "%s:\n", __FUNCTION__);

	if(pstRecvBCM_VDM_IPC_INTF->uchMessage)
	{
       pSessionState = (PST_SESSION_STATE_INFO) pstRecvBCM_VDM_IPC_INTF->uchMessage;
	}
	

	switch(pstRecvBCM_VDM_IPC_INTF->uiCommand)
	{
	case eProvisioningOperation:

         if(pDeviceData->stCallBackFunctionCommonAPIs.pIndProvisioningOperation == NULL)
		 {
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
			     "No Callback for Provisioning Indication\n");
			 return;
		 }

		 if(pSessionState->eSessionType == BCM_VDMVDM_SessionType_DM)
         {
		    bIsValid = TRUE;
            if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Started)
            {
				provOperation = WIMAX_API_PROV_OPERATION_CFG_UPDATE_STARTED;
			}
			else if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Complete)
			{
                provOperation = WIMAX_API_PROV_OPERATION_CFG_UPDATE_COMPLETED;
			}
			else if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Aborted)
			{
                provOperation = WIMAX_API_PROV_OPERATION_CFG_UPDATE_FAILED;
			}
			else
			{
				bIsValid = FALSE;
			}
		}

        if(bIsValid)
		{
			Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		   pDeviceData->stCallBackFunctionCommonAPIs.pIndProvisioningOperation
			   (&pDeviceData->stDeviceID, provOperation);
			Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		}
		break;
		
	case ePackageUpdate:

         if(pDeviceData->stCallBackFunctionCommonAPIs.pIndPackageUpdate == NULL)
		 {
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "No Callback for Package Update\n");
			 return;
		 }

		 if(pSessionState->eSessionType == BCM_VDMVDM_SessionType_DL)
         {
		    bIsValid = TRUE;
            if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Started)
            {
				provOperation = WIMAX_API_PACK_UPDATE_STARTED;
			}
			else if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Complete)
			{
                provOperation = WIMAX_API_PACK_UPDATE_COMPLETED;
			}
			else if(pSessionState->eSessionState == BCM_VDMVDM_SessionState_Aborted)
			{
                provOperation = WIMAX_API_PACK_UPDATE_FAILED;
			}
			else
			{
				bIsValid = FALSE;
			}
		}

        if(bIsValid)
		{
		   Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		   pDeviceData->stCallBackFunctionCommonAPIs.pIndPackageUpdate
			   (&pDeviceData->stDeviceID, provOperation);
		   Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		}

		break;
	default:
		break;
	}
}

int SendOMA_DMClient(PBCM_VDM_IPC_INTF pSendpayload)
{
	INT32 i32SendCount = 0;
	if(pDeviceData->socketCAPIOMAIPC == BCM_INVALID_SOCKET || !pDeviceData->socketCAPIOMAIPC)
		return 0;
	
	i32SendCount = Socket_Send(pDeviceData->socketCAPIOMAIPC, (UINT8 *) pSendpayload, sizeof(BCM_VDM_IPC_INTF));

	if (i32SendCount == BCM_SOCKET_SEND_ERROR)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "%s:Send failed \n", __FUNCTION__);
		return 0;
	}
	return i32SendCount;
}
