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
 * Description	:	Receive control message handler.
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h"
#include "globalobjects.h"
VOID BeceemWiMAX_NotificationRoutine(
					PVOID pContext,
					DWORD dwErrorCode,
					DWORD dwBytesTransfered
				)
{
	USHORT usSwStatus = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pContext;
	if(!pWiMAX)
	{
		Event_Set(&(pWiMAX->m_objReadFileEvent));
		return;
	}
    HEX_DUMP(pWiMAX->m_szMsg,dwBytesTransfered);
	if(dwErrorCode)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Error code received from the driver!. Error code : %d",
		   (INT) dwErrorCode);
		if(pWiMAX->m_bFirmwareDownloadInProgress)
			return ;
#ifndef WIN32
		Mutex_Lock(&pWiMAX->m_DeviceRemovalIssuedMutex);
		BeceemWiMAX_ProcessUnloadDevResponse(pWiMAX);
		Mutex_Unlock(&pWiMAX->m_DeviceRemovalIssuedMutex);
		DeviceIO_CloseHandle(&pWiMAX->m_Device); 
#endif
		return;
	}

    /* Check for length */
    if(dwBytesTransfered < sizeof(USHORT))
    {
		Event_Set(&(pWiMAX->m_objReadFileEvent));
        return;
    }

	
   	if (dwBytesTransfered > sizeof(LEADER_ST)+sizeof(usSwStatus)+MAX_MGMT_MESSAGE_LENGTH)
   	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "PAYLOAD has Invalid Length.");
		return;
   	}

	usSwStatus=*((PUSHORT)(pWiMAX->m_szMsg + pWiMAX->u32OffsetForUnalignedAccessCorrection));

	/* Returning if bit in bitmap is reset for SwStatus received */
	if(((pWiMAX->m_uiControlMsgEnableMask>>(usSwStatus&SWSTATUS_BIT_MASK))&1)==0)
		return;

	if(pWiMAX->m_bControlMessageLogging)
	{
		BeceemWiMAX_ProcessControlMsgResponse(pWiMAX, dwBytesTransfered);				
	}
		

	switch(usSwStatus) 
	{
		case LINK_CONTROL_RESP:
		case WTM_CONTROL_RESPONSE:
			BeceemWiMAX_ProcessLinkControlResponse(pWiMAX, dwBytesTransfered);
			break;

		case STATUS_RSP:
	        DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "RECEIVED STATUS_RSP");
			BeceemWiMAX_ProcessStatusResponseMessage(pWiMAX,dwBytesTransfered);
			break;

		/*case UNLOAD_DEVICE:
			BeceemWiMAX_ProcessUnloadDevResponse(pWiMAX);
			break;*/
	
		case MGMT_MSG_INFO_SW_STATUS:
			BeceemWiMAX_ProcessMgmtInfoResponse(pWiMAX, dwBytesTransfered);
			break;

		case SERVICE_FLOW_MSG_RESP:
			BeceemWiMAX_ProcessServiceFlowResponse(pWiMAX, dwBytesTransfered);
			break;

		case AUTH_SS_HOST_MSG:
			BeceemWiMAX_ProcessAuthMessageResponse(pWiMAX);
			break;
		case SLEEP_MODE_IND:
			BeceemWiMAX_ProcessSleepModeResponse(pWiMAX, dwBytesTransfered);
			break;
			
		case MAC_LOGGING_NOTIFICATION:
			if(pWiMAX->m_bEnableMACLogging)
			{
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
			     "\n***************** Received MAC_LOGGING_NOTIFICATION **********************************************\n");
				BeceemWiMAX_ProcessMACLOggingNotification(pWiMAX,dwBytesTransfered);
			}
			break;	

		case CID_LOGGING_NOTIFICATION:
			  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
			     "\n***************** Received CID_LOGGING_NOTIFICATION **********************************************\n");
                BeceemWiMAX_ProcessCIDLoggingNotification(pWiMAX,dwBytesTransfered);
	
			break;

		default:
		/*	DPRINT(DEBUG_ERROR, "UNKNOWN response message received from driver MsgType : 0x%x.",usSwStatus); */
			break;
	}
	Event_Set(&(pWiMAX->m_objReadFileEvent));
}

int BeceemWiMAX_ProcessStatusResponseMessage(PVOID pArg,DWORD dwBytesTransferred)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UCHAR  ucType = 0;
	PUCHAR	pucPayloadData = 0;
	USHORT usSwStatus=0;

	pucPayloadData = (PUCHAR)(pWiMAX->m_szMsg +  sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);

	usSwStatus=*((PUSHORT)(pWiMAX->m_szMsg + pWiMAX->u32OffsetForUnalignedAccessCorrection));

	ucType = *(UCHAR *)pucPayloadData;

    if(DISCONNECT_LOGGING_RESP==ucType ||START_CID_SNOOPING_RSP==ucType )
    {
        return BeceemWiMAX_Resync_CID_Response(pWiMAX, pucPayloadData,dwBytesTransferred,usSwStatus);
    }

	if(MAC_ADDR_RSP == ucType)
	{
		Event_Set(&(pWiMAX->m_objMACaddressRespEvent)); 
		if(Event_Wait(&(pWiMAX->m_objMACaddressRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Wait on m_objMACaddressRespEvent TIMEDOUT");
		}
		return BeceemWiMAX_SendMACAddressWriteResponse(pWiMAX, pucPayloadData);	
	}
	
	if(SET_WIMAX_OPTIONS_RSP == ucType)
	{
		UINT *pVal = NULL;
		UCHAR ucBitType = 0;

		pucPayloadData++;
		ucBitType = *pucPayloadData;
		Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent)); 
		if(Event_Wait(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
			   "Wait on m_objSetWIMAXOptionsRespEventMsgRecvd TIMEDOUT");
		}
		pucPayloadData++;
		pVal = (UINT*)pucPayloadData;
		//*pVal = ntohl(*pVal);
		
		pWiMAX->m_bWIMAXOptionsSetInProgress = FALSE;
		pWiMAX->m_bKillWIMAXOptionsSetInProgress = 1;

		if ((pWiMAX->m_bSetWiMAXInfoReqFlag == ucBitType) &&
			(pWiMAX->m_iSetWiMAXReqInfo ==(INT) *pVal))
		{
			UCHAR payload[32] = {0};
			SINT32 iVal = 0;

			payload[0] = pWiMAX->m_bSetWiMAXInfoReqFlag;
			memcpy(&payload[1], &iVal, sizeof(SINT32));

			return BeceemWiMAX_SendWimaxOptionsResponse(pWiMAX, payload, (sizeof(UCHAR) + sizeof(UINT32)));	
		}
		else
		{
			UCHAR payload[32] = {0};
			SINT32 iVal = 0;

			payload[0] = pWiMAX->m_bSetWiMAXInfoReqFlag;
			iVal = ERROR_SET_WIMAX_OPTIONS_RESP_FAILED;
			memcpy(&payload[1], &iVal, sizeof(SINT32));

			return BeceemWiMAX_SendWimaxOptionsResponse(pWiMAX, payload, (sizeof(UCHAR) + sizeof(UINT32)));	
		}
	}
	
	if(SET_WIMAX_OPTIONS_ALL_RESP == ucType)
			{
				pucPayloadData++;
				
					if(GET_WIMAX_OPTIONS_ALL == *pucPayloadData)
					{
						UCHAR uchOptionSpecifier;
						pucPayloadData++;
						uchOptionSpecifier = *pucPayloadData;
						Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent)); 
						if(Event_Wait(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
						{
			    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
				   "Wait on m_objSetWIMAXOptionsRespEventMsgRecvd TIMEDOUT");
						}
						pucPayloadData++;
						return BeceemWiMAX_SendAppWiMAXOptionsAll(pWiMAX,pucPayloadData,uchOptionSpecifier,SUCCESS,SUBTYPE_GET_ALL_WIMAX_OPTIONS_RESP);
						
					}
					else if(SET_WIMAX_OPTIONS_ALL == *pucPayloadData)
					{
						UCHAR uchOptionSpecifier;
						pucPayloadData++;
						uchOptionSpecifier = *pucPayloadData;
						Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent)); 
						if(Event_Wait(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
						{
			    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
				   "Wait on m_objSetWIMAXOptionsRespEventMsgRecvd TIMEDOUT");
						}
						pucPayloadData++;
						Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
						
						return BeceemWiMAX_SendAppWiMAXOptionsAll(pWiMAX,pucPayloadData,uchOptionSpecifier,SUCCESS,SUBTYPE_SET_ALL_WIMAX_OPTIONS_RESP);		
					}
			}
	
	return SUCCESS;	
}

int BeceemWiMAX_SetRssiCinrResponse(PVOID pArg, PUCHAR pPayload)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	ST_SET_RSSI_CINR stRSSI_CINR = {0};
	PST_SET_RSSI_CINR pstRSSI_CINR = (PST_SET_RSSI_CINR )pPayload;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	stRSSI_CINR.s32CINR = ntohl(pstRSSI_CINR->s32CINR);
	stRSSI_CINR.s32RSSI = ntohl(pstRSSI_CINR->s32RSSI);
	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_DEVICE_CONFIG, 
						SUBTYPE_SET_RSSI_CINR_THRESHOLD_RESP, 
						(UCHAR *)&stRSSI_CINR,
						sizeof(ST_SET_RSSI_CINR)
						);

	
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;			

}

