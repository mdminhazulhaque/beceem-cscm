/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "BeceemAPI.h"
#include "Utility.h"
#include "BeceemCscmOEM.h"

static INT32 BeceemAPI_FillWiMAXMsg(
	PWIMAX_MESSAGE pWMMsg, 
	UINT16 u16MessageType,
	UINT16 u16SubType, 
	UINT16 u16MsgSize
	);
static INT32 BeceemAPI_SendWiMAXMessage(BeceemAPI_t hAPI,
								        UINT8 *pu8SendBuffer,
								        UINT32 u32Length);
static BOOL BeceemAPI_IsRequestBlocked(BeceemAPI_t hAPI, STRING bszRequestType);

static BOOL BeceemAPI_StartCAPLNetworkSearch(BeceemAPI_t hAPI) ;

// -----------------------------------------------------------------------------------------
// void AbortNetworkSearch
//
// - Aborts an ongoing network search
//
//   Returns FALSE on error.

BOOL BeceemAPI_AbortNetworkSearch (BeceemAPI_t hAPI) 
	{

	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return FALSE;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending abort network search request\n"));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
		TYPE_NETWORK_SEARCH, SUBTYPE_NETWORK_SEARCH_ABORT_REQ, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

	hAPI->eNetworkSearchState  = NsAborted;
	BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
	hAPI->bIsNetworkSearchDone = TRUE;

	return TRUE;

	} /* BeceemAPI_AbortNetworkSearch  */


static BOOL BeceemAPI_StartCAPLNetworkSearch(BeceemAPI_t hAPI) 
{
	ST_CAPL_BASED_SEARCH_PARAMS lpsmParams;
	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if(!BeceemCscmOEM_GetCAPLParams(&lpsmParams))
	{
		PrintDiag(DIAG_VERBOSE, hAPI, S("\n***** ERROR: CAPL Parameters not configured! \n"));		
		return FALSE;
	}
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_SEARCH, 
								  SUBTYPE_CAPL_BASED_SEARCH_REQ, sizeof(ST_CAPL_BASED_SEARCH_PARAMS));
	memcpy(stRequestMessage.szData, (void *) &lpsmParams, sizeof(lpsmParams));

	PrintDiag(DIAG_VERBOSE, hAPI, S("\n***** Sending CAPL Search request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));		
	if(BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen) == SUCCESS)
		return TRUE;
	else
		return FALSE;
}

// -----------------------------------------------------------------------------------------
// void NetworkSearchRequest (void)
//
// - Requests that the WiMAX card search for all available base stations
//
//   Returns FALSE on error.

