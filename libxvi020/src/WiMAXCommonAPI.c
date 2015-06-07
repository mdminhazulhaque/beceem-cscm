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
 * Description	:	CAPI 1.2 implementation.
 * Author		:
 * Reviewer		:
 *
 */




#include "WiMAXCommonAPI.h"
#ifdef WIN32
#include "TCHAR.H"
#endif
#ifdef WIN32
#pragma warning(disable : 4127)
#endif
#include "globalobjects.h"

extern BOOL g_bInitiateNetworkEntry;
PBECEEM_COMMON_API_PERSISTANT_DATA pCommonAPIPersistantData = NULL;
PBECEEM_COMMON_API_DEVICE_DATA pDeviceData = NULL;

PWIMAX_MESSAGE pWiMAXMessage = NULL;

/**
Opens the CAPI library.
*/



WIMAX_API_RET WiMaxAPIOpen(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    pDeviceId->structureSize = sizeof(WIMAX_API_DEVICE_ID);
    pDeviceId->apiVersion = COMMON_API_VERSION;
    pDeviceId->sdkHandle = 0;
    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);

    if (WiMAXCommonAPI_CheckIfBCAPIIsUsedByApp())
	{
	    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
	    return WIMAX_API_RET_FAILED;
	}

    if (pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is opened already. Return Error */
	    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
	    return WIMAX_API_RET_FAILED;
	}
    pWiMAX->m_bCommonAPIIfActive = TRUE;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Acquired @ %s and %d", __FUNCTION__, __LINE__);
    if (!DeviceIO_OpenHandle(&(pWiMAX->m_Device), BCM_SYMBOLIC_NAME))
	{
	    pDeviceId->deviceIndex = 1;
	    pDeviceId->devicePresenceStatus = TRUE;

	    DeviceIO_CloseHandle(&(pWiMAX->m_Device));
	}
    else
	{
	    pDeviceId->deviceIndex = 0;
	    pDeviceId->devicePresenceStatus = FALSE;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Released @ %s and %d", __FUNCTION__, __LINE__);
    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);

    if (!pCommonAPIPersistantData)
	{
	    pWiMAX->m_bCommonAPIIfActive = FALSE;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d previlege %d", __FUNCTION__, __LINE__,
			     pDeviceId->privilege);
	    return WIMAX_API_RET_FAILED;
	}
    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));

    SharedMem_ReadData(&pWiMAX->m_SharedMemCommonAPI, (PUCHAR) pCommonAPIPersistantData,
		       sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));

    if ((pCommonAPIPersistantData->privilege == WIMAX_API_PRIVILEGE_READ_ONLY) &&
	(pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE))
	{
	    pWiMAX->m_bCommonAPIIfActive = FALSE;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d previlege %d", __FUNCTION__, __LINE__,
			     pDeviceId->privilege);
	    return WIMAX_API_RET_PERMISSION_DENIED;
	}

/*	pCommonAPIPersistantData->u32NumberOfAppRunning++;*/
    if (pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
	{
	    pCommonAPIPersistantData->privilege = WIMAX_API_PRIVILEGE_READ_ONLY;
	    Mutex_Lock(&(pWiMAX->m_ShMemCommonAPIMutex));
	    SharedMem_WriteData(&pWiMAX->m_SharedMemCommonAPI,
				(PUCHAR) pCommonAPIPersistantData,
				sizeof(BECEEM_COMMON_API_PERSISTANT_DATA), 0);

	    Mutex_Unlock(&(pWiMAX->m_ShMemCommonAPIMutex));
	}

    pCommonAPIPersistantData->privilege = pDeviceId->privilege;
    memcpy(&pDeviceData->stDeviceID, pDeviceId, sizeof(WIMAX_API_DEVICE_ID));

    if (!pDeviceData->u32FlagForDeviceInsertMonitorRunning)
	{
	    if (Thread_Run(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor), (PTHREADFUNC)
			   WiMAXCommonAPI_CardInsertMonitorThreadFunc, pDeviceData))
		{
		    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Device insert monitor thread failed !!!");
		    return WIMAX_API_RET_FAILED;
		}
#ifndef WIN32
	    usleep(1000);
#else
	    Sleep(1);
#endif

	}

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);

#ifndef MACOS
    CAPIOMAInterfaceInit(NULL);
#endif

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "pDeviceId Previlege %d", pDeviceData->privilege);

    pDeviceData->privilege = pDeviceId->privilege;
    return WIMAX_API_RET_SUCCESS;
}

/**
Only one device will be listed
*/
WIMAX_API_RET GetListDevice(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_HW_DEVICE_ID_P pHwDeviceIdList,
			    UINT32 * pHwDeviceIdListSize)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    int nLen = 0;

    if (!pDeviceId || !pHwDeviceIdListSize)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    if (*pHwDeviceIdListSize == 0)
	{
	    *pHwDeviceIdListSize = 1;
	    return WIMAX_API_RET_BUFFER_SIZE_TOO_SMALL;
	}

    if (!pHwDeviceIdList)
	{
	    return WIMAX_API_RET_BUFFER_SIZE_TOO_SMALL;
	}
    nLen = (int) strlen(BCM_DEVICE_NAME) + 1;
    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);
    if (pDeviceData->hDevice)
	{
	    pHwDeviceIdList->structureSize = sizeof(WIMAX_API_HW_DEVICE_ID);
	    pHwDeviceIdList->deviceIndex = 1;
	    ConvertToUnicode(BCM_DEVICE_NAME, pHwDeviceIdList->deviceName, (UINT *) & nLen);
	    pHwDeviceIdList->deviceType = WIMAX_API_DEV_TYPE_WIMAX;

	    *pHwDeviceIdListSize = 1;
	    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
	    return WIMAX_API_RET_SUCCESS;
	}


    if (!DeviceIO_OpenHandle(&(pWiMAX->m_Device), BCM_SYMBOLIC_NAME))
	{
	    pHwDeviceIdList->structureSize = sizeof(WIMAX_API_HW_DEVICE_ID);
	    pHwDeviceIdList->deviceIndex = 1;
	    ConvertToUnicode(BCM_DEVICE_NAME, pHwDeviceIdList->deviceName, (UINT *) & nLen);
	    pHwDeviceIdList->deviceType = WIMAX_API_DEV_TYPE_WIMAX;
	    *pHwDeviceIdListSize = 1;
	    DeviceIO_CloseHandle(&(pWiMAX->m_Device));

#ifdef ScanningTime
#ifdef WIN32
	    OutputDebugString("Beceem: deviced detected by CM\n");
#endif
#endif

	}
    else
	{
	    *pHwDeviceIdListSize = 0;
	    memset(pHwDeviceIdList, 0, sizeof(WIMAX_API_HW_DEVICE_ID));
	    //set after clearing out.
	    pHwDeviceIdList->structureSize = sizeof(WIMAX_API_HW_DEVICE_ID);
	}
    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
    return WIMAX_API_RET_SUCCESS;
}