int BeceemWiMAX_SendWimaxOptionsResponse(PVOID pArg, PUCHAR pPayload, UINT uiLength)
{
	PWIMAXMESSAGE pWMMsg = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_SET_WIMAX_OPTIONS, 
						SUBTYPE_SET_WIMAX_OPTIONS_RESP, 
						(UCHAR *)pPayload,
						uiLength
						);

	
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
int BeceemWiMAX_ProcessMACLOggingNotification(PVOID pArg,DWORD dwBytesTransferred)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PUCHAR	pucPayloadData = NULL;
    PUCHAR pucDataAfterSwapping = NULL;
    int nIndex = 0;
    UINT uiDataBeforeSwapping = 0;
    UINT uiDataAfterSwapping = 0;

    BECEEMCALLOC(pucDataAfterSwapping,PUCHAR,CHAR,dwBytesTransferred)
	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
	if(!pWMMsg)
	{
		BECEEM_MEMFREE(pucDataAfterSwapping)
		return ERROR_MALLOC_FAILED;
	}

    pucPayloadData = (PUCHAR)(pWiMAX->m_szMsg +  sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);
    
    for(nIndex = 0; nIndex < (int)(dwBytesTransferred - sizeof(USHORT)) ;)
    {
       
        memcpy(&uiDataBeforeSwapping,&pucPayloadData[nIndex],sizeof(UINT));
        
        uiDataAfterSwapping =  SWAP_ENDIAN32(uiDataBeforeSwapping);   
        
        memcpy(&pucDataAfterSwapping[nIndex],&uiDataAfterSwapping,sizeof(UINT));
        
        nIndex+=sizeof(UINT);
        
        uiDataBeforeSwapping = 0;
        uiDataAfterSwapping = 0;
    }
     
    
	
	
	
	/* Build the TYPE_MAC_LGGING_INFO and Send it to the application */
	WiMAXMessage_Init(
					pWMMsg,
					TYPE_MAC_LOGING_INFO, 
					SUBTYPE_MAC_LOGING_INFO_INDICATION, 
					pucDataAfterSwapping,
					(dwBytesTransferred - sizeof(USHORT))
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
    BECEEM_MEMFREE(pucDataAfterSwapping)
	BECEEM_MEMFREE(pWMMsg)
	return 0;
	
}
int BeceemWiMAX_ProcessCIDLoggingNotification(PVOID pArg,DWORD dwBytesTransferred)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PUCHAR	pucPayloadData = NULL;
	USHORT usSwStatus;
	UCHAR  ucType = 0;
	
	usSwStatus=*((PUSHORT)(pWiMAX->m_szMsg + pWiMAX->u32OffsetForUnalignedAccessCorrection));

	pucPayloadData = (PUCHAR)(pWiMAX->m_szMsg +  sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);
    
	ucType = *(UCHAR *)(pucPayloadData+4);

	if(ucType==START_CID_SNOOPING_RSP)
	{
	    //Below increment in pointer is for buffer from firmware starts at a differnet location than posted data
		BeceemWiMAX_Resync_CID_Response(pWiMAX,pucPayloadData+4,dwBytesTransferred-4,usSwStatus);
	}	

	else // PHY Logging is not supported any more. Keeping for backward comaptibility
	{
		PWIMAXMESSAGE pOWMMsg = NULL;

		if(pWiMAX->m_bEnablePHYLogging)
		{

			BECEEMCALLOC(pOWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

			/* Build the TYPE_PHY_LOGING_INFO and Send it to the application */
			WiMAXMessage_Init(
							pOWMMsg,
							TYPE_PHY_LOGING_INFO, 
							SUBTYPE_PHY_LOGING_INFO_INDICATION, 
							pucPayloadData,
							(dwBytesTransferred - sizeof(USHORT))
							);

			BeceemWiMAX_CallAppCallback(pWiMAX, pOWMMsg);
			BECEEM_MEMFREE(pOWMMsg)

		}

	}
	


	return SUCCESS;
}
int BeceemWiMAX_ProcessAuthMessageResponse(PVOID pArg)
{
	/* Offset 2 bytes(swStatus). Type will be after swStatus */
	PUCHAR	pucPayloadData = 0;
	UCHAR  ucType = 0;
	USHORT  usLength = 0;

	PWIMAXMESSAGE pWMMsg = NULL;
	EAP_PAYLOAD stEAPPayload = {0};
	ST_EAP_RSA_PRIVATE stEAPEncrypt = {0};
	PST_EAP_RSA_PRIVATE pstEAPEncryptReceived = NULL;
	PST_EAP_RSA_PRIVATE pstEAPEncryptSent = NULL; 
		
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

		BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	pucPayloadData = (PUCHAR)(pWiMAX->m_szMsg +  sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);
	ucType = *(UCHAR *)pucPayloadData;

	switch(ucType)
	{

		case AUTH_REQ_PAYLOAD:
			/* Length is present after 5 bytes */
			memcpy(&usLength, pWiMAX->m_szMsg + 5 + pWiMAX->u32OffsetForUnalignedAccessCorrection, 
						sizeof(USHORT));

			usLength = SWAP16(usLength);

			
			stEAPPayload.usEAPPayloadLength = usLength;
			memcpy(stEAPPayload.aucEAPPayload, pWiMAX->m_szMsg + 3 + pWiMAX->u32OffsetForUnalignedAccessCorrection, 
						usLength);
			
			/* Build the EAP_TRANSFER_INDICATION and Send it to the application */
			WiMAXMessage_Init(	
							pWMMsg,
							TYPE_AUTHENTICATION, 
							SUBTYPE_EAP_TRANSFER_INDICATION, 
							(UCHAR *)&stEAPPayload,
							sizeof(EAP_PAYLOAD) + stEAPPayload.usEAPPayloadLength - sizeof(stEAPPayload.aucEAPPayload) 
							);
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			break;
		case AUTH_ENCRYPT_PVT_RESP:
			pstEAPEncryptReceived = (PST_EAP_RSA_PRIVATE)(pWiMAX->m_szMsg + 3 + 
							pWiMAX->u32OffsetForUnalignedAccessCorrection);
			
			memcpy(&stEAPEncrypt.u32DataLen, &pstEAPEncryptReceived->u32DataLen, sizeof(UINT32));
			memcpy(&stEAPEncrypt.u32Padding, &pstEAPEncryptReceived->u32Padding, sizeof(UINT32));
			stEAPEncrypt.u32DataLen = SWAP_ENDIAN32(stEAPEncrypt.u32DataLen);
			stEAPEncrypt.u32Padding = SWAP_ENDIAN32(stEAPEncrypt.u32Padding);		

			memcpy(stEAPEncrypt.u8AESkey,pstEAPEncryptReceived->u8AESkey,(sizeof(ST_EAP_RSA_PRIVATE)-sizeof(UINT32)*2-1));

			/* Build the SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST and Send it to the application */
			WiMAXMessage_Init(	
							pWMMsg,
							TYPE_AUTHENTICATION, 
							SUBTYPE_EAP_ENCRYPT_PRIVATE_RESPONSE, 
							(UCHAR *)&stEAPEncrypt,
							sizeof(ST_EAP_RSA_PRIVATE)  -1
							);

			pstEAPEncryptSent = (PST_EAP_RSA_PRIVATE)pWMMsg->stMessage.szData;
			
			pWMMsg->uiMessageLen += stEAPEncrypt.u32DataLen;

			memcpy(pstEAPEncryptSent->u8Data,pstEAPEncryptReceived->u8Data,stEAPEncrypt.u32DataLen);
			
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

			break;
	}
	BECEEM_MEMFREE(pWMMsg)
	return 0;
}

int BeceemWiMAX_ProcessUnloadDevResponse(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	Event_Set(&(pWiMAX->m_objReadFileEvent));

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Setting callback thread to exit - ProcessUnloadDevResponse");

	pWiMAX->m_bKillCallbackThread = 1;

	/* Stop Periodic Statstics thread */
	if(Thread_Stop(&(pWiMAX->m_StatsThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillStatsThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Stats thread!!!");
		Thread_Terminate(&(pWiMAX->m_StatsThread)); 
	}

	/* Stop Logger thread */
	if(Thread_Stop(&(pWiMAX->m_LoggerThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillLoggerThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Logger thread!!!");
		Thread_Terminate(&(pWiMAX->m_LoggerThread)); 
	}

	/* Stop NetEntry thread */
	if(Thread_Stop(&(pWiMAX->m_NetEntryThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetEntryThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Net Entry thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_NetEntryThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));
        }
    }

	/* Stop SyncUp thread */
	if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop sync-up thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
        }
	}

	/* Stop Shutdown resp thread */
	if(Thread_Stop(&(pWiMAX->m_ShutDownRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillShutDownRespThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Shutdown Response thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_ShutDownRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objShutDownRespEventMsgRecvd));
        }
	}

	/* Device Init Thread req */
	if(Thread_Stop(&(pWiMAX->m_DeviceInitThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillDeviceInitThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Device Init Event thread!!!");
		Thread_Terminate(&(pWiMAX->m_DeviceInitThread));
	}

	/* Stop Multiple MIB Periodic Indication Thread */
	if(Thread_Stop(&(pWiMAX->m_MultipleMibThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillMultipleMibThread, pWiMAX) != SUCCESS)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Multiple Mib thread!!!");
		Thread_Terminate(&(pWiMAX->m_MultipleMibThread)); 
	}

	DeviceIO_CloseHandle(&pWiMAX->m_Device);	
	BeceemWiMAX_SendAppDeviceUnload(pWiMAX);	
	

	return SUCCESS;
}

int BeceemWiMAX_SendAppDeviceUnload(PVOID pArg)
{
	int iRet = 0;
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
#ifdef WIN32
    UINT32 u32OldVal = 0;
#endif
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

#ifdef WIN32
    u32OldVal = InterlockedCompareExchange(&pWiMAX->m_bUnloadNotified,1,0);

    if(u32OldVal)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Value set to u32OldVal is: [%d]", u32OldVal);
		return SUCCESS;
	}
#endif
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "About to sent UNLOAD Notification to Apps");
	WiMAXMessage_Init(	
						pWMMsg,
						DEVICE_RESET_UNLOAD_TYPE, 
						DEVICE_UNLOAD, 
						NULL, 
						0
					);
	iRet = BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
#ifndef WIN32
	pWiMAX->m_bUnloadNotified = TRUE;
#endif
	BECEEM_MEMFREE(pWMMsg)
	return iRet;
}

