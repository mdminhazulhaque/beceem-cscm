/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include "BeceemAPI.h"
#include "BeceemCscmServer.h"
#include "MibUserApi.h"
#include "SysLogFunctions.h"

#ifdef MS_VISUAL_CPP
#pragma warning(push)
#pragma warning(disable: 4100)
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef LINUX
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#ifdef WIN32
#include "windows.h"
#include "Iphlpapi.h"
#endif
#include "cntrl_SignalingInterface.h"
#include "Version.h"
#include "Utility.h"
#ifdef LINUX
#include "ProcessFunctions.h"
#endif


#ifdef LINUX_OR_OSX
#define EXTERN_C
#endif

#include "eap_supplicant.h"

#ifdef CALL_OMA_CLI
#include "Oma_relative.h"
static int oma_start_flag = 0;
#endif


// -------------------------------------------------------------------
// Response handling functions

static void BeceemAPI_NetEntryExitResponse (UINT32 u32DiagLevel, BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_IdleModeResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_SleepModeResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_ShutdownResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);

static void BeceemAPI_NetworkSearchResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_ServiceFlowResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg);
static void BeceemAPI_SSInfoResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_MIBResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMAXMsg, UINT32 u32MsgLength);
static void BeceemAPI_NetworkLayerControlResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMAXMsg);
static void BeceemAPI_MACMgmtMessage(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg);
static void BeceemAPI_EAPResponse(BeceemAPI_t hAPI, eSourceType eSource, PWIMAX_MESSAGE pWiMaxMsg);
static void BeceemAPI_DeviceResetUnloadResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg);
static void BeceemAPI_ChipsetResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg);
static void BeceemAPI_SetWiMAXOptionsResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg, UINT32 u32MsgLength);
static void BeceemAPI_HalfMiniCardResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg);

// =========================================================================================
// Callback responses
// =========================================================================================

#define RESP_MSG_LEVEL(level, pMsg) \
	PrintDiag((level), hAPI, B_SF S(" response received "), (pMsg)); \
	PrintDiag((level), hAPI, S(" (Type = %d - Subtype = %d - Length = %d)\n"), pWiMaxMsg->usType, pWiMaxMsg->usSubType, pQEntry->u32Length);

#define RESP_MSG(pMsg) RESP_MSG_LEVEL(DIAG_NORMAL, pMsg)

// -----------------------------------------------------------------------------------------
// Here messages are dequeued, decoded, and printed
// 
// This function runs in its own thread, as do all the other functions in this file.
// SetState is also called from the main thread.
CSCM_THREAD_FUNCTION BeceemAPI_ProcessMessages (void *pvThreadParam)
{

	BeceemAPI_t hAPI;
	BOOL bHasError = FALSE;
	BOOL bIsMACMsg = FALSE;
	BOOL bDone = FALSE;
	PWIMAX_MESSAGE pWiMaxMsg;
	UINT32 u32DiagPrintLevel;

	hAPI = (BeceemAPI_t) pvThreadParam;
	hAPI->bProcessMesssagesThreadRunning = TRUE;
	u32DiagPrintLevel = hAPI->u32CSCMDebugLogLevel;
	PrintDiag(DIAG_NORMAL,hAPI, S("Process Messages thread has started.\n"));
	SysLogWPrintf(SYSLOG_DEBUG, S("Process Messages thread has started.\n"));

	while (!bDone) 
		{

		// The following call blocks the calling thread until
		// the queue has something to process
		hQueueEntry_t pQEntry = Dequeue(hAPI);

		if (pQEntry == NULL)
			{
			// PrintDiag(DIAG_NORMAL, hAPI, S("WARNING: Dequeue returned NULL\n"));
			continue;
			}

		// quit immediately
		if (pQEntry->eSource == eAbortThread)
			{
			FreeEntry(pQEntry);
			bDone = TRUE;
			continue;
			}

		// check for errors
		bHasError = FALSE;
		if (pQEntry->u8Status == QUEUE_ENTRY_NULL_BUFFER) 
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: BCMCallback called with null buffer.\n"));
			bHasError = TRUE;
			}

		if (pQEntry->u32Length == 0)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Zero length message received.\n"));
			bHasError = FALSE;
			}

		if (pQEntry->pvAPI != (void *) hAPI) 
			{
			PrintDiag(DIAG_NORMAL,hAPI, S("WARNING: Message received with unknown context 0x%08X\n"), pQEntry->pvAPI);
			bHasError = FALSE;
			}

		if (bHasError) {
			PrintDiag(DIAG_NORMAL,hAPI, S("=============== Received Message End ===============\n"));
			FreeEntry(pQEntry);
			continue;
			}

		if (pQEntry->eSource == eFromEAPSupplicant)
			{
			const UINT8 * au8EAPMessage = (const UINT8 *) pQEntry->pu8Message;
			PrintDiag(DIAG_NORMAL,hAPI, S("\n=============== Received Message Start (") B_SF S(") ===========\n"), BeceemAPI_DateTime(hAPI));
			PrintDiag(DIAG_NORMAL,hAPI, S("u32State = ") B_SF, BeceemAPI_DecodeState(hAPI));
			PrintDiag(DIAG_NORMAL,hAPI, S("Message from EAP supplicant to Base Station - length = %d\n"), pQEntry->u32Length);

			if (hAPI->bInnerNAIChange && BeceemAPI_EAP_IsEAPApplicationData(au8EAPMessage, pQEntry->u32Length))
				{
#ifdef SUBTYPE_EAP_SPOOF_INFO_REQ
				// Process Inner NAI change 
				BeceemAPI_EAPNaiSpoofInfoRequest(hAPI, au8EAPMessage, pQEntry->u32Length);
#endif
				}
			else
				{
				// Process messages from the EAP Supplicant to the base station
				BeceemAPI_EAPTransferRequest(hAPI, au8EAPMessage, pQEntry->u32Length);
				}

			PrintDiag(DIAG_NORMAL,hAPI, S("=============== Received Message End ===============\n"));
			}
		else
			{
			// Process messages from the WiMAX API DLL

			pWiMaxMsg = (PWIMAX_MESSAGE) pQEntry->pu8Message;

			bIsMACMsg = pWiMaxMsg->usType == TYPE_MACMGMT;

			if (!bIsMACMsg)
				{
				//Print_SetBuffered(hAPI->hPrintDiag, TRUE);
				Print_SetBuffered(hAPI->hPrintDiag, FALSE);
				if (pWiMaxMsg->usType == TYPE_NETWORK_ENTRY && 
					(pWiMaxMsg->usSubType == SUBTYPE_NETWORK_ENTRY_STATUS_RESP
#ifdef SUBTYPE_DEVICE_STATUS_INDICATION
					 || pWiMaxMsg->usSubType == SUBTYPE_DEVICE_STATUS_INDICATION
#endif
					)
					&& hAPI->u32CSCMDebugLogLevel < DIAG_VERBOSE)
					u32DiagPrintLevel = DIAG_VERBOSE;
				else if (pWiMaxMsg->usType == TYPE_MIB && 
					     pWiMaxMsg->usSubType == SUBTYPE_MULTIPLE_MIB_GET_RESP &&
					     hAPI->u32CSCMDebugLogLevel < DIAG_VERBOSE)
					u32DiagPrintLevel = DIAG_VERBOSE;
				else 
					u32DiagPrintLevel = hAPI->u32CSCMDebugLogLevel;

				PrintDiag(u32DiagPrintLevel, hAPI, S("\n=============== Received Message Start (") B_SF S(") ===========\n"), BeceemAPI_DateTime(hAPI));
				PrintDiag(u32DiagPrintLevel, hAPI, S("u32State = ") B_SFNL, BeceemAPI_DecodeState(hAPI));
				}

			switch(pWiMaxMsg->usType)
				{

				case TYPE_NETWORK_ENTRY: // Type = 2
					RESP_MSG_LEVEL(u32DiagPrintLevel, S("Network entry"));
					BeceemAPI_NetEntryExitResponse(u32DiagPrintLevel, hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_IDLE_MODE:     // Type = 19 
					RESP_MSG(S("Idle mode"));
					BeceemAPI_IdleModeResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_SHUTDOWN:      // Type = 30
					RESP_MSG(S("Shutdown"));
					BeceemAPI_ShutdownResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_HALF_MINI_CARD:// Type = 36					
					RESP_MSG(S("Half-mini card"));
					BeceemAPI_HalfMiniCardResponse(hAPI, pWiMaxMsg);
					break;

				case TYPE_NETWORK_SEARCH:
					RESP_MSG(S("Network search"));
					BeceemAPI_NetworkSearchResponse (hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_LINK_DOWN:
					// Note: No LINK_DOWN responses are implemented
					RESP_MSG(S("Link down"));
					break;

				case TYPE_AUTHENTICATION:
					RESP_MSG(S("Authentication"));
					BeceemAPI_EAPResponse(hAPI, pQEntry->eSource, pWiMaxMsg);
					break;

				case TYPE_SERVICE_FLOW:
					RESP_MSG(S("Service flow"));
					BeceemAPI_ServiceFlowResponse(hAPI, pWiMaxMsg);
					break;

				case TYPE_SS_INFO:
					RESP_MSG(S("SS info"));
					BeceemAPI_SSInfoResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_START_LOG:
					RESP_MSG(S("Start log"));
					break;

				case TYPE_STOP_LOG:
					RESP_MSG(S("Stop log"));
					break;

				case TYPE_VENDOR_SPECIFIC_INFO:
					RESP_MSG(S("Vendor specific info"));
					break;

				case TYPE_MACADDRESS:
					RESP_MSG(S("MAC address response"));
					break;

				case DEVICE_RESET_UNLOAD_TYPE:
					RESP_MSG(S("Device reset or unloaded"));
					BeceemAPI_DeviceResetUnloadResponse(hAPI, pWiMaxMsg);
					break;

				case TYPE_DEVICE_CONFIG:
					RESP_MSG(S("Device config"));
					break;

				case TYPE_CHIPSET_CONTROL:
					RESP_MSG(S("Chipset control"));
					// Note: The client responses are sent from inside this function.
					BeceemAPI_ChipsetResponse(hAPI, pWiMaxMsg);
					break;

				case TYPE_STATISTICS:
					RESP_MSG(S("Statistics"));
					break;

				case TYPE_MACMGMT:
					// RESP_MSG(S("MAC management"));
					BeceemAPI_MACMgmtMessage(hAPI, pWiMaxMsg);
					break;

				case TYPE_GPIO:
					RESP_MSG(S("GPIO"));
					break;
				
				case TYPE_HANDOFF_MESSAGE:
					RESP_MSG(S("Handoff"));
					break;

				case TYPE_MIB:
					RESP_MSG(S("MIB"));
					BeceemAPI_MIBResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_NETWORK_LAYER_CONTROL:
					RESP_MSG(S("Network layer"));
					BeceemAPI_NetworkLayerControlResponse(hAPI, pWiMaxMsg);
					break;

				case TYPE_SLEEP_MODE:
					RESP_MSG(S("Sleep mode"));
					BeceemAPI_SleepModeResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_MAC_LOGING_INFO:
					RESP_MSG(S("MAC debug logging"));
					break;

				case TYPE_ANTENNA_SWITCHING_SELECTION:
					RESP_MSG(S("Antenna switching"));
					break;

				case TYPE_SET_WIMAX_OPTIONS:
					RESP_MSG(S("Set WiMAX options"));
					BeceemAPI_SetWiMAXOptionsResponse(hAPI, pWiMaxMsg, pQEntry->u32Length);
					break;

				case TYPE_PHY_LOGING_INFO:
					RESP_MSG(S("PHY debug logging Info"));
					break;

				case TYPE_DRV_LOGING_INFO:
					RESP_MSG(S("Driver debug logging info"));
					break;

				case TYPE_OMADM:
					RESP_MSG(S("OMADM"));
					break;

				case TYPE_SECURITY:
					RESP_MSG(S("Security"));
					break;

				case TYPE_OMADM_CALIBRATION:
					RESP_MSG(S("OMADM calibration"));
					break;

#ifdef TYPE_TRIGGER_INFO
				case TYPE_TRIGGER_INFO:
					RESP_MSG(S("Trigger info"));
					break;
#endif

				// ignore all others
				default:
					RESP_MSG(S("ERROR: Unknown callback"));
					break;

				} // switch (...

			if (!bIsMACMsg)
				{
				PrintDiag(u32DiagPrintLevel, hAPI, S("=============== Received Message End (type=%d sub-type=%d) ===============\n"),
					pWiMaxMsg->usType, pWiMaxMsg->usSubType);
				Print_Flush(hAPI->hPrintDiag);
				}
			
			}

			FreeEntry(pQEntry);

		} // while (...

		PrintDiag(DIAG_NORMAL,hAPI, S("Process Messages thread has finished.\n"));
		hAPI->bProcessMesssagesThreadRunning = FALSE;
		WcmThread_Exit(0);

		return 0;

} // ProcessMessages (...)

