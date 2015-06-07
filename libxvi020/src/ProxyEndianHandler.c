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
 * Description	:	Endian-ness handler for proxy (applicable only for
 *					big-endian machines).
 * Author		:
 * Reviewer		:
 *
 */


#include "ProxyEndianHandler.h"
extern BOOLEAN g_CallByTestApp;
#include "globalobjects.h"
extern volatile BOOL *g_pBigEndianMachine;

int MIBEndianHandler(PMIB_OBJECT_INFO pMIBInfo,USHORT usMIBSubtype,UINT32 uiDirectionOfDataFlow);
#define BE_SWAP_INT(x) x=Swap_Int(x)

UINT CorrectEndiannessForProxy(PWIMAX_MESSAGE pWiMAXMessage,
	UINT uiDataLenth,const UINT uiDirectionOfDataFlow /*1 - going out of library
												0- Coming into  Library*/)
{
	UNUSED_ARGUMENT(uiDataLenth);
	if(!(GpWiMAX->m_bLibraryOperatingOVerProxy && GpWiMAX->m_bBigEndianMachine && !g_CallByTestApp))
	{
		return SUCCESS;
	}

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "CorrectEndiannessForProxy with #Type  %d",
	   pWiMAXMessage->usType);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "Printing Data Before Swap !!!");
	/*HexDump(pWiMAXMessage->szData, uiDataLenth);*/
	switch(pWiMAXMessage->usType)
	{
		case TYPE_NETWORK_SEARCH:

			if((pWiMAXMessage->usSubType == SUBTYPE_NETWORK_SEARCH_RESP)  && uiDirectionOfDataFlow)
			{
				PBSLIST pstBsList = (PBSLIST)pWiMAXMessage->szData;
				int iNumOfBSFound = pstBsList->u32NumBaseStationsFound;
				int i = 0;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstBsList, sizeof(BSLIST) + (iNumOfBSFound -1 )* sizeof(BSINFO), 1);
				for(i = 0; i < iNumOfBSFound; i++)
				{
					/*Revert back the endianness for BSID as it is raw data!*/
					UINT *puiBSID = (UINT *)pstBsList->stBSInformation[i].aucBSID;
					*puiBSID = Swap_Int(*puiBSID);
					*(puiBSID+1) = Swap_Int(*(puiBSID+1));
				}
			}
			else if((SUBTYPE_NETWORK_SEARCH_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
				PST_NETWORK_SEARCH_PARAMS pNetworkSearchParam = (PST_NETWORK_SEARCH_PARAMS)pWiMAXMessage->szData;	
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pNetworkSearchParam, sizeof(ST_NETWORK_SEARCH_PARAMS), 1);
			}
			else if((SUBTYPE_NETWORK_DISCOVERY_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
			
				PST_CONFIG_INFO_PER_NAPID pstConfigInfoPerNAPID = NULL;
				pstConfigInfoPerNAPID = (PST_CONFIG_INFO_PER_NAPID) pWiMAXMessage->szData;
				
				BE_SWAP_INT(pstConfigInfoPerNAPID->u32Duration);
				BE_SWAP_INT(pstConfigInfoPerNAPID->u32NAPID);
				BE_SWAP_INT(pstConfigInfoPerNAPID->u32CountNumBS);
				BE_SWAP_INT(pstConfigInfoPerNAPID->u32NSPChangeCount);


				if (pstConfigInfoPerNAPID->u32CountNumBS>0)
					ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pstConfigInfoPerNAPID->stPhyInfo[0].u32CF, sizeof(ST_NW_DISCOVERY_PHY_INFO)*pstConfigInfoPerNAPID->u32CountNumBS, 1);
		
				
			}
			else if((pWiMAXMessage->usSubType == SUBTYPE_NETWORK_DISCOVERY_RESP)  && uiDirectionOfDataFlow)
			{
				PBSNSPLIST  pBSInfoList=NULL;
				INT i;
				INT u32NumNSPEntries=0;
				pBSInfoList = (PBSNSPLIST)pWiMAXMessage->szData;
				u32NumNSPEntries=(INT)pBSInfoList->stBSInformation.u32NumNSPEntries;
				
				
				BE_SWAP_INT(pBSInfoList->u32Status);
				BE_SWAP_INT(pBSInfoList->u32EndOfDiscoveryRsp);

				BE_SWAP_INT(pBSInfoList->stBSInformation.u32DCDChangeCount);
				BE_SWAP_INT(pBSInfoList->stBSInformation.u32NSPChangeCount);
				BE_SWAP_INT(pBSInfoList->stBSInformation.u32NumNSPEntries);
		
				

				for(i = 0; i < u32NumNSPEntries; i++)
				{					
					BE_SWAP_INT(pBSInfoList->stBSInformation.astNSPList[i].u32NspNameLength);
					BE_SWAP_INT(pBSInfoList->stBSInformation.astNSPList[i].u32NSPID);
				}

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pBSInfoList->stBSInformation.stPHYINFO.u32PID, sizeof(PHY_INFO_NSP_RESP), 1);

				
			}else if((SUBTYPE_CAPL_BASED_SEARCH_REQ == pWiMAXMessage->usSubType || SUBTYPE_AUTONOMOUS_SEARCH_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
				
				
				PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams = NULL;
				pstCaplSearchParams = (PST_CAPL_BASED_SEARCH_PARAMS) pWiMAXMessage->szData;

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstCaplSearchParams,  sizeof(ST_CAPL_BASED_SEARCH_PARAMS), 1); 

				
			}else if((pWiMAXMessage->usSubType == SUBTYPE_CAPL_BASED_SEARCH_RSP)  && uiDirectionOfDataFlow)
			{
				UINT32 u32NumBS=0;
				UINT32 u32Index;
				PST_CAPL_SEARCH_RSP pstCaplRsp = (PST_CAPL_SEARCH_RSP)pWiMAXMessage->szData;

				u32NumBS=pstCaplRsp->u32NumBS;
				BE_SWAP_INT(pstCaplRsp->u32ErrorStatus);
				BE_SWAP_INT(pstCaplRsp->u32SearchCompleteFlag);				
				BE_SWAP_INT(pstCaplRsp->u32NumBS);

				 for (u32Index = 0; u32Index < u32NumBS; u32Index++)
				{
					UINT32 u32Size=0;
					u32Size=(UINT32)(((char*)&pstCaplRsp->bsInfo[u32Index].u32Index-(char*)&pstCaplRsp->bsInfo[u32Index].u32PreambleIndex)+sizeof(UINT32));
					ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pstCaplRsp->bsInfo[u32Index].u32PreambleIndex,  u32Size, 1); 

				}

			}else if((SUBTYPE_SET_LOWPOWER_SCANMODE_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{	
				PST_SCAN_CONFIG_PARAM pstScanConfigParam;
				pstScanConfigParam = (PST_SCAN_CONFIG_PARAM)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstScanConfigParam,  sizeof(ST_SCAN_CONFIG_PARAM), 1); 
			}else if((pWiMAXMessage->usSubType ==SUBTYPE_SET_LOWPOWER_SCANMODE_RESP)  && uiDirectionOfDataFlow)
			{
				PST_SCAN_CONFIG_PARAM_STATUS pstScanParamStatus;
				pstScanParamStatus = (PST_SCAN_CONFIG_PARAM_STATUS)pWiMAXMessage->szData;
				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstScanParamStatus,  sizeof(ST_SCAN_CONFIG_PARAM_STATUS), 1); 
						
			}


			break;

		case TYPE_NETWORK_ENTRY:
			if((pWiMAXMessage->usSubType == SUBTYPE_SYNC_STATUS_RESP) && uiDirectionOfDataFlow)
			{

				PST_SYNC_STATUS pstSyncStatus = (PST_SYNC_STATUS)pWiMAXMessage->szData;
				pstSyncStatus->u32SyncStatus = Swap_Int(pstSyncStatus->u32SyncStatus);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pstSyncStatus->u32PreambleId,  sizeof(UINT)*13, 1); 
				
			}
			else if ((pWiMAXMessage->usSubType ==SUBTYPE_NETWORK_ENTRY_STATUS_RESP) && uiDirectionOfDataFlow)
			{

				PLINK_STATUS pstLinkStatus = (PLINK_STATUS) pWiMAXMessage->szData;
				pstLinkStatus->eStatusCode = Swap_Int(pstLinkStatus->eStatusCode);
				if(pstLinkStatus->BsInfoIsValidFlag)
				{
					pstLinkStatus->BsInfo.s16CINRMean = Swap_Short(pstLinkStatus->BsInfo.s16CINRMean);

					pstLinkStatus->BsInfo.s16CINRDeviation = Swap_Short(pstLinkStatus->BsInfo.s16CINRDeviation);
					pstLinkStatus->BsInfo.s32MeanRssiReport = Swap_Int(pstLinkStatus->BsInfo.s32MeanRssiReport );
					pstLinkStatus->BsInfo.u32DlCenterFrequency= Swap_Int(pstLinkStatus->BsInfo.u32DlCenterFrequency);
					pstLinkStatus->BsInfo.u32Duration= Swap_Int(pstLinkStatus->BsInfo.u32Duration);
					pstLinkStatus->BsInfo.u32StdDeviationRssiReport= Swap_Int(pstLinkStatus->BsInfo.u32StdDeviationRssiReport);
					pstLinkStatus->BsInfo.u32UlCenterFrequency= Swap_Int(pstLinkStatus->BsInfo.u32UlCenterFrequency);				
				}
				pstLinkStatus->BsInfoIsValidFlag = Swap_Int(pstLinkStatus->BsInfoIsValidFlag);
				pstLinkStatus->u32VendorSpecificInformationLength = Swap_Int(pstLinkStatus->u32VendorSpecificInformationLength);
				
				
				
			}
			else if((SUBTYPE_SYNC_UP_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow) 
			{

				PST_SYNCUP_REQ_PARAMS pstSyncupReq = (PST_SYNCUP_REQ_PARAMS)pWiMAXMessage->szData;
				
				pstSyncupReq->u32CenterFreq = Swap_Int(pstSyncupReq->u32CenterFreq);
				pstSyncupReq->u32Bandwidth = Swap_Int(pstSyncupReq->u32Bandwidth);
				pstSyncupReq->u32PreambleId = Swap_Int(pstSyncupReq->u32PreambleId);
				
			}
			else if((SUBTYPE_NETWORK_ENTRY_REQ == pWiMAXMessage->usSubType ) && !uiDirectionOfDataFlow) 
			{
				PVENDOR_SPECIFIC_LINKPARAMS pVendorSpecificData = (PVENDOR_SPECIFIC_LINKPARAMS)pWiMAXMessage->szData;
				pVendorSpecificData->u32VendorSpecificInformationLength = Swap_Int(pVendorSpecificData->u32VendorSpecificInformationLength);
				
			}
			else if((SUBTYPE_NSP_NETWORK_ENTRY_REQ == pWiMAXMessage->usSubType ) && !uiDirectionOfDataFlow) 
			{
					PNSP_BASED_NETENTRY_ST pstNSPBasedNetEntry; 
					pstNSPBasedNetEntry = (PNSP_BASED_NETENTRY_ST)pWiMAXMessage->szData;

					BE_SWAP_INT(pstNSPBasedNetEntry->u32NspIdValid);
					BE_SWAP_INT(pstNSPBasedNetEntry->u32NspId);
					BE_SWAP_INT(pstNSPBasedNetEntry->stVendorSpecificParam.u32VendorSpecificInformationLength);
		

			}
			else if((pWiMAXMessage->usSubType == SUBTYPE_DEVICE_STATUS_INDICATION) && uiDirectionOfDataFlow)
			{

				PST_MODEM_STATE_INFO pstModemStatus;
				pstModemStatus = (PST_MODEM_STATE_INFO)pWiMAXMessage->szData;
				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstModemStatus,  sizeof(ST_MODEM_STATE_INFO), 1); 
			

			}

			
			break;
		
		case TYPE_SHUTDOWN:
			if (((pWiMAXMessage->usSubType ==SUBTYPE_SHUTDOWN_RESP)
				|| (pWiMAXMessage->usSubType == SUBTYPE_SHUTDOWN_MODE_STATUS_RESP))
				&& uiDirectionOfDataFlow) 
			{
				UINT *pData = (UINT*)pWiMAXMessage->szData;
				*pData = Swap_Int(*pData);
			}
			else if (pWiMAXMessage->usSubType == SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND)
			{
				PBSINFO pstBSInfo = (PBSINFO)pWiMAXMessage->szData;
				pstBSInfo->s32RelativeSignalStrength = Swap_Int(pstBSInfo->s32RelativeSignalStrength);
				pstBSInfo->u32Bandwidth = Swap_Int(pstBSInfo->u32Bandwidth);
				pstBSInfo->u32CenterFrequency = Swap_Int(pstBSInfo->u32CenterFrequency);
				pstBSInfo->s32CINR = Swap_Int(pstBSInfo->s32CINR);
				pstBSInfo->u32PreambleIndex = Swap_Int(pstBSInfo->u32PreambleIndex);
				pstBSInfo->u32DcdChangeCount = Swap_Int(pstBSInfo->u32DcdChangeCount);
				pstBSInfo->u32NspChangeCount = Swap_Int(pstBSInfo->u32NspChangeCount);
			}
			break;

		case TYPE_SS_INFO:
			if((pWiMAXMessage->usSubType == SUBTYPE_SS_INFO_INDICATION)&& uiDirectionOfDataFlow) 
			{
				SUBSCRIBER_STATION_INFO *pstSSInfo =(SUBSCRIBER_STATION_INFO *) pWiMAXMessage->szData;
				pstSSInfo->BasebandChipVersion = Swap_Int(pstSSInfo->BasebandChipVersion);
				
				pstSSInfo->u32FirmwareVersion[0]= Swap_Int(pstSSInfo->u32FirmwareVersion[0]);
				pstSSInfo->u32FirmwareVersion[1]= Swap_Int(pstSSInfo->u32FirmwareVersion[1]);
				pstSSInfo->u32FirmwareVersion[2]= Swap_Int(pstSSInfo->u32FirmwareVersion[2]);

				pstSSInfo->u32LibraryVersion[0]= Swap_Int(pstSSInfo->u32LibraryVersion[0]);
				pstSSInfo->u32LibraryVersion[1]= Swap_Int(pstSSInfo->u32LibraryVersion[1]);
				pstSSInfo->u32LibraryVersion[2]= Swap_Int(pstSSInfo->u32LibraryVersion[2]);
				pstSSInfo->RFChipVersion= Swap_Int(pstSSInfo->RFChipVersion);
				
				pstSSInfo->u32EEPROMVersion[0] = Swap_Int(pstSSInfo->u32EEPROMVersion[0]);
				pstSSInfo->u32EEPROMVersion[1] = Swap_Int(pstSSInfo->u32EEPROMVersion[1]);
			}
			else if ((pWiMAXMessage->usSubType == SUBTYPE_SS_EEPROM_INFO_RESP)&& uiDirectionOfDataFlow)
			{
				UINT *pData = (UINT*)pWiMAXMessage->szData;
				*pData = Swap_Int(*pData);			
			}
			else if ((pWiMAXMessage->usSubType == SUBTYPE_SS_EEPROM_DETAILED_INFO_RESP)&& uiDirectionOfDataFlow)
			{
				PST_SS_EEPROM_DETAILED_INFO pEEPROMDetailedInfo=NULL;
				pEEPROMDetailedInfo = (PST_SS_EEPROM_DETAILED_INFO)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pEEPROMDetailedInfo, sizeof(ST_SS_EEPROM_DETAILED_INFO), 1);

			}

			break;
			
		case TYPE_MACADDRESS:
			if((pWiMAXMessage->usSubType == SUBTYPE_MACADDRESS_SET_RESP)&& uiDirectionOfDataFlow) 
			{
				UINT *pUINT = (UINT*)pWiMAXMessage->szData;
				*pUINT = Swap_Int(*pUINT);

			}
			break;

		case  TYPE_CHIPSET_CONTROL :
			if((pWiMAXMessage->usSubType == SUBTYPE_FIRMWARE_DNLD_STATUS_RESP) && uiDirectionOfDataFlow) 
			{
				FIRMWARE_STATUS_INDICATION *pFirmwareStatusInd = (FIRMWARE_STATUS_INDICATION*)pWiMAXMessage->szData;
				pFirmwareStatusInd->u32FirmwareDownloadStatus = 
					Swap_Int(pFirmwareStatusInd->u32FirmwareDownloadStatus);


				pFirmwareStatusInd->stFirmWareInfo.u32ImageType = 
					Swap_Int(pFirmwareStatusInd->stFirmWareInfo.u32ImageType);
				
			}
			else if((pWiMAXMessage->usSubType == SUBTYPE_FIRMWARE_DNLD_REQ) && !uiDirectionOfDataFlow) 
			{
					PST_FIRMWARE_DNLD pFirmwareDwnload =(PST_FIRMWARE_DNLD)pWiMAXMessage->szData;
					pFirmwareDwnload->u32ImageType = Swap_Int(pFirmwareDwnload->u32ImageType);
			}
			else if((pWiMAXMessage->usSubType == SUBTYPE_RESET_STATUS_RESP) &&  uiDirectionOfDataFlow)
					*(UINT *)pWiMAXMessage->szData = Swap_Int(*(UINT *)pWiMAXMessage->szData);
			break;

		case DEVICE_RESET_UNLOAD_TYPE:
			break;
			
		case TYPE_STATISTICS :
			if((SUBTYPE_BASEBAND_STATISTICS_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				stStatistics_SS* pStatistics = (stStatistics_SS*)pWiMAXMessage->szData;
	
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER,
			     "Size of Stats before Swap: [%d]\n", pStatistics->u32SizeOfStatsStruct);
				pStatistics->u32SizeOfStatsStruct = Swap_Int(pStatistics->u32SizeOfStatsStruct);
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER,
			     "Size of Stats after Swap: [%d]\n", pStatistics->u32SizeOfStatsStruct);
				pStatistics->u32NumOfFramesRecvd = Swap_Int(pStatistics->u32NumOfFramesRecvd);

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tLinkStats,  sizeof(stLinkStats), 1);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tMPStats, sizeof(stMPStatistics), 1);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tDPStats,  sizeof(stDPStatistics), 1);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tLinkStatsEx,  sizeof(UINT)*2, 1);				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pStatistics->au32DLMCS, sizeof(UINT) *NUMBER_OF_FEC_TYPES*2, 1);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tPhyStateInfo.u32FrameNumber, sizeof(UINT)*9, 1);				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tHoEventInfo,  sizeof(stHoEventInfo), 1);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->u32ModemState,  sizeof(UINT)*6,1);

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tHOEventInfoEx,  sizeof(stHoEventInfoEx), 1);

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->u32PhySyncGtbCount,  sizeof(UINT)*2,1);

				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pStatistics->tErrorCounter,  sizeof(stErrorCounter), 1);

			}
			else if((SUBTYPE_BASEBAND_STATISTICS_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
					UINT *pUINT = (UINT*)pWiMAXMessage->szData;
					*pUINT = Swap_Int(*pUINT);
			}
			else if((SUBTYPE_STATISTICS_RESET_CONTROL_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
					UINT *pUINT = (UINT*)pWiMAXMessage->szData;
					*pUINT = Swap_Int(*pUINT);
			}else if((SUBTYPE_STATISTICS_RESET_CONTROL_RESP == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
					UINT *pUINT = (UINT*)pWiMAXMessage->szData;
					*pUINT = Swap_Int(*pUINT);
			}


			break;

		case TYPE_IDLE_MODE:
			if((SUBTYPE_IDLE_MODE_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				PST_IDLEMODE_INDICATION pstIdleModeIndication = (PST_IDLEMODE_INDICATION)pWiMAXMessage->szData;
	
				pstIdleModeIndication->u32IdleModePeriod = Swap_Int(pstIdleModeIndication->u32IdleModePeriod);
				pstIdleModeIndication->s32RSSI = Swap_Int(pstIdleModeIndication->s32RSSI);
				pstIdleModeIndication->s32CINR = Swap_Int(pstIdleModeIndication->s32CINR);
				pstIdleModeIndication->u32PreambleIndex = Swap_Int(pstIdleModeIndication->u32PreambleIndex);
				pstIdleModeIndication->u32VendorSpecificInformationLength = Swap_Int(pstIdleModeIndication->u32VendorSpecificInformationLength);
			}else if((SUBTYPE_IDLE_MODE_STATUS_RESP == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				*(UINT *)pWiMAXMessage->szData = Swap_Int(*(UINT *)pWiMAXMessage->szData);
			}	
			else if((SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
				*(UINT *)pWiMAXMessage->szData = Swap_Int(*(UINT *)pWiMAXMessage->szData);
			}
			else if((SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP == pWiMAXMessage->usSubType) && uiDirectionOfDataFlow)
			{
				*(UINT *)pWiMAXMessage->szData = Swap_Int(*(UINT *)pWiMAXMessage->szData);
			}else if((SUBTYPE_IDLE_MODE_WAKEUP_RESP == pWiMAXMessage->usSubType) && uiDirectionOfDataFlow)
			{
				*(UINT *)pWiMAXMessage->szData = Swap_Int(*(UINT *)pWiMAXMessage->szData);
			}
			break;
			
		case TYPE_MACMGMT:
			if((SUBTYPE_MACMGMT_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				PS_MAC_MGMTMESSAGE pMAC_MGMT_Message = (PS_MAC_MGMTMESSAGE)pWiMAXMessage->szData;
				pMAC_MGMT_Message ->u32FrameNumber = Swap_Int(pMAC_MGMT_Message ->u32FrameNumber);
				pMAC_MGMT_Message ->u32LengthOfMessage = Swap_Int(pMAC_MGMT_Message ->u32LengthOfMessage);
				pMAC_MGMT_Message ->u32CID = Swap_Int(pMAC_MGMT_Message ->u32CID);
				pMAC_MGMT_Message ->u32MsgID = Swap_Int(pMAC_MGMT_Message ->u32MsgID);
			}
			else if ((SUBTYPE_MACMGMT_GET_STATUS_RESP == pWiMAXMessage->usSubType) && uiDirectionOfDataFlow)
			{
					UINT *pUINT = (UINT*)pWiMAXMessage->szData;
					*pUINT = Swap_Int(*pUINT);
			}
			break;
		
		case TYPE_AUTHENTICATION:
			if((SUBTYPE_AUTH_PARAM_INDICATION == pWiMAXMessage->usSubType)&& uiDirectionOfDataFlow) 
			{
				PAUTH_PARAMETER_INDICATION pAuthParam = (PAUTH_PARAMETER_INDICATION)pWiMAXMessage->szData;
				pAuthParam->usAuthPayloadLength = Swap_Short(pAuthParam->usAuthPayloadLength);
				
			}
			else if((SUBTYPE_AUTH_WRITE_PARAM_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
				PWRITE_AUTH_PARAMS pAuthParams = (PWRITE_AUTH_PARAMS)pWiMAXMessage->szData;
				pAuthParams->usAuthParamsPayloadLength = Swap_Short(pAuthParams->usAuthParamsPayloadLength);
				
			}

			else if((SUBTYPE_EAP_TRANSFER_INDICATION == pWiMAXMessage->usSubType )||(SUBTYPE_EAP_TRANSFER_REQ == pWiMAXMessage->usSubType ))
			{
				PEAP_PAYLOAD pEAPParams = (PEAP_PAYLOAD)pWiMAXMessage->szData;
				pEAPParams->usEAPPayloadLength = Swap_Short(pEAPParams->usEAPPayloadLength );
				
			}
			else if((SUBTYPE_EAP_COMPLETE_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
				{
					PEAP_COMPLETE_INFO pEAP_Comp =  (PEAP_COMPLETE_INFO)pWiMAXMessage->szData;
					
					pEAP_Comp->usEAPCompletePayloadLength = Swap_Short(pEAP_Comp->usEAPCompletePayloadLength );
				}
			else if((SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
				PST_EAP_RSA_PRIVATE pstEAPRSAPrivate = NULL;
				pstEAPRSAPrivate = (PST_EAP_RSA_PRIVATE) pWiMAXMessage->szData;

				BE_SWAP_INT(pstEAPRSAPrivate->u32DataLen);
				BE_SWAP_INT(pstEAPRSAPrivate->u32Padding);

			}
			else if((SUBTYPE_EAP_ENCRYPT_PRIVATE_RESPONSE == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				PST_EAP_RSA_PRIVATE pstEAPRSAPrivate = NULL;
				pstEAPRSAPrivate = (PST_EAP_RSA_PRIVATE) pWiMAXMessage->szData;

				BE_SWAP_INT(pstEAPRSAPrivate->u32DataLen);
				BE_SWAP_INT(pstEAPRSAPrivate->u32Padding);
			}

			break;
		case TYPE_SERVICE_FLOW:
			 if((SUBTYPE_SF_ADD_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
		 	 {
		 		stLocalSFAddIndication *pDSAInd = (stLocalSFAddIndication *)pWiMAXMessage->szData;
				ConvertAddIndicationEndianess(pDSAInd, 2);
		 	 }
			 else  if((SUBTYPE_SF_DELETE_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			 {
				stLocalSFDeleteIndication *pDSDInd = (stLocalSFDeleteIndication *)pWiMAXMessage->szData;
				pDSDInd->u16CID = Swap_Short(pDSDInd->u16CID);
				pDSDInd->u16TID = Swap_Short(pDSDInd->u16TID);
				pDSDInd->u16VCID = Swap_Short(pDSDInd->u16VCID);
				pDSDInd->u32SFID = Swap_Int(pDSDInd->u32SFID);
			 }
			 else  if((SUBTYPE_SF_CHANGE_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			 {
				stLocalSFChangeIndication *pDSCInd = (stLocalSFChangeIndication *)pWiMAXMessage->szData;

				ConvertAddIndicationEndianess(pDSCInd,2);
				
			 }
			else if((SUBTYPE_SF_ADD_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
		 	{
		 		stLocalSFAddRequest *pstSFAddInd = (stLocalSFAddRequest *)pWiMAXMessage->szData;
				
				ConvertAddRequestEndianess(pstSFAddInd, 2);
		 	}
			else if((SUBTYPE_SF_DELETE_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
					stLocalSFDeleteRequest *pSFDelete = (stLocalSFDeleteRequest *)pWiMAXMessage->szData;
					pSFDelete->u32SFID = Swap_Int(pSFDelete->u32SFID);
					pSFDelete->u16TID = Swap_Short(pSFDelete->u16TID);
			}
			else if((SUBTYPE_SF_CHANGE_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow)
			{
					stLocalSFAddIndication *pstLocalSFAddIndication = (stLocalSFAddIndication*)pWiMAXMessage->szData;
					ConvertAddIndicationEndianess(pstLocalSFAddIndication, 2);
			}
			else if((SUBTYPE_SF_ERTPS_CHANGE_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow)
			{
				PERTPS_GRANT_SIZE_SET_REQ peRTPSGrantSizeSetReq = NULL;
				peRTPSGrantSizeSetReq = (PERTPS_GRANT_SIZE_SET_REQ)pWiMAXMessage->szData;
				peRTPSGrantSizeSetReq->u16VCID=Swap_Short(peRTPSGrantSizeSetReq->u16VCID);
				BE_SWAP_INT(peRTPSGrantSizeSetReq->u32BytesPerUGI);
				BE_SWAP_INT(peRTPSGrantSizeSetReq->u32SDUsPerUGI);


			}

			break;
		case TYPE_GPIO:

			if((SUBTYPE_GPIO_SET_REQ == pWiMAXMessage->usSubType ) ||
				(SUBTYPE_GPIO_STATUS_REQ == pWiMAXMessage->usSubType ) ||
				(SUBTYPE_GPIO_STATUS_INDICATION == pWiMAXMessage->usSubType ))
				 
			{
				PGPIO_INFO pGPIO = (PGPIO_INFO)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pGPIO,  sizeof(GPIO_INFO), 1);
				
			}else if((SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
				LED_INFORMATION *pLedInformation = (LED_INFORMATION *)pWiMAXMessage->szData;

				BE_SWAP_INT(pLedInformation->bSaveToConfigFile);
			}else if((SUBTYPE_GPIO_MULTI_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
				PST_ARRAY_GPIO_MULTI_INFO pGpioMultiInfo = (PST_ARRAY_GPIO_MULTI_INFO)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pGpioMultiInfo, sizeof(ST_ARRAY_GPIO_MULTI_INFO), 1);

			}else if((SUBTYPE_GPIO_MODE_REQ == pWiMAXMessage->usSubType )&& !uiDirectionOfDataFlow) 
			{
				PST_ARRAY_GPIO_MULTI_MODE pGpioMultiMode = (PST_ARRAY_GPIO_MULTI_MODE)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pGpioMultiMode, sizeof(ST_ARRAY_GPIO_MULTI_MODE), 1);

			}
				
							
			break;
		case TYPE_HANDOFF_MESSAGE:
			if((SUBTYPE_HANDOFF_INDICATION== pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
			{

				PS_HOINFO pHOInfo = (PS_HOINFO)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)&pHOInfo->u32NoOfRangingFailure,  sizeof(UINT)*5, 1);
			}
			
			break;
		case TYPE_DEVICE_CONFIG:
			if(((SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_REQ== pWiMAXMessage->usSubType ) && !uiDirectionOfDataFlow) ||
			((SUBTYPE_GET_BASEBAND_TEMPERATURE_RESP == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow))
			{
					UINT *pData = (UINT*)pWiMAXMessage->szData;
					*pData = Swap_Int(*pData);
					
			}
			else if ((SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_RESP == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
			{
				PS_SET_TX_POWER_RESPONSE pPowerSetResp = (PS_SET_TX_POWER_RESPONSE)pWiMAXMessage->szData;
				pPowerSetResp->u16Status = Swap_Short(pPowerSetResp->u16Status);
			}
			else if ((SUBTYPE_RESET_POWER_CONTROL_RESP == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
			{

				UINT *pData = (UINT*)pWiMAXMessage->szData;
				*pData = Swap_Int(*pData);
			
			}
			else if ((SUBTYPE_SET_RSSI_CINR_THRESHOLD_REQ == pWiMAXMessage->usSubType ) && !uiDirectionOfDataFlow)
			{
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pWiMAXMessage->szData, sizeof(ST_SET_RSSI_CINR), 1);

			
			}else if ((SUBTYPE_SET_RSSI_CINR_THRESHOLD_RESP == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
			{
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pWiMAXMessage->szData, sizeof(ST_SET_RSSI_CINR), 1);

			
			}else if ((SUBTYPE_IP_FILTER_STATUS_NOTIFICATION == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
			{
				UINT *pData = (UINT*)pWiMAXMessage->szData;
				*pData = Swap_Int(*pData);
			
			}
			break;

		case TYPE_MIB:
				{
					int iRet=0;
                    int iObjSize=0;
					PMIB_OBJECT_INFO pMIBInfo = (PMIB_OBJECT_INFO)pWiMAXMessage->szData;

                    if(pWiMAXMessage->usSubType == SUBTYPE_MIB_SET_REQ ||
                       pWiMAXMessage->usSubType == SUBTYPE_MIB_SET_RESP||
                       pWiMAXMessage->usSubType == SUBTYPE_MIB_GET_REQ ||
                       pWiMAXMessage->usSubType == SUBTYPE_MIB_GET_RESP) 
                        {
							    iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);
                        }
                /***************************For Multiple MIB**************************/
		
					else if ((SUBTYPE_MULTIPLE_MIB_SET_REQ == pWiMAXMessage->usSubType ) && !uiDirectionOfDataFlow)
					{
						PMIB_OBJECTS pMIBObjs=NULL;
						UINT uiCount = 0;	
						PMIB_OBJECT_INFO pMIBInfo = NULL;
						UINT uiReqMsgLen=0;
						PUCHAR pIBaseAddr = NULL;

						pMIBObjs=(PMIB_OBJECTS)pWiMAXMessage->szData;
						pMIBObjs->u16NumMibObjs=Swap_Short(pMIBObjs->u16NumMibObjs);
						uiReqMsgLen = sizeof(UINT16);
						pIBaseAddr =(PUCHAR) pMIBObjs;

				
						for (uiCount = 0; uiCount < pMIBObjs->u16NumMibObjs; uiCount++)
						{
						    iObjSize=0;
							pMIBInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + uiReqMsgLen);

                          

							iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);
							if(iRet!=SUCCESS)	
								break;	
                            if(pMIBInfo->u32MibObjSize)
                                iObjSize=pMIBInfo->u32MibObjSize-1;
							uiReqMsgLen += sizeof(MIB_OBJECT_INFO)+iObjSize;
									
						}

					}
					else if ((SUBTYPE_MULTIPLE_MIB_SET_RESP == pWiMAXMessage->usSubType ) && uiDirectionOfDataFlow)
					{
					
						PMIB_OBJECTS pMIBObjs=NULL;
						UINT uiCount = 0;	
						PMIB_OBJECT_INFO pMIBInfo = NULL;
						UINT uiReqMsgLen=0;
						PUCHAR pIBaseAddr = 0;
						UINT16 u16NumMibObjs=0;

						pMIBObjs=(PMIB_OBJECTS)pWiMAXMessage->szData;
						u16NumMibObjs=pMIBObjs->u16NumMibObjs;

						pMIBObjs->u16NumMibObjs=Swap_Short(pMIBObjs->u16NumMibObjs);

						uiReqMsgLen = sizeof(UINT16);
						pIBaseAddr = (PUCHAR)pMIBObjs;

						for (uiCount = 0; uiCount < u16NumMibObjs; uiCount++)
						{
							int iObjID ;
                            iObjSize=0;
							pMIBInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + uiReqMsgLen);

							iObjID = pMIBInfo->u32MibObjId;
							if(pMIBInfo->u32MibObjSize)
                                iObjSize=pMIBInfo->u32MibObjSize-1;		

                            iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);
							if(iRet!=SUCCESS)	
								break;		
							

							uiReqMsgLen += sizeof(MIB_OBJECT_INFO)+iObjSize;
								
						}


					}
					else if((SUBTYPE_MULTIPLE_MIB_GET_REQ == pWiMAXMessage->usSubType )  && !uiDirectionOfDataFlow)
					{
    					PMIB_OBJECTS pMIBObjs=NULL;
    					UINT uiCount = 0;	
    					PMIB_OBJECT_INFO pMIBInfo = NULL;
    					UINT uiReqMsgLen=0;
    					PUCHAR pIBaseAddr = NULL;
                       


    					pMIBObjs=(PMIB_OBJECTS)pWiMAXMessage->szData;

    					pMIBObjs->u16NumMibObjs=Swap_Short(pMIBObjs->u16NumMibObjs);
    					uiReqMsgLen = sizeof(UINT16);
    					pIBaseAddr =(PUCHAR) pMIBObjs;

						for (uiCount = 0; uiCount < pMIBObjs->u16NumMibObjs; uiCount++)
						{
    						pMIBInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + uiReqMsgLen);
                            iObjSize=0;                                       
                           

    						iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);

                            if(iRet!=SUCCESS)	
    						    break;
                            if(pMIBInfo->u32MibObjSize)
                                iObjSize=pMIBInfo->u32MibObjSize-1; 
    						uiReqMsgLen += sizeof(MIB_OBJECT_INFO)+iObjSize;
    						
						}

					}
					else if((SUBTYPE_MULTIPLE_MIB_PERIODIC_INDICATION_REQ == pWiMAXMessage->usSubType )  && !uiDirectionOfDataFlow)
					{
    					PMIB_OBJECTS_TIMER pMIBObjs=NULL;
    					UINT uiCount = 0;	
    					PMIB_OBJECT_INFO pMIBInfo = NULL;
    					UINT uiReqMsgLen=0;
    					PUCHAR pIBaseAddr = NULL;

    					pMIBObjs=(PMIB_OBJECTS_TIMER)pWiMAXMessage->szData;

    					pMIBObjs->u16NumMibObjs=Swap_Short(pMIBObjs->u16NumMibObjs);
    					pMIBObjs->u16TimePeriod=Swap_Short(pMIBObjs->u16TimePeriod);
    					pMIBObjs->u16MIBFragmentSize=Swap_Short(pMIBObjs->u16MIBFragmentSize);

    					
    					uiReqMsgLen =3*sizeof(UINT16);
    					pIBaseAddr =(PUCHAR) pMIBObjs;

						for (uiCount = 0; uiCount < pMIBObjs->u16NumMibObjs; uiCount++)
						{
                            iObjSize=0;
    						pMIBInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + uiReqMsgLen);
                           
                           

    						iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);
    				        if(iRet!=SUCCESS)	
    					        break;
                            if(pMIBInfo->u32MibObjSize)
                                iObjSize=pMIBInfo->u32MibObjSize-1; 
    						uiReqMsgLen += sizeof(MIB_OBJECT_INFO)+iObjSize;
								
						}
					}
					else if((SUBTYPE_MULTIPLE_MIB_GET_RESP == pWiMAXMessage->usSubType )  && uiDirectionOfDataFlow)
					{

    					PMIB_OBJECTS pMIBObjs=NULL;
    					UINT uiCount = 0;		
    					PMIB_OBJECT_INFO pMIBInfo = NULL;
    					UINT uiReqMsgLen=0;
    					PUCHAR pIBaseAddr = 0;
    					UINT16 u16NumMibObjs=0;


    					pMIBObjs=(PMIB_OBJECTS)pWiMAXMessage->szData;	


    					u16NumMibObjs=pMIBObjs->u16NumMibObjs;
    					pMIBObjs->u16NumMibObjs=Swap_Short(pMIBObjs->u16NumMibObjs);

    					uiReqMsgLen = sizeof(UINT16);
    					pIBaseAddr = (PUCHAR)pMIBObjs;

    					for (uiCount = 0; uiCount < u16NumMibObjs; uiCount++)
    					{
    						iObjSize=0;
    						pMIBInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + uiReqMsgLen);

    						if(pMIBInfo->u32MibObjSize)
                                iObjSize=pMIBInfo->u32MibObjSize-1; 
                            iRet=MIBEndianHandler(pMIBInfo,pWiMAXMessage->usSubType,uiDirectionOfDataFlow);
    				        if(iRet!=SUCCESS)	
    					        break;
    						
    					    uiReqMsgLen += sizeof(MIB_OBJECT_INFO)+iObjSize;
    						

    					}					
					}
/***************************END Multiple MIB**************************/		
		
			}
			break;
       
		case TYPE_SLEEP_MODE:
			
			if(SUBTYPE_SLEEP_MODE_REQ== pWiMAXMessage->usSubType|| SUBTYPE_SLEEP_MODE_RESP == pWiMAXMessage->usSubType)
			{
				UINT32 uiIndex;
				ST_SLEEPMODE_MSG *pstSleepModeMsg = (ST_SLEEPMODE_MSG*)pWiMAXMessage->szData;
				BE_SWAP_INT(pstSleepModeMsg->numOfDefinitions);
				for(uiIndex = 0; uiIndex < MAX_NUM_OF_SLEEP_MODE_DEFNS ; uiIndex++)
				{

					BE_SWAP_INT(pstSleepModeMsg->PSC_Defn[uiIndex].enActivity);					
					BE_SWAP_INT(pstSleepModeMsg->PSC_Defn[uiIndex].u32NoOfCids);				
					pstSleepModeMsg->PSC_Defn[uiIndex].u16FinalSleepWindowBase=Swap_Short(pstSleepModeMsg->PSC_Defn[uiIndex].u16FinalSleepWindowBase);
					ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstSleepModeMsg->PSC_Defn[uiIndex].aCIDs,sizeof( UINT16)* MAX_NUM_CIDS, 0);
			
				}
				
			}
			

			break;
    
		case TYPE_ANTENNA_SWITCHING_SELECTION:

			if ((SUBTYPE_ANTENNA_CONFIG_REQ == pWiMAXMessage->usSubType) ||
				(SUBTYPE_ANTENNA_CONFIG_RESP == pWiMAXMessage->usSubType))
			{
				PST_GPIO_SETTING_INFO pGpioSettingInfoParam = (PST_GPIO_SETTING_INFO)pWiMAXMessage->szData;	
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pGpioSettingInfoParam, sizeof(ST_GPIO_SETTING_INFO), 1);
			}
			else if ((SUBTYPE_ANTENNA_SCANNING_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
				PST_RSSI_INFO_REQ pRssiInfoReqParam = (PST_RSSI_INFO_REQ)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pRssiInfoReqParam, sizeof(ST_RSSI_INFO_REQ), 1);
			}
			else if ((SUBTYPE_ANTENNA_SCANNING_RESP == pWiMAXMessage->usSubType) && uiDirectionOfDataFlow)
			{
				PST_RSSI_INFO_IND pRssiInfoIndParam = (PST_RSSI_INFO_IND)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pRssiInfoIndParam, sizeof(ST_RSSI_INFO_IND), 1);
			}
			else if ((SUBTYPE_ANTENNA_SELECTION_SET_REQ == pWiMAXMessage->usSubType) && !uiDirectionOfDataFlow)
			{
				PST_ANTENNA_INFO_REQ pAntennaInfoReqParam = (PST_ANTENNA_INFO_REQ)pWiMAXMessage->szData;
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pAntennaInfoReqParam, sizeof(ST_ANTENNA_INFO_REQ), 1);				
			}
			else if ((SUBTYPE_ANTENNA_SELECTION_SET_RESP == pWiMAXMessage->usSubType) && uiDirectionOfDataFlow)
			{
				UINT *pData = (UINT*)pWiMAXMessage->szData;
				*pData = Swap_Int(*pData);
			}
			break;

		case TYPE_SET_WIMAX_OPTIONS:
			if((pWiMAXMessage->usSubType == SUBTYPE_SET_WIMAX_OPTIONS_RESP) && uiDirectionOfDataFlow) 
			{
				PST_WIMAX_OPTIONS pstWimaxOpt =(PST_WIMAX_OPTIONS)pWiMAXMessage->szData;
				pstWimaxOpt->u32WimaxOptionsPayload = Swap_Int(pstWimaxOpt->u32WimaxOptionsPayload);
				
			}
			else if((pWiMAXMessage->usSubType == SUBTYPE_SET_WIMAX_OPTIONS_REQ) && !uiDirectionOfDataFlow) 
			{
				PST_WIMAX_OPTIONS pstWimaxOpt =(PST_WIMAX_OPTIONS)pWiMAXMessage->szData;
				pstWimaxOpt->u32WimaxOptionsPayload = Swap_Int(pstWimaxOpt->u32WimaxOptionsPayload);
			}
			else if(pWiMAXMessage->usSubType == SUBTYPE_SET_ALL_WIMAX_OPTIONS_REQ
				||	pWiMAXMessage->usSubType == SUBTYPE_SET_ALL_WIMAX_OPTIONS_RESP
				||	pWiMAXMessage->usSubType ==	SUBTYPE_GET_ALL_WIMAX_OPTIONS_REQ
				||	pWiMAXMessage->usSubType ==	SUBTYPE_GET_ALL_WIMAX_OPTIONS_RESP) 
			{
				PST_WIMAX_OPTIONS_ALL pstWiMAXOptionsAll;
				pstWiMAXOptionsAll = (PST_WIMAX_OPTIONS_ALL)pWiMAXMessage->szData;
				BE_SWAP_INT(pstWiMAXOptionsAll->i32Status);
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstWiMAXOptionsAll->au32WimaxOptionsPayloadPerOpt, sizeof(UINT32)*MAX_WIMAX_OPTIONS_POSSIBLE, 1);

				
			}
			break;
		
		case TYPE_MAC_LOGING_INFO:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "TYPE_MAC_LOGING_INFO NOT handled");
			break;
		case TYPE_SECURITY:
			if(pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_SET_REQ
				|| pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_SET_RESP
				|| pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_GET_REQ
				|| pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_GET_RESP)
			{
				PSEC_OBJECT_INFO pSec;
				pSec = (PSEC_OBJECT_INFO)pWiMAXMessage->szData;

				BE_SWAP_INT(pSec->u32SecObjId);
				BE_SWAP_INT(pSec->u32SecObjSize);
			}
			else if(pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_GET_PARAM_SIZE_REQ
				|| pWiMAXMessage->usSubType ==SUBTYPE_SECURITY_GET_PARAM_SIZE_RESP) 
			{
				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pWiMAXMessage->szData, sizeof(SEC_PARAMETER_INFO), 1);

			}			
			break;

		case TYPE_VENDOR_SPECIFIC_INFO:
			if(pWiMAXMessage->usSubType ==SUBTYPE_VSA_INFO_READ_REQUEST
				||pWiMAXMessage->usSubType ==SUBTYPE_VSA_INFO_READ_INDICATION
				||pWiMAXMessage->usSubType ==SUBTYPE_VSA_INFO_WRITE_REQUEST
				||pWiMAXMessage->usSubType ==SUBTYPE_VSA_INFO_WRITE_INDICATION) 
			{
				PVENDOR_SPECIFIC_PARAM pstReadATRReq;
				pstReadATRReq = (PVENDOR_SPECIFIC_PARAM)pWiMAXMessage->szData;

				pstReadATRReq->u16Offset=Swap_Short(pstReadATRReq->u16Offset);	
				pstReadATRReq->u16Size=	Swap_Short(pstReadATRReq->u16Size);	
			}else if((SUBTYPE_VSA_INFO_REGION_SIZE_INDICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
					UINT *pUINT = (UINT*)pWiMAXMessage->szData;
					*pUINT = Swap_Int(*pUINT);
			}


			break;

        case TYPE_CID_TRACKING_LOG_CONTROL:
			if((SUBTYPE_CID_LOG_NOTIFICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pWiMAXMessage->szData,  uiDataLenth, 1);

			}
			break;    

		case TYPE_RESYNC_LOG_CONTROL:
			if((SUBTYPE_RESYNC_DISCONNECT_LOG_NOTIFICATION == pWiMAXMessage->usSubType )&& uiDirectionOfDataFlow) 
			{
				PST_RESYNC_LOG_ADDR_SIZE pstAddr_Size =NULL;
				pstAddr_Size = (PST_RESYNC_LOG_ADDR_SIZE )pWiMAXMessage->szData;
				pstAddr_Size->u32ADDR = Swap_Int(pstAddr_Size->u32ADDR);
				pstAddr_Size->u32TotalSize= Swap_Int(pstAddr_Size->u32TotalSize);


			}
			break;
		case TYPE_PROXIMITY_SENSOR:
			if((SUBTYPE_PROXIMITY_FUNCTIONS_RESP == pWiMAXMessage->usSubType)|| (SUBTYPE_PROXIMITY_FUNCTIONS_REQ == pWiMAXMessage->usSubType) )
			{
				UINT uiLength=0;
				PST_PROXIMITY_CONTROL pstProximityControl=NULL;
				pstProximityControl = (PST_PROXIMITY_CONTROL )pWiMAXMessage->szData;


				uiLength=pstProximityControl->uilength;

				pstProximityControl->uilength=Swap_Int(pstProximityControl->uilength);
				if(uiLength>0)
				{
					ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pstProximityControl->auchPayload,  uiLength, 1);

				}		


			}
			break;
		default:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER,
		     "Invalid Message Type sent CheckEndiannessForProxy, This message will be ignored !!!");
			break;
			

	}
	if(uiDirectionOfDataFlow)
	{
		pWiMAXMessage->usSubType = Swap_Short(pWiMAXMessage->usSubType);
		pWiMAXMessage->usType= Swap_Short(pWiMAXMessage->usType);
	}
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "Printing Data After Swap !!!");
	/*HexDump(pWiMAXMessage->szData, uiDataLenth);*/
	

	return SUCCESS;
}


void CheckMachineEndianness() 
{ 
	UINT uiValue = 0x11223344; 
	UCHAR * pCharVal = (UCHAR*)&uiValue; 
	if(*pCharVal == 0x44) 
	{ 
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "\nLittle Endian Machine\n");
		 GpWiMAX->m_bBigEndianMachine = FALSE; 
	} 
	else 
	{ 
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ENDIAN_HANDLER, "\nBIG Endian Machine\n");
		GpWiMAX->m_bBigEndianMachine = TRUE; 
	} 
	g_pBigEndianMachine=&GpWiMAX->m_bBigEndianMachine;//Set pointer in CFG access

} 

int MIBEndianHandler(PMIB_OBJECT_INFO pMIBInfo,USHORT usMIBSubtype,UINT32 uiDirectionOfDataFlow)
{
	int iRet=SUCCESS;
	int nIndex=0;
	UINT32 u32MibObjId=0;
	UINT32 u32MibObjSize=0;

	u32MibObjId=pMIBInfo->u32MibObjId;
	u32MibObjSize=pMIBInfo->u32MibObjSize;
	
	pMIBInfo->u32MibObjId = Swap_Int(pMIBInfo->u32MibObjId);
	pMIBInfo->u32MibObjSize = Swap_Int(pMIBInfo->u32MibObjSize);

	if(!uiDirectionOfDataFlow)
	{
		u32MibObjId=pMIBInfo->u32MibObjId;
		u32MibObjSize=pMIBInfo->u32MibObjSize;
	}

						
	for(nIndex = 0 ; nIndex < 4; nIndex++)
	{
		pMIBInfo->au32Key[nIndex]= Swap_Int(pMIBInfo->au32Key[nIndex]);

	}

	if(usMIBSubtype==SUBTYPE_MIB_SET_REQ ||
		usMIBSubtype==SUBTYPE_MULTIPLE_MIB_SET_REQ ||
		usMIBSubtype==SUBTYPE_MIB_SET_RESP ||
		usMIBSubtype== SUBTYPE_MULTIPLE_MIB_SET_RESP)
	{
		if(u32MibObjSize==0)
		{
			iRet=-1;
			goto ret_path;			
		}
		if(u32MibObjId >= BCM_STANDARD_CFG_MIB_BASE_START && u32MibObjId <= BCM_STANDARD_CFG_MIB_BASE_END)	
 		{
			UINT uiType = GpWiMAX->m_sMIBTable[u32MibObjId].u8FieldType;
			UINT uiAttr = GpWiMAX->m_sMIBTable[u32MibObjId].u16MIBAttribute;
			UINT uiSize = GpWiMAX->m_sMIBTable[u32MibObjId].u32FieldSize;
			USHORT usNumIndex = (USHORT)uiAttr & 0x00FF;
			if(usNumIndex != BIT_NON_INDEX)
				goto ret_path;

			if(TYPE_UINT16 == uiType)
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pMIBInfo->au8MibObjVal,  uiSize, 0);
			else if(TYPE_UINT32 == uiType)
				ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pMIBInfo->au8MibObjVal,  uiSize, 1);
 		}
		else if(u32MibObjId >= BCM_PROPREITARY_CFG_MIB_BASE_START && u32MibObjId <= BCM_PROPREITARY_CFG_MIB_BASE_END)
 		{
			ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pMIBInfo->au8MibObjVal,  u32MibObjSize, 1);
 		}
	}	
	else if((usMIBSubtype==SUBTYPE_MIB_GET_REQ || 
			usMIBSubtype==SUBTYPE_MULTIPLE_MIB_GET_REQ || 
			usMIBSubtype==SUBTYPE_MULTIPLE_MIB_PERIODIC_INDICATION_REQ)&& !uiDirectionOfDataFlow)
	{
		if(u32MibObjSize==0)
		{
			goto ret_path;			
		}
		if(u32MibObjId == BCM_CFG_RDM_REQ)
		{
        	UINT *pDataBuf = (UINT*)pMIBInfo->au8MibObjVal;
            *pDataBuf  = Swap_Int(*pDataBuf );
        }

	}else if((usMIBSubtype==SUBTYPE_MIB_GET_RESP||
			 usMIBSubtype==SUBTYPE_MULTIPLE_MIB_GET_RESP) && uiDirectionOfDataFlow )
	{
		UINT uiType = GpWiMAX->m_sMIBTable[u32MibObjId].u8FieldType;	
		if(u32MibObjSize==0)
		{
			goto ret_path;			
		}
        switch(u32MibObjId) // UINT8 data type
		{
			case BCM_STATS_NUMPACKETS_RECEIVED_BY_HOST://///////(ULONG64) 
			case BCM_STATS_NUMPACKETS_TRANSMITTED_FROM_HOST:
				{
				ULONG64 *pDataBuf = (ULONG64*)pMIBInfo->au8MibObjVal;
				*pDataBuf  = SWAP_Long64(*pDataBuf );
				}
				goto ret_path;	
		}
		if(u32MibObjSize == 4)
		{
			UINT *pDataBuf = (UINT*)pMIBInfo->au8MibObjVal;
			*pDataBuf  = Swap_Int(*pDataBuf);		
		}
		else if(u32MibObjSize == 2)
		{
			USHORT *pDataBuf = (USHORT*)pMIBInfo->au8MibObjVal;
			*pDataBuf  = Swap_Short(*pDataBuf );	
		}
		else if((u32MibObjSize > 6) && (uiType ==TYPE_UINT16))
		{
			ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pMIBInfo->au8MibObjVal,  u32MibObjSize, 0);
		}
		else if(u32MibObjSize > 6)
		{
			ConvertToLittleEndianForProxy(GpWiMAX,(UCHAR *)pMIBInfo->au8MibObjVal,  u32MibObjSize, 1);
		}
	


	}else
	{

			printf("\n**INVALID SUBTYPE****\n");
	}

ret_path:
	return iRet;

}