/**
Open the device specified in the deviceID structure.
*/
WIMAX_API_RET WiMaxDeviceOpen(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    WIMAX_API_RET eWiMAXRetVal = WIMAX_API_RET_FAILED;
    INT iRetries = 0;

#ifdef ScanningTime
#ifdef WIN32
    OutputDebugString("\nWiMaxDeviceOpen: Beceem Device being opened\n");
#endif
#endif
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Acquired @ %s and %d", __FUNCTION__, __LINE__);
    if (!pDeviceId)
	{
	    eWiMAXRetVal = WIMAX_API_RET_INVALID_PARAMETER;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    goto return_path;
	}
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    eWiMAXRetVal = WIMAX_API_RET_FAILED;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    goto return_path;
	}
    if (WiMAXCommonAPI_CheckIfBCAPIIsUsedByApp())
	{
	    eWiMAXRetVal = WIMAX_API_RET_FAILED;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
	    goto return_path;
	}
    if (pDeviceData->hDevice)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Device is already opened");
	    eWiMAXRetVal = WIMAX_API_RET_SUCCESS;
	    goto return_path;
	}
    if (!pDeviceId->devicePresenceStatus)
	{
	    if (!DeviceIO_OpenHandle(&(pWiMAX->m_Device), BCM_SYMBOLIC_NAME))
		{
		    pDeviceId->devicePresenceStatus = TRUE;	/*Question: pDeviceId is an input 
								   parameter not an in/out parameter ?? */
		    pDeviceData->stDeviceID.devicePresenceStatus = TRUE;

		    DeviceIO_CloseHandle(&(pWiMAX->m_Device));
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		}
	    else
		{
		    eWiMAXRetVal = WIMAX_API_RET_FAILED;
		    pDeviceData->stDeviceID.devicePresenceStatus = FALSE;
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
		    goto return_path;
		}
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "%s and %d", __FUNCTION__, __LINE__);
    while (pDeviceData->u32FlagForDeviceInsertMonitorRunning)
	{
	    pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = TRUE;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Waiting for deviceInsertMonitor Thread to terminate");
#ifndef WIN32
	    usleep(20000);
#else
	    Sleep(20);
#endif
	    iRetries++;
	    if (iRetries > 50)
		{
		    eWiMAXRetVal = WIMAX_API_RET_FAILED;
		    goto return_path;
		}

	}
    eWiMAXRetVal = WiMAXCommonAPI_OpenNRegisterCallback(pDeviceId);
    
    if (!pDeviceData->u32FlagForDeviceInsertMonitorRunning && (eWiMAXRetVal != WIMAX_API_RET_SUCCESS))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Launching Device Insert Monitor thread");
        pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = FALSE;
        pDeviceData->u32FlagForDeviceInsertMonitorRunning = FALSE;
	    if (Thread_Run(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor), (PTHREADFUNC)
			   WiMAXCommonAPI_CardInsertMonitorThreadFunc, pDeviceData))
		{
		    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Device insert monitor thread failed !!!");
		   
		}
#ifndef WIN32
	    usleep(1000);
#else
	    Sleep(1);
#endif
      eWiMAXRetVal = WIMAX_API_RET_FAILED;

	}
  return_path:
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Released @ %s and %d", __FUNCTION__, __LINE__);
    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
    return eWiMAXRetVal;

}

/**
	Device close
*/
WIMAX_API_RET WiMaxDeviceClose(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    if (!pDeviceData->hDevice)
	return WIMAX_API_RET_FAILED;

    if (WiMAXCommonAPI_SubscribeToStats(0))
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stMutexForInterfaceAccess);
    CloseWiMAXDevice();
    pDeviceData->hDevice = NULL;
    pDeviceData->u32RxvdCmdConnectToNetwork = FALSE;
    pDeviceData->u32SearchComplete = FALSE;
    if (!pDeviceData->u32FlagForDeviceInsertMonitorRunning)
	{
	    pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = FALSE;
	    if (Thread_Run(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor),
			   (PTHREADFUNC) WiMAXCommonAPI_CardInsertMonitorThreadFunc, pDeviceData))
		{
		    DPRINT_COMMONAPI(DEBUG_ERROR, "Launching Device insert monitor thread failed !!!");
		}

	}


    Mutex_Unlock(&pDeviceData->stMutexForInterfaceAccess);
    return WIMAX_API_RET_SUCCESS;
}

/**
API close
*/
WIMAX_API_RET WiMaxAPIClose(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);

    pWiMAX->m_bCommonAPIIfActive = FALSE;

    WiMAXCommonAPI_UpdateAccessrightsOnClose();

    if (pDeviceData->hDevice)
	WiMaxDeviceClose(pDeviceId);

    memset(&pDeviceData->stCallBackFunctionCommonAPIs, 0, sizeof(CALLBACK_FNS_COMMON_API));

    return WIMAX_API_RET_SUCCESS;
}

