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
 * Description	:	Utility functions for CAPI 1.2
 * Author		:
 * Reviewer		:
 *
 */

#include "globalobjects.h"
#include "WiMAXCommonAPI.h"

#ifdef WIN32
#include "Iprtrmib.h"
#include "Iphlpapi.h"
#include "pdh.h"
#elif MACOS
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#elif LINUX
#include <stdio.h>
#include <sys/socket.h>
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
#include <linux/sysctl.h>
#endif
#include <linux/if.h>

#include <sys/ioctl.h>
#include <sys/errno.h>
#endif
extern PWIMAX_MESSAGE pWiMAXMessage;
extern PBECEEM_COMMON_API_PERSISTANT_DATA pCommonAPIPersistantData;
BOOL g_bInitiateNetworkEntry = FALSE;
UINT32 u32ChipID=0;


void WiMAXCommonAPI_Cleanup(PBECEEMWIMAX pWiMAX)
{

#ifdef WIN32
    OutputDebugString("WiMAXCommonAPI_Cleanup");
#endif
    WiMAXCommonAPI_UpdatePersistantDataOnClose();
    if (Thread_Stop
	(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor),
	 (PTHREADSTOPFUNC) BeceemWiMAX_KillDeviceInsertThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Failed to stop DeviceInsertThread thread!!!");
	    Thread_Terminate(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor));
	}

    Mutex_Cleanup(&pDeviceData->stProprietaryCallback.objCallbackMutex);
    Mutex_Cleanup(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    Thread_Cleanup(&(pDeviceData->stThreadNoDevNotifier));
    Mutex_Cleanup(&pDeviceData->stMutexForInterfaceAccess);
    BECEEM_MEMFREE(pCommonAPIPersistantData)
	BECEEM_MEMFREE(pWiMAXMessage)
	BECEEM_MEMFREE(pDeviceData->pPrioritizedCAPL_LISTFor_WideScan)
	BECEEM_MEMFREE(pDeviceData->pOperatorChannelPlanSettings)
	BECEEM_MEMFREE(pDeviceData->pPrioritizedCAPL)
	BECEEM_MEMFREE(pDeviceData->pstChannelPlan)
	BECEEM_MEMFREE(pDeviceData) 
	SharedMem_Close(&(pWiMAX->m_SharedMemCommonAPI));
    Mutex_Cleanup(&(pWiMAX->m_ShMemCommonAPIMutex));
    Mutex_Cleanup(&(pWiMAX->m_CAPINWSearchMutex));

    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse));
    Event_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse));


}

int WiMAXCommonAPI_ReadSharedMemory(PBECEEMWIMAX pWiMAX)
{
    INT iRet = 0;

    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));
    iRet =
	SharedMem_Open(&(pWiMAX->m_SharedMemCommonAPI), sizeof(BECEEM_COMMON_API_PERSISTANT_DATA),
		       BECEEM_SHARED_COMMON_API_DATA);

    if (!pCommonAPIPersistantData)
	{
	    pCommonAPIPersistantData =
		(PBECEEM_COMMON_API_PERSISTANT_DATA) calloc(sizeof(BECEEM_COMMON_API_PERSISTANT_DATA),
							    1);
	    if (!pCommonAPIPersistantData)
		{
		    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));
		    DPRINT_COMMONAPI(DEBUG_ERROR,
				     "\nWiMAXCommonAPI_Init  pCommonAPIPersistantData is NULL");
		    return ERROR_MALLOC_FAILED;
		}

	}
    if (iRet)
	{
	    pCommonAPIPersistantData->u32NumberOfAppRunning = 1;
	    pCommonAPIPersistantData->privilege = WIMAX_API_PRIVILEGE_READ_WRITE;
	    iRet =
		SharedMem_Create(&(pWiMAX->m_SharedMemCommonAPI),
				 sizeof(BECEEM_COMMON_API_PERSISTANT_DATA),
				 BECEEM_SHARED_COMMON_API_DATA);
	    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) pCommonAPIPersistantData,
				sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

	}
    else
	{

	    SharedMem_ReadData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) pCommonAPIPersistantData,
			       sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);


	    pCommonAPIPersistantData->u32NumberOfAppRunning++;

	    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) pCommonAPIPersistantData,
				sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

	}
    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));
    return SUCCESS;
}

int WiMAXCommonAPI_Init(PBECEEMWIMAX pWiMAX)
{
    INT iRet = 0;


#ifdef WIN32
    WiMAXCommonAPI_RegistryCreateKeys();
#endif


    Mutex_Init(&(pWiMAX->m_ShMemCommonAPIMutex));
    Mutex_Create(&(pWiMAX->m_ShMemCommonAPIMutex), BECEEM_SHMEM_COMMON_API_MUTEX);

    Mutex_Init(&(pWiMAX->m_CAPINWSearchMutex));
    Mutex_Create(&(pWiMAX->m_CAPINWSearchMutex), BECEEM_NWSEARCH_COMMON_API_MUTEX);

    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry), TRUE, FALSE,
#ifdef MACOS
		 "Beceem10"
#else
		 NULL
#endif
	);

    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete), TRUE, FALSE,
#ifdef MACOS
		 "Beceem11"
#else
		 NULL
#endif
	);

    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse), TRUE, FALSE,
#ifdef MACOS
		 "Beceem12"
#else
		 NULL
#endif
	);

    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload), TRUE, FALSE,
#ifdef MACOS
		 "Beceem13"
#else
		 NULL
#endif
	);
    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest), TRUE, FALSE,
#ifdef MACOS
		 "Beceem14"
#else
		 NULL
#endif
	);

    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse), TRUE, FALSE,
#ifdef MACOS
		 "Beceem15"
#else
		 NULL
#endif
	);
    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse), TRUE, FALSE,
#ifdef MACOS
		 "Beceem16"
#else
		 NULL
#endif
	);
    Event_Init(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse));
    Event_Create(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse), TRUE, FALSE,
#ifdef MACOS
		 "Beceem17"
#else
		 NULL
#endif
	);


    if (!pWiMAXMessage)
	BECEEMCALLOC(pWiMAXMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1);

    iRet = WiMAXCommonAPI_ReadSharedMemory(pWiMAX);

    if (iRet)
	return iRet;

    if (!pDeviceData)
	{
	BECEEMCALLOC(pDeviceData, PBECEEM_COMMON_API_DEVICE_DATA, BECEEM_COMMON_API_DEVICE_DATA, 1)}

    Mutex_Init(&(pDeviceData->stProprietaryCallback.objCallbackMutex));
    Mutex_Create(&(pDeviceData->stProprietaryCallback.objCallbackMutex), BECEEM_PROPRIETARY_MUTEX);

    Mutex_Init(&(pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex));
    Mutex_Create(&(pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex),
		 BECEEM_CAPI_CALLBACK_MUTEX);
    Thread_Init(&(pDeviceData->stThreadNoDevNotifier), pWiMAX);
    Mutex_Init(&pDeviceData->stMutexForInterfaceAccess);
    Mutex_Create(&(pDeviceData->stMutexForInterfaceAccess), BECEEM_CAPI_INTERFACE_MUTEX);

	pDeviceData->s32CINRThreshold = CINR_THRESHOLD;
	pDeviceData->s32RSSIThreshold = RSSI_THRESHOLD;

    return SUCCESS;

}

void WiMAXCommonAPI_WriteSharedMem(PBECEEMWIMAX pWiMAX)
{
    if (!pCommonAPIPersistantData)
	return;
    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));
    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) pCommonAPIPersistantData,
			sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);
    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));
}

int WiMAXCommonAPI_FillWiMAXMsg(PWIMAX_MESSAGE pWMMsg,
				USHORT usMessageType, USHORT usSubType, USHORT usMsgSize)
{
    if (!pWMMsg)
	return -1;

    memset(pWMMsg, 0, sizeof(WIMAX_MESSAGE));
    pWMMsg->usType = usMessageType;
    pWMMsg->usSubType = usSubType;
    return sizeof(WIMAX_MESSAGE) + usMsgSize - MAX_VARIABLE_LENGTH;
}

void WiMAXCommonAPI_BCMCommonAPICallback(PVOID context, PVOID pBCMMessage, UINT32 ulBCMMessageLength)
{
    PWIMAX_MESSAGE pWiMaxMsg = (PWIMAX_MESSAGE) pBCMMessage;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(context);
    if (!pBCMMessage)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nAppcallback called with null buffer !!!");
	    return;
	}
    switch (pWiMaxMsg->usType)
	{
	  case TYPE_NETWORK_SEARCH:
	      WiMAXCommonAPI_SearchResponse(pWiMaxMsg);
	      break;

	  case TYPE_SHUTDOWN:
	      WiMAXCommonAPI_ShutDownResponse(pWiMaxMsg);
	      break;

	  case TYPE_IDLE_MODE:
	      {
		  switch (pWiMaxMsg->usSubType)
		      {
			case SUBTYPE_IDLE_MODE_INDICATION:
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Received SUBTYPE_IDLE_MODE_INDICATION");
			    break;
			case SUBTYPE_IDLE_MODE_STATUS_RESP:
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Received SUBTYPE_IDLE_MODE_STATUS_RESP");
			    break;
			case SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP:
			    DPRINT_COMMONAPI(DEBUG_MESSAGE,
					     "Received SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP");
			    break;
			case SUBTYPE_IDLE_MODE_WAKEUP_RESP:
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Received SUBTYPE_IDLE_MODE_WAKEUP_RESP");
			    Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse));
			    break;

			default:
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Received SUBTYPE %d on IDLE MODE ",
					     pWiMaxMsg->usSubType);
			    break;

		      }
	      }
	      break;

	  case TYPE_NETWORK_ENTRY:
	      WiMAXCommonAPI_NetEntryExitResponse(pWiMaxMsg);
	      break;

	  case TYPE_SS_INFO:
	      WiMAXCommonAPI_SSInfoResponse(pWiMaxMsg);
	      break;

	  case TYPE_MACADDRESS:
	      break;

	  case TYPE_CHIPSET_CONTROL:
	      WiMAXCommonAPI_ChipsetResponse(pWiMaxMsg);
	      break;

	  case TYPE_STATISTICS:
	      WiMAXCommonAPI_GetStatisticsResponse(pWiMaxMsg);
	      break;

	  case TYPE_MACMGMT:
	      break;

	  case TYPE_SERVICE_FLOW:
	      WiMAXCommonAPI_ServiceFlowResponse(pWiMaxMsg);
	      break;

	  case TYPE_AUTHENTICATION:
	      break;

	  case TYPE_GPIO:
	      break;

	  case TYPE_HANDOFF_MESSAGE:
	      break;
	  case TYPE_START_LOG:
	      break;

	  case TYPE_VENDOR_SPECIFIC_INFO:
	      break;
	  case DEVICE_RESET_UNLOAD_TYPE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n~~~~~~~~~~ DEVICE_RESET_UNLOAD_TYPE RECEIVED ~~~~~~~~~~\n");
	      if ((pWiMaxMsg->usSubType == DEVICE_UNLOAD)
		  /*&& (pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove) */ )
		  {
		      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
		      if (Thread_Run
			  (&(pDeviceData->stThreadNoDevNotifier),
			   (PTHREADFUNC) WiMAXCommonAPI_NoCardNotificationThread, pDeviceData))
			  {
			      DPRINT_COMMONAPI(DEBUG_ERROR,
					       "Launching Device insert monitor thread failed !!!");
			  }
#ifdef WIN32
		      Sleep(2);
#else
		      usleep(2000);
#endif

		      //
		  }

	      break;
	  case TYPE_MIB:
	      WiMAXCommonAPI_MIBIndication(pWiMaxMsg);
	      break;

	  case TYPE_DEVICE_CONFIG:
	      break;
	  case TYPE_MAC_LOGING_INFO:
	      break;

	  case TYPE_SET_WIMAX_OPTIONS:
	      break;

	  case TYPE_SLEEP_MODE:
	      break;
	  case TYPE_NETWORK_LAYER_CONTROL:
	      break;
	  case TYPE_ANTENNA_SWITCHING_SELECTION:
	      break;
	  default:
	      break;
	}

    if (pDeviceData->stProprietaryCallback.pAppCallback &&
	pDeviceData->stProprietaryCallback.usTYPE == pWiMaxMsg->usType)
	{
	    PFNBCMCALLBACK pAppCallback = NULL;

	    Mutex_Lock(&pDeviceData->stProprietaryCallback.objCallbackMutex);
	    pAppCallback = pDeviceData->stProprietaryCallback.pAppCallback;
	    Mutex_Unlock(&pDeviceData->stProprietaryCallback.objCallbackMutex);
	    if (pAppCallback)
		pAppCallback(pDeviceData->stProprietaryCallback.pAppCallbackContext, pBCMMessage,
			     ulBCMMessageLength);
	    pDeviceData->stProprietaryCallback.usTYPE = 0;
	}
}

int WiMAXCommonAPI_ShutDownResponse(PWIMAX_MESSAGE pWiMAXMsg)
{
    UINT uiShutdownModeStatus = 0;
    PBSLIST pstBSList = 0;
    PBSINFO pBaseInfo = 0;
    UINT uiNumOfBaseStations = 0;
    UINT bNotifyApp = FALSE;
    int i = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    WIMAX_API_RF_STATE eRFStatus = WIMAX_API_RF_ON;

    if (!pWiMAXMsg)
	return -1;

    switch (pWiMAXMsg->usSubType)
	{
	  case SUBTYPE_SHUTDOWN_MODE_STATUS_RESP:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n~~~~~~~~~~ SHUTDOWN MODE STATUS RESPONSE RECEIVED ~~~~~~~~~~\n");

	      uiShutdownModeStatus = *(UINT *) pWiMAXMsg->szData;
	      if (uiShutdownModeStatus == 0)
		  {

		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBASEBAND IS IN SHUTDOWN MODE");
		      if (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Data_Connected
			  && pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork)
			  {

			      IndDisconnectToNetwork pIndDisconnectToNetwork = NULL;

			      Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
			      pIndDisconnectToNetwork =
				  pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork;
			      Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\npIndDisconnectToNetwork");
			      if (pIndDisconnectToNetwork)
				  pIndDisconnectToNetwork(&pDeviceData->stDeviceID,
							  WIMAX_API_CONNECTION_SUCCESS);
			  }

		      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_SW;

		      eRFStatus = WIMAX_API_RF_OFF;
		  }
	      else
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBASEBAND IS NOT IN SHUTDOWN MODE");
		      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Scanning;
		      eRFStatus = WIMAX_API_RF_ON;

		  }

	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
	      break;
	  case SUBTYPE_SHUTDOWN_RESP:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~ SHUTDOWN MODE RESPONSE RECEIVED ~~~~~~~~~~\n");

	      uiShutdownModeStatus = *(UINT *) pWiMAXMsg->szData;
	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
	      if (uiShutdownModeStatus == 0)
		  {

		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n\tBASEBAND IS IN SHUTDOWN STATE");
		      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_SW;
		      eRFStatus = WIMAX_API_RF_OFF;
              bNotifyApp = TRUE;
		  }

	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	      break;
	  case SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND:
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Scanning;

	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~ PERIODIC WAKE UP INDICATION RECEIVED IN SHUTDOWN MODE ~~~~~~~~~~\n");

	      pstBSList = (PBSLIST) pWiMAXMsg->szData;
	      /*Indication to CM regarding RF status of the device */
	      eRFStatus = WIMAX_API_RF_ON;

	      if (pstBSList)
		  {
		      uiNumOfBaseStations = pstBSList->u32NumBaseStationsFound;
		      pBaseInfo = pstBSList->stBSInformation;

		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n%d BASE STATIONS FOUND.\n",
				       uiNumOfBaseStations);

		      for (i = 0; i < (int) uiNumOfBaseStations; i++, pBaseInfo++)
			  {
			      DPRINT_COMMONAPI(DEBUG_MESSAGE,
					       "\nBASE STATION : %d\n-------------------\n", i + 1);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE,
					       "\nBS ID  :%02X %02X %02X %02X %02X %02X %02X",
					       pBaseInfo->aucBSID[1], pBaseInfo->aucBSID[2],
					       pBaseInfo->aucBSID[3], pBaseInfo->aucBSID[4],
					       pBaseInfo->aucBSID[5], pBaseInfo->aucBSID[6],
					       pBaseInfo->aucBSID[7]);
			      if (pBaseInfo->aucBSID[0])
				  {
				      DPRINT_COMMONAPI(DEBUG_MESSAGE, " - COMPRESSED MAP");
				  }
			      else
				  DPRINT_COMMONAPI(DEBUG_MESSAGE, " - NORMAL MAP");

			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nPreamble Index ID  :%d",
					       pBaseInfo->u32PreambleIndex);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nCenter Frequency : %d kHz",
					       pBaseInfo->u32CenterFrequency);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBandwidth : %d kHz",
					       pBaseInfo->u32Bandwidth);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nRelative Signal Strength : %d dBm",
					       pBaseInfo->s32RelativeSignalStrength);
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nCINR : %d dB\n\n", pBaseInfo->s32CINR);
			  }
		  }
	      break;

	  case SUBTYPE_SHUTDOWN_REQ_FROM_FIRMWARE:
	      eRFStatus = WIMAX_API_RF_OFF;
	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_SW;
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~ SHUTDOWN REQUEST FROM FIRMWARE RECEIVED ~~~~~~~~~~\n");
	      break;

	  case SUBTYPE_SHUTDOWN_MODE_COMPLETE_WAKE_UP_IND:
	      eRFStatus = WIMAX_API_RF_ON;
	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~ COMPLETE WAKE UP INDICATION RECEIVED IN SHUTDOWN MODE ~~~~~~~~~~\n");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Scanning;

	      break;
	}

    if (pDeviceData->stCallBackFunctionCommonAPIs.pIndControlPowerManagement 
		&& bNotifyApp)
	{
	    IndControlPowerManagement pIndControlPowerManagement = NULL;;
	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndControlPowerManagement =
		pDeviceData->stCallBackFunctionCommonAPIs.pIndControlPowerManagement;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndControlPowerManagement(&pDeviceData->stDeviceID, eRFStatus);
	}

    return SUCCESS;
}

VOID WiMAXCommonAPI_SearchResponse(PWIMAX_MESSAGE pWiMAXMsg)
{
    PST_CAPL_SEARCH_RSP pstCaplRsp = NULL;
    char szOutputData[300] = { 0 };
#ifdef ScanningTime
#ifdef WIN32
    char tempStr[256] = { 0 };
#endif
#endif

    int nLen = 0;
    UINT32 u32Index = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    sprintf(szOutputData, "%s", NSP_NAME);
    nLen = (int) strlen(szOutputData) + 1;

    if (!pWiMAXMsg)
	return;

    switch (pWiMAXMsg->usSubType)
	{
	  case SUBTYPE_NETWORK_SEARCH_RESP:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n~~~~~~~~~~ NETWORK SEARCH RESPONSE RECEIVED ~~~~~~~~~~\n");
	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse));
	      break;

	  case SUBTYPE_CAPL_BASED_SEARCH_RSP:
	      {

		  PST_CAPL_SEARCH_RSP pstCAPL_Search_Resp = (PST_CAPL_SEARCH_RSP) & pWiMAXMsg->szData;

		  Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse));

		  if (pstCAPL_Search_Resp->u32ErrorStatus)
		      {
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error code returned from CAPL search %d\n",
					   pstCAPL_Search_Resp->u32ErrorStatus);
			  Event_Set(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
			  return;
		      }

		  WiMAXCommonAPI_CheckAndAddBStoList(pstCAPL_Search_Resp);

		  pstCaplRsp = (PST_CAPL_SEARCH_RSP) & pWiMAXMsg->szData;
		  if (pstCaplRsp->u32SearchCompleteFlag)
		      {
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "*****CAPL Search Completed******\n");
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Number of BS Detected:%d\n",
					   pstCaplRsp->u32NumBS);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error Status:%d\n",
					   pstCaplRsp->u32ErrorStatus);
			  Event_Set(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
		      }
		  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Detected BS details\n");
#ifdef ScanningTime
#ifdef WIN32
		  OutputDebugString("\nBeceem Device search found the folloiwng BS\n");

		  for (u32Index = 0; u32Index < pstCaplRsp->u32NumBS; u32Index++)
		      {
			  sprintf(tempStr, "Beceem: #######BS Index:%d########\n", u32Index);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: bCompressedMap = %d\n",
				  pstCaplRsp->bsInfo[u32Index].bCompressedMap);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: bFullBsid = %d\n",
				  pstCaplRsp->bsInfo[u32Index].bFullBsid);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: BSID\t: %02x%02x%02x%02x%02x%02x\n",
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[0],
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[1],
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[2],
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[3],
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[4],
				  pstCaplRsp->bsInfo[u32Index].au8Bsid[5]);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: Preamble Index = %d\n",
				  pstCaplRsp->bsInfo[u32Index].u32PreambleIndex);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: Bandwidth = %dKHz\n",
				  pstCaplRsp->bsInfo[u32Index].u32Bandwidth);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: CenterFrequency = %d\n",
				  pstCaplRsp->bsInfo[u32Index].u32CenterFreq);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: CINR = %d dB\n",
				  pstCaplRsp->bsInfo[u32Index].s32CINR[0]);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: RSSI = %d dBm\n",
				  pstCaplRsp->bsInfo[u32Index].s32RSSI[0]);
			  OutputDebugString(tempStr);
			  sprintf(tempStr, "Beceem: Index = %d\n",
				  pstCaplRsp->bsInfo[u32Index].u32Index);
			  OutputDebugString(tempStr);

		      }		//End of for loop

#endif

#else
		  for (u32Index = 0; u32Index < pstCaplRsp->u32NumBS; u32Index++)
		      {
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "#######BS Index:%d########\n", u32Index);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "bCompressedMap = %d\n",
					   pstCaplRsp->bsInfo[u32Index].bCompressedMap);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "bFullBsid = %d\n",
					   pstCaplRsp->bsInfo[u32Index].bFullBsid);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "BSID\t: %02x%02x%02x%02x%02x%02x\n",
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[0],
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[1],
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[2],
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[3],
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[4],
					   pstCaplRsp->bsInfo[u32Index].au8Bsid[5]);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Preamble Index = %d\n",
					   pstCaplRsp->bsInfo[u32Index].u32PreambleIndex);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Bandwidth = %dKHz\n",
					   pstCaplRsp->bsInfo[u32Index].u32Bandwidth);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "CenterFrequency = %d\n",
					   pstCaplRsp->bsInfo[u32Index].u32CenterFreq);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "CINR = %d dB\n",
					   pstCaplRsp->bsInfo[u32Index].s32CINR[0]);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "RSSI = %d dBm\n",
					   pstCaplRsp->bsInfo[u32Index].s32RSSI[0]);
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Index = %d\n",
					   pstCaplRsp->bsInfo[u32Index].u32Index);

		      }		//End of for loop