// -----------------------------------------------------------------------------------------
static void BeceemAPI_NetworkSearchResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	PBSLIST pstBSList = 0;
	PBSINFO pBaseInfo = 0;
	UINT uiNumOfBaseStations = 0;
	BOOL bUseEntry, bGotResponse;
	hBSInformation_t hBSInfo;
	int n, idx;

	if (pWiMAXMsg == NULL)
        return;	

	bGotResponse = FALSE;
	switch(pWiMAXMsg->usSubType)
		{	
		case SUBTYPE_NETWORK_SEARCH_RESP:
			
			if (u32MsgLength == 0)
				{
				PrintDiag(DIAG_NORMAL,hAPI, S("\n0 base stations found - got 0 length message.\n"));
				bGotResponse = TRUE;
				}
			else
				{

				pstBSList = (PBSLIST) pWiMAXMsg->szData;
				uiNumOfBaseStations = pstBSList->u32NumBaseStationsFound;
				pBaseInfo = pstBSList->stBSInformation;

				PrintDiag(DIAG_NORMAL,hAPI, S("\n%d base stations found.\n"), uiNumOfBaseStations);

				for (idx = 0; idx < (int)uiNumOfBaseStations; idx++, pBaseInfo++)
					{
					
					PrintDiag(DIAG_NORMAL,hAPI, S("\nBase Station %d\n"), idx+1);
					PrintDiag(DIAG_NORMAL,hAPI, S("    Preamble Index ID:        \t0x%04X\n"), pBaseInfo->u32PreambleIndex);
					PrintDiag(DIAG_NORMAL,hAPI, S("    Relative Signal Strength: \t0x%08X\n"), pBaseInfo->s32RelativeSignalStrength);
					PrintDiag(DIAG_NORMAL,hAPI, S("    CINR                      \t0x%08X\n"), pBaseInfo->s32CINR);
					PrintDiag(DIAG_NORMAL,hAPI, S("    Bandwidth                 \t0x%08X\n"), pBaseInfo->u32Bandwidth);
					PrintDiag(DIAG_NORMAL,hAPI, S("    Center frequency          \t0x%08X\n"), pBaseInfo->u32CenterFrequency);
					PrintDiag(DIAG_NORMAL,hAPI, S("    Base Station ID           \t%02X"), pBaseInfo->aucBSID[7]);
					for (n = 6; n >= 0; n--)
						PrintDiag(DIAG_NORMAL,hAPI, S(":%02X"), pBaseInfo->aucBSID[n]);
					PrintDiag(DIAG_NORMAL,hAPI, S("\n"));
						
					// Clean up some problems with the returned base station data
					bUseEntry = TRUE;
					if (pBaseInfo->u32PreambleIndex & 0xFF00)
						{
						bUseEntry = FALSE;
						PrintDiag(DIAG_NORMAL,hAPI, S("    WARNING: Invalid Preamble Index - entry will not be shown to user.\n"));
						}

					if (bUseEntry) 
						{
						hBSInfo = BSInformation_CreateEx(
							(UINT8 *) pBaseInfo->aucBSID,
							pBaseInfo->u32PreambleIndex,
							pBaseInfo->u32CenterFrequency,
							pBaseInfo->u32Bandwidth,
							(INT32) pBaseInfo->s32RelativeSignalStrength,
							pBaseInfo->s32CINR);

						if (hAPI->u32BSInfoCount < MAX_NET_SEARCH_RESULTS)
							hAPI->ahBSInfo[hAPI->u32BSInfoCount++] = hBSInfo;
						else
							PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Base station information array size exceeded\n"));
						}

					}

				PrintDiag(DIAG_NORMAL,hAPI, S("\n-- Network Search status: "));
				switch (pstBSList->u32SearchCompleteFlag)
					{
					case 0: // Search in progress
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search in progress\n"));
						break;
					case 1: // Search complete
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search complete\n"));
						break;
					case 2: 
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search timed out\n"));
						break;
					default:
						PrintDiag(DIAG_NORMAL,hAPI, S("ERROR: Unknown network search search complete flag\n"));
						break;
					}

				if (pstBSList->u32SearchCompleteFlag == 1 ||
					pstBSList->u32SearchCompleteFlag == 2)
					{
					bGotResponse = TRUE;
					}

				} // if (u32MsgLength == 0)

			if (bGotResponse)
				{
				if (hAPI->hNetSearchParam->bSearchMoreCF)
					{
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
					BeceemAPI_NetworkSearchRequest(hAPI);
					}
				else
					{
					hAPI->eNetworkSearchState  = NsDone;
					hAPI->bIsNetworkSearchDone = TRUE;
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
					BeceemAPI_SetState(hAPI, STATE_NET_SEARCH_DONE);
					BeceemAPI_SetState(hAPI, STATE_WAIT_PHY_SYNC_CMD);
					}
				}
				
			break;

#ifdef SUBTYPE_NETWORK_DISCOVERY_RESP
		// TO DO: Implement response to SUBTYPE_NETWORK_DISCOVERY_RESP
		case SUBTYPE_NETWORK_DISCOVERY_RESP:
			break;
#endif

		case SUBTYPE_CAPL_BASED_SEARCH_RSP:
		{
			static BOOL SearchCompleted = TRUE;
			PST_CAPL_SEARCH_RSP pstSearchResp = NULL;
			PST_DETECTED_BS_INFO pBaseInfo = NULL;

			pstSearchResp = (PST_CAPL_SEARCH_RSP) pWiMAXMsg->szData;
			uiNumOfBaseStations = pstSearchResp->u32NumBS;
			pBaseInfo = pstSearchResp->bsInfo;
					
			PrintDiag(DIAG_NORMAL,hAPI, S("\nReceived CAPL based search response.\n"));
			if (u32MsgLength == 0)
			{
				PrintDiag(DIAG_NORMAL,hAPI, S("\n0 base stations found - got 0 length message.\n"));
				SearchCompleted= TRUE;
			}
			else
			{
				if(SearchCompleted)
					BeceemAPI_ClearBSInformation(hAPI);
				
				PrintDiag(DIAG_NORMAL,hAPI, S("\n-- Network Search status: "));
				switch (pstSearchResp->u32SearchCompleteFlag)
				{
					case 0: // Search in progress
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search in progress\n"));
						SearchCompleted = FALSE;
						break;
					case 1: // Search complete
						SearchCompleted= TRUE;
						PrintDiag(DIAG_NORMAL,hAPI, S("\nA total of %d base stations found.\n"), uiNumOfBaseStations);
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search complete\n"));
						break;
					case 2: 
						SearchCompleted = TRUE;
						PrintDiag(DIAG_NORMAL,hAPI, S("Network search timed out\n"));
						break;
					default:
						SearchCompleted = TRUE;
						PrintDiag(DIAG_NORMAL,hAPI, S("ERROR: Unknown network search search complete flag\n"));
						break;
				}

				if(SearchCompleted)
				{
				
					hAPI->eNetworkSearchState  = NsDone;
					hAPI->bIsNetworkSearchDone = TRUE;
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
					BeceemAPI_SetState(hAPI, STATE_NET_SEARCH_DONE);
					BeceemAPI_SetState(hAPI, STATE_WAIT_PHY_SYNC_CMD);
				}
				else
				{

					PrintDiag(DIAG_NORMAL,hAPI, S("\n%d base stations found.\n"), uiNumOfBaseStations);

					for (idx = 0; idx < (int)uiNumOfBaseStations; idx++, pBaseInfo++)
					{
						
						PrintDiag(DIAG_NORMAL,hAPI, S("\nBase Station %d\n"), idx+1);
						PrintDiag(DIAG_NORMAL,hAPI, S("    Preamble Index ID:        \t0x%04X\n"), pBaseInfo->u32PreambleIndex);
						PrintDiag(DIAG_NORMAL,hAPI, S("    Relative Signal Strength: \t0x%08X\n"), pBaseInfo->s32RSSI);
						PrintDiag(DIAG_NORMAL,hAPI, S("    CINR                      \t0x%08X\n"), pBaseInfo->s32CINR);
						PrintDiag(DIAG_NORMAL,hAPI, S("    Bandwidth                 \t0x%08X\n"), pBaseInfo->u32Bandwidth);
						PrintDiag(DIAG_NORMAL,hAPI, S("    Center frequency          \t0x%08X\n"), pBaseInfo->u32CenterFreq);
						PrintDiag(DIAG_NORMAL,hAPI, S("    Base Station ID           \t%02X"), pBaseInfo->au8Bsid[5]);
						for (n = 4; n >= 0; n--)
							PrintDiag(DIAG_NORMAL,hAPI, S(":%02X"), pBaseInfo->au8Bsid[n]);
						PrintDiag(DIAG_NORMAL,hAPI, S("\n"));
							
						// Clean up some problems with the returned base station data
						bUseEntry = TRUE;
						if (pBaseInfo->u32PreambleIndex & 0xFF00)
						{
							bUseEntry = FALSE;
							PrintDiag(DIAG_NORMAL,hAPI, S("    WARNING: Invalid Preamble Index - entry will not be shown to user.\n"));
						}

						if (bUseEntry) 
						{
							UINT8 cscmBSID[8] = {0,};
							cscmBSID[0] = pBaseInfo->bCompressedMap;
							cscmBSID[1] = pBaseInfo->bFullBsid;
							memcpy(&cscmBSID[2], pBaseInfo->au8Bsid, 6);
							hBSInfo = BSInformation_CreateEx(
								(UINT8 *) cscmBSID,
								pBaseInfo->u32PreambleIndex,
								pBaseInfo->u32CenterFreq,
								pBaseInfo->u32Bandwidth,
								(INT32) pBaseInfo->s32RSSI[0],
								pBaseInfo->s32CINR[0]);

							if (hAPI->u32BSInfoCount < MAX_NET_SEARCH_RESULTS)
								hAPI->ahBSInfo[hAPI->u32BSInfoCount++] = hBSInfo;
							else
								PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Base station information array size exceeded\n"));
						}

					}
				}
			} // if (u32MsgLength == 0)
		}
			break;
			
		default:
			break;

		}

} // NetworkSearchResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_NetEntryExitResponse(UINT32 u32DiagLevel, BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	PLINK_STATUS pLinkStatus = 0;
	PST_SYNC_STATUS pSyncStatus = 0;
#ifdef SUBTYPE_DEVICE_STATUS_INDICATION
	PST_MODEM_STATE_INFO pModemStateInfo = 0;
#endif
	BOOL bInvalidStateTransition = FALSE;
	// BOOL bLoopbackWarningMessage = FALSE;
	STRING bszSyncStatus = EMPTY_STRING;
	STRING bszLinkStatus = EMPTY_STRING;
	STRING bszEAPStatus  = NULL;
	UINT32 u32SyncStatus;
	UINT32 u32IgnoreWaitNetEntryCmd = 0;
	INT32 i32AuthState = 0;
	int i;
	B_CHAR bszBuffer[128];
	BOOL bConnected = TRUE;
	BOOL bNoSync = FALSE;
#ifdef LINUX
	UINT32 u32LastState = hAPI->u32State;
#endif

    if(!pWiMAXMsg)
        return;

	// Save time stamp
	hAPI->u32TotalSeconds = (UINT32) (time(NULL) - hAPI->timeServerStartSeconds);
		
// SYNC_UP is no longer defined in API library headers
#ifndef SYNC_UP
#define SYNC_UP 0
#endif
	if (pWiMAXMsg->usSubType == SUBTYPE_SYNC_STATUS_RESP)
		{

		pSyncStatus = (PST_SYNC_STATUS) pWiMAXMsg->szData;
		u32SyncStatus = pSyncStatus->u32SyncStatus;
		PrintDiag(DIAG_NORMAL,hAPI, S("Sync Status Response Received\n"));
		if( u32SyncStatus == SYNC_UP) {
			bszSyncStatus = S("Sync up");
			PrintDiag(DIAG_NORMAL, hAPI, S("    Preamble Index ID:        \t0x%04X\n"), pSyncStatus->u32PreambleId);
			PrintDiag(DIAG_NORMAL, hAPI, S("    Relative Signal Strength: \t0x%08X\n"), pSyncStatus->s32RSSIMean);
			PrintDiag(DIAG_NORMAL, hAPI, S("    CINR                      \t0x%08X\n"), pSyncStatus->s32CINRMean);
			PrintDiag(DIAG_NORMAL, hAPI, S("    Bandwidth                 \t0x%08X\n"), pSyncStatus->u32Bandwidth);
			PrintDiag(DIAG_NORMAL, hAPI, S("    Center frequency          \t0x%08X\n"), pSyncStatus->u32CenterFreq);
			PrintDiag(DIAG_NORMAL, hAPI, S("    Base Station ID           \t%02X"), pSyncStatus->aucBSID[7]);
			for (i = 6; i >= 0; i--)
				PrintDiag(DIAG_NORMAL,hAPI, S(":%02X"), pSyncStatus->aucBSID[i]);
			PrintDiag(DIAG_NORMAL,hAPI, S("\n"));
			
			BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_UP_TIMEOUT);
			
			BeceemAPI_SetState(hAPI, STATE_WAIT_NET_ENTRY_CMD);
			if ((!hAPI->bIgnoreWaitNetEntryCmd || 
				BeceemAPI_IsActivated(hAPI, ID_EVENT_SYNC_UP_TIMEOUT)) && 
				(hAPI->i32AuthenticationState == EAP_AUTH_DISABLED ||
				 hAPI->i32AuthenticationState == EAP_AUTH_WAIT_SUPPLICANT))
				{
				BeceemAPI_NetworkEntryRequest(hAPI);
				hAPI->bIgnoreWaitNetEntryCmd = TRUE;
				}
			
			hAPI->eIPRefreshState = IpIdle; // Reset the IP refresh state machine

			} 
		else if (u32SyncStatus == PHY_SYNC_ERROR)
			{
			bszSyncStatus = S("PHY Sync Error");
			BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT);
			if (hAPI->u32State != STATE_SHUTDOWN && hAPI->u32State != STATE_HMC_SHUTDOWN)
				BeceemAPI_ConnectionError(hAPI, S("No Sync"));
			hAPI->eIPRefreshState = IpIdle; // Reset the IP refresh state machine
			}
		else
			bszSyncStatus = StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("Unknown response: %d") + u32SyncStatus);
		
#if 0
		Print_SetBuffered(hAPI->hPrintConStat, TRUE);
		PrintConStat(hAPI, S("Sync Status\t ") B_SFNL, bszSyncStatus);
#endif

		PrintDiag(DIAG_NORMAL,hAPI, S("Sync Status\t ") B_SFNL, bszSyncStatus);

#if 0
		if (u32SyncStatus == PHY_SYNC_ERROR
			&& hAPI->eNetworkSearchState == NsSyncDown) 	
			BeceemAPI_NetworkSearchRequest(hAPI);
#endif

		}

#ifdef SUBTYPE_DEVICE_STATUS_INDICATION
	else if (pWiMAXMsg->usSubType == SUBTYPE_DEVICE_STATUS_INDICATION)
		{
		pModemStateInfo = (PST_MODEM_STATE_INFO) pWiMAXMsg->szData;
		switch (pModemStateInfo->eModemStatus)
			{
			case DEVICE_STATUS_IND:
				PrintDiag(DIAG_NORMAL, hAPI, S("Device status indication: ") B_SFNL, 
					BeceemAPI_DecodeDeviceStatus(pModemStateInfo->u32Status, bszBuffer, BCHARSOF(bszBuffer)));
				break;

			case CONNECTION_STATUS_IND:
				PrintDiag(DIAG_NORMAL, hAPI, S("Connection status indication: ") B_SFNL, 
					BeceemAPI_DecodeConnectionProgress(pModemStateInfo->u32Status, bszBuffer, BCHARSOF(bszBuffer)));
				break;

			case LINK_STATUS_IND:
				PrintDiag(DIAG_NORMAL, hAPI, S("Link status indication: ") B_SFNL, 
					BeceemAPI_DecodeLinkStatusReason(pModemStateInfo->u32Status, bszBuffer, BCHARSOF(bszBuffer)));
				break;

			default:
				PrintDiag(DIAG_NORMAL, hAPI, S("Unknown device status type\n"));
				break;
			}

		}
