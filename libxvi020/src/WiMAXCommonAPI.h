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
 * Description	:	CAPI 1.2 declarations.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_WIMAX_COMMONAPI_H
#define BECEEM_WIMAX_COMMONAPI_H

#include "BeceemWiMAX.h"
#ifdef WIN32
#include "Typedefs.h"
#else
#include "Typedefs.h"
#include "WiMaxType.h"
#include "WiMaxError.h"
#endif
#include "BeceemWiMAXExport.h"

#include "mruEEPROM.h"

#ifndef MACOS
#include "socketipc.h"
#endif

/*
versions
1 - 8 initial base releases with bug fixes from IOT.
9. Bug fix for Bug ID:4049,4050,4051
10. Reverting back the Network Search changes.
11. Adding Search thread & Bug fixes 
12. Widescan API implementation
13. Corrections to search and search after disconnect. Statistics number packets recevied is corrected.
14. Total operators found is reset after RF-OFF.Getnetworklist check is added see if the modem is in RF OFF state.
 Use SUBTYPE_SYNC_DOWN_REQ instead of SUBTYPE_SYNC_DEREG_REQ for disconnect command.
15.WiMAXCommonAPI_IssueSearchRequest fails for any reason it should send search abort and try search once more. 
16. MRU based search implementation and MIB added for device,connection status and failure reasons
17.Bug fixes for MRU search 
18.Checking if MRU entries are zero, so that those entries are not added to search payload.
19.Sync down before search start. So that baseband if by any chance in phy sync mode(transcient state) puts it into search mode.
20.Compiled out widescan search.
21. Fix for WIMAX_API_DEVICE_ID::deviceIndex.
22. Indication to CM for periodic wake up 
23. Fixes for Bugs 5958, 6003,6083,6086
24. Changes to RSSI and CINR threshold for search
25. Fixed SharedMem_Open return value improper in WiMAXCommonAPI_ReadSharedMemory.
26. bug fix for MRU reset 6641.
27. New debug prints added
28. Bug fix for shutdown mode (6463).
29. BlackListing BS implementation.
30. Any = 2 support for Search and filtering.
31. RF ON/OFF issue fix.
32. Fix for bug id 7406
33. Fix for bug id 7626 
34. Added BLACK_LISTING_BS_ENABLED to enable/disable Blacklisting BS feature
35. fix for 7644 
36. Fix for 8069
*/
#define COMMON_API_VERSION 10

/*Temporary Settings and information START*/
#define NSP_NAME   "SPRINT NETWORK"
#define BCM_DEVICE_NAME "Beceem Communications Inc. Tarang3"


#define BCM_HW_NAME "Beceem Communications"
#define BCM_SW_NAME "Beceem Communications"
#define BCM_ASIC_NAME "Beceem Communications"
#define BCM_RF_NAME "Beceem Communications"
#define DRIVER_PATH "\\Drivers\\Macxvi200.bin"
#define DRIVER_PATH_350 "\\Drivers\\Macxvi350.bin"
#define BCM_REGISTRY_KEY_CAPI "SOFTWARE\\WiMax\\CommonAPI\\Libs\\Beceem Communications\\WiMAX Devices"
#define BCM_KEY_EXTENSION_CAPI "Extension"
#define BCM_KEY_EXTENSION_VALUE_CAPI "libxvi010.dll"
#define BCM_KEY_PATH_CAPI "Path"
#define SEARCH_STATUS_PERCENTAGE 100
/*Temporary Settings and information END*/

/* MIB Version for Periodic Wakeup Timer Fetching */
#define PERIODIC_TIMER_SUPPORTED_MIB_VER	0x12

#define MINIMUM_SHUTDOWN_TIMEOUT	60000
#define DEFAULT_SHUTDOWN_TIMEOUT	300000
#define MRUSEARCH_TIMEOUT			60000
#define SYNC_RESPONSE_TIMEOUT        5000
#define ABORT_SEARCH_TIMEOUT        500
#define SYNC_DOWN_REQ_TIMEOUT        10000
/*
CINR threshold = +6dB
RSSI threshold = -93dBm
If required should be fine tuned to different value as is right for the deployment.
*/
#define CINR_THRESHOLD 6
#define RSSI_THRESHOLD -93