BOOL BeceemAPI_NetworkSearchRequest (BeceemAPI_t hAPI) 
	{

		ST_NETWORK_SEARCH_PARAMS stNetSearchPar;
		WIMAX_MESSAGE stRequestMessage;
		INT32 iLen, i32RetCode;
		hNetSearchParameters_t hNetSearchParam = hAPI->hNetSearchParam;
		BOOL bUseScanMode = FALSE, bUseDiscreteCFMode = FALSE;
		B_CHAR bszBuffer[100];
		UINT32 n, u32Value;
		B_CHAR *bszMsg = NULL;

		if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return FALSE;

		if (hNetSearchParam == NULL)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Network search request without net search parameters.\n"));
			return FALSE;
			}

		
		hAPI->eNetworkSearchState  = NsSearching;
		hAPI->bIsNetworkSearchDone = FALSE;

		if(hAPI->bUseCAPLForNetworkSearch)
		{
			if(BeceemAPI_StartCAPLNetworkSearch(hAPI))
				i32RetCode = SUCCESS;
			else
				i32RetCode = -1;
		}
		else
		{
			memset(&stNetSearchPar, 0, sizeof(stNetSearchPar));

			stNetSearchPar.u32Duration   = 0;  // in ms
			stNetSearchPar.u32ReportType = 2;  // 1 = BS by BS return; 2 = consolidated list

			// -------------------------------------------------
			// Retrieve search parameters only on first pass
			// -------------------------------------------------

			if (!hNetSearchParam->bSearchMoreCF)
				{
				if (!Options_GetNetSearchParameters(hAPI->hOptionTable, hNetSearchParam))
					{
					PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Options_GetNetSearchParameters failed in BeceemAPI_NetworkSearchRequest\n"));
					return FALSE;
					}
				}

			// -----------------------------
			// Scan mode
			// -----------------------------
#define MIN_CENTER_FREQUENCY_KHZ 2300000
#define MAX_CENTER_FREQUENCY_KHZ 3600000
#define MIN_STEP_FREQUENCY_KHZ	     250

			bUseScanMode =	!hNetSearchParam->bSearchMoreCF &&
							(hNetSearchParam->u32ScanMinFrequencyKHz  > 0 ||
							 hNetSearchParam->u32ScanMaxFrequencyKHz  > 0 ||
							 hNetSearchParam->u32ScanStepFrequencyKHz > 0);

			if (bUseScanMode)
				{

				stNetSearchPar.stContCFRange.u32CenterFreqStart     = hNetSearchParam->u32ScanMinFrequencyKHz;
				stNetSearchPar.stContCFRange.u32CenterFrequencyEnd  = hNetSearchParam->u32ScanMaxFrequencyKHz;
				stNetSearchPar.stContCFRange.u32CenterFrequencyStep = hNetSearchParam->u32ScanStepFrequencyKHz;

				if (!(stNetSearchPar.stContCFRange.u32CenterFreqStart >= MIN_CENTER_FREQUENCY_KHZ &&
					  stNetSearchPar.stContCFRange.u32CenterFrequencyEnd > stNetSearchPar.stContCFRange.u32CenterFreqStart &&
					  stNetSearchPar.stContCFRange.u32CenterFrequencyEnd <= MAX_CENTER_FREQUENCY_KHZ &&
					  stNetSearchPar.stContCFRange.u32CenterFrequencyStep >= MIN_STEP_FREQUENCY_KHZ))
					{
					bszMsg = S("Invalid Scan Min, Max, or Step value in search request");
					PrintDiag(DIAG_ERROR, hAPI, bszMsg);
					ServerSocketResponse(hAPI, ID_MSG_ERROR, bszMsg, TRUE);
					return FALSE;
					}

				}

			// -----------------------------
			// List of bandwidths
			// -----------------------------

			if (hNetSearchParam->u32NumberOfBandwidths == 0)
				{
				bszMsg = S("At least one bandwidth required for search request");
				PrintDiag(DIAG_ERROR, hAPI, bszMsg);
				ServerSocketResponse(hAPI, ID_MSG_ERROR, bszMsg, TRUE);
				return FALSE;
				}
			else if (hNetSearchParam->u32NumberOfBandwidths > MAX_NUM_BANDWIDTHS_SUPPORTED)
				{
				StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("No more than %lu bandwidths allowed in search request"), 
					MAX_NUM_BANDWIDTHS_SUPPORTED);
				PrintDiag(DIAG_ERROR, hAPI, bszBuffer);
				ServerSocketResponse(hAPI, ID_MSG_ERROR, bszBuffer, TRUE);
				return FALSE;
				}
			else
				{
				stNetSearchPar.stBandwidthList.u32NumOfBwsToScan = hNetSearchParam->u32NumberOfBandwidths;
				for (n=0; n < hNetSearchParam->u32NumberOfBandwidths; n++)
					{
					stNetSearchPar.stBandwidthList.au32BandwidthList[n] = hNetSearchParam->u32BandwidthKHz[n];	
					u32Value = stNetSearchPar.stBandwidthList.au32BandwidthList[n];
					if (u32Value != 5000 && u32Value != 7000 && u32Value != 8750 && u32Value != 10000)
						{
						StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("Invalid Bandwidth (%lu) in search request"), u32Value);
					    PrintDiag(DIAG_ERROR, hAPI, bszBuffer);
						ServerSocketResponse(hAPI, ID_MSG_ERROR, bszBuffer, TRUE);
						return FALSE;
						}
					}
				}

			// -----------------------------
			// Discrete center frequencies
			// -----------------------------	

			bUseDiscreteCFMode = hNetSearchParam->u32NumberOfFrequencies > 0;

			if (!bUseScanMode && !bUseDiscreteCFMode)
				{
				bszMsg = S("Neither scan range nor discrete frequency list specified");
				PrintDiag(DIAG_ERROR, hAPI, bszMsg);
				ServerSocketResponse(hAPI, ID_MSG_ERROR, bszMsg, TRUE);
				return FALSE;
				}
			else if (bUseScanMode && bUseDiscreteCFMode)
				{
				bszMsg = S("Cannot specify both scan range and discrete frequency list");
				PrintDiag(DIAG_ERROR, hAPI, bszMsg);
				ServerSocketResponse(hAPI, ID_MSG_ERROR, bszMsg, TRUE);
				return FALSE;
				}
			else if (hNetSearchParam->u32NumberOfFrequencies > MAX_NUM_DISCRETE_CF_SUPPORTED)
				{
				StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("No more than %lu center frequencies allowed in search request"), 
					MAX_NUM_DISCRETE_CF_SUPPORTED);
				PrintDiag(DIAG_ERROR, hAPI, bszBuffer);
				ServerSocketResponse(hAPI, ID_MSG_ERROR, bszBuffer, TRUE);
				return FALSE;
				}

	//split the list into 2.3, 2.5 and 3.5 GHz bands.
			if(!hNetSearchParam->bSearchMoreCF)
			{
				hNetSearchParam->u32FirstFreqIndex = 0;
			}
			else
			{
				hNetSearchParam->u32FirstFreqIndex = hNetSearchParam->u32LastFreqIndex + 1;
				hNetSearchParam->bSearchMoreCF = FALSE;
			}

			hNetSearchParam->u32LastFreqIndex  = hNetSearchParam->u32NumberOfFrequencies - 1;
			
			if(hNetSearchParam->u32CenterFrequencyKHz[hNetSearchParam->u32FirstFreqIndex] <= 2400000)
			{
				for (n=hNetSearchParam->u32FirstFreqIndex+1; n < hNetSearchParam->u32NumberOfFrequencies; n++)
				{
					if (hNetSearchParam->u32CenterFrequencyKHz[n] > 2400000)
					{
						hNetSearchParam->bSearchMoreCF    = TRUE;
						hNetSearchParam->u32LastFreqIndex = n-1;
						break;
					}
				}
			}
			else if(hNetSearchParam->u32CenterFrequencyKHz[hNetSearchParam->u32FirstFreqIndex] <= 2700000)
			{
				for (n=hNetSearchParam->u32FirstFreqIndex+1; n < hNetSearchParam->u32NumberOfFrequencies; n++)
				{
					if (hNetSearchParam->u32CenterFrequencyKHz[n] > 2700000)
					{
						hNetSearchParam->bSearchMoreCF	  = TRUE;
						hNetSearchParam->u32LastFreqIndex = n-1;
						break;
					}
				}
			}
			else //for 3.5GHz
			{
				hNetSearchParam->u32LastFreqIndex  = hNetSearchParam->u32NumberOfFrequencies - 1;
				hNetSearchParam->bSearchMoreCF     = FALSE;
			}

			stNetSearchPar.stDiscreteCFList.u32NumDiscreteFreq  = 
				(hNetSearchParam->u32LastFreqIndex + 1) - hNetSearchParam->u32FirstFreqIndex;

			PrintDiag(DIAG_VERBOSE, hAPI, S("Search CF: "));
			for (n=0; n < stNetSearchPar.stDiscreteCFList.u32NumDiscreteFreq; n++)
				{
				stNetSearchPar.stDiscreteCFList.au32CenterFreqList[n] = 
					hNetSearchParam->u32CenterFrequencyKHz[n + hNetSearchParam->u32FirstFreqIndex];
				u32Value = stNetSearchPar.stDiscreteCFList.au32CenterFreqList[n];
				PrintDiag(DIAG_VERBOSE, hAPI, S("%lu "), u32Value);
				if (u32Value < MIN_CENTER_FREQUENCY_KHZ || u32Value > MAX_CENTER_FREQUENCY_KHZ)
					{
					StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("Invalid valid Center Frequency (%lu) in search request"), u32Value);
					PrintDiag(DIAG_ERROR, hAPI, bszBuffer);
					ServerSocketResponse(hAPI, ID_MSG_ERROR, bszBuffer, TRUE);
					return FALSE;
					}
				}
			PrintDiag(DIAG_VERBOSE, hAPI, S("\n"));
		
			PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending network search request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
			iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
				TYPE_NETWORK_SEARCH, SUBTYPE_NETWORK_SEARCH_REQ, sizeof(stNetSearchPar));
			memcpy(stRequestMessage.szData, (void *) &stNetSearchPar, sizeof(stNetSearchPar));
			i32RetCode = BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);
		}

		if (i32RetCode == SUCCESS)
			BeceemAPI_Activate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
		else
			{
			hAPI->bIsNetworkSearchDone = TRUE;
			hAPI->eNetworkSearchState  = NsDone;
			}

		return TRUE;

	} // BeceemAPI_NetworkSearchRequest