#endif

	      }

	      break;

	}


}

VOID WiMAXCommonAPI_GetBSInfoToConnect(PBSINFO pBSInfo, WIMAX_API_WSTRING nspName)
{
    UCHAR16 *puchNSPName = NULL;
    UINT nLen = 0;

    UINT32 uiBSInfoIndex = 0, uiIndex = 0, uiNetEntryBSInfoIndex = 0, CINR = 0, RSSI = 0;

    BECEEMCALLOC_NOT_RET(puchNSPName, UCHAR16 *, UCHAR16, MAX_SIZE_OPERATORNAME)
	for (uiIndex = 0; uiIndex < pDeviceData->u32TotalOperatorsFound; uiIndex++)
	{


	    if (!ConvertToUnicode
		((char *) pDeviceData->stNwSearchResp[uiIndex].OperatorName, puchNSPName, &nLen))
		{
		    continue;
		}
	    if (memcmp(puchNSPName, nspName, nLen * sizeof(UCHAR16)) != 0)
		{
		    continue;
		}

	    CINR = pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[0].s32CINR + 10;
	    RSSI =
		pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[0].s32RelativeSignalStrength +
		123;

	    uiNetEntryBSInfoIndex = 0;

	    for (uiBSInfoIndex = 0; uiBSInfoIndex <
		 pDeviceData->stNwSearchResp[uiIndex].u32TotalBaseStationsFound; uiBSInfoIndex++)

		{
		    if (CINR <
			(UINT32) (pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[uiBSInfoIndex].
				  s32CINR + 10))
			{
			    uiNetEntryBSInfoIndex = uiBSInfoIndex;
			}

		}
	    memcpy(pBSInfo,
		   &pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[uiNetEntryBSInfoIndex],
		   sizeof(BSINFO));
	    break;
	}
BECEEM_MEMFREE(puchNSPName)}
WIMAX_API_RET WiMAXCommonAPI_SyncUpRequest()
{
    INT iLen = 0, indx = 0;
    INT iRetVal = 0;
    PST_SYNCUP_REQ_PARAMS pstSyncReqParams = NULL;
    PWIMAX_MESSAGE pWiMAXMsg = NULL;
    WIMAX_API_RET iCAPIRet = WIMAX_API_RET_SUCCESS;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    BECEEMCALLOC_RET(pWiMAXMsg, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1, WIMAX_API_RET)
	DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d and num of BS %d", __FUNCTION__, __LINE__,
			 pDeviceData->stCaplSearchRsp.u32NumBS);

    if (pDeviceData->stCaplSearchRsp.u32NumBS == 1)
	{
	    pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
	}
    else
	{
	    pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
	    for (indx = 1; indx < (INT) pDeviceData->stCaplSearchRsp.u32NumBS; indx++)
		{

		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "comparing below channels");
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "freq %d indx %d and CINR %d",
				     pDeviceData->stCaplSearchRsp.bsInfo[indx].u32CenterFreq, indx,
				     pDeviceData->stCaplSearchRsp.bsInfo[indx].s32CINR[0]);
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "freq %d indx %d and CINR %d",
				     pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->
									 u32PresentSyncup_CAPL_BSIndex].
				     u32CenterFreq, pDeviceData->u32PresentSyncup_CAPL_BSIndex,
				     pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->
									 u32PresentSyncup_CAPL_BSIndex].
				     s32CINR[0]);

		    if (pDeviceData->stCaplSearchRsp.bsInfo[indx].s32CINR[0] >
			pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
			s32CINR[0])
			{
			    pDeviceData->u32PresentSyncup_CAPL_BSIndex = indx;
			}

		}
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "selected freq %d indx %d and its CINR is %d",
		     pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
		     u32CenterFreq, pDeviceData->u32PresentSyncup_CAPL_BSIndex,
		     pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
		     s32CINR[0]);

    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pWiMAXMsg, TYPE_NETWORK_ENTRY, SUBTYPE_SYNC_UP_REQ,
				    sizeof(ST_SYNC_STATUS));
    pstSyncReqParams = (PST_SYNCUP_REQ_PARAMS) pWiMAXMsg->szData;
    pstSyncReqParams->u32CenterFreq =
	pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].u32CenterFreq;
    pstSyncReqParams->u32PreambleId =
	pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
	u32PreambleIndex;
    pstSyncReqParams->u32Bandwidth =
	pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].u32Bandwidth;
    memcpy(pstSyncReqParams->aucBSID,
	   pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].au8Bsid,
	   sizeof(pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
		  au8Bsid));
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Sync Up Request Details\n");
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "CenterFreq =%d\n", pstSyncReqParams->u32CenterFreq);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "u32PreambleId =%d\n", pstSyncReqParams->u32PreambleId);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "u32Bandwidth =%d\n", pstSyncReqParams->u32Bandwidth);
#ifdef ScanningTime
#ifdef WIN32
    OutputDebugString("Beceem: sending out SYNC Up request\n");
#endif
#endif

    g_bInitiateNetworkEntry = TRUE;
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry));

    iRetVal = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pWiMAXMsg, iLen);
    if (iRetVal)
	{
	    iCAPIRet = WIMAX_API_RET_FAILED;
	    g_bInitiateNetworkEntry = FALSE;
	    goto return_path;
	}
    if (Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry), SYNC_RESPONSE_TIMEOUT) == WAIT_TIMEOUT)
	{
	    iCAPIRet = WIMAX_API_RET_FAILED;
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "m_WiMAXCommonAPI_NetworkEntry timedout");
	    g_bInitiateNetworkEntry = FALSE;
	    pDeviceData->u32RxvdCmdConnectToNetwork = FALSE;
	    goto return_path;
	}


  return_path:
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "return value of %s is %d", __FUNCTION__, iCAPIRet);
    BECEEM_MEMFREE(pWiMAXMsg) return iCAPIRet;
}

VOID WiMAXCommonAPI_NetEntryExitResponse(PWIMAX_MESSAGE pWiMAXMsg)
{
#if 1

    PLINK_STATUS pLinkStatus = 0;
    PST_SYNC_STATUS pstSyncStatus;
    int i = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAXMsg)
	return;

    switch (pWiMAXMsg->usSubType)
	{
	  case SUBTYPE_SYNC_STATUS_RESP:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n~~~~~~~~~~ SYNC STATUS RESPONSE RECEIVED ~~~~~~~~~~\n");
	      pstSyncStatus = (PST_SYNC_STATUS) pWiMAXMsg->szData;
	      pDeviceData->u32RxvdCmdConnectToNetwork = FALSE;
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n SYNC STATUS  : ");
	      if (!pstSyncStatus->u32SyncStatus)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "PHY_SYNC ACHIEVED");
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n PREAMBLE ID  : %d",
				       pstSyncStatus->u32PreambleId);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n CINR MEAN : %d dB",
				       pstSyncStatus->s32CINRMean);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n CINR DEVIATION: %d dB",
				       pstSyncStatus->s32CINRDev);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n RSSI MEAN: %d dBm",
				       pstSyncStatus->s32RSSIMean);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n RSSI DEVIATION: %d dBm",
				       pstSyncStatus->s32RSSIDev);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n CENTER FREQUENCY: %d kHz",
				       pstSyncStatus->u32CenterFreq);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n BANDWIDTH : %d kHz",
				       pstSyncStatus->u32Bandwidth);

#ifdef ScanningTime
#ifdef WIN32
		      OutputDebugString("Beceem: SYNC Up Achieved\n");
#endif
#endif

		      if (g_bInitiateNetworkEntry)
			  WiMAXCommonAPI_NetworkEntryRequest();

		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "m_WiMAXCommonAPI_NetworkEntry is set");
		      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_NetworkEntry));

		  }
	      else
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "PHY_SYNC ERROR");
		      WiMAXCommonAPI_StartSearch();
		  }



	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	      break;

	  case SUBTYPE_NETWORK_ENTRY_STATUS_RESP:
	      pLinkStatus = (PLINK_STATUS) pWiMAXMsg->szData;
	      if (pLinkStatus->eStatusCode == LINKUP_ACHIEVED)
		  memcpy(&pDeviceData->stLinkStatus, pLinkStatus, sizeof(LINK_STATUS));
	      else
		  {
		      pDeviceData->stLinkStatus.eStatusCode = pLinkStatus->eStatusCode;
		      pDeviceData->stLinkStatus.BsInfoIsValidFlag = pLinkStatus->BsInfoIsValidFlag;
		  }

	      Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse));

	      if ((pLinkStatus->eStatusCode == LINKUP_ACHIEVED) && pLinkStatus->BsInfoIsValidFlag)
		  {
		      pDeviceData->u32CenterFreq = pLinkStatus->BsInfo.u32DlCenterFrequency;

#ifdef ScanningTime
#ifdef WIN32
		      OutputDebugString
			  ("\nBeceem: LinkUp achieved, call Trellia CM callback IndConnectToNetwork with Success\n");
#endif
#endif
		  }
	      else if (pLinkStatus->eStatusCode == WAIT_FOR_NETWORK_ENTRY_CMD)
		  {
		      if (g_bInitiateNetworkEntry)
			  WiMAXCommonAPI_NetworkEntryRequest();
		  }
	      else if (pLinkStatus->eStatusCode != LINKUP_ACHIEVED)
		  {
		      if (g_bInitiateNetworkEntry)
			  {
			      WiMAXCommonAPI_NetworkEntryRequest();
			      return;
			  }

		      pDeviceData->u32RxvdSyncRsp = TRUE;

		      switch (pLinkStatus->eStatusCode)
			  {
			    case PHY_SYNC_ERROR:
			    case MAC_SYNC_ERROR:
			    case RANGING_FAILURE:
			    case SBC_FAILURE:
			    case PKM_FAILURE:
			    case SATEK_FAILURE:
			    case DREG_RECEIVED:
			    case RESET_RECEIVED:
			    case REGISTRATION_FAILURE:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "######Link ERROR ########\n");
				WiMAXCommonAPI_StartSearch();
				break;
			    default:
				break;
			  }
		  }

	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n~~~~~~~~~~~ NETWORK ENTRY STATUS RESPONSE RECEIVED ~~~~~~~~~~\n");

	      if (pLinkStatus->u32VendorSpecificInformationLength > 0 &&
		  pLinkStatus->u32VendorSpecificInformationLength < 256)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nVENDOR SPECIFIC DATA \n");
		      for (i = 0; i < (int) pLinkStatus->u32VendorSpecificInformationLength; i++)
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "0x%02x ",
					   pLinkStatus->au8VendorSpecificInformation[i]);
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n");
		  }

	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	      break;
	  case SUBTYPE_DEVICE_STATUS_INDICATION:
	      WiMAXCommonAPI_DeviceStatusIndication(pWiMAXMsg);
	      break;
	}
#endif
}

VOID WiMAXCommonAPI_NetworkEntryRequest()
{
    PWIMAX_MESSAGE pstWiMAXMsg = NULL;
    INT iLen = 0;
    INT iRetVal = 0;
    PVENDOR_SPECIFIC_LINKPARAMS pVendorInfo = 0;

    BECEEMCALLOC_NOT_RET(pstWiMAXMsg, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1) g_bInitiateNetworkEntry = FALSE;

    iLen = WiMAXCommonAPI_FillWiMAXMsg(pstWiMAXMsg, TYPE_NETWORK_ENTRY, SUBTYPE_NETWORK_ENTRY_REQ, 4);	/*  Data len = 14 = 4+10 -> 10 bytes of Vendor specific data */
    pVendorInfo = (PVENDOR_SPECIFIC_LINKPARAMS) pstWiMAXMsg->szData;
    pVendorInfo->u32VendorSpecificInformationLength = 0;
    pVendorInfo->au8VendorSpecificInformation[0] = 0;
#ifdef ScanningTime
#ifdef WIN32
    OutputDebugString("Beceem: sending out LinkUp request CM\n");
#endif
#endif
    iRetVal = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstWiMAXMsg, iLen);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n Nw Entry request Returned %d", iRetVal);

    BECEEM_MEMFREE(pstWiMAXMsg);

}

int WiMAXCommonAPI_SubscribeToStats(INT iPeriod)
{
    PWIMAX_MESSAGE pstWiMAXMsg = NULL;
    INT iLen = 0;
    INT iRetVal = 0;

    BECEEMCALLOC(pstWiMAXMsg, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstWiMAXMsg, TYPE_STATISTICS, SUBTYPE_BASEBAND_STATISTICS_REQ, 4);
    *(UINT *) pstWiMAXMsg->szData = iPeriod;

    iRetVal = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstWiMAXMsg, iLen);

    BECEEM_MEMFREE(pstWiMAXMsg);
    return SUCCESS;

}

VOID WiMAXCommonAPI_GetStatisticsResponse(PWIMAX_MESSAGE pstWiMAXMessage)
{
    stStatistics_SS *pStatistics = 0;
    WIMAX_API_DEVICE_STATUS eDeviceStatusPrevious;
    WIMAX_API_STATUS_REASON eStatusReasonPrevious;
    WIMAX_API_CONNECTION_PROGRESS_INFO eConnectionProgressPrevious;

    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (pWiMAX->m_bModemInShutdown || pWiMAX->m_bIdleMode)
	{
#ifdef WIN32
	    Sleep(100);
#else
	    usleep(100000);
#endif
	    return;
	}

    eDeviceStatusPrevious = pDeviceData->eDeviceStatus;
    eStatusReasonPrevious = pDeviceData->eStatusReason;
    eConnectionProgressPrevious = pDeviceData->eConnectionProgress;

    switch (pstWiMAXMessage->usSubType)
	{
	  case SUBTYPE_BASEBAND_STATISTICS_INDICATION:
	      /*DPRINT_COMMONAPI(DEBUG_MESSAGE,"\n~~~~~~~~~~ BASEBAND STATISTICS RESPONSE ~~~~~~~~~~\n"); */

	      pStatistics = (stStatistics_SS *) pstWiMAXMessage->szData;

	      pDeviceData->stStatisticsInfo.structureSize = sizeof(WIMAX_API_CONNECTION_STAT);
	      pDeviceData->stStatisticsInfo.totalRxByte = pStatistics->tDPStats.u32NumofBytesToHost;
	      pDeviceData->stStatisticsInfo.totalRxPackets = pStatistics->tDPStats.u32NumOfSdusToHost;
	      pDeviceData->stStatisticsInfo.totalTxByte = pStatistics->tDPStats.u32NumofBytesFromHost;
	      pDeviceData->stStatisticsInfo.totalTxPackets =
		  pStatistics->tDPStats.u32NumOfPdusTransmitted;
	      pDeviceData->uchTransmitPower = pStatistics->tLinkStatsEx.s8TxPower;
	      pDeviceData->u32CenterFrequency = pStatistics->u32CenterFrequency;
	      pDeviceData->s32CINR = pStatistics->tLinkStatsEx.s32MeanCinr;
	      pDeviceData->s32RSSI = pStatistics->tLinkStats.s32CurrentRssi;
	      memcpy(pDeviceData->stLinkStatus.BsInfo.au8BSID,
		     pStatistics->tLinkStatsEx.au8BSID, sizeof(UCHAR) * 6);
	      pDeviceData->stLinkStatus.BsInfoIsValidFlag = TRUE;
	      break;

	  default:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nINVALID STATISTICS RESPONSE MESSAGE SUB-TYPE");
	      break;
	}
}
INT WiMAXCommonAPI_SendIdleModeWakeupRequest()
{
    INT iLen = 0, iRet = WIMAX_API_RET_SUCCESS;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    PWIMAX_MESSAGE pstRequestMessage = (PWIMAX_MESSAGE) calloc(1, sizeof(WIMAX_MESSAGE));

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_IDLE_MODE, SUBTYPE_IDLE_MODE_WAKEUP_REQ, 0);
    if (SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE,
			     "\nFailed to send the IDLE MODE WAKEUP REQUEST Message to device!!");
	    iRet = WIMAX_API_RET_FAILED;
	    goto return_path;
	}

    if (Event_Wait
	(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse),
	 (TIMEOUT_FOR_IDLEMODEWAKEUP_REQ + 100)) != WAIT_TIMEOUT)
	{
	    iRet = WIMAX_API_RET_SUCCESS;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "IDLE MODE STATUS # %d", pWiMAX->m_bIdleMode);

	}
    else
	{
	    iRet = WIMAX_API_RET_FAILED;
	}

  return_path:
    if (pstRequestMessage)
	free(pstRequestMessage);

    return iRet;
}

INT WiMAXCommonAPI_GetShutdownStatus()
{
    PWIMAX_MESSAGE pstWiMAXMsg = NULL;
    INT iLen = 0;
    INT iRet = 0, iRetval = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    BECEEMCALLOC(pstWiMAXMsg, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    iLen = WiMAXCommonAPI_FillWiMAXMsg(pstWiMAXMsg, TYPE_SHUTDOWN, SUBTYPE_SHUTDOWN_MODE_STATUS_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstWiMAXMsg, iLen);
    if (iRet)
	{
	    iRetval = WIMAX_API_RET_FAILED;
	    goto returnCode;
	}
    if (Event_Wait
	(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse),
	 (TIMEOUT_FOR_SHUTDOWN_REQ + 100)) != WAIT_TIMEOUT)
	{
	    iRetval = WIMAX_API_RET_SUCCESS;
	    goto returnCode;
	}
    else
	{
	    iRetval = WIMAX_API_RET_FAILED;
	    goto returnCode;
	}
  returnCode:
    BECEEM_MEMFREE(pstWiMAXMsg) return iRetval;

}

int WiMAXCommonAPI_GetSSInfo()
{
    PWIMAX_MESSAGE pstWiMAXMsg = NULL;
    INT iLen = 0;
    INT iRetVal = 0;

    BECEEMCALLOC(pstWiMAXMsg, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	iLen = WiMAXCommonAPI_FillWiMAXMsg(pstWiMAXMsg, TYPE_SS_INFO, SUBTYPE_SS_INFO_REQ, 0);
    iRetVal = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstWiMAXMsg, iLen);

    BECEEM_MEMFREE(pstWiMAXMsg);
    return SUCCESS;
}

VOID WiMAXCommonAPI_SSInfoResponse(PWIMAX_MESSAGE pWiMAXMsg)
{
    PSUBSCRIBER_STATION_INFO pSSInfo = NULL;
    char szOutputData[300] = { 0 };
    int nLen = 0;

    if (!pWiMAXMsg)
	return;

    if (pWiMAXMsg->usType != TYPE_SS_INFO || pWiMAXMsg->usSubType != SUBTYPE_SS_INFO_INDICATION)
	return;

    memset(&pDeviceData->stDeviceInfo, 0, sizeof(WIMAX_API_DEVICE_INFO));

    pDeviceData->stDeviceInfo.structureSize = sizeof(WIMAX_API_DEVICE_INFO);

    pSSInfo = (PSUBSCRIBER_STATION_INFO) pWiMAXMsg->szData;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n~~~~~~~~~~~~~~~ SS INFO RESPONSE ~~~~~~~~~~~~~~\n");
    WiMAXCommonAPI_FormatVersion(&pDeviceData->stDeviceInfo.asicVersion,
				 &pSSInfo->BasebandChipVersion, BCM_ASIC_NAME, 1);

    WiMAXCommonAPI_FormatVersion(&pDeviceData->stDeviceInfo.hwVersion,
				 &pSSInfo->BasebandChipVersion, BCM_HW_NAME, 1);

	u32ChipID=pSSInfo->BasebandChipVersion;

    WiMAXCommonAPI_FormatVersion(&pDeviceData->stDeviceInfo.rfVersion,
				 &pSSInfo->RFChipVersion, BCM_HW_NAME, 1);

    WiMAXCommonAPI_FormatVersion(&pDeviceData->stDeviceInfo.swVersion,
				 pSSInfo->u32FirmwareVersion, BCM_SW_NAME, 3);

    sprintf(szOutputData, "%s", BCM_DEVICE_NAME);
    ConvertToUnicode(szOutputData, pDeviceData->stDeviceInfo.vendorName, (UINT *) & nLen);

    memcpy(pDeviceData->stDeviceInfo.macAddress, pSSInfo->u8MacAddress, 6);

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

VOID WiMAXCommonAPI_FormatVersion(WIMAX_API_DEVICE_VERSION_P pVersion,
				  const UINT32 * pVersionSSinfo, const char *pName,
				  const UINT32 FormatCount)
{
    CHAR szOutputData[300] = { 0 };
#ifndef WIN32
#define LPSTR char*
#endif
    int nLen = 0;

    pVersion->structureSize = sizeof(WIMAX_API_DEVICE_VERSION);

    switch (FormatCount)
	{
	  case 1:
	      sprintf((LPSTR) szOutputData, "%08x", pVersionSSinfo[0]);
	      break;
	  default:
	      sprintf((LPSTR) szOutputData, "%d.%d.%d", pVersionSSinfo[0], pVersionSSinfo[1],
		      pVersionSSinfo[2]);
	      break;
	}

    ConvertToUnicode((char *) szOutputData, pVersion->version, (UINT *) & nLen);
    memset(szOutputData, 0, sizeof(szOutputData));
    sprintf((char *) szOutputData, "%s", pName);
    ConvertToUnicode((char *) szOutputData, pVersion->name, (UINT *) & nLen);
}

#ifdef WIN32
BOOL WiMAXCommonAPI_GetAdapterInfo(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo)
{
    //pDeviceData->stDeviceInfo.macAddress
    //GetMACAddressfrom the device  
    //get the adapter info if MAC addres matches 
    //Take the interface name as IP INterface index
    PIP_ADAPTER_INFO pAdapterInfo;
    UCHAR uchEndPartToCut[30] = " - Packet Scheduler Miniport";
    UCHAR uchTemp[255] = { 0 };
    UCHAR uchAdapterName[255] = { 0 };

    UINT uiAdapterNameLengthUnicode = 0;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT ulOutBufLen = 0;

    BECEEMCALLOC_RET(pAdapterInfo, IP_ADAPTER_INFO *, IP_ADAPTER_INFO, 1, BOOL)
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
	    BECEEM_MEMFREE(pAdapterInfo);
	    pAdapterInfo = (IP_ADAPTER_INFO *) calloc(1, ulOutBufLen);
	}

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
	    int iRetVal = 0;

	    pAdapter = pAdapterInfo;

	    while (pAdapter)
		{
		    iRetVal = memcmp(pDeviceData->stDeviceInfo.macAddress,
				     pAdapter->Address, pAdapter->AddressLength);
		    if (iRetVal == 0)
			{
			    UINT uiAdapterNameStringLength = 0;

			    strcpy((char *) uchTemp, pAdapter->Description);
			    if (strstr((char *) uchTemp, (char *) uchEndPartToCut))
				{
				    uiAdapterNameStringLength =
					(UCHAR) strlen((char *) uchTemp) -
					(UCHAR) strlen((char *) uchEndPartToCut);
				}
			    else
				{
				    uiAdapterNameStringLength = (UCHAR) strlen((char *) uchTemp);
				}
			    memcpy(uchAdapterName, uchTemp, uiAdapterNameStringLength);
			    if (ConvertToUnicode
				((char *) uchAdapterName, (PUSHORT) pInterfaceInfo->interfaceName,
				 &uiAdapterNameLengthUnicode))
				{
				    pInterfaceInfo->structureSize = sizeof(WIMAX_API_INTERFACE_INFO);
				    BECEEM_MEMFREE(pAdapterInfo);
				    return TRUE;
				}
			}
		    pAdapter = pAdapter->Next;
		}
	    BECEEM_MEMFREE(pAdapterInfo);
	}
    else
	{
	    BECEEM_MEMFREE(pAdapterInfo);
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Call to GetAdaptersInfo failed");
	}

    return FALSE;
}
#endif