#endif

	else if (pWiMAXMsg->usSubType == SUBTYPE_NETWORK_ENTRY_STATUS_RESP)
		{
		pLinkStatus = (PLINK_STATUS) pWiMAXMsg->szData;
		PrintDiag(u32DiagLevel, hAPI, S("Network Entry Status Received\n"));
		bszLinkStatus = BeceemAPI_DecodeLinkStatus(pLinkStatus->eStatusCode, bszBuffer, BCHARSOF(bszBuffer));
		bszEAPStatus  = pLinkStatus->eStatusCode == WAIT_FOR_NETWORK_ENTRY_CMD ?
			BeceemAPI_DecodeAuthState(hAPI->i32AuthenticationState) : NULL;
		if (hAPI->u32CSCMDebugLogLevel >= DIAG_VERBOSE) 
			{
			PrintDiag(DIAG_NORMAL,hAPI, S("Link status = \t ") B_SFNL, bszLinkStatus);
			if (bszEAPStatus != NULL)
				PrintDiag(DIAG_NORMAL, hAPI, B_SFNL, bszEAPStatus);
			}
		else
			{
			if (bszEAPStatus == NULL)
				PrintDiag(DIAG_NORMAL, hAPI, S("(") B_SF S(") Link status = ") B_SFNL, 
					BeceemAPI_DateTime(hAPI), bszLinkStatus);
			else
				PrintDiag(DIAG_NORMAL, hAPI, S("(") B_SF S(") Link status = ") B_SF S(" (") B_SF S(")\n"), 
					BeceemAPI_DateTime(hAPI), bszLinkStatus, bszEAPStatus);
			}

		if (pLinkStatus->u32VendorSpecificInformationLength > 0 && pLinkStatus->u32VendorSpecificInformationLength < 256)
		{
			PrintDiag(DIAG_NORMAL, hAPI, S("\n Vendor specific data \n"));
			for (i=0; i< (int) pLinkStatus->u32VendorSpecificInformationLength; i++)
				PrintDiag(DIAG_NORMAL, hAPI, S("0x%02x "),pLinkStatus->au8VendorSpecificInformation[i]); 
			PrintDiag(DIAG_NORMAL, hAPI, S("\n"));
		}

		BeceemAPI_Deactivate(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT);
		BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_DEREG_TIMEOUT);		// Network dereg request results in a link status message
		bConnected = TRUE;
		bNoSync    = FALSE;
		switch (pLinkStatus->eStatusCode)
			{

			case HOST_INITIATED_SYNC_DOWN:
			case LINK_DOWN_HOST_INITIATED:
			case WAIT_FOR_PHY_SYNC_CMD:
			case PHY_SYNC_ERROR:
				bConnected = FALSE;
				bNoSync    = TRUE;

				// Get ready for the WAIT_FOR_NETWORK_ENTRY_CMD indication 
				if (!(hAPI->eNetworkSearchState >= NsSyncDown1 &&
					  hAPI->eNetworkSearchState <= NsSyncDown2_Wait_2))
					{
					hAPI->bIgnoreWaitNetEntryCmd = FALSE;
					}

				BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT);
				if (hAPI->u32State != STATE_SHUTDOWN && hAPI->u32State != STATE_HMC_SHUTDOWN)
					{
					if (pLinkStatus->eStatusCode == WAIT_FOR_PHY_SYNC_CMD)
						BeceemAPI_SetState(hAPI, STATE_WAIT_PHY_SYNC_CMD);
					else if (pLinkStatus->eStatusCode == HOST_INITIATED_SYNC_DOWN)
						BeceemAPI_ConnectionError(hAPI, S("Host initiated sync down"));
					else
						BeceemAPI_ConnectionError(hAPI, S("No Sync"));
					}
				break;

			case WAIT_FOR_NETWORK_ENTRY_CMD:
				bConnected = FALSE;
				//	BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT);
				if (hAPI->u32State != STATE_NET_ENTRY_DONE || 
					hAPI->u32LastLinkStatus == WAIT_FOR_NETWORK_ENTRY_CMD)
					{
					BeceemAPI_SetState(hAPI, STATE_WAIT_NET_ENTRY_CMD);
					u32IgnoreWaitNetEntryCmd = hAPI->bIgnoreWaitNetEntryCmd ? 1 : 0;
					i32AuthState = hAPI->i32AuthenticationState;
					if ((!hAPI->bIgnoreWaitNetEntryCmd || 
						BeceemAPI_IsActivated(hAPI, ID_EVENT_SYNC_UP_TIMEOUT)) && 
						(hAPI->i32AuthenticationState == EAP_AUTH_DISABLED ||
						 hAPI->i32AuthenticationState == EAP_AUTH_WAIT_SUPPLICANT))
						{
						BeceemAPI_NetworkEntryRequest(hAPI);
						hAPI->bIgnoreWaitNetEntryCmd = TRUE;
						}
					}
				BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_UP_TIMEOUT);
				break;

			case NETWORK_ENTRY_IN_PROGRESS:
				bConnected = FALSE;
				BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_UP_TIMEOUT);
				BeceemAPI_SetState(hAPI,STATE_NET_ENTRY_IN_PROG);
				break;

			case LINKUP_ACHIEVED:
				BeceemAPI_Deactivate(hAPI, ID_EVENT_SYNC_UP_TIMEOUT);
				if (hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD || hAPI->u32State == NET_ENTRY_IN_PROGRESS)
					{
					hAPI->u32NetEntryCompletedCount++;
#ifdef LINUX
					if (hAPI->bNetEntryIPRefresh)
						{
						if (hAPI->eIPRefreshState == IpIdle)
							{
							hAPI->eIPRefreshState = IpRefreshRequested;
							PrintDiag(DIAG_NORMAL, hAPI, S("IP Refresh state changed to 'IpRefreshRequested'\n"));
							}
						else
							PrintDiag(DIAG_ERROR, hAPI, 
								S("***** ERROR: IP Refresh requested with invalid eIPRefreshState = %d\n"), 
								hAPI->eIPRefreshState);
						}
#endif
					}
				if (pLinkStatus->BsInfoIsValidFlag) 
					{
					BeceemAPI_SetState(hAPI, STATE_NET_ENTRY_DONE);
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
					}

#ifdef CALL_OMA_CLI
				PrintDiag( DIAG_NORMAL, hAPI, S("========LINKUP_ACHIEVED=============\n"));
				if(oma_start_flag == 0){
					call_OMA_cli(7549, 5);
					oma_start_flag = 1;
				}
#endif
				break;

			case IN_IDLE_MODE:
				BeceemAPI_SetState(hAPI, STATE_IDLE);
				break;

			case MAC_SYNC_ERROR: /* no dl map , dcd, ucd etc. */
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("MAC sync error"));
				hAPI->bIgnoreWaitNetEntryCmd = FALSE;
				break;

			case RANGING_FAILURE:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("Ranging failure"));
				break;

			case SBC_FAILURE:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("SBC failure"));
				break;

			case PKM_FAILURE:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("PKM failure"));
				if (BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT))
					{
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
					BeceemAPI_Activate (hAPI, ID_EVENT_NET_ENTRY_FAILURE);
					hAPI->u32NetEntryPKMCount++;
					}
				break;

			case SATEK_FAILURE:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("SATEK failure"));
				if (BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT))
					{
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
					BeceemAPI_Activate (hAPI, ID_EVENT_NET_ENTRY_FAILURE);
					hAPI->u32NetEntrySATEKCount++;
					}
				break;

			case DREG_RECEIVED:
				bConnected = FALSE;
				if (BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT))
					{
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
					BeceemAPI_Activate (hAPI, ID_EVENT_NET_ENTRY_FAILURE);
					hAPI->u32NetEntryDeregCount++;
					}
				else
					hAPI->u32OtherDeregRcvdCount++;
				BeceemAPI_ConnectionError(hAPI, S("DEREG received from BS"));
				break;

			case REGISTRATION_FAILURE:
				bConnected = FALSE;
				if (BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT))
					{
					BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
					BeceemAPI_Activate (hAPI, ID_EVENT_NET_ENTRY_FAILURE);
					hAPI->u32NetEntryRegCount++;
					}
				BeceemAPI_ConnectionError(hAPI, S("Registration failure"));
				break;

			case RESET_RECEIVED:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("Reset received"));
				break;

			case IN_SLEEP_MODE:
				BeceemAPI_SetState(hAPI, STATE_SLEEP);
				break;

			case SHUTDOWN_RECEIVED_CMD:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("Shutdown command received"));
				break;

			case SHUTDOWN_RESYNC_RECEIVED:
				bConnected = FALSE;
				BeceemAPI_ConnectionError(hAPI, S("Shutdown resync received"));
				break;
			
			case LINK_STATUS_NORMAL:
			case INVALID_REASON_CODE_RECEIVED:
			case MAC_VERSION_SWITCH:
			case IDLE_MODE_FAILURE_CODE_RECEIVED:
			case LOCATION_UPDATE_WITH_POWER_DOWN_SUCCESS_RECEIVED:
				break;

			default:
				PrintDiag(DIAG_ERROR, hAPI, S("PROGRAM ERROR: Unknown link status code = %d\n"), pLinkStatus->eStatusCode);

			}

#ifdef LINUX
		// Kill DHCP on disconnect
		if (u32LastState == STATE_NET_ENTRY_DONE && 
			!(hAPI->u32State == STATE_NET_ENTRY_DONE || 
              hAPI->u32State == STATE_IDLE           || 
			  hAPI->u32State == STATE_LEAVING_IDLE   ||
			  // TO DO: Should sleep be included here? 
			  hAPI->u32State == STATE_SLEEP))
			{
			if (hAPI->bTerminateDHCPClient)
				BeceemAPI_IPRefreshKillProcess(hAPI);
			}
#endif

		// Advance network search state
		// Note: The NsSyncDown2 states are needed to work around a USB driver bug. These
		//       should be removed once the bug is fixed.
		switch (hAPI->eNetworkSearchState)
			{
			case NsSyncDown1:        
				if (bNoSync)
					BeceemAPI_NetworkSearchRequest(hAPI);
				else
					{
					BeceemAPI_SyncDownRequest(hAPI);
					}
				break;
			default: break;
			}

		// Display baseband status
		WcmMutex_Wait(hAPI->hMutexStatusResponse);
		BeceemAPI_DisplayBasebandStatus(hAPI, bConnected, S("Link"), bszLinkStatus, bszEAPStatus);
		if (pLinkStatus->BsInfoIsValidFlag)
			{
			Print_Newline(hAPI->hPrintConStat);
			BeceemAPI_DisplayBaseInfo(hAPI, hAPI->hPrintConStat, &pLinkStatus->BsInfo);
			}
		WcmMutex_Release(hAPI->hMutexStatusResponse);

		hAPI->u32LastLinkStatus = pLinkStatus->eStatusCode;
			
		}

	// Issue warning and error messages 
	if (bInvalidStateTransition)
		PrintDiag(DIAG_NORMAL,hAPI, S("PROGRAM ERROR: Invalid state transition from state ") B_SF S("\nSync status = ") B_SF S("\nLink status = ") B_SFNL,
			BeceemAPI_DecodeState(hAPI), bszSyncStatus, bszLinkStatus);

} /* BeceemAPI_NetEntryExitResponse */