#define FIRST_FREQ 1
#define SECOND_FREQ 2
#define LAST_FREQ 3

/*-------------------Temporary settings for wide scan network search START-------------------*/
#define NUMBER_OF_BW_TOSCAN   2
#define BW1_FOR_WIDESCAN_SEARCH 10000
#define BW2_FOR_WIDESCAN_SEARCH 5000
#define WIDESCAN_START_FREQ   (2496.0  * 1000)
#define WIDESCAN_END_FREQ     (2690.0  * 1000)
#define WIDESCAN_FREQ_STEP     250

/*-------------------Temporary settings for wide scan network search END-------------------*/

/*----------------------------------------OMA-DM Related parameters for n/w search/entry START----------------------------------------------*/

/*Network search related changes for common API*/

#define MAX_BASESTATIONS_POSSIBLE_PER_NAP_ID 24	/*To be confirmed beceem internally */
#define MAX_PRIORITY_POSSIBLE     250
#define FIRMWARE_DOWNLOAD_TIMEOUT         10000
#define CAPL_SEARCH_TIMER 50000000

#define MAX_NUM_REQUESTS()((1400 -(sizeof(ST_CAPL_BASED_SEARCH_PARAMS)-(sizeof(ST_PHY_INFO) * MAX_NUM_CHANNELS)))/sizeof(ST_PHY_INFO))
#define NUM_CHANNELS_PER_CHANNELPLAN 3
#define TAIL_MRU_INDEX 0x3F


#define MAX_NUMBER_OF_BLACKLISTED_BS 6
#ifdef BLACK_LISTING_BS_ENABLED
typedef struct BLACK_LIST_BS_ENTRIES {
    UINT32 u32NumBS;
    UINT8 u8ChPlanIndxMapping[MAX_NUMBER_OF_BLACKLISTED_BS + 2];
    ST_DETECTED_BS_INFO bsInfo[MAX_NUMBER_OF_BLACKLISTED_BS];

} BLACK_LIST_BS_ENTRIES, *PBLACK_LIST_BS_ENTRIES;
#endif
typedef struct _NW_SEARCH_RESP_ {
    UINT32 u32NAP_ID;
    UINT32 u32TotalBaseStationsFound;
    UCHAR OperatorName[MAX_SIZE_OPERATORNAME];
    BSINFO stBSINFO_Entries[MAX_BASESTATIONS_POSSIBLE_PER_NAP_ID];
    UINT32 u32NSPID;
} NW_SEARCH_RESP, *PNW_SEARCH_RESP;

typedef struct _PRIORITIZED_CAPL_ENTRIES_LIST {
    ST_CAPLEntries stCAPLENTRIES[MAX_CAPL_ENTRIES_PER_NSP];
    UINT32 u32Count;
} PRIORITIZED_CAPL_ENTRIES_LIST, *PPRIORITIZED_CAPL_ENTRIES_LIST;

typedef struct _PRESENT_CAPL_NAPID_IN_SEARCH {
    UINT32 u32PRESENT_NAP_ID;
    UINT32 u32PRESENT_CAPL_ENTRY_IN_SEARCH;
    UINT32 u32OperatorIndex;
} PRESENT_CAPL_NAPID_IN_SEARCH, *PPRESENT_CAPL_NAPID_IN_SEARCH;

typedef struct _ST_ChannelPlanTable_ {
    ST_ChannelPlanEntries Entries;
    BOOL bFlagIncludedInSearchReq;
} ST_ChannelPlanTable, *PST_ChannelPlanTable;

typedef struct _MRU_ENTRY {
    UINT32 u32PrimaryFrequency;
    UINT32 u32Secondary1Frequency;
    UINT32 u32Secondary2Frequency;
    UINT8 u8WhichIsPrimary;
    UINT8 u8WhichIsSecondary1;
    UINT8 u8ChannelPlanIndex;
    UINT8 NextMruEEPROMIndex;
    struct _MRU_ENTRY *pNext;
    struct _MRU_ENTRY *pPrev;
    UINT8 PrevMruEEPROMIndex;
    UINT8 u8CurrIndex;
    UINT8 u8padding[2];
} MRU_ENTRY, *PMRU_ENTRY;