BOOL WiMAXCommonAPI_GetIPInterfaceIndex(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo)
{
#ifdef WIN32
    return WiMAXCommonAPI_GetAdapterInfo(pInterfaceInfo);
#else
    return WiMAXCommonAPI_GetInterfaceInfo(pInterfaceInfo);
#endif
}

PVOID WiMAXCommonAPI_NoCardNotificationThread(PVOID pData)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32CardNotificationRetries = 0;
    UNUSED_ARGUMENT(pData);
    SET_THREAD_NAME();
    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Acquired @ %s and %d", __FUNCTION__, __LINE__);
    pDeviceData->stDeviceID.devicePresenceStatus = FALSE;
    pDeviceData->hDevice = NULL;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Released @ %s and %d", __FUNCTION__, __LINE__);
    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);

    if (pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove)
	{
	    IndDeviceInsertRemove pIndDeviceInsertRemove = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDeviceInsertRemove = pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndDeviceInsertRemove)
		pIndDeviceInsertRemove(&pDeviceData->stDeviceID, FALSE);
	}
    pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = TRUE;
     DPRINT_COMMONAPI(DEBUG_MESSAGE, "@ %s and %d", __FUNCTION__, __LINE__);
    while(pDeviceData->u32FlagForDeviceInsertMonitorRunning && (u32CardNotificationRetries < 20))
	{
        
        u32CardNotificationRetries++;
        DPRINT_COMMONAPI(DEBUG_MESSAGE, "Trying to kill  CardInsertMonitorThreadFunc %s and retry %d", __FUNCTION__, u32CardNotificationRetries);
        #ifdef WIN32
            Sleep(200);
        #else
            usleep(200*1000);
        #endif
        
	}
    
    pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = FALSE;
    if (Thread_Run
	(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor),
	 (PTHREADFUNC) WiMAXCommonAPI_CardInsertMonitorThreadFunc, pDeviceData))
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Device insert monitor thread failed !!!");
	}

#ifdef WIN32
    Sleep(2);
#else
    usleep(2000);
#endif
   return SUCCESS;
}

PVOID WiMAXCommonAPI_CardInsertMonitorThreadFunc(PVOID pData)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32PreviouseStatus = pDeviceData->stDeviceID.devicePresenceStatus;

    SET_THREAD_NAME();
    UNUSED_ARGUMENT(pData);
    pDeviceData->u32FlagForDeviceInsertMonitorRunning = TRUE;

    pDeviceData->u32RxvdCmdConnectToNetwork = FALSE;
    pDeviceData->u32RfOffFromUser = FALSE;
#ifndef WIN32
    usleep(1000000);
#else
    Sleep(1000);
#endif
    while (!pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor)
	{
	    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Acquired @ %s and %d", __FUNCTION__, __LINE__);
	    if (!DeviceIO_OpenHandle(&(pWiMAX->m_Device), BCM_SYMBOLIC_NAME))
		{
		    DeviceIO_CloseHandle(&(pWiMAX->m_Device));
		    pDeviceData->stDeviceID.devicePresenceStatus = TRUE;

		}
	    else
		{
		    pDeviceData->stDeviceID.devicePresenceStatus = FALSE;

		}
	    pDeviceData->hDevice = NULL;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Released @ %s and %d", __FUNCTION__, __LINE__);
	    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);

	    if (pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove &&
		(u32PreviouseStatus != (UINT32) pDeviceData->stDeviceID.devicePresenceStatus))
		{
		    IndDeviceInsertRemove pIndDeviceInsertRemove = NULL;

		    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		    pIndDeviceInsertRemove =
			pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove;
		    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
		    u32PreviouseStatus = pDeviceData->stDeviceID.devicePresenceStatus;
		    if (pIndDeviceInsertRemove)
			pIndDeviceInsertRemove(&pDeviceData->stDeviceID,
					       pDeviceData->stDeviceID.devicePresenceStatus);


		}

	    if (pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor)
		{
		    break;
		}
#ifndef WIN32
	    usleep(1000000);
#else
	    Sleep(1000);
#endif
	}
    pDeviceData->u32FlagForDeviceInsertMonitorRunning = FALSE;
    return SUCCESS;
}

WIMAX_API_RET WiMAXCommonAPI_OpenNRegisterCallback(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    MIB_OBJECT_INFO stMIBObj = { 0 };
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    WIMAX_API_RET  eRet = WIMAX_API_RET_SUCCESS;
    UINT32 u32InitRetries = 0;
    stMIBObj.u32MibObjId = BCM_CFG_FIXED_CENTER_FREQUENCY;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (pDeviceData->hDevice)
        {
            goto check_device_status;
        }
	
	    pWiMAX->m_bCommonAPIIfActive = FALSE;
	    pWiMAX->pDeviceMRUData = (PVOID) & pDeviceData->g_mru_init_struct;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    pDeviceData->hDevice = OpenWiMAXDevice();
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    //all of these calls check for
	    if (pDeviceData->hDevice == INVALID_HANDLE_VALUE)
		pDeviceData->hDevice = NULL;

	    pWiMAX->m_bCommonAPIIfActive = TRUE;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    if (!pDeviceData->hDevice)
		{
		    pDeviceId->devicePresenceStatus = FALSE;
		    pDeviceData->stDeviceID.devicePresenceStatus = FALSE;
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
		}
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    pDeviceId->devicePresenceStatus = TRUE;
	    pDeviceData->stDeviceID.devicePresenceStatus = TRUE;
	    pDeviceData->stDeviceID.deviceIndex = 1;
	    RegisterCallback(pDeviceData->hDevice, (void *) WiMAXCommonAPI_BCMCommonAPICallback, NULL);
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    if (WiMAXCommonAPI_GetShutdownStatus() != WIMAX_API_RET_SUCCESS)
		{
		    CloseWiMAXDevice();
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
		}
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Dev Status # %d and Previlege %d",
			     pDeviceData->eDeviceStatus, pDeviceData->privilege);
	    if (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_RF_OFF_SW &&
		(pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE))
		{
		    /*wake up the device here */
		    WiMAXCommonAPI_DoDummySearch();
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    if (pWiMAX->m_bDeviceInitInProgress)
			BeceemWiMAX_DeviceInitThreadFunc(pWiMAX);
		}
	    if (pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
		WiMAXCommonAPI_NwSearchPreconditioning();
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    pDeviceData->stStatisticsInfo.structureSize = sizeof(WIMAX_API_CONNECTION_STAT);
	    WiMAXCommonAPI_GetLinkStatus();
	    if (pWiMAX->m_bIdleMode)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
		    WiMAXCommonAPI_WakeupDeviceFromIdleMode();
		}

	    if (WiMAXCommonAPI_GetSSInfo())
        {   
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
        }
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    if (WiMAXCommonAPI_SubscribeToStats(1000))
		{   
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
        }
        
        if (pDeviceId->privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
		{
		    switch (WiMAXCommonAPI_GetLinkStatus())
			{
			  case IN_IDLE_MODE:
			      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Data_Connected;
			      pDeviceData->eConnectionProgress =
				  WIMAX_API_DEVICE_CONNECTION_PROGRESS_Registered;
			      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
			      break;
			  case SHUTDOWN_RECEIVED_CMD:
			      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_SW;
			      pDeviceData->eConnectionProgress =
				  WIMAX_API_DEVICE_CONNECTION_PROGRESS_Ranging;
			      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
			      break;
			  default:
			      break;
			}
		    goto start_mib_requests;
		}
        
	    if(WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError())
        {   
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
        }   
	    
	    pDeviceData->bOMADMInitFlag = FALSE;
	    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    if (pWiMAX->m_bDeviceInitInProgress && (u32InitRetries < 10))
            
        {
            DPRINT_COMMONAPI(DEBUG_MESSAGE, "Device INIT retry %d in progress -%s",u32InitRetries, __FUNCTION__);
        #ifdef WIN32
            Sleep(1000);
        #else
            usleep(1000000);
        #endif
        u32InitRetries++;
        }
#ifdef BLACK_LISTING_BS_ENABLED
	    memset(&pDeviceData->stBlackListedBSes, 0, sizeof(BLACK_LIST_BS_ENTRIES));
#endif
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
	    WiMAXCommonAPI_LaunchSearchThread((PTHREADFUNC) WiMAXCommonAPI_CaplSearchThread);
	    WiMAXCommonAPI_GetNetworkParameterNode();
	    WiMAXCommonAPI_InitCAPLTable();
	    if (WiMAXCommonAPI_InitChannelPlanTable())
        {   
		    eRet = WIMAX_API_RET_FAILED;
            goto ret_path;
        }  
	    WimaxCommonAPI_InitMruTable();
	    WiMAXCommonAPI_StartSearch();
 start_mib_requests:
	    pWiMAX->m_bKillWimaxCommonAPI_PeriodicMIBRequests = FALSE;
#if 1
	    if (!Thread_IsValidThread(&pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread))
		{
		    if (Thread_Run
			(&(pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread),
			 (PTHREADFUNC) WiMAXCommonAPI_PeriodicMIBRequestThread, pWiMAX))
			{
			    pWiMAX->m_bKillWimaxCommonAPI_PeriodicMIBRequests = FALSE;
			    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Net search thread failed !!!");
			     eRet = WIMAX_API_RET_FAILED;
                goto ret_path;
			}
		}
#endif
	
check_device_status:
    //Check modem status here - based on that return value can be set.
    if(WiMAXCommonAPI_CheckModemStatus())
        {
            eRet = WIMAX_API_RET_FAILED;
        }
    else
        {
            eRet = WIMAX_API_RET_SUCCESS;
        }
    
  ret_path:
    return eRet ;
}

void WiMAXCommonAPI_UpdateAccessrightsOnClose()
{
    BECEEM_COMMON_API_PERSISTANT_DATA stCAPIPersistantData = { 0 };
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));

    SharedMem_ReadData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) & stCAPIPersistantData,
		       sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

    if (pCommonAPIPersistantData->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
	{
	    stCAPIPersistantData.privilege = WIMAX_API_PRIVILEGE_READ_WRITE;
	}
    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) & stCAPIPersistantData,
			sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));

}

void WiMAXCommonAPI_UpdatePersistantDataOnClose()
{
    BECEEM_COMMON_API_PERSISTANT_DATA stCAPIPersistantData = { 0 };
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));
    SharedMem_ReadData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) & stCAPIPersistantData,
		       sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

    if (pCommonAPIPersistantData->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
	{
	    stCAPIPersistantData.privilege = WIMAX_API_PRIVILEGE_READ_WRITE;
	}
    if (stCAPIPersistantData.u32NumberOfAppRunning > 0)
	stCAPIPersistantData.u32NumberOfAppRunning--;

    if (pCommonAPIPersistantData->u32NumberOfAppRunning > 0)
	pCommonAPIPersistantData->u32NumberOfAppRunning--;

    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) & stCAPIPersistantData,
			sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));
}

#ifdef WIN32
BOOL WiMAXCommonAPI_RegistryCreateKeys()
{

#if 0
    HKEY hCommonAPI = NULL;
    DWORD dwSize = 0;
    DWORD dwOptions = 0, j = 0;
    CHAR strPath[255] = { 0 };
    LONG iRet = 0;

    if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_LOCAL_MACHINE, BCM_REGISTRY_KEY_CAPI,
					0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
					&hCommonAPI, &dwOptions))
	{
	    DWORD retval = GetLastError();

	    return FALSE;
	}

    dwSize = (DWORD) strlen(BCM_KEY_EXTENSION_VALUE_CAPI);
    iRet = RegSetValueEx(hCommonAPI, BCM_KEY_EXTENSION_CAPI, 0, REG_SZ,
			 BCM_KEY_EXTENSION_VALUE_CAPI, dwSize);

    GetCurrentDirectory(sizeof(strPath), strPath);

    dwSize = (DWORD) strlen(strPath);
    iRet = RegSetValueEx(hCommonAPI, BCM_KEY_PATH_CAPI, 0, REG_SZ, strPath, dwSize);

    RegCloseKey(hCommonAPI);
#endif
    return TRUE;
}
#endif

BOOL WiMAXCommonAPI_FirmwareDownload()
{
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    PST_FIRMWARE_DNLD pFirmwareDownload = NULL;
    char cFilename[100] = { 0 };
    int iLen = 0, iRet = 0;
    ST_FIRMWARE_DNLD stFirmwareDwnload = { 0 };
    BECEEMCALLOC_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1, BOOL)
	pFirmwareDownload = (PST_FIRMWARE_DNLD) pstRequestMessage->szData;

#ifdef WIN32
    memset(cFilename, 0, 100);

    if (!GetSystemDirectory(cFilename, 100))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nGetSystemDirectory failed. FirmwareDownload aborted!!!");
	    BECEEM_MEMFREE(pstRequestMessage) return FALSE;
	}


	if((u32ChipID&0xFFFFFF00) == SYMPHONY_CHIP_MASK)
		strcat(cFilename, DRIVER_PATH_350);
	else
	
		strcat(cFilename, DRIVER_PATH);
	
		

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMacxvi File path : %s", cFilename);

    strcpy((char *) pFirmwareDownload->szImageName, cFilename);
    pFirmwareDownload->u32ImageType = IMAGE_FIRMWARE;
    memcpy(&stFirmwareDwnload, pFirmwareDownload, sizeof(ST_FIRMWARE_DNLD));

    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_FIRMWARE_DNLD_REQ,
				    sizeof(ST_FIRMWARE_DNLD));
    memcpy(pFirmwareDownload, &stFirmwareDwnload, sizeof(ST_FIRMWARE_DNLD));

    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);

#else

    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    memset(cFilename, 0, 100);
    strcpy(cFilename, LIB_PATH);
    strcat(cFilename, SYS_CONFIG_FILE_NAME);
    strcpy((char *) pFirmwareDownload->szImageName, cFilename);
    pFirmwareDownload->u32ImageType = IMAGE_CONFIG_FILE;

    //printf("Image Name is: [%s]\n", pFirmwareDownload->szImageName);
    memcpy(&stFirmwareDwnload, pFirmwareDownload, sizeof(ST_FIRMWARE_DNLD));
    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_FIRMWARE_DNLD_REQ,
				    sizeof(ST_FIRMWARE_DNLD));

    memcpy(pFirmwareDownload, &stFirmwareDwnload, sizeof(ST_FIRMWARE_DNLD));
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);

    //printf("Should be success as downloading config file only >>>>\n");
    if (Event_Wait
	(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload),
	 (FIRMWARE_DOWNLOAD_TIMEOUT * 10)) == WAIT_TIMEOUT)
	{
	    BECEEM_MEMFREE(pstRequestMessage) return WIMAX_API_RET_FAILED;
	}

    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload));
    memset(cFilename, 0, 100);
    memset(pstRequestMessage, 0, sizeof(WIMAX_MESSAGE));
    strcpy(cFilename, LIB_PATH);

    
	if((u32ChipID&0xFFFFFF00) == SYMPHONY_CHIP_MASK)
		strcat(cFilename, SYS_350_FW_FILE_NAME);
	else	
		strcat(cFilename, SYS_FW_FILE_NAME);

    strcpy((char *) pFirmwareDownload->szImageName, cFilename);

    //printf("Image Name is: [%s]\n", pFirmwareDownload->szImageName);
    pFirmwareDownload->u32ImageType = IMAGE_FIRMWARE;
    memset(&stFirmwareDwnload, 0, sizeof(ST_FIRMWARE_DNLD));
    memcpy(&stFirmwareDwnload, pFirmwareDownload, sizeof(ST_FIRMWARE_DNLD));

    DPRINT_COMMONAPI(DEBUG_ERROR, "received path\n");
    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_FIRMWARE_DNLD_REQ,
				    sizeof(ST_FIRMWARE_DNLD));
    memcpy(pFirmwareDownload, &stFirmwareDwnload, sizeof(ST_FIRMWARE_DNLD));
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);
    //printf("Should be success as downloading firmware file only >>>>\n");
#endif
    BECEEM_MEMFREE(pstRequestMessage) return 0;	/* iRet; */
}

VOID WiMAXCommonAPI_ChipsetResponse(PWIMAX_MESSAGE pWiMaxMsg)
{
    PFIRMWARE_STATUS_INDICATION pFm = { 0 };
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMaxMsg)
	return;

    switch (pWiMaxMsg->usSubType)
	{
	  case SUBTYPE_RESET_INDICATION:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n~~~~~~~~~~ RESET INDICATION RECEIVED ~~~~~~~~~~\n");
	      break;
	  case SUBTYPE_FIRMWARE_DNLD_STATUS_RESP:
	      {
		  DPRINT_COMMONAPI(DEBUG_MESSAGE,
				   "\n~~~~~~~~~~ FIRMWARE DOWNLOAD STATUS RECEIVED ~~~~~~~~~~\n");
		  pFm = (PFIRMWARE_STATUS_INDICATION) pWiMaxMsg->szData;
		  DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n Firmware Download Status : %d",
				   pFm->u32FirmwareDownloadStatus);
		  if (!pFm->u32FirmwareDownloadStatus)
		      {
			  Event_Set(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload));
		      }

		  if (pFm->stFirmWareInfo.u32ImageType == IMAGE_CONFIG_FILE)
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n Configuration File : %s",
				       pFm->stFirmWareInfo.szImageName);
		  else if (pFm->stFirmWareInfo.u32ImageType == IMAGE_FIRMWARE)
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n Firmware File : %s",
				       pFm->stFirmWareInfo.szImageName);
		  else
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n Invalid file type set");

		  DPRINT_COMMONAPI(DEBUG_MESSAGE,
				   "\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	      }
	      break;
	}
}

#if 1
int WiMAXCommonAPI_LaunchSearchThread(PTHREADFUNC pSearchFunction)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    pWiMAX->m_bKillWimaxCommonAPI_SearhThread = FALSE;
    if (Thread_IsValidThread(&pWiMAX->m_WiMAXCommonAPI_NwSearch))
	return SUCCESS;

    if (Thread_Run(&(pWiMAX->m_WiMAXCommonAPI_NwSearch), (PTHREADFUNC) pSearchFunction, pWiMAX))
	{
	    pWiMAX->m_bKillWimaxCommonAPI_SearhThread = FALSE;
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Net search thread failed !!!");
	    return ERROR_THREAD_NOT_STARTED;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Net search thread started");
    return SUCCESS;
}

int WiMAXCommonAPI_KillSearchThread()
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    pWiMAX->m_bKillWimaxCommonAPI_SearhThread = TRUE;
#ifndef WIN32
    usleep(300);
#else
    Sleep(3);
#endif

    return 0;
}


VOID WiMAXCommonAPI_IssueWideScanSearch(UINT32 u32FirstFreq, UINT32 u32LastFreq, UINT32 u32Step)
{
    PST_NETWORK_SEARCH_PARAMS pstNetSearchParams = NULL;
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    UINT32 u32Len = 0;
    INT32 iRet = 0;

    BECEEMCALLOC_NOT_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	pstNetSearchParams = (PST_NETWORK_SEARCH_PARAMS) pstRequestMessage->szData;
    pstNetSearchParams->u32ReportType = 1;
    pstNetSearchParams->u32Duration = 0;
    pstNetSearchParams->stContCFRange.u32CenterFreqStart = u32FirstFreq;
    pstNetSearchParams->stContCFRange.u32CenterFrequencyEnd = u32LastFreq;
    pstNetSearchParams->stContCFRange.u32CenterFrequencyStep = u32Step;

    pstNetSearchParams->stBandwidthList.u32NumOfBwsToScan = NUMBER_OF_BW_TOSCAN;
    pstNetSearchParams->stBandwidthList.au32BandwidthList[0] = BW1_FOR_WIDESCAN_SEARCH;
    pstNetSearchParams->stBandwidthList.au32BandwidthList[1] = BW2_FOR_WIDESCAN_SEARCH;

    u32Len =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_NETWORK_SEARCH, SUBTYPE_NETWORK_SEARCH_REQ,
				    sizeof(ST_NETWORK_SEARCH_PARAMS));
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, u32Len);
BECEEM_MEMFREE(pstRequestMessage)}

