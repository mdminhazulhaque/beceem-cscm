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
 * Description	:	Transmit control message handlers.	
 * Author		:
 * Reviewer		:
 *
 */



#include "BeceemWiMAX.h"
#include "ProxyEndianHandler.h"
#include "vendorspecificextension.h"
#include "globalobjects.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

UINT8 gCheckSumErrorFlag = 0;

/**
@ingroup Utility_Functions
*	Function		:	This will create the appropriate data packet and send it to the driver via IOCTL.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is the pointer to the message to be sent to the device(driver).
*	@param	UINT uiLength - length of the message
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0;
	ST_VENDOR_SPECIFIC_EXTN_RET stVendorSpecificExtRet = {0};
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!BeceemWiMAX_IsValidHandle(pWiMAX))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid handle supplied for sending message !!!");
		return ERROR_INVALID_DEVICE_HANDLE;
	}

	if(pWiMAX->m_bFirmwareDownloadDone == FALSE)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "Firmware download under process, No messages will be sent to driver, please wait...");
		return ERROR_FIRMWARE_DOWNLOAD_INPROGRESS;
	}

	if (pWiMAX->m_iDeviceRemovalIssued)
	{
		/* Return since unload is triggered */
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Device Removal Issued is Triggered !!!");
		return ERROR_CLOSE_DEVICE;
	}

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
        return iRet;
	
	CorrectEndiannessForProxy(pMsg,uiLength,0);

		
	iRet = BeceemWiMAX_PowerSavingModeStatusCheck(pArg, pMsg);
	if(iRet)
		return iRet;
	if(!pWiMAX->m_bInitEEPROM)
		BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
	
	/*---Check for Vendor specific handler for this API call -START*/
	stVendorSpecificExtRet = VendorSpecificExtensionDispatch(pWiMAX, pMsg,uiLength);
	if(!stVendorSpecificExtRet.bContinueWithBCMPath || stVendorSpecificExtRet.iReturnValue)
	{
		return stVendorSpecificExtRet.iReturnValue;
	}
	/*---Check for Vendor specific handler for this API call -END*/

	
	switch(pMsg->usType)
	{
		case TYPE_NETWORK_SEARCH:
			BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
			return BeceemWiMAX_SendNetworkSearchRequest(pWiMAX, pMsg, uiLength);

		case TYPE_NETWORK_ENTRY:
			return BeceemWiMAX_SendNetEntryRequest(pWiMAX, pMsg, uiLength);
		
		case TYPE_LINK_DOWN:
			return BeceemWiMAX_SendLinkDownRequest(pWiMAX, pMsg, uiLength);

		case TYPE_SHUTDOWN:
		case TYPE_HIBERNATE:
			return BeceemWiMAX_SendShutDownORHibernateRequest(pWiMAX, pMsg, uiLength);

		case TYPE_SS_INFO:
			return BeceemWiMAX_SendAppSSInforesponse(pWiMAX, pMsg);

		case TYPE_MACADDRESS:
			if(gCheckSumErrorFlag)
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "CHECKSUM Computation failed\n");
				return ERROR_AUTH_CRC_MISMATCH;
			}
			BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
			return BeceemWiMAX_MACAddress(pWiMAX, pMsg, uiLength);

		case  TYPE_CHIPSET_CONTROL :
			return BeceemWiMAX_SendChipsetControlMessage(pWiMAX, pMsg, uiLength);

		case TYPE_STATISTICS :
			return BeceemWiMAX_SendStatisticsRequest(pWiMAX, pMsg, uiLength);

		case TYPE_IDLE_MODE:
			return BeceemWiMAX_SendIdleModeRequest(pWiMAX, pMsg, uiLength);

		case TYPE_MACMGMT:
			return BeceemWiMAX_SendMACMgmtMessage(pWiMAX, pMsg, uiLength);
		
		case TYPE_AUTHENTICATION:
			BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
			return BeceemWiMAX_SendAuthMessage(pWiMAX, pMsg, uiLength);

		case TYPE_SERVICE_FLOW:
			return BeceemWiMAX_SendServiceFlowMessage(pWiMAX,pMsg, uiLength);

		case TYPE_GPIO:
			return BeceemWiMAX_SendGPIOMessage(pWiMAX, pMsg, uiLength);

		case TYPE_START_LOG:
			return BeceemWiMAX_SendLogReqMessage(pWiMAX, pMsg, uiLength);

		case TYPE_STOP_LOG:
			return BeceemWiMAX_SendLogStopReqMessage(pWiMAX, pMsg, uiLength);

		case TYPE_VENDOR_SPECIFIC_INFO:
			BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
			return BeceemWiMAX_SendVendorSpecificInfoMessage(pWiMAX, pMsg, uiLength);

		case TYPE_MIB:
			if(!gCheckSumErrorFlag)
			{
				if (pWiMAX->m_bModemInShutdown)
					return ERROR_MODEM_IN_SHUT_DOWN_MODE;
				else if (pWiMAX->m_bIdleMode)
					return ERROR_MODEM_IN_IDLE_MODE;
				return BeceemWiMAX_MIBReqMessage(pWiMAX, pMsg, uiLength);
			}
			else
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "BINARY DOES NOT SUPPORT MIBS\n");
				return ERROR_AUTH_CRC_MISMATCH;
			}

		case TYPE_DEVICE_CONFIG:
			return SendDeviceConfigRequest(pWiMAX, pMsg, uiLength);
        
		case TYPE_SLEEP_MODE:
			return BeceemWiMAX_SendSleepModeMsg(pWiMAX,pMsg, uiLength);
    
		case TYPE_ANTENNA_SWITCHING_SELECTION:
			return BeceemWiMax_AntennaReqMessage(pWiMAX,pMsg, uiLength);

		case TYPE_SET_WIMAX_OPTIONS:
			return BeceemWiMAX_HandleWiMAXOptionsReq(pWiMAX, pMsg);
		
		case TYPE_MAC_LOGING_INFO:
			return BeceemWiMAX_ControlMACLogging(pWiMAX, pMsg,uiLength);

		case TYPE_PHY_LOGING_INFO:
			return BeceemWiMAX_ControlPHYLogging(pWiMAX, pMsg,uiLength);

		case TYPE_DRV_LOGING_INFO:
			return BeceemWiMAX_ControlDrvLogging(pWiMAX, pMsg,uiLength);
		case TYPE_OMADM:
			return BeceemWiMAX_OMADMReqMessage(pWiMAX, pMsg, uiLength);

		case TYPE_SECURITY:
			return BeceemWiMAX_SecReqMessage(pWiMAX, pMsg, uiLength);

		case TYPE_OMADM_CALIBRATION:
			return BeceemWiMAX_OMADMCalibReqMessage(pWiMAX, pMsg, uiLength);
			
		case TYPE_HALF_MINI_CARD:
			return BeceemWiMAX_HalfMiniCardMessage(pWiMAX, pMsg, uiLength);

		case TYPE_LOGGING_INFO:
			return BeceemWiMAX_LoggingRequestMessage(pWiMAX, pMsg, uiLength);
		
		case TYPE_FLASH_API:
			return BeceemWiMAX_FlashAPIHandler(pWiMAX,pMsg,uiLength);
			
	  case TYPE_LOGGING_CONTROL:
	      return BeceemWiMAX_DynLoggingHandler(pWiMAX, pMsg, uiLength);

	  case TYPE_CID_TRACKING_LOG_CONTROL:
	  case TYPE_RESYNC_LOG_CONTROL:
			return BeceemWiMAX_Resync_CID_LogHandler(pWiMAX, pMsg, uiLength);

	 case TYPE_PROXIMITY_SENSOR:
	      return BeceemWiMAX_ProximitySensorHandler(pWiMAX, pMsg, uiLength);
	default:
	      DPRINT(DEBUG_WARNING, DEBUG_PATH_TX,
		     "Invalid Message Type sent, This message will be ignored !!!");
			break;
	}
	return SUCCESS;
}

int BeceemWiMAX_LoggingRequestMessage(PBECEEMWIMAX pWiMAX,PWIMAX_MESSAGE pMsg,UINT uiLength)
{
	INT32 iLoggingReq = 0;
	
	UNUSED_ARGUMENT(uiLength);

	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
	case SUBTYPE_CONTROLMSGS_LOGGING_REQ:
		{
			iLoggingReq = *(PINT)pMsg->szData;
			
			if(iLoggingReq == CONTROLMSG_LOGGING_ENABLE)
				pWiMAX->m_bControlMessageLogging = TRUE;
			
			else if(iLoggingReq == CONTROLMSG_LOGGING_DISABLE)
				pWiMAX->m_bControlMessageLogging = FALSE;

			else if(iLoggingReq == CONTROLMSG_LOGGING_STATUS)
				{
					*(PINT)pMsg->szData = pWiMAX->m_bControlMessageLogging;
				}
		}
		break;

	default:
		return ERROR_INVALID_VALUE;
		break;
	}
	return SUCCESS;
}	

int BeceemWiMAX_HalfMiniCardMessage(PVOID pArg,PVOID pMsg,UINT uiLength)
{
	PWIMAX_MESSAGE pWMsg = (PWIMAX_MESSAGE)pMsg;
	PDRIVERPACKET pDpkt = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	INT iRet = 0,iRetval=0;
	INT iReturnVal = 0;
	DWORD dwNumBytesReturned = 0;
	UNUSED_ARGUMENT(uiLength);
	
	switch(pWMsg->usSubType)
		{
			case SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST:
				{
					BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)					
					DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
					DriverPacket_AppendChar(pDpkt, HMC_REQUEST);
					DriverPacket_AppendChar(pDpkt, SUBTYPE_HMC_STATUS_REQUEST);
					DriverPacket_AppendChar(pDpkt, 0);
					DriverPacket_AppendChar(pDpkt, 0);
					/* Append the data */
					DriverPacket_AppendData(	pDpkt, 
								(UCHAR *)&iReturnVal, 
								sizeof(UINT32)
							);

					iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_SEND_CONTROL_MESSAGE,
							(PVOID)pDpkt,
							sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pDpkt),
							(PVOID)pDpkt,
							sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pDpkt),
							&dwNumBytesReturned
						);
					if(!iRet && pDpkt->m_stPacket.aucData[4])
						{
							iRetval=ERROR_DEVICE_NOT_IN_HMC_SHUTDOWN;
							goto returnCode;
						}
						else if(iRet)
						{
							iRetval=iRet;
							goto returnCode;
						}
				}
				break;

			default:
				iRetval=ERROR_INVALID_VALUE;
				goto returnCode;
						
		}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}


int BeceemWiMAX_PowerSavingModeStatusCheck(PVOID pArg, PVOID pvMsg)
{
	PWIMAX_MESSAGE pWMsg = (PWIMAX_MESSAGE)pvMsg;
	LINK_STATE stLinkState = {0};
	UINT uiIdleModeFlag = 0, uiShutdownModeFlag = 0;
	int iRet, iListedApi = 0;
	PMIB_OBJECT_INFO pMIB = NULL;

	iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);
	if(iRet)
		return iRet;
	uiIdleModeFlag = stLinkState.bIdleMode;
	uiShutdownModeFlag = stLinkState.bShutdownMode;
	
	switch(pWMsg->usType)
		{
			case TYPE_SS_INFO:
			case TYPE_DEVICE_CONFIG:
			case TYPE_MACMGMT:
				iListedApi = 1;
				break;
			
			case TYPE_MIB:
				pMIB = (PMIB_OBJECT_INFO)pWMsg->szData;
				if(pMIB->u32MibObjId >= BCM_PROPREITARY_CFG_MIB_BASE_START && pMIB->u32MibObjId <= BCM_PROPREITARY_CFG_MIB_BASE_END)
					break;
				else 
					iListedApi = 1;
				break;

		}
	if(iListedApi&& uiIdleModeFlag)
		return ERROR_RDM_IN_IDLEMODE;
	else if(iListedApi&& uiShutdownModeFlag)
		return ERROR_MODEM_IN_SHUT_DOWN_MODE;
	else
		return SUCCESS;

}



int BeceemWiMAX_ControlMACLogging(PVOID pArg, PVOID pvMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAX_MESSAGE pMsg = (PWIMAX_MESSAGE)pvMsg;

	UNUSED_ARGUMENT(uiLength);

	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
	case SUBTYPE_ENABLE_MAC_LOGGING:
		pWiMAX->m_bEnableMACLogging = TRUE;
		break;
	case SUBTYPE_DISABLE_MAC_LOGGING:
		pWiMAX->m_bEnableMACLogging = FALSE;
		break;

	default:
		return ERROR_INVALID_VALUE;
		break;
	}
	return SUCCESS;
}
int BeceemWiMAX_ControlPHYLogging(PVOID pArg, PVOID pvMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAX_MESSAGE pMsg = (PWIMAX_MESSAGE)pvMsg;
	UNUSED_ARGUMENT(uiLength);

	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
	case SUBTYPE_ENABLE_PHY_LOGGING:
		pWiMAX->m_bEnablePHYLogging = TRUE;
		break;
	case SUBTYPE_DISABLE_PHY_LOGGING:
		pWiMAX->m_bEnablePHYLogging = FALSE;
		break;

	default:
		return ERROR_INVALID_VALUE;
		break;
	}
	return SUCCESS;
}

int BeceemWiMAX_ControlDrvLogging(PVOID pArg, PVOID pvMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAX_MESSAGE pMsg = (PWIMAX_MESSAGE)pvMsg;
	int iRet = 0;
	UNUSED_ARGUMENT(uiLength);

	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
	case SUBTYPE_SET_DRV_LOGGING:
		iRet = DeviceIO_SendOnly(&(pWiMAX->m_Device), IOCTL_DUMP_DEBUG, (PVOID)&pMsg->szData, sizeof(ST_DUMP_DEBUG));
		if (iRet < 0)
			return iRet;
		break;
	
	default:
		return ERROR_INVALID_VALUE;
		break;
	}
	return SUCCESS;
}

/**
@ingroup authentication_status
*	Function		:	When Application wishes to send SUBTYPE_EAP_TRANSFER_REQ or 
					SUBTYPE_EAP_COMPLETE_REQ this function will be called.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is the pointer to the message to be sent to the device(driver).
*	@param	UINT uiLength - length of the message
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendAuthMessage(PVOID pArg, PVOID pvMsg, UINT uiLength)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAX_MESSAGE pMsg = (PWIMAX_MESSAGE)pvMsg;


	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
		case SUBTYPE_EAP_TRANSFER_REQ:
		case SUBTYPE_EAP_COMPLETE_REQ:
		case SUBTYPE_EAP_SPOOF_INFO_REQ:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		     "Sending IOCTL_AUTH_CONTROL_REQ - SUBTYPE_EAP_TRANSFER/COMPLETE_REQ/SUBTYPE_EAP_SPOOF_INFO_REQ");
			
			iRet = BeceemWiMAX_AuthPacketTransferRequest(pWiMAX, pMsg);
			if(iRet)
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Sending IOCTL_AUTH_CONTROL_REQ - SUBTYPE_EAP_TRANSFER/COMPLETE_REQ/EAP_SPOOF_INFO_REQ Failed!.");
				return iRet;
			}
			break;
		case SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		     "Sending IOCTL_AUTH_CONTROL_REQ - SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST.");
			iRet = BeceemWiMAX_EncryptPrivateRequest(pWiMAX, pMsg);
			if(iRet)
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Sending IOCTL_AUTH_CONTROL_REQ - SUBTYPE_EAP_TRANSFER/COMPLETE_REQ Failed!.");
				return iRet;
			}
			
			break;
	}

	return SUCCESS;
}
/**
@ingroup authentication_status
*	Function		:	This function is called from BeceemWiMAX_SendAuthMessage. 
					It sends the authentication request payload to the driver.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is the pointer to the message to be sent to the device(driver).
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_EncryptPrivateRequest(PVOID pArg, PWIMAX_MESSAGE pWMsg)
{
	PDRIVERPACKET pDpkt = NULL;
	PST_EAP_RSA_PRIVATE pEAPEncrypt =(PST_EAP_RSA_PRIVATE) 	pWMsg->szData;
	UINT32 u32Length = pEAPEncrypt->u32DataLen;
	int iRetval=0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX || !pWMsg)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)	
	DriverPacket_InitWithVal(pDpkt,0, AUTH_HOST_SS_MSG);
	DriverPacket_AppendChar(pDpkt, AUTH_ENCRYPT_PVT_REQ);


	ConvertToHostEndian(	(UCHAR *)pEAPEncrypt, 
					sizeof(UINT32)*2,
						1
				);


	DriverPacket_AppendData(pDpkt, (UCHAR *)pEAPEncrypt,sizeof(ST_EAP_RSA_PRIVATE) - 1);
	DriverPacket_AppendData(pDpkt, (UCHAR *)pEAPEncrypt->u8Data,u32Length);

	iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
	BECEEM_MEMFREE(pDpkt)
	return iRetval;

}


int BeceemWiMAX_AuthPacketTransferRequest(PVOID pArg, PWIMAX_MESSAGE pWMsg)
{
	PDRIVERPACKET pDpkt = NULL;
	PEAP_PAYLOAD pstEAPPayload = (PEAP_PAYLOAD)pWMsg->szData;
	PEAP_COMPLETE_INFO pstEAPCompleteInfo = (PEAP_COMPLETE_INFO)pWMsg->szData;

	USHORT usLen = 0;
	USHORT usType = 0;
	int iRetval=0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX || !pWMsg)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt,0, AUTH_HOST_SS_MSG);

	
	switch(pWMsg->usSubType)
	{
	case SUBTYPE_EAP_TRANSFER_REQ:

		DriverPacket_AppendChar(pDpkt, AUTH_RSP_PAYLOAD);

		usLen = SWAP16(((pstEAPPayload->usEAPPayloadLength)+4));
		DriverPacket_AppendData(pDpkt,(UCHAR *)&usLen, 2);

		usType = SWAP16(0x00A0);
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usType, 2);

		usLen = SWAP16(pstEAPPayload->usEAPPayloadLength);
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usLen, 2);

		DriverPacket_AppendData(pDpkt, (UCHAR *)pstEAPPayload->aucEAPPayload, pstEAPPayload->usEAPPayloadLength);
		break;
	case SUBTYPE_EAP_SPOOF_INFO_REQ:

		DriverPacket_AppendChar(pDpkt, AUTH_RSP_PAYLOAD_NAI_SPOOFING);

		usLen = SWAP16(pstEAPPayload->usEAPPayloadLength);
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usLen, 2);
		DriverPacket_AppendData(pDpkt, (UCHAR *)pstEAPPayload->aucEAPPayload, pstEAPPayload->usEAPPayloadLength);

		break;
	case SUBTYPE_EAP_COMPLETE_REQ:

		DriverPacket_AppendChar(pDpkt, AUTH_COMP_PAYLOAD);

		usLen = SWAP16(((pstEAPCompleteInfo->usEAPCompletePayloadLength)+4));
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usLen, 2);

		usType = SWAP16((USHORT)(pstEAPCompleteInfo->ucStatus == 0 ? 1 : 2));
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usType, 2);

		usLen = SWAP16(pstEAPCompleteInfo->usEAPCompletePayloadLength);
		DriverPacket_AppendData(pDpkt, (UCHAR *)&usLen, 2);

		DriverPacket_AppendData(pDpkt, (UCHAR *)pstEAPCompleteInfo->aucEAPCompletePayload, pstEAPCompleteInfo->usEAPCompletePayloadLength);
		break;

	default:
		iRetval=ERROR_INVALID_SUBTYPE;
		goto returnCode;
	}
	iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}


/**
@ingroup network_search
*	Function		:	This function is called when network search is requested by the application. 
					It handles SUBTYPE_NETWORK_SEARCH_REQ,SUBTYPE_NETWORK_SEARCH_RESP and SUBTYPE_NETWORK_SEARCH_ABORT_REQ for TYPE_NETWORK_SEARCH.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is the pointer to the message to be sent to the device(driver).
*	@param	UINT uiLength - length of the message
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendNetworkSearchRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;

	switch(pMsg->usSubType)
	{
		case SUBTYPE_NETWORK_SEARCH_REQ:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "In SUBTYPE_NETWORK_SEARCH_REQ ");
			return BeceemWiMAX_NetworkSearchRequest(pArg, (PST_NETWORK_SEARCH_PARAMS) pMsg->szData);
			
		case SUBTYPE_NETWORK_SEARCH_ABORT_REQ:
			return BeceemWiMAX_NetworkSearchAbort(pArg);
			
		case SUBTYPE_NETWORK_SEARCH_RESULTS_QUERY_REQ:
			return BeceemWiMAX_NetworkSearchResults(pArg);
		case SUBTYPE_CAPL_BASED_SEARCH_REQ:
	           return BeceemWiMAX_CaplBasedSearchRequest(pArg, (PST_CAPL_BASED_SEARCH_PARAMS) pMsg->szData,SUBTYPE_CAPL_BASED_SEARCH_REQ); 
   
		case SUBTYPE_NETWORK_DISCOVERY_REQ:
			return 	BeceemWiMAX_NetworkNSPSearchRequest(pArg,pMsg->szData); 
		
		case SUBTYPE_SET_LOWPOWER_SCANMODE_REQ:
			return  BeceemWiMAX_SetLowPowerScanModeRequest(pArg,pMsg->szData);
		case SUBTYPE_AUTONOMOUS_SEARCH_REQ:
			return  BeceemWiMAX_CaplBasedSearchRequest(pArg,(PST_CAPL_BASED_SEARCH_PARAMS)pMsg->szData,SUBTYPE_AUTONOMOUS_SEARCH_REQ);
			
		
			   	
     
	}
	return SUCCESS;
}

int BeceemWiMAX_SetLowPowerScanModeRequest(PVOID pArg,PVOID pPayload)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	INT32 iRet = 0;
	int iRetval=0;

	
	if(!pPayload)
		return ERROR_INVALID_ARGUMENT;

	ConvertToHostEndian((UCHAR *)pPayload, sizeof(ST_SCAN_CONFIG_PARAM), 1);
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_LOW_POWER);
	DriverPacket_AppendChar(pDpkt, SUBTYPE_NW_SEARCH_LOW_POWER_REQ);
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendData(pDpkt,(UCHAR *)pPayload, sizeof(ST_SCAN_CONFIG_PARAM));
	
	iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "Sending IOCTL_LINK_REQ - SUBTYPE_NW_SEARCH_LOW_POWER_REQ failed!!");
		iRetval=(int)iRet;
		goto returnCode;
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}

int BeceemWiMAX_NetworkNSPSearchRequest(PVOID pArg,PVOID pPayload)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0,iRetval=0,iPacketSize = 0,iIndex = 0;
	
	PST_CONFIG_INFO_PER_NAPID pstConfigInfoPerNAPID = pPayload;


	if(!pWiMAX || !pPayload)
		return ERROR_INVALID_ARGUMENT;

	if(BeceemWiMAX_IsSyncAcheived(pWiMAX))
		return ERROR_SS_IS_INSYNC;
		
		if(pWiMAX->m_bNetSearchInProgress || pWiMAX->m_bNSPSearchInProgress)
			return ERROR_NET_SEARCH_INPROGRESS;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
		/* Build the packet with VCID, Status*/
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_REQ);
	
		DriverPacket_AppendChar(pDpkt, SUBTYPE_BCM_WIMAX_NW_SEARCH_REQ);
		DriverPacket_AppendChar(pDpkt, 0);
		DriverPacket_AppendChar(pDpkt, 0);
	iPacketSize  = sizeof(ST_CONFIG_INFO_PER_NAPID) ;
	
		for(iIndex = 0; iIndex < (INT32)pstConfigInfoPerNAPID->u32CountNumBS; iIndex++)
		{
			pstConfigInfoPerNAPID->stPhyInfo[iIndex].u32CF = (UINT32)(pstConfigInfoPerNAPID->stPhyInfo[iIndex].u32CF* 8.0/1000.0);
			pstConfigInfoPerNAPID->stPhyInfo[iIndex].u32BW = (UINT32)(pstConfigInfoPerNAPID->stPhyInfo[iIndex].u32BW *1000);
		}

		ConvertToHostEndian(	(UCHAR *)pstConfigInfoPerNAPID, 
							iPacketSize,
							1
						);
			/* Append the data */
		DriverPacket_AppendData(	pDpkt, 
								(UCHAR *)pstConfigInfoPerNAPID, 
								iPacketSize
							);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending SUBTYPE_BCM_WIMAX_NW_SEARCH_REQ.");
		iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
		if(iRet)
		{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "Sending IOCTL_LINK_REQ - SUBTYPE_BCM_WIMAX_NW_SEARCH_REQ failed!!");
			iRetval=iRet;
			goto returnCode;
		}
		
		if (pWiMAX->m_bModemInShutdown)
			pWiMAX->m_bModemInShutdown = FALSE;
		
		pWiMAX->m_bNSPSearchInProgress = TRUE;
			
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;

}
int BeceemWiMAX_NetworkSearchRequest(PVOID pArg, PST_NETWORK_SEARCH_PARAMS pstNetSearchParams)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0, iRet = 0, i = 0;
	int CfRange2_5Count = 0, CfRange3_5Count = 0, CfRange2_3Count = 0;
	int iPacketSize = 0;

	if(!pWiMAX || !pstNetSearchParams)
		return ERROR_INVALID_ARGUMENT;

	if(BeceemWiMAX_IsSyncAcheived(pWiMAX))
		return ERROR_SS_IS_INSYNC;

    if(BeceemWiMAX_IsNetworkSearchInProgress(pWiMAX))
        return ERROR_NET_SEARCH_INPROGRESS;