typedef struct _MRUTable {
    UINT32 u32NumMruEntries;
    PMRU_ENTRY mruHead;
    MRU_ENTRY stMruEntries[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];

} MRU_TABLE_OBJECT, *PMRU_TABLE_OBJECT;

typedef struct ST_NAPID_CAPL {
    UINT32 au32NAPIDList[MAX_CAPL_ENTRIES_PER_NSP];
    UINT32 u32NAPIDCount;
    UINT32 u32Any;
} ST_NAPID_CAPL, *PST_NAPID_CAPL;

typedef struct _ST_SEARCH_REQ_DETAILS {
    UINT32 u32MruTableIndex;
    UINT32 u32PriorityIndex;
    UINT32 u32CAPLIndex;
    UINT8 u8MruSearchDone;
    UINT8 u8ChannelPlanIndex;
    UINT8 u8Valid;
    UINT8 u8Padding;

} ST_SEARCH_REQ_DETAILS, *PST_SEARCH_REQ_DETAILS;


/*----------------------------------------OMA-DM Related parameters for n/w search/entry END----------------------------------------------*/


typedef struct _BECEEM_COMMON_API_PERSISTANT_DATA {
    UINT32 u32NumberOfAppRunning;
    WIMAX_API_PRIVILEGE privilege;
} BECEEM_COMMON_API_PERSISTANT_DATA, *PBECEEM_COMMON_API_PERSISTANT_DATA;

typedef struct _CALLBACK_FNS_COMMON_API {
    IndConnectToNetwork pIndConnectToNetwork;
    IndControlPowerManagement pIndControlPowerManagement;
    IndDeviceInsertRemove pIndDeviceInsertRemove;
    IndDeviceStatusUpdate pIndDeviceStatusUpdate;
    IndDisconnectToNetwork pIndDisconnectToNetwork;
    IndNetworkSearchWideScan pIndNetworkSearchWideScan;
    IndPackageUpdate pIndPackageUpdate;
    IndProvisioningOperation pIndProvisioningOperation;
    IndNetworkSearch pIndNetworkSearch;
    MUTEX objCallbackMutex;

} CALLBACK_FNS_COMMON_API, *PCALLBACK_FNS_COMMON_API;

typedef struct _BECEEM_PROPRIETARY_CALLBACK {
    PFNBCMCALLBACK pAppCallback;
    PVOID pAppCallbackContext;
    MUTEX objCallbackMutex;
    USHORT usTYPE;
    USHORT usSUBTYPE;

} BECEEM_PROPRIETARY_CALLBACK, *PBECEEM_PROPRIETARY_CALLBACK;