// -----------------------------------------------------------------------------------------
static void BeceemAPI_IdleModeResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	
	STRING bszIdleModeStatus = NULL;

    if(!pWiMAXMsg)
        return;
	
    if(pWiMAXMsg->usType != TYPE_IDLE_MODE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("Idle mode response length = %d\n"), u32MsgLength);
		
	switch (pWiMAXMsg->usSubType)
		{
		case SUBTYPE_IDLE_MODE_INDICATION:
			{
			UINT32 u32IdleModePeriod = *((UINT32 *) pWiMAXMsg->szData);
			if (u32IdleModePeriod == 0) 
				{
				bszIdleModeStatus = S("Not in idle mode");
				BeceemAPI_SetState(hAPI, STATE_LEAVING_IDLE);
				} 
			else
				{
				//PrintDiag(DIAG_NORMAL,hAPI, S("Entering idle mode for %d ms.\n"), u32IdleModePeriod);
				bszIdleModeStatus = S("Entering idle mode");
				BeceemAPI_SetState(hAPI, STATE_IDLE);
				}

			// Reset the IP refresh state machine
			hAPI->eIPRefreshState = IpIdle;
			} 
			break;
		case SUBTYPE_IDLE_MODE_STATUS_RESP:
			{
			UINT32 u32InIdleMode = *((UINT32 *) pWiMAXMsg->szData);
			if (u32InIdleMode == 0) 
				{
				bszIdleModeStatus = S("Not in idle mode");
				BeceemAPI_SetState(hAPI, STATE_LEAVING_IDLE);
				} 
			else
				{
				bszIdleModeStatus = S("In idle mode");
				BeceemAPI_SetState(hAPI, STATE_IDLE);
				}
			BeceemAPI_Deactivate(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT);
			}
			break;

		// Not used by connection manager
		case SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP:
			break;

		case SUBTYPE_IDLE_MODE_WAKEUP_RESP:
			bszIdleModeStatus = S("Waking up from idle mode");
			break;

		default:
			break;
		}

	// Report status
	WcmMutex_Wait(hAPI->hMutexStatusResponse);
	if (bszIdleModeStatus != NULL)
		{
		PrintDiag(DIAG_NORMAL, hAPI, B_SF S(".\n"), bszIdleModeStatus);
		BeceemAPI_DisplayBasebandStatus(hAPI, TRUE, S("Idle mode"), bszIdleModeStatus, NULL);
		}
	else
		{
		PrintDiag(DIAG_NORMAL, hAPI, S("\n"));
		BeceemAPI_DisplayBasebandStatus(hAPI, TRUE, S("Idle mode"), S("Unknown"), NULL);
		}
	WcmMutex_Release(hAPI->hMutexStatusResponse);

} // IdleModeResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_SleepModeResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	
	UINT32 n;

	ST_SLEEPMODE_MSG *pstSleepModeMsg;

    if(!pWiMAXMsg)
        return;
	
    if (pWiMAXMsg->usType != TYPE_SLEEP_MODE || pWiMAXMsg->usSubType != SUBTYPE_SLEEP_MODE_RESP)
		return;

	pstSleepModeMsg = (ST_SLEEPMODE_MSG *) pWiMAXMsg->szData;

	PrintDiag(DIAG_NORMAL,hAPI,S("Sleep: Number of sleep definitions = %u\n"), pstSleepModeMsg->numOfDefinitions);
	for (n=0; n < pstSleepModeMsg->numOfDefinitions; n++)
		PrintDiag(DIAG_NORMAL,hAPI,S("Sleep[n]: Final sleep window base = %u\n"),
			n, (UINT32) pstSleepModeMsg->PSC_Defn[n].u16FinalSleepWindowBase);

} // SleepModeResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_ShutdownResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	
	UINT32 u32IsShutdown;
	UINT32 u32RetCode;
	STRING bszShutdownModeStatus = EMPTY_STRING;

    if(!pWiMAXMsg)
        return;
	
    if(pWiMAXMsg->usType != TYPE_SHUTDOWN)  
        return;

	// While in shutdown, don't issue link status requests, instead
	// issue shutdown mode requests
	switch (pWiMAXMsg->usSubType)
		{
		case SUBTYPE_SHUTDOWN_RESP:
			// 0 = shutdown succeeded
			BeceemAPI_Deactivate(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT);
			u32RetCode = *((UINT32 *) pWiMAXMsg->szData);
			PrintDiag(DIAG_NORMAL, hAPI, S("SUBTYPE_SHUTDOWN_RESP return value = %08x\n"), u32RetCode);
			u32IsShutdown = u32RetCode == SUCCESS;
			break;

		case SUBTYPE_SHUTDOWN_REQ_FROM_FIRMWARE:
			// Firmware has entered shutdown mode
			PrintDiag(DIAG_NORMAL, hAPI, S("SUBTYPE_SHUTDOWN_REQ_FROM_FIRMWARE\n"));
			u32IsShutdown = TRUE;
			break;

		case SUBTYPE_SHUTDOWN_MODE_STATUS_RESP:
			// 0 = shutdown succeeded
			BeceemAPI_Deactivate(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT);
			u32RetCode = *((UINT32 *) pWiMAXMsg->szData);
			PrintDiag(DIAG_NORMAL, hAPI, S("SUBTYPE_SHUTDOWN_MODE_STATUS_RESP return value = %08x\n"), u32RetCode);
			u32IsShutdown = u32RetCode == SUCCESS;
			BeceemAPI_Deactivate(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT);
			break;

		case SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND:
			PrintDiag(DIAG_NORMAL, hAPI, S("SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND\n"));
			u32IsShutdown = TRUE;
			break;

		case SUBTYPE_SHUTDOWN_MODE_COMPLETE_WAKE_UP_IND:
			PrintDiag(DIAG_NORMAL, hAPI, S("SUBTYPE_SHUTDOWN_MODE_COMPLETE_WAKE_UP_IND\n"));
			u32IsShutdown = FALSE;
			break;

		default:
			PrintDiag(DIAG_NORMAL, hAPI, S("Unknown shutdown indication sub-type = %d\n"), pWiMAXMsg->usSubType);
			u32IsShutdown = FALSE;
			break;
		}

	if (u32IsShutdown) {
		BeceemAPI_SetState(hAPI, STATE_SHUTDOWN);
		bszShutdownModeStatus = S("Device is shut down");
		}

	else
		{
		BeceemAPI_SetState(hAPI, STATE_LEAVING_SHUTDOWN);
		bszShutdownModeStatus = S("Device is not shut down");
		}
		
	WcmMutex_Wait(hAPI->hMutexStatusResponse);
	BeceemAPI_DisplayBasebandStatus(hAPI, FALSE, S("Shutdown mode"), bszShutdownModeStatus, NULL);
	WcmMutex_Release(hAPI->hMutexStatusResponse);

} // ShutdownResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_ServiceFlowResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg)
	{

	stLocalSFAddIndication    *pstSFAdd; // Used for Add and Change
	stLocalSFDeleteIndication *pstSFDel; // Used for Delete 
	STRING bszSubType = EMPTY_STRING;

    if(!pWiMAXMsg)
        return;

	if(pWiMAXMsg->usType != TYPE_SERVICE_FLOW)
        return;

	pstSFAdd = (stLocalSFAddIndication    *) pWiMAXMsg->szData;
	pstSFDel = (stLocalSFDeleteIndication *) pWiMAXMsg->szData;

	switch (pWiMAXMsg->usSubType)
		{
		case SUBTYPE_SF_ADD_INDICATION:    bszSubType = S("Add"); break;
		case SUBTYPE_SF_CHANGE_INDICATION: bszSubType = S("Change"); break;
		case SUBTYPE_SF_DELETE_INDICATION: bszSubType = S("Delete"); break;
		default:						   bszSubType = S("Unknown"); break;
		}

	PrintDiag(DIAG_NORMAL,hAPI, S("Service Flow ") B_SF S(" Indication:\n"), bszSubType);

	switch (pWiMAXMsg->usSubType)
		{
		case SUBTYPE_SF_ADD_INDICATION:  
		case SUBTYPE_SF_CHANGE_INDICATION:
			PrintDiag(DIAG_NORMAL,hAPI, S("Type               = %lu\n"), (UINT32) pstSFAdd->u8Type);
			PrintDiag(DIAG_NORMAL,hAPI, S("Direction          = ") B_SFNL, 
				(UINT32) pstSFAdd->eConnectionDir == 0 ? S("Downlink") : S("Uplink"));
			PrintDiag(DIAG_NORMAL,hAPI, S("Connection ID      = %lu\n"), (UINT32) pstSFAdd->u16CID);
			PrintDiag(DIAG_NORMAL,hAPI, S("Virtual CID        = %lu\n"), (UINT32) pstSFAdd->u16VCID);
			PrintDiag(DIAG_NORMAL,hAPI, S("Service flow ID    = %lu\n"),
				(UINT32) pstSFAdd->sfActiveSet.u32SFID);
			PrintDiag(DIAG_NORMAL,hAPI, S("Max sustained rate = %lu kbps\n"),  
				(UINT32) pstSFAdd->sfActiveSet.u32MaxSustainedTrafficRate/1000);
			PrintDiag(DIAG_NORMAL,hAPI, S("Traffic priority   = %lu\n"),
				(UINT32) pstSFAdd->sfActiveSet.u8TrafficPriority);
			break;
		case SUBTYPE_SF_DELETE_INDICATION:
			PrintDiag(DIAG_NORMAL,hAPI, S("Type               = %lu\n"), (UINT32) pstSFDel->u8Type);
			PrintDiag(DIAG_NORMAL,hAPI, S("Connection ID      = %lu\n"), (UINT32) pstSFDel->u16CID);
			PrintDiag(DIAG_NORMAL,hAPI, S("Virtual CID        = %lu\n"), (UINT32) pstSFDel->u16VCID);
			break;
		default:
			break;
		}

	// Start IP Refresh
	if (pWiMAXMsg->usSubType == SUBTYPE_SF_ADD_INDICATION)
		switch (hAPI->eIPRefreshState)
			{
			case IpIdle:
				break;
			case IpRefreshRequested:		
				hAPI->eIPRefreshState = pstSFAdd->eConnectionDir == 0 ? IpSfAddDL : IpSfAddUL;
				break;
			case IpSfAddUL:
			case IpSfAddDL:
				// Previously saw UL add and now getting a DL add
				if ((hAPI->eIPRefreshState = IpSfAddUL && pstSFAdd->eConnectionDir == 0) ||
				// or previously saw DL add and now getting UL add
					(hAPI->eIPRefreshState = IpSfAddDL && pstSFAdd->eConnectionDir != 0))
					{
					// IP refresh runs when we've seen at least one DL SF add and one UL SF add
					// These first two UL/DL may not be able to carry DHCP traffic, so one or 
					// more DHCP packets may be lost while the other SF's are added.
#ifndef OSX
					BeceemAPI_RefreshIPAddress(hAPI);
#endif
					hAPI->eIPRefreshState = IpIdle;
					}
				// else 
				//	{Leave IP refresh state unchanged}
				break;
			default:
				PrintDiag(DIAG_ERROR, hAPI, S("***** ERROR: Invalid IP Refresh state = %d\n"), hAPI->eIPRefreshState);
				break;
			}

	} // ServiceFlowResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_SSInfoResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg, UINT32 u32MsgLength)
{
	PSUBSCRIBER_STATION_INFO pSSInfo;
	UINT32 u32ActualSize;
	INT32  ai32VersionID[3];
	STRING bszCalStatus = EMPTY_STRING;
	UINT8  u8Calibrated;

    if(!pWiMAXMsg)
        return;
	
	// Other sub-type: SUBTYPE_SS_EEPROM_INFO_RESP
    if(pWiMAXMsg->usType != TYPE_SS_INFO  || pWiMAXMsg->usSubType != SUBTYPE_SS_INFO_INDICATION )
        return;

	pSSInfo = (PSUBSCRIBER_STATION_INFO) pWiMAXMsg->szData;

	// ------------------------------------------
	// Build version strings
	// ------------------------------------------
	StrPrintf(hAPI->bszActualFWVersion, BCHARSOF(hAPI->bszActualFWVersion), S("%d.%d.%d"), 
		pSSInfo->u32FirmwareVersion[0],
		pSSInfo->u32FirmwareVersion[1],
		pSSInfo->u32FirmwareVersion[2]);
	StrPrintf(hAPI->bszActualDLLVersion, BCHARSOF(hAPI->bszActualDLLVersion), S("%d.%d.%d"), 
		pSSInfo->u32LibraryVersion[0],
		pSSInfo->u32LibraryVersion[1],
		pSSInfo->u32LibraryVersion[2]);
	b_mbstobcs(hAPI->bszActualDriverVersion, (char *) pSSInfo->uchDriverVersion, BCHARSOF(hAPI->bszActualDriverVersion));

	// ---------------------------------------------------------
	// Print the information
	// ---------------------------------------------------------

	ServerSocketBeginPrint(hAPI->hPrintVersions);
	
	//PrintVersions(S("\n~~~~~~~~~~~~~~ SS Info Response ~~~~~~~~~~~~~~~~~~\n\n"));
	PrintVersions(hAPI, S("\n\n"));
	PrintVersions(hAPI, S("CM Version\t\t ") B_SFNL, CSCM_PACKAGE_VERSION);
	PrintVersions(hAPI, S("Firmware Version\t ") B_SFNL, hAPI->bszActualFWVersion);
	PrintVersions(hAPI, S("Driver Version\t\t ") B_NSFNL, (char *) pSSInfo->uchDriverVersion );
	PrintVersions(hAPI, S("Library Version\t\t ") B_SFNL, hAPI->bszActualDLLVersion);
	PrintVersions(hAPI, S("Baseband Chip Version\t %x\n"), pSSInfo->BasebandChipVersion);
	PrintVersions(hAPI, S("RF Chip Version\t\t %x\n"), pSSInfo->RFChipVersion);
	PrintVersions(hAPI, S("Device Data Version\t %02x.%02x\n"), 
		pSSInfo->u32EEPROMVersion[0], pSSInfo->u32EEPROMVersion[1]);
	PrintVersions(hAPI, S("MAC Address\t\t %02x-%02x-%02x"),
												pSSInfo->u8MacAddress[0],
												pSSInfo->u8MacAddress[1],
												pSSInfo->u8MacAddress[2]);
	PrintVersions(hAPI, S("-%02x-%02x-%02x\n"),	pSSInfo->u8MacAddress[3],
												pSSInfo->u8MacAddress[4],
												pSSInfo->u8MacAddress[5]);

	u8Calibrated = pSSInfo->u8Calibrated;
	if (u8Calibrated == NOT_CALIBRATED)
		bszCalStatus = S("No");
	else if (u8Calibrated == CALIBRATION_STATUS_NOT_KNOWN)
		bszCalStatus = S("Unknown");
	else if (u8Calibrated & 0x1)
		bszCalStatus = S("Yes");
	PrintVersions(hAPI, S("Calibrated\t\t ") B_SF, bszCalStatus);

	if (u8Calibrated & 0xE)
		{
		PrintVersions(hAPI, S(" - Band(s)"));
		if (u8Calibrated & 2)
			PrintVersions(hAPI, S(" 0"));
		if (u8Calibrated & 4)
			PrintVersions(hAPI, S(" 1"));
		if (u8Calibrated & 8)
			PrintVersions(hAPI, S(" 2"));
		}
		
	PrintVersions(hAPI, S("\n"));

	//PrintVersions(S("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"));

	ServerSocketEndPrint(hAPI, hAPI->hPrintVersions, ID_MSG_VERSIONS);

	// ------------------------------------------------------
	// Runtime check for correct library and firmware version
	// -------------------------------------------------------

	hAPI->bValidFWVersion = TRUE;
	memset(ai32VersionID, 0, sizeof(ai32VersionID));
	if (!ParseVersionString(hAPI->bszActualFWVersion, ai32VersionID, 3, &u32ActualSize) || u32ActualSize != 3)
		hAPI->bValidFWVersion = FALSE;
	else if (ai32VersionID[0] < 0 || ai32VersionID[1] < 0 || ai32VersionID[2] < 0)
		hAPI->bValidFWVersion = FALSE;

	hAPI->bValidDLLVersion  = 
		b_strncmp(TARGET_DLL_VERSION, hAPI->bszActualDLLVersion, DLLVERSION_LEN) == 0 ||
		b_strncmp(TARGET_DLL_VERSION_ALT, hAPI->bszActualDLLVersion, DLLVERSION_LEN);

	PrintDiag(DIAG_NORMAL,hAPI, S("Found ") B_SF S(" library version: ") B_SFNL, 
		hAPI->bValidDLLVersion ? S("valid") : S("invalid"), hAPI->bszActualDLLVersion);
		
} /* BeceemAPI_SSInfoResponse */