#if 0
	if(pstNetSearchParams->stContCFRange.u32CenterFrequencyStep < 250)
		return ERROR_CF_STEP_INVALID;
#endif

	/*Check if the discrete frequencies given are in same band*/
	for(i=0; i<(int)pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq; i++)
	{	
		if(pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] >= FREQBAND_2_3_RANGE_START && pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] <= FREQBAND_2_3_RANGE_END)
			CfRange2_3Count++;
		else if(pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] >= FREQBAND_2_5_RANGE_START && pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] <= FREQBAND_2_5_RANGE_END)
			CfRange2_5Count++;
		else if(pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] >= FREQBAND_3_5_RANGE_START && pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] <= FREQBAND_3_5_RANGE_END)
			CfRange3_5Count++;
		else 
			return ERROR_DISCRETEFREQS_NOT_IN_SAME_RANGE;
				
	}

	if((UINT32)CfRange2_3Count != pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq && (UINT32)CfRange2_5Count != pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq && (UINT32)CfRange3_5Count != pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq)
		return ERROR_DISCRETEFREQS_NOT_IN_SAME_RANGE;

	/*Check if the given discrete frequencies are sorted*/
	if(pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq > 0)
	{
		UINT *puiFreqList = pstNetSearchParams->stDiscreteCFList.au32CenterFreqList;
		QuickSort(puiFreqList, (int)pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq);
	}
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	/* Build the packet with VCID, Status*/
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_TYPE_BCM_WIMAX);
   
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);

	iPacketSize  = sizeof(ST_NETWORK_SEARCH_PARAMS) +  (pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq - MAX_NUM_DISCRETE_CF_SUPPORTED) * sizeof(UINT32);
	

	/* conversion from KHZ to MHZ and multipy it by 8 */
	pstNetSearchParams->stContCFRange.u32CenterFreqStart =
		(UINT32)(pstNetSearchParams->stContCFRange.u32CenterFreqStart/1000.0 * 8.0);
	
	pstNetSearchParams->stContCFRange.u32CenterFrequencyEnd=
		(UINT32)(pstNetSearchParams->stContCFRange.u32CenterFrequencyEnd/1000.0 * 8.0);

	pstNetSearchParams->stContCFRange.u32CenterFrequencyStep = 
		(UINT32)(pstNetSearchParams->stContCFRange.u32CenterFrequencyStep * 8.0/1000.0);
	for(i =0; i < (int) pstNetSearchParams->stDiscreteCFList.u32NumDiscreteFreq ; i++)
	{
		pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] = 
				(UINT32)(pstNetSearchParams->stDiscreteCFList.au32CenterFreqList[i] *8.0 /1000);
	}

	for(i =0; i < (int) pstNetSearchParams->stBandwidthList.u32NumOfBwsToScan; i++)
	{
		pstNetSearchParams->stBandwidthList.au32BandwidthList[i] = 
				(UINT32)(pstNetSearchParams->stBandwidthList.au32BandwidthList[i] * 1000);
	}

	/* Convert data to Host Endian*/
	ConvertToHostEndian(	(UCHAR *)pstNetSearchParams, 
							iPacketSize,
							1
						);
				
	/* Append the data */
	DriverPacket_AppendData(	pDpkt, 
								(UCHAR *)pstNetSearchParams, 
								iPacketSize
							);

    /* timing bug fix, everything must be setup correctly
       Prior to sending the Driver packet. */
    if (pstNetSearchParams->u32Duration == 0)
    {
        /* reset PRIOR to sending out info.
           this is because the repsonse could be processed
           prior to you launched the thread.
	    */
        pWiMAX->m_bNetSearchInProgress = TRUE;
        pWiMAX->m_bKillNetSearchThread = 0;
        Event_Reset(&(pWiMAX->m_objNetSearchEvent));

        //must reset prior to making the call.
        Event_Reset(&(pWiMAX->m_objNetSearchEventMsgRecvd));
    }	
    else
    {
        //we would just leave it..
        //same as before
    }
    
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending NW_SEARCH_REQ.");
	iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending IOCTL_LINK_REQ - NW_SEARCH_REQ failed!!");

        if (pstNetSearchParams->u32Duration == 0)
        {
            /* Set back to the initial state since we failed to launch the request */
            pWiMAX->m_bNetSearchInProgress = FALSE;
            pWiMAX->m_bKillNetSearchThread = 0;
            Event_Set(&(pWiMAX->m_objNetSearchEvent));
            Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd));
        }

        iRetval=iRet;
		goto returnCode;
	}
		
	if (pWiMAX->m_bModemInShutdown)
		pWiMAX->m_bModemInShutdown = FALSE;

	if (pstNetSearchParams->u32Duration == 0)
	{
		if(Thread_Run(&(pWiMAX->m_NetSearchThread),(PTHREADFUNC)BeceemWiMAX_NetSearchThreadFunc,pWiMAX))
		{
            /* Set back to the initial state since we failed to launch the request */
            pWiMAX->m_bNetSearchInProgress = FALSE;
            pWiMAX->m_bKillNetSearchThread = 0;
            Event_Set(&(pWiMAX->m_objNetSearchEvent));
            Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd));
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching Network Search thread failed !!!");
			iRetval=ERROR_THREAD_NOT_STARTED;
			goto returnCode;
		}
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Net search thread started");
	}	

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;


}
int BeceemWiMAX_CaplBasedSearchRequest(PVOID pArg, PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams,USHORT	usSubType)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0,iRet = 0;
	int iPacketSize = 0;
	UINT32 u32RqstIndx = 0;
	
	if(!pWiMAX || !pstCaplSearchParams)
		return ERROR_INVALID_ARGUMENT;

	if(BeceemWiMAX_IsSyncAcheived(pWiMAX))
		return ERROR_SS_IS_INSYNC;

    if(BeceemWiMAX_IsNetworkSearchInProgress(pWiMAX))
        return ERROR_NET_SEARCH_INPROGRESS;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	/* Build the packet with VCID, Status*/
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_REQ);

	if(usSubType==SUBTYPE_AUTONOMOUS_SEARCH_REQ)
		DriverPacket_AppendChar(pDpkt, SUBTYPE_AUTONOMOUS_BASED_NW_SEARCH_REQ);
	else
		DriverPacket_AppendChar(pDpkt, NW_SEARCH_TYPE_CAPL);


	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);
    
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"######Sending CAPL Search Request######\n");
    
	iPacketSize  = sizeof(ST_CAPL_BASED_SEARCH_PARAMS)
		-(((MAX_NUM_CHANNELS - pstCaplSearchParams->stChannelSpec.u32NumChannels) * sizeof(ST_PHY_INFO))); 
    
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"Search request size = %d\n",iPacketSize);

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," CinrThreshold	:%d \n",pstCaplSearchParams->iCinrThreshold);
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," RssiThreshold	:%d \n",pstCaplSearchParams->iRssiThreshold);
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," ReportType	:%d \n",pstCaplSearchParams->u32ReportType);
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," Duration	:%d \n",pstCaplSearchParams->u32Duration);
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," Flags		:%d \n",pstCaplSearchParams->u32Flags);


	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," #NapID		:%d \n",pstCaplSearchParams->u32NumNapID);
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," List of NapId: \n");
	for(u32RqstIndx=0;u32RqstIndx<pstCaplSearchParams->u32NumNapID; u32RqstIndx++)
	{
		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX," %d \n",pstCaplSearchParams->au32NapIdList[u32RqstIndx]);
	}

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"Num Channels included = %d\n",pstCaplSearchParams->stChannelSpec.u32NumChannels);

	for(u32RqstIndx = 0; u32RqstIndx < pstCaplSearchParams->stChannelSpec.u32NumChannels; u32RqstIndx++)
	{
		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"Search Request Index:%d\tCenteFreq:%d\tBandwidth=%dKHz\n", 
			u32RqstIndx,pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32CenterFreq, pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32Bandwidth);
		pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32CenterFreq 
			= (UINT32)(pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32CenterFreq * 8.0 /1000.0);
		pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32Bandwidth 
			= (UINT32)(pstCaplSearchParams->stChannelSpec.stChannelList[u32RqstIndx].u32Bandwidth * 1000); 
	} 
		


	if(iPacketSize>0)
	{		
			/* Append the data */
        PUCHAR pPayloadConvert = (PUCHAR )(pDpkt->m_stPacket.aucData+pDpkt->m_iPayloadLen);

		DriverPacket_AppendData(pDpkt,(UCHAR *)pstCaplSearchParams,iPacketSize);
		ConvertToHostEndian(pPayloadConvert, iPacketSize,1);
	}


    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending CAPL_SEARCH_REQ.");
	iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending IOCTL_LINK_REQ - CAPL_SEARCH_REQ failed!!");
		iRetval=iRet;
		goto returnCode;
	}
		
	if (pWiMAX->m_bModemInShutdown)
		pWiMAX->m_bModemInShutdown = FALSE;
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;


}

/**
@ingroup network_search
*	Function		:	A thread function for network search request which will wait for first response from the device
					or time out and send network search abort message to the device.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on successful or ERROR_INVALID_ARGUMENT on failure.
*/
PVOID BeceemWiMAX_NetSearchThreadFunc(PVOID pArg)
{
	int i = 0;
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	for(i=0; i<TIMEOUT_FOR_NET_SEARCH;i+=200)
	{
		if(pWiMAX->m_bKillNetSearchThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objNetSearchEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_NET_SEARCH)
	{
		BSLIST stBsList;
		PWIMAXMESSAGE pWMMsg = NULL;
		
		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
		/* Notify the NETWORK SEARCH NAK to APP */
		stBsList.u32NumBaseStationsFound = 0;
		stBsList.u32SearchCompleteFlag = 2;
		memset(stBsList.stBSInformation, 0, sizeof(BSINFO));

		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_NETWORK_SEARCH, 
						SUBTYPE_NETWORK_SEARCH_RESP, 
						(UCHAR *) &stBsList,
						sizeof(BSLIST)
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
		iRet = BeceemWiMAX_NetworkSearchAbort(pWiMAX);
		if(iRet)
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to abort NETWORK SEARCH ");
		}
	}
	pWiMAX->m_bNetSearchInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd)); 

	return SUCCESS;
}

VOID BeceemWiMAX_KillNetSearchThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillNetSearchThread--------");
	Event_Set(&(pWiMAX->m_objNetSearchEvent));
	pWiMAX->m_bKillNetSearchThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillNetSearchThread---------");

	return;
}

VOID BeceemWiMAX_KillDeviceInsertThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start BeceemWiMAX_KillDeviceInsertThread--------");
	
	pWiMAX->m_bKillWiMAXCommonAPI_DeviceInsertMonitor = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End BeceemWiMAX_KillDeviceInsertThread---------");

	return;
}

VOID BeceemWiMAX_KillMACAddressWriteThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillMACAddressWriteThread--------");
	Event_Set(&(pWiMAX->m_objMACaddressRespEvent));
	pWiMAX->m_bKillMACAddressRespThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillMACAddressWriteThread---------");
}

VOID BeceemWiMAX_KillIdleModeWakeupThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillIdleModeWakeupThread--------");
	pWiMAX->m_bKillIdleModeWakeupThread= 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillIdleModeWakeupThread---------");
	
}
VOID BeceemWiMAX_KillWiMAXOptionsThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start m_bKillWIMAXOptionsSetInProgress--------");
	Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
	pWiMAX->m_bKillWIMAXOptionsSetInProgress = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End m_bKillWIMAXOptionsSetInProgress---------");
}

VOID BeceemWiMAX_KillShutDownRespThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillShutDownRespThread--------");
	Event_Set(&(pWiMAX->m_objShutDownRespEvent));
	pWiMAX->m_bKillShutDownRespThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillShutDownRespThread---------");
}

/**
@ingroup network_search
*	Function		:	To abort the network search

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on successful or ERROR_INVALID_ARGUMENT on failure.
*/
int BeceemWiMAX_NetworkSearchAbort(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0, iRet = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	Event_Set(&(pWiMAX->m_objNetSearchEvent));
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	/* Build the packet with VCID, Status*/
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_ABORT);
	DriverPacket_AppendChar(pDpkt, 0);
			
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending NW_SEARCH_ABORT.");
	iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending IOCTL_LINK_REQ - NW_SEARCH_ABORT failed!!");
		iRetval=iRet;
		goto returnCode;
	}
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}
/**
@ingroup network_search
*	Function		:	This function is used to query on the network search results. It is used to retrieve 
					the results of the last network search performed. 

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on successful or ERROR_INVALID_ARGUMENT on failure.
*/
int BeceemWiMAX_NetworkSearchResults(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	DWORD dwNumBytesReturned;
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0, iRet = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	/* Build the packet with VCID, Status*/
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_QUERY);
	DriverPacket_AppendChar(pDpkt, 0);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending NW_SEARCH_QUERY.");
	iRet =  DeviceIO_Send(	&(pWiMAX->m_Device), 
							IOCTL_LINK_REQ, 
							DriverPacket_GetPacketToSend(pDpkt), 
							DriverPacket_GetPacketToSendLength(pDpkt),
							NULL, 
							0,
							&dwNumBytesReturned
						);
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending IOCTL_LINK_REQ - NW_SEARCH_QUERY failed!!");
		iRetval=iRet;
		goto returnCode;	
	}
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}


/**
@ingroup network_entry_exit
*	Function		:	This will handle following messages
*								SUBTYPE_SYNC_UP_REQ
*								SUBTYPE_NETWORK_ENTRY_REQ
*								SUBTYPE_NETWORK_ENTRY_STATUS_REQ

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is the pointer to the message to be sent to the device(driver)
*	@param	UINT uiLength - length of the message
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendNetEntryRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0, iRet = 0;
	int iVendorInfoLen = 0;
	PVENDOR_SPECIFIC_LINKPARAMS  pVendorSpecificInfo = 0;
	PST_SYNCUP_REQ_PARAMS pstSyncupReqParams;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);

	switch(pMsg->usSubType)
	{
		/* SYNC UP REQUEST */
		case SUBTYPE_SYNC_UP_REQ :

            pWiMAX->m_bNSPSearchInProgress = FALSE;
			if(BeceemWiMAX_IsSyncInProgress(pWiMAX))
			{
					iRetval=ERROR_SYNC_UP_INPROGRESS;
					goto returnCode;
			}	
			if(pWiMAX->m_bMACAddressWriteInProgress)
			{
					iRetval=ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS;
					goto returnCode;
			}	
			if(!BeceemWiMAX_IsSyncAcheived(pWiMAX))
			{
				int iRet = 0;

				DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_PAYLOAD);
				DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_SYNC_UP_PAYLOAD);
				DriverPacket_AppendChar(pDpkt, 0);
				DriverPacket_AppendChar(pDpkt, 0);

				/* Convert data to Host Endian*/

				pstSyncupReqParams = (PST_SYNCUP_REQ_PARAMS )pMsg->szData;
				pstSyncupReqParams->u32CenterFreq = (unsigned int)(pstSyncupReqParams->u32CenterFreq*8.0/1000.0);
				pstSyncupReqParams->u32Bandwidth = (unsigned int)(pstSyncupReqParams->u32Bandwidth*1000.0);
				
				pstSyncupReqParams->u32CenterFreq = htonl(pstSyncupReqParams->u32CenterFreq);
				pstSyncupReqParams->u32Bandwidth = htonl(pstSyncupReqParams->u32Bandwidth);
				pstSyncupReqParams->u32PreambleId = htonl(pstSyncupReqParams->u32PreambleId);

				DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(ST_SYNCUP_REQ_PARAMS));


                /* timing fix, all of these must be set prior to issueing the driver packet */
                pWiMAX->m_bSyncUpInProgress = TRUE;
                pWiMAX->m_bKillSyncUpThread = 0;
                Event_Reset(&(pWiMAX->m_objSyncUpEvent));
                Event_Reset(&(pWiMAX->m_objSyncUpEventMsgRecvd));


		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
			     "Sending LINK_UP_REQ_PAYLOAD-LINK_UP_REQ_SYNC_UP_PAYLOAD.");
				iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
				if(iRet)
                {
                	//reset on failure
                    pWiMAX->m_bSyncUpInProgress = FALSE;
                    Event_Set(&(pWiMAX->m_objSyncUpEvent));
                    Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
                    iRetval=iRet;
					goto returnCode;
                }

				if (pWiMAX->m_bModemInShutdown)
					pWiMAX->m_bModemInShutdown = FALSE;

				if(Thread_Run(&(pWiMAX->m_SyncUpThread),(PTHREADFUNC)BeceemWiMAX_SyncUpThreadFunc,pWiMAX))
				{
					//reset on failure
                    pWiMAX->m_bSyncUpInProgress = FALSE;
                    Event_Set(&(pWiMAX->m_objSyncUpEvent));
                    Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));

			      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				     "Launching NetEntry thread failed !!!");
					iRetval=ERROR_THREAD_NOT_STARTED;
					goto returnCode;
				}
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "SyncUp thread started");
			}
			else
			{
				iRetval=BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_NET_ENTRY_CMD_CODE, NULL);
				goto returnCode;
			}
			break;
		/* NET ENTRY REQUEST */
		case SUBTYPE_NETWORK_ENTRY_REQ:
			{
				LINK_STATE stLinkState;

				if(pWiMAX->m_bMACAddressWriteInProgress)
				{
					iRetval=ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS;
					goto returnCode;
				}			
		

                if(BeceemWiMAX_IsNetworkEntryInProgress(pWiMAX))
				{
					iRetval=ERROR_NET_ENTRY_INPROGRESS;
					goto returnCode;
				}	
				BeceemWiMAX_GetLinkState(pWiMAX, &stLinkState);

				if(stLinkState.ucLinkStatus == LINKUP_DONE)
				{
					iRetval=SUCCESS;
					goto returnCode;
				}

				DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_PAYLOAD);
				DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_NET_ENTRY_PAYLOAD);

				pVendorSpecificInfo = (PVENDOR_SPECIFIC_LINKPARAMS)pMsg->szData; 

				/* copy the length first */
				iVendorInfoLen = htonl(pVendorSpecificInfo->u32VendorSpecificInformationLength);
				DriverPacket_AppendData(pDpkt, (UCHAR *)&iVendorInfoLen, sizeof(UINT32));
				/* copy the data */
				if(pVendorSpecificInfo->u32VendorSpecificInformationLength > 0 && pVendorSpecificInfo->u32VendorSpecificInformationLength < MAX_BUFFER_LENGTH)
					DriverPacket_AppendData(pDpkt, pVendorSpecificInfo->au8VendorSpecificInformation, pVendorSpecificInfo->u32VendorSpecificInformationLength);

                /* timing bug fix. set this up prior to sending the driver packet */
                pWiMAX->m_bNetEntryInProgress = TRUE;
                pWiMAX->m_bKillNetEntryThread = 0;
                Event_Reset(&(pWiMAX->m_objNetEntryEvent));
                Event_Reset(&(pWiMAX->m_objNetEntryEventMsgRecvd));
               
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
			 "Sending LINK_UP_REQ_PAYLOAD-LINK_UP_REQ_NET_ENTRY_PAYLOAD.");
				iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);

				if(iRet)
                {
                    //If failed. then set again Before sending
                    pWiMAX->m_bNetEntryInProgress = FALSE;
                    Event_Set(&(pWiMAX->m_objNetEntryEvent));
                    Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));
					iRetval=iRet;
					goto returnCode;
                }

				if(Thread_Run(&(pWiMAX->m_NetEntryThread),(PTHREADFUNC)BeceemWiMAX_NetEntryThreadFunc,pWiMAX))
				{
                    //Set again 
                    pWiMAX->m_bNetEntryInProgress = FALSE;
                    Event_Set(&(pWiMAX->m_objNetEntryEvent));
                    Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));

			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching NetEntry thread failed !!!");
					iRetval=ERROR_THREAD_NOT_STARTED;
					goto returnCode;
				}
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "NetEntry thread started");
			}
			break;

		/* NET ENTRY STATUS REQUEST */
		case SUBTYPE_NETWORK_ENTRY_STATUS_REQ:
			iRetval=BeceemWiMAX_SendLinkStateRequest(pWiMAX);
			goto returnCode;
		case SUBTYPE_NSP_NETWORK_ENTRY_REQ:
			iRetval=BeceemWiMAX_SendNSPLinkupRequest(pWiMAX,pMsg);
			goto returnCode;
		case SUBTYPE_TIME_ELAPSED_SINCE_NETWORK_ENTRY:
			iRetval=BeceemWiMAX_GetTimeSinceNetEntry(pWiMAX,pMsg);	
			goto returnCode;		
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}
UINT32 BeceemWiMAX_SendNSPLinkupRequest(PVOID pArg,PWIMAX_MESSAGE pMsg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PDRIVERPACKET pDpkt = NULL;
	LINK_STATE stLinkState = {0};
	int iRetval=0,iRet = 0;
	PNSP_BASED_NETENTRY_ST pNSP_NETENTRY_REQ = NULL;
	UINT32 u32VendorSpecificLength = 0;
	
	
	if(pWiMAX->m_bMACAddressWriteInProgress)
			return (UINT32)ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS;

	if(pWiMAX->m_bNetEntryInProgress)
		return (UINT32)ERROR_NET_ENTRY_INPROGRESS;

	BeceemWiMAX_GetLinkState(pWiMAX, &stLinkState);

	if(stLinkState.ucLinkStatus == LINKUP_DONE)
	{
		return SUCCESS;
	}
	BECEEMCALLOC_RET(pDpkt,PDRIVERPACKET,DRIVERPACKET,1,UINT32)
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);

	DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_PAYLOAD);
	DriverPacket_AppendChar(pDpkt, LINK_UP_REQ_NSP_NET_ENTRY_PAYLOAD);

	pNSP_NETENTRY_REQ = (PNSP_BASED_NETENTRY_ST)pMsg->szData; 


	pNSP_NETENTRY_REQ->u32NspId = htonl(pNSP_NETENTRY_REQ->u32NspId);
	pNSP_NETENTRY_REQ->u32NspIdValid= htonl(pNSP_NETENTRY_REQ->u32NspIdValid);
	u32VendorSpecificLength = pNSP_NETENTRY_REQ->stVendorSpecificParam.u32VendorSpecificInformationLength;
	
	pNSP_NETENTRY_REQ->stVendorSpecificParam.u32VendorSpecificInformationLength = htonl(pNSP_NETENTRY_REQ->stVendorSpecificParam.u32VendorSpecificInformationLength);

	DriverPacket_AppendData(pDpkt, (UCHAR *)pNSP_NETENTRY_REQ, sizeof(NSP_BASED_NETENTRY_ST)- sizeof(VENDOR_SPECIFIC_LINKPARAMS)+ sizeof(UINT32));

	/* copy the data */
	if(u32VendorSpecificLength> 0 && u32VendorSpecificLength < MAX_BUFFER_LENGTH)
		DriverPacket_AppendData(pDpkt,(UCHAR*) &pNSP_NETENTRY_REQ->stVendorSpecificParam, u32VendorSpecificLength);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
	   "Sending LINK_UP_REQ_PAYLOAD-LINK_UP_REQ_NSP_NET_ENTRY_PAYLOAD.");

	iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}
	pWiMAX->m_bNetEntryInProgress = TRUE;
	pWiMAX->m_bKillNetEntryThread = 0;
	Event_Reset(&(pWiMAX->m_objNetEntryEvent));
	Event_Reset(&(pWiMAX->m_objNetEntryEventMsgRecvd));

	if(Thread_Run(&(pWiMAX->m_NetEntryThread),(PTHREADFUNC)BeceemWiMAX_NetEntryThreadFunc,pWiMAX))
	{
 	    pWiMAX->m_bNetEntryInProgress = FALSE;
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching NetEntry thread failed !!!");
		iRetval=ERROR_THREAD_NOT_STARTED;
		goto returnCode;
	}
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "NetEntry thread started");

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return (UINT32)iRetval;

}
BOOL BeceemWiMAX_IsSyncAcheived(PVOID pArg)
{
	LINK_STATE stLinkState = {0};
	int iRet = 0;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return FALSE;

	if (pWiMAX->m_bModemInShutdown)
		return FALSE;

	iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);

	if(iRet)
		return FALSE;

	if(stLinkState.bIdleMode)
	{
		return FALSE;
	}
	else
	{
		switch(stLinkState.ucLinkStatus)
		{
		case WAIT_FOR_SYNC:
		case DREG_RECIEVED:
			return FALSE;

		case LINKUP_DONE:
		case LINKUP_IN_PROGRESS:
		case PHY_SYNC_ACHIVED:
			return TRUE;
		}
	}
	return FALSE;
}