typedef struct _BECEEM_COMMON_API_DEVICE_DATA {
    HANDLE hDevice;
    BECEEM_PROPRIETARY_CALLBACK stProprietaryCallback;

    WIMAX_API_DEVICE_ID stDeviceID;
    UINT32 u32NetworkCount;
    LINK_STATUS stLinkStatus;
    UINT32 u32CenterFrequency;
    UINT32 u32FlagForDeviceInsertMonitorRunning;
    WIMAX_API_CONNECTION_STAT stStatisticsInfo;
    WIMAX_API_DEVICE_INFO stDeviceInfo;
    WIMAX_API_ROAMING_MODE eRoamingMode;
    WIMAX_API_DEVICE_STATUS eDeviceStatus;
    WIMAX_API_STATUS_REASON eStatusReason;
    WIMAX_API_CONNECTION_PROGRESS_INFO eConnectionProgress;
    CALLBACK_FNS_COMMON_API stCallBackFunctionCommonAPIs;
    ST_NetworkParameters stNetworkParameters[WIMAXSUPP_OPERATOR_NUMBER_OF_NODE];
    P_ST_OperatorChannelPlanSettings pOperatorChannelPlanSettings;
    UINT32 u32SizeOfChannelPlanSettings;
    NW_SEARCH_RESP stNwSearchResp[MAX_CAPL_ENTRIES_PER_NSP];
    PPRIORITIZED_CAPL_ENTRIES_LIST pPrioritizedCAPL_LISTFor_WideScan;
    UINT32 u32TotalOperatorsFound;
    PRESENT_CAPL_NAPID_IN_SEARCH stCAPL_NAPID_IN_SEARCH;
    MRU_INIT_STRUCT g_mru_init_struct;
    UCHAR uchTransmitPower;
    BOOL bOnWideScanSearch;
    BOOL bOMADMInitFlag;
    BOOL unUsed;
    PPRIORITIZED_CAPL_ENTRIES_LIST pPrioritizedCAPL;
    PST_ChannelPlanTable pstChannelPlan;
    MRU_TABLE_OBJECT MruTable;
    /*ST_DETECTED_BS_INFO stDetectedBsDetails; */
    UINT32 u32PresentSyncup_CAPL_BSIndex;
    ST_CAPL_SEARCH_RSP stCaplSearchRsp;
    UINT8 u8ChPlanIndxMapping[64];
    ST_SEARCH_REQ_DETAILS stCaplSearchDetails;
    ST_NAPID_CAPL stNAPIDRequest;
    UINT32 u32SearchComplete;
    UINT32 u32RxvdCmdConnectToNetwork;
    UINT32 u32RxvdSyncRsp;
    UINT32 u32RfOffFromUser;
    UINT32 u32CenterFreq;
    SINT32 s32CINR;
    SINT32 s32RSSI;

#ifdef BLACK_LISTING_BS_ENABLED
    BLACK_LIST_BS_ENTRIES stBlackListedBSes;
#endif

#ifndef MACOS
    bcm_socket_t socketCAPIOMAIPC;
#endif

    THREAD stThreadNoDevNotifier;
    MUTEX stMutexForInterfaceAccess;
    UINT32 u32ConnectionStatusUpdate;
    WIMAX_API_PRIVILEGE privilege;
	SINT32 s32CINRThreshold;
	SINT32 s32RSSIThreshold;
} BECEEM_COMMON_API_DEVICE_DATA, *PBECEEM_COMMON_API_DEVICE_DATA;

#define PREVILIEGE_VALIDATION \
						do{\
							if(pDeviceData->privilege != pDeviceId->privilege)\
							{\
							    DPRINT_COMMONAPI(DEBUG_ERROR, "Permission denied @ %s and %d",__FUNCTION__,__LINE__); \
								return WIMAX_API_RET_PERMISSION_DENIED;\
							}\
						}while(0)
//Functions

void WiMAXCommonAPI_Cleanup(PBECEEMWIMAX pWiMAX);

int WiMAXCommonAPI_Init(PBECEEMWIMAX pWiMAX);
int WiMAXCommonAPI_ReadSharedMemory(PBECEEMWIMAX pWiMAX);


void WiMAXCommonAPI_WriteSharedMem(PBECEEMWIMAX pWiMAX);

void WiMAXCommonAPI_ReadSharedMem(PBECEEMWIMAX pWiMAX);

void WiMAXCommonAPI_BCMCommonAPICallback(PVOID context, PVOID pBCMMessage, UINT32 ulBCMMessageLength);
int WiMAXCommonAPI_FillWiMAXMsg(PWIMAX_MESSAGE pWMMsg,
				USHORT usMessageType, USHORT usSubType, USHORT usMsgSize);