INT WiMAXCommonAPI_NwSearchPreconditioning()
{
    WIMAX_MESSAGE stRequestMessage = { 0 };
    INT iLen = 0, iRet = 0, iWaitRet = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse));
    iLen = WiMAXCommonAPI_FillWiMAXMsg(&stRequestMessage, TYPE_LINK_DOWN, SUBTYPE_SYNC_DOWN_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) & stRequestMessage, iLen);
    pDeviceData->u32RxvdSyncRsp = FALSE;
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "SUBTYPE_SYNC_DOWN_REQ returned [%d]", iRet);
	    goto ret_path;
	}
    iWaitRet = Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_WaitForLinkStatusResponse), SYNC_DOWN_REQ_TIMEOUT);
    if (WAIT_TIMEOUT == iWaitRet)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "m_WiMAXCommonAPI_WaitForLinkStatusResponse timeed out");
	    iRet = WAIT_TIMEOUT;
	}

  ret_path:
    return iRet;
}

int WiMAXCommonAPI_AbortSearch()
{
    WIMAX_MESSAGE stRequestMessage = { 0 };
    INT32 iRet = 0, iLen = 0, iWaitRet = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse));
    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_SEARCH,
				    SUBTYPE_NETWORK_SEARCH_ABORT_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) & stRequestMessage, iLen);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, " SUBTYPE_NETWORK_SEARCH_ABORT_REQ returned [%d]", iRet);
	    goto ret_path;
	}
    pDeviceData->stCaplSearchDetails.u8Valid = FALSE;
    memset(&pDeviceData->stCaplSearchDetails, 0, sizeof(ST_SEARCH_REQ_DETAILS));
    iWaitRet = Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_WaitForAbortResponse), ABORT_SEARCH_TIMEOUT);
    if (WAIT_TIMEOUT == iWaitRet)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "m_WiMAXCommonAPI_WaitForAbortResponse timeed out");
	    iRet = WAIT_TIMEOUT;
	}

  ret_path:
    return iRet;
}

int WiMAXCommonAPI_SendOMADMGetRequest(POMADM_OBJECT_INFO pOMADMReq)
{
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    int iLen = 0, iRetval = 0;

    BECEEMCALLOC(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_OMADM, SUBTYPE_OMADM_GET_REQ,
				    sizeof(OMADM_OBJECT_INFO));
    memcpy(pstRequestMessage->szData, pOMADMReq, sizeof(OMADM_OBJECT_INFO));

    iRetval = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);
    BECEEM_MEMFREE(pstRequestMessage) return iRetval;
}

UINT WiMAXCommonAPI_GetOMADMParameterSize(POMA_DM_TREE_LEVEL pstTestLevel, UINT uiLevel)
{
    POMADM_PARAMETER_INFO pstOMAParaminfo = NULL;
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    int iLen = 0, iRet = 0, iRetval = 0;

    BECEEMCALLOC_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1, UINT)
	iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_OMADM, SUBTYPE_OMADM_GET_PARAM_SIZE_REQ,
				    sizeof(OMADM_PARAMETER_INFO));

    pstOMAParaminfo = (POMADM_PARAMETER_INFO) pstRequestMessage->szData;
    memcpy(&pstOMAParaminfo->stTreeLevel, pstTestLevel, uiLevel * sizeof(OMA_DM_TREE_LEVEL));

    pstOMAParaminfo->u32NumOfLevel = uiLevel;

    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);
    if (iRet == SUCCESS)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, " Parameter size get returned [%d]",
			     pstOMAParaminfo->u32OMADMObjSize);

	    iRetval = (UINT) pstOMAParaminfo->u32OMADMObjSize;
	    goto returnCode;
	}
    else
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error Code Returned: [%d]", iRet);
	    iRetval = 0;
	    goto returnCode;
	}
  returnCode:
    BECEEM_MEMFREE(pstRequestMessage) return iRetval;


}

void WiMAXCommonAPI_GetProvisionedState(PUINT32 pActivated)
{
    POMADM_OBJECT_INFO pstOMADMObj = NULL;
    int iLen = 0;
    UINT uiParameterSize = 0;
    int iRet = 0;
    PWIMAX_MESSAGE pstRequestMessage = NULL;

    OMA_DM_TREE_LEVEL stTestLevel[] = { {OMA_DM_ST_ROOT_WIMAXSUPP, 0},
    {OMA_DM_ST_WIMAXSUPP_OPERATOR, 0},
    {OMA_DM_ST_OPERATOR_SUBSCRIPTION_PARAMETERS, 0},
    {OMA_DM_ST_SUBSCRIPTION_PARAMETERS_PRIMARY, 0},
    {OMA_DM_ST_PRIMARY_ACTIVATED, 0}
    };
    BECEEMCALLOC_NOT_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	uiParameterSize =
	WiMAXCommonAPI_GetOMADMParameterSize((POMA_DM_TREE_LEVEL) stTestLevel,
					     sizeof(stTestLevel) / sizeof(stTestLevel[0]));

    if (uiParameterSize == 0)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error in getting the param size");
	    goto returnCode;
	}

    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_OMADM, SUBTYPE_OMADM_GET_REQ,
				    sizeof(OMADM_OBJECT_INFO));

    pstOMADMObj = (POMADM_OBJECT_INFO) pstRequestMessage->szData;
    pstOMADMObj->u32NumOfLevel = sizeof(stTestLevel) / sizeof(stTestLevel[0]);

    memcpy(&pstOMADMObj->stTreeLevel, &stTestLevel,
	   pstOMADMObj->u32NumOfLevel * sizeof(OMA_DM_TREE_LEVEL));
    pstOMADMObj->u32OMADMObjSize = uiParameterSize;
    memset(pstOMADMObj->au8OMADMObjVal, 0, pstOMADMObj->u32OMADMObjSize);

    iLen += uiParameterSize;
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);

    if (iRet < 0)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error Received as: [%d]", iRet);
	    goto returnCode;
	}

    memcpy(pActivated, pstOMADMObj->au8OMADMObjVal, sizeof(UINT32));
  returnCode:
    BECEEM_MEMFREE(pstRequestMessage) return;

}

int WiMAXCommonAPI_GetNetworkParameterNode()
{
    POMADM_OBJECT_INFO pstOMADMObj = NULL;
    int iLen = 0;
    UINT uiParameterSize = 0;
    int iRet = 0;
    UINT uiOperatorIndex = 0;
    PWIMAX_MESSAGE pstRequestMessage = NULL;

    OMA_DM_TREE_LEVEL stTestLevel[] = { {OMA_DM_ST_ROOT_WIMAXSUPP, 0},
    {OMA_DM_ST_WIMAXSUPP_OPERATOR, 0},
    {OMA_DM_ST_OPERATOR_NETWORK_PARAMETERS, 0},
    };
    if (pDeviceData->pOperatorChannelPlanSettings)
	{
	    pDeviceData->bOMADMInitFlag = TRUE;
	    return SUCCESS;
	}
    BECEEMCALLOC(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	for (uiOperatorIndex = 0; uiOperatorIndex < WIMAXSUPP_OPERATOR_NUMBER_OF_NODE;
	     uiOperatorIndex++)
	{
	    stTestLevel[1].u32FieldInstance = uiOperatorIndex;
	    uiParameterSize =
		WiMAXCommonAPI_GetOMADMParameterSize((POMA_DM_TREE_LEVEL) stTestLevel,
						     sizeof(stTestLevel) / sizeof(stTestLevel[0]));

	    if (uiParameterSize == 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error in getting the param size");
		    break;
		}

	    iLen =
		WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_OMADM, SUBTYPE_OMADM_GET_REQ,
					    sizeof(OMADM_OBJECT_INFO));

	    pstOMADMObj = (POMADM_OBJECT_INFO) pstRequestMessage->szData;
	    pstOMADMObj->u32NumOfLevel = sizeof(stTestLevel) / sizeof(stTestLevel[0]);

	    memcpy(&pstOMADMObj->stTreeLevel, &stTestLevel,
		   pstOMADMObj->u32NumOfLevel * sizeof(OMA_DM_TREE_LEVEL));
	    pstOMADMObj->u32OMADMObjSize = uiParameterSize;
	    memset(pstOMADMObj->au8OMADMObjVal, 0, pstOMADMObj->u32OMADMObjSize);

	    iLen += uiParameterSize;
	    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);

	    if (iRet < 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Error Received as: [%d]", iRet);
		}

	    if (sizeof(ST_NetworkParameters) == pstOMADMObj->u32OMADMObjSize)
		{
		    memset(&pDeviceData->stNetworkParameters[uiOperatorIndex], 0,
			   sizeof(ST_NetworkParameters));
		    memcpy(&pDeviceData->stNetworkParameters[uiOperatorIndex],
			   pstOMADMObj->au8OMADMObjVal, sizeof(ST_NetworkParameters));
		}
	    else
		{
		    memset(&pDeviceData->stNetworkParameters, 0, sizeof(ST_NetworkParameters));
		    DPRINT_COMMONAPI(DEBUG_MESSAGE,
				     "Error in getting the Network paramenter structure instance");
		}
	}
    pDeviceData->bOMADMInitFlag = TRUE;
    BECEEM_MEMFREE(pstRequestMessage) return SUCCESS;
}

INT WiMAXCommonAPI_CheckIfBCAPIIsUsedByApp()
{

    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    HANDLE hDevice = NULL;

    hDevice = DeviceIO_GetDeviceHandle(&(pWiMAX->m_Device));
    if ((hDevice == INVALID_HANDLE_VALUE) || (hDevice == pDeviceData->hDevice))
	return SUCCESS;

    return -1;			/*Application is using the other BAPI interface */
}

VOID WiMAXCommonAPI_SearchWidescanResponse(PWIMAX_MESSAGE pWiMaxMsg)
{
    UINT32 u32OperatorIndex = 0, u32PriorityIndex = 0, u32CAPLIndex = 0, u32NAPID = 0, u32Index = 0;
    PBSINFO pBaseInfo = 0;
    UINT uiNumOfBaseStations = 0, i = 0;
    BOOL bFoundBSForCAPL = FALSE;
    PBSLIST pstBSList = (PBSLIST) pWiMaxMsg->szData;

    uiNumOfBaseStations = pstBSList->u32NumBaseStationsFound;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n%d INSIDE SEARCH WIDESCAN PROCESS.. BASE STATIONS FOUND.\n",
		     uiNumOfBaseStations);

    if (pstBSList->u32SearchCompleteFlag == 1)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "SEARCH IS COMPLETE!!");
	    pDeviceData->bOnWideScanSearch = FALSE;
	    return;
	}
    pBaseInfo = pstBSList->stBSInformation;

    for (i = 0; i < (int) uiNumOfBaseStations; i++, pBaseInfo++)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBASE STATION : %d\n-------------------\n", i + 1);
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBS ID  :%02X %02X %02X %02X %02X %02X %02X",
			     pBaseInfo->aucBSID[1], pBaseInfo->aucBSID[2], pBaseInfo->aucBSID[3],
			     pBaseInfo->aucBSID[4], pBaseInfo->aucBSID[5], pBaseInfo->aucBSID[6],
			     pBaseInfo->aucBSID[7]);

	    if (pBaseInfo->aucBSID[0])
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, " - COMPRESSED MAP");
		}
	    else
		DPRINT_COMMONAPI(DEBUG_MESSAGE, " - NORMAL MAP");

	    u32Index = 2;
	    while (u32Index < 5)
		{
		    u32NAPID |= pBaseInfo->aucBSID[u32Index];
		    u32NAPID = u32NAPID << 8;
		    u32Index++;
		}
	    u32NAPID = u32NAPID >> 8;

	    for (u32OperatorIndex = 0; u32OperatorIndex < WIMAXSUPP_OPERATOR_NUMBER_OF_NODE;
		 u32OperatorIndex++)
		{
		    for (u32PriorityIndex = 0; u32PriorityIndex < MAX_PRIORITY_POSSIBLE;
			 u32PriorityIndex++)
			{
			    for (u32CAPLIndex = 0;
				 u32CAPLIndex <
				 pDeviceData->pPrioritizedCAPL_LISTFor_WideScan[u32PriorityIndex].
				 u32Count; u32CAPLIndex++)
				{
				    if (memcmp
					(&pDeviceData->
					 pPrioritizedCAPL_LISTFor_WideScan[u32PriorityIndex].
					 stCAPLENTRIES[u32CAPLIndex].NAP_ID, &u32NAPID,
					 sizeof(UINT)) == 0)
					{
					    UINT32 index = 0;
					    PBSINFO pBSINFO_Dest = NULL;

					    DPRINT_COMMONAPI(DEBUG_MESSAGE,
							     " BASE STATION FOUND FOR THE PRESENT NAP");

					    index =
						pDeviceData->stNwSearchResp[u32CAPLIndex].
						u32TotalBaseStationsFound;
					    pBSINFO_Dest =
						&(pDeviceData->stNwSearchResp[u32CAPLIndex].
						  stBSINFO_Entries[index]);

					    memcpy(pDeviceData->stNwSearchResp[index].OperatorName,
						   pDeviceData->stNetworkParameters[u32CAPLIndex].
						   OperatorName, MAX_SIZE_OPERATORNAME);
					    pDeviceData->stNwSearchResp[index].u32NSPID =
						pDeviceData->stNetworkParameters[u32PriorityIndex].
						H_NSP[0].H_NSP_ID;

					    if (pDeviceData->u32TotalOperatorsFound <
						WIMAXSUPP_OPERATOR_NUMBER_OF_NODE)
						pDeviceData->u32TotalOperatorsFound++;

					    pDeviceData->stNwSearchResp[pDeviceData->
									stCAPL_NAPID_IN_SEARCH.
									u32PRESENT_CAPL_ENTRY_IN_SEARCH].
						u32NAP_ID = u32NAPID;
					    memcpy(pBSINFO_Dest, pBaseInfo, sizeof(BSINFO));

					    pDeviceData->stNwSearchResp[u32CAPLIndex].
						u32TotalBaseStationsFound++;

					    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nPreamble Index ID  :%d",
							     pBaseInfo->u32PreambleIndex);
					    DPRINT_COMMONAPI(DEBUG_MESSAGE,
							     "\nCenter Frequency : %d kHz",
							     pBaseInfo->u32CenterFrequency);
					    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nBandwidth : %d kHz",
							     pBaseInfo->u32Bandwidth);
					    DPRINT_COMMONAPI(DEBUG_MESSAGE,
							     "\nRelative Signal Strength : %d dBm",
							     pBaseInfo->s32RelativeSignalStrength);
					    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nCINR : %d dB",
							     pBaseInfo->s32CINR);

					    bFoundBSForCAPL = TRUE;
					    WiMAXCommonAPI_AbortSearch();
					}
				}
			}
		}
	}

    if (pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearchWideScan && bFoundBSForCAPL)
	{
	    /*Call callback here */
	    WiMAXCommonAPI_CallWideScanCallback();
	}
}

int WiMAXCommonAPI_PrioritizedCAPL_for_WideScan()
{
    UINT32 u32PriorityIndex = 0;

    UINT32 u32CAPLIndex = 0, u32OperatorIndex = 0;

    if (!pDeviceData->pPrioritizedCAPL_LISTFor_WideScan)
	{
	BECEEMCALLOC(pDeviceData->pPrioritizedCAPL_LISTFor_WideScan, PPRIORITIZED_CAPL_ENTRIES_LIST,
			 PRIORITIZED_CAPL_ENTRIES_LIST, MAX_PRIORITY_POSSIBLE)}


    for (u32OperatorIndex = 0; u32OperatorIndex < WIMAXSUPP_OPERATOR_NUMBER_OF_NODE; u32OperatorIndex++)
	{
	    for (u32CAPLIndex = 0; u32CAPLIndex < MAX_CAPL_ENTRIES_PER_NSP; u32CAPLIndex++)
		{
		    if (pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.Entries[u32CAPLIndex].
			Priority > 250)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Not a valid CAPL entry priority!!");
			    continue;
			}
		    switch (pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
			    Entries[u32CAPLIndex].Priority)
			{
			  case 0:
			      //0 is not a valid priority
			      break;

			  default:
			      u32PriorityIndex =
				  pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
				  Entries[u32CAPLIndex].Priority - 1;
			      memcpy(&pDeviceData->pPrioritizedCAPL_LISTFor_WideScan[u32PriorityIndex].
				     stCAPLENTRIES[pDeviceData->
						   pPrioritizedCAPL_LISTFor_WideScan[u32PriorityIndex].
						   u32Count],
				     &pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
				     Entries[u32CAPLIndex], sizeof(ST_CAPLEntries));
			      pDeviceData->pPrioritizedCAPL_LISTFor_WideScan[u32PriorityIndex].
				  u32Count++;
			      break;
			}
		}
	}
    return SUCCESS;
}

VOID WiMAXCommonAPI_CallWideScanCallback()
{
    UINT uiIndex = 0, uiBSInfoIndex = 0;
    UINT8 RSSI = 0;
    UINT8 CINR = 0;
    WIMAX_API_NSP_INFO NspList[WIMAXSUPP_OPERATOR_NUMBER_OF_NODE];
    UINT32 u32ListSize = 0, u32UnicodeOperatorLength = 0;

    memset(NspList, 0, sizeof(WIMAX_API_NSP_INFO) * WIMAXSUPP_OPERATOR_NUMBER_OF_NODE);

    for (uiIndex = 0; uiIndex < pDeviceData->u32TotalOperatorsFound; uiIndex++)
	{
	    ConvertToUnicode((char *) pDeviceData->stNwSearchResp[uiIndex].OperatorName,
			     NspList[uiIndex].NSPName, &u32UnicodeOperatorLength);

	    NspList[uiIndex].NSPid = pDeviceData->stNwSearchResp[uiIndex].u32NSPID;
	    NspList[uiIndex].networkType = WIMAX_API_HOME;

	    CINR = (CFG_UINT8) (pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[0].s32CINR + 10);
	    RSSI =
		(CFG_UINT8) (pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[0].
			     s32RelativeSignalStrength + 123);

	    for (uiBSInfoIndex = 0; uiBSInfoIndex <
		 pDeviceData->stNwSearchResp[uiIndex].u32TotalBaseStationsFound; uiBSInfoIndex++)

		{
		    if (CINR <
			(pDeviceData->stNwSearchResp[uiIndex].stBSINFO_Entries[uiBSInfoIndex].s32CINR +
			 10))
			{
			    CINR =
				(CFG_UINT8) (pDeviceData->stNwSearchResp[uiIndex].
					     stBSINFO_Entries[uiBSInfoIndex].s32CINR + 10);
			    RSSI =
				(CFG_UINT8) (pDeviceData->stNwSearchResp[uiIndex].
					     stBSINFO_Entries[uiBSInfoIndex].s32RelativeSignalStrength +
					     123);
			}

		}
	    NspList[uiIndex].CINR = CINR;
	    NspList[uiIndex].RSSI = RSSI;
	}

    u32ListSize = pDeviceData->u32TotalOperatorsFound;

    if (pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearchWideScan)
	{
	    IndNetworkSearchWideScan pIndNetworkSearchWideScan = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndNetworkSearchWideScan =
		pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearchWideScan;
	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndNetworkSearchWideScan)
		pIndNetworkSearchWideScan(&pDeviceData->stDeviceID, NspList, u32ListSize);
	}
}

VOID WiMAXCommonAPI_DoDummySearch()
{
    PST_NETWORK_SEARCH_PARAMS pstNetSearchParams = { 0 };
    PWIMAX_MESSAGE pstRequestMessage = NULL;

    UINT32 u32Len = 0;
    INT32 iRet = 0;

    memset(&pDeviceData->stCAPL_NAPID_IN_SEARCH, 0, sizeof(PRESENT_CAPL_NAPID_IN_SEARCH));
    memset(&pDeviceData->stNwSearchResp, 0, sizeof(NW_SEARCH_RESP) * MAX_CAPL_ENTRIES_PER_NSP);
    BECEEMCALLOC_NOT_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1)
	u32Len =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_NETWORK_SEARCH, SUBTYPE_NETWORK_SEARCH_REQ,
				    sizeof(ST_NETWORK_SEARCH_PARAMS));
    pstNetSearchParams = (PST_NETWORK_SEARCH_PARAMS) pstRequestMessage->szData;
    pstNetSearchParams->u32ReportType = 1;
    pstNetSearchParams->u32Duration = 0;
    pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq = 1;
    pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[0] = FREQBAND_2_3_RANGE_START;	/*Invalid frequency just to wake up the device */
    pstNetSearchParams->stBandwidthList.u32NumOfBwsToScan = 1;
    pstNetSearchParams->stBandwidthList.au32BandwidthList[0] = 7000;

    /*inititate search to wake up modem */
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, u32Len);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Network search request returned %d", iRet);
	}

    /*Abort search */
    u32Len =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_NETWORK_SEARCH,
				    SUBTYPE_NETWORK_SEARCH_ABORT_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, u32Len);
BECEEM_MEMFREE(pstRequestMessage)}

LINK_ERROR_CODES WiMAXCommonAPI_GetLinkStatus()
{
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    INT iLen = 0, iRet = 0;

    BECEEMCALLOC_RET(pstRequestMessage, PWIMAX_MESSAGE, WIMAX_MESSAGE, 1, LINK_ERROR_CODES)
	iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_NETWORK_ENTRY,
				    SUBTYPE_NETWORK_ENTRY_STATUS_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);
    if (iRet != SUCCESS)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, " GetLinkStatus request failed");
	}
    BECEEM_MEMFREE(pstRequestMessage) return pDeviceData->stLinkStatus.eStatusCode;
}
#endif

PVOID WiMAXCommonAPI_CaplSearchThread(PVOID pData)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32PeriodicWakeupTimeout = 0;
    UINT32 u32MIBVersion = 0;
    int iSize = 0, iWaitRet = 0;
    UINT32 bWhileLoop = 1;

    UNUSED_ARGUMENT(pData);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered Thread %s and %d", __FUNCTION__, __LINE__);
    while (!pDeviceData->bOMADMInitFlag && !pWiMAX->m_bKillWimaxCommonAPI_SearhThread)
	{
#ifdef WIN32
	    Sleep(100);
#else
	    usleep(100000);
#endif
	}