/**
@ingroup network_entry_exit
*	Function		:	This will query for the link status and call-back with Link status response
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PLINK_STATE pLinkState - pointer to LINK_STATE stucture

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_GetLinkState(PVOID pArg, PLINK_STATE pLinkState)
{
	DWORD dwNumBytesReturned = 0;
	int iRetval=0;
	PDRIVERPACKET pDpkt = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;

	if(!pWiMAX || !pLinkState)
		return ERROR_INVALID_ARGUMENT;
	
	memset(pLinkState, 0, sizeof(LINK_STATE));
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, LINK_STATUS_REQ_PAYLOAD);

	iRetval = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_CURRENT_STATUS,
							(PVOID)DriverPacket_GetLeaderData(pDpkt),
							sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pDpkt),
							(PVOID)pLinkState,
							sizeof(LINK_STATE),
							&dwNumBytesReturned
						);
	pWiMAX->m_bIdleMode = pLinkState->bIdleMode;
	pWiMAX->m_bModemInShutdown= pLinkState->bShutdownMode;
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
}



/**
@ingroup network_entry_exit
*	Function		:	This will query for the link status and call-back with Link status response
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PLINK_STATE pLinkState - pointer to LINK_STATE stucture

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendLinkStateRequest(PVOID pArg)
{
	LINK_STATE stLinkState = {0};
	int iRet = 0;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);

	if(iRet)
		return iRet;

	if(stLinkState.bIdleMode)
		return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, IDLE_MODE_CODE, NULL);
	else if(stLinkState.bShutdownMode)
		return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, SHUTDOWN_RECEIVED_CODE, NULL);
	else
	{
		switch(stLinkState.ucLinkStatus)
		{
		case WAIT_FOR_SYNC:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_PHY_SYNC_CMD_CODE, NULL);
		case DREG_RECIEVED:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, DREG_RECEIVED_CODE, NULL);
		case PHY_SYNC_ACHIVED:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_NET_ENTRY_CMD_CODE, NULL);
		case LINKUP_IN_PROGRESS:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, NET_ENTRY_PROGRESS_CODE, NULL);
		case LINKUP_DONE:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_UP, LINKUP_ACHIEVED, NULL);
		default:
			return BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_PHY_SYNC_CMD_CODE, NULL);
		}
	}
	
}
/*
*	Function		:	BeceemWiMAX_SendShutdownModeStatusRequest
*	Parameters		:
*						PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*
*	Description		:	This will query for the shutdown mode status and send response to call-back
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendShutdownModeStatusRequest(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT uiShutdownModeStatus = 0;
	LINK_STATE stLinkState = {0};
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
		
	BeceemWiMAX_GetLinkState(pArg, &stLinkState);
	
	/*Update uiShutdownModeStatus variable with shutdown mode status value*/
	uiShutdownModeStatus = !stLinkState.bShutdownMode;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_SHUTDOWN, 
					SUBTYPE_SHUTDOWN_MODE_STATUS_RESP, 
					(UCHAR *) &uiShutdownModeStatus,
					sizeof(UINT)
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
/**
@ingroup network_entry_exit
*	Function		:	This is a thread function used to check the network entry status. If timed out It will send
					WAIT_NET_ENTRY_CMD_CODE to the application.
*	@param	PVOID pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
PVOID BeceemWiMAX_NetEntryThreadFunc(PVOID pParam)

{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	for(i=0; i<TIMEOUT_FOR_NET_ENTRY;i+=200)
	{
		if(pWiMAX->m_bKillNetEntryThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objNetEntryEvent),200)!= WAIT_TIMEOUT)
			{
			break;
			}
	}

	pWiMAX->m_bNetEntryInProgress = FALSE;
	if(i>=TIMEOUT_FOR_NET_ENTRY)
		BeceemWiMAX_SendAppLinkStatus(pWiMAX, LINK_DOWN, WAIT_NET_ENTRY_CMD_CODE, NULL);

	Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));
	return SUCCESS;
}
/**
@ingroup network_entry_exit
*	Function		:	This is a thread function used to check the Sync up status. If timed out It will send
					PHY_SYNC_ERROR to the application.
*	@param	PVOID pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
PVOID BeceemWiMAX_SyncUpThreadFunc(PVOID pParam)
{
	int i = 0;
	ST_SYNC_STATUS stSyncStatus = {0};
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;

    SET_THREAD_NAME();
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;
	
	for(i=0; i<TIMEOUT_FOR_SYNC_UP;i+=100)
	{
		if(pWiMAX->m_bKillSyncUpThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objSyncUpEvent),100)!= WAIT_TIMEOUT)
				break;
	}

	if(i>=TIMEOUT_FOR_SYNC_UP)
	{
		stSyncStatus.u32SyncStatus = PHY_SYNC_ERROR;
		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
		WiMAXMessage_Init(
					pWMMsg,
			    	TYPE_NETWORK_ENTRY, 
					SUBTYPE_SYNC_STATUS_RESP, 
					(UCHAR *)&stSyncStatus,
					sizeof(ST_SYNC_STATUS)
					);
	
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
	}
	
	pWiMAX->m_bSyncUpInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
	return SUCCESS;
}

VOID BeceemWiMAX_KillCommonAPI_NwSearchThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillNetSearchThread--------");
	pWiMAX->m_bKillWimaxCommonAPI_SearhThread = TRUE;

    Event_Set(&(pWiMAX->m_WiMAXCommonAPI_StartCaplSearchRequest));
     Event_Set(&(pWiMAX->m_WiMAXCommonAPI_ModemSearchComplete));

	#ifndef WIN32
		usleep(300);
	#else
		Sleep(3);
	#endif

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillNetSearchThread---------");

	return;
}

/*
*	Function		:	BeceemWiMAX_KillCallbackThread
*	Parameters		:	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	Description		:	This will stop net entry thread.
*	Return value	:	void.
*/
VOID BeceemWiMAX_KillNetEntryThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillNetEntryThread--------");
	Event_Set(&(pWiMAX->m_objNetEntryEvent));
	pWiMAX->m_bKillNetEntryThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillNetEntryThread---------");

	return;
}

VOID BeceemWiMAX_KillSyncUpThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillSyncUpThread--------");
	Event_Set(&(pWiMAX->m_objSyncUpEvent));
	pWiMAX->m_bKillSyncUpThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillSyncUpThread---------");

	return;
}

/**
@ingroup network_entry_exit
*	Function		:	This function will handle TYPE_LINK_DOWN. It will send Driver with payload LINK_DOWN_REQ_PAYLOAD or
					LINK_DOWN_DEREG_PAYLOAD
*	@param	PVOID pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendLinkDownRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRetval=0, iRet = 0;
	PDRIVERPACKET pDpkt = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	if(pWiMAX->m_bSyncUpInProgress)
	{
		pWiMAX->m_bSyncUpInProgress = FALSE;
		if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
		{
			if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
            {
                //so we can recover if we terminate..
                Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
            }
		}
	}

	switch(pMsg->usSubType)
	{
	case SUBTYPE_SYNC_DOWN_REQ:
		DriverPacket_AppendChar(pDpkt, LINK_DOWN_REQ_PAYLOAD);
		DriverPacket_AppendChar(pDpkt, SYNC_DOWN_REQ);
		iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
		goto returnCode;

     case SUBTYPE_SYNC_DEREG_REQ:
		DriverPacket_AppendChar(pDpkt, LINK_DOWN_DEREG_PAYLOAD);
		DriverPacket_AppendChar(pDpkt, DEREG_REQ);
		iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
		goto returnCode;
     case SUBTYPE_SYNC_DOWN_NO_DREG_REQ:
        DriverPacket_AppendChar(pDpkt, LINK_DOWN_REQ_PAYLOAD);
		DriverPacket_AppendChar(pDpkt, SYNC_DOWN_NO_DREG_REQ);
		iRetval = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
		goto returnCode;
	case SUBTYPE_SYNC_DOWN_AUTONOMOUS_SCAN_REQ:
       	DriverPacket_AppendChar(pDpkt, LINK_DOWN_REQ_PAYLOAD);
       	DriverPacket_AppendChar(pDpkt, SYNC_DOWN_AUTONOMOUS_SCAN);
		iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
		goto returnCode;

        
        
	 default:
		break;
	}
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;


}

/**
@ingroup Shutdown_Options
*	Function		:	This function will handle TYPE_SHUTDOWN. 
						It will send Driver with payload SHUTDOWN_REQ_PAYLOAD
*	@param	PVOID pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendShutDownORHibernateRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0,iRetval=0;
	PDRIVERPACKET pDpkt = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

    if(pMsg->usSubType==SUBTYPE_WAKE_FROM_SHUTDOWN)
        {
        int iTemp = 1;

        DriverPacket_InitWithVal(pDpkt, 0, STATUS_REQ);
        DriverPacket_AppendData(pDpkt, (UCHAR*)&iTemp, sizeof(SINT32));
        DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_BCM_WAKE_UP_DEVICE_FROM_IDLE, pDpkt);
		goto returnCode;
        }


    
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);

	switch(pMsg->usSubType)
	{
    case SUBTYPE_SHUTDOWN_REQ: //handles  SUBTYPE_HIBERNATE_REQ as well
        {
            pWiMAX->m_bNSPSearchInProgress = FALSE;
            //kill this if in progress
            if (pWiMAX->m_bNetSearchInProgress)
            {
                pWiMAX->m_bNetSearchInProgress = FALSE;
                /* stop n/w search thread if running */
                if(Thread_Stop(&(pWiMAX->m_NetSearchThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetSearchThread, pWiMAX) != SUCCESS)
                {
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
					 "Failed to stop NetSearch thread!!!");
                    if(Thread_Terminate(&(pWiMAX->m_NetSearchThread)) == SUCCESS)
                    {
                        //so we can recover if we terminate..
                        Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd));
                    }
                }
            }
            //kill this if in progress
            if (pWiMAX->m_bSyncUpInProgress)
            {
                pWiMAX->m_bSyncUpInProgress = FALSE;
                if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
                {
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to stop SyncUp thread!!!");
                    if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
                    {
                        //so we can recover if we terminate..
                        Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
                    }
                }
            }

          
			//check shutdown mode.
           

            DriverPacket_AppendChar(pDpkt, INITATE_SHUTDOWN_PAYLOAD);

			if(pMsg->usType==TYPE_HIBERNATE)
			{
				DriverPacket_AppendChar(pDpkt,SUBTYPE_HIBERNATION_ENTRY_REQUEST);
			}
			else
			{
				 if(pWiMAX->m_bModemInShutdown)
				{
					iRetval=ERROR_MODEM_IN_SHUT_DOWN_MODE;
					goto returnCode;
				}
				

			}

            pWiMAX->m_bKillShutDownRespThread = 0;
            Event_Reset(&(pWiMAX->m_objShutDownRespEvent));

            //send shutdown packet.
            iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);

            if (iRet < 0)
            {
                pWiMAX->m_bKillShutDownRespThread = 0;
                Event_Set(&(pWiMAX->m_objShutDownRespEvent));

				if(pMsg->usType==TYPE_HIBERNATE)
					DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending HIBERNATE Req Message failed !!");
				else
					DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Sending SHUTDOWN Req Message failed !!");
				iRetval=iRet;
				goto returnCode;
            }
            else if(pMsg->usType!=TYPE_HIBERNATE)
			{
                if(Thread_Run(&(pWiMAX->m_ShutDownRespThread),(PTHREADFUNC)BeceemWiMAX_ShutDownRespThread,pWiMAX))
                {
                    Event_Set(&(pWiMAX->m_objShutDownRespEvent));
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
					 "Launching Shutdown Response THREAD failed !!!");
                    iRetval=ERROR_THREAD_NOT_STARTED;
                	goto returnCode;
				}
            }
        }
		break;

	case SUBTYPE_SHUTDOWN_MODE_STATUS_REQ:
		iRetval=BeceemWiMAX_SendShutdownModeStatusRequest(pWiMAX);
		goto returnCode;
	default:
		break;
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pDpkt)
	return iRetval;

}

BOOL BeceemWiMAX_GetDLLVersion(UINT *puiVersion)
{
	/*
	Change this each time you change the version
	Example uiByte1.uiByte2.uiByte3.uiByte4 =1.0.0.9
	*/

	int iRet = 0;
	char szVersion[100];
	UINT uiByte1,uiByte2,uiByte3;
	
	memset(szVersion, 0 , 100);
	memcpy(szVersion,DLL_VERSION, strlen(DLL_VERSION));

	iRet = sscanf(szVersion, "%u.%u.%u", &uiByte1, &uiByte2, &uiByte3);
		
	*puiVersion++ = uiByte1;
	*puiVersion++ = uiByte2;
	*puiVersion++ = uiByte3;

	return TRUE;
}

int CheckForCalibrationStatus(PVOID pArg,UINT uiChipID)
{
	UINT uiEEPROMChkSumInfo = 0, uiEEPROMChkValidInfo = 0;
	UINT uiCalibFlag = 0, uiAntennaVal = 0;
	INT iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
	BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_INT_ADDR, &uiEEPROMChkSumInfo, 4);
	if (uiChipID < MS130)
	{
		BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_VALID_ADDR, &uiEEPROMChkValidInfo, 4);
		if (uiEEPROMChkValidInfo == 0x0ADD0ADD && ((uiEEPROMChkSumInfo & 0xC) == 0xC))
		{
			/* read from PHY reg 0x1f60f214 */
			BeceemWiMAX_rdm(pWiMAX, ANTENNA_VALUE, &uiAntennaVal, sizeof(UINT32));
			if ((uiAntennaVal & 0xFFFF)!= 0)
			{
				return CARD_CALIBRATED;
			}
			else
			{
				return CALIBRATION_STATUS_NOT_KNOWN;
			}
		}
		else
		{
			return NOT_CALIBRATED;
		}
	}

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&uiCalibFlag, sizeof(UINT32),CFG_CAL_CALIBRATION_FLAG_ID );
	
	
	if(iRet == CFG_ERR_PARAMID_NOT_SUPPORTED )
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "\nCFG_CAL_CALIBRATION_FLAG_ID not supported\n");
		uiCalibFlag = GetCalibInfoFromCalibBaseAddress(pArg);
	}
	else if(CFG_ERR_INVALID_ACCESS == iRet)
	{
			return CALIBRATION_STATUS_NOT_KNOWN;
	}

	
	if(pWiMAX->m_bBigEndianMachine)
	{
		uiCalibFlag = Swap_Int(uiCalibFlag);
	
	}
	uiCalibFlag &= 0x000000FF;
	
	/* Checksum validity is done for Compatibility, RF and Calibration bits */
	if (EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,2) && (EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,3)) 
		&& (EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,5)))
	{
		/* calib flag bit 0 validation is done here */
		if (EEPROM_CHK_VAILIDITY(uiCalibFlag,0))
		{
			return uiCalibFlag;
		}
		else
		{
			return NOT_CALIBRATED;
		}
	}
	else
	{
		return NOT_CALIBRATED;
	}
}

int GetCalibInfoFromCalibBaseAddress(PVOID pArg)
{
	UINT uiCalibFlagAddr = 0;
	UINT uiCalibAddr = 0, uiCalibFlag = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;

	BeceemWiMAX_rdm(pWiMAX, EEPROM_CALIB_BASE_ADDR, &uiCalibAddr, 4);

	uiCalibAddr &= 0x0000FFFF;
	uiCalibFlagAddr = MAC_ADDRESS_LOCATION + uiCalibAddr;

	BeceemWiMAX_rdm(pWiMAX, uiCalibFlagAddr, &uiCalibFlag, 4);

	return uiCalibFlag;
}

int GetEEPROMDetailedInformation(PBECEEMWIMAX pWiMAX, PST_SS_EEPROM_DETAILED_INFO pstEEPROMDetailedInfo)
{
	INT iRet = 0;
	unsigned char ucVersion = 0;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_RF_VERSION_ID);
	if(iRet<0)
		return iRet;
	pstEEPROMDetailedInfo->u32EEPROM_RFSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_PHY_VERSION_ID);
	if(iRet<0)
		return iRet;
	pstEEPROMDetailedInfo->u32EEPROM_PHYSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_MAC_VERSION_ID);
	if(iRet<0)
		return iRet;	
	pstEEPROMDetailedInfo->u32EEPROM_MACSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_CAL_VERSION_ID);	
	if(iRet<0)
		return iRet;	
	pstEEPROMDetailedInfo->u32EEPROM_CALSectionVersion = (UINT)ucVersion;
	

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_HW_VERSION_ID);
	if(iRet<0)
		return iRet;	

	pstEEPROMDetailedInfo->u32EEPROM_HWSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_MIB_VERSION_ID);
	if(iRet<0)
		return iRet;
	pstEEPROMDetailedInfo->u32EEPROM_MIBSectionVersion = (UINT)ucVersion;	

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_SECU_VERSION_ID);
	if(iRet<0)
		return iRet;
	pstEEPROMDetailedInfo->u32EEPROM_SecuritySectionVersion = (UINT)ucVersion;	

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_OMA_VERSION_ID);
	if(iRet<0)
		return iRet;	
	pstEEPROMDetailedInfo->u32EEPROM_OMADMSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_MRU_VERSION_ID);
	if(iRet<0)
		return iRet;	
	pstEEPROMDetailedInfo->u32EEPROM_MRUSectionVersion = (UINT)ucVersion;

	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)&ucVersion, sizeof(UINT8), CFG_VSA_VERSION_ID);
	if(iRet<0)
		return iRet;
	pstEEPROMDetailedInfo->u32EEPROM_VSASectionVersion = (UINT)ucVersion;
	
	return SUCCESS;


}


/**
@ingroup chipset_calib_conf
*	Function		:	This will do series of RDMs to get the SS info and send it to applicaiton.
					
*	@param	PVOID pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendAppSSInforesponse(PVOID pArg, PWIMAX_MESSAGE pMsg)
{
	SUBSCRIBER_STATION_INFO stSSInfo;
	ST_SS_EEPROM_DETAILED_INFO stEEPROMDetailedInfo = {0};
	DWORD dwNumBytesReturned = 0;
	int iRet = 0,iRetval=0;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT uiEEPROMChkSumInfo = 0;
	UINT uiEEPROMSize = 0;
	UINT uiEEPROMTotalInfo = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	memset(&stSSInfo,0,sizeof(SUBSCRIBER_STATION_INFO));

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	switch (pMsg->usSubType)
	{
	case SUBTYPE_SS_INFO_REQ:
		{
			memset(&stSSInfo,0,SIZE_SUBSCRIBER_STATION_INFO);
			/*	DLL Version */
			BeceemWiMAX_GetDLLVersion(stSSInfo.u32LibraryVersion);

			/* Driver Version */	
			iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_DRIVER_VERSION,
							NULL,
							0,
							(PVOID)stSSInfo.uchDriverVersion,
							sizeof(stSSInfo.uchDriverVersion),
							&dwNumBytesReturned
						);
			if(iRet)
			{
				memset(stSSInfo.uchDriverVersion, 0 , sizeof(stSSInfo.uchDriverVersion));
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to get driver version!");
			}

			/*RF Chip version */
			BeceemWiMAX_rdmBulk(pWiMAX, RF_CHIP_VERSION, (UCHAR *)&stSSInfo.RFChipVersion, 4);

			/*Baseband Chip version */

			BeceemWiMAX_rdmBulk(pWiMAX, BASEBAND_CHIP_VER_ADDR, (UCHAR *)&stSSInfo.BasebandChipVersion, 4);
			
			/*MAC Address */
			GetMACAddress(pWiMAX, stSSInfo.u8MacAddress);

			/* Firmware Version */
			BeceemWiMAX_rdmBulk(pWiMAX, FIRMWARE_VER_INT_ADDR, (UCHAR *)&stSSInfo.u32FirmwareVersion[0], 4);
			BeceemWiMAX_rdmBulk(pWiMAX, FIRMWARE_VER_FLOAT1_ADDR, (UCHAR *)&stSSInfo.u32FirmwareVersion[1], 4);
			BeceemWiMAX_rdmBulk(pWiMAX, FIRMWARE_VER_FLOAT2_ADDR, (UCHAR *)&stSSInfo.u32FirmwareVersion[2], 4);

			/* Calibration status */
			stSSInfo.u8Calibrated = NOT_CALIBRATED;

			/* EEPROM Major & Minor Version */
			
            CFG_ReadVersion(pWiMAX->pEEPROMContextTable,stSSInfo.u32EEPROMVersion);

			if (stSSInfo.u32EEPROMVersion[0])
				stSSInfo.u8Calibrated = (UINT8)CheckForCalibrationStatus(pWiMAX,stSSInfo.BasebandChipVersion);
			else
				stSSInfo.u8Calibrated = NOT_CALIBRATED;

			WiMAXMessage_Init(	
							pWMMsg,
							TYPE_SS_INFO, 
							SUBTYPE_SS_INFO_INDICATION, 
							(UCHAR *)&stSSInfo,
							SIZE_SUBSCRIBER_STATION_INFO
							);
		}
		break;

	case SUBTYPE_SS_EEPROM_INFO_REQ:
		{
			#ifndef NVM_NOT_SUPPORTED
				if(BeceemWiMAX_GetNVMSize(&uiEEPROMSize))
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to get NVM size !");
				}
			#else
				BeceemWiMAX_rdm(pWiMAX, EEPROM_SIZE_INT_ADDR, &uiEEPROMSize, 4);
				uiEEPROMSize >>= 16;
			#endif

			BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_INT_ADDR, &uiEEPROMChkSumInfo, 4);

			uiEEPROMSize /= 0x400;

			uiEEPROMTotalInfo = uiEEPROMSize;
			uiEEPROMTotalInfo <<= 24; 

			uiEEPROMTotalInfo |= uiEEPROMChkSumInfo;
			WiMAXMessage_Init(	
							pWMMsg,
							TYPE_SS_INFO, 
							SUBTYPE_SS_EEPROM_INFO_RESP, 
							(UCHAR *)&uiEEPROMTotalInfo,
							sizeof(UINT)
							);

		}
		break;

	case SUBTYPE_SS_EEPROM_DETAILED_INFO_REQ:
		{

			/* EEPROM detailed Infomation */

			iRet = GetEEPROMDetailedInformation(pWiMAX,&stEEPROMDetailedInfo);

			if(iRet < 0)
			{
				iRetval=iRet;
				goto returnCode;
			}


			WiMAXMessage_Init(	
							pWMMsg,
							TYPE_SS_INFO, 
							SUBTYPE_SS_EEPROM_DETAILED_INFO_RESP, 
							(UCHAR *)&stEEPROMDetailedInfo,
							sizeof(stEEPROMDetailedInfo)
							);

		}
		break;
	}
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
}
/**
@ingroup chipset_calib_conf
*	Function		:	This will handle SUBTYPE_RESET_REQ,SUBTYPE_FIRMWARE_DNLD_REQ and SUBTYPE_START_FIRMWARE_REQ.
					
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is pointer to WIMAX_MESSAGE
*	@param	UINT uiLength - Length of pMsg buffer.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendChipsetControlMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0,iRetries = 0;
	UINT32 u32BasebandmodemStatus = 0;
	PE_BASEBAND_MODEM_STATUS pBaseBandModemStatus = NULL;
	LINK_STATE stLink_status = {0};

	UINT32 u32ChipVersion = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;


	BeceemWiMAX_rdm(pWiMAX, BASEBAND_CHIP_VER_ADDR, &u32ChipVersion, sizeof(UINT32));

	switch(pWiMAX->stDriverInfo.u32InterfaceType)
	{
		case BCM_USB:
		case BCM_SDIO:
			break;
		
		default:
			if((u32ChipVersion&0xFFFFFF00) == SYMPHONY_CHIP_MASK && 
				pMsg->usSubType != SUBTYPE_BASEBAND_MODEM_STATUS_REQ &&
				pMsg->usSubType != SUBTYPE_CONFIGFILE_RE_DNLD_REQ)
				return ERROR_API_NOT_SUPPORTED;
	}


    iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
    if(iRet)
        return iRet;

	switch(pMsg->usSubType)
	{
		case SUBTYPE_RESET_REQ :
			return BeceemWiMAX_ChipReset(pWiMAX);
			break;
		case SUBTYPE_RESET_STATUS_REQ:
			return BeceemWiMAX_ChipResetStatus(pWiMAX);
			break;
		case SUBTYPE_CONFIGFILE_RE_DNLD_REQ:
            return BeceemWiMAX_ConfigFileReDownload(pWiMAX,pMsg->szData);
		case SUBTYPE_FIRMWARE_DNLD_REQ :
			{
				/* Copy the firmware details */
				memcpy(&(pWiMAX->m_FirmwareInfo), pMsg->szData, sizeof(ST_FIRMWARE_DNLD));
				/*
				 Launch a thread function to send packets to perform formware download
				*/
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Launching firmware download thread.");
				pWiMAX->m_bFirmwareDownloadDone = FALSE;

				if(Thread_Run(&(pWiMAX->m_FirmwareThread),(PTHREADFUNC)BeceemWiMAX_FirmwareDownloadThreadFunc,pWiMAX))
				{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching Firmware thread failed !!!");
					return ERROR_THREAD_NOT_STARTED;
				}
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Firmware thread started");
			}
			break;
		case SUBTYPE_BASEBAND_MODEM_STATUS_REQ:

			pBaseBandModemStatus = 	(PE_BASEBAND_MODEM_STATUS)pMsg->szData;

            /* Initializing modem status to active */
            *pBaseBandModemStatus = MODE_ACTIVE;
			
			if(pWiMAX->m_bFirmwareDownloadInProgress)
				{
					*pBaseBandModemStatus = MODEM_FIRMWARE_DOWNLOAD_IN_PROGRESS;
					goto end_return_success;
				}
			
			iRet = BeceemWiMAX_GetLinkState(pWiMAX,&stLink_status);

			if(iRet)
				return iRet;

			if(stLink_status.bIdleMode)
				{
					*pBaseBandModemStatus = MODEM_IN_IDLE_MODE;
					goto end_return_success;
				}
			
			if(stLink_status.bShutdownMode)
				{
					*pBaseBandModemStatus = MODEM_IN_SHUTDOWN_MODE;
					goto end_return_success;
				}
			