int BeceemWiMAX_ProcessLinkControlResponse(PVOID pArg, UINT uiNumBytesRecvd)
{
	BOOL bSyncFail = FALSE;
	PUCHAR pucPayloadData = 0;
	UINT uiPayloadData = 0;
	USHORT usMessageSubType = 0;
	PWIMAXMESSAGE pWMMsg = NULL;
	PS_HOINFO pstHOInfo;
	int status = 0;
	int iRetval=0;
	PST_CAPL_SEARCH_RSP pstCaplRsp = NULL;

	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UNUSED_ARGUMENT(uiNumBytesRecvd);
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	pucPayloadData=(PUCHAR)(pWiMAX->m_szMsg+sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);
	
	uiPayloadData = *(UINT *)pucPayloadData;
	uiPayloadData = htonl(uiPayloadData);
	
	if(uiPayloadData == GO_TO_IDLE_MODE_RSP_PAYLOAD)
	{
		/*
			Target to Host Direction
			Software Status =  WTM_SOFTWARE_STATUS = 0xA3
			Length=8 bytes (4 bytes of IdleModeNotification + 4 bytes NumMilliseconds)excl leader
			PayLoad32BitsInt0: GO_TO_IDLE_MODE_RSP_PAYLOAD    = 210
			Payload32BitsInt1=NumMillisecondsOfPlannedIdleMode

			-Hari
		*/

	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "GOT IDLE MODE RESPONSE FROM THE DEVICE!");
		ConvertIdleModeEndianess((ST_IDLEMODE_INDICATION *)(pucPayloadData+sizeof(UINT)));
		BeceemWiMAX_SendAppIdleresponse(pWiMAX, pucPayloadData+sizeof(UINT));
		return SUCCESS;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	switch(*pucPayloadData)
	{
		case DEVICE_STATUS_PAYLOAD:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "DEVICE_STATUS_PAYLOAD received.");
			pucPayloadData++;
				{
					ST_MODEM_STATE_INFO stModemStatus = {0};
					stModemStatus.eModemStatus = DEVICE_STATUS_IND;
					stModemStatus.u32Status = *pucPayloadData;
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "DEVICE_STATUS_PAYLOAD payload %d.",
			 stModemStatus.u32Status);
					BeceemWiMAX_SendAppModemStatus(pWiMAX,&stModemStatus);
				}
		break;

		case LINK_STATUS_IND_PAYLOAD:
			{
				ST_MODEM_STATE_INFO stModemStatus = {0};
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "CONNECTION_STATUS_IND payload %d.",
			 stModemStatus.u32Status);
				pucPayloadData++;
		
				stModemStatus.eModemStatus = CONNECTION_STATUS_IND;
				stModemStatus.u32Status = *pucPayloadData;
				BeceemWiMAX_SendAppModemStatus(pWiMAX,&stModemStatus);
				
				pucPayloadData++;
				stModemStatus.eModemStatus = LINK_STATUS_IND;
				stModemStatus.u32Status = *pucPayloadData;
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "LINK_STATUS_IND payload %d.",
			 stModemStatus.u32Status);

				BeceemWiMAX_SendAppModemStatus(pWiMAX,&stModemStatus);
			}
			
			
		break;
		
		case LINK_UP_ACK_PAYLOAD:
		{
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "LINK_UP_ACK_PAYLOAD received.");
			pucPayloadData++;
            if(*pucPayloadData == SLEEP_MODE_CODE)
            {
                iRetval=BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, SLEEP_MODE_CODE, NULL);
            	goto returnCode;
			}
			switch(*pucPayloadData)
			{
				case WAIT_FOR_SYNC:
						{

					/* kill syncup thread if running */
					if (pWiMAX->m_bSyncUpInProgress)
					{
						pWiMAX->m_bSyncUpInProgress = FALSE;
						if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
						{
						DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
						       "Failed to stop SyncUp thread!!!");
                            if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
                            {
                                //so we can recover if we terminate..
                                Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
                            }
						}
					}
/*
					if (pWiMAX->m_bModemInShutdown)
					{
						pWiMAX->m_bModemInShutdown = FALSE;
						return SUCCESS;
					}
*/
                    if(*(pucPayloadData+1) == INNER_NAI_MATCH_FAILURE)
                    {
                        BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, INNER_NAI_MATCH_FAILURE_CODE, NULL);
                		iRetval = SUCCESS;
						goto returnCode;
                    }
                     
					bSyncFail = BeceemWiMAX_CheckForSyncFail(pWiMAX, *(pucPayloadData+1));
					if(bSyncFail)
					{
						iRetval=SUCCESS;
						goto returnCode;
					}
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "WAIT_FOR_SYNC received.");

						if((*(pucPayloadData+1) == INVALID_REASON_VALUE) || 
								(*(pucPayloadData+1) == 0))
						{
							*(pucPayloadData+1)  = WAIT_PHY_SYNC_CMD_CODE;
						}
						
					iRetval=BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, *(pucPayloadData+1), (PVENDOR_SPECIFIC_INFO)(pucPayloadData+2));
					goto returnCode;
					}

				case PHY_SYNC_ACHIVED:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "PHY_SYNC_ACHIVED received.");
					Event_Set(&(pWiMAX->m_objSyncUpEvent));
					if(Event_Wait(&(pWiMAX->m_objSyncUpEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
					{
				    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
					   "Wait on m_objSyncUpEventMsgRecvd TIMEDOUT");
					}

					BeceemWiMAX_SendAppLinkSyncStatus(pWiMAX, SUBTYPE_SYNC_STATUS_RESP, (PST_SYNC_STATUS)(pucPayloadData+1));
					if (pWiMAX->m_bModemInShutdown)
						pWiMAX->m_bModemInShutdown = FALSE;
					iRetval=SUCCESS;
					goto returnCode;
					/* return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_NET_ENTRY_CMD_CODE, (PVENDOR_SPECIFIC_INFO)(pucPayloadData+2)); */

				case LINKUP_IN_PROGRESS:
					bSyncFail = BeceemWiMAX_CheckForSyncFail(pWiMAX, *(pucPayloadData+1));
					if(bSyncFail)
						{
							iRetval=SUCCESS;
							goto returnCode;
						}
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "LINKUP_IN_PROGRESS received.");
					iRetval=SUCCESS;
					goto returnCode;

				case LINKUP_DONE:
					{
						const UINT uiIPRefresh = 1;
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "LINKUP_DONE received.");
						Event_Set(&(pWiMAX->m_objNetEntryEvent)); 
						if(Event_Wait(&(pWiMAX->m_objNetEntryEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
							{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
					       "Wait on m_objNetEntryEventMsgRecvd TIMEDOUT");
							}
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "The Dhcp Status is %x",
				       *(pucPayloadData + 1));
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "Payload bytes before vendor specific info 0x%x,0x%x,0x%x,0x%x",
				       *(pucPayloadData), *(pucPayloadData + 1), *(pucPayloadData + 2),
				       *(pucPayloadData + 3));
						if(*(pucPayloadData+1) == uiIPRefresh)
						{
					
							WiMAXMessage_Init(
												pWMMsg,
												TYPE_NETWORK_LAYER_CONTROL, 
												SUBTYPE_IP_REFRESH_INDICATION, 
												NULL,
												0
											);
							BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
						}
					}
					if (pWiMAX->m_bModemInShutdown)
						pWiMAX->m_bModemInShutdown = FALSE;
					iRetval=BeceemWiMAX_SendAppLinkStatus (pWiMAX, LINK_UP, 0, (PVENDOR_SPECIFIC_INFO)(pucPayloadData+4));
					goto returnCode;

				case DREG_RECIEVED:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "DREG_RECIEVED received.");
					iRetval=BeceemWiMAX_SendAppLinkStatus (pWiMAX, LINK_DOWN, DREG_RECEIVED_CODE, (PVENDOR_SPECIFIC_INFO)(pucPayloadData+1));
					goto returnCode;

				case RESET_RECIEVED:
					bSyncFail = BeceemWiMAX_CheckForSyncFail(pWiMAX, *(pucPayloadData+1));
					if(bSyncFail)
					{	
						iRetval=SUCCESS;
						goto returnCode;
					}
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "RESET_RECIEVED received.");
					iRetval=BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, RESET_RECEIVED_CODE, (PVENDOR_SPECIFIC_INFO)(pucPayloadData+2));
					goto returnCode;

				case SHUTDOWN_REQ_FROM_FIRMWARE:
				case SHUTDOWN_REQ_FRM_FW_STANDBY_TIMER:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "SHUTDOWN_REQ_FROM_FIRMWARE received.");
					// Kill Syncup thread if running
					if(pWiMAX->m_bSyncUpInProgress)
					{
						pWiMAX->m_bSyncUpInProgress = FALSE;
						/* Stop SyncUp thread */
						if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
						{
						DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
						       "Failed to stop SyncUp thread!!!");
                            if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
                            {
                                //so we can recover if we terminate..
                                Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
                            }
						}
					}
					
					// Kill N/W Search thread if running
					if(pWiMAX->m_bNetSearchInProgress)
					{
						pWiMAX->m_bNetSearchInProgress = FALSE;
						/* Stop SyncUp thread */
						if(Thread_Stop(&(pWiMAX->m_NetSearchThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetSearchThread, pWiMAX) != SUCCESS)
						{
						DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
						       "Failed to stop NetSearch thread!!!");
                            if(Thread_Terminate(&(pWiMAX->m_NetSearchThread)) == SUCCESS)
                            {
                                //so we can recover if we terminate..
                                Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd));
                            }
						}					
					}
					pWiMAX->m_bNSPSearchInProgress  = FALSE;
				
					BeceemWiMAX_CheckForSyncFail(pWiMAX, SHUTDOWN_REQ_FROM_FIRMWARE);
                    pWiMAX->m_bModemInShutdown=1;
					
					WiMAXMessage_Init(
								pWMMsg,
								TYPE_SHUTDOWN, 
								SUBTYPE_SHUTDOWN_REQ_FROM_FIRMWARE, 
								(UCHAR *)NULL, 
								0
								);
					iRetval = BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					goto returnCode;
					}

				case PERIODIC_WAKE_UP_FROM_SHUTDOWN:
				case COMPLETE_WAKE_UP_FROM_SHUTDOWN:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "%d received.", (*pucPayloadData));
					iRetval=BeceemWiMAX_SendAppWakeupStatus (pWiMAX,(*pucPayloadData), (PBSLIST)(pucPayloadData+3));
					goto returnCode;

				case RANGING_IN_PROGRESS:
				case SBC_IN_PROGRESS:
				case EAP_AUTH_DEVICE_IN_PROGRESS:
				case EAP_AUTH_USER_IN_PROGRESS:
				case SATEK_IN_PROGRESS:
				case REGISTRATION_IN_PROGRESS:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "%d received.", (*pucPayloadData));
					break;
				case HMC_SHUTDOWN_REQ_FROM_FIRMWARE:
					{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "HMC_SHUTDOWN_REQ_FROM_FIRMWARE received.");
					WiMAXMessage_Init(
											pWMMsg,
											TYPE_HALF_MINI_CARD, 
											SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION, 
											NULL,
											0
									 );
					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					}
					break;

				case HMC_WAKE_UP_NOTIFICATION_FRM_FW:
					{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "HMC_WAKE_UP_NOTIFICATION_FRM_FW received.");
					WiMAXMessage_Init(
											pWMMsg,
											TYPE_HALF_MINI_CARD, 
											SUBTYPE_HALF_MINI_CARD_WAKEUP_RESPONSE, 
											NULL,
											0
									 );
					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
						
					}
					break;
					
				case BS_OUT_OF_COVERAGE:
					{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "BS_OUT_OF_COVERAGE received.");
						
						BeceemWiMAX_SendAppLinkStatus(pWiMAX,LINK_DOWN,BS_NOT_INCOVERAGE,NULL);
					}
					break;
                case ASR_ENTER_FRM_FW:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ASR_ENTER_FRM_FW received.");
                    BeceemWiMAX_SendAppLinkStatus(pWiMAX,LINK_DOWN,ASR_ENTER_FRM_FW_CODE,NULL);
                    break;
                case ASR_EXIT_FRM_FW:
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ASR_EXIT_FRM_FW received.");
                    BeceemWiMAX_SendAppLinkStatus(pWiMAX,LINK_DOWN,ASR_EXIT_FRM_FW_CODE,NULL);
                    break;
				case COMPLETE_WAKE_UP_FROM_HIBERNATION:
					DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
						   "HIBRNATE_WAKE_UP_NOTIFICATION_FRM_FW received.");
					WiMAXMessage_Init(
						pWMMsg,
						TYPE_HIBERNATE, 
						SUBTYPE_HIBERNATION_WAKE_UP_IND, 
						NULL,
						0
						);
					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					iRetval=SUCCESS;
					goto returnCode;
					break;				
				case SHUTDOWN_REQ_FRM_FW_HIBERNATION_BUTTON_PRESS:
					DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
						   "HIBRNATE ENTRY RESPONSE FROM FW received.");
					WiMAXMessage_Init(
						pWMMsg,
						TYPE_HIBERNATE, 
						SUBTYPE_HIBERNATE_RESP, 
						0,
						0);		
					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					iRetval=SUCCESS;
					goto returnCode;
			
				break;

				case BELOW_THRESHOLD_CODE:
					{
						DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "BELOW_THRESHOLD_CODE received.");
						BeceemWiMAX_SendAppLinkStatus(pWiMAX,LINK_DOWN,BELOW_THRESHOLD_CODE,NULL);
					}
				break;

				case ABOVE_THRESHOLD_CODE:
					{
						DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ABOVE_THRESHOLD_CODE received.");
						BeceemWiMAX_SendAppLinkStatus(pWiMAX,LINK_DOWN,ABOVE_THRESHOLD_CODE,NULL);
					}
					break;
				default:
			    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
				   "Invalid Message with SwStatus [%d] received.", (*pucPayloadData));
					break;
			}
		}
		break;
		
		case NW_SEARCH_RSP:
		{
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "NW_SEARCH_RSP received.");
			if (pWiMAX->m_bModemInShutdown)
				pWiMAX->m_bModemInShutdown = FALSE;
			
			Event_Set(&(pWiMAX->m_objNetSearchEvent)); 

			if(Event_Wait(&(pWiMAX->m_objNetSearchEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
				 "Wait on m_objNetSearchEventMsgRecvd TIMEDOUT");
			}
 
			
			pucPayloadData++;
            switch(*pucPayloadData)
			{
		    	case NW_SEARCH_TYPE_BCM_WIMAX:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "BCM WIMAX SEARCH RESPONSE received.");
                   	iRetval=BeceemWiMAX_SendAppNetSearchresponse(pWiMAX, (PBSLIST) (pucPayloadData+3));		
					goto returnCode;
				} 
				break;

				case NW_SEARCH_TYPE_CAPL:
				{   
				   pstCaplRsp = (PST_CAPL_SEARCH_RSP)(pucPayloadData+3);
				   iRetval=BeceemWiMAX_HandleCaplResponse(pWiMAX,pstCaplRsp);
					goto returnCode;
				}
				break;		
				case SUBTYPE_BCM_WIMAX_NW_SEARCH_RESP:
				{
					PBSNSPLIST pBSInfoList = (PBSNSPLIST)(pucPayloadData+3);
					iRetval=BeceemWiMAX_HandleNSPSearchResponse(pWiMAX,pBSInfoList);
					goto returnCode;
				}
				break;
				case SUBTYPE_BCM_WIMAX_NW_SEARCH_PER_ANTENNA:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "BCM WIMAX SEARCH RESPONSE Per Antenna received.");
                   iRetval=BeceemWiMAX_SendAppNetSearchresponsePerAntenna(pWiMAX, (PST_BSLIST_PER_ANTENNA) (pucPayloadData+3));		
					goto returnCode;
				} 
				break;
				case NW_SEARCH_SET_RSSI_CINR_RESP:
				{
					pucPayloadData++;
					pucPayloadData = pucPayloadData + 2;

					iRetval=BeceemWiMAX_SetRssiCinrResponse(pWiMAX, pucPayloadData);
					goto returnCode;
				}
				break;

			}
		
		}
		break;

		case HANDOVER_MESSAGE:
		{
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Sending HandOver Message.");
			pucPayloadData++;

			pstHOInfo = (PS_HOINFO) pucPayloadData;

			pstHOInfo->u32CurrentPreambleID = ntohl(pstHOInfo->u32CurrentPreambleID);
			pstHOInfo->u32HandoverLatency = ntohl(pstHOInfo->u32HandoverLatency);
			pstHOInfo->u32LastPreambleID = ntohl(pstHOInfo->u32LastPreambleID);
			pstHOInfo->u32NoOfRangingFailure = ntohl(pstHOInfo->u32NoOfRangingFailure);
			pstHOInfo->u32VendorSpecificInformationLength  = ntohl(pstHOInfo->u32VendorSpecificInformationLength);
			usMessageSubType = SUBTYPE_HANDOFF_INDICATION;
			WiMAXMessage_Init(
							pWMMsg,
							TYPE_HANDOFF_MESSAGE, 
							usMessageSubType, 
							(UCHAR *)pstHOInfo,
							sizeof(S_HOINFO) - 1 + pstHOInfo->u32VendorSpecificInformationLength
							);

			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			
			/*Handling the IP refresh part below*/
			//pucPayloadData++;
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "The Dhcp Status is %x", *pucPayloadData);
			memset(pWMMsg,0,sizeof(WIMAXMESSAGE));
			
			if(pstHOInfo->u8DHCPRefresh)
			{
				WiMAXMessage_Init(
							pWMMsg,
							TYPE_NETWORK_LAYER_CONTROL, 
							SUBTYPE_IP_REFRESH_INDICATION, 
							NULL,
							0
							);
				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

			}
			iRetval=SUCCESS;
			goto returnCode;
		} 
		break;

		case LINK_UP_NAK_PAYLOAD:
		case LINK_DOWN_ACK_PAYLOAD:
		case LINK_DOWN_NAK_PAYLOAD:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "LINK_UP_NAK/LINK_DOWN_ACK/NAK received.");
			iRetval=BeceemWiMAX_SendAppLinkStatus (pWiMAX, LINK_DOWN, *(pucPayloadData+2), NULL);
			goto returnCode;
		case ANTENNA_MSG:
		{
			pucPayloadData++;
			switch(*pucPayloadData)
			{
				case ANTENNA_CONFIG_RSP:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ANTENNA_CONFIG_RSP received.");
					Event_Set(&(pWiMAX->m_objAntennaConfigReqEvent)); 
					if(Event_Wait(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
					{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
					       "Wait on m_objAntennaConfigReqEventMsgRecvd TIMEDOUT");
					}
					iRetval=BeceemWiMAX_SendAppAntennaConfigresponse(pWiMAX, (PST_GPIO_SETTING_INFO) (pucPayloadData+3));
					goto returnCode;
				}
				break;

				case ANTENNA_SCANNING_RSP:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ANTENNA_SCANNING_RSP received.");
					Event_Set(&(pWiMAX->m_objAntennaScanningReqEvent)); 
					if(Event_Wait(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
					{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
					       "Wait on m_objAntennaScanningReqEventMsgRecvd TIMEDOUT");
					}
					iRetval=BeceemWiMAX_SendAppAntennaScanningresponse(pWiMAX, (PST_RSSI_INFO_IND) (pucPayloadData+3));
					goto returnCode;
				}
				break;

				case ANTENNA_SELECT_RSP:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "ANTENNA_SELECTION_RSP received.");
					Event_Set(&(pWiMAX->m_objAntennaSelectionReqEvent)); 
					if(Event_Wait(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
					{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
					       "Wait on m_objAntennaSelectionReqEventMsgRecvd TIMEDOUT");
					}
					iRetval=BeceemWiMAX_SendAppAntennaSelectionresponse(pWiMAX, (UINT*) (pucPayloadData+3));
					goto returnCode;
				}
				break;
			}
		}
		case NW_SEARCH_LOW_POWER:
		{
			pucPayloadData++;

			
			switch(*pucPayloadData)
			{
				case SUBTYPE_NW_SEARCH_LOW_POWER_RESP:
				{
				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				       "SUBTYPE_NW_SEARCH_LOW_POWER_RESP received.");
					status = htonl(*((UINT *)(pucPayloadData + 3)));
					pucPayloadData = pucPayloadData + 7;
		           	iRetval=BeceemWiMAX_SendAppLowPowerModeResponse(pWiMAX, (PST_SCAN_CONFIG_PARAM)pucPayloadData,status);	
					goto returnCode;
				}
			}
		}
		break;

		case UART_IP_FILTER_QUERY_STATUS:
		{
			pucPayloadData++;
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
			 "UART_IP_FILTER_QUERY_STATUS received with status %d", *pucPayloadData);

			status = *pucPayloadData;
			WiMAXMessage_Init(
							pWMMsg,
							TYPE_DEVICE_CONFIG, 
							SUBTYPE_IP_FILTER_STATUS_NOTIFICATION, 
							(UCHAR *)&status,
							sizeof(INT32)
							);

			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			iRetval=SUCCESS;
			goto returnCode;
		}
		break;

	case TYPE_PS_INFO_RESP:
		{
			iRetval=BeceemWiMAX_ProximitySensor_Response(pWiMAX,pucPayloadData,uiNumBytesRecvd);
			goto returnCode;
		}
		break;
		
		default:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Unexpected payload .%d", *pucPayloadData);
			break;
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;

}