// -----------------------------------------------------------------------------------------
static void BeceemAPI_MIBResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMAXMessage, UINT32 u32MsgLength)
{

	BOOL bResetStats; // Make a local copy 
	PMIB_OBJECT_INFO pMIBObjInfo = 0;
	PMIB_OBJECTS pMIBObjs=0;
	UINT8 * pBaseAddr = 0;
	UINT32 u32MibID, u32MibType, u32Value, u32Size, n, nb;
	UINT32 u32NumberOfFramesReceived = 0, u32NumberOfFramesInError = 0;
	STRING bszModemState;
	B_CHAR  bszBuffer[50];
	int iLen = 0;
	UINT32 u32RptIdx;
	INT32 i32MaxLen = u32MsgLength - sizeof(pstWiMAXMessage->usType) - sizeof(pstWiMAXMessage->usSubType);
	hMibData_t hStatisticsReport = NULL;
	BOOL abUseRpt[MAX_STATS_REPORT];
	BOOL bPrintEnabled = TRUE;

	// We ignore SUBTYPE_MIB_SET_RESP, SUBTYPE_MIB_GET_RESP, and SUBTYPE_MULTIPLE_MIB_SET_RESP
	if (pstWiMAXMessage->usSubType != SUBTYPE_MULTIPLE_MIB_GET_RESP)
		{
		// Kannon uses single MIB gets, so this is normal
		// PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Invalid MIB message sub-type.\n"));
		return;
		}

#define MIB_OBJ_LEN(pMIBObjInfo) \
	(pMIBObjInfo->u32MibObjSize == 0 ? sizeof(MIB_OBJECT_INFO) : (sizeof(MIB_OBJECT_INFO) + pMIBObjInfo->u32MibObjSize - 1))

	// ------------------------------
	// Find the matching stat reports
	// ------------------------------

	pBaseAddr = pstWiMAXMessage->szData;
	iLen = sizeof(UINT16);

	for (u32RptIdx = 0; u32RptIdx < MAX_STATS_REPORT; u32RptIdx++)
		{

		abUseRpt[u32RptIdx] = FALSE;
		n = 0;
		hStatisticsReport = hAPI->hLinkStatsReport[u32RptIdx].hStatisticsReport;	

		// Loop over the returned MIBS, looking for matches with a report
		for (; iLen < i32MaxLen && hStatisticsReport[n].i32MibID != 0; iLen += MIB_OBJ_LEN(pMIBObjInfo)) 
			{

			pMIBObjInfo = (PMIB_OBJECT_INFO) (pBaseAddr + iLen);

			// Skip all non-matching report records, including heading records
			while (!(hStatisticsReport[n].i32MibID == (INT32) pMIBObjInfo->u32MibObjId ||
				     hStatisticsReport[n].i32MibID == 0))
				{
				n++;
				}

			if (hStatisticsReport[n].i32MibID != 0)
				{
				// We found a match for this report
				abUseRpt[u32RptIdx] = TRUE;
				n++;
				}
			}

		} // for (u32RptIdx = 0; ...
		
	// ----------------------------------------------------
	// Generate statistics report
	// ----------------------------------------------------

	// Initialize the statistics report print context
	ServerSocketBeginPrint(hAPI->hPrintStats);
	PrintStats(hAPI, S("\n\n"));

	pMIBObjs  = (PMIB_OBJECTS)pstWiMAXMessage->szData;
	iLen      = sizeof(UINT16);
	pBaseAddr = pstWiMAXMessage->szData;

	for (u32RptIdx=0; u32RptIdx < MAX_STATS_REPORT && iLen < i32MaxLen && bPrintEnabled; u32RptIdx++)
		{

		// Skip any unused reports
		if (!abUseRpt[u32RptIdx])
			continue;

		hStatisticsReport = hAPI->hLinkStatsReport[u32RptIdx].hStatisticsReport;
		bResetStats       = hAPI->hLinkStatsReport[u32RptIdx].bBBStatsOffset;
		hAPI->hLinkStatsReport[u32RptIdx].bBBStatsOffset = FALSE;

		// Loop through the entire report
		for (n=0; iLen < i32MaxLen && hStatisticsReport[n].i32MibID != 0 && bPrintEnabled; n++)
			{

			// Print descriptions, including headings
			PrintStats(hAPI, B_SF S("\t "), hStatisticsReport[n].bszMIBDescription);
			if (hStatisticsReport[n].u32MibType == MIB_DATA_TYPE_HEADING)	
				{
				PrintStats(hAPI, S("\n"));
				continue;
				}

			// Check object ID
			pMIBObjInfo = (PMIB_OBJECT_INFO)(pBaseAddr + iLen);
			u32MibID    = (UINT32) hStatisticsReport[n].i32MibID;
			u32MibType  = hStatisticsReport[n].u32MibType;

			if (u32MibID != pMIBObjInfo->u32MibObjId)
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR in MIB ID: Expected ID=%lu - found ID=%lu\n"),
					u32MibID, pMIBObjInfo->u32MibObjId);
				PrintDiag(DIAG_ERROR, hAPI, S("      Searching for a match to this MIB ID.\n"));
				}

			// Skip all non-matching report records, including heading records
			while (!(u32MibID == (INT32) pMIBObjInfo->u32MibObjId || u32MibID == 0))
				{
				n++;
				u32MibID    = (UINT32) hStatisticsReport[n].i32MibID;
				u32MibType  = hStatisticsReport[n].u32MibType;
				}

			// If we didn't find a match inside this report, go on to the next report
			if (u32MibID == 0)
				{
				continue;
				}
	
			// ------------------------------------------------------------
			// Capture offset values on user Reset
			// ------------------------------------------------------------

			u32Value = * ((UINT32*) pMIBObjInfo->au8MibObjVal); 
			u32Size  = pMIBObjInfo->u32MibObjSize;
			switch(u32MibID)
				{
				case BCM_STATS_NUMFRAMESRECVD:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfFramesRecvd = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfFramesRecvd;
					u32NumberOfFramesReceived = u32Value;
					break;
				case BCM_STATS_NUMFRAMESINERR:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfFramesInError = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfFramesInError;
					u32NumberOfFramesInError = u32Value;
					break;
				case BCM_STATS_NUMCRCERR:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfCrcErrors = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfCrcErrors;
					break;
				case BCM_STATS_NUMHCSERR:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfHcsErrors = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfHcsErrors;
					break;
				case BCM_STATS_NUMBYTES_FROM_HOST:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBytesFromHost = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBytesFromHost;
					break;
				case BCM_STATS_NUMBYTES_TO_HOST:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBytesToHost = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBytesToHost;
					break;
				case BCM_BASEBAND_TX_PACKETS_HISTOGRAM:
					u32Value = ArraySumUint32((UINT32 *) pMIBObjInfo->au8MibObjVal, 
						                                 pMIBObjInfo->u32MibObjSize/sizeof(UINT32));
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfPacketsFromHost = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfPacketsFromHost;
					u32MibType = MIB_DATA_TYPE_UINT32;
					u32Size    = 4;
					break;	
				case BCM_BASEBAND_RX_PACKETS_HISTOGRAM:
					u32Value = ArraySumUint32((UINT32 *) pMIBObjInfo->au8MibObjVal, 
						                                 pMIBObjInfo->u32MibObjSize/sizeof(UINT32));
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfPacketsToHost = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfPacketsToHost;
					u32MibType = MIB_DATA_TYPE_UINT32;
					u32Size    = 4;
					break;
				case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP6:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep6 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep6;
					break;
				case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP4:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep4 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep4;
					break;
				case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP2:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep2 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep2;
					break;
				case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP1:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep1 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRcvQPSK12CTCBurstsRep1;
					break;
				case BCM_STATS_BURSTRCVQPSK12:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcvQPSK12 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcvQPSK12;
					break;
				case BCM_STATS_BURSTRCVQPSK34:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcvQPSK34 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcvQPSK34;
					break;
				case BCM_STATS_BURSTRCV16QAM12:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcv16QAM12 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcv16QAM12;
					break;
				case BCM_STATS_BURSTRCV16QAM34:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcv16QAM34 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcv16QAM34;
					break;
				case BCM_STATS_BURSTRCV64QAM23:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM23 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM23;
					break;
				case BCM_STATS_BURSTRCV64QAM34:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM34 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM34;
					break;
				case BCM_STATS_BURSTRCV64QAM56:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM56 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstRcv64QAM56;
					break;
				case BCM_STATS_BURSTSNDQPSK12:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstSndQPSK12 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstSndQPSK12;
					break;
				case BCM_STATS_BURSTSNDQPSK34:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstSndQPSK34 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstSndQPSK34;
					break;
				case BCM_STATS_BURSTSND16QAM12:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstSnd16QAM12 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstSnd16QAM12;
					break;
				case BCM_STATS_BURSTSND16QAM34:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfBurstSnd16QAM34 = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfBurstSnd16QAM34;
					break;
				case BCM_BASEBAND_NUMOF_TX_BE_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfTxBEBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfTxBEBytes;
					break;
				case BCM_BASEBAND_NUMOF_RX_BE_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRxBEBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRxBEBytes;
					break;
				case BCM_BASEBAND_NUMOF_TX_UGS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfTxUGSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfTxUGSBytes;
					break;
				case BCM_BASEBAND_NUMOF_RX_UGS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRxUGSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRxUGSBytes;
					break;
				case BCM_BASEBAND_NUMOF_TX_RTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfTxRTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfTxRTPSBytes;
					break;
				case BCM_BASEBAND_NUMOF_RX_RTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRxRTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRxRTPSBytes;
					break;
				case BCM_BASEBAND_NUMOF_TX_ERTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfTxERTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfTxERTPSBytes;
					break;
				case BCM_BASEBAND_NUMOF_RX_ERTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRxERTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRxERTPSBytes;
					break;
				case BCM_BASEBAND_NUMOF_TX_NRTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfTxNRTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfTxNRTPSBytes;
					break;
				case BCM_BASEBAND_NUMOF_RX_NRTPS_BYTES:
					if (bResetStats)
						hAPI->hBBStatsOffset->u32NumOfRxNRTPSBytes = u32Value;
					u32Value -= hAPI->hBBStatsOffset->u32NumOfRxNRTPSBytes;
					break;

				default:
					break;

				} // switch (u32MibID)

			// ------------------------------------------------------------
			// Decode and print the values
			// ------------------------------------------------------------

			if (u32MibID == BCM_STATS_MODEMSTATE)
				{
				switch (u32Value)
					{
					case 0: bszModemState = S("Waiting for sync"); break;
					case 1: bszModemState = S("PHY sync achieved"); break;
					case 2: bszModemState = S("Waiting for RNG response"); break;
					case 3: bszModemState = S("Waiting for SBC response"); break;
					case 4: bszModemState = S("Waiting for REG response"); break;
					case 5: bszModemState = S("Waiting for DSA ACK"); break;
					case 6: bszModemState = S("Network entry completed"); break;
					case 7: bszModemState = S("Deregistration request received from BS"); break;
					default: bszModemState = StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
								 S("Unknown modem state (%lu)"), u32Value); 
						break;
					}
				PrintStats(hAPI, B_SFNL, bszModemState);

				// Don't print rest of report if we don't have sync
				// if basic was requested
				if (u32Value == 0 && bPrintEnabled)
					{
					int iRpt;
					bPrintEnabled = FALSE;
					for (iRpt=0; iRpt < MAX_STATS_REPORT; iRpt++)
						{
						if (iRpt != DEFAULT_STATS_REPORT_INDEX && abUseRpt[iRpt])
							bPrintEnabled = TRUE;
						}
					}
				}
			else if (u32MibID == BCM_STATS_CURRENT_RSSI)
				PrintStats(hAPI, S("%ld\n"), (*((INT32 *) pMIBObjInfo->au8MibObjVal)) / 4);

			else if (u32Size == 1 && u32MibType == MIB_DATA_TYPE_UINT8)
				PrintStats(hAPI, S("%ld\n"), (UINT32) *((UINT8 *) pMIBObjInfo->au8MibObjVal));	

			// Because char is unsigned on some systems, we must explicitly use 'signed char'
			else if (u32Size == 1 && u32MibType == MIB_DATA_TYPE_SINT8)
				PrintStats(hAPI, S("%ld\n"), (INT32) *((signed char *) pMIBObjInfo->au8MibObjVal));				

			else if (u32Size == 4 && u32MibType == MIB_DATA_TYPE_UINT32)
				{
				PrintStats(hAPI, S("%lu\n"), u32Value);
				if (u32MibID == BCM_STATS_NUMFRAMESINERR)
					{
					double dFER = u32NumberOfFramesReceived > 0 ?
						(double) u32NumberOfFramesInError/ ((double) u32NumberOfFramesReceived) : 0.0;
					PrintStats(hAPI, S("Frame error rate \t %0.4lf"), dFER);
					}
				}

			else if (u32Size == 4 && u32MibType == MIB_DATA_TYPE_INT32)
				PrintStats(hAPI, S("%ld\n"), *((INT32 *) pMIBObjInfo->au8MibObjVal));

			else if (u32Size > 0)
				{
				for (nb = 0; nb < pMIBObjInfo->u32MibObjSize; nb++)
					{
					if (nb > 0)
						PrintStats(hAPI, S(":"));
					PrintStats(hAPI, S("%02X"), pMIBObjInfo->au8MibObjVal[nb]);
					}
				PrintStats(hAPI, S("\n"));
				if (u32Size != 6)
					PrintDiag(DIAG_NORMAL, hAPI, S("WARNING: MIB ID = %lu has object size = %lu\n"),
						u32MibID, u32Size);
				}

			else 
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Invalid MIB type for ID=%lu\n"), u32MibID);
				break;
				}

			// Bump the pointer
			iLen += MIB_OBJ_LEN(pMIBObjInfo);

			} // for (n=0; iLen < i32MaxLen && ...

			PrintStats(hAPI, S("\n"));

		} // for (u32RptIdx=0; u32RptIdx < MAX_STATS_REPORT && ...

	ServerSocketEndPrint(hAPI, hAPI->hPrintStats, bResetStats ? ID_MSG_RESET_LINK_STATS : ID_MSG_LINK_STATS);

	if (bPrintEnabled && iLen < i32MaxLen)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Excess MIB data received: iLen = %d - iMaxLen = %d.\n"), 
			iLen, i32MaxLen);
		}

	// ---------------------------------------------------
	// for DIAG_DEBUG: Dump the entire MIB payload
	// ---------------------------------------------------

	iLen = sizeof(UINT16);
	if (hAPI->u32CSCMDebugLogLevel >= DIAG_DEBUG)
		{
		int i, j;
		for (j = 0; j < pMIBObjs->u16NumMibObjs; j++)
			{
			
			pMIBObjInfo = (PMIB_OBJECT_INFO) (pBaseAddr + iLen);
			PrintDiag(DIAG_VERBOSE, hAPI, S("\nMIB OID - %d: "), pMIBObjInfo->u32MibObjId);
			if (pMIBObjInfo->s8ErrorCode == 0)
				{
				for (i=0; i<(int)pMIBObjInfo->u32MibObjSize; i++)
					PrintDiag(DIAG_VERBOSE, hAPI, S(" %02X"), pMIBObjInfo->au8MibObjVal[i]);

				if (pMIBObjInfo->u32MibObjSize == 4)
					{
					u32Value = *(UINT32 *)pMIBObjInfo->au8MibObjVal;
					PrintDiag(DIAG_VERBOSE, hAPI, S(" UINT32 = %lu [rev = %lu]"), 
						u32Value, ntohl(u32Value));
					}
				}
			else
				PrintDiag(DIAG_VERBOSE, hAPI, S(" Error Code - (%d)"), pMIBObjInfo->s8ErrorCode);

			// Bump the pointer
			iLen += MIB_OBJ_LEN(pMIBObjInfo);

			} // for (j=0

		PrintDiag(DIAG_VERBOSE, hAPI, S("\n"));

		} // if (

} /* MIB Response */

// -----------------------------------------------------------------------------------------
static void BeceemAPI_NetworkLayerControlResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMAXMessage)
{

	if (pstWiMAXMessage->usSubType == SUBTYPE_IP_REFRESH_INDICATION)
		{

		// Change the state
		if (hAPI->eIPRefreshState == IpIdle)
			{
			hAPI->eIPRefreshState = IpRefreshRequested;
			PrintDiag(DIAG_NORMAL, hAPI, S("IP Refresh state changed to 'IpRefreshRequested'\n"));
			}
		else
			PrintDiag(DIAG_ERROR, hAPI, 
				S("***** ERROR: IP Refresh Indication received with invalid eIPRefreshState = %d\n"), 
				hAPI->eIPRefreshState);
		}
	
	} // NetworkLayerControl (...)