int WiMAXCommonAPI_ShutDownResponse(PWIMAX_MESSAGE pWiMAXMsg);
VOID WiMAXCommonAPI_SearchResponse(PWIMAX_MESSAGE pWiMAXMsg);
WIMAX_API_RET WiMAXCommonAPI_SyncUpRequest();
VOID WiMAXCommonAPI_NetEntryExitResponse(PWIMAX_MESSAGE pWiMAXMsg);
VOID WiMAXCommonAPI_NetworkEntryRequest();
int WiMAXCommonAPI_SubscribeToStats(INT iPeriod);
VOID WiMAXCommonAPI_GetStatisticsResponse(PWIMAX_MESSAGE pstWiMAXMessage);
int WiMAXCommonAPI_GetSSInfo();
VOID WiMAXCommonAPI_SSInfoResponse(PWIMAX_MESSAGE pWiMAXMsg);
VOID WiMAXCommonAPI_FormatVersion(WIMAX_API_DEVICE_VERSION_P pVersion, const UINT32 * pVersionSSinfo,
				  const char *pName, const UINT32 FormatCount);
BOOL WiMAXCommonAPI_GetIPInterfaceIndex(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo);
PVOID WiMAXCommonAPI_CardInsertMonitorThreadFunc(PVOID pData);
PVOID WiMAXCommonAPI_NoCardNotificationThread(PVOID pData);
VOID WiMAXCommonAPI_SendNetworkSearchReq();
WIMAX_API_RET WiMAXCommonAPI_OpenNRegisterCallback(WIMAX_API_DEVICE_ID_P pDeviceId);
void WiMAXCommonAPI_UpdatePersistantDataOnClose();
void WiMAXCommonAPI_UpdateAccessrightsOnClose();
BOOL WiMAXCommonAPI_RegistryCreateKeys();
void WiMAXCommonAPI_ConvertStringToUnicode(char *pInput, UINT16 * pOutput);
BOOL WiMAXCommonAPI_FirmwareDownload();
VOID WiMAXCommonAPI_ChipsetResponse(PWIMAX_MESSAGE pWiMaxMsg);
VOID WiMAXCommonAPI_EnumerateAdapters();
int WiMAXCommonAPI_SendOMADMGetRequest(POMADM_OBJECT_INFO pOMADMReq);
int WiMAXCommonAPI_AbortSearch();
int WiMAXCommonAPI_LaunchSearchThread(PTHREADFUNC pSearchFunction);
PVOID WiMAXCommonAPI_NetworkSearchThread(PVOID pData);
BOOL WiMAXCommonAPI_WaitDuringSearch();
BOOL WiMAXCommonAPI_GetAdapterInfo(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo);
VOID WiMAXCommonAPI_CallWideScanCallback();
INT WiMAXCommonAPI_NwSearchPreconditioning();
UINT WiMAXCommonAPI_GetOMADMParameterSize(POMA_DM_TREE_LEVEL pstTestLevel, UINT uiLevel);
int WiMAXCommonAPI_GetNetworkParameterNode();
VOID WiMAXCommonAPI_IssueWideScanSearch(UINT32 u32FirstFreq, UINT32 u32LastFreq, UINT32 u32Step);
VOID WiMAXCommonAPI_GetBSInfoToConnect(PBSINFO pBSInfo, WIMAX_API_WSTRING nspName);
INT WiMAXCommonAPI_CheckIfBCAPIIsUsedByApp();
int WiMAXCommonAPI_PrioritizedCAPL_for_WideScan();
VOID WiMAXCommonAPI_SearchWidescanResponse(PWIMAX_MESSAGE pWiMaxMsg);
VOID WiMAXCommonAPI_DoDummySearch();
LINK_ERROR_CODES WiMAXCommonAPI_GetLinkStatus();
int WiMAXCommonAPI_InitCAPLTable();
VOID WiMAXCommonAPI_PopulateNetworkSearchReq();
int WiMAXCommonAPI_InitChannelPlanTable();
VOID WimaxCommonAPI_FillChannelPlanDetails(PUINT8 pu8RefID, PST_CHANNEL_SPECIFICATION pStChannelSpec);
VOID WimaxCommonAPI_ResetFlag();
VOID WimaxCommonAPI_InitMruTable();
int WimaxCommonAPI_GetChannelPlanIndex(UINT32 u32freq, UINT32 u32BandWidth, PUINT32 preamble,
				       PUINT8 pu8WhichIsPrimary);