int BeceemWiMAX_SendAppLowPowerModeResponse(PVOID pArg, PST_SCAN_CONFIG_PARAM pScanConfigParam, INT status)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	ST_SCAN_CONFIG_PARAM_STATUS stParamStatus = {0};

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	
	stParamStatus.bStatus = status;
	memcpy(&stParamStatus.stScanConfigParam,pScanConfigParam,sizeof(ST_SCAN_CONFIG_PARAM));
	ConvertToHostEndian((UCHAR *)&stParamStatus.stScanConfigParam, sizeof(ST_SCAN_CONFIG_PARAM), 1);
	
	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_NETWORK_SEARCH, 
						SUBTYPE_SET_LOWPOWER_SCANMODE_RESP, 
						(UCHAR *)&stParamStatus,
						sizeof(ST_SCAN_CONFIG_PARAM_STATUS)
					  );

	

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)	
	return SUCCESS;

}



INT BeceemWiMAX_SendAppWiMAXOptionsAll(PVOID pArg,PUCHAR pPayload,
									   UCHAR uchOptionSpecifer, INT32 i32Status,USHORT usSubType)
{		
		PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
		PWIMAXMESSAGE pWMMsg = NULL;
		int i = 0;
		ST_WIMAX_OPTIONS_ALL stWiMAXOptionsAll = {0};
		stWiMAXOptionsAll.u8WimaxOptionSpecifier = uchOptionSpecifer;
		pPayload++;
		memcpy(stWiMAXOptionsAll.au32WimaxOptionsPayloadPerOpt,pPayload,sizeof(UINT32)*MAX_WIMAX_OPTIONS_POSSIBLE);
		for(i = 0 ; i < MAX_WIMAX_OPTIONS_POSSIBLE ; i++)
		{
			stWiMAXOptionsAll.au32WimaxOptionsPayloadPerOpt[i] = ConvertLongEndian(stWiMAXOptionsAll.au32WimaxOptionsPayloadPerOpt[i],1);
		}
		stWiMAXOptionsAll.i32Status = i32Status;
		if(pWiMAX->stWiMAXOptionAll.u8WimaxOptionSpecifier & WIMAX_SET_OPTION_BIT5)
			{
				stWiMAXOptionsAll.au32WimaxOptionsPayloadPerOpt[5] = pWiMAX->stWiMAXOptionAll.au32WimaxOptionsPayloadPerOpt[5];
			}
		BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
		
		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_SET_WIMAX_OPTIONS, 
						usSubType, 
						(UCHAR *)&stWiMAXOptionsAll,
						sizeof(ST_WIMAX_OPTIONS_ALL)
					  );
		
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)	
		return SUCCESS;
}

INT BeceemWiMAX_SendAppModemStatus(PVOID pArg, PST_MODEM_STATE_INFO  pstModemStatus)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	
	
	if(!pWiMAX || !pstModemStatus)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_NETWORK_ENTRY, 
						SUBTYPE_DEVICE_STATUS_INDICATION, 
						(UCHAR *)pstModemStatus,
						sizeof(ST_MODEM_STATE_INFO)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
int BeceemWiMAX_HandleCaplResponse(PVOID pArg, PST_CAPL_SEARCH_RSP pstCaplRsp)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT32 index = 0;

	if(!pWiMAX || !pstCaplRsp)
		return ERROR_INVALID_ARGUMENT;

	pstCaplRsp->u32SearchCompleteFlag = ntohl(pstCaplRsp->u32SearchCompleteFlag);
	pstCaplRsp->u32NumBS = ntohl(pstCaplRsp->u32NumBS);
	pstCaplRsp->u32ErrorStatus =  ntohl(pstCaplRsp->u32ErrorStatus);

	ConvertToHostEndian((UCHAR *)pstCaplRsp->bsInfo, sizeof(ST_DETECTED_BS_INFO) + (pstCaplRsp->u32NumBS-1)* sizeof(ST_DETECTED_BS_INFO), 1);
	
    for(index=0; index< pstCaplRsp->u32NumBS; index++) 
	{
		UINT *puiBSID = (UINT *)&pstCaplRsp->bsInfo[index].bCompressedMap; 
		*puiBSID = htonl(*puiBSID);
		*(puiBSID+1) = htonl(*(puiBSID+1));
		pstCaplRsp->bsInfo[index].u32Bandwidth = (UINT32)(pstCaplRsp->bsInfo[index].u32Bandwidth/1000.0);
		pstCaplRsp->bsInfo[index].u32CenterFreq = (UINT32)((pstCaplRsp->bsInfo[index].u32CenterFreq/8.0) * 1000);

	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
					pWMMsg,
						TYPE_NETWORK_SEARCH, 
						SUBTYPE_CAPL_BASED_SEARCH_RSP, 
						(UCHAR *)pstCaplRsp,
						sizeof(ST_CAPL_SEARCH_RSP) - ((MAX_NUM_BS - pstCaplRsp->u32NumBS)* sizeof(ST_DETECTED_BS_INFO))
					  );

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}