/**
control power management of the device 
*/
WIMAX_API_RET CmdControlPowerManagement(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_RF_STATE powerState)
{
    INT iLen = 0;
    INT iRetVal = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    WIMAX_API_RET eAPIRetVal = WIMAX_API_RET_SUCCESS;

    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    DPRINT_COMMONAPI(DEBUG_ERROR, "pWiMAX->m_bCommonAPIIfActive in %s", __FUNCTION__);
	    eAPIRetVal = WIMAX_API_RET_FAILED;
	    goto ret_path;
	}
    if (!pDeviceId)
	{
	    eAPIRetVal = WIMAX_API_RET_INVALID_PARAMETER;
	    goto ret_path;
	}

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    eAPIRetVal = WIMAX_API_RET_INVALID_DEVICE;
	    goto ret_path;
	}

    if (pDeviceData->stDeviceID.privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	{

	    eAPIRetVal = WIMAX_API_RET_PERMISSION_DENIED;
	    goto ret_path;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d and powerState %d", __FUNCTION__, __LINE__,
		     powerState);
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse));
    switch (powerState)
	{
	  case WIMAX_API_RF_OFF:

	      if (pWiMAX->m_bModemInShutdown)
		  {
		      eAPIRetVal = WIMAX_API_RET_SUCCESS;
		      goto ret_path;
		  }
	      pDeviceData->u32RfOffFromUser = TRUE;
	      if ((FALSE == pDeviceData->u32SearchComplete) &&
		  (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Scanning))
		  {
		     WiMAXCommonAPI_AbortSearch();
			 

		  }
	      else if (pWiMAX->m_bIdleMode)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
		      if (WiMAXCommonAPI_WakeupDeviceFromIdleMode() != SUCCESS)
			  {
			      return WIMAX_API_RET_FAILED;
			  }

		  }
	      else if (pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected)
		  {
		      WiMAXCommonAPI_NwSearchPreconditioning();
			 
		  }

	      iLen = WiMAXCommonAPI_FillWiMAXMsg(pWiMAXMessage, TYPE_SHUTDOWN, SUBTYPE_SHUTDOWN_REQ, 0);
	      iRetVal = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pWiMAXMessage, iLen);
	      if (iRetVal)
		  {
		      pDeviceData->u32RfOffFromUser = FALSE;
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
		      eAPIRetVal = WIMAX_API_RET_FAILED;
		      goto ret_path;
		  }


	      if (Event_Wait
		  (&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse),
		   (TIMEOUT_FOR_SHUTDOWN_REQ + 100)) != WAIT_TIMEOUT)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
		      pDeviceData->u32TotalOperatorsFound = 0;

		      if (pWiMAX->m_bModemInShutdown)
			  {
			      eAPIRetVal = WIMAX_API_RET_SUCCESS;
			      goto ret_path;
			  }
		      else
			  {
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Shutdown request failed");
			      eAPIRetVal = WIMAX_API_RET_FAILED;
			      pDeviceData->u32RfOffFromUser = FALSE;
			      goto ret_path;
			  }

		  }
	      else
		  {
		      pDeviceData->u32RfOffFromUser = FALSE;
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "shutdown failed %s and %d and ", __FUNCTION__,
				       __LINE__);
		      eAPIRetVal = WIMAX_API_RET_FAILED;
		      goto ret_path;
		  }
	      break;
	  case WIMAX_API_RF_ON:
	      pDeviceData->u32RfOffFromUser = FALSE;
	      if (pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_RF_OFF_SW)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
		      eAPIRetVal = WIMAX_API_RET_SUCCESS;
		      goto ret_path;
		  }



	      WiMAXCommonAPI_DoDummySearch();

	      ///
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);
	      if (Event_Wait
		  (&(pWiMAX->m_WiMAXCommonAPI_WaitForShutdownResponse),
		   (TIMEOUT_FOR_SHUTDOWN_REQ + 100)) != WAIT_TIMEOUT)
		  {
		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);

		      if (!pWiMAX->m_bModemInShutdown)
			  {
			      WiMAXCommonAPI_StartSearch();
			      if (pDeviceData->stCallBackFunctionCommonAPIs.pIndControlPowerManagement)
				  {
				      IndControlPowerManagement pIndControlPowerManagement = NULL;;
				      Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.
						 objCallbackMutex);
				      pIndControlPowerManagement =
					  pDeviceData->stCallBackFunctionCommonAPIs.
					  pIndControlPowerManagement;
				      Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.
						   objCallbackMutex);
				      if (pIndControlPowerManagement)
					  pIndControlPowerManagement(&pDeviceData->stDeviceID,
								     WIMAX_API_RF_ON);
				  }

			      eAPIRetVal = WIMAX_API_RET_SUCCESS;
			      goto ret_path;
			  }
		      else
			  {
			      DPRINT_COMMONAPI(DEBUG_MESSAGE, "Shutdown wakeup failed");
			      eAPIRetVal = WIMAX_API_RET_FAILED;
			      goto ret_path;
			  }

		  }
	      else
		  {

		      DPRINT_COMMONAPI(DEBUG_MESSAGE, "shutdown wakeup failed %s and %d and ",
				       __FUNCTION__, __LINE__);
		      eAPIRetVal = WIMAX_API_RET_FAILED;
		      goto ret_path;
		  }
	      ///


	      break;
	  default:
	      eAPIRetVal = WIMAX_API_RET_FAILED;
	      goto ret_path;
	}
  ret_path:
    return eAPIRetVal;
}

WIMAX_API_RET CmdResetWimaxDevice(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

#ifdef WIN32
    UINT32 iLen;
#endif
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}

    if (pDeviceData->stDeviceID.privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	return WIMAX_API_RET_PERMISSION_DENIED;
    if (FALSE == pDeviceData->u32SearchComplete)
	{
	    WiMAXCommonAPI_AbortSearch();
	}
    if (pWiMAX->m_bIdleMode)
	{
	    //Device is in idle mode. 
	    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForIdleModeResponse));
	    WiMAXCommonAPI_SendIdleModeWakeupRequest();

	}
    WiMAXCommonAPI_NwSearchPreconditioning();

#ifdef WIN32
    iLen = WiMAXCommonAPI_FillWiMAXMsg(pWiMAXMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_RESET_REQ, 0);
    if (SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pWiMAXMessage, iLen))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\nFailed to send the RESET Message to device!!");
	    return WIMAX_API_RET_FAILED;
	}