#if 1

    while (bWhileLoop)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Inside CAPL Search thread");
	    if ((pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_RF_OFF_SW))
		{
		    iSize =
			CFG_ParamSize((CFG_UINT8 *) pWiMAX->pEEPROMContextTable, CFG_MIB_VERSION_ID);
		    CFG_ReadParam((CFG_UINT8 *) pWiMAX->pEEPROMContextTable,
				  (CFG_UINT8 *) & u32MIBVersion, iSize, CFG_MIB_VERSION_ID);
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);

		    if (u32MIBVersion > PERIODIC_TIMER_SUPPORTED_MIB_VER)
			{
			    CFG_ReadParam((CFG_UINT8 *) pWiMAX->pEEPROMContextTable,
					  (CFG_UINT8 *) & u32PeriodicWakeupTimeout,
					  pWiMAX->m_sMIBTable[BCM_CFG_NETSEARCH_PERIODIC_WAKEUP_TIMER].
					  u32FieldSize, CFG_MIB_BCMCONFIGNETSEARCHPERIODICWAKEUP_ID);

			    if (u32PeriodicWakeupTimeout < MINIMUM_SHUTDOWN_TIMEOUT)
				u32PeriodicWakeupTimeout = MINIMUM_SHUTDOWN_TIMEOUT;
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
			}
		    else
			u32PeriodicWakeupTimeout = DEFAULT_SHUTDOWN_TIMEOUT;
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "u32PeriodicWakeupTimeout %d",
				     u32PeriodicWakeupTimeout);


		}

	    DPRINT_COMMONAPI(DEBUG_MESSAGE,
			     "CAPL Search thread m_WiMAXCommonAPI_StartCaplSearchRequest waiting to be set");
	    iWaitRet =
		Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest),
			   u32PeriodicWakeupTimeout);
	    if (WAIT_TIMEOUT == iWaitRet)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE,
				     "Event Wait timeouted for CAPL search periodic wake up timer value is %d",
				     u32PeriodicWakeupTimeout);
		}
	    {
		Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
		Mutex_Lock(&(pWiMAX->m_CAPINWSearchMutex));
		if (TRUE == pWiMAX->m_bKillWimaxCommonAPI_SearhThread)
		    {
			pWiMAX->m_bKillWimaxCommonAPI_SearhThread = FALSE;
			Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
			DPRINT_COMMONAPI(DEBUG_MESSAGE, "Exiting Network Search Thread\n");
			Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
			return SUCCESS;
		    }
#ifdef WIN32
		Sleep(100);
#else
		usleep(100000);
#endif
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError();
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		if ((pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Connecting)
		    || (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Data_Connected)
		    || pDeviceData->u32RxvdCmdConnectToNetwork)
		    {
			DPRINT_COMMONAPI(DEBUG_MESSAGE, "Not required to send CAPL search request\n");
			DPRINT_COMMONAPI(DEBUG_MESSAGE,
					 "eDeviceStatus %d and u32RfOffFromUser %d and u32RxvdCmdConnectToNetwork %d",
					 pDeviceData->eDeviceStatus, pDeviceData->u32RfOffFromUser,
					 pDeviceData->u32RxvdCmdConnectToNetwork);


			pDeviceData->u32SearchComplete = TRUE;
			Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
			continue;
		    }

		//
		switch (pDeviceData->u32ConnectionStatusUpdate)
		    {
		      case LINKUP_DONE:
		      case LINKUP_IN_PROGRESS:
		      case PHY_SYNC_ACHIVED:
		      case EAP_AUTH_USER_IN_PROGRESS:
		      case SATEK_IN_PROGRESS:
		      case REGISTRATION_IN_PROGRESS:
			  DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s connect status is %d ", __FUNCTION__,
					   pDeviceData->u32ConnectionStatusUpdate);
			  pDeviceData->u32SearchComplete = TRUE;
			  Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
			  continue;

		    }
		if (pDeviceData->u32RfOffFromUser)
		    {
			switch (pDeviceData->eDeviceStatus)
			    {
			      case WIMAX_API_DEVICE_STATUS_RF_OFF_HW:
			      case WIMAX_API_DEVICE_STATUS_RF_OFF_SW:
			      case WIMAX_API_DEVICE_STATUS_RF_OFF_HW_SW:
				  DPRINT_COMMONAPI(DEBUG_MESSAGE, "RF OFF From user don't search");
				  pDeviceData->u32SearchComplete = TRUE;
				  Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
				  continue;

			      default:
				  DPRINT_COMMONAPI(DEBUG_MESSAGE, "RF OFF From user started");
				  pDeviceData->u32SearchComplete = TRUE;
				  Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
				  continue;
			    }
		    }

		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		pDeviceData->u32SearchComplete = FALSE;

		Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
		WiMAXCommonAPI_NwSearchPreconditioning();
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
		WiMAXCommonAPI_PopulateNetworkSearchReq();
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		if (TRUE == pDeviceData->u32SearchComplete)
		    {
			DPRINT_COMMONAPI(DEBUG_MESSAGE, "CAPL Search thread u32SearchComplete is TRUE");
			/* If search Request does not contain any frequncies */
			Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
			continue;
		    }
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete), MRUSEARCH_TIMEOUT);
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    pDeviceData->u32SearchComplete = TRUE;
		    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));

		    if (TRUE == pWiMAX->m_bKillWimaxCommonAPI_SearhThread)
			{
			    pWiMAX->m_bKillWimaxCommonAPI_SearhThread = FALSE;
			    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Exiting Network Search Thread\n");
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
			    Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));
			    return SUCCESS;
			}
		    if (pDeviceData->stCaplSearchRsp.u32NumBS == 0)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Num BS detected is zero\n");
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Sending the next search request\n");
			    WiMAXCommonAPI_StartSearch();
			}
		}
		DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));

	    }			/*Event_Wait(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest) */
	}			/* End of while */
#endif
    return 0;
}

INT WiMAXCommonAPI_SetCAPICAPLListPriority()
{
    UINT32 u32CAPLIndex = 0, u32OperatorIndex = 0;
    UINT32 u32PriorityIndex = 0;
    PST_NetworkParameters pNetworkParams = NULL;


    for (u32OperatorIndex = 0;
	 u32OperatorIndex < pDeviceData->pOperatorChannelPlanSettings->u32NumOfNetworkParamFields;
	 u32OperatorIndex++)
	{
	    pNetworkParams =
		&pDeviceData->pOperatorChannelPlanSettings->astNetworkParameters[u32OperatorIndex];

	    pDeviceData->stNetworkParameters[0].CAPL.Any =
		pDeviceData->pOperatorChannelPlanSettings->astNetworkParameters[0].CAPL.Any;
	    memcpy(pDeviceData->stNetworkParameters[0].OperatorName,
		   pDeviceData->pOperatorChannelPlanSettings->astNetworkParameters[0].OperatorName,
		   MAX_SIZE_OPERATORNAME);

	    for (u32CAPLIndex = 0; u32CAPLIndex < MAX_CAPL_ENTRIES_PER_NSP; u32CAPLIndex++)
		{
		    if (pNetworkParams->CAPL.Entries[u32CAPLIndex].Priority > 250)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Not a valid CAPL entry priority!!");
			    continue;
			}
		    switch (pNetworkParams->CAPL.Entries[u32CAPLIndex].Priority)
			{
			  case 0:
			      //0 is not a valid priority
			      break;

			  default:
			      u32PriorityIndex =
				  pNetworkParams->CAPL.Entries[u32CAPLIndex].Priority - 1;
			      memcpy(&pDeviceData->pPrioritizedCAPL[u32PriorityIndex].
				     stCAPLENTRIES[pDeviceData->pPrioritizedCAPL[u32PriorityIndex].
						   u32Count],
				     &pNetworkParams->CAPL.Entries[u32CAPLIndex],
				     sizeof(ST_CAPLEntries));
			      pDeviceData->pPrioritizedCAPL[u32PriorityIndex].u32Count++;
			      break;
			}
		}
	}
    return SUCCESS;
}

int WiMAXCommonAPI_InitCAPLTable()
{
    UINT32 u32CAPLIndex = 0, u32OperatorIndex = 0;
    UINT32 u32PriorityIndex = 0;

    if (!pDeviceData->pPrioritizedCAPL)
	{
	BECEEMCALLOC(pDeviceData->pPrioritizedCAPL, PPRIORITIZED_CAPL_ENTRIES_LIST,
			 PRIORITIZED_CAPL_ENTRIES_LIST, MAX_PRIORITY_POSSIBLE)}

    if (pDeviceData->pOperatorChannelPlanSettings)
	return WiMAXCommonAPI_SetCAPICAPLListPriority();

    for (u32OperatorIndex = 0; u32OperatorIndex < WIMAXSUPP_OPERATOR_NUMBER_OF_NODE; u32OperatorIndex++)
	{
	    for (u32CAPLIndex = 0; u32CAPLIndex < MAX_CAPL_ENTRIES_PER_NSP; u32CAPLIndex++)
		{
		    if (pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.Entries[u32CAPLIndex].
			Priority > 250)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Not a valid CAPL entry priority!!");
			    continue;
			}
		    switch (pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
			    Entries[u32CAPLIndex].Priority)
			{
			  case 0:
			      //0 is not a valid priority
			      break;

			  default:
			      u32PriorityIndex =
				  pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
				  Entries[u32CAPLIndex].Priority - 1;
			      memcpy(&pDeviceData->pPrioritizedCAPL[u32PriorityIndex].
				     stCAPLENTRIES[pDeviceData->pPrioritizedCAPL[u32PriorityIndex].
						   u32Count],
				     &pDeviceData->stNetworkParameters[u32OperatorIndex].CAPL.
				     Entries[u32CAPLIndex], sizeof(ST_CAPLEntries));
			      pDeviceData->pPrioritizedCAPL[u32PriorityIndex].u32Count++;
			      break;
			}
		}
	}
    return SUCCESS;
}

VOID WiMAXCommonAPI_PopulateNetworkSearchReq()
{
    UINT32 u32Len = 0;
    INT32 iRet = 0;
    WIMAX_MESSAGE stRequestMessage = { 0 };
    PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams = NULL;


    u32Len = WiMAXCommonAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_SEARCH,
					 SUBTYPE_CAPL_BASED_SEARCH_REQ,
					 sizeof(ST_CAPL_BASED_SEARCH_PARAMS));
    pstCaplSearchParams = (PST_CAPL_BASED_SEARCH_PARAMS) stRequestMessage.szData;
    pstCaplSearchParams->u32ReportType = 2;
    pstCaplSearchParams->u32Duration = 0;
    pstCaplSearchParams->u32Flags = 0;

    pstCaplSearchParams->iCinrThreshold = pDeviceData->s32CINRThreshold;
    pstCaplSearchParams->iRssiThreshold = pDeviceData->s32RSSIThreshold;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    /* Read the Head Index each time CAPL search request is issued */
    /* Get the Head Index */
#ifdef ENABLE_MRU_UPDATES	//diabled since MRU updates can cause DSD data corruption
    CFG_ReadParam((CFG_UINT8 *) pWiMAX->pEEPROMContextTable, (B_UINT8 *) & u8HeadIndex,
		  sizeof(u8HeadIndex), CFG_MRU_HEAD_INDEX_ID);

    if (u8HeadIndex == TAIL_MRU_INDEX)
	WiMAXCommonAPI_ResetMRUTable(u8MruResetFlag);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    if (!pDeviceData->stCaplSearchDetails.u8Valid)
	{
	    WimaxCommonAPI_ResetFlag();
	    if (NULL != pDeviceData->MruTable.mruHead)
		{
		    u32MruTableIndex = pDeviceData->MruTable.mruHead->u8CurrIndex;
		}
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	}
    else
	{
	    u32MruTableIndex = pDeviceData->stCaplSearchDetails.u32MruTableIndex;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    /* Fill MRU Table Details */
    if (!pDeviceData->stCaplSearchDetails.u8MruSearchDone
	&& (pDeviceData->MruTable.u32NumMruEntries > 0))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Filling MRU details\n");
	    pDeviceData->stCaplSearchDetails.u8Valid = FALSE;

	    pMruEntry = &pDeviceData->MruTable.stMruEntries[u32MruTableIndex];
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    while (pMruEntry != NULL)
		{
		    if ((u32NumChannels + NUM_CHANNELS_PER_CHANNELPLAN) >= MAX_NUM_REQUESTS())
			{
			    pDeviceData->stCaplSearchDetails.u32MruTableIndex = pMruEntry->u8CurrIndex;
			    pDeviceData->stCaplSearchDetails.u8Valid = TRUE;
			    break;
			}
		    u8ChannelPlanIndex = pMruEntry->u8ChannelPlanIndex;
		    /* Get Channel Plan */
		    pstChannelPlan = &(pDeviceData->pstChannelPlan[u8ChannelPlanIndex].Entries);
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Channel Plan Index Included from MRU Table:%d",
				     u8ChannelPlanIndex);
		    /* Set Flag to indicate this channel plan is included in request */
		    pDeviceData->pstChannelPlan[u8ChannelPlanIndex].bFlagIncludedInSearchReq = TRUE;
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    /* Fill Primary Frequency details */
		    if ((pMruEntry->u32PrimaryFrequency > 0) &&
			!WiMAXCommonAPI_CheckIfBlackListedNw(pMruEntry->u32PrimaryFrequency))
			{

			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32CenterFreq = pMruEntry->u32PrimaryFrequency;
			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32Bandwidth = pstChannelPlan->BW;
			    memcpy(pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				   au32Preamble, pstChannelPlan->Preambles,
				   (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
			    /* Update Mapping Info */
			    pDeviceData->u8ChPlanIndxMapping[u32NumChannels] = u8ChannelPlanIndex;
			    u32NumChannels++;
			}
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    if ((pMruEntry->u32Secondary1Frequency > 0) &&
			!WiMAXCommonAPI_CheckIfBlackListedNw(pMruEntry->u32Secondary1Frequency))
			{
			    /* Fill Secondary1 Frequency details */
			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32CenterFreq = pMruEntry->u32Secondary1Frequency;
			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32Bandwidth = pstChannelPlan->BW;
			    memcpy(pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				   au32Preamble, pstChannelPlan->Preambles,
				   (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
			    /* Update Mapping Info */
			    pDeviceData->u8ChPlanIndxMapping[u32NumChannels] = u8ChannelPlanIndex;
			    u32NumChannels++;
			}
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    if ((pMruEntry->u32Secondary2Frequency > 0) &&
			!WiMAXCommonAPI_CheckIfBlackListedNw(pMruEntry->u32Secondary2Frequency))
			{
			    /* Fill Secondary2 Frequency details */
			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32CenterFreq = pMruEntry->u32Secondary2Frequency;
			    pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				u32Bandwidth = pstChannelPlan->BW;
			    memcpy(pstCaplSearchParams->stChannelSpec.stChannelList[u32NumChannels].
				   au32Preamble, pstChannelPlan->Preambles,
				   (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
			    /* Update Mapping Info */
			    pDeviceData->u8ChPlanIndxMapping[u32NumChannels] = u8ChannelPlanIndex;
			    u32NumChannels++;
			}
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    pMruEntry = pMruEntry->pNext;
		}

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "End of MRU Table\n");
	    if ((u32NumChannels + NUM_CHANNELS_PER_CHANNELPLAN) >= MAX_NUM_REQUESTS())
		{
		    pDeviceData->stCaplSearchDetails.u8Valid = TRUE;
		    pDeviceData->stCaplSearchDetails.u8MruSearchDone = TRUE;
		}
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    pstCaplSearchParams->stChannelSpec.u32NumChannels = u32NumChannels;
	}			/* End of if(!pDeviceData->stCaplSearchDetails.u8MruSearchDone) */


    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    if (!pDeviceData->stCaplSearchDetails.u8Valid
	|| (pDeviceData->stCaplSearchDetails.u8Valid &&
	    pDeviceData->stCaplSearchDetails.u8MruSearchDone))
#endif
	{
	    pDeviceData->stCaplSearchDetails.u8Valid = FALSE;
	    WimaxCommonAPI_ResetFlag();
	    pDeviceData->stCaplSearchDetails.u8MruSearchDone = TRUE;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Filling CAPL Table details\n");
	    WiMAXCommonAPI_FillCAPLDetails(pstCaplSearchParams);
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Number of freq%d",
			     pstCaplSearchParams->stChannelSpec.u32NumChannels);
	    if (pstCaplSearchParams->stChannelSpec.u32NumChannels > 0)
		{
		    pDeviceData->stCaplSearchDetails.u8Valid = TRUE;
		}
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    if (pDeviceData->stCaplSearchDetails.u8Valid == FALSE)
	{
	    memset(&pDeviceData->stCaplSearchDetails, 0, sizeof(ST_SEARCH_REQ_DETAILS));
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    if (pstCaplSearchParams->stChannelSpec.u32NumChannels == 0)
	{
	    DPRINT_COMMONAPI(DEBUG_WARNING, "CAPL search request contains no frequncies\n");
	    DPRINT_COMMONAPI(DEBUG_WARNING, "No CAPL search request sent to Firmware\n");
	    pDeviceData->u32SearchComplete = TRUE;
	    return;

	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    if (FALSE == pDeviceData->u32RxvdSyncRsp)
	{
#ifdef WIN32
	    Sleep(100);
#else
	    usleep(100000);
#endif

	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) & stRequestMessage, u32Len);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_WARNING, "Network search request returned %d", iRet);
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
}

VOID WiMAXCommonAPI_FillCAPLDetails(PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams)
{
    INT32 iRet = 0;
    UINT32 u32PriorityIndex = pDeviceData->stCaplSearchDetails.u32PriorityIndex;
    UINT32 u32CAPLIndex = pDeviceData->stCaplSearchDetails.u32CAPLIndex;
    UINT8 u8ChannelPlanIndex = pDeviceData->stCaplSearchDetails.u8ChannelPlanIndex;
    PUINT8 pu8RefID = NULL;

    pDeviceData->stNAPIDRequest.u32Any = pDeviceData->stNetworkParameters[0].CAPL.Any;
    pDeviceData->stNAPIDRequest.u32NAPIDCount = 0;

    for (; u32PriorityIndex < MAX_PRIORITY_POSSIBLE; u32PriorityIndex++)
	{
	    for (; u32CAPLIndex < pDeviceData->pPrioritizedCAPL[u32PriorityIndex].u32Count;
		 u32CAPLIndex++)
		{
		    /*pstCaplSearchParams->au32NapIdList[pstCaplSearchParams->u32NumNapID] = pDeviceData->pPrioritizedCAPL[u32PriorityIndex].stCAPLENTRIES[u32CAPLIndex].NAP_ID;
		       pstCaplSearchParams->u32NumNapID++; */

		    pDeviceData->stNAPIDRequest.au32NAPIDList[pDeviceData->stNAPIDRequest.
							      u32NAPIDCount] =
			pDeviceData->pPrioritizedCAPL[u32PriorityIndex].stCAPLENTRIES[u32CAPLIndex].
			NAP_ID;
		    pDeviceData->stNAPIDRequest.u32NAPIDCount++;

		    for (; u8ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP; u8ChannelPlanIndex++)
			{
			    if ((pstCaplSearchParams->stChannelSpec.u32NumChannels +
				 NUM_CHANNELS_PER_CHANNELPLAN) >= (MAX_NUM_REQUESTS()))
				{
				    pDeviceData->stCaplSearchDetails.u8Valid = 1;
				    pDeviceData->stCaplSearchDetails.u32PriorityIndex =
					u32PriorityIndex;
				    pDeviceData->stCaplSearchDetails.u32CAPLIndex = u32CAPLIndex;
				    pDeviceData->stCaplSearchDetails.u8ChannelPlanIndex =
					u8ChannelPlanIndex;
				    return;

				}

			    pu8RefID =
				pDeviceData->pPrioritizedCAPL[u32PriorityIndex].
				stCAPLENTRIES[u32CAPLIndex].ChPlanRefIds[u8ChannelPlanIndex].RefId;
			    iRet = strcmp((char *) pu8RefID, "");
			    if (iRet != 0)
				{
				    WimaxCommonAPI_FillChannelPlanDetails(pu8RefID,
									  &pstCaplSearchParams->
									  stChannelSpec);
				}
			}
		    u8ChannelPlanIndex = 0;
		}
	    u32CAPLIndex = 0;
	}

    WiMAXCommonAPI_AddBlackListedEntriesIfAny(&pstCaplSearchParams->stChannelSpec);
}

int WiMAXCommonAPI_CAPISetChannelPlanTable()
{
    UINT32 ChannelPlanIndex = 0, u32OperatorIndex = 0;
    INT32 iRet = 0;
    UINT32 u32NumOfNwParams = pDeviceData->pOperatorChannelPlanSettings->u32NumOfNetworkParamFields;
    PST_ChannelPlan pChannelPlan = NULL;

    if (pDeviceData->pstChannelPlan)
	{
	    free(pDeviceData->pstChannelPlan);
	    pDeviceData->pstChannelPlan = NULL;
	}

    if (!pDeviceData->pstChannelPlan)
	{
	    pDeviceData->pstChannelPlan = (PST_ChannelPlanTable) malloc(sizeof(ST_ChannelPlanTable)
									*
									MAX_CHANNEL_PLAN_ENTRIES_PER_NSP
									* u32NumOfNwParams);
	}

    if (!pDeviceData->pstChannelPlan)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Memory allocation failed");
	    return ERROR_MALLOC_FAILED;
	}
    memset(pDeviceData->pstChannelPlan, 0,
	   (sizeof(ST_ChannelPlanTable) * MAX_CHANNEL_PLAN_ENTRIES_PER_NSP * u32NumOfNwParams));
    //
    for (u32OperatorIndex = 0; u32OperatorIndex < u32NumOfNwParams; u32OperatorIndex++)
	{
	    pChannelPlan =
		&pDeviceData->pOperatorChannelPlanSettings->astNetworkParameters[u32OperatorIndex].
		ChannelPlan;

	    for (ChannelPlanIndex = 0; ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP;
		 ChannelPlanIndex++)
		{

		    memcpy(&
			   (pDeviceData->
			    pstChannelPlan[ChannelPlanIndex +
					   (u32OperatorIndex *
					    MAX_CHANNEL_PLAN_ENTRIES_PER_NSP)].Entries),
			   &(pChannelPlan->Entries[ChannelPlanIndex]), sizeof(ST_ChannelPlanEntries));

		}

	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####Channel Plan Details####\n");
    for (ChannelPlanIndex = 0; ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP * u32NumOfNwParams;
	 ChannelPlanIndex++)
	{
	    iRet = strcmp((char *) pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.Id, "");
	    if (iRet != 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "ChnlPlanIndex = %d Prim Freq=%d BW=%d\n",
				     ChannelPlanIndex,
				     pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.FirstFreq,
				     pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.BW);
		}
	}

    //

    return SUCCESS;
}

int WiMAXCommonAPI_InitChannelPlanTable()
{
    UINT32 ChannelPlanIndex = 0;
    INT32 iRet = 0;

    if (pDeviceData->pOperatorChannelPlanSettings)
	return WiMAXCommonAPI_CAPISetChannelPlanTable();
    if (!pDeviceData->pstChannelPlan)
	{
	    pDeviceData->pstChannelPlan = (PST_ChannelPlanTable) malloc(sizeof(ST_ChannelPlanTable)
									*
									MAX_CHANNEL_PLAN_ENTRIES_PER_NSP);
	}
    if (!pDeviceData->pstChannelPlan)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Memory allocation failed");
	    return ERROR_MALLOC_FAILED;
	}
    memset(pDeviceData->pstChannelPlan, 0,
	   (sizeof(ST_ChannelPlanTable) * MAX_CHANNEL_PLAN_ENTRIES_PER_NSP));
    for (ChannelPlanIndex = 0; ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP; ChannelPlanIndex++)
	{
	    memcpy(&(pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries),
		   &(pDeviceData->stNetworkParameters[0].ChannelPlan.Entries[ChannelPlanIndex]),
		   sizeof(ST_ChannelPlanEntries));

	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####Channel Plan Details####\n");
    for (ChannelPlanIndex = 0; ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP; ChannelPlanIndex++)
	{
	    iRet = strcmp((char *) pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.Id, "");
	    if (iRet != 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "ChnlPlanIndex = %d Prim Freq=%d BW=%d\n",
				     ChannelPlanIndex,
				     pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.FirstFreq,
				     pDeviceData->pstChannelPlan[ChannelPlanIndex].Entries.BW);
		}
	}
    return SUCCESS;
}

VOID WimaxCommonAPI_FillChannelPlanDetails(PUINT8 pu8RefID, PST_CHANNEL_SPECIFICATION pStChannelSpec)
{
    UINT8 u8ChannelPlanIndex = 0;
    PST_ChannelPlanEntries pstSelectedChannelPlan = NULL;

    for (u8ChannelPlanIndex = 0; u8ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP;
	 u8ChannelPlanIndex++)
	{
	    if (0 ==
		(memcmp
		 (pDeviceData->pstChannelPlan[u8ChannelPlanIndex].Entries.Id, pu8RefID,
		  CHANNEL_REFID_LEAF_SIZE)))
		{
		    if (pDeviceData->pstChannelPlan[u8ChannelPlanIndex].bFlagIncludedInSearchReq)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Channel Plan:%d Already included:::\n",
					     u8ChannelPlanIndex);
			    break;
			}
		    pDeviceData->pstChannelPlan[u8ChannelPlanIndex].bFlagIncludedInSearchReq = TRUE;
		    pstSelectedChannelPlan = &pDeviceData->pstChannelPlan[u8ChannelPlanIndex].Entries;
		    break;
		}
	}

    if (pstSelectedChannelPlan)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Channel Plan Index Included from CAPL :%d",
			     u8ChannelPlanIndex);


	    if (!WiMAXCommonAPI_CheckIfBlackListedNw(pstSelectedChannelPlan->FirstFreq))
		{
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32CenterFreq =
			pstSelectedChannelPlan->FirstFreq;
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32Bandwidth =
			pstSelectedChannelPlan->BW;
		    memcpy(pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].au32Preamble,
			   pstSelectedChannelPlan->Preambles, (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
		    /* Update Mapping info */
		    pDeviceData->u8ChPlanIndxMapping[pStChannelSpec->u32NumChannels] =
			u8ChannelPlanIndex;
		    pStChannelSpec->u32NumChannels++;
		}


	    if (((pstSelectedChannelPlan->FirstFreq + pstSelectedChannelPlan->NextFreqStep) <=
		 pstSelectedChannelPlan->LastFreq) &&
		!WiMAXCommonAPI_CheckIfBlackListedNw((pstSelectedChannelPlan->FirstFreq +
						      pstSelectedChannelPlan->NextFreqStep)))
		{
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32CenterFreq
			= (pstSelectedChannelPlan->FirstFreq + pstSelectedChannelPlan->NextFreqStep);
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32Bandwidth =
			pstSelectedChannelPlan->BW;
		    memcpy(pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].au32Preamble,
			   pstSelectedChannelPlan->Preambles, (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
		    /* Update Mapping info */
		    pDeviceData->u8ChPlanIndxMapping[pStChannelSpec->u32NumChannels] =
			u8ChannelPlanIndex;

		    pStChannelSpec->u32NumChannels++;
		}

	    if (((pstSelectedChannelPlan->FirstFreq + (2 * pstSelectedChannelPlan->NextFreqStep)) <=
		 pstSelectedChannelPlan->LastFreq) &&
		!WiMAXCommonAPI_CheckIfBlackListedNw((pstSelectedChannelPlan->FirstFreq +
						      (2 * pstSelectedChannelPlan->NextFreqStep))))
		{
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32CenterFreq =
			pstSelectedChannelPlan->LastFreq;
		    pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].u32Bandwidth =
			pstSelectedChannelPlan->BW;
		    memcpy(pStChannelSpec->stChannelList[pStChannelSpec->u32NumChannels].au32Preamble,
			   pstSelectedChannelPlan->Preambles, (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
		    /* Update Mapping info */
		    pDeviceData->u8ChPlanIndxMapping[pStChannelSpec->u32NumChannels] =
			u8ChannelPlanIndex;

		    pStChannelSpec->u32NumChannels++;
		}
	}
}

VOID WimaxCommonAPI_ResetFlag()
{
    UINT8 u8ChannelPlanIndex = 0;

    for (u8ChannelPlanIndex = 0; u8ChannelPlanIndex < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP;
	 u8ChannelPlanIndex++)
	{
	    pDeviceData->pstChannelPlan[u8ChannelPlanIndex].bFlagIncludedInSearchReq = FALSE;
	}
}

VOID WimaxCommonAPI_InitMruTable()
{
    PMRU_ROW_FORMAT pMruEEPROMData = NULL;
    UINT8 u8MruTableIndex = 0, u8PrevIndex = 0;
    PMRU_ENTRY pMruTable = NULL;

    pDeviceData->MruTable.mruHead = NULL;

    if (!pDeviceData->g_mru_init_struct.u8Valid)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "EEPROM MRU Table is Not Valid\n");
	    return;
	}
    u8MruTableIndex = pDeviceData->g_mru_init_struct.u8HeadIndex;
    pMruEEPROMData = pDeviceData->g_mru_init_struct.mruRows;


    if (u8MruTableIndex == TAIL_MRU_INDEX)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "MRU Table is empty\n");
	    return;
	}
    pDeviceData->MruTable.mruHead = &pDeviceData->MruTable.stMruEntries[u8MruTableIndex];

    pMruTable = &pDeviceData->MruTable.stMruEntries[u8MruTableIndex];
    pMruTable->u8CurrIndex = u8MruTableIndex;
    pMruTable->u8ChannelPlanIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].ChannelPlanIndex;
    pMruTable->NextMruEEPROMIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].NextIndex;
    pMruTable->PrevMruEEPROMIndex = 0;
    if (TAIL_MRU_INDEX == pMruTable->NextMruEEPROMIndex)
	{
	    pMruTable->pNext = NULL;
	}
    else
	{
	    pMruTable->pNext = &(pDeviceData->MruTable.stMruEntries[pMruTable->NextMruEEPROMIndex]);
	}
    pMruTable->pPrev = NULL;
    pMruTable->u8WhichIsPrimary = (UINT8) pMruEEPROMData[u8MruTableIndex].WhichIsPrimary;
    pMruTable->u8WhichIsSecondary1 = (UINT8) pMruEEPROMData[u8MruTableIndex].WhichIsSecondary1;
    WimaxCommonAPI_InitMruChannelPlanInfo(pMruTable->u8WhichIsPrimary, pMruTable->u8WhichIsSecondary1,
					  u8MruTableIndex, pMruTable->u8ChannelPlanIndex);
    u8PrevIndex = pMruTable->u8CurrIndex;
    u8MruTableIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].NextIndex;

    pDeviceData->MruTable.u32NumMruEntries++;

    while (u8MruTableIndex != TAIL_MRU_INDEX)
	{
	    pMruTable = &pDeviceData->MruTable.stMruEntries[u8MruTableIndex];
	    pMruTable->u8CurrIndex = u8MruTableIndex;
	    pMruTable->u8ChannelPlanIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].ChannelPlanIndex;
	    pMruTable->NextMruEEPROMIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].NextIndex;
	    pMruTable->PrevMruEEPROMIndex = u8PrevIndex;
	    if (TAIL_MRU_INDEX == pMruTable->NextMruEEPROMIndex)
		{
		    pMruTable->pNext = NULL;
		}
	    else
		{
		    pMruTable->pNext =
			&(pDeviceData->MruTable.stMruEntries[pMruTable->NextMruEEPROMIndex]);
		}
	    pMruTable->pPrev = &(pDeviceData->MruTable.stMruEntries[u8PrevIndex]);
	    pMruTable->u8WhichIsPrimary = (UINT8) pMruEEPROMData[u8MruTableIndex].WhichIsPrimary;
	    pMruTable->u8WhichIsSecondary1 = (UINT8) pMruEEPROMData[u8MruTableIndex].WhichIsSecondary1;
	    WimaxCommonAPI_InitMruChannelPlanInfo(pMruTable->u8WhichIsPrimary,
						  pMruTable->u8WhichIsSecondary1, u8MruTableIndex,
						  pMruTable->u8ChannelPlanIndex);

	    u8PrevIndex = pMruTable->u8CurrIndex;
	    pDeviceData->MruTable.u32NumMruEntries++;
	    u8MruTableIndex = (UINT8) pMruEEPROMData[u8MruTableIndex].NextIndex;
	}
    return;
}