retry_path_if_hung:

			iRet = BeceemWiMAX_rdm(pWiMAX, FIRMWARE_STATUS_CHECK_LOCATION, &u32BasebandmodemStatus, sizeof(UINT32));

			if(iRet)
				return iRet;

			
			if(u32BasebandmodemStatus != FIRMWARE_HEALTH_SIGNATURE)
				{
#ifdef WIN32
				    Sleep(10);
#else
					usleep(10* 1000);
#endif
                    iRetries++;
					 
                    if(iRetries < 4)
                        goto retry_path_if_hung;
                    else
					{
						*pBaseBandModemStatus = MODEM_CRASHED_HANG;	
#ifdef WIN32	
						OutputDebugString("modem crashed after 3 retries");
#endif
					}
				}
			
			break;
	}
end_return_success:
	return SUCCESS;
}
/**
@ingroup chipset_calib_conf
*	Function		:	This will send IOCTL_CHIP_RESET to driver and notifies the application of chip reset status.
					
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ChipReset(PVOID pArg)
{
	int iRet = SUCCESS;
	DWORD dwTemp = 0;
	PWIMAXMESSAGE pWMMsg = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending IOCTL_CHIP_RESET.");
	//ReloadDevice("Beceem");
	//BeceemWiMAX_Cleanup(pWiMAX);
	/*
	 Send IOCTL_LINK_DOWN message
	*/
	pWiMAX->m_bFirmwareDownloadInProgress = TRUE;
	if(DeviceIO_SendOnly(&(pWiMAX->m_Device),IOCTL_CHIP_RESET,(PVOID)&dwTemp,4))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "SendToDriver failed for IOCTL_CHIP_RESET. RESET aborted!!!");
		pWiMAX->m_bFirmwareDownloadInProgress = FALSE;
		iRet = ERROR_RESET_FAILED;
		return iRet;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_CHIPSET_CONTROL, 
					SUBTYPE_RESET_INDICATION, 
					NULL,
					0
					);
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return iRet;
}


/**
@ingroup chipset_calib_conf
*	Function		:	This will send IOCTL_CHIP_RESET to driver and notifies the application of chip reset status.
					
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ConfigFileReDownload(PVOID pArg,PUCHAR pPayload)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;	
	CONFIGFILE_INFO stConfigfileDownload= {0};
	PST_CONFIGFILE_DNLD pstConfigfile=(PST_CONFIGFILE_DNLD)pPayload;
	
	INT iRet = 0;
	UINT uoldsize=0;
	FILE *fp =NULL;
	struct stat file_stat;
	size_t rw_count=0;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	

	fp=fopen((char*)pstConfigfile->szImageName,"r");
	if(!fp)
	{
			DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "File could not be opened for %s\n",pstConfigfile->szImageName);
			iRet=ERROR_SYSCONFIG_OPEN_FAILED;
			goto returnCode;
	}else
	{
		stat((char*)pstConfigfile->szImageName, &file_stat);
		stConfigfileDownload.u32Length = file_stat.st_size;
		
		ALLOCATE_MEMORY(stConfigfileDownload.puCofigBuffer,UCHAR,stConfigfileDownload.u32Length,uoldsize);
			
		if(!stConfigfileDownload.puCofigBuffer)
		{
			fclose(fp);
			return ERROR_MALLOC_FAILED;

		}
		rw_count = fread(stConfigfileDownload.puCofigBuffer, stConfigfileDownload.u32Length, 1, fp);
		fclose(fp);	
	}


	if(DeviceIO_SendOnly(&(pWiMAX->m_Device), IOCTL_BCM_CONFIG_FILE_DOWNLOAD,
		(void *)stConfigfileDownload.puCofigBuffer,stConfigfileDownload.u32Length))
	{
		DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "SendToDriver failed for IOCTL_BCM_CONFIG_FILE_DOWNLOAD !!!");
		iRet=ERROR_DEVICEIOCONTROL_FAILED;
		goto returnCode;

	}

	iRet=SUCCESS;
	
returnCode:	
	FREE_MEMORY(stConfigfileDownload.puCofigBuffer);
   
return iRet;
}

/**
@ingroup chipset_calib_conf
*	Function		:	This will send IOCTL_CHIP_RESET to driver and notifies the application of chip reset status.
					
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ChipResetStatus(PVOID pArg)
{
	int iRet = SUCCESS;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT uiRetVal = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;


	BeceemWiMax_GetDriverInfoSettings(pWiMAX);	

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
		if(pWiMAX->u32DeviceStatus==1)
			uiRetVal=1;
		else uiRetVal=0;


	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_CHIPSET_CONTROL, 
					SUBTYPE_RESET_STATUS_RESP, 
					(UCHAR *)&uiRetVal,
					sizeof(UINT32)
					);
	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return iRet;
}

/**
@ingroup chipset_calib_conf
*	Function		:	This will notify the application of chip reset status of firmware download status.
					
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PVOID pvMappedConfigAddress - pointer to mapped config address of firmware file.
*	@param	UINT u32ConfigLength - Length of the pvMappedConfigAddress buffer.
*	@param	PVOID pvMappedFirmwareAddress - pointer to the mapped firmware file.
*	@param	UINT u32FirmwareLength - Length of the pvMappedFirmwareAddress buffer.
*	@param	int iStatus - Firmware download status.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendFirmwareDnldIndication(PVOID pArg,
                                           int iStatus)
{
	FIRMWARE_STATUS_INDICATION tFmStatus = {0};
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	tFmStatus.u32FirmwareDownloadStatus = iStatus;
	memcpy(&tFmStatus.stFirmWareInfo,&pWiMAX->m_FirmwareInfo,sizeof(ST_FIRMWARE_DNLD));

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_CHIPSET_CONTROL, 
					SUBTYPE_FIRMWARE_DNLD_STATUS_RESP, 
					(UCHAR *)&tFmStatus,
					SIZE_FIRMWARE_STATUS_INDICATION
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
/**
@ingroup chipset_calib_conf
*	Function		:	This will download the firmware to the driver 
						- It sends IOCTL_LINK_DOWN to driver to disconnect from the BS.
						- It sends IOCTL_BCM_FIRMWARE_DOWNLOAD with the firmware info to download the firmware
						- It notify the application of the status of the firmware download.

*	@param	void *pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
BOOL  OpenFileForDwnload(PVOID *pArg)
{
		PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
		
#ifdef WIN32
	DWORD dwLastError = 0;
	MEMORY_BASIC_INFORMATION stMemInfo = {0}; 
	DWORD	dwBytesRet = 0;
	
	pWiMAX->m_hFileHandle = CreateFile(
						pWiMAX->m_FirmwareInfo.szImageName,
						FILE_READ_DATA,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN,
						NULL
					);
	
	if(pWiMAX->m_hFileHandle==INVALID_HANDLE_VALUE)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "Couldnot open file MacAsic.bin. FirmwareDownload aborted!!!");
		return FALSE;
	}
	/*
	Create file mapping
	*/
	pWiMAX->m_hFileMap = CreateFileMapping
							(
								pWiMAX->m_hFileHandle,			/*  File Handle */
								NULL,			/*  Attributes */
								PAGE_READONLY,  /*  Protect */
								0,				/*  MaxSize high */
								0,				/*  4096, ( MaxSize low) */
								NULL			/*  "Mapping" (Name) */
							);
	
	if(!pWiMAX->m_hFileMap)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "CreateFileMapping failed. FirmwareDownload aborted!!!");
		dwLastError = GetLastError();
		CloseHandle(pWiMAX->m_hFileHandle);
		return FALSE;
	}

	/*
	Map view of a file into the address space
	*/
	
	pWiMAX->m_hMappedFile =MapViewOfFile(
									pWiMAX->m_hFileMap,
									FILE_MAP_READ,
									0,
									0,
									0
								);
	

	if(!pWiMAX->m_hMappedFile)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "MapViewOfFile failed. FirmwareDownload aborted!!!");
		CloseHandle(pWiMAX->m_hFileHandle );
		CloseHandle(pWiMAX->m_hFileMap);
		return FALSE;
	}

	/*
	Get Memory info for the Memory map
	*/
	dwBytesRet = (DWORD)VirtualQuery(pWiMAX->m_hMappedFile, &stMemInfo, sizeof(stMemInfo)); 
	if(!dwBytesRet)
	{
		printf("\nVirtualQuery  failed . FirmwareDownload aborted!!!");
		UnmapViewOfFile(pWiMAX->m_hMappedFile);
		CloseHandle(pWiMAX->m_hFileMap);
		CloseHandle(pWiMAX->m_hFileHandle );
		return FALSE;
	}
	pWiMAX->pFirmwareFile= pWiMAX->m_hMappedFile;
	pWiMAX->m_u32FirmwareSize = (UINT32)stMemInfo.RegionSize;
	
	
#else
	struct stat stFileStat={0};

	 pWiMAX->m_u32FileDescriptor  = open((char *)pWiMAX->m_FirmwareInfo.szImageName, O_RDONLY, S_IRUSR | S_IWUSR);

	if(pWiMAX->m_u32FileDescriptor == -1)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "File could not be opened, plz check the path or name");
		
		return FALSE;	
	}
	fstat(pWiMAX->m_u32FileDescriptor, &stFileStat);
	pWiMAX->m_u32FirmwareSize = stFileStat.st_size;
	
#endif
return TRUE;
}

BOOL CloseDownloadFile(PVOID *pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
#ifdef WIN32
		UnmapViewOfFile(pWiMAX->m_hMappedFile);
		CloseHandle(pWiMAX->m_hFileMap);
		CloseHandle(pWiMAX->m_hFileHandle );
		pWiMAX->m_hFileHandle = NULL;
		pWiMAX->m_hFileMap = NULL;
		pWiMAX->m_hMappedFile = NULL;
#else
	close(pWiMAX->m_u32FileDescriptor);
	pWiMAX->m_u32FileDescriptor = 0;
#endif

	return TRUE;
}

int BeceemWiMAX_FirmwareDownloadThreadFunc(void *pParam)
{

#ifdef WIN32
	void *lpvMappedFirmwareAddress = 0;
	UINT uiFirmwareLength = 0;
	DWORD dwTemp = 0;
	DWORD dwBytesRet = 0;
	int iTimeOutCount = 0, iMACMgmtClients = 0;
	UINT32 u32ChipVersion = 0, uiMACRegValue = 0;
#endif
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	
	Mutex_Lock(&(pWiMAX->m_FirmwareDownloadMutex));
	
#ifndef WIN32
	iRet = BeceemWiMAX_FirmwareDownload(pWiMAX);
	Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
	pWiMAX->m_bStatsPointerFlag = FALSE;
	return iRet;
#else

		
     CheckAndDisableFileSystemRedirection(&GpWiMAX->pPlaceHodlerForFSRedirectionStatus);

	if(!OpenFileForDwnload(pParam))
	{
		pWiMAX->m_bFirmwareDownloadDone=TRUE;
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX,ERROR_FILE_OPERATION);
		Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
		return ERROR_FILE_OPERATION;
	}
	
	lpvMappedFirmwareAddress = pWiMAX->pFirmwareFile;
	uiFirmwareLength = pWiMAX->m_u32FirmwareSize;

	if(!lpvMappedFirmwareAddress || uiFirmwareLength == 0)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid firmware address. Download aborted!!!");
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		CloseDownloadFile((PVOID)pWiMAX);
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
              ERROR_FIRMWAREDNLD_FAILED);
		Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
		return ERROR_FIRMWAREDNLD_FAILED;
	}


    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending IOCTL_LINK_DOWN.");
	/*
	 Send IOCTL_LINK_DOWN message
	*/
	if(DeviceIO_SendOnly(&(pWiMAX->m_Device),IOCTL_LINK_DOWN,(PVOID)&dwTemp,4))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "SendToDriver failed for IOCTL_LINK_DOWN. Firmware download aborted!!!");
		CloseDownloadFile((PVOID)pWiMAX);
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
					ERROR_FIRMWAREDNLD_FAILED);
		Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
		return ERROR_FIRMWAREDNLD_FAILED;
	}


    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending IOCTL_BCM_FIRMWARE_DOWNLOAD.");

	/*
	 Start Firmware Download
	*/
	dwBytesRet = 0;
	if(DeviceIO_SendOnly(&(pWiMAX->m_Device), IOCTL_BCM_FIRMWARE_DOWNLOAD, lpvMappedFirmwareAddress, uiFirmwareLength))

	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "SendToDriver failed for IOCTL_BCM_FIRMWARE_DOWNLOAD !!!");
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		CloseDownloadFile((PVOID)pWiMAX);
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FIRMWAREDNLD_FAILED);
		Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
		return ERROR_FIRMWAREDNLD_FAILED;
	}
	
	CloseDownloadFile((PVOID)pWiMAX);

	CheckAndRestoreFileSystemRedirection(&GpWiMAX->pPlaceHodlerForFSRedirectionStatus);


	iRet = -1;
	iTimeOutCount = 0;
	while(iRet < 0 && (iTimeOutCount < 20))
	{
		iRet = BeceemWiMAX_rdm(pWiMAX, BASEBAND_CHIP_VER_ADDR, &u32ChipVersion, sizeof(UINT32));
		Sleep(100);
		iTimeOutCount++;
		if(iTimeOutCount > 20)
		{
			pWiMAX->m_bFirmwareDownloadDone = TRUE;
			pWiMAX->m_bFirmwareDownloadInProgress = FALSE;
			
			BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FIRMWAREDNLD_FAILED);
			Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
			return SUCCESS;

		}
	}
	pWiMAX->m_bInitEEPROM = FALSE;
	BeceemWiMAX_DeviceInitThreadFunc(pWiMAX);

	/* If Clients are reading MAC Management messages, keep it enabled */
	SharedMem_ReadData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iMACMgmtClients, sizeof(int), 0);
	if(iMACMgmtClients > 0)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Number of clients reading MAC Mgmt Messages %d",
		   iMACMgmtClients);
		BeceemWiMAX_rdmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT));
		uiMACRegValue = uiMACRegValue & 0xFFFE;
		BeceemWiMAX_wrmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT));
	}


	pWiMAX->m_bFirmwareDownloadDone = TRUE;
	pWiMAX->m_bFirmwareDownloadInProgress = FALSE;
	
	BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				SUCCESS);


	Mutex_Unlock(&(pWiMAX->m_FirmwareDownloadMutex));
	pWiMAX->m_bStatsPointerFlag = FALSE;
	return SUCCESS;
#endif
}


/**
@ingroup chipset_calib_conf
*	Function		:	This will handle TYPE_STATISTICS and SUBTYPE_BASEBAND_STATISTICS_REQ.
						This will call the callback function with the statistics information.
						This will spawn a thread to periodically notify the applicatio of the statistics.

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure.
*	@param	UINT uiLength	- Lenght of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendStatisticsRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0;
	PWIMAXMESSAGE pWMMsg = NULL;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if (pWiMAX->m_bIdleMode) 
		return ERROR_MODEM_IN_IDLE_MODE;
	else if (pWiMAX->m_bModemInShutdown)
		return ERROR_MODEM_IN_SHUT_DOWN_MODE;

    iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
    if(iRet)
        return iRet;

    switch(pMsg->usSubType)
	{
		case SUBTYPE_BASEBAND_STATISTICS_REQ:
		{
			UINT uiQueryPeriod = *(UINT *)pMsg->szData;
			if(uiQueryPeriod == 0)
			{
				PUCHAR pucStatBuffer = NULL;
				int iLengthOfStat = 0;

				if(pWiMAX->m_bStatThreadRunning)
				{
					/* Stop Periodic Statstics thread */
					if(Thread_Stop(&(pWiMAX->m_StatsThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillStatsThread, pWiMAX) != SUCCESS)
					{
					  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
						 "Failed to stop Stats thread!!!");
						Thread_Terminate(&(pWiMAX->m_StatsThread)); 
					}
				  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
					 "Successfully stopped Stats thread!!!");
					break;
				}
				/* Send Statistics Indication */
				BECEEMCALLOC(pucStatBuffer,PUCHAR,CHAR,1024)
				
				iLengthOfStat = BeceemWiMAX_GetStatisticsReal(pWiMAX, pucStatBuffer, 1024);

				/* If invalid len, then fill everything with zeros and send */
				if(iLengthOfStat == ERROR_INVALID_STATS_LEN)
				{
					iLengthOfStat = SIZE_stStatistics_SS-sizeof(stBcmProprietaryStatsMIBs)-sizeof(stMIBStatistics);
					memset(pucStatBuffer, 0, 1024);
                    return ERROR_NO_STATISTIC_RESPONSE;
                    
				}
				else if(iLengthOfStat <=0)
				{
					BECEEM_MEMFREE(pucStatBuffer)
					return iLengthOfStat;
				
				}
				pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
				if(!pWMMsg)
				{	
					BECEEM_MEMFREE(pucStatBuffer)	
					return ERROR_MALLOC_FAILED;
				}
				WiMAXMessage_Init(
								pWMMsg,
								TYPE_STATISTICS, 
								SUBTYPE_BASEBAND_STATISTICS_INDICATION, 
								pucStatBuffer,
								iLengthOfStat
								);
				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			
				BECEEM_MEMFREE(pWMMsg)
				BECEEM_MEMFREE(pucStatBuffer)
			}
			else if(uiQueryPeriod > 0)
			{
				pWiMAX->m_uiStatsPeriod = uiQueryPeriod;
				if(pWiMAX->m_bStatThreadRunning)
				{
					/* Return since thread is running */
					break;
				}
				/* Start periodic statistics thread */
				if(Thread_Run(&(pWiMAX->m_StatsThread),(PTHREADFUNC)BeceemWiMAX_StatsThreadFunc,pWiMAX))
				{
				  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
					 "Launching Statistics thread failed!");
					return ERROR_THREAD_NOT_STARTED;
				}
			}
		}
		break;

		case SUBTYPE_STATISTICS_RESET_CONTROL_REQ:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		     "Processing SUBTYPE_STATISTICS_RESET_CONTROL_REQ...");
			return BeceemWiMAX_SendResetStatistics(pWiMAX, *(UINT *)pMsg->szData);
	}
   	return SUCCESS;
}
/**
@ingroup chipset_calib_conf
*	Function		:	This will send statistics structure to the application periodically.

*	@param	void *pParam - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
void *BeceemWiMAX_StatsThreadFunc(void *pParam)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	PUCHAR pucStatBuffer = NULL;	

	stStatistics_SS *pStatistics = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
	UINT32 bWhileLoop=1;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	pWiMAX->m_bStatThreadRunning = TRUE;
	BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
	pucStatBuffer = (PUCHAR)calloc(1,3048);
	if(!pucStatBuffer)
	{
		BECEEM_MEMFREE(pWMMsg)
		return (void *)ERROR_MALLOC_FAILED;
	}
	pStatistics = (stStatistics_SS *)pucStatBuffer;
	while(bWhileLoop)
	{
		int iLengthOfStat = 0;

		/* Check to kill this thread */
		if (!pWiMAX->m_bStatThreadRunning)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Stats thread.");
			break;
		}

		if ((pWiMAX->m_bIdleMode == TRUE) || (pWiMAX->m_bModemInShutdown == TRUE))
		{
#ifdef WIN32
			Sleep(500*4);
#else
			usleep(500000*4);
#endif
			continue;
		}

		iLengthOfStat = BeceemWiMAX_GetStatisticsReal(pWiMAX, pucStatBuffer, 3048);

		/* If error, make stats zero and send it to app */
		if(iLengthOfStat <= 0) 
		{
		
		//	iLengthOfStat = SIZE_stStatistics_SS-sizeof(stBcmProprietaryStatsMIBs)-sizeof(stMIBStatistics);
		//	memset(aucStatBuffer, 0, sizeof(aucStatBuffer));
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Error in thread %s to get statistics",
			   __FUNCTION__);
#ifdef WIN32
			Sleep(500*4);
#else
			usleep(500000*4);
#endif			
			continue;
		}
		/* Form response packet and send it to application */

	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		   "from host %d to host %d pkts from host %d and to host %d",
		   pStatistics->tDPStats.u32NumofBytesFromHost,
		   pStatistics->tDPStats.u32NumofBytesToHost,
		   pStatistics->tDPStats.u32NumOfSdusFromHost,
		   pStatistics->tDPStats.u32NumOfSdusToHost);
		WiMAXMessage_Init(	
			pWMMsg,
			TYPE_STATISTICS, 
			SUBTYPE_BASEBAND_STATISTICS_INDICATION, 
			pucStatBuffer,
			iLengthOfStat
			);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