#endif
    Event_Reset(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload));
    WiMAXCommonAPI_FirmwareDownload();
    if (Event_Wait
	(&(pWiMAX->m_WiMAXCommonAPI_WaitForFirmwareDownload),
	 (FIRMWARE_DOWNLOAD_TIMEOUT * 10)) == WAIT_TIMEOUT)
	{
	    return WIMAX_API_RET_FAILED;
	}

    if (FALSE == pDeviceData->u32SearchComplete)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "\n Error while firmware download!!!!!!!!!");
	    WiMAXCommonAPI_AbortSearch();
	}
    else
	{

	    WiMAXCommonAPI_StartSearch();
	}
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET CmdResetToFactorySettings(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetErrorString(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_RET errorCode,
			     WIMAX_API_WSTRING buffer, UINT32 * pLength)
{
    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(errorCode);
    UNUSED_ARGUMENT(buffer);
    UNUSED_ARGUMENT(pLength);

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET SetServiceProviderUnLock(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_WSTRING lockCode)
{
    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(lockCode);

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetServiceProviderLockStatus(WIMAX_API_DEVICE_ID_P pDeviceId,
					   WIMAX_API_LOCK_STATUS_P pLockStatus,
					   UCHAR16 NSPName[MAX_SIZE_OF_NSP_NAME])
{
    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(pLockStatus);
    UNUSED_ARGUMENT(NSPName);

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetNetworkList(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_NSP_INFO_P pNSPInfo,
			     UINT32 * pArrayLength)
{

    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32UnicodeOperatorLength = 0;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "GetNetworkList");
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pArrayLength)
	return WIMAX_API_RET_INVALID_PARAMETER;

    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (pNSPInfo == NULL)
	{
	    *pArrayLength = 0;
	    return WIMAX_API_RET_SUCCESS;
	}

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}

    if (pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_RF_OFF_SW)
	{
	    *pArrayLength = 0;
	    return WIMAX_API_IN_RF_OFF_MODE;
	}

    if (*pArrayLength == 0)
	{
	    *pArrayLength = pDeviceData->u32TotalOperatorsFound;
	    return WIMAX_API_RET_BUFFER_SIZE_TOO_SMALL;
	}
    if (pWiMAX->m_bIdleMode)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
	    WiMAXCommonAPI_WakeupDeviceFromIdleMode();
	}

    if (pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_ONLY)
	{
	    LINK_ERROR_CODES eLink_Error_codes;
	    UINT32 u32Ind = 0, u32NAPID = 0;

	    eLink_Error_codes = WiMAXCommonAPI_GetLinkStatus();

	    switch (eLink_Error_codes)
		{
		  case LINKUP_ACHIEVED:	//In Network Entry state
		  case IN_SLEEP_MODE:
		  case IN_IDLE_MODE:
		      pNSPInfo->structureSize = sizeof(WIMAX_API_NSP_INFO);
		      pNSPInfo->CINR = (CFG_UINT8) (pDeviceData->s32CINR + 10);
		      pNSPInfo->RSSI = (CFG_UINT8) (pDeviceData->s32RSSI + 123);
		      pNSPInfo->networkType = WIMAX_API_HOME;

		      /*Check for NAPID here */
		      while (u32Ind < 3)
			  {
			      u32NAPID |= pDeviceData->stLinkStatus.BsInfo.au8BSID[u32Ind];
			      u32NAPID = u32NAPID << 8;
			      u32Ind++;
			  }
		      u32NAPID = u32NAPID >> 8;

		      pNSPInfo->NSPid = u32NAPID;

		      *pArrayLength = 1;
		      break;
		  default:	// Not in Net entry state
		      break;
		}
	    goto return_path;
	}

    if ((pDeviceData->stCaplSearchRsp.u32NumBS == 0) &&
	(pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_RF_OFF_SW))
	{

	    *pArrayLength = 0;
	    return WIMAX_API_RET_SUCCESS;
	}

    pNSPInfo->structureSize = sizeof(WIMAX_API_NSP_INFO);
    pNSPInfo->CINR = (CFG_UINT8) (pDeviceData->stCaplSearchRsp.bsInfo[0].s32CINR[0] + 10);
    pNSPInfo->RSSI = (CFG_UINT8) (pDeviceData->stCaplSearchRsp.bsInfo[0].s32RSSI[0] + 123);
    pNSPInfo->networkType = WIMAX_API_HOME;
    ConvertToUnicode((char *) pDeviceData->stNetworkParameters[0].OperatorName, pNSPInfo->NSPName,
		     &u32UnicodeOperatorLength);
    pNSPInfo->NSPid = pDeviceData->stNetworkParameters[0].H_NSP[0].H_NSP_ID;
    *pArrayLength = 1;

#ifdef ScanningTime
#ifdef WIN32
    OutputDebugString("\nGetNetworkList: Beceem Device search result returned\n");
#endif
#endif
  return_path:
    return WIMAX_API_RET_SUCCESS;
}



WIMAX_API_RET CmdConnectToNetwork(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_WSTRING nspName,
				  WIMAX_API_PROFILE_ID profileId, WIMAX_API_WSTRING password)
{

    BSINFO stBSToConnect;
    UCHAR16 uchNSPName[MAX_SIZE_OPERATORNAME] = { 0 };
    UINT nLen = 0;

    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(profileId);

    memset(&stBSToConnect, 0, sizeof(stBSToConnect));

#ifdef ScanningTime
#ifdef WIN32
    OutputDebugString("\nCmdConnectToNetwork: Beceem Device trying to make connection\n");
#endif
#endif
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (pDeviceId->privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	return WIMAX_API_RET_PERMISSION_DENIED;
    if (!pDeviceId || !nspName || !password)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    //sync up and linkup
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}
    if (pDeviceData->u32RfOffFromUser || pWiMAX->m_bModemInShutdown || pWiMAX->m_bIdleMode)
	{
	    return WIMAX_API_RET_FAILED;
	}

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "\n CmdConnectToNetwork \n");
    if ((pDeviceData->eDeviceStatus == WIMAX_API_DEVICE_STATUS_Data_Connected))
	{
	    return WIMAX_API_RET_SUCCESS;
	}
    if (!ConvertToUnicode
	((char *) pDeviceData->stNetworkParameters[0].OperatorName, (PUSHORT) uchNSPName, &nLen))
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "ConvertToUnicode failed");
	}

    if (memcmp(uchNSPName, nspName, nLen * sizeof(UCHAR16)) != 0)
	{
	    return WIMAX_API_RET_INVALID_PARAMETER;
	}

    if (FALSE == pDeviceData->u32SearchComplete)
	{
	    WiMAXCommonAPI_AbortSearch();
	}
    pDeviceData->u32RxvdCmdConnectToNetwork = TRUE;
    pDeviceData->u32RfOffFromUser = FALSE;
    return WiMAXCommonAPI_SyncUpRequest(pWiMAXMessage, &stBSToConnect);

}