VOID WimaxCommonAPI_InitMruChannelPlanInfo(UINT8 u8WhichIsPrimary, UINT8 u8WhichIsSecondary1,
					   UINT8 u8MruTableIndex, UINT8 u8ChannelPlanIndex)
{
    PST_ChannelPlanEntries pstChannelPlan = &pDeviceData->pstChannelPlan[u8ChannelPlanIndex].Entries;

    switch (u8WhichIsPrimary)
	{
	  case 1:
	      {
		  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32PrimaryFrequency
		      = pstChannelPlan->FirstFreq;
		  if (u8WhichIsSecondary1 == 2)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->FirstFreq + pstChannelPlan->NextFreqStep);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->LastFreq);

		      }
		  else if (u8WhichIsSecondary1 == 3)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->LastFreq);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->FirstFreq + pstChannelPlan->NextFreqStep);

		      }
		  else
		      {
			  DPRINT_COMMONAPI(DEBUG_ERROR, "Invalid Mru Entry\n");
		      }
		  break;
	      }
	  case 2:
	      {
		  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32PrimaryFrequency
		      = (pstChannelPlan->FirstFreq + pstChannelPlan->NextFreqStep);
		  if (u8WhichIsSecondary1 == 1)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->FirstFreq);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->LastFreq);

		      }
		  else if (u8WhichIsSecondary1 == 3)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->LastFreq);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->FirstFreq);
		      }
		  else
		      {
			  DPRINT_COMMONAPI(DEBUG_ERROR, "Invalid Mru Entry\n");

		      }
		  break;

	      }
	  case 3:
	      {
		  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32PrimaryFrequency
		      = pstChannelPlan->LastFreq;
		  if (u8WhichIsSecondary1 == 1)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->FirstFreq);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->FirstFreq + pstChannelPlan->NextFreqStep);

		      }
		  else if (u8WhichIsSecondary1 == 2)
		      {
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary1Frequency
			      = (pstChannelPlan->FirstFreq + pstChannelPlan->NextFreqStep);
			  pDeviceData->MruTable.stMruEntries[u8MruTableIndex].u32Secondary2Frequency
			      = (pstChannelPlan->FirstFreq);

		      }
		  else
		      {
			  DPRINT_COMMONAPI(DEBUG_ERROR, "Invalid Mru Entry\n");
		      }
		  break;

	      }

	}
}

VOID WimaxCommonAPI_UpdateMRUTable(UINT32 u32freq)
{
    UINT8 u8ChannelPlanIndex = 0;
    UINT32 bFound = FALSE, bUpdateEEPROM = FALSE;
    PMRU_ENTRY pstMruEntry = pDeviceData->MruTable.mruHead;
    ST_MRU_CHANGELIST stMruChangeList = { 0 };
    INT32 iRetVal = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;


    PMRU_ROW_FORMAT pstMruEEPROMEntry = pDeviceData->g_mru_init_struct.mruRows;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Function: UpdateMRUTable\n");
    ClearBSBlackList();
    u8ChannelPlanIndex =
	pDeviceData->u8ChPlanIndxMapping[pDeviceData->stCaplSearchRsp.
					 bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].u32Index];
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "u8ChannelPlanIndex %d", u8ChannelPlanIndex);

    if (pstMruEntry == NULL)
	{
	    /* Update details of the node */
	    pstMruEntry = (PMRU_ENTRY) & pDeviceData->MruTable.stMruEntries[0];
	    WimaxCommonAPI_ManipulateMruEntry(u32freq, u8ChannelPlanIndex, pstMruEntry);
	    pstMruEntry->u8ChannelPlanIndex = u8ChannelPlanIndex;
	    pstMruEntry->pPrev = NULL;
	    pstMruEntry->pNext = NULL;
	    pstMruEntry->NextMruEEPROMIndex = TAIL_MRU_INDEX;
	    pstMruEntry->PrevMruEEPROMIndex = 0;
	    pstMruEntry->u8CurrIndex = 0;
	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));


	    /* Update Head Index Details */
	    stMruChangeList.u8HeadIndex = pstMruEntry->u8CurrIndex;
	    pDeviceData->MruTable.mruHead = pstMruEntry;
	    pDeviceData->MruTable.u32NumMruEntries++;
	    pDeviceData->g_mru_init_struct.u8HeadIndex = stMruChangeList.u8HeadIndex;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "EEPROM MRU Table Updated\n");

	    iRetVal =
		CFG_WriteMRU(pWiMAX->pEEPROMContextTable, (PUINT8) & stMruChangeList,
			     sizeof(ST_MRU_CHANGELIST), CFG_MRU_MRUDATA_ID);
	    if (iRetVal < 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####MRU WRITE FAILED#########\n");
		}


	    return;
	}

    while (pstMruEntry->pNext != NULL)
	{
	    if (pstMruEntry->u8ChannelPlanIndex == u8ChannelPlanIndex)
		{
		    bFound = TRUE;
		    break;
		}
	    else
		{
		    pstMruEntry = pstMruEntry->pNext;
		}
	}
    if ((pstMruEntry->u8ChannelPlanIndex == u8ChannelPlanIndex) &&
	(pDeviceData->MruTable.mruHead == pstMruEntry))
	{
	    if (pstMruEntry->u32PrimaryFrequency != u32freq)
		{
		    WimaxCommonAPI_ManipulateMruEntry(u32freq, u8ChannelPlanIndex, pstMruEntry);
		    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry, &stMruChangeList);
		    /* Update Head Index Details */
		    stMruChangeList.u8HeadIndex = pstMruEntry->u8CurrIndex;

		    memcpy(&(pstMruEEPROMEntry[pstMruEntry->u8CurrIndex]),
			   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].
			     stMruContents), sizeof(MRU_ROW_FORMAT));
		    iRetVal =
			CFG_WriteMRU(pWiMAX->pEEPROMContextTable, (PUINT8) & stMruChangeList,
				     sizeof(ST_MRU_CHANGELIST), CFG_MRU_MRUDATA_ID);
		    if (iRetVal < 0)
			{
			    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####MRU WRITE FAILED#########\n");
			}


		}
	    else
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "NO EEPROM MRU Table Update\n");
		}

	    return;

	}



    if ((pstMruEntry->pNext == NULL) && (pstMruEntry->u8ChannelPlanIndex == u8ChannelPlanIndex))	/* Last Node */
	{

	    /* Update Previous node details */
	    pstMruEntry->pPrev->pNext = NULL;	/* Make previous node as last node. */
	    pstMruEntry->pPrev->NextMruEEPROMIndex = TAIL_MRU_INDEX;
	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry->pPrev, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->pPrev->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));

	    pstMruEntry->pPrev = NULL;
	    pstMruEntry->pNext = pDeviceData->MruTable.mruHead;
	    pstMruEntry->NextMruEEPROMIndex = pDeviceData->MruTable.mruHead->u8CurrIndex;
	    pstMruEntry->PrevMruEEPROMIndex = 0;
	    if (pstMruEntry->u32PrimaryFrequency != u32freq)
		{
		    WimaxCommonAPI_ManipulateMruEntry(u32freq, u8ChannelPlanIndex, pstMruEntry);
		}

	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));


	    /* Updating head index details */
	    stMruChangeList.u8HeadIndex = pstMruEntry->u8CurrIndex;
	    pDeviceData->MruTable.mruHead->pPrev = pstMruEntry;
	    pDeviceData->MruTable.mruHead = pstMruEntry;
	    pDeviceData->g_mru_init_struct.u8HeadIndex = stMruChangeList.u8HeadIndex;

	    iRetVal =
		CFG_WriteMRU(pWiMAX->pEEPROMContextTable, (PUINT8) & stMruChangeList,
			     sizeof(ST_MRU_CHANGELIST), CFG_MRU_MRUDATA_ID);
	    if (iRetVal < 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####MRU WRITE FAILED#########\n");
		}

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "EEPROM MRU Table Updated\n");
	    return;
	}

    if (bFound == TRUE)
	{
	    /* Update Previous node details */
	    pstMruEntry->pPrev->pNext = pstMruEntry->pNext;
	    pstMruEntry->pNext->pPrev = pstMruEntry->pPrev;
	    pstMruEntry->pPrev->NextMruEEPROMIndex = pstMruEntry->NextMruEEPROMIndex;
	    pstMruEntry->pNext->PrevMruEEPROMIndex = pstMruEntry->PrevMruEEPROMIndex;
	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry->pPrev, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->pPrev->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));

	    pstMruEntry->pPrev = NULL;
	    pstMruEntry->pNext = pDeviceData->MruTable.mruHead;
	    pstMruEntry->NextMruEEPROMIndex = pDeviceData->MruTable.mruHead->u8CurrIndex;
	    pstMruEntry->PrevMruEEPROMIndex = 0;
	    if (pstMruEntry->u32PrimaryFrequency != u32freq)
		{
		    WimaxCommonAPI_ManipulateMruEntry(u32freq, u8ChannelPlanIndex, pstMruEntry);
		}
	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));

	    /* Updating head index details */
	    stMruChangeList.u8HeadIndex = pstMruEntry->u8CurrIndex;
	    pDeviceData->MruTable.mruHead->pPrev = pstMruEntry;
	    pDeviceData->MruTable.mruHead = pstMruEntry;
	    pDeviceData->g_mru_init_struct.u8HeadIndex = stMruChangeList.u8HeadIndex;

	    bUpdateEEPROM = TRUE;

	}
    else if (bFound == FALSE)
	{
	    /* Update details of the node */
	    pstMruEntry =
		(PMRU_ENTRY) & pDeviceData->MruTable.stMruEntries[pDeviceData->MruTable.
								  u32NumMruEntries];
	    WimaxCommonAPI_ManipulateMruEntry(u32freq, u8ChannelPlanIndex, pstMruEntry);
	    pstMruEntry->u8ChannelPlanIndex = u8ChannelPlanIndex;
	    pstMruEntry->pPrev = NULL;
	    pstMruEntry->pNext = pDeviceData->MruTable.mruHead;
	    pstMruEntry->NextMruEEPROMIndex = pDeviceData->MruTable.mruHead->u8CurrIndex;
	    pstMruEntry->PrevMruEEPROMIndex = 0;
	    pstMruEntry->u8CurrIndex = (CFG_UINT8) pDeviceData->MruTable.u32NumMruEntries;
	    WimaxCommonAPI_UpdateMruChangelist(pstMruEntry, &stMruChangeList);
	    memcpy(&(pstMruEEPROMEntry[pstMruEntry->u8CurrIndex]),
		   &(stMruChangeList.stRowDesciptor[stMruChangeList.u16NumRows - 1].stMruContents),
		   sizeof(MRU_ROW_FORMAT));


	    /* Update head index details */
	    stMruChangeList.u8HeadIndex = pstMruEntry->u8CurrIndex;
	    pDeviceData->MruTable.mruHead->pPrev = pstMruEntry;
	    pDeviceData->MruTable.mruHead = pstMruEntry;
	    pDeviceData->g_mru_init_struct.u8HeadIndex = stMruChangeList.u8HeadIndex;

	    pDeviceData->MruTable.u32NumMruEntries++;
	    bUpdateEEPROM = TRUE;


	}			/*else if bFfound == FALSE */

    if (bUpdateEEPROM)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "MRU Table Updated in EEPROM\n");

	    iRetVal =
		CFG_WriteMRU(pWiMAX->pEEPROMContextTable, (PUINT8) & stMruChangeList,
			     sizeof(ST_MRU_CHANGELIST), CFG_MRU_MRUDATA_ID);
	    if (iRetVal < 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####MRU WRITE FAILED#########\n");
		}
	}
    else
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "NO EEPROM MRU Table Update\n");
	}
    return;
}

VOID WimaxCommonAPI_UpdateMruChangelist(PMRU_ENTRY pstMruEntry, PST_MRU_CHANGELIST pstMruChangeList)
{
    UINT16 u16Rows = pstMruChangeList->u16NumRows;

    pstMruChangeList->stRowDesciptor[u16Rows].u16RowIndex = pstMruEntry->u8CurrIndex;
    pstMruChangeList->stRowDesciptor[u16Rows].stMruContents.ChannelPlanIndex =
	pstMruEntry->u8ChannelPlanIndex;
    pstMruChangeList->stRowDesciptor[u16Rows].stMruContents.NextIndex = pstMruEntry->NextMruEEPROMIndex;
    pstMruChangeList->stRowDesciptor[u16Rows].stMruContents.WhichIsPrimary =
	pstMruEntry->u8WhichIsPrimary;
    pstMruChangeList->stRowDesciptor[u16Rows].stMruContents.WhichIsSecondary1 =
	pstMruEntry->u8WhichIsSecondary1;
    pstMruChangeList->u16NumRows++;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Updating EEPROM row: %d\n Details:\n", pstMruEntry->u8CurrIndex);
    DPRINT_COMMONAPI(DEBUG_MESSAGE,
		     "Channel Plan Index = %d \t NextMruEEPROMIndex = %d\t u8WhichIsPrimary = %d\n",
		     pstMruEntry->u8ChannelPlanIndex, pstMruEntry->NextMruEEPROMIndex,
		     pstMruEntry->u8WhichIsPrimary);
    return;
}