#ifdef WIN32
		Sleep(pWiMAX->m_uiStatsPeriod);
#else
        usleep(pWiMAX->m_uiStatsPeriod*1000); /*convert millisecs to microsecs */
#endif
	}
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Exiting out of Stats thread.");
	BECEEM_MEMFREE(pucStatBuffer)
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

void BeceemWiMAX_KillStatsThread(void *pParam)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillStatsThread--------");
	pWiMAX->m_bStatThreadRunning = FALSE;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillStatsThread---------");
}

/* On success returns size of Statistics structure else a negative value */
/**
@ingroup chipset_calib_conf
*	Function		:	This will do rdm to get the stStatistics_SS filled.

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UCHAR *pucStatsBuff - pointer to statistics buffer structure.
*	@param	int iStatsBuffSize	- Length of the pucStatsBuff buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/

int BeceemWiMAX_GetStatisticsReal(PVOID pArg, UCHAR *pucStatsBuff, int iStatsBuffSize)
{
	int iLenStats = 0;
	stStatistics_SS *pStatistics = 0;
	UINT uiCF = 0;
	INT iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	Mutex_Lock(&(pWiMAX->m_objMutex));
	iLenStats = BeceemWiMAX_ReadStatistics(pArg);
    if(iLenStats <= 0)
    {
		Mutex_Unlock(&(pWiMAX->m_objMutex));
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to get statistics");
		return iLenStats;
    }

	pStatistics = (stStatistics_SS *)pWiMAX->m_aucStatBuffer;
	if(!pStatistics)
    {
		Mutex_Unlock(&(pWiMAX->m_objMutex));
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to send GET_STATISTIC_MESSAGE");
		return ERROR_NO_STATISTIC_RESPONSE;
	}
	if(pStatistics->u32SizeOfStatsStruct == 0)
	{
		Mutex_Unlock(&(pWiMAX->m_objMutex));
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid stats structure length.");
		return ERROR_INVALID_STATS_LEN;
	}

	/* Temporary fix for CF - Start*/
	iRet = BeceemWiMAX_rdm(pWiMAX,CENTER_FREQ_LOCATION,&uiCF, sizeof(UINT32)); 

	if (iRet < 0)
		{
			Mutex_Unlock(&(pWiMAX->m_objMutex));
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "RDM FAILED ON CENTER_FREQ_LOCATION");
			return ERROR_INVALID_STATS_LEN;
		}
	
	pStatistics->u32CenterFrequency = (uiCF*1000)/4;

	pStatistics->tLinkStats.s32CurrentRssi = (B_SINT32)((FLOAT)pStatistics->tLinkStats.s32CurrentRssi/4.0);
	pStatistics->u32SizeOfStatsStruct = iLenStats;

	BeceemWiMAX_CheckAndSwapStatistics(pArg, pStatistics);

	memset(pucStatsBuff, 0, iStatsBuffSize);
	memcpy(pucStatsBuff, pStatistics, iLenStats);

	Mutex_Unlock(&(pWiMAX->m_objMutex));
	return pStatistics->u32SizeOfStatsStruct;
}

void BeceemWiMAX_CheckAndSwapStatistics(PBECEEMWIMAX pWiMAX, stStatistics_SS *pStatistics)
{
	int i = 0;
	PUCHAR pucData = NULL;
	
	if(!pWiMAX->m_bBigEndianMachine)
		return;


	for(i=0, pucData=(PUCHAR)&pStatistics->tCPStats; i<sizeof(stCPStatistics); i+=4)
		*(UINT *)(pucData+i)= Swap_Int(*(UINT *)(pucData+i));


	for(i=0, pucData=(PUCHAR)&pStatistics->tLinkStatsEx.s8TxReferencePower; i<sizeof(stLinkStatEx)-16; i+=4)	
		*(UINT *)(pucData+i)= Swap_Int(*(UINT *)(pucData+i));


	*(UINT *)&pStatistics->tPhyStateInfo = Swap_Int(*(UINT *)&pStatistics->tPhyStateInfo);

	for(i=0, pucData=(PUCHAR)&pStatistics->tPhyStateInfo.u8DownlinkBurstDataFecRepetition; i<sizeof(UINT)*2; i+=4)	
		*(UINT *)(pucData+i)= Swap_Int(*(UINT *)(pucData+i));	

}

/**
@ingroup idle_mode
*	Function		:	This will request the driver to go to Idle mode.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure
*	@param	UINT uiLength	- Length of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendIdleModeRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0;
	UINT uiRetVal = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	LINK_STATE stLinkState = {0};
	PDRIVERPACKET pDpkt = NULL;
	PWIMAXMESSAGE pWMMsg = NULL;
	SINT32 iRetVal = SUCCESS;


	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;

	switch(pMsg->usSubType)
	{
	case SUBTYPE_IDLE_MODE_REQ:
		BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	

		DriverPacket_InitWithVal(pDpkt, 0, STATUS_REQ);

		DriverPacket_AppendChar(pDpkt, GO_TO_IDLE_MODE_PAYLOAD);

		iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
		BECEEM_MEMFREE(pDpkt)
		return iRet;
	case SUBTYPE_IDLE_MODE_STATUS_REQ:
		BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
		
		iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);
		uiRetVal = stLinkState.bIdleMode;
		WiMAXMessage_Init(	
				pWMMsg,
				TYPE_IDLE_MODE, 
				SUBTYPE_IDLE_MODE_STATUS_RESP, 
				(UCHAR *)&uiRetVal,
				sizeof(UINT32)
				);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
		break;

	case SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_REQ:
			memcpy(&uiRetVal, pMsg->szData, sizeof(UINT32));
			uiRetVal = uiRetVal * 200;
			iRetVal = BeceemWiMAX_wrm(pWiMAX, IDLE_MODE_INACITIVITY_ADDR, uiRetVal);
			if (iRetVal < 0)
				iRetVal = ERROR_IDLE_MODE_INACTIVITY_TIMER_SET_FAILED;
			BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
			WiMAXMessage_Init(	
				pWMMsg,
				TYPE_IDLE_MODE, 
				SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP, 
				(UCHAR *)&iRetVal,
				sizeof(SINT32)
				);

			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			BECEEM_MEMFREE(pWMMsg)
		break;
	
	case SUBTYPE_IDLE_MODE_WAKEUP_REQ:
		iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);

		if (stLinkState.bIdleMode)
		{
			if(Thread_Run(&(pWiMAX->m_IdleModeWakeupThread),(PTHREADFUNC)BeceemWiMAX_IdleModeWakeupThread,pWiMAX))
			{
			      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching  IDLE Mode thread failed!");
				return ERROR_THREAD_NOT_STARTED;
			}
			
		}
		else
		{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Device is not in IDLE Mode !!!");
			return ERROR_DEVICE_NOT_IN_IDLE_MODE;
		}
		break;
	}
	return SUCCESS;
}
PVOID BeceemWiMAX_IdleModeWakeupThread(PVOID pArg)
{
	int iTemp = 1;
	PDRIVERPACKET pDpkt = NULL;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT32 uiRetVal = FALSE, i = 0,uiTimeout = 60/*6 seconds timeout*/;
	LINK_STATE stLinkState = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
    SET_THREAD_NAME();
	BECEEMCALLOC_RET(pDpkt,PDRIVERPACKET,DRIVERPACKET,1,PVOID)
	DriverPacket_InitWithVal(pDpkt, 0, STATUS_REQ);
	DriverPacket_AppendData(pDpkt, (UCHAR*)&iTemp, sizeof(SINT32));
	DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_BCM_WAKE_UP_DEVICE_FROM_IDLE, pDpkt);

			
	for (i = 0; i <= uiTimeout; i++)
	{
		if (pWiMAX->m_bKillIdleModeWakeupThread == 1)
		{	
			BECEEM_MEMFREE(pDpkt)
			return SUCCESS;
		}

#ifndef WIN32
		usleep(100000);
#else
		Sleep(100);
#endif
		BeceemWiMAX_GetLinkState(pArg, &stLinkState);
		if (stLinkState.bIdleMode == FALSE)
		{
			uiRetVal = TRUE;
			break;
		}
	}

	if (i >= uiTimeout)
		uiRetVal = FALSE;

	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
	if(!pWMMsg)
	{
			BECEEM_MEMFREE(pDpkt)		
			return (PVOID)ERROR_MALLOC_FAILED;
	}
		WiMAXMessage_Init(	
			pWMMsg,
			TYPE_IDLE_MODE, 
			SUBTYPE_IDLE_MODE_WAKEUP_RESP, 
			(UCHAR *)&uiRetVal,
			sizeof(UINT32)
			);
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
		BECEEM_MEMFREE(pDpkt)
		return SUCCESS;	
}


/**
@ingroup chipset_calib_conf
*	Function		:	This is to enable or Disable the MAC management messages in the Library.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure
*	@param	UINT uiLength	- Length of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendMACMgmtMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0;
	
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;

    switch(pMsg->usSubType)
    {
        case SUBTYPE_MACMGMT_ENABLE_REQ:
			if(pWiMAX->m_bMACMgmtMsgsEnabled == 0)
			{
				ToggleMACMgmtMsg(pWiMAX, TRUE);
				pWiMAX->m_bMACMgmtMsgsEnabled = 1;
			}
            break;
        case SUBTYPE_MACMGMT_DISABLE_REQ:
			if(pWiMAX->m_bMACMgmtMsgsEnabled)
			{
				ToggleMACMgmtMsg(pWiMAX, FALSE);
				pWiMAX->m_bMACMgmtMsgsEnabled = 0;
			}
			break;
		case SUBTYPE_MACMGMT_GET_STATUS_REQ:
			iRet = GetMACMgmtMsgStatus(pWiMAX);
			BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
			WiMAXMessage_Init(	
				pWMMsg,
				TYPE_MACMGMT, 
				SUBTYPE_MACMGMT_GET_STATUS_RESP, 
				(UCHAR *)&iRet,
				sizeof(UINT32)
				);

			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
			BECEEM_MEMFREE(pWMMsg)
			break;
        default:
            break;
    }

    return SUCCESS;
}
/**
@ingroup service_flow
*	Function		:	This is to handle TYPE_SERVICE_FLOW,SUBTYPE_SF_ADD_REQ,SUBTYPE_SF_CHANGE_REQ,SUBTYPE_SF_DELETE_REQ
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure
*	@param	UINT uiLength	- Length of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendServiceFlowMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0, iswStatus = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iswStatus = SERVICE_FLOW_MSG;
	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, SERVICE_FLOW_MSG);

	switch(pMsg->usSubType) 
	{
		case SUBTYPE_SF_ADD_REQ:
			ConvertAddRequestEndianess((stLocalSFAddRequest *)pMsg->szData, 0);
			DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(stLocalSFAddRequest));
			break;

		case SUBTYPE_SF_CHANGE_REQ:
			ConvertAddIndicationEndianess((stLocalSFAddIndication *)pMsg->szData, 0);
			DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(stLocalSFChangeRequest));
			break;

		case SUBTYPE_SF_DELETE_REQ:
			{
				stLocalSFDeleteRequest *pSFDelete = (stLocalSFDeleteRequest *)pMsg->szData;
				pSFDelete->u32SFID = htonl(pSFDelete->u32SFID);
				pSFDelete->u16TID = htons(pSFDelete->u16TID);
				DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(stLocalSFDeleteRequest));
			}
			break;

		case SUBTYPE_SF_ERTPS_CHANGE_REQ:
			{
				PERTPS_GRANT_SIZE_SET_REQ peRTPSGrantSizeSetReq = (PERTPS_GRANT_SIZE_SET_REQ)pMsg->szData;
				iswStatus = SERVICE_FLOW_ERTPS_GRANT_CHANGE_REQ;
				DriverPacket_InitWithVal(pDpkt, 0, SERVICE_FLOW_ERTPS_GRANT_CHANGE_REQ);
				peRTPSGrantSizeSetReq->u8MsgType = SET_ERTPS_TX_MSG_CHANGE_REQ;
				peRTPSGrantSizeSetReq->u8Padding = 0;
				peRTPSGrantSizeSetReq->u16VCID = htons(peRTPSGrantSizeSetReq->u16VCID);
				peRTPSGrantSizeSetReq->u32BytesPerUGI = htonl(peRTPSGrantSizeSetReq->u32BytesPerUGI);
				peRTPSGrantSizeSetReq->u32SDUsPerUGI = htonl(peRTPSGrantSizeSetReq->u32SDUsPerUGI);
				DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(ERTPS_GRANT_SIZE_SET_REQ));
			}
			break;

		default:
			BECEEM_MEMFREE(pDpkt)
			return ERROR_INVALID_SF_SUBTYPE;
	}

	iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
	if (iRet < 0)
	DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
	       "FAILED SENDING MESSAGE TO THE DEVICE FOR SWSTATUS: [%d], TYPE: [%d], SUBTYPE: [%d]",
	       iswStatus, pMsg->usType, pMsg->usSubType);
	else
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
	       "SUCCESSFULLY SENT THE MESSAGE TO THE DEVICE FOR SWSTATUS: [%d], TYPE: [%d], SUBTYPE: [%d]",
	       iswStatus, pMsg->usType, pMsg->usSubType);
	BECEEM_MEMFREE(pDpkt)
	return iRet;
}

int BeceemWiMAX_SendSleepModeMsg(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, SLEEP_MODE_REQ);

	switch(pMsg->usSubType) 
	{
		case SUBTYPE_SLEEP_MODE_REQ:
			ConvertSLeepModeEndianess((ST_SLEEPMODE_MSG*)pMsg->szData, 0);
            DriverPacket_AppendChar(pDpkt, HOST_PSC_REQ);
            DriverPacket_AppendChar(pDpkt, 0);
            DriverPacket_AppendChar(pDpkt, 0);
            DriverPacket_AppendChar(pDpkt, 0);
            DriverPacket_AppendData(pDpkt, pMsg->szData, sizeof(ST_SLEEPMODE_MSG));
            
            //printf("\n REached SendSleep Mode pkt in SendMessage.c..\n"); 
			break;

		default:
			BECEEM_MEMFREE(pDpkt)
			return ERROR_INVALID_SLEEP_MODE_TYPE;
	}

	iRet= DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
	BECEEM_MEMFREE(pDpkt)
	return iRet;
}
UINT GetSystemDirectoryEx(char *lpszBuffer, UINT uiSize)
{
	UNUSED_ARGUMENT(uiSize);
#ifdef WIN32
	return GetSystemDirectory(lpszBuffer,MAX_FILENAME_LENGTH);
#else
	strcpy(lpszBuffer, LIB_PATH);
	return strlen(lpszBuffer);
#endif
}

int OpenSysConfigFile(FILE **fp, const char *szMode)
{
	char cFilename[MAX_FILENAME_LENGTH] = {0};
	int iRet = 0;
#ifdef WIN32
		CheckAndDisableFileSystemRedirection(&GpWiMAX->pPlaceHodlerForFSRedirectionStatus);
#endif 
	
	if(!GetSystemDirectoryEx(cFilename,MAX_FILENAME_LENGTH))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "GetSystemDirectory failed!");
		iRet =  ERROR_SYSCONFIG_GET_FAILED;
		goto error_return_Case;
	}

	strcat(cFilename,SYS_CONFIG_FILE_NAME);
	*fp = fopen(cFilename, szMode);

	if(!*fp)
		{
			iRet =  ERROR_SYSCONFIG_OPEN_FAILED;
	    DPRINT(DEBUG_WARNING, DEBUG_PATH_TX, "CFG File is not found [%s] \n", cFilename);
			goto error_return_Case;
		}
	
	return SUCCESS;

error_return_Case:
	#ifdef WIN32
		CheckAndRestoreFileSystemRedirection(&GpWiMAX->pPlaceHodlerForFSRedirectionStatus);
	#endif
	return iRet; 
}

int CloseSysConfigFile(FILE **fp)
{
	#ifdef WIN32
		CheckAndRestoreFileSystemRedirection(&GpWiMAX->pPlaceHodlerForFSRedirectionStatus);
	#endif
	fclose(*fp);
	return SUCCESS;
}

BOOL SaveLEDInformationToCFG(UINT *pLEDInfo)
{
	int iOffset, iOpenSysFile;
	FILE *fp = NULL;

	UINT32 uiIndex = 0;
	UINT32 uiData = 0; 
	
	iOffset = (UADDTYPE)&((SYSTEM_CONFIGURATION *)NULL)->HostDrvrConfig1;

	iOpenSysFile = OpenSysConfigFile(&fp, "r+");
	if(iOpenSysFile)
	{
		return iOpenSysFile;
	}


	if(fseek(fp, iOffset, SEEK_SET))
	{
		return ERROR_INVALID_MIB_OBJ_ID;
	}

	for(uiIndex=0; uiIndex<3; uiIndex++)
	{ 
		uiData = *pLEDInfo++;
		fwrite((void*)&uiData,1,4,fp);
	}

	CloseSysConfigFile(&fp);
	return TRUE;
}
/********************************************************/
/* 				GPIO Message							*/
/********************************************************/
/**
@ingroup chipset_calib_conf
*	Function		:	This is to handle TYPE_GPIO,SUBTYPE_GPIO_SET_REQ, SUBTYPE_GPIO_STATUS_REQ,
						SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS and SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS.
					- For SUBTYPE_GPIO_SET_REQ it does IOCTL_BCM_GPIO_SET_REQUEST to driver with GPIO_INFO
					- For SUBTYPE_GPIO_STATUS_REQ it does IOCTL_BCM_GPIO_STATUS_REQUEST to driver
                    - For SUBTYPE_GPIO_MULTI_REQ it does IOCTL_BCM_GPIO_MULTI_REQUEST to driver
                    - For SUBTYPE_GPIO_MODE_REQ it does IOCTL_BCM_GPIO_MODE_REQUEST to driver
					- Notify the application with SUBTYPE_GPIO_STATUS_INDICATION on GPIO status
					- For SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS it does IOCTL_SET_LED_BEHAVIOR_SETTING 
					  to driver with LEDStateInfo.
					- Notify the application with SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_INDICATION 
					- For SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS it does IOCTL_GET_LED_BEHAVIOR_SETTING 
					  to driver with LEDStateInfo.
					- Notify the application with SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS_INDICATION with 
					  the private LED information of the driver.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure
*	@param	UINT uiLength	- Length of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendGPIOMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet;
	int iRetval=0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAXMESSAGE pWMMsg = NULL;


	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	switch(pMsg->usSubType)
    {
    case SUBTYPE_GPIO_MULTI_REQ:
    {
	  GPIO_MULTI_INFO astGPIOMultiInfo[MAX_IDX];
      PGPIO_MULTI_INFO pstGPIOMultiInfo = NULL;
	  UINT32 u32Instance=(uiLength-4) /SIZE_GPIO_MULTI_INFO;
			
      DWORD dwNumBytesReturned = 0;
      memset(&astGPIOMultiInfo,0,sizeof(GPIO_MULTI_INFO)*MAX_IDX);

      pstGPIOMultiInfo = (PGPIO_MULTI_INFO) pMsg->szData;
      //TODO check GPIO mask validity for t3b

      iRet = DeviceIO_Send(
          &(pWiMAX->m_Device),
          IOCTL_BCM_GPIO_MULTI_REQUEST,
          pstGPIOMultiInfo,
          SIZE_GPIO_MULTI_INFO *u32Instance,
          pstGPIOMultiInfo,
          SIZE_GPIO_MULTI_INFO *u32Instance,
          &dwNumBytesReturned
          );

      if(iRet)
      {
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_MULTI_REQUEST!!");
        iRetval=iRet;
		goto returnCode;
      }

      memcpy((UCHAR*)&astGPIOMultiInfo,(UCHAR*)pstGPIOMultiInfo,SIZE_GPIO_MULTI_INFO*u32Instance);

      /* then notify the application with GPIO_STATUS_INDICATION */
      WiMAXMessage_Init(
          pWMMsg,
          TYPE_GPIO,
          SUBTYPE_GPIO_STATUS_INDICATION,
          (UCHAR*)astGPIOMultiInfo,
          SIZE_GPIO_MULTI_INFO *u32Instance
          );
      BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

      break;
    }
    case SUBTYPE_GPIO_MODE_REQ:
    {
      GPIO_MULTI_MODE astGPIOMultiMode[MAX_IDX];
      PGPIO_MULTI_MODE pstGPIOMultiMode = NULL;
	  UINT32 u32Instance=(uiLength-4) /SIZE_GPIO_MULTI_MODE;
      DWORD dwNumBytesReturned = 0;
      memset(&astGPIOMultiMode,0,sizeof(GPIO_MULTI_MODE)*MAX_IDX);

      pstGPIOMultiMode = (PGPIO_MULTI_MODE) pMsg->szData;

      iRet = DeviceIO_Send(
          &(pWiMAX->m_Device),
          IOCTL_BCM_GPIO_MODE_REQUEST,
          pstGPIOMultiMode,
          SIZE_GPIO_MULTI_MODE *u32Instance ,
          pstGPIOMultiMode,
          SIZE_GPIO_MULTI_MODE *u32Instance,
          &dwNumBytesReturned
          );

      if(iRet)
      {
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_MODE_REQUEST!!");
		
        iRetval=iRet;
		goto returnCode;
		
      }

      memcpy((UCHAR*)&astGPIOMultiMode,(UCHAR*)pstGPIOMultiMode,SIZE_GPIO_MULTI_MODE*u32Instance);

      /* then notify the application with GPIO_STATUS_INDICATION */
      WiMAXMessage_Init(
          pWMMsg,
          TYPE_GPIO,
          SUBTYPE_GPIO_STATUS_INDICATION,
          (UCHAR*)astGPIOMultiMode,
          SIZE_GPIO_MULTI_MODE*u32Instance
          );
      BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

      break;
    }
	case SUBTYPE_GPIO_SET_REQ:
		{
			GPIO_INFO stGPIOInfo;

			PGPIO_INFO pstGPIOInfo = NULL;
			DWORD dwNumBytesReturned = 0;


			pstGPIOInfo = (PGPIO_INFO) pMsg->szData;
			if(pstGPIOInfo->uiGPIONumber > MAX_GPIO_NUMBER_SUPPORTED)	/* valid numbers 0-16. */
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid GPIO Number GPIO_SET_REQ!");
				iRetval=ERROR_INVALID_GPIO_NUMBER;
				goto returnCode;
			}

			iRet = DeviceIO_Send(
									&(pWiMAX->m_Device),
									IOCTL_BCM_GPIO_SET_REQUEST,
									pstGPIOInfo,
									SIZE_GPIO_INFO,
									NULL,
									0,
									&dwNumBytesReturned
									);

			if(iRet)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_SET_REQUEST!!");
				iRetval=iRet;
				goto returnCode;
			}

			memset(&stGPIOInfo, 0, SIZE_GPIO_INFO);
			stGPIOInfo.uiGPIONumber = pstGPIOInfo->uiGPIONumber;
			stGPIOInfo.uiGPIOValue = pstGPIOInfo->uiGPIOValue;

			/* then notify the application with GPIO_STATUS_INDICATION */
			WiMAXMessage_Init(
								pWMMsg,
								TYPE_GPIO,
								SUBTYPE_GPIO_STATUS_INDICATION,
								(UCHAR*)&stGPIOInfo,
								SIZE_GPIO_INFO
							);
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		}
		break;
	case SUBTYPE_GPIO_STATUS_REQ:
		{
			UINT uiGPIOValue;
			GPIO_INFO stGPIOInfo;

			PGPIO_INFO pstGPIOInfo = NULL;
			DWORD dwNumBytesReturned = 0;


			pstGPIOInfo = (PGPIO_INFO) pMsg->szData;
			if(pstGPIOInfo->uiGPIONumber > MAX_GPIO_NUMBER_SUPPORTED)	/* valid numbers 0-16. */
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid GPIO Number GPIO_SET_REQ!");
				iRetval=ERROR_INVALID_GPIO_NUMBER;
				goto returnCode;
			}

			iRet = DeviceIO_Send(
									&(pWiMAX->m_Device),
									IOCTL_BCM_GPIO_STATUS_REQUEST,
									pstGPIOInfo,
									SIZE_GPIO_INFO,
									&uiGPIOValue,
									sizeof(UINT),
									&dwNumBytesReturned
								);
			if(iRet)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_STATUS_REQUEST!!");
				iRetval=iRet;
				goto returnCode;
			}

			memset(&stGPIOInfo, 0, SIZE_GPIO_INFO);
			stGPIOInfo.uiGPIONumber = pstGPIOInfo->uiGPIONumber;
			stGPIOInfo.uiGPIOValue = uiGPIOValue;

			/* then notify the application with GPIO_STATUS_INDICATION */
			WiMAXMessage_Init(
								pWMMsg,
								TYPE_GPIO,
								SUBTYPE_GPIO_STATUS_INDICATION,
								(UCHAR*)&stGPIOInfo,
								SIZE_GPIO_INFO
							);
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		}
		break;
	case SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_REQ:
#ifdef WIN32
		{
			LED_STATE_INFO *pStateInfo = NULL;
			LED_INFORMATION *pstLEDInfo = NULL;
		
			pstLEDInfo = (LED_INFORMATION *) pMsg->szData;

			if(pstLEDInfo->bSaveToConfigFile)
				SaveLEDInformationToCFG((UINT *)pstLEDInfo->LedInfo);


			pStateInfo = pstLEDInfo->LedInfo;

			iRet = DeviceIO_SendOnly(
									&(pWiMAX->m_Device),
									IOCTL_SET_LED_BEHAVIOR_SETTING,
									pStateInfo,
									sizeof(LED_STATE_INFO) * 4); 
			if(iRet)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_STATUS_REQUEST!!");
				iRetval=iRet;
				goto returnCode;
			}

			/* then notify the application with GPIO_STATUS_INDICATION */
			WiMAXMessage_Init(
								pWMMsg,
								TYPE_GPIO,
								SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_RESP,
								0,
								0
							);
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		}
#else
		iRetval=ERROR_API_NOT_SUPPORTED;
		goto returnCode;
#endif
		break;
	case SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS_REQ:
#ifdef WIN32		
		{

			LED_STATE_INFO *pStateInfo = NULL;
			DWORD dwRetValue = 0;
			//pStateInfo = (LED_STATE_INFO *) oWMMsg.stMessage.szData;
			pStateInfo = (LED_STATE_INFO *) pMsg->szData;

			iRet = DeviceIO_Send(&(pWiMAX->m_Device),
									IOCTL_GET_LED_BEHAVIOR_SETTING,
									pStateInfo,
									sizeof(LED_STATE_INFO) * 4,
									pStateInfo,
									sizeof(LED_STATE_INFO) * 4,
									&dwRetValue); 
			if(iRet)
			{
			  DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
				 "Error occurred while sending IOCTL_BCM_GPIO_STATUS_REQUEST!!");
				iRetval=iRet;
				goto returnCode;
			}

			/* then notify the application with GPIO_STATUS_INDICATION */
			WiMAXMessage_Init(
								pWMMsg,
								TYPE_GPIO,
								SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS_RESP,
								(UCHAR *)pStateInfo,
								sizeof(LED_STATE_INFO) * 4
							);
			BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

		}
#else
		iRetval=ERROR_API_NOT_SUPPORTED;
		goto returnCode;
#endif
		break;

	case SUBTYPE_LED_THREAD_STATE_REQ:
		{
			USER_THREAD_REQ stThreadState = {0};
			stThreadState.ThreadState = *(PUINT)pMsg->szData;


			iRet = DeviceIO_SendOnly(
									&(pWiMAX->m_Device),
									IOCTL_BCM_LED_THREAD_STATE_CHANGE_REQ,
									&stThreadState,
									sizeof(stThreadState)); 

			if(iRet)
			{
				DPRINT(DEBUG_ERROR,DEBUG_PATH_TX,"Error occurred while sending IOCTL_BCM_LED_THREAD_STATE_CHANGE_REQ!!");
				iRetval = iRet;
				goto returnCode;
			}

		}
		break;
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
}

/**
@ingroup chipset_calib_conf
*	Function		:	It does WRM to set the LOG request to the device and also it launches the 
					logger thread to notify the application of the logging info.
						
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - pointer to WIMAX_MESSAGE structure
*	@param	UINT uiLength	- Length of the pMsg buffer.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_SendLogReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
    {
		case SUBTYPE_IR_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_IRLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_BR_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_BRLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_PR_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_PRLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_BWL_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_BWLLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_CL_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_CLLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_OL_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_OLLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_CID_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_CIDLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		case SUBTYPE_CQICH_LOG_REQ:
			if(!WiMAXLogger_InitParam(&(pWiMAX->m_CQICHLogger), pMsg->usSubType))
				pWiMAX->m_iLoggerCount++;
			break;

		default:
			return ERROR_INVALID_SUBTYPE;
	}
	if(pWiMAX->m_iLoggerCount == 1)
	{
		BeceemWiMAX_LaunchLoggerThread(pWiMAX);
	}
	return SUCCESS;
}
/**
@ingroup chipset_calib_conf
*	Function		:	This will launch the BeceemWiMAX_LoggerThreadFunc function for notification to the application
					with the logger info.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_LaunchLoggerThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(pWiMAX->m_iLoggerCount == 1)
	{
		if(Thread_Stop(&(pWiMAX->m_LoggerThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillLoggerThread, pWiMAX) != SUCCESS)
			Thread_Terminate(&(pWiMAX->m_LoggerThread));

	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Starting logger thread !!!");
		pWiMAX->m_bKillLoggerThread = 0;
		if(Thread_Run(&(pWiMAX->m_LoggerThread), (PTHREADFUNC)BeceemWiMAX_LoggerThreadFunc, pWiMAX))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching Logger thread failed !!!");
			return ERROR_THREAD_NOT_STARTED;
		}
	}
	return SUCCESS;
}
/**
@ingroup chipset_calib_conf
*	Function		:	Send the logger information to the application if the application has requested for the same on a periodic basis.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
void *BeceemWiMAX_LoggerThreadFunc(void *pParam)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
	UINT32 bWhileLoop=1;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return NULL;

	while (bWhileLoop) 
	{
		/*  Check to kill this thread */
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}

		/*  Process for Log - Check if any log enabled, if enabled call GetLoggerData by passing prevois index */

		pWiMAX->m_iIRPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_IRLogger), pWiMAX->m_iIRPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}

		pWiMAX->m_iBRPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_BRLogger), pWiMAX->m_iBRPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		pWiMAX->m_iPRPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_PRLogger), pWiMAX->m_iPRPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		pWiMAX->m_iBWLPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_BWLLogger), pWiMAX->m_iBWLPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		pWiMAX->m_iCLPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_CLLogger), pWiMAX->m_iCLPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		pWiMAX->m_iOLPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_OLLogger), pWiMAX->m_iOLPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		pWiMAX->m_iCIDPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_CIDLogger), pWiMAX->m_iCIDPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}

		pWiMAX->m_iCQICHPrevLogIndex = BeceemWiMAX_ProcessAndSendLoggerData(pWiMAX, &(pWiMAX->m_CQICHLogger), pWiMAX->m_iCQICHPrevLogIndex);
		if (pWiMAX->m_bKillLoggerThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Breaking out of loop, for Logger thread.");
			break;
		}
		/* Sleep for a second */
#ifndef WIN32
		usleep(1000000);
#else
		Sleep(1000);
#endif
	}
	return (PVOID)SUCCESS; 
}
/**
@ingroup chipset_calib_conf
*	Function		:	Send the logger information to the application if the application has requested for the same on a periodic basis.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ProcessAndSendLoggerData(void *pArg, PWIMAXLOGGER pLogger, int iPrevIndex)
{

	int iLogArrayLen = 0;
	PUCHAR pucData = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !pLogger)
		return ERROR_INVALID_ARGUMENT;

	if(!WiMAXLogger_GetLoggerStatus(pLogger))
		return -1;
	BECEEMCALLOC(pucData,PUCHAR,CHAR,MAX_VARIABLE_LENGTH)
	iLogArrayLen = WiMAXLogger_GetLoggerData(pLogger, &iPrevIndex, pucData, MAX_VARIABLE_LENGTH-4); /* 4 bytes for Logger array length */
	
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Got Logger data. Logger array length is %d", iLogArrayLen);

	if(iLogArrayLen > 0)
	{
		PWIMAXMESSAGE pWMMsg = NULL;
		USHORT usNumBytes =(USHORT)(iLogArrayLen*WiMAXLogger_GetLogElementSize(pLogger));
		pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
		if(!pWMMsg)
		{
			BECEEM_MEMFREE(pucData)
			return ERROR_MALLOC_FAILED;
		}	

	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Building WiMAX Message for CALLBACK...");

		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_START_LOG, 
						WiMAXLogger_GetLogType(pLogger),
						(UCHAR *)&usNumBytes,
						2
						);
		WiMAXMessage_AppendData(pWMMsg, pucData, usNumBytes);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)		
		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sent Logger data to application CALLBACK");
	}
	BECEEM_MEMFREE(pucData)
	return iPrevIndex;
}

int  BeceemWiMAX_KillLoggerThread(void *pvParam)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pvParam;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillLoggerThread--------");
	pWiMAX->m_bKillLoggerThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillLoggerThread---------");
	return SUCCESS;
}

int BeceemWiMAX_SendLogStopReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
    {
		case SUBTYPE_IR_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_IRLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_BR_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_BRLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_PR_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_PRLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_BWL_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_BWLLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_CL_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_CLLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_OL_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_OLLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_CID_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_CIDLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		case SUBTYPE_CQICH_LOG_REQ:
			if(!WiMAXLogger_DisableLog(&(pWiMAX->m_CQICHLogger)))
				pWiMAX->m_iLoggerCount--;
			break;

		default:
			return 0;
	}

	if(pWiMAX->m_iLoggerCount == 0)
	{
		if(Thread_Stop(&(pWiMAX->m_LoggerThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillLoggerThread, pWiMAX) != SUCCESS)
			Thread_Terminate(&(pWiMAX->m_LoggerThread));
	}

	return 0;
}

int BeceemWiMAX_MACAddress(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	int iRet = 0;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
    {
		case SUBTYPE_MACADDRESS_SET_REQ:
		{
			/*WIMAXMESSAGE oWMMsg;*/
			iRet = BeceemWiMAX_WriteMACAddress(pWiMAX, pMsg->szData);

			/*WiMAXMessage_Init(	
							&oWMMsg,
							TYPE_MACADDRESS, 
							SUBTYPE_MACADDRESS_SET_RESP, 
							(UCHAR *)&iRet,
							4
							);

			BeceemWiMAX_CallAppCallback(pWiMAX, &oWMMsg);				*/
		}
		return iRet;
		break;
		default:
			return ERROR_INVALID_ARGUMENT;
	}
	/*return SUCCESS;*/
}

int BeceemWiMAX_WriteMACAddress(PVOID pArg, UCHAR *pucMACAddress)
{
	
	UINT32 uiRetVal = 0, iRet = 0;	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	LINK_STATE stLinkState = {0};
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	
	if(!pWiMAX->m_bInitEEPROM)
		return ERROR_EEPROM_NOTINITIALIZED;

	if(pWiMAX->m_bMACAddressWriteInProgress)
		return ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS;

	iRet = BeceemWiMAX_GetLinkState(pArg, &stLinkState);

	if(stLinkState.ucLinkStatus == LINKUP_DONE)
	{

		PWIMAXMESSAGE pWMMsg = NULL;
		BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
		uiRetVal = (UINT32)ERROR_SS_IS_LINKED_UP;
		WiMAXMessage_Init(	
				pWMMsg,
				TYPE_MACADDRESS, 
				SUBTYPE_MACADDRESS_SET_RESP, 
				(UCHAR *)&uiRetVal,
				4
				);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
		return ERROR_WRITE_MAC_ADDRESS_FAILED;
	}


    //must reset prior to doing the call.
    pWiMAX->m_bMACAddressWriteInProgress = TRUE;
    pWiMAX->m_bKillMACAddressRespThread = 0;
    Event_Reset(&(pWiMAX->m_objMACaddressRespEvent));
    Event_Reset(&(pWiMAX->m_objMACaddressRespEventMsgRecvd));

	Mutex_Lock(&pWiMAX->m_ShMemMutex);

   /* DPRINT(DEBUG_MESSAGE,"Writing MAC : %x-%x-%x-%x-%x-%x",pucMACAddress[0],pucMACAddress[1],pucMACAddress[2],pucMACAddress[3],pucMACAddress[4],pucMACAddress[5]);
	PRINTEEPROMLOG(DEBUG_EEPROM_INFO,"Writing MAC : %x-%x-%x-%x-%x-%x",pucMACAddress[0],pucMACAddress[1],pucMACAddress[2],pucMACAddress[3],pucMACAddress[4],pucMACAddress[5]);

	uiRetVal  = CFG_WriteMacAddress(pucMACAddress);
	if(6 != uiRetVal)
	{
		return ERROR_EEPROM_WRITE_FAILED;
	}
    memset((UCHAR *)ui64MAC, 0, 8);
	CFG_ReadParam(ui64MAC, 6, CFG_COMP_MAC_ADDRESS_ID);
    DPRINT(DEBUG_MESSAGE,"MAC been written : %x-%x-%x-%x-%x-%x",ui64MAC[0],ui64MAC[1],ui64MAC[2],ui64MAC[3],ui64MAC[4],ui64MAC[5]);*/

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Writing MAC : %x-%x-%x-%x-%x-%x", pucMACAddress[0],
	   pucMACAddress[1], pucMACAddress[2], pucMACAddress[3], pucMACAddress[4], pucMACAddress[5]);


	uiRetVal = SendMACAddressRequest(pWiMAX,pucMACAddress);
	Mutex_Unlock(&pWiMAX->m_ShMemMutex);
	if(uiRetVal)
	{
        //must reset prior to doing the call.
        pWiMAX->m_bMACAddressWriteInProgress = FALSE;
        pWiMAX->m_bKillMACAddressRespThread = 0;
        Event_Set(&(pWiMAX->m_objMACaddressRespEvent));
        Event_Set(&(pWiMAX->m_objMACaddressRespEventMsgRecvd));
       
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Writing MAC address IOCTL failed!!");
		return uiRetVal;
	}

	if(SUCCESS == uiRetVal)
	{
		if(Thread_Run(&(pWiMAX->m_MACAddressWriteRespThread),(PTHREADFUNC)BeceemWiMAX_MACAddressWriteThread,pWiMAX))
		{
            //must reset prior to doing the call.
            pWiMAX->m_bMACAddressWriteInProgress = FALSE;
            pWiMAX->m_bKillMACAddressRespThread = 0;
            Event_Set(&(pWiMAX->m_objMACaddressRespEvent));
            Event_Set(&(pWiMAX->m_objMACaddressRespEventMsgRecvd));

		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching MAC address WRITE THREAD failed !!!");
			return ERROR_THREAD_NOT_STARTED;
		}

	}
	return SUCCESS; 
}

PVOID BeceemWiMAX_MACAddressWriteThread(PVOID pArg)
{
	int i = 0;
	int iRespVal  = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objMACaddressRespEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_MAC_ADDRESS;i+=200)
	{
		if(pWiMAX->m_bKillMACAddressRespThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objMACaddressRespEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_MAC_ADDRESS)
	{
		PWIMAXMESSAGE pWMMsg = NULL;
		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
		iRespVal = ERROR_WRITE_MAC_ADDRESS_FAILED; 
		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_MACADDRESS, 
						SUBTYPE_MACADDRESS_SET_RESP, 
						(UCHAR *)&iRespVal,
						4
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
		BECEEM_MEMFREE(pWMMsg)
	}
	pWiMAX->m_bMACAddressWriteInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objMACaddressRespEventMsgRecvd)); 

	return SUCCESS;
}

PVOID BeceemWiMAX_ShutDownRespThread(PVOID pArg)
{
	int i = 0;
	int iRespVal  = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	SET_THREAD_NAME();
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objShutDownRespEventMsgRecvd));

	for(i=0; i<TIMEOUT_FOR_SHUTDOWN_REQ;i+=200)
	{
		if(pWiMAX->m_bKillShutDownRespThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objShutDownRespEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
	if(i>=TIMEOUT_FOR_SHUTDOWN_REQ)
	{
		iRespVal = ERROR_SHUTDOWN_MODE_RESP_TIMEOUT; 
		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_SHUTDOWN, 
						SUBTYPE_SHUTDOWN_RESP, 
						(UCHAR *)&iRespVal,
						4
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	}
	else
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "SUBTYPE_SHUTDOWN_RESP received.");
		 pWiMAX->m_bModemInShutdown=1;			
		WiMAXMessage_Init(
						pWMMsg,
						TYPE_SHUTDOWN, 
						SUBTYPE_SHUTDOWN_RESP, 
						0,
						sizeof(UINT32)
						);
		
		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	}
	Event_Set(&(pWiMAX->m_objShutDownRespEventMsgRecvd));
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int SendMACAddressRequest(PVOID pArg, UCHAR *pucMACAddress)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRetval=0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, SET_MAC_ADDR);
	DriverPacket_AppendData(pDpkt, pucMACAddress, 6);


	iRetval=DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_LINK_REQ, pDpkt);
	BECEEM_MEMFREE(pDpkt)
	return iRetval;
	
}
int BeceemWiMAX_SendWiMAXGetAllMessage(PVOID pArg)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	
	if(pWiMAX->m_bWIMAXOptionsSetInProgress || pWiMAX->m_bWIMAXOptionsAllSetInProgress)
	{
		return ERROR_WIMAX_OPTIONS_SET_IN_PROGRESS;
	}
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, SET_WIMAX_OPTIONS_ALL_REQ );
	DriverPacket_AppendChar(pDpkt, GET_WIMAX_OPTIONS_ALL );
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);

    iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_LINK_REQ, pDpkt);

	BECEEM_MEMFREE(pDpkt)
	return iRet;	
}

int BeceemWiMAX_SendWiMAXSetAllMessage(PVOID pArg,PWIMAX_MESSAGE pMsg)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0, i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PST_WIMAX_OPTIONS_ALL pWiMAXOptionsAll = (PST_WIMAX_OPTIONS_ALL)pMsg->szData;

	

	if(pWiMAX->m_bWIMAXOptionsSetInProgress || pWiMAX->m_bWIMAXOptionsAllSetInProgress)
	{
		return ERROR_WIMAX_OPTIONS_SET_IN_PROGRESS;
	}
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)	
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, SET_WIMAX_OPTIONS_ALL_REQ );
	DriverPacket_AppendChar(pDpkt, SET_WIMAX_OPTIONS_ALL );
	DriverPacket_AppendChar(pDpkt, pWiMAXOptionsAll->u8WimaxOptionSpecifier);
	DriverPacket_AppendChar(pDpkt, 0);

	for( i = 0 ; i < 8; i++)
	{

		switch (pWiMAXOptionsAll->u8WimaxOptionSpecifier & (1<<i))
		{
			case WIMAX_SET_OPTION_BIT0:
			case WIMAX_SET_OPTION_BIT2:
			case WIMAX_SET_OPTION_BIT3:
			case WIMAX_SET_OPTION_BIT4:
            case WIMAX_SET_OPTION_BIT6:
					break;

			case WIMAX_SET_OPTION_BIT5:
				pWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt[i] = ((pWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt[i] * 1000)/5);
								/* Convert from msec to frames */
					break;
		
			case WIMAX_SET_OPTION_BIT1:
				CFG_SetMIBWriteOption(pWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt[i]);
				break;

			
		}

	}
	memcpy(&pWiMAX->stWiMAXOptionAll,pWiMAXOptionsAll, sizeof(ST_WIMAX_OPTIONS_ALL));

	ConvertToHostEndian((PUCHAR)pWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt,sizeof(UINT)*8,1);

	DriverPacket_AppendData(pDpkt, (PUCHAR)pWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt, sizeof(UINT)*8);
	
	
	
	pWiMAX->m_bWIMAXOptionsAllSetInProgress = TRUE;
    pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
    Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
    Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
	
	iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_LINK_REQ, pDpkt);

	if(SUCCESS == iRet)
	{
        if(Thread_Run(&(pWiMAX->m_WIMAXOptionsRespThread),(PTHREADFUNC)BeceemWiMAX_WiMAXOptionsAllMonitor,pWiMAX))
        {
            pWiMAX->m_bWIMAXOptionsAllSetInProgress = FALSE;
            pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
            Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
            Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));

		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching WiMAX OPTIONS ALL thread failed !!!");
			BECEEM_MEMFREE(pDpkt)
            return ERROR_THREAD_NOT_STARTED;
        }
	
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "WiMAX OPTIONS thread started");
		
	}
    else
    {
        pWiMAX->m_bWIMAXOptionsAllSetInProgress = FALSE;
        pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
        Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
        Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
    }
	BECEEM_MEMFREE(pDpkt)	
	return iRet;
}


int BeceemWiMAX_HandleWiMAXOptionsReq(PVOID pArg, PWIMAX_MESSAGE pMsg)
{
	
	switch(pMsg->usSubType)
	{
	case SUBTYPE_SET_WIMAX_OPTIONS_REQ:
		return BeceemWiMAX_SendWimaxOptionsRequest(pArg,pMsg);
	case SUBTYPE_SET_ALL_WIMAX_OPTIONS_REQ:
		return BeceemWiMAX_SendWiMAXSetAllMessage(pArg,pMsg);
	case SUBTYPE_GET_ALL_WIMAX_OPTIONS_REQ:
		return BeceemWiMAX_SendWiMAXGetAllMessage(pArg);
	case SUBTYPE_CONTROLMSG_SUBSCRIPTION_REQ:
		return BeceemWiMAX_SelectControlMsgSubscription(pArg,pMsg);

	default:
		return ERROR_INVALID_ARGUMENT;
		
	}
}
int BeceemWiMAX_SendWimaxOptionsRequest(PVOID pArg, PWIMAX_MESSAGE pMsg)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0;
	UINT *pVal = NULL;
	PST_WIMAX_OPTIONS pstWiMAXOptions = {0};

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(pWiMAX->m_bWIMAXOptionsSetInProgress)
	{
		return ERROR_WIMAX_OPTIONS_SET_IN_PROGRESS;
	}

	pstWiMAXOptions = (PST_WIMAX_OPTIONS)pMsg->szData;

	switch (pstWiMAXOptions->u8WimaxOptionSpecifier)
	{
		case WIMAX_SET_OPTION_BIT0:
		case WIMAX_SET_OPTION_BIT2:
		case WIMAX_SET_OPTION_BIT3:
		case WIMAX_SET_OPTION_BIT4:
		case WIMAX_SET_OPTION_BIT6:
		case (WIMAX_SET_OPTION_BIT3 | WIMAX_SET_OPTION_BIT4):
			pVal = (UINT*)&pstWiMAXOptions->u32WimaxOptionsPayload;
			*pVal = htonl(*pVal);
			break;

		case WIMAX_SET_OPTION_BIT5:
			pVal = (UINT*)&pstWiMAXOptions->u32WimaxOptionsPayload;
			*pVal = ((*pVal * 1000)/5); /* Convert from msec to frames */
			*pVal = htonl(*pVal);
			break;
		
		case WIMAX_SET_OPTION_BIT1:
			CFG_SetMIBWriteOption(pstWiMAXOptions->u32WimaxOptionsPayload);
			return BeceemWiMAX_SendWimaxOptionsResponse(pWiMAX, SUCCESS, sizeof(UINT32));

		default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid WIMAX_SET_OPTION !!!");
			return FALSE;
	}
	
	pWiMAX->m_bSetWiMAXInfoReqFlag = pstWiMAXOptions->u8WimaxOptionSpecifier;
	pWiMAX->m_iSetWiMAXReqInfo = *pVal;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, SET_WIMAX_OPTIONS_REQ);
	DriverPacket_AppendData(pDpkt, (UCHAR*)&pstWiMAXOptions->u8WimaxOptionSpecifier, 1);
	DriverPacket_AppendData(pDpkt, (PUCHAR)pVal, sizeof(UINT32));


    //set prior to calling
    pWiMAX->m_bWIMAXOptionsSetInProgress = TRUE;
    pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
    Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
    Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));

    iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_LINK_REQ, pDpkt);

	if(SUCCESS == iRet)
	{
        if(Thread_Run(&(pWiMAX->m_WIMAXOptionsRespThread),(PTHREADFUNC)BeceemWiMAX_WiMAXOptionsMonitor,pWiMAX))
        {
            pWiMAX->m_bWIMAXOptionsSetInProgress = FALSE;
            pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
            Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
            Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));

		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching WiMAX OPTIONS thread failed !!!");
			BECEEM_MEMFREE(pDpkt)
            return ERROR_THREAD_NOT_STARTED;
        }
	
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "WiMAX OPTIONS thread started");
		
	}
    else
    {
        pWiMAX->m_bWIMAXOptionsSetInProgress = FALSE;
        pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;
        Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
        Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
    }
	BECEEM_MEMFREE(pDpkt)
	return iRet;	
}