WIMAX_API_RET CmdDisconnectFromNetwork(WIMAX_API_DEVICE_ID_P pDeviceId)
{
//link down

    INT iRetVal = 0;
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (pDeviceId->privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	return WIMAX_API_RET_PERMISSION_DENIED;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    iRetVal = WiMAXCommonAPI_StartSearch();
    g_bInitiateNetworkEntry = FALSE;

    if (iRetVal)
	return WIMAX_API_RET_FAILED;
    else
	return WIMAX_API_RET_SUCCESS;
}




WIMAX_API_RET CmdNetworkSearchWideScan(WIMAX_API_DEVICE_ID_P pDeviceId)
{
/*
- Compiling out widescan search code. 
As it is no more required to be supported.
*/
#ifdef WIDESCAN_SUPPORTED
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "CmdNetworkSearchWideScan");
    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	return WIMAX_API_RET_INVALID_DEVICE;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    while (pWiMAX->m_bDeviceInitInProgress)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Device INIT is in progress ");
#ifndef WIN32
	    usleep(200000);
#else
	    Sleep(200);
#endif
	}

    if (pWiMAX->m_bWimaxCommonAPI_SearhThreadRunning)
	{
	    WiMAXCommonAPI_AbortSearch();
	}
    else
	{

	    pWiMAX->m_bKillWimaxCommonAPI_SearhThread = FALSE;
	}

    WiMAXCommonAPI_GetNetworkParameterNode();

    if (WiMAXCommonAPI_PrioritizedCAPL_for_WideScan())
	return WIMAX_API_RET_FAILED;

    pDeviceData->bOnWideScanSearch = TRUE;
    WiMAXCommonAPI_NwSearchPreconditioning();
    WiMAXCommonAPI_IssueWideScanSearch((UINT32) WIDESCAN_START_FREQ, (UINT32) WIDESCAN_END_FREQ,
				       (UINT32) WIDESCAN_FREQ_STEP);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Widescan search thread started");
    return WIMAX_API_RET_SUCCESS;
#else
    UNUSED_ARGUMENT(pDeviceId);

    return WIMAX_API_RET_FAILED;
#endif

}

WIMAX_API_RET GetIPInterfaceIndex(WIMAX_API_DEVICE_ID_P pDeviceId,
				  WIMAX_API_INTERFACE_INFO_P pInterfaceInfo)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (WiMAXCommonAPI_GetIPInterfaceIndex(pInterfaceInfo))
	{
	    return WIMAX_API_RET_SUCCESS;
	}
    else
	return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetSelectProfileList(WIMAX_API_DEVICE_ID_P pDeviceId,
				   WIMAX_API_PROFILE_INFO_P pProfileList, UINT32 * pListSize)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(pProfileList);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (pListSize == NULL)
	return WIMAX_API_RET_FAILED;
    else
	{
	    *pListSize = 0;
	    return WIMAX_API_RET_SUCCESS;
	}
}
WIMAX_API_RET GetLinkStatus(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_LINK_STATUS_INFO_P pLinkStatus)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}

    if ((pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected) ||
	pWiMAX->m_bModemInShutdown || pDeviceData->u32RfOffFromUser)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "eDeviceStatus %d m_bIdleMode %d m_bModemInShutdown %d",
			     pDeviceData->eDeviceStatus, pWiMAX->m_bIdleMode,
			     pWiMAX->m_bModemInShutdown);

	    return WIMAX_API_RET_LINK_NOT_CONNECTED;
	}



    WiMAXCommonAPI_GetLinkStatus();
    if (pWiMAX->m_bIdleMode)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
	    /*WiMAXCommonAPI_WakeupDeviceFromIdleMode();*/
	    pDeviceData->stLinkStatus.BsInfoIsValidFlag = TRUE;
	}

    if (pDeviceData->stLinkStatus.BsInfoIsValidFlag)
	{
	    pLinkStatus->structureSize = sizeof(WIMAX_API_LINK_STATUS_INFO);
	    memcpy(pLinkStatus->bsId, pDeviceData->stLinkStatus.BsInfo.au8BSID, 6);
	    pLinkStatus->CINR = (UINT8) (pDeviceData->s32CINR + 10);
	    pLinkStatus->RSSI = (UINT8) (pDeviceData->s32RSSI + 123);
	    pLinkStatus->txPWR = (pDeviceData->uchTransmitPower + 84) * 2;

	    pLinkStatus->centerFrequency = pDeviceData->u32CenterFrequency;

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "devicestatus %d and BSID %02X %02X %02X %02X %02X %02X",
			     pDeviceData->eDeviceStatus, pLinkStatus->bsId[0], pLinkStatus->bsId[1],
			     pLinkStatus->bsId[2], pLinkStatus->bsId[3], pLinkStatus->bsId[4],
			     pLinkStatus->bsId[5]);

	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "CINR %d RSSI %d txpower %d CF %d", pLinkStatus->CINR,
			     pLinkStatus->RSSI, pLinkStatus->txPWR, pLinkStatus->centerFrequency);

	    if (pWiMAX->m_bIdleMode)
		{
		    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
		    pDeviceData->stLinkStatus.BsInfoIsValidFlag = FALSE;
		}

	    return WIMAX_API_RET_SUCCESS;
	}

    return WIMAX_API_RET_LINK_NOT_CONNECTED;
}