VOID WimaxCommonAPI_ManipulateMruEntry(UINT32 u32freq, UINT8 u8ChannelPlanIndex, PMRU_ENTRY pstMruEntry)
{
    ST_ChannelPlanEntries stChannelPlan;

    stChannelPlan = pDeviceData->pstChannelPlan[u8ChannelPlanIndex].Entries;


    if (stChannelPlan.FirstFreq == u32freq)
	{
	    pstMruEntry->u8WhichIsPrimary = FIRST_FREQ;
	    if (pstMruEntry->u32PrimaryFrequency != 0)
		{
		    pstMruEntry->u32Secondary1Frequency = pstMruEntry->u32PrimaryFrequency;
		}
	    else
		{
		    pstMruEntry->u32Secondary1Frequency =
			(stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep);
		}

	    if (pstMruEntry->u32Secondary1Frequency ==
		(stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep))
		{
		    pstMruEntry->u32Secondary2Frequency = stChannelPlan.LastFreq;
		    pstMruEntry->u8WhichIsSecondary1 = SECOND_FREQ;
		}
	    else
		{
		    if (stChannelPlan.FirstFreq != stChannelPlan.LastFreq)
			pstMruEntry->u32Secondary2Frequency =
			    (stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep);
		    else
			pstMruEntry->u32Secondary2Frequency = stChannelPlan.FirstFreq;

		    pstMruEntry->u8WhichIsSecondary1 = LAST_FREQ;
		}
	}
    else if ((stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep) == u32freq)
	{
	    pstMruEntry->u8WhichIsPrimary = SECOND_FREQ;
	    if (pstMruEntry->u32PrimaryFrequency != 0)
		{
		    pstMruEntry->u32Secondary1Frequency = pstMruEntry->u32PrimaryFrequency;
		}
	    else
		{
		    pstMruEntry->u32Secondary1Frequency = stChannelPlan.FirstFreq;
		}
	    if (pstMruEntry->u32Secondary1Frequency == stChannelPlan.FirstFreq)
		{
		    pstMruEntry->u32Secondary2Frequency = stChannelPlan.LastFreq;
		    pstMruEntry->u8WhichIsSecondary1 = FIRST_FREQ;
		}
	    else
		{
		    pstMruEntry->u32Secondary2Frequency = stChannelPlan.FirstFreq;
		    pstMruEntry->u8WhichIsSecondary1 = LAST_FREQ;
		}
	}
    else if (stChannelPlan.LastFreq == u32freq)
	{
	    pstMruEntry->u8WhichIsPrimary = LAST_FREQ;
	    if (pstMruEntry->u32PrimaryFrequency != 0)
		{
		    pstMruEntry->u32Secondary1Frequency = pstMruEntry->u32PrimaryFrequency;
		}
	    else
		{
		    pstMruEntry->u32Secondary1Frequency = stChannelPlan.FirstFreq;
		}
	    if (pstMruEntry->u32Secondary1Frequency ==
		(stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep))
		{
		    pstMruEntry->u32Secondary2Frequency = stChannelPlan.FirstFreq;
		    pstMruEntry->u8WhichIsSecondary1 = SECOND_FREQ;
		}
	    else
		{
		    if (stChannelPlan.FirstFreq != stChannelPlan.LastFreq)
			pstMruEntry->u32Secondary2Frequency =
			    (stChannelPlan.FirstFreq + stChannelPlan.NextFreqStep);
		    else
			pstMruEntry->u32Secondary2Frequency = stChannelPlan.FirstFreq;
		    pstMruEntry->u8WhichIsSecondary1 = FIRST_FREQ;
		}
	}
    pstMruEntry->u32PrimaryFrequency = u32freq;
}

VOID WiMAXCommonAPI_PrintMruTable()
{
    PMRU_ENTRY pMruEntry = NULL;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Printing MRU Table in Mem\n");
    pMruEntry = pDeviceData->MruTable.mruHead;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Head Index = %d\n", pMruEntry->u8CurrIndex);
    while (pMruEntry != NULL)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE,
			     "MRU Table Index = %d\tNext Index = %d\tChannel Plan Index = %d\tPrimary Frequency = %d\n",
			     pMruEntry->u8CurrIndex, pMruEntry->NextMruEEPROMIndex,
			     pMruEntry->u8ChannelPlanIndex, pMruEntry->u32PrimaryFrequency);
	    pMruEntry = pMruEntry->pNext;
	}
}

VOID WiMAXCommonAPI_PrintEEPROMMruTable()
{
    PMRU_ROW_FORMAT pMruEERPOM = pDeviceData->g_mru_init_struct.mruRows;
    UINT8 u8MruTableIndex = pDeviceData->g_mru_init_struct.u8HeadIndex;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Printing EEPROM MRU Table\n");
    while (u8MruTableIndex != TAIL_MRU_INDEX)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE,
			     "EEPROM ROW Index = %d\tNext Index = %d\tChannel Plan Index = %d\t Which is Primary= %d\n",
			     u8MruTableIndex, pMruEERPOM[u8MruTableIndex].NextIndex,
			     pMruEERPOM[u8MruTableIndex].ChannelPlanIndex,
			     pMruEERPOM[u8MruTableIndex].WhichIsPrimary);
	    u8MruTableIndex = (UINT8) pMruEERPOM[u8MruTableIndex].NextIndex;
	}
}

INT WiMAXCommonAPI_ResetMRUTable(UINT8 u8MruResetFlag)
{
#ifdef ENABLE_MRU_UPDATES
    INT32 iRetVal = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (NULL != pDeviceData->MruTable.mruHead)
	{
	    pDeviceData->MruTable.mruHead->u8CurrIndex = TAIL_MRU_INDEX;
	    pDeviceData->MruTable.mruHead = NULL;
	}
    pDeviceData->MruTable.u32NumMruEntries = 0;
    memset(pDeviceData->MruTable.stMruEntries, 0,
	   (sizeof(MRU_ENTRY) * MAX_CHANNEL_PLAN_REF_ID_PER_CAPL));


    memset(pDeviceData->g_mru_init_struct.mruRows, 0,
	   sizeof(MRU_ROW_FORMAT) * MAX_CHANNEL_PLAN_REF_ID_PER_CAPL);
    pDeviceData->g_mru_init_struct.u8HeadIndex = TAIL_MRU_INDEX;
    if (u8MruResetFlag)
	{
	    iRetVal = CFG_ResetMRU(pWiMAX->pEEPROMContextTable);
	    if (iRetVal < 0)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "#####MRU RESET FAILED#########\n");
		    return FALSE;
		}
	}
#else
    UNUSED_ARGUMENT(u8MruResetFlag);
#endif
    return TRUE;
}

VOID WiMAXCommonAPI_ServiceFlowResponse(PWIMAX_MESSAGE pstWiMAXMessage)
{
    UNUSED_ARGUMENT(pstWiMAXMessage);
#ifdef ENABLE_MRU_UPDATES
    if (pDeviceData->u32RxvdCmdConnectToNetwork)
	{
	    pDeviceData->u32RxvdCmdConnectToNetwork = FALSE;
	    WimaxCommonAPI_UpdateMRUTable(pDeviceData->u32CenterFreq);
	}
#endif

}


int WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError()
{
    MIB_OBJECT_INFO MIBObjInfo = { 0 };
    INT iRet = 0;

    MIBObjInfo.u32MibObjId = BCM_BASEBAND_DEVICE_STATUS;
    iRet = WiMAXCommonAPI_SendMIBGetRequest(&MIBObjInfo);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "MIB GET for MIB ID %d returned %d", MIBObjInfo.u32MibObjId,
			     iRet);
	    return iRet;
	}
    MIBObjInfo.u32MibObjId = BCM_BASEBAND_DEVICE_CONNECTION_STATUS;
    iRet = WiMAXCommonAPI_SendMIBGetRequest(&MIBObjInfo);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "MIB GET for MIB ID %d returned %d", MIBObjInfo.u32MibObjId,
			     iRet);
	    return iRet;
	}
    MIBObjInfo.u32MibObjId = BCM_BASEBAND_LINK_STATUS_REASON;
    iRet = WiMAXCommonAPI_SendMIBGetRequest(&MIBObjInfo);
    if (iRet)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "MIB GET for MIB ID %d returned %d", MIBObjInfo.u32MibObjId,
			     iRet);
	    return iRet;
	}

    return SUCCESS;
}

VOID WiMAXCommonAPI_UpdateDeviceStatus(UINT32 u32DeviceStatus)
{
    WIMAX_API_DEVICE_STATUS eDevicePreviousStatus = pDeviceData->eDeviceStatus;

    switch (u32DeviceStatus)
	{
	  case DEV_UNINITIALIZED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_UNINITIALIZED");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_UnInitialized;
	      break;
	  case DEV_RF_OFF_HW_SW:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_RF_OFF_HW_SW");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_HW_SW;
	      break;
	  case DEV_RF_OFF_HW:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_RF_OFF_HW");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_HW;
	      break;
	  case DEV_RF_OFF_SW:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_RF_OFF_SW");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_RF_OFF_SW;
	      break;
	  case DEV_READY:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_READY");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Ready;
	      break;
	  case DEV_SCANNING:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_SCANNING");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Scanning;
	      break;
	  case DEV_CONNECTING:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_CONNECTING");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Connecting;
	      break;
	  case DEV_DATA_CONNECTED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus DEV_DATA_CONNECTED");
	      pDeviceData->eDeviceStatus = WIMAX_API_DEVICE_STATUS_Data_Connected;
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Reset stBlackListedBSes as DEV_DATA_CONNECTED");
	      break;
	  default:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateDeviceStatus  Unknown LINK status code %d",
			       u32DeviceStatus);
	      break;
	}
    if ((eDevicePreviousStatus == WIMAX_API_DEVICE_STATUS_Data_Connected) &&
	(pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected) &&
	(pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork))
	{
	    IndDisconnectToNetwork pIndDisconnectToNetwork = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDisconnectToNetwork = pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndDisconnectToNetwork)
		pIndDisconnectToNetwork(&pDeviceData->stDeviceID, WIMAX_API_CONNECTION_SUCCESS);
	}
    if ((eDevicePreviousStatus != pDeviceData->eDeviceStatus) &&
	(pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork))
	{
	    IndConnectToNetwork pIndConnectToNetwork = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndConnectToNetwork = pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);

	    if ((eDevicePreviousStatus == WIMAX_API_DEVICE_STATUS_Connecting) &&
		(pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected))
		{
		    pIndConnectToNetwork(&pDeviceData->stDeviceID, WIMAX_API_CONNECTION_FAILURE);
		}
	    else if (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Data_Connected)
		{
		    pIndConnectToNetwork(&pDeviceData->stDeviceID, WIMAX_API_CONNECTION_SUCCESS);
		}

	}
#ifndef MACOS
    if (eDevicePreviousStatus != pDeviceData->eDeviceStatus)
	{
	    PBCM_VDM_IPC_INTF pstSendpayload = NULL;

	    BECEEMCALLOC_NOT_RET(pstSendpayload, PBCM_VDM_IPC_INTF, BCM_VDM_IPC_INTF, 1)
		pstSendpayload->eInterfaceType = CAPI_INTF;
	    if (eDevicePreviousStatus == WIMAX_API_DEVICE_STATUS_Data_Connected)
		{

		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Sending linkdown message ");
		    pstSendpayload->uiCommand = eLinkdown;
		    SendOMA_DMClient(pstSendpayload);
		}
	    else if (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Data_Connected)
		{

		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Sending linkup message ");
		    pstSendpayload->uiCommand = eLinkup;
		    SendOMA_DMClient(pstSendpayload);
		}

		BECEEM_MEMFREE(pstSendpayload)
	}
#endif

    if ((eDevicePreviousStatus == WIMAX_API_DEVICE_STATUS_Data_Connected) &&
	((pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected)) &&
	(pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork))
	{
	    IndDisconnectToNetwork pIndDisconnectToNetwork = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDisconnectToNetwork = pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndDisconnectToNetwork)
		pIndDisconnectToNetwork(&pDeviceData->stDeviceID, WIMAX_API_CONNECTION_SUCCESS);
	}


    if ((eDevicePreviousStatus != pDeviceData->eDeviceStatus) &&
	pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate)
	{
	    IndDeviceStatusUpdate pIndDeviceStatusUpdate = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDeviceStatusUpdate = pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndDeviceStatusUpdate)
		pIndDeviceStatusUpdate(&pDeviceData->stDeviceID, pDeviceData->eDeviceStatus,
				       pDeviceData->eStatusReason, pDeviceData->eConnectionProgress);
	}
}

VOID WiMAXCommonAPI_UpdateConnectionStatus(UINT32 u32ConnectionStatus)
{
    WIMAX_API_CONNECTION_PROGRESS_INFO eConnectionProgress = pDeviceData->eConnectionProgress;
    WIMAX_API_DEVICE_STATUS eDeviceStatus = pDeviceData->eDeviceStatus;
    UINT32 u32ConnectionStatusInfoPrevious = pDeviceData->u32ConnectionStatusUpdate;
    IndConnectToNetwork pIndConnectToNetwork = NULL;

    pDeviceData->u32ConnectionStatusUpdate = u32ConnectionStatus;

    if ((pDeviceData->u32ConnectionStatusUpdate != u32ConnectionStatusInfoPrevious) &&
	pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  previous %d and present %d",
			     u32ConnectionStatusInfoPrevious, pDeviceData->u32ConnectionStatusUpdate);

	    switch (u32ConnectionStatusInfoPrevious)
		{
		  case PHY_SYNC_ACHIVED:
		  case LINKUP_IN_PROGRESS:
		  case LINKUP_DONE:
		  case SBC_IN_PROGRESS:
		  case EAP_AUTH_DEVICE_IN_PROGRESS:
		  case EAP_AUTH_USER_IN_PROGRESS:
		  case SATEK_IN_PROGRESS:
		  case REGISTRATION_IN_PROGRESS:
		      switch (pDeviceData->u32ConnectionStatusUpdate)
			  {
			    case WAIT_FOR_SYNC:
			    case DREG_RECIEVED:
			    case RESET_RECIEVED:
			    case SHUTDOWN_REQ_FROM_FIRMWARE:
				//disconnect status here.

				Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
				pIndConnectToNetwork =
				    pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork;
				Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.
					     objCallbackMutex);

				DPRINT_COMMONAPI(DEBUG_MESSAGE,
						 "WIMAX_API_CONNECTION_FAILURE INDICATED");
				pIndConnectToNetwork(&pDeviceData->stDeviceID,
						     WIMAX_API_CONNECTION_FAILURE);
				break;

			  }
		  default:
		      break;

		}
	}


    switch (u32ConnectionStatus)
	{
	  case WAIT_FOR_SYNC:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  WAIT_FOR_SYNC");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_Ranging;
	      break;
	  case PHY_SYNC_ACHIVED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  PHY_SYNC_ACHIVED");

	      {
		  DPRINT_COMMONAPI(DEBUG_MESSAGE, "Initiating network entry");
		  WiMAXCommonAPI_NetworkEntryRequest();
	      }
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_Ranging;
	      break;
	  case LINKUP_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  LINKUP_IN_PROGRESS");
	      break;
	  case LINKUP_DONE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  LINKUP_DONE");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_Registered;
	      break;
	  case DREG_RECIEVED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  DREG_RECIEVED");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_De_registration;
	      WiMAXCommonAPI_StartSearch();
	      break;
	  case RESET_RECIEVED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "RESET_RECIEVED starting search now!!!");
	      WiMAXCommonAPI_StartSearch();
	      break;

	  case PERIODIC_WAKE_UP_FROM_SHUTDOWN:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  PERIODIC_WAKE_UP_FROM_SHUTDOWN");
	      //DPRINT_COMMONAPI(DEBUG_MESSAGE,"PERIODIC_WAKE_UP_FROM_SHUTDOWN  starting search now!!!");
	      WiMAXCommonAPI_StartSearch();
	      break;
	  case SHUTDOWN_REQ_FROM_FIRMWARE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  SHUTDOWN_REQ_FROM_FIRMWARE");
	      break;
	  case COMPLETE_WAKE_UP_FROM_SHUTDOWN:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  COMPLETE_WAKE_UP_FROM_SHUTDOWN");
	      break;
	  case RANGING_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  RANGING_IN_PROGRESS");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_Ranging;
	      break;
	  case SBC_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  SBC_IN_PROGRESS");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_SBC;
	      break;
	  case EAP_AUTH_DEVICE_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  EAP_AUTH_DEVICE_IN_PROGRESS");
	      pDeviceData->eConnectionProgress =
		  WIMAX_API_DEVICE_CONNECTION_PROGRESS_EAP_authentication_Device;
	      break;
	  case EAP_AUTH_USER_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  EAP_AUTH_USER_IN_PROGRESS");
	      pDeviceData->eConnectionProgress =
		  WIMAX_API_DEVICE_CONNECTION_PROGRESS_EAP_authentication_User;
	      break;
	  case SATEK_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  SATEK_IN_PROGRESS");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_3_way_handshake;
	      break;
	  case REGISTRATION_IN_PROGRESS:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  REGISTRATION_IN_PROGRESS");
	      pDeviceData->eConnectionProgress = WIMAX_API_DEVICE_CONNECTION_PROGRESS_Registration;
	      break;
	  case HMC_SHUTDOWN_REQ_FROM_FIRMWARE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  HMC_SHUTDOWN_REQ_FROM_FIRMWARE");
	      break;
	  case HMC_WAKE_UP_NOTIFICATION_FRM_FW:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE,
			       "UpdateConnectionStatus  HMC_WAKE_UP_NOTIFICATION_FRM_FW");
	      break;

	  default:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateConnectionStatus  Unknown LINK status code %d",
			       u32ConnectionStatus);
	      break;
	}

    if ((eConnectionProgress != pDeviceData->eConnectionProgress ||
	 eDeviceStatus != pDeviceData->eDeviceStatus) &&
	pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate)
	{
	    IndDeviceStatusUpdate pIndDeviceStatusUpdate = NULL;

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDeviceStatusUpdate = pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "App call back Device status %d conn status %d",
			     pDeviceData->eDeviceStatus, pDeviceData->eConnectionProgress);
	    if (pIndDeviceStatusUpdate)
		pIndDeviceStatusUpdate(&pDeviceData->stDeviceID, pDeviceData->eDeviceStatus,
				       pDeviceData->eStatusReason, pDeviceData->eConnectionProgress);
	}
}

VOID WiMAXCommonAPI_UpdateLinkStatus(UINT32 u32LinkStatus)
{
    WIMAX_API_STATUS_REASON eStatusReason = pDeviceData->eStatusReason;

    switch (u32LinkStatus)
	{
	  case STATUS_NORMAL:
	      //DPRINT_COMMONAPI(DEBUG_MESSAGE,"UpdateLinkStatus  STATUS_NORMAL");                    
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case PHY_SYNC_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  PHY_SYNC_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_to_NW;
	      break;
	  case MAC_SYNC_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  MAC_SYNC_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_to_NW;
	      break;
	  case HOST_INITIATED:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  HOST_INITIATED");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_to_NW;
	      break;
	  case RANGE_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  RANGE_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_Ranging;
	      break;
	  case SBC_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  SBC_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_SBC;
	      break;
	  case SATEK_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  SATEK_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_EAP_AUTH_Device;
	      break;
	  case PKM_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  PKM_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_3_Way_Handshake;
	      break;
	  case REG_FAIL:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  REG_FAIL");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_REG;
	      break;
	  case IDLE_MODE_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  IDLE_MODE_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case SLEEP_MODE_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  SLEEP_MODE_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case DREG_RECEIVED_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  DREG_RECEIVED_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_datapath;
	      break;
	  case RESET_RECEIVED_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  RESET_RECEIVED_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Fail_to_connect_datapath;
	      break;
	  case NET_ENTRY_PROGRESS_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  NET_ENTRY_PROGRESS_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case WAIT_NET_ENTRY_CMD_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  WAIT_NET_ENTRY_CMD_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case WAIT_PHY_SYNC_CMD_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  WAIT_PHY_SYNC_CMD_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case SHUTDOWN_RECEIVED_CODE:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  SHUTDOWN_RECEIVED_CODE");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case HOST_INITIATED_LINK_DOWN:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  HOST_INITIATED_LINK_DOWN");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  case MAC_VERSION_SWITCH:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  MAC_VERSION_SWITCH");
	      pDeviceData->eStatusReason = WIMAX_API_STATUS_REASON_Normal;
	      break;
	  default:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "UpdateLinkStatus  Unknown LINK status code %d",
			       u32LinkStatus);
	      break;
	}

    if ((eStatusReason != pDeviceData->eStatusReason) &&
	pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate)
	{
	    IndDeviceStatusUpdate pIndDeviceStatusUpdate = NULL;

	    /*      if(pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Connecting) */
	    WiMAXCommonAPI_BSBlackListingTakeAction(u32LinkStatus);

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndDeviceStatusUpdate = pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    if (pIndDeviceStatusUpdate)
		pIndDeviceStatusUpdate(&pDeviceData->stDeviceID, pDeviceData->eDeviceStatus,
				       pDeviceData->eStatusReason, pDeviceData->eConnectionProgress);
	}
}