int BeceemWiMAX_HandleNSPSearchResponse(PVOID pArg, PBSNSPLIST  pBSInfoList)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT32 u32Index=0;

	if(!pWiMAX || !pBSInfoList)
		return ERROR_INVALID_ARGUMENT;

	pBSInfoList->u32Status = ntohl(pBSInfoList->u32Status);
	pBSInfoList->u32EndOfDiscoveryRsp = ntohl(pBSInfoList->u32EndOfDiscoveryRsp);
	pWiMAX->m_bNSPSearchInProgress = FALSE;
		
			pBSInfoList->stBSInformation.u32DCDChangeCount= ntohl(pBSInfoList->stBSInformation.u32DCDChangeCount);
			pBSInfoList->stBSInformation.u32NSPChangeCount = ntohl(pBSInfoList->stBSInformation.u32NSPChangeCount);
			pBSInfoList->stBSInformation.u32NumNSPEntries = ntohl(pBSInfoList->stBSInformation.u32NumNSPEntries);

			ConvertToHostEndian((UCHAR *)&pBSInfoList->stBSInformation.stPHYINFO,sizeof(PHY_INFO_NSP_RESP), 1);

			for(u32Index = 0; u32Index < MAX_NSP_LIST_LENGTH; u32Index++)
			{
				pBSInfoList->stBSInformation.astNSPList[u32Index ].u32NSPID = 
					ntohl(pBSInfoList->stBSInformation.astNSPList[u32Index ].u32NSPID);
				pBSInfoList->stBSInformation.astNSPList[u32Index ].u32NspNameLength = 
					ntohl(pBSInfoList->stBSInformation.astNSPList[u32Index ].u32NspNameLength);
				
			}
			
			pBSInfoList->stBSInformation.stPHYINFO.u32CF = (UINT32)((pBSInfoList->stBSInformation.stPHYINFO.u32CF /8.0) * 1000);
			pBSInfoList->stBSInformation.stPHYINFO.u32Bandwidth = (UINT32)(pBSInfoList->stBSInformation.stPHYINFO.u32Bandwidth/1000.0);
		
	if(((pBSInfoList->u32Status == NSP_DISCOVERY_ABORT ||
		pBSInfoList->u32Status == NSP_DISCOVERY_SCAN_DURATION_TIMEOUT ||
		pBSInfoList->u32Status == NSP_DISCOVERY_NOT_NEEDED)
		&& (pBSInfoList->stBSInformation.u32NumNSPEntries == 0))||
		(pBSInfoList->u32Status == NSP_SEARCH_FAILURE) || (pBSInfoList->u32Status == NSP_16G_NOT_SUPPORTED))
	
		{
			memset(&pBSInfoList->stBSInformation,0,sizeof(BSNSPINFO));
		}

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
				pWMMsg,
				TYPE_NETWORK_SEARCH, 
				SUBTYPE_NETWORK_DISCOVERY_RESP, 
				(UCHAR *)pBSInfoList,
				sizeof(BSNSPLIST)
			  );

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_SendAppNetSearchresponse(PVOID pArg, PBSLIST pstBsList)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	int iNumOfBSFound = 0, i = 0;

	if(!pWiMAX || !pstBsList)
		return ERROR_INVALID_ARGUMENT;

	iNumOfBSFound = htonl(pstBsList->u32NumBaseStationsFound);
	ConvertToHostEndian((UCHAR *)pstBsList, sizeof(BSLIST) + (iNumOfBSFound -1 )* sizeof(BSINFO), 1);

	for(i = 0; i < iNumOfBSFound; i++)
	{
		/*Revert back the endianness for BSID as it is raw data!*/
		UINT *puiBSID = (UINT *)pstBsList->stBSInformation[i].aucBSID;
		*puiBSID = htonl(*puiBSID);
		*(puiBSID+1) = htonl(*(puiBSID+1));

		/*Convert frequency into KHz*/
		pstBsList->stBSInformation[i].u32CenterFrequency = (UINT32)((pstBsList->stBSInformation[i].u32CenterFrequency/8.0) * 1000);
		pstBsList->stBSInformation[i].u32Bandwidth = (pstBsList->stBSInformation[i].u32Bandwidth)/1000;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_NETWORK_SEARCH, 
						SUBTYPE_NETWORK_SEARCH_RESP, 
						(UCHAR *)pstBsList,
						sizeof(BSLIST) + (pstBsList->u32NumBaseStationsFound -1 )* sizeof(BSINFO)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_SendAppNetSearchresponsePerAntenna(PVOID pArg, PST_BSLIST_PER_ANTENNA pstBsList)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	int iNumOfBSFound = 0, i = 0;

	if(!pWiMAX || !pstBsList)
		return ERROR_INVALID_ARGUMENT;

	iNumOfBSFound = htonl(pstBsList->u32NumBaseStationsFound);
	ConvertToHostEndian((UCHAR *)pstBsList, sizeof(ST_BSLIST_PER_ANTENNA) + (iNumOfBSFound -1 )* sizeof(ST_BSINFO_PER_ANTENNA), 1);

	for(i = 0; i < iNumOfBSFound; i++)
	{
		/*Revert back the endianness for BSID as it is raw data!*/
		UINT *puiBSID = (UINT *)pstBsList->stBSInformation[i].aucBSID;
		*puiBSID = htonl(*puiBSID);
		*(puiBSID+1) = htonl(*(puiBSID+1));

		/*Convert frequency into KHz*/
		pstBsList->stBSInformation[i].u32CenterFrequency = (UINT32)((pstBsList->stBSInformation[i].u32CenterFrequency/8.0) * 1000);
		pstBsList->stBSInformation[i].u32Bandwidth = (pstBsList->stBSInformation[i].u32Bandwidth)/1000;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_NETWORK_SEARCH, 
						SUBTYPE_NETWORK_SEARCH_RESP, 
						(UCHAR *)pstBsList,
						sizeof(ST_BSLIST_PER_ANTENNA) + (pstBsList->u32NumBaseStationsFound -1 )* sizeof(ST_BSINFO_PER_ANTENNA)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

BOOL BeceemWiMAX_CheckForSyncFail(PVOID pArg, UCHAR ucLinkStatusError)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX)
		return FALSE;

	if(ucLinkStatusError == PHY_SYNC_FAIL)
	{
		BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, SHUTDOWN_RECEIVED_CODE, NULL);
		return TRUE;
	}
   
	else if (ucLinkStatusError == SHUTDOWN_REQ_FROM_FIRMWARE)
	{
		Event_Set(&(pWiMAX->m_objShutDownRespEvent)); 
		if(Event_Wait(&(pWiMAX->m_objShutDownRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
			   "Wait on m_objShutDownRespEventMsgRecvd TIMEDOUT");
		}
		return TRUE;
	}
	return FALSE;
}

int BeceemWiMAX_SendAppLinkSyncStatus(PVOID pArg, USHORT usSubType, PST_SYNC_STATUS pstSyncStatus)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX || !pstSyncStatus)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Sending  AppLinkSyncStatus SubType : %d", usSubType);

	pstSyncStatus->u32CenterFreq = (UINT32)(ntohl(pstSyncStatus->u32CenterFreq)/8.0*1000.0);
	pstSyncStatus->u32Bandwidth = (UINT32)(ntohl(pstSyncStatus->u32Bandwidth)/1000.0);
	pstSyncStatus->s32CINRDev = ntohl(pstSyncStatus->s32CINRDev);
	pstSyncStatus->s32CINRMean = ntohl(pstSyncStatus->s32CINRMean);
	pstSyncStatus->u32PreambleId = ntohl(pstSyncStatus->u32PreambleId);
	pstSyncStatus->s32RSSIDev = ntohl(pstSyncStatus->s32RSSIDev);
	pstSyncStatus->s32RSSIMean = ntohl(pstSyncStatus->s32RSSIMean);
	pstSyncStatus->u32SyncStatus = ntohl(pstSyncStatus->u32SyncStatus);
	pstSyncStatus->u32SyncStatus = (UINT32)BeceemWiMAX_GetLinkErrorCode((UCHAR)pstSyncStatus->u32SyncStatus);
	pstSyncStatus->u32DCDChangeCount = ntohl(pstSyncStatus->u32DCDChangeCount); 
	pstSyncStatus->u32NSPChangeCount = ntohl(pstSyncStatus->u32NSPChangeCount); 

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(
					pWMMsg,
			    		TYPE_NETWORK_ENTRY, 
					usSubType, 
						(UCHAR *)pstSyncStatus,
						sizeof(ST_SYNC_STATUS)
					);
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_GetLinkErrorCode(UCHAR ucLinkFailReason)
{
	switch(ucLinkFailReason)
	{
	case PHY_SYNC_FAIL:
		return PHY_SYNC_ERROR;
	case MAC_SYNC_FAIL:
		return MAC_SYNC_ERROR;
	case HOST_INITIATED:
		return HOST_INITIATED_SYNC_DOWN;
	case RANGE_FAIL:
		return RANGING_FAILURE;
	case SBC_FAIL:
		return SBC_FAILURE;
	case PKM_FAIL:
		return PKM_FAILURE;
	case REG_FAIL:
		return REGISTRATION_FAILURE;
	case DREG_RECEIVED_CODE:
		return DREG_RECEIVED;
	case RESET_RECEIVED_CODE:
		return RESET_RECEIVED;
	case STATUS_NORMAL:
		return LINK_STATUS_NORMAL;
	case IDLE_MODE_CODE:
		return IN_IDLE_MODE;
	case SLEEP_MODE_CODE:
		return IN_SLEEP_MODE;
	case NET_ENTRY_PROGRESS_CODE:
		return NETWORK_ENTRY_IN_PROGRESS;
	case WAIT_NET_ENTRY_CMD_CODE:
		return WAIT_FOR_NETWORK_ENTRY_CMD;
	case WAIT_PHY_SYNC_CMD_CODE:
		return WAIT_FOR_PHY_SYNC_CMD;
	case SHUTDOWN_RECEIVED_CODE :
		GpWiMAX->m_bModemInShutdown = TRUE;
		return SHUTDOWN_RECEIVED_CMD;
	case SATEK_FAIL:
		return SATEK_FAILURE;
	case INVALID_REASON_VALUE:
		return INVALID_REASON_CODE_RECEIVED;
	case HOST_INITIATED_LINK_DOWN:
		return LINK_DOWN_HOST_INITIATED;
	case MAC_VERSION_SWITCH_CODE:
		return MAC_VERSION_SWITCH;
	case SHUTDOWN_RESYNC:
		return SHUTDOWN_RESYNC_RECEIVED;
	case IDLE_MODE_FAILURE:
		return IDLE_MODE_FAILURE_CODE_RECEIVED;
	case LOCATION_UPDATE_SUCCESS:
		return LOCATION_UPDATE_WITH_POWER_DOWN_SUCCESS_RECEIVED;
	case BS_NOT_INCOVERAGE:
		return BS_OUT_OF_REACH;
	case RETAIN_IP_ADDR_TIMEOUT:
		return IP_RETENTION_TIMER_EXPIRED;
    case ASR_ENTER_FRM_FW_CODE:
        return ASR_STATE_ENTER;
        
    case ASR_EXIT_FRM_FW_CODE:
        return ASR_STATE_EXIT;
    case INNER_NAI_MATCH_FAILURE_CODE:
        return INNER_NAI_MATCH_FAILED;
    case BELOW_THRESHOLD_CODE:
         return BELOW_THRESHOLD;
    case ABOVE_THRESHOLD_CODE:
         return ABOVE_THRESHOLD;
    
	case 0:
		return LINKUP_ACHIEVED;
	default:
		 return ucLinkFailReason;
	}

}