WIMAX_API_RET GetDeviceInformation(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_DEVICE_INFO_P pDeviceInfo)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pDeviceInfo)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}

    if (pDeviceData->stDeviceInfo.structureSize > pDeviceInfo->structureSize)
	return WIMAX_API_RET_INVALID_PARAMETER;

    memcpy(pDeviceInfo, &pDeviceData->stDeviceInfo, sizeof(WIMAX_API_DEVICE_INFO));

    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET GetDeviceStatus(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_DEVICE_STATUS_P pDeviceStatus,
			      WIMAX_API_CONNECTION_PROGRESS_INFO_P pConnectionProgressInfo)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pDeviceStatus || !pConnectionProgressInfo)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}
    if (pWiMAX->m_bIdleMode)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
	    WiMAXCommonAPI_WakeupDeviceFromIdleMode();
	}

    *pConnectionProgressInfo = pDeviceData->eConnectionProgress;
    *pDeviceStatus = pDeviceData->eDeviceStatus;

    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET GetConnectedNSP(WIMAX_API_DEVICE_ID_P pDeviceId,
			      WIMAX_API_CONNECTED_NSP_INFO_P pConnectedNSP)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32UnicodeOperatorLength = 0;

    UNUSED_ARGUMENT(pDeviceId);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    if (!pDeviceId || !pConnectedNSP)
	{
	    return WIMAX_API_RET_FAILED;
	}
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    memset(pConnectedNSP, 0, sizeof(WIMAX_API_CONNECTED_NSP_INFO));

    if (pWiMAX->m_bIdleMode)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
	    WiMAXCommonAPI_WakeupDeviceFromIdleMode();

	}
    if ((pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected) ||
	(!pDeviceData->stLinkStatus.BsInfoIsValidFlag) ||
	pDeviceData->u32FlagForDeviceInsertMonitorRunning || pDeviceData->u32RfOffFromUser)
	{
	    return WIMAX_API_RET_LINK_NOT_CONNECTED;
	}

    pConnectedNSP->structureSize = sizeof(WIMAX_API_CONNECTED_NSP_INFO);
    /*Read from OMADM tree */
    pConnectedNSP->activated = FALSE;
    if (pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
	{
	    WiMAXCommonAPI_GetProvisionedState((PUINT32) & pConnectedNSP->activated);
	}
    pConnectedNSP->CINR = (UINT8) pDeviceData->s32CINR + 10;
    pConnectedNSP->RSSI = (UINT8) pDeviceData->s32RSSI + 123;
    pConnectedNSP->networkType = WIMAX_API_HOME;
    if (pDeviceId->privilege == WIMAX_API_PRIVILEGE_READ_WRITE)
	{
	    ConvertToUnicode((char *) pDeviceData->stNetworkParameters[0].OperatorName,
			     pConnectedNSP->NSPName, &u32UnicodeOperatorLength);
	    ConvertToUnicode((char *) pDeviceData->stNetworkParameters[0].OperatorName,
			     pConnectedNSP->NSPRealm, &u32UnicodeOperatorLength);
	    pConnectedNSP->NSPid = pDeviceData->stNetworkParameters[0].H_NSP[0].H_NSP_ID;
	}
    else
	{
	    UINT32 u32Ind = 0, u32NAPID = 0;

	    while (u32Ind < 3)
		{
		    u32NAPID |= pDeviceData->stLinkStatus.BsInfo.au8BSID[u32Ind];
		    u32NAPID = u32NAPID << 8;
		    u32Ind++;
		}
	    u32NAPID = u32NAPID >> 8;
	    pConnectedNSP->NSPid = u32NAPID;
	}
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SetRoamingMode(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_ROAMING_MODE roamingMode)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    //PREVILIEGE_VALIDATION /*Validate the access rights*/
    if (pDeviceId->privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	return WIMAX_API_RET_PERMISSION_DENIED;
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}
    pDeviceData->eRoamingMode = roamingMode;
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET GetRoamingMode(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_ROAMING_MODE_P pRoamingMode)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    //PREVILIEGE_VALIDATION /*Validate the access rights*/  
    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}
    *pRoamingMode = pDeviceData->eRoamingMode;
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET GetStatistics(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_CONNECTION_STAT_P pStatistics)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    UINT32 u32StatsSize = 0;
    PVOID pConnectionStats = NULL;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pStatistics)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData->hDevice || !pDeviceId->devicePresenceStatus)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "hDevice %p and devicePresenceStatus %d",
			     (INT *) pDeviceData->hDevice, pDeviceId->devicePresenceStatus);
	    return WIMAX_API_RET_INVALID_DEVICE;
	}

    u32StatsSize = pStatistics->structureSize;
    if (sizeof(WIMAX_API_CONNECTION_STAT) == u32StatsSize)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "No packing enforced Size %d", u32StatsSize);
	}
    else if (sizeof(WIMAX_API_CONNECTION_STAT_4) == u32StatsSize)
	{
	    pConnectionStats = (PVOID) pStatistics;
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "compiled for 4 byte packing size %d", u32StatsSize);

	}
    else
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "Un supported stats struct size %d", u32StatsSize);
	    return WIMAX_API_RET_INVALID_PARAMETER;
	}

    if (pWiMAX->m_bIdleMode)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "API called on idle mode ");
	    /*if (WiMAXCommonAPI_WakeupDeviceFromIdleMode() != SUCCESS)
		{
		    return WIMAX_API_RET_FAILED;
		}*/

	}
    if ((pDeviceData->eDeviceStatus != WIMAX_API_DEVICE_STATUS_Data_Connected)
	|| pWiMAX->m_bModemInShutdown || pDeviceData->u32FlagForDeviceInsertMonitorRunning ||
	pDeviceData->u32RfOffFromUser)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "eDeviceStatus %d m_bIdleMode %d m_bModemInShutdown %d",
			     pDeviceData->eDeviceStatus, pWiMAX->m_bIdleMode,
			     pWiMAX->m_bModemInShutdown);

	    return WIMAX_API_RET_LINK_NOT_CONNECTED;
	}
    if (!pConnectionStats)
	{
	    memcpy(pStatistics, &pDeviceData->stStatisticsInfo, sizeof(WIMAX_API_CONNECTION_STAT));
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "RX byts %llu  RX PKTS %llu  TX byts %llu TX PKTS %llu",
			     pStatistics->totalRxByte, pStatistics->totalRxPackets,
			     pStatistics->totalTxByte, pStatistics->totalTxPackets);
	}
    else
	{
	    ((WIMAX_API_CONNECTION_STAT_P_4) pConnectionStats)->totalRxByte =
		pDeviceData->stStatisticsInfo.totalRxByte;
	    ((WIMAX_API_CONNECTION_STAT_P_4) pConnectionStats)->totalRxPackets =
		pDeviceData->stStatisticsInfo.totalRxPackets;
	    ((WIMAX_API_CONNECTION_STAT_P_4) pConnectionStats)->totalTxByte =
		pDeviceData->stStatisticsInfo.totalTxByte;
	    ((WIMAX_API_CONNECTION_STAT_P_4) pConnectionStats)->totalTxPackets =
		pDeviceData->stStatisticsInfo.totalTxPackets;
	}
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET GetProvisioningStatus(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_WSTRING nspName,
				    BOOL * pProvisoningStatus)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(pProvisoningStatus);
    UNUSED_ARGUMENT(nspName);


    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetContactInformation(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_WSTRING nspName,
				    WIMAX_API_CONTACT_INFO_P pContactInfo, UINT32 * pSizeOfContactList)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(nspName);
    UNUSED_ARGUMENT(pSizeOfContactList);
    UNUSED_ARGUMENT(pContactInfo);

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET GetPackageInformation(WIMAX_API_DEVICE_ID_P pDeviceId,
				    WIMAX_API_PACKAGE_INFO_P pPackageInfo)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(pPackageInfo);

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SetPackageUpdateState(WIMAX_API_DEVICE_ID_P pDeviceId,
				    WIMAX_API_PACKAGE_UPDATE_STATE packageUpdateState)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    UNUSED_ARGUMENT(pDeviceId);
    UNUSED_ARGUMENT(packageUpdateState);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    return WIMAX_API_RET_FAILED;
}