VOID WiMAXCommonAPI_MIBIndication(PWIMAX_MESSAGE pWiMaxMsg)
{
    //DPRINT_COMMONAPI(DEBUG_MESSAGE,"\n~~~~~~~~~~~~~~~ MIB MESSAGE RECEIVED ~~~~~~~~~~~~~~~\n");

    if (pWiMaxMsg->usSubType == SUBTYPE_MIB_GET_RESP)
	{
	    PMIB_OBJECT_INFO pMIB = NULL;

	    /*      DPRINT_COMMONAPI(DEBUG_MESSAGE,"\nMIB Get Response receved\n"); */
	    pMIB = (PMIB_OBJECT_INFO) pWiMaxMsg->szData;
	    /*DPRINT_COMMONAPI(DEBUG_MESSAGE,"\n MIB OID - %d : ",pMIB->u32MibObjId); */

	    if (pMIB->s8ErrorCode)
		DPRINT_COMMONAPI(DEBUG_MESSAGE, " Error Code while MIB Set: (%d) \n",
				 pMIB->s8ErrorCode);
	    else
		{
		    /*for(i=0; i<(int)pMIB->u32MibObjSize; i++)
		       {
		       DPRINT_COMMONAPI(DEBUG_MESSAGE," 0x%02x",pMIB->au8MibObjVal[i]);
		       } */
		    if (pMIB->u32MibObjSize == 4)
			{
			    /*DPRINT_COMMONAPI(DEBUG_MESSAGE," UINT eq : %d", *(UINT *)pMIB->au8MibObjVal); */

			    if (pMIB->u32MibObjSize == 4)
				{
				    /*DPRINT_COMMONAPI(DEBUG_MESSAGE," UINT eq : %d", *(UINT *)pMIB->au8MibObjVal); */

				    switch (pMIB->u32MibObjId)
					{
					  case BCM_BASEBAND_DEVICE_STATUS:
					      WiMAXCommonAPI_UpdateDeviceStatus(*(UINT32 *) pMIB->
										au8MibObjVal);
					      break;
					  case BCM_BASEBAND_DEVICE_CONNECTION_STATUS:
					      WiMAXCommonAPI_UpdateConnectionStatus(*(UINT32 *) pMIB->
										    au8MibObjVal);
					      break;
					  case BCM_BASEBAND_LINK_STATUS_REASON:
					      WiMAXCommonAPI_UpdateLinkStatus(*(UINT32 *) pMIB->
									      au8MibObjVal);
					      break;
					  default:
					      break;
					}
				}
			    /*DPRINT_COMMONAPI(DEBUG_MESSAGE,"\n"); */
			}
		}
	}
}

VOID WiMAXCommonAPI_DeviceStatusIndication(PWIMAX_MESSAGE pWiMaxMsg)
{
    PST_MODEM_STATE_INFO pstModemStatus = (PST_MODEM_STATE_INFO) pWiMaxMsg->szData;

    switch (pstModemStatus->eModemStatus)
	{
	  case DEVICE_STATUS_IND:
	      WiMAXCommonAPI_UpdateDeviceStatus(pstModemStatus->u32Status);
	      break;

	  case CONNECTION_STATUS_IND:
	      WiMAXCommonAPI_UpdateConnectionStatus(pstModemStatus->u32Status);
	      break;

	  case LINK_STATUS_IND:
	      WiMAXCommonAPI_UpdateLinkStatus(pstModemStatus->u32Status);
	      break;

	  default:
	      break;
	}
}

PVOID WiMAXCommonAPI_PeriodicMIBRequestThread(PVOID pData)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    SET_THREAD_NAME();
    UNUSED_ARGUMENT(pData);
    pWiMAX->m_bWimaxCommonAPI_SearhThreadRunning = TRUE;

    while (pWiMAX->m_bDeviceInitInProgress)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Device INIT is in progress ");
#ifndef WIN32
	    usleep(300000);
#else
	    Sleep(300);
#endif
	    if (pWiMAX->m_bKillWimaxCommonAPI_PeriodicMIBRequests)
		{
		    return SUCCESS;
		}
	}

    while (!pWiMAX->m_bKillWimaxCommonAPI_PeriodicMIBRequests)
	{
#ifndef WIN32
	    usleep(1000000);
#else
	    Sleep(1000);
#endif
	    if (pWiMAX->m_bIdleMode || pWiMAX->m_bModemInShutdown)
		continue;

	    WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError();

	}

    return SUCCESS;
}

int WiMAXCommonAPI_SendMIBGetRequest(PMIB_OBJECT_INFO pMIBReq)
{
    WIMAX_MESSAGE stRequestMessage = { 0 };
    int iLen = 0, iRet = 0;

    memset(&stRequestMessage, 0, sizeof(WIMAX_MESSAGE));

    if (!pDeviceData->hDevice)
	return ERROR_HANDLE_INVALID;

    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(&stRequestMessage, TYPE_MIB, SUBTYPE_MIB_GET_REQ,
				    sizeof(MIB_OBJECT_INFO));

    memcpy(stRequestMessage.szData, pMIBReq, sizeof(MIB_OBJECT_INFO));

    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) & stRequestMessage, iLen);
    if (iRet < 0)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Error Code Returned : [%d]\n", iRet);
	}
    return SUCCESS;
}

INT32 WiMAXCommonAPI_StartSearch()
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    WIMAX_MESSAGE stRequestMessage = { 0 };
    INT32 iLen = 0, iRet = 0;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    iLen = WiMAXCommonAPI_FillWiMAXMsg(&stRequestMessage, TYPE_LINK_DOWN, SUBTYPE_SYNC_DOWN_REQ, 0);
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) & stRequestMessage, iLen);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "LINK DOWN");
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "WiMAXCommonAPI_StartSearch");
    memset(&pDeviceData->stCaplSearchRsp, 0, sizeof(ST_CAPL_SEARCH_RSP));
    Event_Set(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));
    Event_Set(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
    return iRet;
}


#if (defined(MACOS) || defined(LINUX))
BOOL WiMAXCommonAPI_GetInterfaceInfo(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo)
{
    UCHAR ucInfName[20] = { 0 };
    UINT8 u8MacAddress[6] = { 0 };
    UINT uiInterfaceNameLengthUnicode = 0;

#ifdef MACOS
    UINT8 macaddr[6] = { 0 };
    struct if_msghdr *ifm;
    struct sockaddr_dl *sdl;
    UCHAR *buf, *lim, *next;
    size_t buf_len;
    int mib[6];
#elif LINUX
    struct ifreq ifreq;
    int fd = 0, io = 0, i = 0;
#endif
    GetMACAddress(GpWiMAX, u8MacAddress);
#ifdef MACOS
    mib[0] = CTL_NET;
    mib[1] = PF_ROUTE;
    mib[2] = 0;
    mib[3] = 0;			/* address family */
    mib[4] = NET_RT_IFLIST;
    mib[5] = 0;

    if (sysctl(mib, 6, NULL, &buf_len, NULL, 0) < 0)
	DPRINT_COMMONAPI(DEBUG_ERROR,
			 "\nWiMAXCommonAPI_GetInterfaceInfo  Failed to get the estimate\n");

    if ((buf = malloc(buf_len)) == NULL)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "\nWiMAXCommonAPI_GetInterfaceInfo  malloc failed\n");
	    return FALSE;
	}
    if (sysctl(mib, 6, buf, &buf_len, NULL, 0) < 0)
	DPRINT_COMMONAPI(DEBUG_ERROR,
			 "\nWiMAXCommonAPI_GetInterfaceInfo  Failed to fetch interface table\n");

    lim = buf + buf_len;

    next = buf;
    while (next < lim)
	{
	    ifm = (struct if_msghdr *) next;

	    if (ifm->ifm_type == RTM_IFINFO)
		{
		    sdl = (struct sockaddr_dl *) (ifm + 1);
		}

	    next += ifm->ifm_msglen;
	    if (sdl->sdl_alen == sizeof(u8MacAddress))
		{
		    memcpy(macaddr, sdl->sdl_data + sdl->sdl_nlen, sdl->sdl_alen);
		    if (memcmp(u8MacAddress, macaddr, sdl->sdl_alen) == 0)
			{
			    strncpy((char *) ucInfName, sdl->sdl_data, sdl->sdl_nlen);
			    ucInfName[sdl->sdl_nlen] = '\0';
			    if (ConvertToUnicode
				((char *) ucInfName, pInterfaceInfo->interfaceName,
				 &uiInterfaceNameLengthUnicode))
				{
				    pInterfaceInfo->structureSize = sizeof(WIMAX_API_INTERFACE_INFO);
				    free(buf);
				    return TRUE;
				}
			}
		    memset(macaddr, 0, sizeof(macaddr));
		}
	}
    free(buf);
#else
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (fd < 0)
	DPRINT_COMMONAPI(DEBUG_ERROR, "socket creation failed..\n");
    else
	{
	    for (i = 0; i < 256; i++)
		{
		    ifreq.ifr_ifindex = i;
		    io = ioctl(fd, SIOCGIFNAME, &ifreq);
		    if (io != 0)
			{
			    if ((io = ioctl(fd, SIOCGIFHWADDR, &ifreq)) == 0)
				{
				    if (memcmp
					(ifreq.ifr_hwaddr.sa_data, u8MacAddress,
					 sizeof(u8MacAddress)) == 0)
					{
					    strcpy((char *) ucInfName, ifreq.ifr_name);
					    ucInfName[strlen((char *) ucInfName)] = '\0';
					    if (ConvertToUnicode
						((char *) ucInfName, pInterfaceInfo->interfaceName,
						 &uiInterfaceNameLengthUnicode))
						{
						    pInterfaceInfo->structureSize =
							sizeof(WIMAX_API_INTERFACE_INFO);
						    close(fd);
						    return TRUE;
						}
					}
				}
			}
		}
	}
    close(fd);
#endif
    return FALSE;
}
#endif
BOOL WiMAXCommonAPI_CheckIfBlackListedNw(UINT32 u32CF)
{
#ifdef BLACK_LISTING_BS_ENABLED
    UINT32 u32Index = 0;

    for (u32Index = 0; u32Index < pDeviceData->stBlackListedBSes.u32NumBS; u32Index++)
	{
	    if (pDeviceData->stBlackListedBSes.bsInfo[u32Index].u32CenterFreq == u32CF)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE,
				     "WiMAXCommonAPI_CheckIfBlackListedNw Blacklisted NW# frequency is %d",
				     u32CF);
		    return TRUE;
		}
	}
#else
    UNUSED_ARGUMENT(u32CF);
#endif
    return FALSE;
}

VOID ClearBSBlackList()
{
#ifdef BLACK_LISTING_BS_ENABLED
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Reseting stBlackListedBSes contents");
    memset(&pDeviceData->stBlackListedBSes, 0, sizeof(pDeviceData->stBlackListedBSes));
#endif
}

VOID WiMAXCommonAPI_AddBlackListedEntriesIfAny(PST_CHANNEL_SPECIFICATION pChannelPlan)
{
#ifdef BLACK_LISTING_BS_ENABLED
    UINT32 u32Index = 0;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "WiMAXCommonAPI_AddBlackListedEntriesIfAny");

    for (u32Index = 0; u32Index < pDeviceData->stBlackListedBSes.u32NumBS; u32Index++)
	{
	    PST_ChannelPlanEntries pstSelectedChannelPlan = NULL;

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "WiMAXCommonAPI_AddBlackListedEntriesIfAny");

	    pstSelectedChannelPlan =
		&pDeviceData->pstChannelPlan[pDeviceData->stBlackListedBSes.
					     u8ChPlanIndxMapping[u32Index]].Entries;
	    pDeviceData->pstChannelPlan[pDeviceData->stBlackListedBSes.u8ChPlanIndxMapping[u32Index]].
		bFlagIncludedInSearchReq = TRUE;
	    pChannelPlan->stChannelList[pChannelPlan->u32NumChannels].u32Bandwidth =
		pDeviceData->stBlackListedBSes.bsInfo[u32Index].u32Bandwidth;

	    pChannelPlan->stChannelList[pChannelPlan->u32NumChannels].u32CenterFreq =
		pDeviceData->stBlackListedBSes.bsInfo[u32Index].u32CenterFreq;

	    memcpy(pChannelPlan->stChannelList[pChannelPlan->u32NumChannels].au32Preamble,
		   pstSelectedChannelPlan->Preambles, (sizeof(UINT32) * PREAMBLE_ARRAY_SIZE));
	    pDeviceData->u8ChPlanIndxMapping[pChannelPlan->u32NumChannels] =
		pDeviceData->stBlackListedBSes.u8ChPlanIndxMapping[u32Index];
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "channel plan index mapping # %d",
			     pDeviceData->u8ChPlanIndxMapping[pChannelPlan->u32NumChannels]);

	    pChannelPlan->u32NumChannels++;
	}
#else
    UNUSED_ARGUMENT(pChannelPlan);
#endif
}

VOID WiMAXCommonAPI_AddToBlackListArray()
{
#ifdef BLACK_LISTING_BS_ENABLED
    if (pDeviceData->stBlackListedBSes.u32NumBS < MAX_NUMBER_OF_BLACKLISTED_BS)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Adding frequency to Blacklisted BS");
	    memcpy(&pDeviceData->stBlackListedBSes.bsInfo[pDeviceData->stBlackListedBSes.u32NumBS],
		   &pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex],
		   sizeof(ST_DETECTED_BS_INFO));

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Black listed frequency is %d",
			     pDeviceData->stBlackListedBSes.bsInfo[pDeviceData->stBlackListedBSes.
								   u32NumBS].u32CenterFreq);

	    pDeviceData->stBlackListedBSes.u8ChPlanIndxMapping[pDeviceData->stBlackListedBSes.
							       u32NumBS] =
		pDeviceData->u8ChPlanIndxMapping[pDeviceData->stCaplSearchRsp.
						 bsInfo[pDeviceData->u32PresentSyncup_CAPL_BSIndex].
						 u32Index];
	    pDeviceData->stBlackListedBSes.u32NumBS++;
	    pDeviceData->u32PresentSyncup_CAPL_BSIndex++;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Adding frequency to Blacklisted BS");
	}
    else
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "BLACK LISTED ARRAY IS FULL!!!");
	    pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
	}
#endif
}
VOID WiMAXCommonAPI_BSBlackListingTakeAction(UINT32 u32LinkStatus)
{
#ifdef BLACK_LISTING_BS_ENABLED
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "WiMAXCommonAPI_BSBlackListingTakeAction");
    switch (u32LinkStatus)
	{
	  case PHY_SYNC_FAIL:
	  case MAC_SYNC_FAIL:
	  case RANGE_FAIL:
	  case SBC_FAIL:
	  case PKM_FAIL:
	  case SATEK_FAIL:
	  case REG_FAIL:

	      WiMAXCommonAPI_AddToBlackListArray();
	      if (pDeviceData->u32PresentSyncup_CAPL_BSIndex < pDeviceData->stCaplSearchRsp.u32NumBS)
		  {
		      /*Try network entry again. */
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Issue Sync up for Nex BS");
		      WiMAXCommonAPI_SyncUpRequest();
		  }

	      else
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE,
				       "Start Search As no more BS is available for Network entry");
		      pDeviceData->u32PresentSyncup_CAPL_BSIndex = 0;
		      WiMAXCommonAPI_StartSearch();
		  }
	      break;

	  default:
	      /*No Action required */
	      break;
	}
#else
    UNUSED_ARGUMENT(u32LinkStatus);
#endif

}

BOOL WiMAXCommonAPI_CheckIfEntriesAlreadyExist(PST_DETECTED_BS_INFO pDetectedBS)
{
    UINT32 u32Index = 0;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Inside WiMAXCommonAPI_CheckIfEntriesAlreadyExist");
    for (u32Index = 0; u32Index < pDeviceData->stCaplSearchRsp.u32NumBS; u32Index++)
	{

	    if (memcmp
		(pDeviceData->stCaplSearchRsp.bsInfo[u32Index].au8Bsid, pDetectedBS->au8Bsid,
		 sizeof(UINT8) * 6) == 0)
		{
		    return TRUE;
		}
	}
    return FALSE;
}

VOID WiMAXCommonAPI_CheckAndAddBStoList(PST_CAPL_SEARCH_RSP pstCAPL_Search_Resp)
{
    UINT32 u32Index = 0;
    UINT32 u32AddBSToList = 0;
    UINT32 u32IndicatedOfSearchResults = 0;

    for (u32Index = 0; u32Index < pstCAPL_Search_Resp->u32NumBS; u32Index++)
	{
		/*- If Any = 1 check if the NAP ID MATCHES with the one received 
		-  If Any =2 no need to check if the NAPID Matches just add to BS List.*/
	    if (pDeviceData->stNAPIDRequest.u32Any == 2)
		{
		    u32AddBSToList = 1;
		}
	    else if (pDeviceData->stNAPIDRequest.u32Any == 1)
		{
		    /*Check for NAPID here */
		    UINT32 u32Ind = 0, u32NAPID = 0, u32Count = 0;

		    while (u32Ind < 3)
			{
			    u32NAPID |= pstCAPL_Search_Resp->bsInfo[u32Index].au8Bsid[u32Ind];
			    u32NAPID = u32NAPID << 8;
			    u32Ind++;
			}
		    u32NAPID = u32NAPID >> 8;

		    for (u32Count = 0; u32Count < pDeviceData->stNAPIDRequest.u32NAPIDCount; u32Count++)
			{
			    if (pDeviceData->stNAPIDRequest.au32NAPIDList[u32Count] == u32NAPID)
				{
				    u32AddBSToList = 1;
				    break;
				}

			}
		}

	    if (!WiMAXCommonAPI_CheckIfEntriesAlreadyExist(&pstCAPL_Search_Resp->bsInfo[u32Index]) &&
		u32AddBSToList)
		{
		    memcpy(&pDeviceData->stCaplSearchRsp.bsInfo[pDeviceData->stCaplSearchRsp.u32NumBS],
			   &pstCAPL_Search_Resp->bsInfo[u32Index], sizeof(ST_DETECTED_BS_INFO));
		    pDeviceData->stCaplSearchRsp.u32NumBS++;


		}
	    if (u32AddBSToList)
		{
		    WiMAXCommonAPI_SearchCallbackFunction(u32Index,
							  pstCAPL_Search_Resp->u32SearchCompleteFlag);
		    u32IndicatedOfSearchResults = 1;
		}
	    u32AddBSToList = 0;
	}
    if (!u32IndicatedOfSearchResults && pstCAPL_Search_Resp->u32SearchCompleteFlag && pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch)	//To provide callback when no search has 
	{
	    IndNetworkSearch pIndNetworkSearch = NULL;
	    WIMAX_API_NSP_INFO stNSPInfo;

	    memset(&stNSPInfo, 0, sizeof(WIMAX_API_NSP_INFO));

	    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
	    pIndNetworkSearch = pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch;
	    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);

	    pIndNetworkSearch(&pDeviceData->stDeviceID, &stNSPInfo, 0, SEARCH_STATUS_PERCENTAGE);
	}
}

VOID WiMAXCommonAPI_SearchCallbackFunction(UINT32 u32Index, UINT32 bSearchCompleteFlag)
{
    WIMAX_API_NSP_INFO stNSPInfo = { 0 };
    UINT32 u32UnicodeOperatorLength = 0;
    IndNetworkSearch pIndNetworkSearch = NULL;

    UNUSED_ARGUMENT(bSearchCompleteFlag);

    if (!pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch)
	return;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pIndNetworkSearch = pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    if (!pIndNetworkSearch)
	{
	    return;
	}

    stNSPInfo.structureSize = sizeof(WIMAX_API_NSP_INFO);
    stNSPInfo.CINR = (CFG_UINT8) (pDeviceData->stCaplSearchRsp.bsInfo[u32Index].s32CINR[0] + 10);
    stNSPInfo.RSSI = (CFG_UINT8) (pDeviceData->stCaplSearchRsp.bsInfo[u32Index].s32RSSI[0] + 123);
    stNSPInfo.networkType = WIMAX_API_HOME;
    ConvertToUnicode((char *) pDeviceData->stNetworkParameters[0].OperatorName,
		     (PUSHORT) stNSPInfo.NSPName, &u32UnicodeOperatorLength);
    stNSPInfo.NSPid = pDeviceData->stNetworkParameters[0].H_NSP[0].H_NSP_ID;

    pIndNetworkSearch(&pDeviceData->stDeviceID, &stNSPInfo, 1, SEARCH_STATUS_PERCENTAGE);

}

INT WiMAXCommonAPI_WakeupDeviceFromIdleMode()
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    INT iRet = WIMAX_API_RET_SUCCESS;
    INT numTries = 0;

    //Device is in idle mode. 

  wakeup_modem:
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse));

    iRet = WiMAXCommonAPI_SendIdleModeWakeupRequest();

    if (iRet != WIMAX_API_RET_SUCCESS)
	return WIMAX_API_RET_FAILED;

    iRet = BeceemWiMAX_StatsUpdate();
    if (iRet != SUCCESS)
	{
	    if (pWiMAX->m_bIdleMode && numTries < 2)
		{
		    numTries++;
		    goto wakeup_modem;
		}
	}

    return iRet;
}

INT WiMAXCommonAPI_CheckModemStatus()
{
    PWIMAX_MESSAGE pstRequestMessage =NULL;
	int iLen = 0,iRet =0;
    
    BECEEMCALLOC_RET(pstRequestMessage,PWIMAX_MESSAGE,WIMAX_MESSAGE,1,INT)

    iLen = WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_BASEBAND_MODEM_STATUS_REQ, sizeof(E_BASEBAND_MODEM_STATUS));   
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);
    if(iRet != SUCCESS)
        goto ret_path;
    
    switch(*((PE_BASEBAND_MODEM_STATUS)pstRequestMessage->szData))
        {
            case MODE_ACTIVE:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMODEM STATUS: MODE_ACTIVE");
				break;
			case MODEM_CRASHED_HANG:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMODEM STATUS: MODEM_CRASHED_HANG");
                iRet = ERROR_HANDLE_INVALID;
				break;
			case MODEM_IN_SHUTDOWN_MODE:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMODEM STATUS: MODEM_IN_SHUTDOWN_MODE");
				break;
			case MODEM_IN_IDLE_MODE:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMODEM STATUS: MODEM_IN_IDLE_MODE");
				break;
			case MODEM_FIRMWARE_DOWNLOAD_IN_PROGRESS:
				DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nMODEM STATUS: MODEM_FIRMWARE_DOWNLOAD_IN_PROGRESS");
                iRet = ERROR_HANDLE_INVALID;
				break;
			default:
				printf("\nMODEM STATUS: UNKNOWN");
				break;        

    }
    
ret_path:
    
    BECEEM_MEMFREE(pstRequestMessage);
    return iRet;
}