int BeceemWiMAX_SendAppLinkStatus(PVOID pArg, UINT uiLinkStatus, UCHAR ucLinkErrorCode, PVENDOR_SPECIFIC_INFO pVendorInfo)
{
	int iRet = 0,iRetval=0;
	PUCHAR pucBuffer;
	PWIMAXMESSAGE pWMMsg = NULL;
	PLINK_STATUS pstLinkStatus;
	PUCHAR pucVendorInfo;
	UINT32 u32DregActionCode = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pucBuffer,PUCHAR,CHAR,MAX_BUFFER_LENGTH)
	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
	if(!pWMMsg)
	{
		BECEEM_MEMFREE(pucBuffer)	
		return ERROR_MALLOC_FAILED;
	}

	 
	pstLinkStatus = (PLINK_STATUS)pucBuffer;

	if (DREG_RECEIVED_CODE == ucLinkErrorCode)
	{
		pucVendorInfo = (PUCHAR)pVendorInfo;
		if (pucVendorInfo)
		{
			u32DregActionCode = *pucVendorInfo;
			pVendorInfo = (PVENDOR_SPECIFIC_INFO)(pucVendorInfo + 4);
			pstLinkStatus->u32ActionCode = u32DregActionCode;
		}
	}

	pstLinkStatus->BsInfoIsValidFlag = FALSE;
	/* Query for baseInfo only if LINK_UP */
	if(uiLinkStatus == LINK_UP)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
		   "Link up status received... Getting Base Information..");

		pWiMAX->m_bModemInShutdown = FALSE;
		pWiMAX->m_bIdleMode= FALSE;
		iRet = BeceemWiMAX_GetBaseInfoEx(pWiMAX, &pstLinkStatus->BsInfo, uiLinkStatus);
		if(iRet)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "GetBaseInfoEx failed...!!!");
			iRetval=iRet;
			goto returnCode;
		}
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Successfully received Base Information...");

		/*We have a valid values for BS Info structure. */
		pstLinkStatus->BsInfoIsValidFlag = TRUE;	
		ucLinkErrorCode = 0;
	}
	if((ucLinkErrorCode == 0) && uiLinkStatus != LINK_UP)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Message not to be sent to Application");
	  	iRetval=SUCCESS;
		goto returnCode;
	}

	pstLinkStatus->eStatusCode = BeceemWiMAX_GetLinkErrorCode(ucLinkErrorCode);
	if((pstLinkStatus->eStatusCode != LINKUP_ACHIEVED) && pWiMAX->m_bNetEntryInProgress)
	{
		pWiMAX->m_bNetEntryInProgress = FALSE;
		if(Thread_Stop(&(pWiMAX->m_NetEntryThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetEntryThread, pWiMAX) != SUCCESS)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to stop Network entry thread!!!");
            if(Thread_Terminate(&(pWiMAX->m_NetEntryThread)) == SUCCESS)
            {
                //so we can recover if we terminate..
                Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));
            }
		}
	}
	
	if (pstLinkStatus->eStatusCode  == SHUTDOWN_RECEIVED_CMD)
	{	
	
		Event_Set(&(pWiMAX->m_objShutDownRespEvent)); 
		if(Event_Wait(&(pWiMAX->m_objShutDownRespEventMsgRecvd),TIMEOUT_CALL_BACK_SERIALIZATION_EVENT) == WAIT_TIMEOUT)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
			   "Wait on m_objShutDownRespEventMsgRecvd TIMEDOUT");
		}
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "SUBTYPE_SHUTDOWN_RESP received.");
		memset(pWMMsg, 0, sizeof(WIMAXMESSAGE));
        pWiMAX->m_bModemInShutdown=1;
				
		WiMAXMessage_Init(
						pWMMsg,
						TYPE_SHUTDOWN, 
						SUBTYPE_SHUTDOWN_RESP, 
						0,
						sizeof(UINT32)
						);
		
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		iRetval=SUCCESS;
		goto returnCode;
	}

	/* Append Vendor specific information */
	if((uiLinkStatus == LINK_UP) && pVendorInfo)
	{
		pstLinkStatus->u32VendorSpecificInformationLength = ntohl(pVendorInfo->uiLength);
		if(pstLinkStatus->u32VendorSpecificInformationLength > 0 &&  pstLinkStatus->u32VendorSpecificInformationLength < sizeof(pVendorInfo->aucData))
			memcpy(pstLinkStatus->au8VendorSpecificInformation, pVendorInfo->aucData, pstLinkStatus->u32VendorSpecificInformationLength);
		else
			pstLinkStatus->u32VendorSpecificInformationLength = 0;

	}

	WiMAXMessage_Init(
						pWMMsg,
						TYPE_NETWORK_ENTRY, 
						SUBTYPE_NETWORK_ENTRY_STATUS_RESP, 
						(UCHAR *)pstLinkStatus, 
						SIZE_LINK_STATUS - 1 + pstLinkStatus->u32VendorSpecificInformationLength
					);

	/* Call call-back */
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);


	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pucBuffer)
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
}

int BeceemWiMAX_SendAppWakeupStatus(PVOID pArg,UCHAR ucLinkErrorCode, PBSLIST pstBSList)
{
	PWIMAXMESSAGE pOWMMsg = NULL;
	PBSINFO pBaseInfo = 0;
	BOOL bSendFlag = FALSE;
	int i;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if (ucLinkErrorCode != COMPLETE_WAKE_UP_FROM_SHUTDOWN)
	{
			pstBSList->u32NumBaseStationsFound = ntohl(pstBSList->u32NumBaseStationsFound);
			if (pstBSList->u32NumBaseStationsFound > 80)
				return SUCCESS;

			pBaseInfo = pstBSList->stBSInformation;
			
			for(i = 0; i < (int)pstBSList->u32NumBaseStationsFound; i++, pBaseInfo++)
			{
				pBaseInfo->s32RelativeSignalStrength = ntohl(pBaseInfo->s32RelativeSignalStrength);
				pBaseInfo->u32Bandwidth = ntohl(pBaseInfo->u32Bandwidth);
				pBaseInfo->u32CenterFrequency = ntohl(pBaseInfo->u32CenterFrequency);
				pBaseInfo->s32CINR = ntohl(pBaseInfo->s32CINR);
				pBaseInfo->u32PreambleIndex = ntohl(pBaseInfo->u32PreambleIndex);
				pBaseInfo->u32DcdChangeCount = ntohl(pBaseInfo->u32DcdChangeCount);
				pBaseInfo->u32NspChangeCount = ntohl(pBaseInfo->u32NspChangeCount);
			}


		if (pstBSList)
		{
			if (pstBSList->u32NumBaseStationsFound > 80)
				return SUCCESS;
			else if (pstBSList->u32NumBaseStationsFound <= 80)
				bSendFlag = TRUE;
			else
				bSendFlag = FALSE;
		}
		else
			bSendFlag = FALSE;
	}
	BECEEMCALLOC(pOWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	if (bSendFlag == TRUE)
	{	
		WiMAXMessage_Init(
						pOWMMsg,
						TYPE_SHUTDOWN, 
						SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND, 
						(UCHAR *)pstBSList, 
						sizeof(BSINFO)
					 );
	}
	else
	{
		pWiMAX->m_bModemInShutdown = FALSE;
		WiMAXMessage_Init(
				pOWMMsg,
				TYPE_SHUTDOWN, 
				SUBTYPE_SHUTDOWN_MODE_COMPLETE_WAKE_UP_IND, 
				(UCHAR *)NULL, 
				0
			 );
	}

	/* Call call-back */
	BeceemWiMAX_CallAppCallback(pWiMAX, pOWMMsg);
	BECEEM_MEMFREE(pOWMMsg)
	return SUCCESS;
}


/**
@ingroup idle_mode
Notify application with idle mode response
*/
int BeceemWiMAX_SendAppIdleresponse(PVOID pArg, 		/**<pointer to a valid BECEEMWIMAX instance.	 */
											PUCHAR pucPayloadData /**<idle mode payload from firmware */
											)
{
	int iLen;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT uiNumMillisecsOfPlannedIdleMode = 0;
	PST_IDLEMODE_INDICATION pstIdleMode = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "IDLE_NOTIFICATION_PAYLOAD received.");

/*
	TO BE DONE
	----------
Payload = 0 if coming out of idle mode.
Non zero value in milliseconds if going into idle mode.

*/
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	pstIdleMode = (PST_IDLEMODE_INDICATION)pucPayloadData;
	uiNumMillisecsOfPlannedIdleMode = pstIdleMode->u32IdleModePeriod;
	iLen = 22; // till preamble index
	WiMAXMessage_Init(
					pWMMsg,
					TYPE_IDLE_MODE, 
					SUBTYPE_IDLE_MODE_INDICATION, 
					(UCHAR*)pucPayloadData,
					iLen
					);

	/*Non zero value in milliseconds if going into idle mode.*/
	if(uiNumMillisecsOfPlannedIdleMode > 0)
	{
		pWiMAX->m_bIdleMode = TRUE;
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, IDLE_MODE_CODE, NULL);	
	}
	else /*Payload = 0 if coming out of idle mode.*/
	{
		if(pWiMAX->m_bIdleMode)
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		pWiMAX->m_bIdleMode = FALSE;
	}
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_GetBaseInfoEx(PVOID pArg, PBSINFOEX pBaseInfo, int iStatus)
{
	stStatistics_SS *pStats = 0;
	int iRetval=0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PUCHAR pucStatBuff = NULL;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pBaseInfo)
		return ERROR_INVALID_BUFFER;
	BECEEMCALLOC(pucStatBuff,PUCHAR,CHAR,2024)
	if(iStatus)
	{
		/* If sync link is up then only get stats */

		int iRet = BeceemWiMAX_GetStatisticsReal(pWiMAX, pucStatBuff, 2024);
		if(iRet <= 0)
		{
			iRetval=iRet;
			goto returnCode;
		}

		pStats = (stStatistics_SS *)pucStatBuff;
		
		memcpy(pBaseInfo->au8BSID, pStats->tLinkStatsEx.au8BSID, 6);  

		pBaseInfo->u8PreambleIndex = pStats->tLinkStatsEx.u8PreambleIndex;  
		pBaseInfo->u8ULCellID = pStats->tLinkStatsEx.u8ULIDCell;
		pBaseInfo->s16CINRDeviation = (SINT16)pStats->tLinkStatsEx.s32DeviationCinr;
		pBaseInfo->s16CINRMean =(SINT16) pStats->tLinkStatsEx.s32MeanCinr;
		pBaseInfo->u32Duration = 0;
		pBaseInfo->s32MeanRssiReport = pStats->tLinkStats.s32CurrentRssi;
		pBaseInfo->u32StdDeviationRssiReport = 0;
		pBaseInfo->u32UlCenterFrequency = pStats->u32CenterFrequency; 
		pBaseInfo->u32DlCenterFrequency = pStats->u32CenterFrequency;

	}

/*	pBaseInfo->uiLinkStatus = iStatus;
	pBaseInfo->uiSignalStrength = 0;	
*/

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pucStatBuff)
	return iRetval;
}

int BeceemWiMAX_ProcessMgmtInfoResponse(PVOID pArg, UINT uiNumBytesRecvd)
{
	PMAC_MGMT_MESSAGE pMACMgmtMsg = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UNUSED_ARGUMENT(uiNumBytesRecvd);
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pWiMAX->m_bMACMgmtMsgsEnabled)
		return SUCCESS;
	
    /*DPRINT(DEBUG_MESSAGE, "In ProcessMgmtInfoResponse."); */
	
	pMACMgmtMsg = (PMAC_MGMT_MESSAGE)(pWiMAX->m_szMsg+sizeof(USHORT) + pWiMAX->u32OffsetForUnalignedAccessCorrection);
    
    pMACMgmtMsg->uiIfaceMsgType = SWAP_ENDIAN32(pMACMgmtMsg->uiIfaceMsgType);
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
    
	switch(pMACMgmtMsg->uiIfaceMsgType)
	{
		case MGMT_MSG_INFO_DM :
		{
			/*S_MAC_MGMTMESSAGE stMacMgmtInfo;*/
			S_MAC_MGMTMESSAGE *pstMacMgmtInfo = NULL;
			pstMacMgmtInfo = (PS_MAC_MGMTMESSAGE)calloc(1,MAX_BUFFER_LENGTH);
			if(!pstMacMgmtInfo)
			{
				BECEEM_MEMFREE(pWMMsg)
				return ERROR_MALLOC_FAILED;
			}	
			
			pstMacMgmtInfo->u8Direction = (UCHAR)SWAP_ENDIAN32(pMACMgmtMsg->uiMgmtMsgDirection);
			pstMacMgmtInfo->u32FrameNumber  = SWAP_ENDIAN32(pMACMgmtMsg->uiFrameNumber);
			pstMacMgmtInfo->u32MsgID = SWAP_ENDIAN32(pMACMgmtMsg->uiMsgRxTxed);
			pstMacMgmtInfo->u32CID = SWAP_ENDIAN32(pMACMgmtMsg->uiCID);
			pstMacMgmtInfo->u8CmacOk = (UCHAR)SWAP_ENDIAN32(pMACMgmtMsg->uiCmacOk);
			pstMacMgmtInfo->u32LengthOfMessage = SWAP_ENDIAN32(pMACMgmtMsg->uiPayloadLength);

			if(pstMacMgmtInfo->u32LengthOfMessage > MAX_MGMT_MESSAGE_LENGTH)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Mac Management Message length is more.");
				BECEEM_MEMFREE(pWMMsg)
				BECEEM_MEMFREE(pstMacMgmtInfo)
				return SUCCESS;
			}

			memcpy(pstMacMgmtInfo->aMessageBinaryData,pMACMgmtMsg->aucPayload, pstMacMgmtInfo->u32LengthOfMessage);


			WiMAXMessage_Init(
							pWMMsg,
							TYPE_MACMGMT, 
							SUBTYPE_MACMGMT_INDICATION, 
							(UCHAR *)pstMacMgmtInfo, 
							SIZE_stMACMgmtMessage-1 + pstMacMgmtInfo->u32LengthOfMessage
							);

			/* copy the buffer part here */
			/*if(usLen > 0)
				WiMAXMessage_AppendData(&oWMMsg, pMACMgmtMsg->aucPayload,  usLen);*/

			
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			BECEEM_MEMFREE(pstMacMgmtInfo)
			
		}
		break;

		default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
		     "Invalid MAC MGMT message, IfaceMsgType is not MGMT_MSG_INFO_DM.");
			break;
	}
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