WIMAX_API_RET SubscribeDeviceStatusChange(WIMAX_API_DEVICE_ID_P pDeviceId,
					  IndDeviceStatusUpdate pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeDeviceInsertRemove(WIMAX_API_DEVICE_ID_P pDeviceId,
					  IndDeviceInsertRemove pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeControlPowerManagement(WIMAX_API_DEVICE_ID_P pDeviceId,
					      IndControlPowerManagement pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndControlPowerManagement = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeConnectToNetwork(WIMAX_API_DEVICE_ID_P pDeviceId,
					IndConnectToNetwork pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeDisconnectToNetwork(WIMAX_API_DEVICE_ID_P pDeviceId,
					   IndDisconnectToNetwork pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeNetworkSearchWideScan(WIMAX_API_DEVICE_ID_P pDeviceId,
					     IndNetworkSearchWideScan pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearchWideScan = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeProvisioningOperation(WIMAX_API_DEVICE_ID_P pDeviceId,
					     IndProvisioningOperation pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndProvisioningOperation = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribePackageUpdate(WIMAX_API_DEVICE_ID_P pDeviceId, IndPackageUpdate pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndPackageUpdate = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeDeviceStatusChange(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceStatusUpdate = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeDeviceInsertRemove(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDeviceInsertRemove = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeControlPowerManagement(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}
    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndControlPowerManagement = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeConnectToNetwork(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndConnectToNetwork = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeDisconnectToNetwork(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndDisconnectToNetwork = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeNetworkSearchWideScan(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearchWideScan = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeProvisioningOperation(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndProvisioningOperation = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribePackageUpdate(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndPackageUpdate = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}


UINT RegisterBeceemProprietaryCallback(PVOID pCallbackFnPtr, PVOID pvCallbackContext)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return (UINT) ERROR_LIB_INITIALIZATION;
	}


    if (!GpWiMAX)
	return (UINT) ERROR_LIB_INITIALIZATION;

    if (!pCallbackFnPtr)
	return (UINT) ERROR_INVALID_NOTIFY_ROUTINE;

    Mutex_Lock(&pDeviceData->stProprietaryCallback.objCallbackMutex);
    pDeviceData->stProprietaryCallback.pAppCallback = (PFNBCMCALLBACK) pCallbackFnPtr;
    pDeviceData->stProprietaryCallback.pAppCallbackContext = pvCallbackContext;
    Mutex_Unlock(&pDeviceData->stProprietaryCallback.objCallbackMutex);

    return SUCCESS;
}

INT SendBeceemProprietaryMessage(PUCHAR pucSendBuffer, UINT uiLength)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    PWIMAX_MESSAGE pWiMAXMsg = (PWIMAX_MESSAGE) pucSendBuffer;
    INT iRet = 0;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return ERROR_LIB_INITIALIZATION;
	}



    if (!GpWiMAX)
	return ERROR_LIB_INITIALIZATION;

    if (!pucSendBuffer)
	return ERROR_INVALID_BUFFER;
    if (!pDeviceData->hDevice)
	return ERROR_HANDLE_INVALID;
    if (!pDeviceData->stProprietaryCallback.pAppCallback)
	return ERROR_INVALID_NOTIFY_ROUTINE;

    if (uiLength == 0 || (INT) uiLength > SIZE_WIMAX_MESSAGE)
	return ERROR_INVALID_BUFFER_LEN;

    pDeviceData->stProprietaryCallback.usTYPE = pWiMAXMsg->usType;
    pDeviceData->stProprietaryCallback.usSUBTYPE = pWiMAXMsg->usSubType;
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d ", __FUNCTION__, __LINE__);

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "TYPE %d AND SUBTYPE %d SENT ", pWiMAXMsg->usType,
		     pWiMAXMsg->usSubType);
    if (pDeviceData->u32RfOffFromUser)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR, "This is an invalid request");
	    return ERROR_MODEM_IN_SHUT_DOWN_MODE;
	}

    iRet = BeceemWiMAX_SendMessage(GpWiMAX, (PWIMAX_MESSAGE) pucSendBuffer, uiLength);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d return value is %d", __FUNCTION__, __LINE__,
		     iRet);
    return iRet;

}

INT RefreshOMAChannelPlan()
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    INT32 bStartSearch = FALSE;
    UINT8 u8MruResetFlag = TRUE;
    INT iRet = SUCCESS;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    iRet = ERROR_API_NOT_SUPPORTED;
	    goto return_path_no_unlock;
	}
    if (!pDeviceData->hDevice)
	{
	    iRet = ERROR_HANDLE_INVALID;
	    goto return_path_no_unlock;
	}

    Mutex_Lock(&(pWiMAX->m_CAPINWSearchMutex));

    if (FALSE == pDeviceData->u32SearchComplete)
	{
	    WiMAXCommonAPI_AbortSearch();
	    bStartSearch = 1;
	    /* If a search is in progress, abort the search 
	       and set this flag to re issue search request after resetting all Tables */

	}
    WiMAXCommonAPI_GetNetworkParameterNode();

    if (!WiMAXCommonAPI_ResetMRUTable(u8MruResetFlag))
	{
	    iRet = ERROR_API_NOT_SUPPORTED;
	    goto return_path;
	}


    iRet = WiMAXCommonAPI_InitCAPLTable();
    if (iRet)
	{

	    goto return_path;
	}
    iRet = WiMAXCommonAPI_InitChannelPlanTable();
    if (iRet)
	{
	    goto return_path;
	}
    Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));

    if (bStartSearch)
	{
	    Event_Set(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
	}
    goto return_path_no_unlock;
  return_path:
    Mutex_Unlock(&(pWiMAX->m_CAPINWSearchMutex));

  return_path_no_unlock:
    return iRet;
}

WIMAX_API_RET GetConnectionTime(WIMAX_API_DEVICE_ID_P pDeviceId, double *pConnectionMode)
{
    PWIMAX_MESSAGE pstRequestMessage = NULL;
    INT iLen = 0, iRet = 0;
    WIMAX_API_RET eWiMAXAPIRet = WIMAX_API_RET_SUCCESS;

    UNUSED_ARGUMENT(pDeviceId);
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);

    pstRequestMessage = (PWIMAX_MESSAGE) calloc(1, sizeof(WIMAX_MESSAGE));

    iLen =
	WiMAXCommonAPI_FillWiMAXMsg(pstRequestMessage, TYPE_NETWORK_ENTRY,
				    SUBTYPE_TIME_ELAPSED_SINCE_NETWORK_ENTRY, sizeof(DOUBLE));
    iRet = SendWiMAXMessage(pDeviceData->hDevice, (PUCHAR) pstRequestMessage, iLen);

    if (iRet)
	eWiMAXAPIRet = WIMAX_API_RET_FAILED;


    memcpy(pConnectionMode, pstRequestMessage->szData, sizeof(DOUBLE));

    if (pstRequestMessage)
	free(pstRequestMessage);

    return eWiMAXAPIRet;
}

WIMAX_API_RET CmdNetworkSearch(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;
    PREVILIEGE_VALIDATION;	/*Validate the access rights */
    if (pDeviceId->privilege != WIMAX_API_PRIVILEGE_READ_WRITE)
	return WIMAX_API_RET_PERMISSION_DENIED;
    if (!pDeviceData || pDeviceData->u32RfOffFromUser)
	{
	    DPRINT_COMMONAPI(DEBUG_ERROR,
			     "Invoking search when initiated RF OFF ? RF OFF INITIATED STATUS %d",
			     pDeviceData->u32RfOffFromUser);
	    return WIMAX_API_RET_FAILED;
	}
    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);

    WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError();
    switch (pDeviceData->eDeviceStatus)
	{
	  case WIMAX_API_DEVICE_STATUS_RF_OFF_HW:
	  case WIMAX_API_DEVICE_STATUS_RF_OFF_SW:
	  case WIMAX_API_DEVICE_STATUS_RF_OFF_HW_SW:
	      DPRINT_COMMONAPI(DEBUG_MESSAGE, " %s Invoked on RF OFF", __FUNCTION__);
	      return WIMAX_API_RET_FAILED;
	  case WIMAX_API_DEVICE_STATUS_Data_Connected:
	  case WIMAX_API_DEVICE_STATUS_Connecting:
	      WiMAXCommonAPI_NwSearchPreconditioning();
	      break;
	  case WIMAX_API_DEVICE_STATUS_Scanning:
	      WiMAXCommonAPI_AbortSearch();
	      break;

	  default:
	      break;
	}

    if (WiMAXCommonAPI_StartSearch() != SUCCESS)
	return WIMAX_API_RET_FAILED;
    else
	return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET SubscribeNetworkSearch(WIMAX_API_DEVICE_ID_P pDeviceId, IndNetworkSearch pCallbackFnc)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Entered %s and %d", __FUNCTION__, __LINE__);
    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId || !pCallbackFnc)
	return WIMAX_API_RET_INVALID_PARAMETER;

    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;
    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch = pCallbackFnc;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;
}