// -----------------------------------------------------------------------------------------
// void SyncUpRequest (void)
//
// - Requests that the WiMAX card acquire PHY sync with a specific basestation
//

void BeceemAPI_SyncUpRequest (BeceemAPI_t hAPI) 
	{

		WIMAX_MESSAGE stRequestMessage;
		ST_SYNCUP_REQ_PARAMS *pstSyncUpReqParams;
		hBSInformation_t hBS;
		UINT32 iLen;
		int n;

		if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return;

		BeceemAPI_SetState(hAPI, STATE_PHY_SYNC_IN_PROG);

		if (hAPI->hSelectedBS == NULL)
			hBS = &hAPI->stConnectBS;
		else
			hBS = hAPI->hSelectedBS;

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending PHY sync up request for Preamble ID = 0x%02X - Freq = %lu - BW = %lu\n"), 
			hBS->u32PreambleIndex, hBS->u32CenterFrequency, hBS->u32Bandwidth);

		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_ENTRY, SUBTYPE_SYNC_UP_REQ, sizeof(ST_SYNCUP_REQ_PARAMS));
		pstSyncUpReqParams = (PST_SYNCUP_REQ_PARAMS) stRequestMessage.szData;
		pstSyncUpReqParams->u32PreambleId = hBS->u32PreambleIndex;
		pstSyncUpReqParams->u32Bandwidth  = hBS->u32Bandwidth;
		pstSyncUpReqParams->u32CenterFreq = hBS->u32CenterFrequency;
		for (n=0; n<sizeof(pstSyncUpReqParams->aucBSID); n++)
			pstSyncUpReqParams->aucBSID[n] = hBS->au8BSID[n];
			
		BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

		BeceemAPI_Activate(hAPI, ID_EVENT_SYNC_UP_TIMEOUT);

	} // BeceemAPI_SyncUpRequest

// -----------------------------------------------------------------------------------------
// void SyncDownRequest (void)
//
// - Requests that the WiMAX card return to a state in which network search can be done
//
void BeceemAPI_SyncDownRequest (BeceemAPI_t hAPI) 
	{

		WIMAX_MESSAGE stRequestMessage;
		INT32 iLen;

		if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return;

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending PHY sync down request\n"));
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_LINK_DOWN, SUBTYPE_SYNC_DOWN_REQ, 0);
		BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

		BeceemAPI_SetState(hAPI, STATE_SYNC_DOWN_SENT);
		BeceemAPI_Activate(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT);

	} // BeceemAPI_SyncDownRequest

// -----------------------------------------------------------------------------------------
// void BeceemAPI_NetworkEntryRequest (void)
//
// - Requests that the WiMAX card perform the network entry process
//
INT32 BeceemAPI_NetworkEntryRequest (BeceemAPI_t hAPI) 
	{

		UINT32 u32RetCode;
		INT32 iLen;
		WIMAX_MESSAGE stRequestMessage;
		PVENDOR_SPECIFIC_LINKPARAMS pVendorInfo;
		UINT32 u32NetEntryTimeoutMs;
		BOOL bError = FALSE;

		if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return 0;

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending network entry request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));

		// The following sets vendor specific message length to 0
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_ENTRY, SUBTYPE_NETWORK_ENTRY_REQ, 4); /*  Data len = 14 = 4+10 -> 10 bytes of Vendor specific data */		
		pVendorInfo = (PVENDOR_SPECIFIC_LINKPARAMS)stRequestMessage.szData;
		pVendorInfo->u32VendorSpecificInformationLength = 0;
		pVendorInfo->au8VendorSpecificInformation[0] = 0;    // Not used
		u32RetCode = BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

		// Start the timeout interval
		u32NetEntryTimeoutMs = 1000 * Options_GetUint32ByEntry(hAPI->pOptNetworkEntryTimeoutSec, &bError);
		if (bError)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Options_GetUint32ByEntry failed in BeceemAPI_NetworkEntryRequest\n"));
			return 0;
			}
		BeceemAPI_SetTimeout(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT, u32NetEntryTimeoutMs);
		BeceemAPI_Activate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);

		return u32RetCode;

	} // BeceemAPI_NetworkEntryRequest

// -----------------------------------------------------------------------------------------
// void NetworkDeregRequest (void)
//
// - Requests that the WiMAX card leave the current network
//

void BeceemAPI_NetworkDeregRequest (BeceemAPI_t hAPI) 
	{

		WIMAX_MESSAGE stRequestMessage;
		INT32 iLen;

		if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return;

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending network de-registration request\n"));
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_LINK_DOWN, SUBTYPE_SYNC_DEREG_REQ, 0);
		BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

		BeceemAPI_Activate(hAPI, ID_EVENT_NET_DEREG_TIMEOUT);

	} // NetworkDeregRequest