PVOID BeceemWiMAX_WiMAXOptionsAllMonitor(PVOID pArg)
{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	ST_WIMAX_OPTIONS_ALL stWiMAXOptionsAll = {0};
	
    SET_THREAD_NAME();
    
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_WIMAX_OPTIONS;i+=200)
	{
		if(pWiMAX->m_bKillWIMAXOptionsSetInProgress == 1)
			break;
		
		if(Event_Wait(&(pWiMAX->m_objSetWIMAXOptionsRespEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_WIMAX_OPTIONS)
	{
		
		stWiMAXOptionsAll.i32Status = ERROR_SET_WIMAX_OPTIONS_RESP_FAILED;
		BeceemWiMAX_SendAppWiMAXOptionsAll(pWiMAX, (UCHAR*)stWiMAXOptionsAll.au32WimaxOptionsPayloadPerOpt,
					stWiMAXOptionsAll.u8WimaxOptionSpecifier, stWiMAXOptionsAll.i32Status,SUBTYPE_SET_ALL_WIMAX_OPTIONS_RESP);	
	}
	
	pWiMAX->m_bWIMAXOptionsAllSetInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd)); 
	return SUCCESS;
}


PVOID BeceemWiMAX_WiMAXOptionsMonitor(PVOID pArg)
{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();
    
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_WIMAX_OPTIONS;i+=200)
	{
		if(pWiMAX->m_bKillWIMAXOptionsSetInProgress == 1)
			break;
		
		if(Event_Wait(&(pWiMAX->m_objSetWIMAXOptionsRespEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_WIMAX_OPTIONS)
	{
		UCHAR payload[32] = {0};
		SINT32 iVal = ERROR_SET_WIMAX_OPTIONS_RESP_FAILED;

		payload[0] = pWiMAX->m_bSetWiMAXInfoReqFlag;
		memcpy(&payload[1], &iVal, sizeof(SINT32));
		BeceemWiMAX_SendWimaxOptionsResponse(pWiMAX, payload, (sizeof(UCHAR) + sizeof(UINT32)));	
	}
	pWiMAX->m_bWIMAXOptionsSetInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd)); 
	return SUCCESS;
}

int BeceemWiMAX_SendVendorSpecificInfoMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	if(!pWiMAX->m_bInitEEPROM)
		return ERROR_EEPROM_NOTINITIALIZED;

	switch(pMsg->usSubType)
    {
	case SUBTYPE_VSA_INFO_READ_REQUEST:
		iRet = BeceemWiMAX_ProcessVSAReadParam(pWiMAX, (PVENDOR_SPECIFIC_PARAM)pMsg->szData);
		break;

	case SUBTYPE_VSA_INFO_WRITE_REQUEST:
		iRet = BeceemWiMAX_ProcessVSAWriteParam(pWiMAX, (PVENDOR_SPECIFIC_PARAM)pMsg->szData);
		break;

	case SUBTYPE_VSA_INFO_REGION_SIZE_REQUEST:
		iRet = BeceemWiMAX_ProcessVSARegionSizeParam(pWiMAX);
		break;

	default:
		break;
	}
	if (iRet < 0)
		return iRet;
	else
		return SUCCESS;	
}

int BeceemWiMAX_ProcessVSAReadParam(PVOID pArg, PVENDOR_SPECIFIC_PARAM pstVendorSpecificParam)
{
	PVENDOR_SPECIFIC_PARAM pstVSAParam = 0;
	PWIMAXMESSAGE pWMMsg = NULL;
	PWIMAX_MESSAGE pstInWMMsg = NULL;
	UINT uiSizeOfParam;
	int iRet = 0,iRetval=0;
	USHORT usSizeOfVSA = 0;
	int iSizeOfVSA = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pstVendorSpecificParam)
	{
		return ERROR_INVALID_ARGUMENT;	
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)	
	pstInWMMsg = (PWIMAX_MESSAGE)calloc(sizeof(WIMAX_MESSAGE),1);
	if(!pstInWMMsg)
	{
		BECEEM_MEMFREE(pWMMsg)
		return ERROR_MALLOC_FAILED;
	}

	
	pstVSAParam = (PVENDOR_SPECIFIC_PARAM)pstInWMMsg->szData;
	

	uiSizeOfParam = CFG_MAKE_USERPARAM_SIZE(pstVendorSpecificParam->u16Offset, pstVendorSpecificParam->u16Size);
	
	iRet =CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8 *)&usSizeOfVSA, sizeof(usSizeOfVSA), CFG_VSA_LENGTH_ID);
	iSizeOfVSA=(int)usSizeOfVSA-3;

	if (iSizeOfVSA < (pstVendorSpecificParam->u16Offset + pstVendorSpecificParam->u16Size))
		return ERROR_NVM_READ_FAILED;

	iRet = CFG_ReadParam(pWiMAX->pEEPROMContextTable, pstVSAParam->au8VSAData, uiSizeOfParam, CFG_VSA_VENDORSPECIFICDATA_ID);
	if (iRet < 0)
	{
		iRetval=iRet;
		goto returnCode;
	}	

	pstVSAParam->u16Offset = pstVendorSpecificParam->u16Offset;
	pstVSAParam->u16Size = pstVendorSpecificParam->u16Size;

	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_VENDOR_SPECIFIC_INFO, 
					SUBTYPE_VSA_INFO_READ_INDICATION, 
					(UCHAR *) pstVSAParam,
					sizeof(VENDOR_SPECIFIC_PARAM) 
					+ pstVendorSpecificParam->u16Size
					- sizeof(pstVendorSpecificParam->au8VSAData)
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pWMMsg)
	BECEEM_MEMFREE(pstInWMMsg)
	return iRetval;

}


int BeceemWiMAX_ProcessVSARegionSizeParam(PVOID pArg)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	int iSizeOfParam = 0;
	short sSizeOfParam = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8 *)&sSizeOfParam, sizeof(sSizeOfParam), CFG_VSA_LENGTH_ID);
	iSizeOfParam = (int)sSizeOfParam - 3;
	if (iSizeOfParam < 0)
		return iSizeOfParam;

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)	

	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_VENDOR_SPECIFIC_INFO, 
					SUBTYPE_VSA_INFO_REGION_SIZE_INDICATION, 
					(UCHAR *) &iSizeOfParam,
					sizeof(UINT32) 
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_ProcessVSAWriteParam(PVOID pArg, PVENDOR_SPECIFIC_PARAM pstVendorSpecificParam)
{
	PVENDOR_SPECIFIC_PARAM pstVSAParam = 0;
	PWIMAXMESSAGE pWMMsg = NULL;
	PWIMAX_MESSAGE pstInWMMsg = NULL;
	int iRet=0;
    USHORT usSizeOfVSA =0;
	int iSizeOfVSA=0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pstVendorSpecificParam)
	{
		return ERROR_INVALID_ARGUMENT;	
	}
	BECEEMCALLOC(pstInWMMsg,PWIMAX_MESSAGE,WIMAX_MESSAGE,1)	
	
	pstVSAParam = (PVENDOR_SPECIFIC_PARAM)pstInWMMsg->szData;
	
	iRet =CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8 *)&usSizeOfVSA, sizeof(usSizeOfVSA), CFG_VSA_LENGTH_ID);
	iSizeOfVSA=(int)usSizeOfVSA-3;

	if (iSizeOfVSA < (pstVendorSpecificParam->u16Offset + pstVendorSpecificParam->u16Size))
		return ERROR_NVM_WRITE_FAILED;
	
	iRet = CFG_WriteVSA(pWiMAX->pEEPROMContextTable, pstVendorSpecificParam->au8VSAData, pstVendorSpecificParam->u16Offset, pstVendorSpecificParam->u16Size);
	if (iRet < 0)
	{
		BECEEM_MEMFREE(pstInWMMsg)
		return ERROR_EEPROM_NOT_ACCESSIBLE;
	}

	pstVSAParam->u16Offset = pstVendorSpecificParam->u16Offset;
	pstVSAParam->u16Size= pstVendorSpecificParam->u16Size;
	memcpy((void *)pstVSAParam->au8VSAData, pstVendorSpecificParam->au8VSAData, pstVendorSpecificParam->u16Size);

	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
	if(!pWMMsg)
	{
		BECEEM_MEMFREE(pstInWMMsg)
		return ERROR_MALLOC_FAILED;
	}
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_VENDOR_SPECIFIC_INFO, 
					SUBTYPE_VSA_INFO_WRITE_INDICATION, 
					(UCHAR *) pstVSAParam,
					sizeof(VENDOR_SPECIFIC_PARAM) 
					+ pstVendorSpecificParam->u16Size
					- 1024
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	BECEEM_MEMFREE(pWMMsg)
	BECEEM_MEMFREE(pstInWMMsg)
	return SUCCESS;
}
int SendDeviceConfigRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0, uiTxReferencePowerInDB = 0, iResetPCMode = -1;
	PWIMAXMESSAGE pWMMsg = NULL;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
        return iRet;

	switch(pMsg->usSubType)
	{	
	case SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_REQ:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		     "Processing SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_REQ...");
		memcpy(&uiTxReferencePowerInDB, pMsg->szData, sizeof(UINT32));
		return BeceemWiMX_SetBasebandTransmitPower(pWiMAX, uiTxReferencePowerInDB);

	case SUBTYPE_GET_BASEBAND_TEMPERATURE_REQ:
		return BeceemWiMX_SendBasebandTemperature(pWiMAX);

	case SUBTYPE_RESET_POWER_CONTROL_REQ:
	{
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Processing SUBTYPE_RESET_POWER_CONTROL_REQ...");
		BeceemWiMAX_wrm(pWiMAX, POWER_LEVEL_ADDRS + 4,0);

		BeceemWiMAX_rdm(pWiMAX,POWER_LEVEL_ADDRS + 4,(UINT *)&iRet, sizeof(UINT32));
		(iRet == 0) ? (iResetPCMode = SUCCESS) : (iResetPCMode = -1);

		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
			 "Sending Response to SUBTYPE_RESET_POWER_CONTROL_REQ: [%d]\n", iResetPCMode);
		BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)	

		WiMAXMessage_Init(	
					pWMMsg,
					TYPE_DEVICE_CONFIG, 
					SUBTYPE_RESET_POWER_CONTROL_RESP, 
					(UCHAR *) &iResetPCMode,
					sizeof(SINT32)
					);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
		BECEEM_MEMFREE(pWMMsg)
		return SUCCESS;
	}
	
	case SUBTYPE_SET_RSSI_CINR_THRESHOLD_REQ:
		return  BeceemWiMAX_NetworkSearchSetThresholdRssiCinr(pArg,pMsg->szData);
			   	
	
	default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		     "Unknown message subtype for Device Config requests!!");
		return ERROR_INVALID_SUBTYPE;
	}

}

int BeceemWiMAX_NetworkSearchSetThresholdRssiCinr(PVOID pArg, PVOID pPayload)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PST_SET_RSSI_CINR pstset = (PST_SET_RSSI_CINR)pPayload;
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0;

	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_REQ);
	DriverPacket_AppendChar(pDpkt, NW_SEARCH_SET_RSSI_CINR_REQ);

    DriverPacket_AppendChar(pDpkt, 0);    
	DriverPacket_AppendChar(pDpkt, 0);


	/* Convert data to Host Endian*/
	ConvertToHostEndian(	(UCHAR *)pstset, 
							sizeof(ST_SET_RSSI_CINR),
							1
						);
				
	/* Append the data */
	DriverPacket_AppendData(	pDpkt, 
								(UCHAR *)pstset, 
								sizeof(ST_SET_RSSI_CINR)
							);

   
    
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending NW_SEARCH_SET_RSSI_CINR.");
	iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "Sending IOCTL_LINK_REQ - NW_SEARCH_SET_RSSI_CINR failed!!");
		BECEEM_MEMFREE(pDpkt)
        return iRet;
	}
	BECEEM_MEMFREE(pDpkt)
	return SUCCESS;	

}


int BeceemWiMAX_SendResetStatistics(PVOID pArg, UINT uiEnableReset)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	int iRet = 0;
	UINT uiResetStatus = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ToggleResetStatistics(pWiMAX, uiEnableReset);
	
	if(iRet != 0)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to apply Reset Statistics policy");
		uiResetStatus = 0;
	}
	else
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Successfully applied Reset Statistics policy");
		uiResetStatus = 1;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_STATISTICS, 
					SUBTYPE_STATISTICS_RESET_CONTROL_RESP, 
					(UCHAR *) &uiResetStatus,
					sizeof(UINT)
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int SetPowerLevel(PVOID pArg, int iPowerLevel)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UINT uiRet = 0;
	int const POWER_LEVEL_MIN = -40;
	int const POWER_LEVEL_MAX = 35;
	 
	if(iPowerLevel < (POWER_LEVEL_MIN * 4) || iPowerLevel > (POWER_LEVEL_MAX * 4))
		return ERROR_TX_POWER_OUT_OF_RANGE;

	if(!((BeceemWiMAX_wrm(pWiMAX,POWER_LEVEL_ADDRS,iPowerLevel) == SUCCESS)&& (BeceemWiMAX_wrm(pWiMAX,POWER_LEVEL_ADDRS + 4,1) == SUCCESS)))
	{
		return ERROR_TX_POWER_SET_FAILED;
	}
	if(BeceemWiMAX_rdm(pWiMAX,POWER_LEVEL_ADDRS, &uiRet, sizeof(UINT32)) == SUCCESS)
	{
		if(!(uiRet == (UINT)iPowerLevel))
		{
			uiRet = ntohl(uiRet);
			if (!(uiRet == (UINT)iPowerLevel))
				return ERROR_TX_POWER_SET_FAILED;
			else
				return SUCCESS;
		}
	}
	
	return SUCCESS;
	
	
	
}

int BeceemWiMX_SetBasebandTransmitPower(PVOID pArg, UINT uiTxReferencePowerInDB)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	int iRet = 0;
	S_SET_TX_POWER_RESPONSE stSetTxPowerResp;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = SetPowerLevel(pWiMAX,(int)uiTxReferencePowerInDB);
	

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Setting BASEBAND transmit power...");

	memset(&stSetTxPowerResp, 0, sizeof(S_SET_TX_POWER_RESPONSE));

	if(iRet == 0)
	{
		stSetTxPowerResp.u16Status = 0;
		stSetTxPowerResp.u8TxPower =(UINT8) uiTxReferencePowerInDB; 
	}
	else
	{
		stSetTxPowerResp.u16Status = 1;
		//stSetTxPowerResp.u8TxPower = uiTxReferencePowerInDB; 
	}

	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_DEVICE_CONFIG, 
					SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_RESP, 
					(UCHAR *) &stSetTxPowerResp,
					sizeof(S_SET_TX_POWER_RESPONSE)
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}
int BeceemWiMX_SendBasebandTemperature(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	INT iTemperature = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(BeceemWiMX_GetBasebandTemperature(pWiMAX,(UINT *)&iTemperature))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Error occurred while getting baseband Temperature");
		return ERROR_RDM_FAILED;
	}
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	
	WiMAXMessage_Init(	
					pWMMsg,
					TYPE_DEVICE_CONFIG, 
					SUBTYPE_GET_BASEBAND_TEMPERATURE_RESP, 
					(UCHAR *) &iTemperature,
					sizeof(INT)
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMax_AntennaReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PDRIVERPACKET pDpkt = NULL;
	int iRet = 0;
	int iPacketSize = 0;
	PST_GPIO_SETTING_INFO pstGpioSettingInfoReq;
	PST_RSSI_INFO_REQ pstRssiInfoReq;
	PST_ANTENNA_INFO_REQ pstAntennaInfoReq;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);

	switch(pMsg->usSubType) 
	{
		case SUBTYPE_ANTENNA_CONFIG_REQ:
			/* Build the packet with VCID, Status*/
			
			DriverPacket_AppendChar(pDpkt, ANTENNA_MSG);
			DriverPacket_AppendChar(pDpkt, ANTENNA_CONFIG_REQ);
			DriverPacket_AppendChar(pDpkt, 0);
			DriverPacket_AppendChar(pDpkt, 0);

			iPacketSize = sizeof(ST_GPIO_SETTING_INFO);
			pstGpioSettingInfoReq = (PST_GPIO_SETTING_INFO)pMsg->szData;

			/* Convert data to Host Endian*/
			ConvertToHostEndian(	(UCHAR *)pstGpioSettingInfoReq, 
									iPacketSize,
									1
								);
				
			/* Append the data */
			DriverPacket_AppendData(	pDpkt, 
										(UCHAR *)pstGpioSettingInfoReq, 
										iPacketSize
									);

            pWiMAX->m_bAntennaConfigInProgress = TRUE;
            pWiMAX->m_bKillAntennaConfigRespThread = 0;
            Event_Reset(&(pWiMAX->m_objAntennaConfigReqEvent));
            Event_Reset(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));

            iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
			if(iRet)
			{
                pWiMAX->m_bAntennaConfigInProgress = FALSE;
                pWiMAX->m_bKillAntennaConfigRespThread = 0;
                Event_Set(&(pWiMAX->m_objAntennaConfigReqEvent));
                Event_Set(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));

		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Sending IOCTL_LINK_REQ - ANTENNA_CONFIG_REQ failed!!");
				BECEEM_MEMFREE(pDpkt)
				return iRet;
			}
			else
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending - ANTENNA_CONFIG_REQ !!");

			if(Thread_Run(&(pWiMAX->m_AntennaConfigRespThread),(PTHREADFUNC)BeceemWiMAX_AntennaConfigRespThreadFunc,pWiMAX))
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Launching Antenna Config Request thread failed !!!");
                pWiMAX->m_bAntennaConfigInProgress = FALSE;
                pWiMAX->m_bKillAntennaConfigRespThread = 0;
                Event_Set(&(pWiMAX->m_objAntennaConfigReqEvent));
                Event_Set(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));
				BECEEM_MEMFREE(pDpkt)
				return ERROR_THREAD_NOT_STARTED;
			}
			
            break;

		case SUBTYPE_ANTENNA_SCANNING_REQ:
			/* Build the packet with VCID, Status*/
			DriverPacket_AppendChar(pDpkt, ANTENNA_MSG);
			DriverPacket_AppendChar(pDpkt, ANTENNA_SCANNING_REQ);
			DriverPacket_AppendChar(pDpkt, 0);
			DriverPacket_AppendChar(pDpkt, 0);

			iPacketSize  = sizeof(ST_RSSI_INFO_REQ);
			pstRssiInfoReq = (PST_RSSI_INFO_REQ)pMsg->szData;

			/* Convert data to Host Endian*/
			ConvertToHostEndian(	(UCHAR *)pstRssiInfoReq, 
									iPacketSize,
									1
								);
				
			/* Append the data */
			DriverPacket_AppendData(	pDpkt, 
										(UCHAR *)pstRssiInfoReq, 
										iPacketSize
									);


            pWiMAX->m_bAntennaScanningInProgress = TRUE;
            pWiMAX->m_bKillAntennaScanningRespThread = 0;
            Event_Reset(&(pWiMAX->m_objAntennaScanningReqEvent));
            Event_Reset(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));

			iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
			if(iRet)
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Sending IOCTL_LINK_REQ - SUBTYPE_ANTENNA_SCANNING_REQ failed!!");

                pWiMAX->m_bAntennaScanningInProgress = FALSE;
                pWiMAX->m_bKillAntennaScanningRespThread = 0;
                Event_Set(&(pWiMAX->m_objAntennaScanningReqEvent));
                Event_Set(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));
				BECEEM_MEMFREE(pDpkt)
                return iRet;
			}
			else
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending - ANTENNA_SCANNING_REQ !!");



			if(Thread_Run(&(pWiMAX->m_AntennaScanningRespThread),(PTHREADFUNC)BeceemWiMAX_AntennaScanningRespThreadFunc,pWiMAX))
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Launching Antenna Scanning Request thread failed !!!");

                pWiMAX->m_bAntennaScanningInProgress = FALSE;
                pWiMAX->m_bKillAntennaScanningRespThread = 0;
                Event_Set(&(pWiMAX->m_objAntennaScanningReqEvent));
                Event_Set(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));
				BECEEM_MEMFREE(pDpkt)
                return ERROR_THREAD_NOT_STARTED;
			}
			
			break;

		case SUBTYPE_ANTENNA_SELECTION_SET_REQ:
			/* Build the packet with VCID, Status*/
			DriverPacket_AppendChar(pDpkt, ANTENNA_MSG);
			DriverPacket_AppendChar(pDpkt, ANTENNA_SELECT_REQ);
			DriverPacket_AppendChar(pDpkt, 0);
			DriverPacket_AppendChar(pDpkt, 0);

			iPacketSize  = sizeof(ST_ANTENNA_INFO_REQ);
			pstAntennaInfoReq = (PST_ANTENNA_INFO_REQ)pMsg->szData;

			/* Convert data to Host Endian*/
			ConvertToHostEndian(	(UCHAR *)pstAntennaInfoReq, 
									iPacketSize,
									1
								);
				
			/* Append the data */
			DriverPacket_AppendData(	pDpkt, 
										(UCHAR *)pstAntennaInfoReq, 
										iPacketSize
									);

			iRet =  DeviceIO_SendDriverPkt(&(pWiMAX->m_Device), IOCTL_LINK_REQ, pDpkt);
			if(iRet)
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Sending IOCTL_LINK_REQ - SUBTYPE_ANTENNA_SELECTION_SET_REQ failed!!");
				BECEEM_MEMFREE(pDpkt)
				return iRet;
			}
			else
		  DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending - ANTENNA_SELECTION_SET_REQ !!");

			pWiMAX->m_bAntennaSelectionInProgress = TRUE;
			pWiMAX->m_bKillAntennaSelectionRespThread = 0;
			Event_Reset(&(pWiMAX->m_objAntennaSelectionReqEvent));

			if(Thread_Run(&(pWiMAX->m_AntennaSelectionRespThread),(PTHREADFUNC)BeceemWiMAX_AntennaSelectionRespThreadFunc,pWiMAX))
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			     "Launching Antenna Selection Set Request thread failed !!!");
				return ERROR_THREAD_NOT_STARTED;
			}

			break;

		default:
			BECEEM_MEMFREE(pDpkt)
			return ERROR_INVALID_ANTENNA_MODE_TYPE;
	}
	BECEEM_MEMFREE(pDpkt)
	return SUCCESS;
}

PVOID BeceemWiMAX_AntennaConfigRespThreadFunc(PVOID pArg)
{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_ANTENNA_CONFIG_REQ;i+=200)
	{
		if(pWiMAX->m_bKillAntennaConfigRespThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objAntennaConfigReqEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_ANTENNA_CONFIG_REQ)
	{
		PWIMAXMESSAGE pWMMsg = NULL;
		ST_GPIO_SETTING_INFO stGpioSettingInfo;

		memset(&stGpioSettingInfo, 0, sizeof(ST_GPIO_SETTING_INFO));

		stGpioSettingInfo.u32ErrorStatus =(UINT32)ERROR_ANTENNA_CONFIG_RESP_TIMEOUT;

		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_CONFIG_RESP, 
						(UCHAR *) &stGpioSettingInfo,
						sizeof(ST_GPIO_SETTING_INFO)
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
		BECEEM_MEMFREE(pWMMsg)
	}
	pWiMAX->m_bAntennaConfigInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd)); 
	
	return SUCCESS;
}