VOID WimaxCommonAPI_ManipulateMruEntry(UINT32 u32freq, UINT8 u8ChannelPlanIndex,
				       PMRU_ENTRY pstMruEntry);
VOID WimaxCommonAPI_InitMruChannelPlanInfo(UINT8 u8WhichIsPrimary, UINT8 u8WhichIsSecondary1,
					   UINT8 u8MruTableIndex, UINT8 u8ChannelPlanIndex);
PVOID WiMAXCommonAPI_CaplSearchThread(PVOID pData);
VOID WiMAXCommonAPI_FillCAPLDetails(PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams);
VOID WimaxCommonAPI_UpdateMRUTable(UINT32 u32freq);
VOID WimaxCommonAPI_UpdateMruChangelist(PMRU_ENTRY pstMruEntry, PST_MRU_CHANGELIST pstMruChangeList);
VOID WiMAXCommonAPI_PrintMruTable();
VOID WiMAXCommonAPI_PrintEEPROMMruTable();
int WiMAXCommonAPI_ResetMRUTable(UINT8 u8MruResetFlag);
int WiMAXCommonAPI_SendMIBsForDeviceNLinkStatusNError();
VOID WiMAXCommonAPI_MIBIndication(PWIMAX_MESSAGE pWiMaxMsg);
VOID WiMAXCommonAPI_UpdateLinkStatus(UINT32 u32LinkStatus);
VOID WiMAXCommonAPI_UpdateConnectionStatus(UINT32 u32ConnectionStatus);
VOID WiMAXCommonAPI_UpdateDeviceStatus(UINT32 u32DeviceStatus);
int WiMAXCommonAPI_SendMIBGetRequest(PMIB_OBJECT_INFO pMIBReq);
PVOID WiMAXCommonAPI_PeriodicMIBRequestThread(PVOID pData);
INT32 WiMAXCommonAPI_StartSearch();
VOID WiMAXCommonAPI_ServiceFlowResponse(PWIMAX_MESSAGE pstWiMAXMessage);
void WiMAXCommonAPI_GetProvisionedState(PUINT32 pActivated);
BOOL WiMAXCommonAPI_GetInterfaceInfo(WIMAX_API_INTERFACE_INFO_P pInterfaceInfo);
VOID WiMAXCommonAPI_DeviceStatusIndication(PWIMAX_MESSAGE pWiMaxMsg);
BOOL WiMAXCommonAPI_CheckIfBlackListedNw(UINT32 u32CF);
VOID ClearBSBlackList();
VOID WiMAXCommonAPI_AddToBlackListArray();
VOID WiMAXCommonAPI_BSBlackListingTakeAction(UINT32 u32LinkStatus);
VOID WiMAXCommonAPI_AddBlackListedEntriesIfAny(PST_CHANNEL_SPECIFICATION pChannelPlan);
BOOL WiMAXCommonAPI_CheckIfEntriesAlreadyExist(PST_DETECTED_BS_INFO pDetectedBS);
VOID WiMAXCommonAPI_CheckAndAddBStoList(PST_CAPL_SEARCH_RSP pstCAPL_Search_Resp);
VOID WiMAXCommonAPI_SearchCallbackFunction(UINT32 u32Index, UINT32 bSearchCompleteFlag);
INT WiMAXCommonAPI_SendIdleModeWakeupRequest();
INT WiMAXCommonAPI_WakeupDeviceFromIdleMode();
VOID WiMAXCommonAPI_NetEntryExitResponse(PWIMAX_MESSAGE pWiMAXMsg);
INT WiMAXCommonAPI_CheckModemStatus();

#ifndef MACOS
UINT CAPIClientOMAMonitorThread(PVOID pArg);
int CAPIOMAInterfaceInit(PVOID pArg);
VOID CAPI_OMA_DM_interfacehandler(PBCM_VDM_IPC_INTF pstRecvBCM_VDM_IPC_INTF);
int SendOMA_DMClient(PBCM_VDM_IPC_INTF pSendpayload);
int CAPIOMAInterfaceInit(PVOID pArg);

#endif


#endif				/* BECEEM_WIMAX_COMMONAPI_H */