// -----------------------------------------------------------------------------------------
// void Disconnect (BeceemAPI_t hAPI)
//
// - Sends a Dereg request immediately followed by a sync down request
//

void BeceemAPI_Disconnect(BeceemAPI_t hAPI)
	{
	BeceemAPI_NetworkDeregRequest(hAPI);
	BeceemAPI_SyncDownRequest(hAPI);
	}

// -----------------------------------------------------------------------------------------
// void BasebandStatusRequest (void)
//
// - Requests that the WiMAX card report the current baseband status
//

void BeceemAPI_BasebandStatusRequest(BeceemAPI_t hAPI) 
	{

	if (BeceemAPI_IsActivated(hAPI, ID_EVENT_FW_DOWNLOAD_TIMEOUT) || 
		BeceemAPI_IsActivated(hAPI, ID_EVENT_CHIPSET_RESET_TIMEOUT))
		PrintDiag(DIAG_VERBOSE,hAPI, S("Ignoring baseband status request during chipset operations.\n"));

	else if (BeceemAPI_IsActivated(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT))
		PrintDiag(DIAG_VERBOSE,hAPI, S("Ignoring baseband status request while waiting for shutdown.\n"));

	else if (hAPI->u32State == STATE_IDLE)
		BeceemAPI_IdleModeStatusRequest(hAPI);

	else if (hAPI->u32State == STATE_SHUTDOWN)
		BeceemAPI_ShutdownStatusRequest(hAPI);

	else if (hAPI->u32State == STATE_HMC_SHUTDOWN)		
		BeceemAPI_HalfMiniCardStatusRequest(hAPI);

	else
		BeceemAPI_LinkStatusRequest(hAPI);

	BeceemAPI_Activate(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT);

	} // BasebandStatusRequest

// -------------------------------------------------------------------------
// void BeceemAPI_IdleModeRequest(BeceemAPI_t hAPI, BOOL bEnterIdleMode)
// 
// Enter or leave idle mode:
//     bEnterIdleMode is TRUE  -> Enter idle mode
//     bEnterIdleMode is FALSE -> Leave idle mode
//
void BeceemAPI_IdleModeRequest(BeceemAPI_t hAPI, BOOL bEnterIdleMode)
	{

	INT32 iLen;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI)== OPEN_DEVICE_FAILURE)
		return;

	if (bEnterIdleMode)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending idle mode request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_IDLE_MODE, SUBTYPE_IDLE_MODE_REQ, 0);
		}
	else
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending idle mode wakeup request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_IDLE_MODE, SUBTYPE_IDLE_MODE_WAKEUP_REQ, 0);
		}
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	} // BeceemAPI_IdleModeRequest

// -----------------------------------------------------------------------------------------
// void BeceemAPI_IdleModeStatusRequest(BeceemAPI_t hAPI)
//
// - Requests the idle mode status
//
void BeceemAPI_IdleModeStatusRequest(BeceemAPI_t hAPI)
	{

	INT32 iLen;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI)== OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending idle mode status request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_IDLE_MODE, SUBTYPE_IDLE_MODE_STATUS_REQ, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	} // IdleModeStatusRequest