// -----------------------------------------------------------------------------------------
VOID BeceemAPI_MACMgmtMessage(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMAXMsg)
{
	BOOL bError = FALSE;
	PS_MAC_MGMTMESSAGE pMACMgmtInfo;
	UINT32 u32MACMsgLength;
	UINT32 u32NumExtraBytes;
	B_CHAR bszBuffer[25];

    if(!pWiMAXMsg || !hAPI->bMACLoggingEnabled || hAPI->hMACLogFile == NULL)
        return;

	// Ignore SUBTYPE_MACMGMT_GET_STATUS_RESP responses
	if (pWiMAXMsg->usSubType != SUBTYPE_MACMGMT_INDICATION)
		return;

	pMACMgmtInfo     = (PS_MAC_MGMTMESSAGE) pWiMAXMsg->szData; 
	if (!pMACMgmtInfo)
		return;

#define LOG_PREPAD_BYTES 3
	u32MACMsgLength  = sizeof(S_MAC_MGMTMESSAGE) + pMACMgmtInfo->u32LengthOfMessage - 1;
	u32NumExtraBytes = (u32MACMsgLength + LOG_PREPAD_BYTES) % 4;

	// Write the log record to the binary file
	if (!LogFile_WriteBytes(hAPI->hMACLogFile, (UINT8 *)
		DateTimeStamp(bszBuffer, sizeof(bszBuffer)), sizeof(bszBuffer) - 1)) 
		bError = TRUE;
	if (!LogFile_WriteZero(hAPI->hMACLogFile, LOG_PREPAD_BYTES)) 
		bError = TRUE;
	if (LogFile_WriteBytes(hAPI->hMACLogFile, (UINT8 *) pMACMgmtInfo, u32MACMsgLength) != u32MACMsgLength) 
		bError = TRUE;
	if (u32NumExtraBytes > 0 && !LogFile_WriteZero(hAPI->hMACLogFile, 4-u32NumExtraBytes)) 
		bError = TRUE;

	LogFile_Flush(hAPI->hMACLogFile);

	if (bError)
		PrintDiag(DIAG_NORMAL,hAPI, S("\n\nERROR writing MAC management log file.\n\n"));

} // MACMgmtMessage

// -----------------------------------------------------------------------------------------
VOID BeceemAPI_EAPResponse(BeceemAPI_t hAPI, eSourceType eSource, PWIMAX_MESSAGE pstWiMAXMsg)
{
	BOOL bEAPThreadRunning  = FALSE;
	BOOL bIsRequestIdentity = FALSE;
	PEAP_PAYLOAD pstEAPPayload;

	switch(pstWiMAXMsg->usSubType) 
	{
		case SUBTYPE_AUTH_STATUS_RESP:
			break;

		case SUBTYPE_AUTH_PARAM_INDICATION:
			break;

		case SUBTYPE_EAP_TRANSFER_INDICATION:

			pstEAPPayload = (PEAP_PAYLOAD) pstWiMAXMsg->szData;

			if(!pstEAPPayload)
				PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Auth Indication has NULL buffer pointer.\n"));

			else if (pstEAPPayload->usEAPPayloadLength == 0)
				PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Auth Indication has zero length buffer.\n"));

			else {

				PrintDiag(DIAG_NORMAL,hAPI, S("Received Auth EAP Transfer Indication - length = %d\n"),
					pstEAPPayload->usEAPPayloadLength);

				if (!hAPI->bAuthEnabled)
					PrintDiag(DIAG_NORMAL,hAPI, S("Ignored EAP message from BS: Authentication is not enabled in the Options.\n"));
				else if (hAPI->bAuthEthernetToRADIUS && eSource == eFromWiMAXAPI)
					PrintDiag(DIAG_NORMAL,hAPI, S("Ignored EAP message from BS: Using direct Ethernet path to RADIUS server.\n"));
				else
					{

					// Print messages to supplicant
					PrintDiag(DIAG_DEBUG, hAPI, S("EAP msg to supplicant: "));
					StringHexDump(DIAG_DEBUG, hAPI, TRUE, 
						pstEAPPayload->aucEAPPayload, pstEAPPayload->usEAPPayloadLength);

					// Is this a Request-Identity packet?
					if (// Do NOT filter on the packet length
						// byte 0: Request = 1
						pstEAPPayload->aucEAPPayload[0] == 1    &&
						// byte 1 carries the Identifier, which is usually 0
						// bytes 2:3 carry the length
						// byte 4: Identity = 1
						pstEAPPayload->aucEAPPayload[4] == 1)
						bIsRequestIdentity = TRUE;

					// Start the Authentication thread, if it is not already running
					if (BeceemAPI_Authorizing(hAPI))
						bEAPThreadRunning = TRUE;
					else if (bIsRequestIdentity)
						bEAPThreadRunning = BeceemAPI_EAP_StartThread(hAPI);
					else
						bEAPThreadRunning = FALSE;
					
					// Forward the EAP packet to the EAP Supplicant
					if (bEAPThreadRunning && hAPI->bEAPIsOpen)
						{
						hAPI->bAuthExchangeActive = TRUE;
						PrintDiag(DIAG_NORMAL,hAPI, S("Forwarding EAP message to EAP supplicant.\n"));

						if (eSource == eFromCSCM)
							BeceemAPI_EAP_FromCSCMToSupplicant (
								hAPI,
								pstEAPPayload->aucEAPPayload, 
								pstEAPPayload->usEAPPayloadLength); 
						else if (eSource == eFromWiMAXAPI)
							BeceemAPI_EAP_FromBaseStationToSupplicant (
								hAPI,
								pstEAPPayload->aucEAPPayload, 
								pstEAPPayload->usEAPPayloadLength); 
						}
					else
						PrintDiag(DIAG_NORMAL,hAPI, S("Ignored EAP packet, because Authentication Thread is NOT running.\n"));

					}
				}
			break;

#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST
		case SUBTYPE_EAP_ENCRYPT_PRIVATE_RESPONSE:
			// This case receives special handling in BeceemAPICallback.c
			PrintDiag(DIAG_ERROR, hAPI, S("PROGRAM ERROR in EAPResponse()\n"));
			break;
#endif

		default:
			break;
	}

} // EAPResponse(...

// -----------------------------------------------------------------------------------------
static void BeceemAPI_DeviceResetUnloadResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg)
	{

	if (!pWiMaxMsg)
        return;
	
    if (pWiMaxMsg->usType != DEVICE_RESET_UNLOAD_TYPE)
        return;

	switch(pWiMaxMsg->usSubType)
		{
		
		case DEVICE_UNLOAD:
			if (!(hAPI->eNetworkSearchState == NsIdle    ||
				  hAPI->eNetworkSearchState == NsAborted ||
				  hAPI->eNetworkSearchState == NsDone))
				BeceemAPI_AbortNetworkSearch(hAPI);
			BeceemAPI_SetState(hAPI, STATE_UNLOADED);
			break;
		default:
			break;
		}

	} // DeviceUnloadResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_ChipsetResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg)
	{

	PFIRMWARE_STATUS_INDICATION pFm = {0};
	STRING bszStatus;

	if (!pWiMaxMsg)
        return;
	
    if (pWiMaxMsg->usType != TYPE_CHIPSET_CONTROL)
        return;

	switch(pWiMaxMsg->usSubType)
		{

		case SUBTYPE_RESET_INDICATION:
			BeceemAPI_Deactivate(hAPI, ID_EVENT_CHIPSET_RESET_TIMEOUT);
			PrintDiag(DIAG_NORMAL,hAPI, S("Baseband chip reset completed\n"));
			hAPI->eChipsetControlStatus = ChipsetResetSuccess;
			break;

		case SUBTYPE_FIRMWARE_DNLD_STATUS_RESP:
			BeceemAPI_Deactivate(hAPI, ID_EVENT_FW_DOWNLOAD_TIMEOUT);
			pFm = (PFIRMWARE_STATUS_INDICATION) pWiMaxMsg->szData; 
			bszStatus = pFm->u32FirmwareDownloadStatus == SUCCESS ? S("Success") : S("FAILURE");
			PrintDiag(DIAG_NORMAL,hAPI, S("Firmware Download Status = ") B_SF S(" (code = %ld)\n"), 
				bszStatus, pFm->u32FirmwareDownloadStatus); 

			if (pFm->stFirmWareInfo.u32ImageType == IMAGE_CONFIG_FILE)
				{
				ServerSocketResponse(hAPI, ID_MSG_CF_DOWNLOAD, bszStatus, TRUE);
				PrintDiag(DIAG_NORMAL,hAPI, S("Config File = ") B_NSFNL, pFm->stFirmWareInfo.szImageName);
				}
			else if(pFm->stFirmWareInfo.u32ImageType == IMAGE_FIRMWARE)
				{
				ServerSocketResponse(hAPI, ID_MSG_FW_DOWNLOAD, bszStatus, TRUE);
				PrintDiag(DIAG_NORMAL,hAPI, S("Firmware File = ") B_NSFNL, pFm->stFirmWareInfo.szImageName);
				}
			else
				PrintDiag(DIAG_NORMAL,hAPI, S("Invalid file type set.\n"));

			if (pFm->u32FirmwareDownloadStatus == SUCCESS)
				hAPI->eChipsetControlStatus = 
					pFm->stFirmWareInfo.u32ImageType == IMAGE_FIRMWARE ? 
						ChipsetFirmwareDownloadSuccess : ChipsetConfigFileDownloadSuccess;
			else
				hAPI->eChipsetControlStatus = 
					pFm->stFirmWareInfo.u32ImageType == IMAGE_FIRMWARE ? 
						ChipsetFirmwareDownloadFailure : ChipsetConfigFileDownloadFailure;



			break;

		default:
			break;
		}

	} // ChipsetResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_SetWiMAXOptionsResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg, UINT32 u32MsgLength)
	{

	PST_WIMAX_OPTIONS pstWiMAXOptions;

	if (!pWiMaxMsg)
        return;
	
    if (pWiMaxMsg->usType != TYPE_SET_WIMAX_OPTIONS)
        return;

	switch(pWiMaxMsg->usSubType)
		{

		case SUBTYPE_SET_WIMAX_OPTIONS_RESP:
			pstWiMAXOptions = (PST_WIMAX_OPTIONS) pWiMaxMsg->szData;
			PrintDiag(DIAG_NORMAL, hAPI, S("Set WiMAX Options ") B_SF S(" for option 0x%02X.\n"), 
				pstWiMAXOptions->u32WimaxOptionsPayload == 0 ? S("successful") : S("FAILED"),
				pstWiMAXOptions->u8WimaxOptionSpecifier);
			break;	

		default:
			break;

		}

	} // BeceemAPI_SetWiMAXOptionsResponse

// -----------------------------------------------------------------------------------------
static void BeceemAPI_HalfMiniCardResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pWiMaxMsg)
	{

	UINT32 u32IsHMCShutdown = FALSE;
	STRING bszHMCShutdownStatus = EMPTY_STRING;

	if (!pWiMaxMsg)
        return;
	
    if (pWiMaxMsg->usType != TYPE_HALF_MINI_CARD)
        return;

	switch(pWiMaxMsg->usSubType)
		{

		case SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION:
			u32IsHMCShutdown = TRUE;
			break;	

		case SUBTYPE_HALF_MINI_CARD_WAKEUP_RESPONSE:
			u32IsHMCShutdown = FALSE;
			break;

		default:
			PrintDiag(DIAG_NORMAL,hAPI, S("Unknown half-mini card indication sub-type = %d\n"), pWiMaxMsg->usSubType);
			u32IsHMCShutdown = FALSE;
			break;

		}

	if (u32IsHMCShutdown) 
		{
		BeceemAPI_SetState(hAPI, STATE_HMC_SHUTDOWN);
		bszHMCShutdownStatus = S("Half-mini Card Device is shut down");
		}

	else
		{
		BeceemAPI_SetState(hAPI, STATE_LEAVING_HMC_SHUTDOWN);
		bszHMCShutdownStatus = S("Half-mini Card Device is not shut down");
		}
		
	WcmMutex_Wait(hAPI->hMutexStatusResponse);
	BeceemAPI_DisplayBasebandStatus(hAPI, FALSE, S("Half-mini card mode"), bszHMCShutdownStatus, NULL);
	WcmMutex_Release(hAPI->hMutexStatusResponse);

	} // BeceemAPI_HalfMiniCardResponse

// =========================================================================================
// Set the next state
// =========================================================================================

void BeceemAPI_SetStateEx(BeceemAPI_t hAPI, UINT32 u32Value, STRING bszMessage)
	{

	size_t sizBufLen = BCHARSOF(hAPI->bszLinkStatus);
	size_t sizStrLen;

	BeceemAPI_SetState(hAPI, u32Value);

	sizStrLen = b_strlen(hAPI->bszLinkStatus);

	StrPrintf(hAPI->bszLinkStatus + sizStrLen, sizBufLen - sizStrLen, S(": ") B_SF, bszMessage);

	hAPI->bszLinkStatus[sizBufLen] = B_NUL;

	} /* BeceemAPI_SetStateEx */

void BeceemAPI_SetState (BeceemAPI_t hAPI, UINT32 u32Value)
	{

	STRING bszState;
	UINT32 u32PrevState = hAPI->u32State;
		
	// Limit transitions out of shutdown and idle modes
	if (u32Value == STATE_OFF || u32Value == STATE_CLOSED || u32Value == STATE_UNLOADED)
		hAPI->u32State = u32Value;		

	else if (u32PrevState == STATE_SHUTDOWN)
		{
		if (u32Value == STATE_LEAVING_SHUTDOWN)
			hAPI->u32State = u32Value;
		}
	else if (u32PrevState == STATE_HMC_SHUTDOWN)
		{
		// Note: A device in HMC shutdown is also shutdown, so leaving shutdown means that we are
		//       also leaving HMC shutdown.
		if (u32Value == STATE_LEAVING_HMC_SHUTDOWN || u32Value == STATE_LEAVING_SHUTDOWN)
			hAPI->u32State = u32Value;
		}
	else if (u32PrevState == STATE_IDLE)
		{
		if (u32Value == STATE_LEAVING_IDLE)
			hAPI->u32State = u32Value;
		}

	// Ignore idle & shutdown status calls during initialization period
	else if ((u32PrevState == STATE_WAITING_FOR_CARD || u32PrevState == STATE_LIBRARY_INIT) &&
		(u32Value == STATE_LEAVING_SHUTDOWN     || 
		 u32Value == STATE_LEAVING_HMC_SHUTDOWN || 
		 u32Value == STATE_LEAVING_IDLE))
		hAPI->u32State = u32PrevState;

	// Otherwise, update the state
	else
		hAPI->u32State = u32Value;

	bszState = BeceemAPI_DecodeState(hAPI);
	StrPrintf(hAPI->bszLinkStatus, BCHARSOF(hAPI->bszLinkStatus), B_SF, 
		bszState);

	if (u32Value != u32PrevState)
		PrintDiag(DIAG_NORMAL,hAPI, S("Changed state to: ") B_SFNL, bszState);

#if 0
	if (!(u32Value == STATE_SYNC_UP_DONE      || 
		  u32Value == STATE_NET_ENTRY_IN_PROG ||
		  u32Value == STATE_NET_ENTRY_DONE    ||
		  u32Value == STATE_IDLE              ||
		  u32Value == STATE_SLEEP			  ||
		  u32Value == STATE_SHUTDOWN          ||
		  u32Value == STATE_HMC_SHUTDOWN      ))
		/* Print_Clear(hAPI->hPrintConStat) */;
#endif

	} /* BeceemAPI_SetState */