VOID BeceemWiMAX_KillAntennaConfigRespThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillAntennaConfigRespThread--------");
	Event_Set(&(pWiMAX->m_objAntennaConfigReqEvent));
	pWiMAX->m_bKillAntennaConfigRespThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillAntennaConfigRespThread---------");

	return;
}

PVOID BeceemWiMAX_AntennaScanningRespThreadFunc(PVOID pArg)
{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_ANTENNA_SCANNING_REQ;i+=200)
	{
		if(pWiMAX->m_bKillAntennaScanningRespThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objAntennaScanningReqEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_ANTENNA_SCANNING_REQ)
	{
		PWIMAXMESSAGE pWMMsg = NULL;
		PST_RSSI_INFO_IND pstRssiInfoInd = NULL;
	
		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
		pstRssiInfoInd = (PST_RSSI_INFO_IND)calloc(sizeof(ST_RSSI_INFO_IND),1);
		if(!pstRssiInfoInd)
		{
			BECEEM_MEMFREE(pWMMsg)
			return (PVOID)ERROR_MALLOC_FAILED;
		}
		pstRssiInfoInd->u32ErrorStatus =(UINT32)ERROR_ANTENNA_SCANNING_RESP_TIMEOUT;
	

	

		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_SCANNING_RESP, 
						(UCHAR *) pstRssiInfoInd,
						sizeof(ST_RSSI_INFO_IND)
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		BECEEM_MEMFREE(pWMMsg)
		BECEEM_MEMFREE(pstRssiInfoInd)
		
	}
	pWiMAX->m_bAntennaScanningInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd)); 
	
	return SUCCESS;
}

VOID BeceemWiMAX_KillAntennaScanningRespThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillAntennaScanningRespThread--------");
	Event_Set(&(pWiMAX->m_objAntennaScanningReqEvent));
	pWiMAX->m_bKillAntennaScanningRespThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillAntennaScanningRespThread---------");

	return;
}

PVOID BeceemWiMAX_AntennaSelectionRespThreadFunc(PVOID pArg)
{
	int i = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	Event_Reset(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd)); 

	for(i=0; i<TIMEOUT_FOR_ANTENNA_SELECTION_REQ;i+=200)
	{
		if(pWiMAX->m_bKillAntennaSelectionRespThread == 1)
			break;

		if(Event_Wait(&(pWiMAX->m_objAntennaSelectionReqEvent),200)!= WAIT_TIMEOUT)
			break;
	}

	if(i>=TIMEOUT_FOR_ANTENNA_SELECTION_REQ)
	{
		PWIMAXMESSAGE pWMMsg = NULL;
		SINT32	s32RetVal = 0;

		s32RetVal = ERROR_ANTENNA_SELECTION_RESP_TIMEOUT;
		BECEEMCALLOC_RET(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PVOID)
	
		WiMAXMessage_Init(	
						pWMMsg,
						TYPE_ANTENNA_SWITCHING_SELECTION, 
						SUBTYPE_ANTENNA_SELECTION_SET_RESP, 
						(UCHAR *) &s32RetVal,
						sizeof(SINT32)
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);	
		BECEEM_MEMFREE(pWMMsg)
	}
	pWiMAX->m_bAntennaSelectionInProgress = FALSE;
	Event_Set(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd)); 
	return SUCCESS;
}

VOID BeceemWiMAX_KillAntennaSelectionRespThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillAntennaSelectionRespThread--------");
	Event_Set(&(pWiMAX->m_objAntennaSelectionReqEvent));
	pWiMAX->m_bKillAntennaSelectionRespThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillAntennaSelectionRespThread---------");

	return;
}

#ifndef WIN32
int BeceemWiMAX_FirmwareDownload(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
#ifdef REL_4_1
	const UINT CONFIG_START_ADDR  = 0xbf60b004;
#else
	const UINT CONFIG_START_ADDR  = 0xbf60b000;
#endif
	const UINT FIRMWARE_START_ADDR = 0xbfc00000;
	FIRMWARE_INFO stFirmwareDownload= {0};
	int iRet = 0, iMACMgmtClients = 0;
	UINT uiMACRegValue = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	if(!OpenFileForDwnload(pArg))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid file name aborted!!!");
		//Indication here
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FILE_OPERATION);
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		return ERROR_FILE_OPERATION;
	}

	if(pWiMAX->m_FirmwareInfo.u32ImageType == IMAGE_CONFIG_FILE)
	{
			stFirmwareDownload.u32StartAddress =  CONFIG_START_ADDR;
	}
	else if(pWiMAX->m_FirmwareInfo.u32ImageType == IMAGE_FIRMWARE)
	{
			stFirmwareDownload.u32StartAddress =  FIRMWARE_START_ADDR;	
	}
	else
	{
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FIRMWAREDNLD_FAILED);
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		return ERROR_FIRMWAREDNLD_FAILED;
	}


	if(stFirmwareDownload.u32StartAddress == CONFIG_START_ADDR )
	{
		UINT uiDataSize = 0;
		if(BeceemWiMAX_StartReq(pWiMAX))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "IOCTL TO start download failed");
			CloseDownloadFile((PVOID)pWiMAX);
			BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FIRMWAREDNLD_FAILED);
			pWiMAX->m_bFirmwareDownloadDone = TRUE;


			return ERROR_FIRMWAREDNLD_FAILED;
		}
		BECEEMCALLOC(pWiMAX->pFirmwareFile,UCHAR*,char,pWiMAX->m_u32FirmwareSize)
		
		uiDataSize = read(pWiMAX->m_u32FileDescriptor, 
			pWiMAX->pFirmwareFile,pWiMAX->m_u32FirmwareSize);	

		stFirmwareDownload.u32Length = pWiMAX->m_u32FirmwareSize;
		stFirmwareDownload.pvMappedAddress = pWiMAX->pFirmwareFile;
		
		iRet = SendDownloadRequest(pWiMAX,&stFirmwareDownload);
		if (iRet)
			{
				BECEEM_MEMFREE(pWiMAX->pFirmwareFile);
			return iRet;
			}

		CloseDownloadFile((PVOID)pWiMAX);
		
		BECEEM_MEMFREE(pWiMAX->pFirmwareFile);
		
		pWiMAX->m_u32FileDescriptor = 0;
		pWiMAX->pFirmwareFile = NULL;
		pWiMAX->m_u32FirmwareSize = 0;
		
	}
	else if(stFirmwareDownload.u32StartAddress ==  FIRMWARE_START_ADDR)
	{
		UINT uiTotalSize = pWiMAX->m_u32FirmwareSize;
		UCHAR *pFirmwareData = NULL;
		
		const UINT MAX_TRANSFER_LEN	= 	4 * 1024;    /*16K*/
		
		UINT uiIndex = 0,uiTotalChunks = 0,uiOddChunkSize = 0;
		UINT uiReadBytes = 0;

		UINT uiDataSize = MAX_TRANSFER_LEN;
		
				
		
		uiTotalChunks = uiTotalSize/MAX_TRANSFER_LEN;
		uiOddChunkSize = uiTotalSize%MAX_TRANSFER_LEN;

		BECEEMCALLOC(pFirmwareData,UCHAR*,UCHAR,MAX_TRANSFER_LEN);

		

		//Remove after debug
		//printf(" uiTotalChunks %d uiOddChunkSize %d\n",uiTotalChunks,uiOddChunkSize);
		
		//

		for(uiIndex = 0 ; uiIndex < uiTotalChunks + 1 ; uiIndex++)
		{
			
			if(uiIndex == uiTotalChunks)
			{
				if(uiOddChunkSize)
					uiDataSize = uiOddChunkSize;
				else
					break;
			}

			uiReadBytes  = read(pWiMAX->m_u32FileDescriptor, pFirmwareData, uiDataSize);	
			if(uiReadBytes != uiDataSize)
			{
			    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Read operation failed!!");
				CloseDownloadFile((PVOID)pWiMAX);
				BECEEM_MEMFREE(pFirmwareData)
		
				pWiMAX->m_u32FileDescriptor = 0;
				pWiMAX->pFirmwareFile = NULL;
				pWiMAX->m_u32FirmwareSize = 0;
				//Indication here
				pWiMAX->m_bFirmwareDownloadDone = TRUE;
				BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
					ERROR_FIRMWAREDNLD_FAILED);
				BECEEM_MEMFREE(pFirmwareData)
				return ERROR_FIRMWAREDNLD_FAILED;
			}
			
			stFirmwareDownload.pvMappedAddress = pFirmwareData;
			stFirmwareDownload.u32Length  = uiDataSize;
			
			
			iRet = SendDownloadRequest(pWiMAX,&stFirmwareDownload);
			if (iRet)
				{
					BECEEM_MEMFREE(pFirmwareData)
				return iRet;
				}

            stFirmwareDownload.u32StartAddress += uiDataSize;
		}
		
		CloseDownloadFile((PVOID)pWiMAX);
		BECEEM_MEMFREE(pFirmwareData)
		
		pWiMAX->m_u32FileDescriptor = 0;
		pWiMAX->pFirmwareFile = NULL;
		pWiMAX->m_u32FirmwareSize = 0;
		
		if(BeceemWiMAX_StopReq(pWiMAX))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "IOCTL TO stop download failed");
			BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
					ERROR_FIRMWAREDNLD_FAILED);
			pWiMAX->m_bFirmwareDownloadDone = TRUE;
			return ERROR_FIRMWAREDNLD_FAILED;
		}
		pWiMAX->m_bInitEEPROM = FALSE;
		BeceemWiMAX_DeviceInitThreadFunc(pWiMAX);
		
		/* If Clients are reading MAC Management messages, keep it enabled */
		SharedMem_ReadData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iMACMgmtClients, sizeof(int), 0);
		if(iMACMgmtClients > 0)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
			   "Number of clients receiving MAC Mgmt Messages %d", iMACMgmtClients);
			BeceemWiMAX_rdmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT));
			uiMACRegValue = uiMACRegValue & 0xFFFE;
			BeceemWiMAX_wrmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT));
		}
	}
	else
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "u32ConfigAddress is WRONG Firmware download aborted!!!");
		
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
					ERROR_FIRMWAREDNLD_FAILED);
				pWiMAX->m_bFirmwareDownloadDone = TRUE;

			return ERROR_FIRMWAREDNLD_FAILED;
	}
		pWiMAX->m_bFirmwareDownloadDone = TRUE;
		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				SUCCESS);
			pWiMAX->m_bFirmwareDownloadDone = TRUE;

	return SUCCESS;
}
int SendDownloadRequest(PVOID pArg,PFIRMWARE_INFO pFirmwareDownload)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	
	
	if(DeviceIO_SendOnly(&(pWiMAX->m_Device), IOCTL_BCM_BUFFER_DOWNLOAD,
                         (void *)pFirmwareDownload, sizeof(FIRMWARE_INFO)))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "SendToDriver failed for IOCTL_BCM_BUFFER_DOWNLOAD !!!");
		pWiMAX->m_bFirmwareDownloadDone = TRUE;

		BeceemWiMAX_SendFirmwareDnldIndication(pWiMAX, 
				ERROR_FIRMWAREDNLD_FAILED);
		pWiMAX->m_bFirmwareDownloadDone = TRUE;

		return ERROR_FIRMWAREDNLD_FAILED;

	}
	return SUCCESS;
}
int BeceemWiMAX_StartReq(PVOID pArg)
{
	int iRet = SUCCESS;
	DWORD dwTemp = 0;
		
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending IOCTL_START_REQ.");

	/*
	 Send IOCTL_LINK_DOWN message
	*/
	iRet = DeviceIO_SendOnly(&(pWiMAX->m_Device),IOCTL_BCM_BUFFER_DOWNLOAD_START,(PVOID)&dwTemp,4);
	if (iRet < 0)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
		   "SendToDriver failed for IOCTL_CHIP_RESET. RESET aborted!!!");
		return -1;
	}


	return 0;
}
/**
@ingroup chipset_calib_conf
*	Function		:	

*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.

*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_StopReq(PVOID pArg)
{
	int iRet = SUCCESS;
	DWORD dwTemp = 0;


	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Sending IOCTL_STOP_BUFFER_REQ.");
	
	iRet = DeviceIO_SendOnly(&(pWiMAX->m_Device),IOCTL_BCM_BUFFER_DOWNLOAD_STOP,(PVOID)&dwTemp,4);
	if (iRet < 0)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "SendToDriver failed for IOCTL_STOP_BUFFER_REQ.!!!");
		return -1;
	}
	return 0;
}
#endif

BOOL BeceemWiMAX_IsSyncInProgress(PVOID pArg)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    if(!pWiMAX)
        return FALSE;

    if(pWiMAX->m_bSyncUpInProgress)
    {
#ifdef WIN32
        //check to make sure we havn't already recieve the syncup complete
        //notification
        if(Event_Wait(&(pWiMAX->m_objSyncUpEvent),0) == WAIT_TIMEOUT)
        {
            //Thread is still running for real
            return TRUE;
        }
        //The thread has issued the callback but has not cleaned up
        // wait for the thread to exit
        if(Thread_Wait(&(pWiMAX->m_SyncUpThread),INFINITE,0) == WAIT_TIMEOUT)
        {
            return TRUE;
        }
        //already issued sync callback has finished ... etc
        //we can issue another one 
#else
        return TRUE;
#endif
    }
    return FALSE;
}

BOOL BeceemWiMAX_IsNetworkSearchInProgress(PVOID pArg)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    if(!pWiMAX)
        return FALSE;

#ifdef WIN32
    /* Bug fix,  If we called this function immedietly
       after the callback notification to the app was sent
       this call fails because the thread hasn't cleaned up yet

       was the net search in progress or is someone trying to kill the thread?
       some addition sync for these variable should probably be added later
       so ensure that 2 threads calling this function at the same time only results
       in 1 request being issued
    */
    if(pWiMAX->m_bNetSearchInProgress || pWiMAX->m_bKillNetSearchThread)
    {

        //The search is in progres... however
        // this may just mean that we have a race condition,  
        // so we need to figure out if this event has been signalled
        if(Event_Wait(&(pWiMAX->m_objNetSearchEvent),0) == WAIT_TIMEOUT)
        {
            //If this is the case there is really another 
            //running
            return TRUE;
        }
        //If we got here that means the callback has been issued
        //but the thread is still cleaning up so we must wait for the thread to exit
        //this will allow us to safely cleanup the thread
        if(Thread_Wait(&(pWiMAX->m_NetSearchThread),INFINITE,0) == WAIT_TIMEOUT)
        {
            return TRUE;
        }
    }
#else
    //warning needs more thread sync
    if(pWiMAX->m_bNetSearchInProgress)
        return TRUE;
#endif
    return FALSE;
}

BOOL BeceemWiMAX_IsNetworkEntryInProgress(PVOID pArg)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    if(!pWiMAX)
        return FALSE;
    if(pWiMAX->m_bNetEntryInProgress)
    {
#ifdef WIN32
        //Fixes race condition
        if(Event_Wait(&(pWiMAX->m_objNetEntryEvent),0) == WAIT_TIMEOUT)
        {
            //then guesss what,  we are really running
            return TRUE;
        }
        //looks like the thread is "done" but hasnt exited yet.
        //or is set to exit soon. 
        if(Thread_Wait(&(pWiMAX->m_NetEntryThread),INFINITE,0) == WAIT_TIMEOUT)
        {
            return TRUE;
        }
#else
        return TRUE;
#endif
    }
    return FALSE;
}

UINT32 BeceemWiMAX_GetTimeSinceNetEntry(PVOID pArg,PWIMAX_MESSAGE pMsg)
{
	int iRet = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	DWORD dwNumBytesReturned = 0;
	PDRIVERPACKET pDpkt = NULL;
	LINK_STATE stLinkState = {0};
	
	ST_TIME_ELAPSED stTimeElapsed = {0};
	double TimeSinceNetEntryInSeconds = 0;
	memset(pMsg->szData,0,sizeof(double));
	
	if(!pWiMAX )
		{
			iRet = ERROR_INVALID_ARGUMENT;
			goto return_case;
		}


	iRet =  BeceemWiMAX_GetLinkState(pWiMAX,&stLinkState);
	
	if(iRet)
		goto return_case;
	if(stLinkState.ucLinkStatus != LINKUP_DONE)
	{
		iRet = ERROR_DEVICE_NOT_IN_LINKUP;
		goto return_case;
	}
	BECEEMCALLOC_RET(pDpkt,PDRIVERPACKET,DRIVERPACKET,1,UINT32)
	
	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, LINK_STATUS_REQ_PAYLOAD);
	
	iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_TIME_SINCE_NET_ENTRY,
							(PVOID)DriverPacket_GetLeaderData(pDpkt),
							sizeof(LEADER_ST)+ DriverPacket_GetPacketDataLength(pDpkt),
							(PVOID)&stTimeElapsed,
							sizeof(ST_TIME_ELAPSED),
							&dwNumBytesReturned
						);
	
//>> need to check and diff with present time here
	if(iRet )
		{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "failed for IOCTL_BCM_TIME_SINCE_NET_ENTRY with %d",
		   iRet);
			goto return_case;
		}

        TimeSinceNetEntryInSeconds = (double)(stTimeElapsed.ul64TimeElapsedSinceNetEntry);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "TimeSinceNetEntryInSeconds %f", TimeSinceNetEntryInSeconds);

		memcpy(pMsg->szData,&TimeSinceNetEntryInSeconds,sizeof(double));
return_case:
		BECEEM_MEMFREE(pDpkt)
	
	return iRet;

}

int BeceemWiMAX_SelectControlMsgSubscription(PVOID pArg,PWIMAX_MESSAGE pMsg)
{

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	UINT uiCntrlMsgMask = *(PUINT)pMsg->szData;
			
#ifdef LINUX

	INT iRet = 0;

	iRet = DeviceIO_SendOnly(
		&(pWiMAX->m_Device),
		IOCTL_BCM_CNTRLMSG_MASK,
		&uiCntrlMsgMask,
		sizeof(uiCntrlMsgMask)
	);

	if(iRet)
 		return iRet;

#endif

	pWiMAX->m_uiControlMsgEnableMask = uiCntrlMsgMask;			

	return SUCCESS;
}

INT  BeceemWiMAX_StatsUpdate()
{
	PWIMAXMESSAGE pWMMsg = NULL;
	PUCHAR pucStatBuffer = NULL;
    INT iRet = SUCCESS;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
    INT iLengthOfStat = 0;

	if(!pWiMAX)
        goto ret_path;   


        pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
    	pucStatBuffer = (PUCHAR)calloc(1,3048);
		iLengthOfStat = BeceemWiMAX_GetStatisticsReal(pWiMAX, pucStatBuffer, 3048);

		/* If error, make stats zero and send it to app */
		if(iLengthOfStat <= 0) 
		{
			iRet =  ERROR_NO_STATISTIC_RESPONSE;		
            goto ret_path;   
		}
		/* Form response packet and send it to application */

		WiMAXMessage_Init(	
			pWMMsg,
			TYPE_STATISTICS, 
			SUBTYPE_BASEBAND_STATISTICS_INDICATION, 
			pucStatBuffer,
			iLengthOfStat
			);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
 ret_path:
   	BECEEM_MEMFREE(pucStatBuffer)
	BECEEM_MEMFREE(pWMMsg)    
	return iRet;
}

INT BeceemWiMAX_DynLoggingHandler(PVOID pArg, PWIMAX_MESSAGE pWMsg, UINT uiMsgLen)
{
    INT iRet = SUCCESS;
	UNUSED_ARGUMENT(pArg);
if(uiMsgLen	< (sizeof(ST_DYNAMIC_LOGGING) - 
				sizeof(ST_SHM_LOG_REC)*(DYN_LOGGING_MAX_PROCESSES - 1) + sizeof(USHORT)*2))
{
	return ERROR_BUFFER_INSUFFICIENT;
}


    switch (pWMsg->usSubType)
	{
	  case SUBTYPE_LOGGING_SET:
	      if (!DynLoggingAttr_Set(pWMsg->szData))
		  {
		      iRet = ERROR_INVALID_ARGUMENT;	// ? TODO - assign appropriate error code
		      goto ret_path;
		  }
	      break;
	  case SUBTYPE_LOGGING_GET:
	      if (!DynLoggingAttr_Get(pWMsg->szData))
		  {
		      iRet = ERROR_INVALID_ARGUMENT;	// ? TODO - assign appropriate error code
		      goto ret_path;
		  }
	      break;
	  default:
	      printf("INVALID SubType %d!\n", pWMsg->usSubType);
	      break;
	}

  ret_path:
    return iRet;
}


INT BeceemWiMAX_ProximitySensorHandler(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PDRIVERPACKET pDpkt = NULL;
	int iPacketSize = 0;	
	PST_PROXIMITY_CONTROL pstProximityControl=NULL;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;
	
	pstProximityControl=(PST_PROXIMITY_CONTROL)	pMsg->szData;

	ALLOCATE_MEMORY(pDpkt,DRIVERPACKET,sizeof(DRIVERPACKET),0);
			
	if(!pDpkt)
	{
	
		iRet= ERROR_MALLOC_FAILED;
		goto retpath;

	}

	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);
	DriverPacket_AppendChar(pDpkt, TYPE_PS_INFO);
	DriverPacket_AppendChar(pDpkt, pstProximityControl->eProximityControl);
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);

	iPacketSize=pstProximityControl->uilength;


	if(iPacketSize>0)
	{		
			/* Append the data */
        PUCHAR pPayloadConvert = (PUCHAR )(pDpkt->m_stPacket.aucData+pDpkt->m_iPayloadLen);
		DriverPacket_AppendData(pDpkt,(UCHAR *)pstProximityControl->auchPayload,iPacketSize);
		ConvertToHostEndian(pPayloadConvert, iPacketSize,1);
	}

	iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_SEND_CONTROL_MESSAGE, pDpkt);

	FREE_MEMORY(pDpkt);
retpath:
		return iRet;




}

INT BeceemWiMAX_Resync_CID_LogHandler(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PDRIVERPACKET pDpkt = NULL;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	iRet = BeceemWiMAX_ValidateWiMaXMessage(pMsg, uiLength);
	if(iRet)
		return iRet;

	BECEEMCALLOC(pDpkt,PDRIVERPACKET,DRIVERPACKET,1)


	DriverPacket_InitWithVal(pDpkt, 0, LINK_CONTROL_REQ);

	if(pMsg->usType==TYPE_RESYNC_LOG_CONTROL)
	{
		DriverPacket_AppendChar(pDpkt, DISCONNECT_LOGGING_REQUEST);
		switch(pMsg->usSubType)
		{
		case SUBTYPE_RESYNC_ENABLE_REQ:
			DriverPacket_AppendChar(pDpkt, SUBTYPE_DISCONNECT_LOGGING_ENABLE_REQUEST);   
			break;
		case SUBTYPE_RESYNC_DISABLE_REQ:	
			DriverPacket_AppendChar(pDpkt, SUBTYPE_DISCONNECT_LOGGING_DISABLE_REQUEST);
			break;
		default:
			  printf("INVALID SubType %d!\n", pMsg->usSubType);				
			  goto retpath;
		}
	}
	else if(pMsg->usType==TYPE_CID_TRACKING_LOG_CONTROL)
	{
		DriverPacket_AppendChar(pDpkt, START_CID_SNOOPING_REQ);
		switch(pMsg->usSubType)
		{
		
			case SUBTYPE_CID_ENABLE_REQ:
				DriverPacket_AppendChar(pDpkt, SUBTYPE_CID_LOGGING_ENABLE_REQUEST);   
				break;
			case SUBTYPE_CID_DISABLE_REQ:	
				DriverPacket_AppendChar(pDpkt, SUBTYPE_CID_LOGGING_DISABLE_REQUEST);
				break;
			default:
				  printf("INVALID SubType %d!\n", pMsg->usSubType);				
				  goto retpath;

		}
	}else
	{
		printf("INVALID Type %d!\n", pMsg->usType);				
		goto retpath;
	}

	
	
	DriverPacket_AppendChar(pDpkt, 0);
	DriverPacket_AppendChar(pDpkt, 0);

	iRet = DeviceIO_SendDriverPkt(&(pWiMAX->m_Device),IOCTL_SEND_CONTROL_MESSAGE, pDpkt);
retpath:
	BECEEM_MEMFREE(pDpkt)
	return iRet;


}