// -----------------------------------------------------------------------------------------
// void BeceemAPI_LinkStatusRequest (BeceemAPI_t hAPI)
//
// - Requests the network entry status
//
void BeceemAPI_LinkStatusRequest(BeceemAPI_t hAPI)
	{

	INT32 iLen;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI)== OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_VERBOSE, hAPI, S("\n***** Sending link status request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_ENTRY, SUBTYPE_NETWORK_ENTRY_STATUS_REQ, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	} // LinkStatusRequest

// -----------------------------------------------------------------------------------------
// void BeceemAPI_ShutdownStatusRequest(BeceemAPI_t hAPI)
//
// - Requests the shutdown status
//
void BeceemAPI_ShutdownStatusRequest(BeceemAPI_t hAPI)
	{

	INT32 iLen;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL, hAPI, S("\n***** Sending shutdown status request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_SHUTDOWN, SUBTYPE_SHUTDOWN_MODE_STATUS_REQ, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	} // ShutdownStatusRequest

// -----------------------------------------------------------------------------------------
// void BeceemAPI_HalfMiniCardStatusRequest(BeceemAPI_t hAPI)
//
// - Requests the half-mini card status
//
void BeceemAPI_HalfMiniCardStatusRequest(BeceemAPI_t hAPI)
	{

	INT32 iLen;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL, hAPI, S("\n***** Sending half-mini card status request (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_HALF_MINI_CARD, SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	} // HalfMiniCardStatusRequest

// -----------------------------------------------------------------------------------------
// void GetSSInfoRequest (void)
//
// - Requests the subscriber station information
//
void BeceemAPI_GetSSInfoRequest(BeceemAPI_t hAPI)
	{

	INT32 i32Len;
	WIMAX_MESSAGE stRequestMessage;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	if (BeceemAPI_IsRequestBlocked(hAPI, S("Device information")))
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending subscriber station information request\n"));
	i32Len = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_SS_INFO, SUBTYPE_SS_INFO_REQ, 0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, i32Len);

	} /* BeceemAPI_GetSSInfoRequest */

// -----------------------------------------------------------------------------------------
// void GetStatisticsMIBsRequest(void)
//
// - Requests the statistics information, updated at a periodic
//
void BeceemAPI_GetStatisticsMIBsRequest(BeceemAPI_t hAPI, BOOL abUseRpt[])
	{

	UINT32 u32Idx;
	UINT16 u16NumMibObjs = 0;
	UINT8  *pu8BaseAddr = 0;
	UINT32 u32ReqMsgLen = 0;
	MIB_OBJECT_INFO *pstMIBObj;
	WIMAX_MESSAGE stRequestMessage;
	hMibData_t hStatisticsReport;
	int i;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	if (BeceemAPI_IsRequestBlocked(hAPI, S("Link statistics")))
		return;

	memset(&stRequestMessage, 0, sizeof(WIMAX_MESSAGE));

	stRequestMessage.usType    = TYPE_MIB;
	stRequestMessage.usSubType = SUBTYPE_MULTIPLE_MIB_GET_REQ;
	pu8BaseAddr = &stRequestMessage.szData[0];

	u32ReqMsgLen = sizeof(UINT16);  // leave room for number of MIB objects

	for (i = 0; i < MAX_STATS_REPORT; i ++)
		{
		if (abUseRpt[i])
			{
			hStatisticsReport = hAPI->hLinkStatsReport[i].hStatisticsReport;

			for (u32Idx = 0; hStatisticsReport[u32Idx].bszMIBDescription != NULL; u32Idx++)
				{
				// Skip any heading control lines
				if (hStatisticsReport[u32Idx].i32MibID < 0)
					continue;

				// Store next object ID
				pstMIBObj = (PMIB_OBJECT_INFO) (pu8BaseAddr + u32ReqMsgLen);
				pstMIBObj->u32MibObjId = hStatisticsReport[u32Idx].i32MibID;
				u32ReqMsgLen += sizeof(MIB_OBJECT_INFO);
				u16NumMibObjs++;
				}
			}
		}

	// Store MIB object count
	memcpy(pu8BaseAddr, &u16NumMibObjs, sizeof(UINT16));

	u32ReqMsgLen += (2*sizeof(UINT16)); // room for type & subtype
	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending Multiple Get MIB request - len = %lu\n"), u32ReqMsgLen);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, u32ReqMsgLen);

	} // GetStatisticsMIBsRequest 

// -----------------------------------------------------------------------------------------
// void SendEAPCompleteRequest (BeceemAPI_t hAPI, UINT32 u32EAP_status, UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len)
//
// - Transfer an EAP complete packet to the BS
//
void BeceemAPI_EAPCompleteRequest(BeceemAPI_t hAPI, UINT32 u32EAP_status, UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len)
	{

	WIMAX_MESSAGE stRequestMessage;
	PEAP_COMPLETE_INFO pstEAPCompleteInfo;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	pstEAPCompleteInfo = (PEAP_COMPLETE_INFO) stRequestMessage.szData;
	if (u32EAP_msg_len <= (MAX_VARIABLE_LENGTH-16))
		{	
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_AUTHENTICATION, SUBTYPE_EAP_COMPLETE_REQ, 
			(UINT16) (sizeof(EAP_COMPLETE_INFO) + u32EAP_msg_len - MAX_EAP_PACKET_LENGTH));

		pstEAPCompleteInfo->ucStatus = (UINT8) u32EAP_status;
		pstEAPCompleteInfo->usEAPCompletePayloadLength = (UINT16) u32EAP_msg_len;
		memcpy(pstEAPCompleteInfo->aucEAPCompletePayload, u8EAP_msg, u32EAP_msg_len);

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending EAP complete request (EAP status = %d)\n"), u32EAP_status);
		BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);
		}
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Requested EAP complete with packet too long\n"));
		return;
		}

	} // SendEAPCompleteRequest (...

// -----------------------------------------------------------------------------------------
// void EAPTransferRequest (const UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len)
//
// - Transfer an EAP packet to the BS
//
void BeceemAPI_EAPTransferRequest(BeceemAPI_t hAPI, const UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len)
	{

	WIMAX_MESSAGE stRequestMessage;
	PEAP_PAYLOAD pstEAPPayload;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	pstEAPPayload = (PEAP_PAYLOAD) stRequestMessage.szData;

	if (u32EAP_msg_len <= MAX_EAP_PACKET_LENGTH)
		{
		iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_AUTHENTICATION, SUBTYPE_EAP_TRANSFER_REQ, 
			(UINT16) (sizeof(EAP_PAYLOAD) + u32EAP_msg_len - MAX_EAP_PACKET_LENGTH));
		pstEAPPayload->usEAPPayloadLength = (UINT16) u32EAP_msg_len;
		memcpy(pstEAPPayload->aucEAPPayload, u8EAP_msg, u32EAP_msg_len);

		PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending EAP transfer request\n"));
		BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);
		}
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Requested EAP packet transfer too long\n"));
		return;
		}

	} // SendEAPTransferRequest (...

#ifdef SUBTYPE_EAP_SPOOF_INFO_REQ
// -----------------------------------------------------------------------------------------
// void EAPNaiSpoofInfoRequest (const UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len)
//
// - Transfer an EAP packet to the BS, with a request to change the inner NAI
//
void BeceemAPI_EAPNaiSpoofInfoRequest(BeceemAPI_t hAPI, const UINT8 *au8EAP_msg, UINT32 u32EAP_msg_len)
	{

	WIMAX_MESSAGE stRequestMessage;
	EAP_PAYLOAD stEAPPayload;
	UINT16 u16MsgLen;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	// Build the payload
	if (!BeceemAPI_EAP_BuildNaiSpoofInfoRequestPayload(hAPI, 
		stEAPPayload.aucEAPPayload, MAX_EAP_PACKET_LENGTH, &stEAPPayload.usEAPPayloadLength,
		au8EAP_msg, u32EAP_msg_len))
		{
		return;
		}

	u16MsgLen = (UINT16) (sizeof(EAP_PAYLOAD) + stEAPPayload.usEAPPayloadLength - MAX_EAP_PACKET_LENGTH);
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_AUTHENTICATION, SUBTYPE_EAP_SPOOF_INFO_REQ, u16MsgLen);
	memcpy(stRequestMessage.szData, &stEAPPayload, u16MsgLen);

	PrintDiag(DIAG_VERBOSE, hAPI, S("\nEAP_SPOOF_INFO_REQ message dump: (len = 0x%04X)"), iLen);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, (UINT8 *) &stRequestMessage, iLen);

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending EAP NAI Spoof Info request\n"));
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

	} // SendEAPNaiSpoofInfoRequest (...
#endif