BOOL BeceemAPI_IsConnectedState(BeceemAPI_t hAPI)
	{
	
	return hAPI->u32State == STATE_NET_ENTRY_DONE ||
		   hAPI->u32State == STATE_IDLE           ||
		   hAPI->u32State == STATE_SLEEP;

	} /* BeceemAPI_IsConnectedState */


// =========================================================================================
// Callback status display functions
// =========================================================================================

STRING BeceemAPI_DecodeState(BeceemAPI_t hAPI) 
	{
	STRING bszString;

	switch (hAPI->u32State) 
		{
		case STATE_OFF:
			bszString = S("Off");
			break;

		case STATE_ERROR:
			bszString = S("Connection error");
			break;

		case STATE_IDLE:
			bszString = S("Idle mode");
			break;

		case STATE_SLEEP:
			bszString = S("Sleep mode");
			break;

		case STATE_LEAVING_IDLE:
			bszString = S("Left Idle Mode");
			break;

		case STATE_SHUTDOWN:
			bszString = S("Device is shut down");
			break;

		case STATE_LEAVING_SHUTDOWN:
			bszString = S("Device is not shut down");
			break;

		case STATE_HMC_SHUTDOWN:
			bszString = S("Half-mini Card Device is shut down");
			break;

		case STATE_LEAVING_HMC_SHUTDOWN:
			bszString = S("Half-mini Card Device is not shut down");
			break;

		case STATE_SYNC_DOWN_SENT:
			bszString = S("Sync down request sent");
			break;

		case STATE_UNLOADED:
			bszString = S("Card removed");
			break;

		case STATE_LIBRARY_INIT:
			bszString = S("Library initialized");
			break;

		case STATE_LIBRARY_INIT_ERROR:
			bszString = S("ERROR while initializing library");
			break;

		case STATE_WAITING_FOR_CARD:
			bszString = S("Waiting for WiMAX adapter ...");
			break;

		case STATE_NET_WAIT_PROXY_CONN:
			bszString = S("Waiting for connection to target ...");
			break;

		case STATE_CARD_INIT:
			bszString = S("WiMAX adapter initialized");
			break;

		case STATE_CARD_INIT_ERROR:
			bszString = S("ERROR while initializing WiMAX card");
			break;

		case STATE_CLOSED:
			bszString = S("WiMAX adapter interface closed");
			break;

		case STATE_NET_SEARCH_DONE:
			bszString = S("Network Search completed");
			break;
	
		case STATE_WAIT_PHY_SYNC_CMD:
			bszString = S("Waiting for PHY sync start");
			break;

		case STATE_PHY_SYNC_IN_PROG:
			bszString = S("PHY sync in progress");
			break;

		case STATE_SYNC_UP_DONE:
			bszString = S("PHY Sync acquired");
			break;

		case STATE_WAIT_NET_ENTRY_CMD:
			bszString = S("Waiting for network entry command");
			break;

		case STATE_NET_ENTRY_IN_PROG:
			bszString = S("Network Entry in progress");
			break;

		case STATE_NET_ENTRY_DONE:
			bszString = S("Network Entry completed");
			break;

		default:
			bszString = S("Invalid state");
			BeceemAPI_ProgramError(hAPI, S("Invalid state"), S(__FILE__), __LINE__);
			break;

		}

	return bszString;

	} /* BeceemAPI_DecodeState */

// -----------------------------------------------------------------------------------------
STRING BeceemAPI_DecodeLinkStatus(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize)
{

	STRING bszString = EMPTY_STRING;
	BOOL bUnknown = FALSE;

	switch(i32Status)
	{
	case LINKUP_ACHIEVED:           bszString = S("LINKUP ACHIEVED"); break;
	case PHY_SYNC_ERROR:            bszString = S("PHY SYNC ERROR: MS could not find the BS"); break;
	case MAC_SYNC_ERROR:            bszString = S("MAC SYNC ERROR: No DL map, DCD, UCD, etc"); break;
	case HOST_INITIATED_SYNC_DOWN:  bszString = S("HOST INITIATED SYNC DOWN"); break;
	case RANGING_FAILURE:           bszString = S("RANGING FAILURE"); break;
	case SBC_FAILURE:               bszString = S("SBC FAILURE: Basic capability negotiation with the BS failed"); break;
	case PKM_FAILURE:               bszString = S("PKM FAILURE"); break;
	case REGISTRATION_FAILURE:      bszString = S("REGISTRATION FAILURE"); break;
	case DREG_RECEIVED:             bszString = S("DEREG RECEIVED"); break;
	case RESET_RECEIVED:            bszString = S("RESET RECEIVED"); break;
	case IN_IDLE_MODE:              bszString = S("IN IDLE MODE"); break;
	case IN_SLEEP_MODE:             bszString = S("IN SLEEP MODE"); break;
	case NETWORK_ENTRY_IN_PROGRESS: bszString = S("NETWORK ENTRY IN PROGRESS"); break;
	case WAIT_FOR_NETWORK_ENTRY_CMD:bszString = S("WAIT FOR NETWORK ENTRY CMD"); break;
	case WAIT_FOR_PHY_SYNC_CMD:     bszString = S("WAIT FOR PHY SYNC CMD"); break;
	case SHUTDOWN_RECEIVED_CMD:     bszString = S("SHUTDOWN RECEIVED CMD"); break;
	case SATEK_FAILURE:             bszString = S("SATEK FAILURE: Security Association TEK with the BS failed"); break;
	case LINK_STATUS_NORMAL:        bszString = S("LINK STATUS NORMAL"); break;
	case INVALID_REASON_CODE_RECEIVED: bszString = S("INVALID REASON CODE RECEIVED"); break;
	case LINK_DOWN_HOST_INITIATED:  bszString = S("LINK DOWN HOST INITIATED"); break;
	case MAC_VERSION_SWITCH:        bszString = S("MAC VERSION SWITCH"); break;
	case SHUTDOWN_RESYNC_RECEIVED:  bszString = S("SHUTDOWN RESYNC RECEIVED"); break;
	case IDLE_MODE_FAILURE_CODE_RECEIVED: bszString = S("IDLE MODE FAILURE CODE RECEIVED"); break;
	case LOCATION_UPDATE_WITH_POWER_DOWN_SUCCESS_RECEIVED: 
		bszString = S("LOCATION UPDATE WITH POWER DOWN SUCCESS RECEIVED");
		break;
	default:
		bUnknown = TRUE;
		break;
	}

	if (bUnknown)
		return StrPrintf(bszBuffer, u32BcharSize, S("Unknown link status code = %d"), i32Status);
	else
		return bszString;

} /* BeceemAPI_DecodeLinkStatus */

STRING BeceemAPI_DecodeDeviceStatus(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize)
	{

	STRING bszString = EMPTY_STRING;
	BOOL bUnknown = FALSE;

	switch(i32Status)
		{
		case DEV_UNINITIALIZED:  bszString = S("Device is uninitialized"); break;
		case DEV_RF_OFF_HW_SW:   bszString = S("Device RF Off(both H/W and S/W)"); break;
		case DEV_RF_OFF_HW:      bszString = S("Device RF Off(via H/W switch)"); break;
		case DEV_RF_OFF_SW:      bszString = S("Device RF Off(via S/W switch)"); break;
		case DEV_READY:          bszString = S("Device is ready"); break;
		case DEV_SCANNING:       bszString = S("Device is scanning"); break;
		case DEV_CONNECTING:     bszString = S("Connection in progress"); break;
		case DEV_DATA_CONNECTED: bszString = S("Layer 2 connected"); break;
		default:				 bUnknown = TRUE; break;
	}

	if (bUnknown)
		return StrPrintf(bszBuffer, u32BcharSize, S("Unknown connection progress code = %d"), i32Status);
	else
		return bszString;

	} /* BeceemAPI_DecodeDeviceStatus */

STRING BeceemAPI_DecodeConnectionProgress(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize)
	{

	STRING bszString = EMPTY_STRING;
	BOOL bUnknown = FALSE;

	switch(i32Status)
		{
		case WAIT_FOR_SYNC:						bszString = S("Waiting for Sync"); break;
		case PHY_SYNC_ACHIVED:					bszString = S("Phy Sync Achieved"); break;
		case LINKUP_IN_PROGRESS:				bszString = S("Network Entry In Progress"); break;
		case LINKUP_DONE:						bszString = S("Network Entry Achieved"); break;
		case DREG_RECIEVED:						bszString = S("DREG Received"); break;
		case RESET_RECIEVED:					bszString = S("RESET Received"); break;
		case PERIODIC_WAKE_UP_FROM_SHUTDOWN:	bszString = S("Periodic Wake Up From Shutdown"); break;
		case SHUTDOWN_REQ_FROM_FIRMWARE:		bszString = S("Shutdown Notification "); break;	
		case COMPLETE_WAKE_UP_FROM_SHUTDOWN:	bszString = S("Complete Wake Up From Shutdown"); break;
		case RANGING_IN_PROGRESS:				bszString = S("Ranging In Progress"); break;	
		case SBC_IN_PROGRESS:					bszString = S("SBC In Progress"); break;
		case EAP_AUTH_DEVICE_IN_PROGRESS:		bszString = S("Device Authentication In Progress"); break;
		case EAP_AUTH_USER_IN_PROGRESS:			bszString = S("User Authentication In Progress"); break;
		case SATEK_IN_PROGRESS:					bszString = S("SATEK In Progress"); break;
		case REGISTRATION_IN_PROGRESS:			bszString = S("Registration In Progress"); break;
		default:								bUnknown = TRUE; break;
		}

	if (bUnknown)
		return StrPrintf(bszBuffer, u32BcharSize, S("Unknown connection progress code = %d"), i32Status);
	else
		return bszString;

	} /* BeceemAPI_DecodeConnectionProgress */

STRING BeceemAPI_DecodeLinkStatusReason(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize)
	{

	STRING bszString = EMPTY_STRING;
	BOOL bUnknown = FALSE;

	switch(i32Status)
		{
		case STATUS_NORMAL:				bszString = S("Link Status Normal"); break;
		case PHY_SYNC_FAIL:				bszString = S("Phy Sync Failed"); break;
		case MAC_SYNC_FAIL:				bszString = S("Mac Sync Failed"); break;
		case HOST_INITIATED:			bszString = S("Host Initiated Sync Down"); break;	
		case RANGE_FAIL:				bszString = S("Ranging Failed"); break;
		case SBC_FAIL:					bszString = S("SBC Failed"); break;
		case PKM_FAIL:					bszString = S("PKM Failed"); break;
		case SATEK_FAIL:				bszString = S("SATEK Failed"); break;
		case REG_FAIL:					bszString = S("Registration Failed"); break;
		case IDLE_MODE_CODE:			bszString = S("Device In Idle Mode"); break;
		case SLEEP_MODE_CODE:			bszString = S("Device In Sleep Mode "); break;
		case DREG_RECEIVED_CODE:		bszString = S("DREG Received"); break;
		case RESET_RECEIVED_CODE:		bszString = S("RESET Received"); break;
		case NET_ENTRY_PROGRESS_CODE:	bszString = S("Network Entry In Progress"); break;
		case WAIT_NET_ENTRY_CMD_CODE:	bszString = S("Wating for Network Entry"); break;
		case WAIT_PHY_SYNC_CMD_CODE:	bszString = S("Wating for Phy Sync"); break;
		case SHUTDOWN_RECEIVED_CODE:	bszString = S("Shutdown Command Received"); break;	
		case INVALID_REASON_VALUE:		bszString = S("Invalid Reason Value"); break;	
		case MAC_VERSION_SWITCH_CODE:	bszString = S("MAC version switch"); break;
		case HOST_INITIATED_LINK_DOWN:	bszString = S("Host initiated link down"); break;
		case IDLE_MODE_FAILURE:         bszString = S("Idle mode failure"); break;	
		case SHUTDOWN_RESYNC:           bszString = S("Shutdown resync message received"); break;
		case LOCATION_UPDATE_SUCCESS:   bszString = S("Location update success"); break;
		default:						bUnknown = TRUE; break;
		}

	if (bUnknown)
		return StrPrintf(bszBuffer, u32BcharSize, S("Unknown link status reason = %d"), i32Status);
	else
		return bszString;

	} /* BeceemAPI_DecodeLinkStatusReason */

void BeceemAPI_DisplayBasebandStatus(BeceemAPI_t hAPI, BOOL bConnected, 
								     STRING bszStatusLabel, STRING bszStatusValue1, STRING bszStatusValue2)
	{

	UINT32 u32Temp, u32Sec;

	// -------------------------------------
	// Clear the previous connection status
	// -------------------------------------

	Print_Flush(hAPI->hPrintConStat);
	Print_SetBuffered(hAPI->hPrintConStat, TRUE);

	// -------------------------
	// Status
	// ------------------------

	if (bszStatusValue2 == NULL)
		BeceemAPI_PrintConStat(hAPI, B_SF S(" status\t ") B_SFNL, bszStatusLabel, bszStatusValue1);
	else
		BeceemAPI_PrintConStat(hAPI, B_SF S(" status\t ") B_SF S(" (") B_SF S(")\n"), bszStatusLabel, bszStatusValue1, bszStatusValue2);

	// -------------------------
	// Elapsed time counters
	// ------------------------

	if (!bConnected)
		hAPI->u32ConnStartSeconds = hAPI->u32TotalSeconds;
	else
		{
		u32Temp = hAPI->u32TotalSeconds - hAPI->u32ConnStartSeconds;
		u32Sec = u32Temp % 60;
		u32Temp /= 60;
		BeceemAPI_PrintConStat(hAPI, S("Connection time\t %u:%u:%u\n"), 
			u32Temp / 60, u32Temp % 60, u32Sec);
		}

	u32Temp  = hAPI->u32TotalSeconds;
	u32Sec   = u32Temp % 60;
	u32Temp /= 60;
	BeceemAPI_PrintConStat(hAPI, S("Server up time\t %u:%u:%u\n"), 
		u32Temp / 60, u32Temp % 60, u32Sec);

	} /* BeceemAPI_DisplayLinkStatus */