/********************************************************/
/*			Service Flow Messages						*/
/********************************************************/
int BeceemWiMAX_ProcessServiceFlowResponse(PVOID pArg, UINT uiNumBytesRecvd)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	UCHAR *pucType = 0;
	int iRet =0,iRetval=0;
	DWORD dwNumBytesReturned = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UNUSED_ARGUMENT(uiNumBytesRecvd);
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "CM_CONTROL_RESP received.");

	pucType = pWiMAX->m_szMsg + pWiMAX->u32OffsetForUnalignedAccessCorrection;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	switch(*(pucType+2))
	{
		case CM_DSA_ACK_PAYLOAD:
		case CM_DSA_RSP_PAYLOAD:
			{
				UINT nIndex=0;
				UINT32 nAssociatedSFID, ConfirmationCodeOffset = 0;
				UCHAR *ucTempBuffer;
				stLocalSFAddIndication *pDSAInd;
				PST_SF_INDICATION_CC pstSfIndication;

				pstSfIndication = (PST_SF_INDICATION_CC)(pucType+2);

				if(pstSfIndication->u8CC)
				{
			  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				 "Confirmation code received as %d(non-zero)", pstSfIndication->u8CC);
					pDSAInd = (stLocalSFAddIndication *)calloc(sizeof(stLocalSFAddIndication),1);
					if(!pDSAInd)
					{
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Memory Allocation failed");
						iRetval=ERROR_MALLOC_FAILED;
						goto returnCode;
					}	
					
					
					pDSAInd->u16TID = ConvertShortEndian(pstSfIndication->u16TID,1);
					pDSAInd->u16CID= ConvertShortEndian(pstSfIndication->u16CID,1);
					pDSAInd->u16VCID = ConvertShortEndian(pstSfIndication->u16VCID,1);
					pDSAInd->eConnectionDir = pstSfIndication->eConnectionDir;
					pDSAInd->u8Type = pstSfIndication->u8Type;
					pDSAInd->u8CC = pstSfIndication->u8CC;
					
					WiMAXMessage_Init(
							pWMMsg,
							TYPE_SERVICE_FLOW, 
							SUBTYPE_SF_ADD_INDICATION, 
							(UCHAR *)pDSAInd, 
							sizeof(stLocalSFAddIndication));

					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					BECEEM_MEMFREE(pDSAInd)
					iRetval=SUCCESS;
					goto returnCode;
				}
				
				memcpy(&nAssociatedSFID,pucType+3,sizeof(UINT32));
				
				ucTempBuffer = (UCHAR *)calloc(sizeof(stLocalSFAddIndication),1);
				ConfirmationCodeOffset = (UADDTYPE)&((stLocalSFAddIndication *)NULL)->psfAuthorizedSet;
				if(!ucTempBuffer)
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Memory Allocation failed");
					iRetval=ERROR_MALLOC_FAILED;
					goto returnCode;
				}	
				
				*(UINT32 *)ucTempBuffer= htonl(nAssociatedSFID);
				
				iRet = 1;
				while((iRet!=0) && (nIndex <5))
				{
				iRet = DeviceIO_Send(
									&(pWiMAX->m_Device),
									IOCTL_BCM_GET_DSX_INDICATION,
									ucTempBuffer,
									sizeof(stLocalSFAddIndication),
									ucTempBuffer,
									sizeof(stLocalSFAddIndication),
									&dwNumBytesReturned
								);
				nIndex++;
				}
			
				if(iRet)
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Failed to get Add Indication structure!");
					BECEEM_MEMFREE(ucTempBuffer)
					iRetval=iRet;
					goto returnCode;
				}
				pDSAInd = (stLocalSFAddIndication *)ucTempBuffer;
				pDSAInd->u8CC = ucTempBuffer[ConfirmationCodeOffset];

				ConvertAddIndicationEndianess(pDSAInd, 1);

				WiMAXMessage_Init(
								pWMMsg,
								TYPE_SERVICE_FLOW, 
								SUBTYPE_SF_ADD_INDICATION, 
								ucTempBuffer, 
								sizeof(stLocalSFAddIndication));

				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
				BECEEM_MEMFREE(ucTempBuffer)	
			}
			break;

			case CM_DSC_ACK_PAYLOAD:
			case CM_DSC_RSP_PAYLOAD:
			{
				UINT nIndex=0;
				UINT32 nAssociatedSFID, ConfirmationCodeOffset = 0;
				UCHAR *ucTempBuffer;				
				stLocalSFChangeIndication *pDSCInd;
				PST_SF_INDICATION_CC pstSfIndication;

				pstSfIndication = (PST_SF_INDICATION_CC)(pucType+2);

				if(pstSfIndication->u8CC)
				{
			  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX,
				 "Confirmation code received as %d(non-zero)", pstSfIndication->u8CC);
					pDSCInd = (stLocalSFChangeIndication *)calloc(sizeof(stLocalSFChangeIndication),1);
					if(!pDSCInd)
					{
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Memory Allocation failed");
						iRetval=ERROR_MALLOC_FAILED;
						goto returnCode;
					}	
					pDSCInd->u16TID = ConvertShortEndian(pstSfIndication->u16TID,1);
					pDSCInd->u16CID= ConvertShortEndian(pstSfIndication->u16CID,1);
					pDSCInd->u16VCID = ConvertShortEndian(pstSfIndication->u16VCID,1);
					pDSCInd->eConnectionDir = pstSfIndication->eConnectionDir;
					pDSCInd->u8Type = pstSfIndication->u8Type;
					pDSCInd->u8CC = pstSfIndication->u8CC;

					WiMAXMessage_Init(
							pWMMsg,
							TYPE_SERVICE_FLOW, 
							SUBTYPE_SF_CHANGE_INDICATION, 
							(UCHAR *)pDSCInd, 
							sizeof(stLocalSFChangeIndication));

					BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
					BECEEM_MEMFREE(pDSCInd)
				
						
					iRetval=SUCCESS;
					goto returnCode;
				}
				memcpy(&nAssociatedSFID, pucType+3, sizeof(UINT32));
				ucTempBuffer = (UCHAR *)calloc(sizeof(stLocalSFChangeIndication),1);
				ConfirmationCodeOffset = (UADDTYPE)&((stLocalSFChangeIndication *)NULL)->psfAuthorizedSet;
				if(!ucTempBuffer)
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "Memory Allocation failed");
					iRetval=ERROR_MALLOC_FAILED;
					goto returnCode;
				}	
				
				*(UINT32 *)ucTempBuffer= htonl(nAssociatedSFID);

				iRet = 1;
				while((iRet!=0) && (nIndex <5))
				{
				iRet = DeviceIO_Send(
									&(pWiMAX->m_Device),
									IOCTL_BCM_GET_DSX_INDICATION,
									ucTempBuffer,
									sizeof(stLocalSFChangeIndication),
									ucTempBuffer,
									sizeof(stLocalSFChangeIndication),
									&dwNumBytesReturned
								);
					nIndex++;
				}
				if(iRet)
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,
				 "Failed to get Change Indication structure!");
					BECEEM_MEMFREE(ucTempBuffer)
					iRetval=iRet;
					goto returnCode;
				}

				pDSCInd = (stLocalSFChangeIndication *)ucTempBuffer;
				pDSCInd->u8CC = ucTempBuffer[ConfirmationCodeOffset];
				ConvertAddIndicationEndianess((stLocalSFAddIndication *)pDSCInd, 1);

				WiMAXMessage_Init(
								pWMMsg,
								TYPE_SERVICE_FLOW, 
								SUBTYPE_SF_CHANGE_INDICATION, 
								ucTempBuffer, 
								sizeof(stLocalSFChangeIndication));
				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
				BECEEM_MEMFREE(ucTempBuffer)
				
			}
			break;

		case CM_DSD_ACK_PAYLOAD:
			{
				stLocalSFDeleteIndication *pDSDInd = (stLocalSFDeleteIndication *)(pucType+2);
				pDSDInd->u16CID = ntohs(pDSDInd->u16CID);
				pDSDInd->u16TID = ntohs(pDSDInd->u16TID);
				pDSDInd->u16VCID = ntohs(pDSDInd->u16VCID);
				pDSDInd->u32SFID = ntohl(pDSDInd->u32SFID);

				WiMAXMessage_Init(
								pWMMsg,
								TYPE_SERVICE_FLOW, 
								SUBTYPE_SF_DELETE_INDICATION, 
								pucType+2, 
								sizeof(stLocalSFDeleteIndication));
				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			}
			break;
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
}

int BeceemWiMAX_ToggleResetStatistics(PVOID pArg, UINT uiEnableReset)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0;
	UINT uiStdCfgMIBAddress = 0, uiStatsResetAddress = 0;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_rdm(pWiMAX, CONFIG_STD_MIBS_ADDRESS, &uiStdCfgMIBAddress, sizeof(UINT32));
	if(iRet)
		return iRet;

	uiStatsResetAddress = uiStdCfgMIBAddress + sizeof(stConfigParams);

	switch(uiEnableReset)
	{
		case 0:
			BeceemWiMAX_wrm(pArg, uiStatsResetAddress, 0); 
			break;
		case 1:
			BeceemWiMAX_wrm(pArg, uiStatsResetAddress, 1);
			break;
	}
	return SUCCESS;	
}


int BeceemWiMAX_SendMACAddressWriteResponse(PVOID pArg, PUCHAR pPayload)
{
	int iRet = 0;
	PWIMAXMESSAGE pWMMsg = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UNUSED_ARGUMENT(pPayload);
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	
	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_MACADDRESS, 
						SUBTYPE_MACADDRESS_SET_RESP, 
						(UCHAR *)&iRet,
						4
						);

	/* Call call-back */
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
int BeceemWiMAX_ProcessSleepModeResponse(PVOID pArg, UINT uiNumBytesRecvd)
{
	//PMAC_MGMT_MESSAGE pMACMgmtMsg = 0;
	
	ST_SLEEPMODE_MSG *pstSleepModeMsg=0;
    ST_SLEEPMODE_MSG stLocalSleepModeMsg;
    PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UNUSED_ARGUMENT(uiNumBytesRecvd);
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	
	//DPRINT(DEBUG_MESSAGE, "In ProcessSleepModeResponse.");

	
	pstSleepModeMsg = (ST_SLEEPMODE_MSG*)(pWiMAX->m_szMsg+sizeof(USHORT)+sizeof(UINT)+pWiMAX->u32OffsetForUnalignedAccessCorrection);
	
				
	pstSleepModeMsg->numOfDefinitions=SWAP_ENDIAN32(pstSleepModeMsg->numOfDefinitions);		
	ConvertSLeepModeEndianess(pstSleepModeMsg, 0);
    pstSleepModeMsg->numOfDefinitions=SWAP_ENDIAN32(pstSleepModeMsg->numOfDefinitions);		    
    memcpy(&stLocalSleepModeMsg,pstSleepModeMsg,sizeof(ST_SLEEPMODE_MSG));
   
	
	WiMAXMessage_Init(
					pWMMsg,
					TYPE_SLEEP_MODE, 
					SUBTYPE_SLEEP_MODE_RESP, 
					(UCHAR *)&stLocalSleepModeMsg, 
					sizeof(ST_SLEEPMODE_MSG)-1 /* dont copy the buffer part */
					);

	/* copy the buffer part here */
	
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)		
	return SUCCESS;
}