// -----------------------------------------------------------------------------------------
// void MACManagementMessageLoggingRequest (BeceemAPI_t hAPI, BOOL bStart)
//
// - Sends MAC management message logging start/stop requests
//
void BeceemAPI_MACManagementMessageLoggingRequest(BeceemAPI_t hAPI, BOOL bStart)
{

	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending MAC message logging request: ") B_SFNL, (bStart ? "START" : "STOP"));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
		TYPE_MACMGMT, 
		bStart ? SUBTYPE_MACMGMT_ENABLE_REQ : SUBTYPE_MACMGMT_DISABLE_REQ,
		0);
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen);

	}

// -----------------------------------------------------------------------------------------
// void ChipsetResetRequest (BeceemAPI_t hAPI) 
//
// - Resets the baseband chip set
//
void BeceemAPI_ChipsetResetRequest (BeceemAPI_t hAPI)
{

	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending Chipset Reset Request\n"));

	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_RESET_REQ, 0);

	if (BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen) == SUCCESS)
		{
		BeceemAPI_Activate(hAPI, ID_EVENT_CHIPSET_RESET_TIMEOUT);
		hAPI->eChipsetControlStatus = ChipsetResetInProgress;  // Must come after the BeceemAPI_SendWiMAXMessage!
		}
	else
		hAPI->eChipsetControlStatus = ChipsetIdle;

} /* BeceemAPI_ChipsetResetRequest */

// -----------------------------------------------------------------------------------------
// void FirmwareDownload (BeceemAPI_t hAPI, UINT32 u32ImageType, STRING bszFilePath) 
//
// - Initiates a config file or firmware download
//
#define MIN(a,b) ((a) < (b) ? (a) : (b))

void BeceemAPI_FirmwareDownloadRequest (BeceemAPI_t hAPI, UINT32 u32ImageType, STRING bszFilePath)
{

	WIMAX_MESSAGE stRequestMessage = {0};
	PST_FIRMWARE_DNLD pFirmwareDownload = NULL;
	int iLen = 0, iFPLen;
	char *sp;
	int n, sLen = 0;
#ifdef MS_VISUAL_CPP
	int iRet;
#endif

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	pFirmwareDownload = (PST_FIRMWARE_DNLD) stRequestMessage.szData;

	PrintDiag(DIAG_NORMAL, hAPI, 
		S("\n***** Sending Firmware or Config File Download Request for file = ") B_SFNL, 
		pFirmwareDownload->szImageName);

	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_CHIPSET_CONTROL, SUBTYPE_FIRMWARE_DNLD_REQ, sizeof(ST_FIRMWARE_DNLD));

	pFirmwareDownload->u32ImageType = u32ImageType;
	sp = (char *) pFirmwareDownload->szImageName;
	iFPLen = (int) b_strlen(bszFilePath);
	sLen = (int) MIN(sizeof(pFirmwareDownload->szImageName) - 1, iFPLen);
#ifdef MS_VISUAL_CPP
	for (n=0; n<sLen; n++)
		wctomb_s(&iRet, &sp[n], sizeof(sp[n]), bszFilePath[n]);
#else
  #ifdef CSCM_ANDROID
	memcpy(sp, bszFilePath, strlen(bszFilePath));
  #else	
	for (n=0; n<sLen; n++)
		wctomb(&sp[n], bszFilePath[n]);
  #endif
#endif
	sp[sLen] = '\0';

	if (BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen) == SUCCESS)
		{
		BeceemAPI_Activate(hAPI, ID_EVENT_FW_DOWNLOAD_TIMEOUT);
		hAPI->eChipsetControlStatus = 
			u32ImageType == IMAGE_CONFIG_FILE ? ChipsetConfigFileDownloadInProgress :
												ChipsetFirmwareDownloadInProgress;
		}
	else
		hAPI->eChipsetControlStatus = ChipsetIdle;

	} /* BeceemAPI_FirmwareDownloadRequest */

// -----------------------------------------------------------------------------------------
// void ShutdownRequest (void) 
//
// - Shuts down baseband chip 
//
void BeceemAPI_ShutdownRequest (BeceemAPI_t hAPI)
	{

	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending Shutdown Request\n"));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_SHUTDOWN, SUBTYPE_SHUTDOWN_REQ, 0);
	if(BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *)&stRequestMessage, iLen) == SUCCESS)
		BeceemAPI_Activate(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT);
	} /* BeceemAPI_ShutdownRequest */

/*This function will be modified to read the list of CFs, threshold, NAP ID from the config file in the future release */
BOOL BeceemAPI_ShutDownWithLPSM(BeceemAPI_t hAPI)
{
	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return FALSE;
	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending shut down with Low power mode scan request\n"));
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_LINK_DOWN, SUBTYPE_SYNC_DOWN_AUTONOMOUS_SCAN_REQ, 0);
	//iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_SHUTDOWN, SUBTYPE_SHUTDOWN_REQ, 0);
	if(BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen) == SUCCESS)
		return TRUE;
	return FALSE;
}


BOOL BeceemAPI_ConfigLPSMParamsRequest(BeceemAPI_t hAPI)
{
	ST_CAPL_BASED_SEARCH_PARAMS lpsmParams;

	WIMAX_MESSAGE stRequestMessage;
	INT32 iLen;
	
	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
			return FALSE;
	
	if(!BeceemCscmOEM_GetCAPLParams(&lpsmParams))
	{
		PrintDiag(DIAG_VERBOSE, hAPI, S("\n***** ERROR: CAPL Parameters not configured! \n"));		
		return FALSE;
	}
	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, TYPE_NETWORK_SEARCH, 
								  SUBTYPE_AUTONOMOUS_SEARCH_REQ, sizeof(ST_CAPL_BASED_SEARCH_PARAMS));
	memcpy(stRequestMessage.szData, (void *) &lpsmParams, sizeof(lpsmParams));

	PrintDiag(DIAG_VERBOSE, hAPI, S("\n***** Sending config request for LPSM (") B_SF S(")\n"), BeceemAPI_DateTime(hAPI));		
	if(BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen) == SUCCESS)
		return TRUE;
	else
		return FALSE;

}