// -----------------------------------------------------------------------------------------
void BeceemAPI_DisplayBaseInfo(BeceemAPI_t hAPI, PrintContext_t hPrint, PBSINFOEX pBaseInfo)
{

#define s16(x) ((x) > 32767 ? (x) - 65536 : (x))

	if(!pBaseInfo)
		return;

	Print_Newline(hPrint);
	Print_Formatted(hPrint,
		S("Base Station ID\t %02X:%02X:%02X:%02X:%02X:%02X\n"),          
		pBaseInfo->au8BSID[0],
		pBaseInfo->au8BSID[1],
		pBaseInfo->au8BSID[2],
		pBaseInfo->au8BSID[3],
		pBaseInfo->au8BSID[4],
		pBaseInfo->au8BSID[5]);
	Print_Formatted(hPrint, S("Preamble Index\t %d\n"),		pBaseInfo->u8PreambleIndex);
	Print_Formatted(hPrint, S("UL Cell ID\t %d\n"),			pBaseInfo->u8ULCellID);
	hAPI->s32RSSI = (INT32) pBaseInfo->s32MeanRssiReport;
	hAPI->s16CINR = (INT16) pBaseInfo->s16CINRMean;
	Print_Formatted(hPrint, S("RSSI Mean\t %d dBm\n"),		pBaseInfo->s32MeanRssiReport);	
	Print_Formatted(hPrint, S("CINR Mean\t %d dB\n"),		    pBaseInfo->s16CINRMean);
	//Print_Formatted(hPrint, S("RSSI Deviation\t %d\n"),		pBaseInfo->u32StdDeviationRssiReport); 
	//Print_Formatted(hPrint, S("CINR Deviation\t %d dB\n"),	pBaseInfo->u16CINRDeviation); 
	Print_Formatted(hPrint, S("UL Center Freq\t %d kHz\n"),	pBaseInfo->u32UlCenterFrequency); 
	Print_Formatted(hPrint, S("DL Center Freq\t %d kHz\n"),	pBaseInfo->u32DlCenterFrequency); 
	//Print_Formatted(hPrint, S("Duration\t\t %d\n"),			pBaseInfo->u32Duration);

} // DisplayBaseInfo

#ifdef LINUX
VOID BeceemAPI_RefreshIPAddress(BeceemAPI_t hAPI)
	{
	BOOL bError = FALSE;
	STRING bszCommand;
	char * szCommand;
#define MAX_IP_REFRESH_ARGS 20
	int argc;
	char *saveptr, *argv[MAX_IP_REFRESH_ARGS+1];
	pid_t pidChild;
	int iChildStatus, iWaitOptions;

	bszCommand = Options_GetStringByEntry(hAPI->pOptIPRefreshCommand, &bError);

	if (bError)
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR retrieving IP refresh command\n"));
	else
		{
		// Convert the IP refresh command to ASCI
		szCommand = StringToCharStar(bszCommand);
		if (szCommand == NULL)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("Memory allocation failed in BeceemAPI_RefreshIPAddress()\n"));
			return;
			}

		PrintDiag(DIAG_NORMAL, hAPI, S("\n********** Refreshing IP Address (") B_SF S(") ***********\n"),
			BeceemAPI_DateTime(hAPI));
		// Parse the argument list
		argc = 0;
		argv[argc] = strtok_r(szCommand, "\t ", &saveptr);
		while (argv[argc] != NULL && argc < MAX_IP_REFRESH_ARGS)
			{
			argc++;
			argv[argc] = strtok_r(NULL, "\t ", &saveptr);
			}

		// Test for too few or too many args
		if (argc <= 0)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Invalid IP refresh command\n"));
			}
		else if (argv[argc] != NULL)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: More than %d args in IP Refresh command.\n"),
				MAX_IP_REFRESH_ARGS);
			}

		// Args OK, process with fork() call
		else
			{
			// Create the child process
			pidChild = fork();
			if (pidChild == 0)
				{
				// Executed by child
				// TO DO: Do we need to close any handles here?
				execvp(argv[0], argv);
				// Child gets here only if execvp failed
				exit(1);	
				}
			// Executed by server (parent)
			else if (pidChild == -1)
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR: IP Refresh fork() call failed.\n"));
				}
			else
				{
				PrintDiag(DIAG_VERBOSE, hAPI, S("Waiting for child process (pid = %d) to finish.\n"), pidChild);
				iWaitOptions = 0;
				waitpid(pidChild, &iChildStatus, iWaitOptions);
				PrintDiag(DIAG_VERBOSE, hAPI, S("Child process (pid = %d) has finished with status = 0x08x"), 
					pidChild, iChildStatus);
				}
			}

		WcmFree(szCommand);

		PrintDiag(DIAG_NORMAL, hAPI, S("\n********** End IP Address Refresh (") B_SF S(") ***********\n"),
			BeceemAPI_DateTime(hAPI));

		}

	} /* BeceemAPI_RefreshIPAddress */

void BeceemAPI_IPRefreshKillProcess(BeceemAPI_t hAPI)
	{

#define MAX_PID_LIST 20
    pid_t aPid[MAX_PID_LIST];
    UINT32 u32NumPid;
    int n, iRet;
	STRING bszCommand;
	char * szCommand;
	BOOL bError = FALSE;

	// Get the IP refresh command line
	bszCommand = Options_GetStringByEntry(hAPI->pOptIPRefreshCommand, &bError);
	if (bError)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: IP Refresh option get failed.\n"));
		return;
		}

	szCommand = StringToCharStar(bszCommand);
	if (szCommand == NULL)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Memory allocation failed in BeceemAPI_IPRefreshKillProcess().\n"));
		return;
		}

	if (strlen(szCommand) == 0)
		{
		PrintDiag(DIAG_NORMAL, hAPI, S("WARNING: Empty IP Refresh command string\n"));
		return;
		}

    // Find the pid's for this command line
    u32NumPid = FindPid(szCommand, aPid, MAX_PID_LIST);
	WcmFree(szCommand);        

	// Kill all the processes that match exactly this command line
  	for (n=0; n<u32NumPid; n++)
		{
		PrintDiag(DIAG_NORMAL, hAPI, S("Killing IP Refresh process (PID = %u) "), aPid[n]);
        errno = 0;
        iRet = kill(aPid[n], SIGTERM);
        if (iRet != 0)
        	{
        	if (errno == EPERM)
        		PrintDiag(DIAG_NORMAL, hAPI, S(" ERROR: No permission\n"));
        	else if (errno == ESRCH)
        		PrintDiag(DIAG_NORMAL, hAPI, S(" ERROR: No such process\n"));
        	else
        		PrintDiag(DIAG_NORMAL, hAPI, S(" ERROR: Other code = %d\n"), errno);
        	}
        else
        	PrintDiag(DIAG_NORMAL, hAPI, S("OK\n"));
        }
 
	} /* BeceemAPI_IPRefreshKillProcess */
#endif

#ifdef WIN32
// -----------------------------------------------------------------------------------------
VOID BeceemAPI_RefreshIPAddress(BeceemAPI_t hAPI)
	{

	// --------------------------------------------------------
	// Get the name (GUID) of the Beceem adapter
	// --------------------------------------------------------

//#define REFRESH_DHCP_TEST

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	PIP_INTERFACE_INFO pInfo = NULL;
    int i;
	B_CHAR bszAdapterDescription[sizeof(pAdapter->Description)];
	B_CHAR bszBeceemAdapterName[sizeof(pAdapter->AdapterName)];
#ifdef ENABLE_WIDE_CHAR
	STRING bszAdapterName;
#else
	B_CHAR bszAdapterName[BCHARSOF(pInfo->Adapter[0].Name)];
#endif
	STRING bszBeceemAdapterDescription = NULL;
	BOOL bDHCPEnabled = FALSE;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	PrintDiag(DIAG_NORMAL,hAPI, S("\n********** Refreshing IP Address ***********\n"));

	// Allocate the buffer with a default size
	pAdapterInfo = (IP_ADAPTER_INFO *) WcmMalloc( sizeof(IP_ADAPTER_INFO) );
	if (pAdapterInfo == NULL)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to allocate memory for adapter information structures.\n"));
		return;
		}

	// Allocate the buffer again, if necessary, to the correct size returned
	// by GetAdaptersInfo
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
		{
		WcmFree(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) WcmMalloc (ulOutBufLen); 
		if (pAdapterInfo == NULL)
			{
			PrintDiag(DIAG_NORMAL,hAPI, S("Unable to allocate memory for adapter information structures.\n"));
			return;
			}
		}

	// Find the adapter name for the first Beceem adapter
	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
		{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
			{			
			b_mbstobcs(bszAdapterDescription, pAdapter->Description, BCHARSOF(bszAdapterDescription));
			if (BcsStrStrNoCase(bszAdapterDescription, S("Beceem"))     ||
				BcsStrStrNoCase(bszAdapterDescription, S("BCM CARDBUS")) ||
				BcsStrStrNoCase(bszAdapterDescription, S("WiMax"))       
#ifdef REFRESH_DHCP_TEST
				|| sAdapterDescription->Contains(S("Intel"))
#endif
				) 
				{
				PrintDiag(DIAG_NORMAL,hAPI, S("Adapter Name: \t") B_SFNL, pAdapter->AdapterName);
				PrintDiag(DIAG_NORMAL,hAPI, S("Adapter Desc: \t") B_SFNL, pAdapter->Description);
				PrintDiag(DIAG_NORMAL,hAPI, S("Adapter Index:\t%lu\n"), pAdapter->Index);
				bszBeceemAdapterDescription = bszAdapterDescription;
				b_mbstobcs(bszBeceemAdapterName, pAdapter->AdapterName, BCHARSOF(bszBeceemAdapterName));
				bDHCPEnabled                = pAdapter->DhcpEnabled != 0;
				if (bDHCPEnabled) 
					{
					char buffer[128];
					__time32_t time32_value;

					PrintDiag(DIAG_NORMAL,hAPI, S("DHCP Enabled:   Yes\n"));
					PrintDiag(DIAG_NORMAL,hAPI, S("DHCP Server:    ") B_SFNL, pAdapter->DhcpServer.IpAddress.String);

					// NOTE: This is supposed to be 64-bit time value, but it is not,
					//       so we are forcing 32-bit time interpretation.
					time32_value = (__time32_t) pAdapter->LeaseObtained;
					buffer[0] = '\0';
					_ctime32_s(buffer, 128, &time32_value);
					PrintDiag(DIAG_NORMAL,hAPI, S("Lease Obtained: ") B_SFNL, buffer);
					}
				else
					PrintDiag(DIAG_NORMAL,hAPI, S("DHCP Enabled: No\n"));
				break;
				}
			pAdapter = pAdapter->Next;
			}
		}

	WcmFree(pAdapterInfo);

	if (dwRetVal != NO_ERROR)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Call to GetAdaptersInfo failed.\n"));
		return;
		}

	if (bszBeceemAdapterDescription == NULL)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("No Beceem adapter found.\n"));
		return;
		}

	// --------------------------------------------------------
	// Get the interface info structure for the Beceem adapter
	// and release/renew the lease.
	// --------------------------------------------------------

	ulOutBufLen = 0;
    dwRetVal = 0;

	// Allocate the buffer
    dwRetVal = GetInterfaceInfo(NULL, &ulOutBufLen);
    if (dwRetVal == ERROR_INSUFFICIENT_BUFFER) {
        pInfo = (IP_INTERFACE_INFO *) WcmMalloc(ulOutBufLen);
        if (pInfo == NULL) {
            PrintDiag(DIAG_ERROR, hAPI, S("Unable to allocate memory needed to call GetInterfaceInfo\n"));
            return;
        }
    }

	// Get the data
    dwRetVal = GetInterfaceInfo(pInfo, &ulOutBufLen);
    if (dwRetVal == NO_ERROR) {
        //PrintDiag(DIAG_NORMAL,hAPI, S("Number of Adapters: %ld\n\n"), pInfo->NumAdapters);
        for (i = 0; i < (int) pInfo->NumAdapters; i++) {
            //PrintDiag(DIAG_NORMAL,hAPI, S("Adapter Index[%d]: %ld\n"), i, pInfo->Adapter[i].Index);
            //PrintDiag(DIAG_NORMAL,hAPI, S("Adapter Name[%d]: "), i);
			//PrintDiag(DIAG_NORMAL,hAPI, S(""), gcnew String(pInfo->Adapter[i].Name), TRUE);
			//PrintDiag(DIAG_NORMAL,hAPI, S("\n"));
#ifdef ENABLE_WIDE_CHAR
			bszAdapterName = pInfo->Adapter[i].Name;
#else
#ifdef MS_VISUAL_CPP
			{
			size_t sizNumConverted;
			wcstombs_s(&sizNumConverted, bszAdapterName, sizeof(bszAdapterName), pInfo->Adapter[i].Name, _TRUNCATE);
			}
#else
			wcstombs(bszAdapterName, pInfo->Adapter[i].Name, sizeof(bszAdapterName));
#endif
#endif
			if (b_strstr(bszAdapterName, bszBeceemAdapterName) != NULL)
				{
				PrintDiag(DIAG_NORMAL,hAPI, S("Releasing DHCP lease - "));
				if ((dwRetVal = IpReleaseAddress(&pInfo->Adapter[i])) == NO_ERROR) 
					PrintDiag(DIAG_NORMAL,hAPI, S("OK\n"));
				else
					PrintDiag(DIAG_NORMAL,hAPI, S("FAILED\n"));
				PrintDiag(DIAG_NORMAL,hAPI, S("Renewing DHCP lease - "));
				if ((dwRetVal = IpRenewAddress(&pInfo->Adapter[i])) == NO_ERROR)
					PrintDiag(DIAG_NORMAL,hAPI, S("OK\n"));
				else
					PrintDiag(DIAG_NORMAL,hAPI, S("FAILED\n"));
				break;
				}
        }
    } else if (dwRetVal == ERROR_NO_DATA)
        PrintDiag(DIAG_NORMAL,hAPI, S("There are no network adapters with IPv4 enabled on the local system\n"));
    else
        PrintDiag(DIAG_NORMAL,hAPI, S("GetInterfaceInfo failed with error: %d\n"), dwRetVal);

	WcmFree(pInfo);

	PrintDiag(DIAG_NORMAL,hAPI, S("********************************************\n"));

    return;

	} // RefreshIPAddress

#endif

#ifdef MS_VISUAL_CPP
#pragma warning(pop)
#endif