int BeceemWiMAX_SendAppAntennaConfigresponse(PVOID pArg, PST_GPIO_SETTING_INFO pstGpioSettingInfo)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	ST_GPIO_SETTING_INFO stGpioSettingInfo;

	if(!pWiMAX || !pstGpioSettingInfo)
		return ERROR_INVALID_ARGUMENT;

	ConvertAntennaConfigRespEndianess(pstGpioSettingInfo);
	memcpy(&stGpioSettingInfo,pstGpioSettingInfo,sizeof(ST_GPIO_SETTING_INFO));

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)


	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_CONFIG_RESP, 
						(UCHAR *)&stGpioSettingInfo,
						sizeof(ST_GPIO_SETTING_INFO)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_SendAppAntennaScanningresponse(PVOID pArg, PST_RSSI_INFO_IND pstRssiInfoInd)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	ST_RSSI_INFO_IND stRssiInfoInd;

	if(!pWiMAX || !pstRssiInfoInd)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	ConvertAntennaScanningRespEndianess(pstRssiInfoInd);
	memcpy(&stRssiInfoInd,pstRssiInfoInd,sizeof(ST_RSSI_INFO_IND));

	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_SCANNING_RESP, 
						(UCHAR *)&stRssiInfoInd,
						sizeof(ST_RSSI_INFO_IND)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_SendAppAntennaSelectionresponse(PVOID pArg, UINT *puiAntSelResp)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT32 uiAntSelResp = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	*puiAntSelResp = SWAP_ENDIAN32(*puiAntSelResp);
	memcpy(&uiAntSelResp, puiAntSelResp, sizeof(UINT32));
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)


	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_SELECTION_SET_RESP, 
						(UCHAR *)&uiAntSelResp,
						sizeof(UINT32)
					  );
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_ProcessControlMsgResponse(PVOID pArg, DWORD dwBytesTransfered)
{
	ST_CONTROL_MSG stControlMsg = {0};
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	
	stControlMsg.usMsgLength = (USHORT)dwBytesTransfered;
	memcpy(stControlMsg.ucControlMsg,pWiMAX->m_szMsg,dwBytesTransfered);
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(	
						pWMMsg,
						TYPE_LOGGING_INFO, 
						SUBTYPE_CONTROLMSGS_LOGGING_NOTIFICATION, 
						(UCHAR *)&stControlMsg,
						sizeof(stControlMsg)
						);

	
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
INT BeceemWiMAX_ProximitySensor_Response(PVOID pArg, PUCHAR pPayload,UINT uiBytesTransferred)
{
	PWIMAXMESSAGE pWMMsg_Buffer = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	UCHAR  ucSubType = 0;
	UINT uiTotalSize = 0;	
	INT iRet=SUCCESS;
	PUCHAR PSNotifyBuffer=NULL;
	UINT nIndex = 0;
    UINT uiDataBeforeSwapping = 0;
    UINT uiDataAfterSwapping = 0;

	PST_PROXIMITY_CONTROL pstProximityControl=NULL;
	PUCHAR pucDataAfterSwapping = NULL;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	ucSubType = *(UCHAR *)(pPayload+1);

	uiTotalSize=uiBytesTransferred-4;

	PSNotifyBuffer=pPayload+2;  

	ALLOCATE_MEMORY(pucDataAfterSwapping,UCHAR,uiTotalSize+sizeof(ST_PROXIMITY_CONTROL)-sizeof(UCHAR),0);
	if(!pucDataAfterSwapping)
	{	
		iRet=ERROR_MALLOC_FAILED;						
		goto retPath;						
	}
    
    pstProximityControl=(PST_PROXIMITY_CONTROL)pucDataAfterSwapping;

	pstProximityControl->eProximityControl=ucSubType;
	pstProximityControl->uilength=uiTotalSize;

	
	for(nIndex = 0; nIndex < uiTotalSize ;)
		{

		memcpy(&uiDataBeforeSwapping,&PSNotifyBuffer[nIndex],sizeof(UINT));

		uiDataAfterSwapping =  SWAP_ENDIAN32(uiDataBeforeSwapping);   

		memcpy(&pstProximityControl->auchPayload[nIndex],&uiDataAfterSwapping,sizeof(UINT));

		nIndex+=sizeof(UINT);
		if((nIndex+sizeof(UINT))>uiTotalSize)
			break;

		uiDataBeforeSwapping = 0;
		uiDataAfterSwapping = 0;
		}		

	uiTotalSize=uiTotalSize+sizeof(ST_PROXIMITY_CONTROL)-sizeof(UCHAR);

	ALLOCATE_MEMORY(pWMMsg_Buffer,WIMAXMESSAGE,sizeof(WIMAXMESSAGE),0);

	
	if(!pWMMsg_Buffer)
	{	
		iRet=ERROR_MALLOC_FAILED;	
        FREE_MEMORY(pucDataAfterSwapping);
		goto retPath;						
	}
	WiMAXMessage_Init(	
		pWMMsg_Buffer,
		TYPE_PROXIMITY_SENSOR, 
		SUBTYPE_PROXIMITY_FUNCTIONS_RESP, 
		pucDataAfterSwapping,
		uiTotalSize
		);


		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg_Buffer);

        FREE_MEMORY(pucDataAfterSwapping);
		FREE_MEMORY(pWMMsg_Buffer);
retPath:
	return iRet;

}
int BeceemWiMAX_Resync_CID_Response(PVOID pArg, PUCHAR pPayload,DWORD dwBytesTransferred,USHORT usSwStatus)
{

	PWIMAXMESSAGE pWMMsg_Buffer = NULL;
	PST_LOG_NOTIFY_BUFFER pstLogNotifyBuffer=NULL;
	PST_RESYNC_LOG_ADDR_SIZE pstAddr_Size =NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UCHAR  ucType = 0;
	USHORT usType = 0;
	int iTotalSize = 0;

	PUCHAR pucDataAfterSwapping = NULL;
    int nIndex = 0;
    UINT uiDataBeforeSwapping = 0;
    UINT uiDataAfterSwapping = 0;

	PUCHAR poutdata=NULL;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;


	ucType = *(UCHAR *)pPayload;
	BECEEMCALLOC(pWMMsg_Buffer,PWIMAXMESSAGE,WIMAXMESSAGE,1);

	if(usSwStatus!=CID_LOGGING_NOTIFICATION)
	{
		//called with swstatus STATUS_RSP
		if(ucType==DISCONNECT_LOGGING_RESP)
		{
			usType=TYPE_RESYNC_LOG_CONTROL;
			switch(*(pPayload+1))
			{
			case SUBTYPE_DISCONNECT_LOGGING_DISABLE_RESPONSE://Disable the Log
				WiMAXMessage_Init(	
					pWMMsg_Buffer,
					usType, 
					SUBTYPE_RESYNC_DISABLE_RESP, 
					NULL,
					0
					);
			break;
			case SUBTYPE_DISCONNECT_LOGGING_ENABLE_RESPONSE:
				WiMAXMessage_Init(	
					pWMMsg_Buffer,
					usType, 
					SUBTYPE_RESYNC_ENABLE_RESP, 
					NULL,
					0
					);
			break;
			case SUBTYPE_DISCONNECT_LOGGING_LOG_INDICATION_RESPONSE:
				pstAddr_Size = (PST_RESYNC_LOG_ADDR_SIZE)(pPayload+2);
				pstAddr_Size->u32ADDR= ntohl(pstAddr_Size->u32ADDR);
				pstAddr_Size->u32TotalSize= ntohl(pstAddr_Size->u32TotalSize);

				BECEEMCALLOC(poutdata,PUCHAR,UCHAR,sizeof(ST_LOG_NOTIFY_BUFFER)+pstAddr_Size->u32TotalSize-1);

				pstLogNotifyBuffer=(PST_LOG_NOTIFY_BUFFER)poutdata;
				pstLogNotifyBuffer->u32TotalSize=pstAddr_Size->u32TotalSize;

				if(BeceemWiMAX_rdmBulk(pWiMAX,pstAddr_Size->u32ADDR,pstLogNotifyBuffer->u8Logbuffer,pstAddr_Size->u32TotalSize))
				{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_RX,"RDM failed \n");

				}else
				{
				WiMAXMessage_Init(	
						pWMMsg_Buffer,
						usType, 
						SUBTYPE_RESYNC_DISCONNECT_LOG_NOTIFICATION, 
						(UCHAR *)pstLogNotifyBuffer,
						sizeof(ST_LOG_NOTIFY_BUFFER)+pstLogNotifyBuffer->u32TotalSize-1
						);

				}
				BECEEM_MEMFREE(poutdata);
			break;
			default:
				DPRINT(DEBUG_ERROR, DEBUG_PATH_RX," INVALID Subtype %d receive for type %d\n",*(pPayload+1),*(pPayload));

			}
		}
		else // CID Log Response
		{
			usType=TYPE_CID_TRACKING_LOG_CONTROL;


			switch(*(pPayload+1))
			{
				case SUBTYPE_CID_LOGGING_DISABLE_RESPONSE://Disable the Log
					WiMAXMessage_Init(	
						pWMMsg_Buffer,
						usType, 
						SUBTYPE_CID_DISABLE_RESP, 
						NULL,
						0
						);
					break;
				case SUBTYPE_CID_LOGGING_ENABLE_RESPONSE:
					WiMAXMessage_Init(	
						pWMMsg_Buffer,
						usType, 
						SUBTYPE_CID_ENABLE_RESP, 
						NULL,
						0
						);
				break;
			}
		}
	}
	else if(ucType==START_CID_SNOOPING_RSP)
	{
		
		//2*sizeof(UINT32) IS OFFSET because of firmware struct in log buffer
		iTotalSize=dwBytesTransferred-2*sizeof(UINT32)-sizeof(USHORT);
		if(iTotalSize>0)
		{	
			pPayload +=2*sizeof(UINT32);

			BECEEMCALLOC(poutdata,PUCHAR,UCHAR,sizeof(ST_LOG_NOTIFY_BUFFER)+iTotalSize-1);
            pstLogNotifyBuffer=(PST_LOG_NOTIFY_BUFFER)poutdata;
			pstLogNotifyBuffer->u32TotalSize=iTotalSize;
			pucDataAfterSwapping = pstLogNotifyBuffer->u8Logbuffer;

			
			for(nIndex = 0; nIndex < iTotalSize ;)
			{

				memcpy(&uiDataBeforeSwapping,&pPayload[nIndex],sizeof(UINT));

				uiDataAfterSwapping =  SWAP_ENDIAN32(uiDataBeforeSwapping);   

				memcpy(&pucDataAfterSwapping[nIndex],&uiDataAfterSwapping,sizeof(UINT));

				nIndex+=sizeof(UINT);

				uiDataBeforeSwapping = 0;
				uiDataAfterSwapping = 0;
			}		

			WiMAXMessage_Init(	
				pWMMsg_Buffer,
				TYPE_CID_TRACKING_LOG_CONTROL, 
				SUBTYPE_CID_LOG_NOTIFICATION, 
				(UCHAR *)pstLogNotifyBuffer,
				sizeof(ST_LOG_NOTIFY_BUFFER)+pstLogNotifyBuffer->u32TotalSize-1
				);
			
			BECEEM_MEMFREE(poutdata);

		}
		
	}


	//Call call-back 
	if(pWMMsg_Buffer->stMessage.usType!=0)
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg_Buffer);
	
	BECEEM_MEMFREE(pWMMsg_Buffer);
	
		
	return SUCCESS;

}