// -----------------------------------------------------------------------------------------
// void SetWiMAXOptionsRequest (void) 
//
// - Sets WiMAX library options 
//
void BeceemAPI_SetWiMAXOptionsRequest (BeceemAPI_t hAPI, UINT32 u32OptionSpecifier, UINT32 u32Payload)
	{

	WIMAX_MESSAGE stRequestMessage;
	ST_WIMAX_OPTIONS *pstWiMAXOptions;
	UINT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	if (BeceemAPI_IsRequestBlocked(hAPI, S("SetWiMAXOptions")))
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending SetWiMAXOptions request with option = 0x%02X payload = 0x%08X\n"), 
		u32OptionSpecifier, u32Payload);

	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
		TYPE_SET_WIMAX_OPTIONS, SUBTYPE_SET_WIMAX_OPTIONS_REQ, sizeof(ST_WIMAX_OPTIONS));
	pstWiMAXOptions = (PST_WIMAX_OPTIONS) stRequestMessage.szData;
	pstWiMAXOptions->u8WimaxOptionSpecifier = (UINT8) u32OptionSpecifier;
	pstWiMAXOptions->u32WimaxOptionsPayload = u32Payload;
		
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

	BeceemAPI_Activate(hAPI, ID_EVENT_WIMAX_OPTIONS_TIMEOUT);

	} /* BeceemAPI_SetWiMAXOptionsRequest */

#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST
// -----------------------------------------------------------------------------------------
// void EAPEncryptPrivateRequest (void) 
//
// - Encrypts a message with the private key
//
void BeceemAPI_EAPEncryptPrivateRequest (BeceemAPI_t hAPI, 	
			UINT32 u32Padding, const UINT8 u8AESKey[32], const UINT8 u8Data[], UINT32 u32DataLen)
	{

	WIMAX_MESSAGE stRequestMessage;
	ST_EAP_RSA_PRIVATE *pstEAPRsaPrivate;
	UINT32 iLen;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending EAPEncryptPrivateRequest request (") B_SF S(")\n"),
		BeceemAPI_DateTime(hAPI));

	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
		TYPE_AUTHENTICATION, SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST, (UINT16) (sizeof(ST_EAP_RSA_PRIVATE) + u32DataLen - 1));

	pstEAPRsaPrivate = (PST_EAP_RSA_PRIVATE) stRequestMessage.szData;
	memset(pstEAPRsaPrivate, 0, sizeof(ST_EAP_RSA_PRIVATE) + u32DataLen - 1);
	pstEAPRsaPrivate->u32Padding = u32Padding;
	memcpy(pstEAPRsaPrivate->u8AESkey, u8AESKey, PRIV_KEY_AES_KEY_LEN);
	memcpy(pstEAPRsaPrivate->u8Data,   u8Data,   u32DataLen);
	pstEAPRsaPrivate->u32DataLen = u32DataLen;
		
	BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);

	} /* BeceemAPI_EAPEncryptPrivateRequest */
#endif

/*Request the Certs and Keys from the API LIBRARY. */

void BeceemAPI_EAP_GetSecCredsRequest(BeceemAPI_t hAPI, UINT32 uSecParamId)
	{

	WIMAX_MESSAGE stRequestMessage;
	SEC_OBJECT_INFO *pSecReq;
	UINT32 iLen, sendResult;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Sending BeceemAPI_EAP_GetSecCredsRequest request (") B_SF S(")\n"),
		BeceemAPI_DateTime(hAPI));

	iLen = BeceemAPI_FillWiMAXMsg(&stRequestMessage, 
		TYPE_SECURITY, SUBTYPE_SECURITY_GET_REQ, (UINT16) (sizeof(SEC_OBJECT_INFO)));

	pSecReq = (SEC_OBJECT_INFO*) stRequestMessage.szData;
	memset(pSecReq, 0, sizeof(SEC_OBJECT_INFO));
	pSecReq->u32SecObjId = uSecParamId;
		
	sendResult = BeceemAPI_SendWiMAXMessage(hAPI, (UINT8 *) &stRequestMessage, iLen);
	if(sendResult != SUCCESS)
		PrintDiag(DIAG_ERROR,hAPI, S("\n***** BeceemAPI_EAP_GetSecCredsRequest sending Failed!!!"));

	} /* BeceemAPI_EAPEncryptPrivateRequest */


// ===============================================================================================
// Helper functions for sending requests
// ===============================================================================================

// -----------------------------------------------------------------------------------------
// INT32 BeceemAPI_FillWiMAXMsg (PWIMAX_MESSAGE pWMMsg, UINT16 u16MessageType, UINT16 u16SubType, UINT16 u16MsgSize)
//
// - Fills a WiMAX message structure, given the type, subtype, and size values
//
// Returns length of message

static INT32 BeceemAPI_FillWiMAXMsg(
					PWIMAX_MESSAGE pWMMsg, 
					UINT16 u16MessageType,
					UINT16 u16SubType, 
					UINT16 u16MsgSize
				)
{
	if(pWMMsg == NULL)
		return -1;

	pWMMsg->usType    = u16MessageType;
	pWMMsg->usSubType = u16SubType;
	memset (pWMMsg->szData, 0, MAX_VARIABLE_LENGTH);

	return sizeof(WIMAX_MESSAGE) + u16MsgSize - MAX_VARIABLE_LENGTH;

} // BeceemAPI_FillWiMAXMsg

// -----------------------------------------------------------------------------------------
// INT32 BeceemAPI_SendWiMAXMessage(void *pvHandle, UINT8 *pu8SendBuffer, UINT32 u32Length)
//
//    - Sends a request to the driver
//
// See below for an explanation of the possible return codes.