WIMAX_API_RET UnsubscribeNetworkSearch(WIMAX_API_DEVICE_ID_P pDeviceId)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;

    if (!pWiMAX->m_bCommonAPIIfActive)
	{
	    /*API is Not opened yet */
	    return WIMAX_API_RET_FAILED;
	}

    if (!pDeviceId)
	return WIMAX_API_RET_INVALID_PARAMETER;


    if (!pDeviceData)
	return WIMAX_API_RET_FAILED;

    Mutex_Lock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    pDeviceData->stCallBackFunctionCommonAPIs.pIndNetworkSearch = NULL;
    Mutex_Unlock(&pDeviceData->stCallBackFunctionCommonAPIs.objCallbackMutex);
    return WIMAX_API_RET_SUCCESS;

}

INT SetChannelPlanParamsForNDnS(PVOID pST_OperatorChannelPlanSettings,
				UINT32 u32SizeOfChannelPlanSettings)
{
    //
    //If Search thread is running stop it.
    //Set search parameters as is specified in the channel plan settings.
    //
    P_ST_OperatorChannelPlanSettings pOperatorChannelPlanSettings =
	(P_ST_OperatorChannelPlanSettings) pST_OperatorChannelPlanSettings;

    WIMAX_API_RET iRet = WIMAX_API_RET_FAILED;




    if ((pDeviceData->u32SizeOfChannelPlanSettings != u32SizeOfChannelPlanSettings) &&
	pDeviceData->pOperatorChannelPlanSettings)
	{
	    BECEEM_MEMFREE(pDeviceData->pOperatorChannelPlanSettings);
    }
    if (!pDeviceData->pOperatorChannelPlanSettings)
	{
	    BECEEMCALLOC(pDeviceData->pOperatorChannelPlanSettings,
			 P_ST_OperatorChannelPlanSettings, UCHAR, u32SizeOfChannelPlanSettings)
		pDeviceData->u32SizeOfChannelPlanSettings = u32SizeOfChannelPlanSettings;

	}
    memcpy(pDeviceData->pOperatorChannelPlanSettings, pOperatorChannelPlanSettings,
	   u32SizeOfChannelPlanSettings);

    if (pDeviceData->hDevice)
	{
	    DPRINT_COMMONAPI(DEBUG_MESSAGE, "Device is already openend!!");
	    RefreshOMAChannelPlan();
	}

    iRet = WIMAX_API_RET_SUCCESS;

    return iRet;
}

WIMAX_API_RET SetSearchThreshold(SINT32 s32RSSI,SINT32 s32CINR)
{
	pDeviceData->s32RSSIThreshold = s32RSSI;
	pDeviceData->s32CINRThreshold = s32CINR;

	return WIMAX_API_RET_SUCCESS;

}