static INT32 BeceemAPI_SendWiMAXMessage(BeceemAPI_t hAPI,
								  UINT8 *pu8SendBuffer,
								  UINT32 u32Length) {

		INT32 i32RetCode;
		STRING bszErrorMsg = NULL;
		B_CHAR bszBuffer[100];

		if (!hAPI)
			return CSCM_ERROR_NULL_POINTER; 

		// Don't print periodic link status requests
		if (hAPI->u32CSCMDebugLogLevel >= DIAG_VERBOSE ||
			!(u32Length == 4 && pu8SendBuffer[0] == 2 && pu8SendBuffer[2] == 3))
			{
			PrintDiag(DIAG_VERBOSE, hAPI, S("Sent: "));
			StringHexDump(DIAG_VERBOSE, hAPI, FALSE, pu8SendBuffer, u32Length);
			}

		if (hAPI->eChipsetControlStatus != ChipsetIdle) {
			PrintDiag(DIAG_NORMAL, hAPI, S("\n***** Ignoring BeceemAPI_SendWiMAXMessage request during chipset operations *****\n"));
			return ERROR_FIRMWARE_DOWNLOAD_INPROGRESS; 
			}

		// Serialize calls with a mutex
		WcmMutex_Wait(hAPI->hMutexSendWiMAXMessage);
#ifdef WIN32
		i32RetCode = lpfnDllSendWiMAXMessage(hAPI->hDevice, pu8SendBuffer, u32Length);
#else
		i32RetCode = SendWiMAXMessage(hAPI->hDevice, pu8SendBuffer, u32Length);
#endif
		WcmMutex_Release(hAPI->hMutexSendWiMAXMessage);

		if (i32RetCode == SUCCESS) {
			PrintDiag(DIAG_VERBOSE, hAPI, S("***** Message sent successfully\n"));
			}
		else 
			{
			switch (i32RetCode) 
				{

				case SUCCESS:
					break;

				case ERROR_INVALID_DEVICE_HANDLE:
					StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("Invalid device handle = 0x%08X.\n"), hAPI->hDevice);
					bszErrorMsg = bszBuffer;
					break;

				case ERROR_INVALID_BUFFER:
					bszErrorMsg = S("Invalid buffer pointer.");
					break;

				case ERROR_INVALID_STATS_LEN:
					bszErrorMsg = S("Invalid statistics length.");
					break;

				case ERROR_INVALID_BUFFER_LEN:
					bszErrorMsg = S("Invalid buffer length.");
					break;

				case ERROR_FIRMWARE_DOWNLOAD_INPROGRESS:
					bszErrorMsg = S("Firmware download in progress. Please try later.");
					break;

				case ERROR_SYNC_UP_INPROGRESS:
					bszErrorMsg = S("Sync up in progress.");
					break;

				case ERROR_NET_ENTRY_INPROGRESS:
					bszErrorMsg = S("Network entry in progress.");
					break;

				case ERROR_NET_SEARCH_INPROGRESS:
					bszErrorMsg = S("Network search in progress.");
					break;

				case ERROR_SS_IS_INSYNC:
					bszErrorMsg = S("Subscriber station is already in sync.");
					break;

				case ERROR_AUTH_CRC_MISMATCH:
					bszErrorMsg = S("Firmware does not support MIBS.");
					break;

				case ERROR_DEVICEIOCONTROL_FAILED:
					bszErrorMsg = S("Device I/O control request failed.");
					break;

				case ERROR_MODEM_IN_SHUT_DOWN_MODE:
					bszErrorMsg = S("Modem is in shutdown mode.");
					break;

				case ERROR_MODEM_IN_IDLE_MODE:
					bszErrorMsg = S("Modem is in idle mode.");
					break;

				case ERROR_DEVICE_NOT_IN_HMC_SHUTDOWN:
					bszErrorMsg = S("Device is not in HMC shutdown mode (info message)");
					break;

#ifdef ERROR_DISCRETEFREQS_NOT_IN_SAME_RANGE
				case ERROR_DISCRETEFREQS_NOT_IN_SAME_RANGE:
					bszErrorMsg = S("Invalid discrete center frequency search list");
					break;
#endif

#ifdef ERROR_CF_STEP_INVALID
				case ERROR_CF_STEP_INVALID:
					bszErrorMsg = S("Network search center frequency step is invalid.");
					break;
#endif

				default:
					StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
						S("Unknown WiMAX message return code = %ld."), i32RetCode);
					bszErrorMsg = bszBuffer;
					break;

				}
			ServerSocketResponse(hAPI, ID_MSG_ERROR, bszErrorMsg, TRUE);
			if (i32RetCode == ERROR_DEVICE_NOT_IN_HMC_SHUTDOWN)
				{
				PrintDiag(DIAG_VERBOSE, hAPI, S("BeceemAPI_SendWiMAXMessage return code: ") B_SFNL, bszErrorMsg);
				PrintDiag(DIAG_VERBOSE, hAPI, S("***** Message sent successfully\n"));
				}
			else
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR - BeceemAPI_SendWiMAXMessage return code: ") B_SFNL, bszErrorMsg);
				PrintDiag(DIAG_ERROR, hAPI, S("***** Message sent with ERRORS\n"));
				}
			}

		Print_Flush(hAPI->hPrintDiag);

		return i32RetCode;

	} // BeceemAPI_SendWiMAXMessage

// -----------------------------------------------------------------------------------------
// BOOL BeceemAPI_IsRequestBlocked(BeceemAPI_t hAPI, STRING bszRequestType)
//
//    - Return TRUE if state is IDLE, SHUTDOWN, or HMC_SHUTDOWN 
//

static BOOL BeceemAPI_IsRequestBlocked(BeceemAPI_t hAPI, STRING bszRequestType)
	{

	STRING bszMode = NULL;

	if (hAPI->u32State == STATE_IDLE)
		bszMode = S("Idle");
	else if (hAPI->u32State == STATE_SHUTDOWN)
		bszMode = S("Shutdown");	
	else if (hAPI->u32State == STATE_HMC_SHUTDOWN)
		bszMode = S("Half-mini Card Shutdown");

	if (bszMode != NULL)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("\nWarning: ") B_SF S(" is not available in ") B_SF S(" Mode.\n"), 
			bszRequestType, bszMode);
		return TRUE;
		}
	else
		return FALSE;

	} /* BeceemAPI_IsRequestBlocked */

