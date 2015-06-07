/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "BeceemCscmServer.h"
#include "BeceemAPI.h"
#include "Options.h"
#include "Utility.h"
#include "SysLogFunctions.h"
#include "LogFile.h"

#ifdef MS_VISUAL_CPP
#pragma warning(push)
#pragma warning(disable: 4100)
#endif

extern stCmdInformation_t *g_astCmdInfo;

#define MAC_LOGFILE_DESC            S("MAC message log file")

// -----------------------------------------------------------------------------------------
// Constructor

BeceemAPI_t BeceemAPI_Create(STRING bszConfFilePath, BOOL bDebugMode)
	{
	BOOL bCreateFailed = FALSE;
	BeceemAPI_t hAPI = NULL;
	hOptionTable_t hTable;
	STRING bszActConfFilePath = NULL;
	STRING bszConfMessages    = NULL;
	BOOL bError = FALSE;
	UINT32 u32TextLength = 0, u32EventID;
	int n;

	// Allocate the structure
	if ((hAPI = WcmMalloc(sizeof(stBeceemAPI_t))) == NULL)
		return NULL;

	// Set it all to zero
	memset(hAPI, 0, sizeof(stBeceemAPI_t));

	// Debug mode
	hAPI->bDebugMode = bDebugMode;

	// Message queue
	hAPI->pQueueStart      = NULL;
	hAPI->pQueueEnd        = NULL;
	hAPI->hQueueMutex      = NULL;
	hAPI->hQueueSemaphore  = NULL;

	// Print contexts
	hAPI->hPrintConStat   = NULL;
	hAPI->hPrintVersions  = NULL;
	hAPI->hPrintInfo      = NULL;
	hAPI->hPrintSearch    = NULL;
	hAPI->hPrintDiag      = NULL;
	hAPI->hPrintStats     = NULL;
	hAPI->hPrintEAPConfig = NULL;
	hAPI->hPrintMisc      = NULL;

	// Initialize other variables
#ifdef WIN32
	hAPI->hDLL						= NULL;
#endif
	hAPI->hDevice					= INVALID_HANDLE_VALUE;
	hAPI->bIsNetworkSearchDone		= FALSE;
	hAPI->bInvertMSKByteOrder		= FALSE;
	hAPI->bIgnoreWaitNetEntryCmd	= TRUE;
	hAPI->bSkipNetSearch			= FALSE;
	hAPI->bAuthEnabled				= FALSE;
	hAPI->bAuthEthernetToRADIUS		= FALSE;
	hAPI->i32AuthenticationState	= EAP_AUTH_OFF;
	hAPI->i32ReauthenticationState	= STATE_REAUTH_OFF;
	hAPI->u32AuthStartCount			= 0;
	hAPI->u32NetEntryCompletedCount	= 0;
	hAPI->u32NetEntryDeregCount		= 0;     
	hAPI->u32NetEntryPKMCount		= 0;      
	hAPI->u32NetEntrySATEKCount		= 0;    
	hAPI->u32NetEntryRegCount		= 0;  
	hAPI->u32OtherDeregRcvdCount	= 0;
	hAPI->bAuthExchangeActive		= FALSE;
	hAPI->bszBaseStationID			= EMPTY_STRING;
	BeceemAPI_SetState(hAPI, STATE_OFF);
	hAPI->s32RSSI					= -128;
	hAPI->s16CINR					= 0;

	hAPI->u32LastLinkStatus			= 999;

	// Request wait indicators
	hAPI->hSelectedBS				= NULL;
	hAPI->eNetworkSearchState		= NsIdle;

	// CSCM logging
	hAPI->u32CSCMDebugLogLevel      = DIAG_NORMAL;

	// MAC Logging
	hAPI->bMACLoggingEnabled		= FALSE;
	hAPI->bszMACLogFileName			= EMPTY_STRING;
	hAPI->hMACLogFile 				= NULL;

	// Other
	hAPI->bValidDLLVersion			= TRUE;
	hAPI->bValidFWVersion			= TRUE;

	// Actual versions
	hAPI->bszActualDLLVersion[0]	= B_NUL;
	hAPI->bszActualDriverVersion[0]	= B_NUL;
	hAPI->bszActualFWVersion[0]		= B_NUL;

	// EAP authentication
	hAPI->bEAPIsOpen				= FALSE;	
	hAPI->bRADIUSIsOpen				= FALSE;	
	hAPI->bszCurrentIdentity[0]		= B_NUL;
	hAPI->bszCurrentPassword[0]		= B_NUL;
	hAPI->u32EAPMethod				= EAP_METHOD_TTLS_MSCHAPV2; 
	hAPI->bszCACertFilename			= EMPTY_STRING;
	hAPI->bszTLSDeviceCertFilename			= EMPTY_STRING;
	hAPI->bszTLSDevicePrivateKeyFilename	= EMPTY_STRING;
	hAPI->bszTLSDevicePrivateKeyPassword	= EMPTY_STRING;
	hAPI->bszPrivateKeyPasswordFormat		= EMPTY_STRING;
	hAPI->bszCurrentRADIUSIPAddress			= EMPTY_STRING;
	hAPI->u32CurrentRADIUSPort				= 0;
	hAPI->bszCurrentRADIUSSecret			= EMPTY_STRING;
	hAPI->bThreadAuthenticationExists		= FALSE;
	hAPI->threadAuthentication				= (WcmThread_t) NULL;
	memset(hAPI->au8MasterSessionKey, 0, MSK_LEN+EMSK_LEN);
	hAPI->au32SessionKeyLength = 0;
	// Firmware download
	hAPI->eChipsetControlStatus     = ChipsetIdle;

	// IP refresh
	hAPI->eIPRefreshState			= IpIdle;
	hAPI->bNetEntryIPRefresh		= FALSE;
	hAPI->bTerminateDHCPClient      = FALSE;

	// Client socket
	hAPI->socClient					= CSCM_INVALID_SOCKET;

	// String buffer pool
	hAPI->iStringNxtBufBchar = 0;

	// Elapsed time variables
	hAPI->u32TotalSeconds        = 0;
	hAPI->timeServerStartSeconds = time(NULL);
	hAPI->u32ConnStartSeconds    = 0;

	// Create the options table
	hTable = Options_CreateTable();
	if (!hTable)
		{
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}
	BeceemAPI_SetOptionsTable(hAPI, hTable);

	// Read the options file
	hAPI->bszActConfFilePath = NULL;
	if ((hAPI->hPrintMisc = Print_Create(FALSE, 1024, 1024)) == NULL)
		{
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}
	Print_SetBuffered(hAPI->hPrintMisc, TRUE);
	bszActConfFilePath = bszConfFilePath != NULL && bszConfFilePath[0] != B_NUL ? bszConfFilePath : DEFAULT_CONFIG_FILE_PATH;
	SysLogWPrintf(SYSLOG_INFO, S("Reading configuration from file ") B_SF, bszActConfFilePath);
	if (!Options_LoadOptionsFromFile(hAPI->hOptionTable, bszActConfFilePath, hAPI->hPrintMisc, NULL))
		{
		if (bszConfFilePath != NULL)
			{
			BeceemAPI_Destroy(hAPI);
			return NULL;
			}
		else
			{
			Print_Formatted(hAPI->hPrintMisc, 
				S("Configuration file ") B_SF S(" not found - using default option values"), bszActConfFilePath);
			SysLogWPrintf(SYSLOG_INFO, 
				S("Configuration file ") B_SF S(" not found - using default option values"), bszActConfFilePath);
			bszActConfFilePath = EMPTY_STRING;
			}
		}	
	bszConfMessages = Print_GetText(hAPI->hPrintMisc, &u32TextLength);
	Print_SetBuffered(hAPI->hPrintMisc, FALSE);
	Print_Flush(hAPI->hPrintMisc);
	BeceemAPI_SetConfFilePath(hAPI, bszActConfFilePath);

	// Create the print contexts
	if ((hAPI->hPrintConStat = Print_Create(FALSE, 2048, 2048)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintDiag = Print_Create(hAPI->bDebugMode, 16*1024, 16*1024)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintStats = Print_Create(FALSE, 8*1024, 8*1024)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintInfo = Print_Create(FALSE, 1024, 1024)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintSearch = Print_Create(FALSE, 1024, 1024)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintVersions = Print_Create(FALSE, 2048, 2048)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hPrintEAPConfig = Print_Create(FALSE, 2048, 1024)) == NULL)
		bCreateFailed = TRUE;

	else if ((hAPI->hNetSearchParam = NetSearchParameters_Create()) == NULL)
		bCreateFailed = TRUE;

	if (bCreateFailed || bError)
		{
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Update internal variables from options values
	BeceemAPI_UpdateOptions(hAPI);

	// Print any error messages from processing the options file
	// A delayed print is needed because the options file is read before the debug log
	// has been opened for writing.
	PrintDiag(DIAG_NORMAL, hAPI, S("\nProcessing configuration file\n"));
	PrintDiag(DIAG_NORMAL, hAPI, S("=============================\n"));
	if (DIAG_NORMAL <= hAPI->u32CSCMDebugLogLevel)
		// We can't use PrintDiag because of printf line length limits
		Print_Append(hAPI->hPrintDiag, bszConfMessages);
	PrintDiag(DIAG_NORMAL, hAPI, S("\n"));
	WcmFree(bszConfMessages);

	// Print the default option value
	PrintDiag(DIAG_NORMAL,hAPI, S("\nOption Values In Effect\n"));
	PrintDiag(DIAG_NORMAL,hAPI,   S("=======================\n\n"));
	Options_GetOptionByName(hAPI->hOptionTable, NULL, hAPI->hPrintDiag);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"));

	// BS information array
	hAPI->u32BSInfoCount = 0; // MUST be initialized before the call to BeceemAPI_ClearBSInformation
	BeceemAPI_ClearBSInformation(hAPI);

	// Other initializations
	BeceemAPI_EnableTimeouts(hAPI);
	hAPI->hBBStatsOffset			= BBStatisticsCounters_Create();
	hAPI->hLinkStatsReport          = MibData_Create();

	// Initialize the message queue
	if (!InitQueue(hAPI))
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to initialize message queue in cBeceemWiMAX constructor."));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}
	else
		PrintDiag(DIAG_NORMAL,hAPI, S("Message queue initialized.\n"));

	// Miscellaneous variables
	hAPI->u32Seed               = 1234567L;
	hAPI->u32StatisticsInterval = 0;

	// Timeout limits
	for (n=0; g_astCmdInfo[n].u32ID != ID_LAST_ENTRY; n++)
		{
		u32EventID = g_astCmdInfo[n].u32TimeoutEventID;
		if (u32EventID != ID_EVENT_INVALID)
			hAPI->astEventData[u32EventID].u32TimeoutMs = g_astCmdInfo[n].u32TimeoutEventMs;
		}

	// Create and immediately start the ProcessMessages thread
	// This must be done after the queue is initialized
	hAPI->bProcessMesssagesThreadRunning = FALSE; 
	hAPI->u32ProcessMesssagesThreadID    = 0;
	hAPI->threadProcessMessages = 
			WcmThread_Create(
				&BeceemAPI_ProcessMessages,
				hAPI,
				&hAPI->u32ProcessMesssagesThreadID);
	if (!hAPI->threadProcessMessages)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to initialize ProcessMessages thread.\n"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}
	else
		PrintDiag(DIAG_NORMAL,hAPI, S("Process messages thread started successfully.\n"));

	// Create mutex for serializing messages sent to the API library
	hAPI->hMutexSendWiMAXMessage = WcmMutex_Create();
	if (!hAPI->hMutexSendWiMAXMessage)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create SendWiMAXMessage mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Create mutex for access to socket's send function
	hAPI->hMutexSocketSend = WcmMutex_Create();
	if (!hAPI->hMutexSocketSend)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create Socket Send mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Create mutex for status response synchronization
	hAPI->hMutexStatusResponse = WcmMutex_Create();
	if (!hAPI->hMutexStatusResponse)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create Status Response mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Create mutex for client lock on entire BeceemAPI
	hAPI->hMutexClientLock = WcmMutex_Create();
	if (!hAPI->hMutexClientLock)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create Client Lock mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}
    hAPI->bClientMutexLocked            = FALSE;
	hAPI->u32ClientMutexLockedMs        = 0;
	hAPI->u32ClientMutexLockedTimeoutMs = DEFAULT_CLIENT_MUTEX_LOCK_WATCHDOG_TIMEOUT_MS;

	// Create mutex for EAP supplicant access
	hAPI->hMutexEAPSupplicant = WcmMutex_Create();
	if (!hAPI->hMutexEAPSupplicant)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create EAP Supplicant mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Create mutex for EAP supplicant traffic calls
	hAPI->hMutexEAPTraffic = WcmMutex_Create();
	if (!hAPI->hMutexEAPTraffic)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create EAP Traffic mutex"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}

	// Create event for RSA private key encryption response
	hAPI->hEventRSAPrivEncryptResponse = WcmEvent_Create();
	if (!hAPI->hEventRSAPrivEncryptResponse)
		{
		PrintDiag(DIAG_NORMAL,hAPI, S("Unable to create RSA private key encryption response event"));
		BeceemAPI_Destroy(hAPI);
		return NULL;
		}	

	// Create the options table

#ifdef WIN32
	// Try loading the DLL
	hAPI->hDLL = LoadLibrary(TEXT(BECEEM_WIMAX_DLL_FILENAME));

	if (hAPI->hDLL != NULL) 
		{

		// Then look for the four DLL interface functions
		lpfnDllOpenWiMAXDevice  = (LPFNDLL1) GetProcAddress(hAPI->hDLL, "OpenWiMAXDevice");
		lpfnDllCloseWiMAXDevice = (LPFNDLL3) GetProcAddress(hAPI->hDLL, "CloseWiMAXDevice");
		lpfnDllRegisterCallback = (LPFNDLL4) GetProcAddress(hAPI->hDLL, "RegisterCallback");
		lpfnDllSendWiMAXMessage = (LPFNDLL5) GetProcAddress(hAPI->hDLL, "SendWiMAXMessage");

		if (lpfnDllOpenWiMAXDevice  == NULL ||
			lpfnDllCloseWiMAXDevice == NULL ||
			lpfnDllRegisterCallback == NULL ||
			lpfnDllSendWiMAXMessage == NULL) 
			{
				Print_Error(hAPI->hPrintDiag, S("Invalid ") S(BECEEM_WIMAX_DLL_FILENAME) S(" function pointer"));
				BeceemAPI_SetState(hAPI, STATE_LIBRARY_INIT_ERROR);
				FreeLibrary(hAPI->hDLL);
				hAPI->hDLL = NULL;
				BeceemAPI_Destroy(hAPI);
				return FALSE;
			}

		// Notify 
		BeceemAPI_SetState(hAPI, STATE_LIBRARY_INIT);

		// DEBUG ONLY
		// RefreshIPAddress();
		// END DEBUG ONLY

		}
	else
		{
		BeceemAPI_SetState(hAPI, STATE_LIBRARY_INIT_ERROR);
		Print_Error(hAPI->hPrintDiag, S(BECEEM_WIMAX_DLL_FILENAME) S(" not found."));
		}	
#else
	BeceemAPI_SetState(hAPI, STATE_LIBRARY_INIT);
#endif

	return hAPI;

	} /* BeceemAPI_Create */

// -----------------------------------------------------------------------------------------
// Destructor
void BeceemAPI_Destroy(BeceemAPI_t hAPI) {

	hLogFile_t hLogFileDiag = NULL;

	if (!hAPI)
		return;

	if (hAPI->hPrintDiag)
		hLogFileDiag = hAPI->hPrintDiag->hLogFile;

	PrintDiag(DIAG_NORMAL, hAPI, S("\nShutting down the Beceem API interface.\n"));
	// Exit the Process Messages thread immediately
	PrintDiag(DIAG_NORMAL, hAPI, S("Terminating Process Messages thread.\n"));
	Enqueue(hAPI, eAbortThread, NULL, 0);
	WcmThread_Destroy(hAPI->threadProcessMessages);
	PrintDiag(DIAG_NORMAL,hAPI, S("Process Messages thread has exited\n"));

	// Options table
	PrintDiag(DIAG_NORMAL, hAPI, S("Freeing the Options Table.\n"));
	Options_DestroyTable(hAPI->hOptionTable);

	// Other structures
	PrintDiag(DIAG_NORMAL, hAPI, S("Freeing other structures.\n"));
	NetSearchParameters_Destroy(hAPI->hNetSearchParam);
	MibData_Destroy(hAPI->hLinkStatsReport);
	BBStatisticsCounters_Destroy(hAPI->hBBStatsOffset);

	if (hAPI->bMACLoggingEnabled && hAPI->hMACLogFile != NULL)
		{
		PrintDiag(DIAG_NORMAL, hAPI, S("Closing MAC log file.\n"));
		LogFile_Close(hAPI->hMACLogFile);
		}

	PrintDiag(DIAG_NORMAL, hAPI, S("Closing EAP supplicant.\n"));
	BeceemAPI_EAP_Close(hAPI);

	PrintDiag(DIAG_NORMAL, hAPI, S("Closing Beceem WiMAX device.\n"));
	BeceemAPI_CloseDevice(hAPI);

	// Destroy mutex and semaphore created by InitQueue
	PrintDiag(DIAG_NORMAL, hAPI, S("Freeing message queue.\n"));
	WcmMutex_Destroy(hAPI->hQueueMutex);
	WcmSemaphore_Destroy(hAPI->hQueueSemaphore);

	// Destroy mutexes for serializing messages sent to the API library
	PrintDiag(DIAG_NORMAL, hAPI, S("Freeing mutexes.\n"));
	WcmMutex_Destroy(hAPI->hMutexSendWiMAXMessage);
	WcmMutex_Destroy(hAPI->hMutexSocketSend);
	WcmMutex_Destroy(hAPI->hMutexStatusResponse);
	WcmMutex_Destroy(hAPI->hMutexClientLock);
	WcmMutex_Destroy(hAPI->hMutexEAPSupplicant);
	WcmMutex_Destroy(hAPI->hMutexEAPTraffic);

	// Destroy synchronization events
	WcmEvent_Destroy(hAPI->hEventRSAPrivEncryptResponse);
	
	// Destroy the print context objects
	PrintDiag(DIAG_NORMAL, hAPI, S("Freeing print context structures.\n"));
	Print_Destroy(hAPI->hPrintVersions);
	Print_Destroy(hAPI->hPrintStats);
	Print_Destroy(hAPI->hPrintInfo);
	Print_Destroy(hAPI->hPrintSearch);
	Print_Destroy(hAPI->hPrintConStat);
	Print_Destroy(hAPI->hPrintEAPConfig);
	Print_Destroy(hAPI->hPrintMisc);

	// Clear the BS information array 
	BeceemAPI_ClearBSInformation(hAPI);
	if (hAPI->bszActConfFilePath != NULL)
		WcmFree(hAPI->bszActConfFilePath);

	// Final message
	PrintDiag(DIAG_NORMAL, hAPI, S("Beceem API interface has been shut down.\n"));
	Print_Destroy(hAPI->hPrintDiag);

#ifdef WIN32
	if (hAPI->hDLL != NULL) 
		{
		FreeLibrary(hAPI->hDLL);  // NOT COMPATIBLE with Remote Proxy
		hAPI->hDLL = NULL;
		}
#endif

	// Free the Beceem API structure
	WcmFree(hAPI);

	} // BeceemWiMAX destructor

// -----------------------------------------------------------------------------------------
// UINT32 BeceemAPI_OpenDevice(hAPI) (void)
//
//    - Opens the WiMAX device
//    - Registers the callback function
//
// Returns OPEN_DEVICE_SUCCESS or OPEN_DEVICE_FAILURE

UINT32 BeceemAPI_OpenDevice(BeceemAPI_t hAPI) 
	{

	Print_SetBuffered(hAPI->hPrintDiag, FALSE);

	if (!BeceemAPI_IsLoaded(hAPI))
		return OPEN_DEVICE_FAILURE;
	else if (BeceemAPI_IsOpen(hAPI)) {
		BeceemAPI_PrintDiag(DIAG_NORMAL, hAPI, S("INFO: Device is already open.\n"));
		return OPEN_DEVICE_SUCCESS;
		}

	BeceemAPI_SetState(hAPI, STATE_WAITING_FOR_CARD);

	PrintDiag(DIAG_NORMAL,hAPI, S("Trying to open device.\n"));

#ifdef WIN32
	lpfnDllOpenWiMAXDevice = (LPFNDLL1) GetProcAddress(hAPI->hDLL, "OpenWiMAXDevice");
	hAPI->hDevice = lpfnDllOpenWiMAXDevice();
#else
	hAPI->hDevice = OpenWiMAXDevice();
#endif

	if (hAPI->hDevice != INVALID_HANDLE_VALUE) 
		{
		UINT32 u32RetCode;
		PrintDiag(DIAG_NORMAL,hAPI, S("Device opened successfully.\n"));

		// Register the callback function
		PrintDiag(DIAG_NORMAL,hAPI, S("Registering callback function"));
#ifdef MS_VISUAL_CPP
#pragma warning(push)
#pragma warning(disable: 4054)
#endif
#ifdef WIN32
		u32RetCode = lpfnDllRegisterCallback(hAPI->hDevice, (void *) CSCMCallback, (void *) hAPI);
#else
		u32RetCode = RegisterCallback(hAPI->hDevice, (void *) CSCMCallback, (void *) hAPI);
#endif
#ifdef MS_VISUAL_CPP
#pragma warning(pop)
#endif

		switch (u32RetCode) 
			{
			case SUCCESS:
				PrintDiag(DIAG_NORMAL,hAPI, S(" - Success.\n"));
				break;
			case ERROR_INVALID_DEVICE_HANDLE:
				PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Invalid device handle.\n"));
				break;
			case 0xFFFFFFFF:
				PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Library initialization error.\n"));
				break;
			case ERROR_INVALID_NOTIFY_ROUTINE:
				PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Invalid notify routine.\n"));
				break;
			case ERROR_DEVICEIOCONTROL_FAILED:
				PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Device I/O control failed.\n"));
				break;
			default:
				PrintDiag(DIAG_ERROR, hAPI, S("\nERROR: Unknown error code ") B_SF S(".\n"), u32RetCode);
				break;
			}

		if (u32RetCode == SUCCESS)
			{
			BeceemAPI_SetState(hAPI, STATE_CARD_INIT);
			// Get SS information
			BeceemAPI_GetSSInfoRequest(hAPI);

			// Done!
			Print_SetBuffered(hAPI->hPrintDiag, TRUE);

			return OPEN_DEVICE_SUCCESS;
			}

		}

	BeceemAPI_SetState(hAPI, STATE_WAITING_FOR_CARD);
	BeceemAPI_EnableTimeouts(hAPI);
	hAPI->hDevice = INVALID_HANDLE_VALUE;
	return OPEN_DEVICE_FAILURE;

	} // BeceemAPI_OpenDevice(hAPI)

// -----------------------------------------------------------------------------------------
// void CloseDevice (void)
//
//    - Closes the WiMAX device
//

void BeceemAPI_CloseDevice(BeceemAPI_t hAPI) 
	{

	if (!BeceemAPI_IsLoaded(hAPI) || !BeceemAPI_IsOpen(hAPI))
		return;

	Print_SetBuffered(hAPI->hPrintDiag, FALSE);

	PrintDiag(DIAG_NORMAL,hAPI, S("Closing device.\n"));

	if (hAPI->hDevice != INVALID_HANDLE_VALUE) 
		{
#ifdef WIN32
		lpfnDllCloseWiMAXDevice(hAPI->hDevice);
#else
		CloseWiMAXDevice(hAPI->hDevice);
#endif
		hAPI->hDevice = INVALID_HANDLE_VALUE;
		}

	BeceemAPI_SetState(hAPI, STATE_CLOSED);
	BeceemAPI_DisableTimeouts(hAPI);

	Print_Newline(hAPI->hPrintDiag);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"), StringRepChar(hAPI, '/', 80));
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"), StringRepChar(hAPI, '\\', 80));
	Print_Newline(hAPI->hPrintDiag);
	Print_Flush(hAPI->hPrintDiag);

	} // CloseDevice

// -----------------------------------------------------------------------------------------
// Clear the base station information array
// -----------------------------------------------------------------------------------------

void BeceemAPI_ClearBSInformation (BeceemAPI_t hAPI)
	{

	UINT32 n;

	if (hAPI == NULL)
		return;

	for (n=0; n < hAPI->u32BSInfoCount; n++)
		BSInformation_Destroy(hAPI->ahBSInfo[n]);

	hAPI->hSelectedBS    = NULL;
	hAPI->u32BSInfoCount = 0;
	memset(hAPI->ahBSInfo, 0, MAX_NET_SEARCH_RESULTS * sizeof(hAPI->ahBSInfo[0]));

	} /* BeceemAPI_ClearBSInformation */


static UINT32 BeceemAPI_CalcNWSearchTimeOut(BeceemAPI_t hAPI)
{
	hNetSearchParameters_t hNetSearchParam = hAPI->hNetSearchParam;
	UINT32 ui32PerCFTimeOut = 300; //in ms.
	UINT32 ui32CFCount = 0;
	if (!Options_GetNetSearchParameters(hAPI->hOptionTable, hNetSearchParam))
	{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Options_GetNetSearchParameters failed in BeceemAPI_CalcNWSearchTimeOut\n"));
		return 0;
	}
	if (hNetSearchParam->u32ScanMinFrequencyKHz  > 0 &&
		 hNetSearchParam->u32ScanMaxFrequencyKHz  > 0 &&
		 hNetSearchParam->u32ScanStepFrequencyKHz > 0)
	{
		ui32CFCount = 1 + ((hNetSearchParam->u32ScanMaxFrequencyKHz - 
							hNetSearchParam->u32ScanMinFrequencyKHz)/hNetSearchParam->u32ScanStepFrequencyKHz);
	}
	else if(hNetSearchParam->u32NumberOfFrequencies > 0)
	{
		ui32CFCount = hNetSearchParam->u32NumberOfFrequencies;
	}
	
	return (ui32CFCount * hNetSearchParam->u32NumberOfBandwidths * ui32PerCFTimeOut)+3000; //a min of 3secs.
}

// -----------------------------------------------------------------------------------------
// BOOL InitNetworkSearch
//
// - Prepares for the network search process
//
//   Returns FALSE on error.

BOOL BeceemAPI_InitNetworkSearch (BeceemAPI_t hAPI) 
	{

	BOOL bError = FALSE;
	UINT32 u32NetSearchTimeoutMs, u32CalculatedTimeOut;

	if (!BeceemAPI_IsOpen(hAPI) && BeceemAPI_OpenDevice(hAPI) == OPEN_DEVICE_FAILURE)
		return FALSE;

	// Set the network search timeout
	u32CalculatedTimeOut = BeceemAPI_CalcNWSearchTimeOut(hAPI);
	u32NetSearchTimeoutMs = 1000 * Options_GetUint32ByEntry(hAPI->pOptNetworkSearchTimeoutSec, &bError);
	if(u32CalculatedTimeOut > u32NetSearchTimeoutMs)
	{
		PrintDiag(DIAG_NORMAL,hAPI, S("BeceemAPI_InitNetworkSearch: Changing the NW search time out from: %d")
									S(" to: %d\n"),u32NetSearchTimeoutMs, u32CalculatedTimeOut);
		u32NetSearchTimeoutMs = u32CalculatedTimeOut;
	}
	hAPI->u32ClientMutexLockedTimeoutMs = u32NetSearchTimeoutMs + 10000 > DEFAULT_CLIENT_MUTEX_LOCK_WATCHDOG_TIMEOUT_MS ?
		u32NetSearchTimeoutMs + 10000 : DEFAULT_CLIENT_MUTEX_LOCK_WATCHDOG_TIMEOUT_MS;
	if (bError)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Options_GetUint32ByEntry failed in BeceemAPI_InitNetworkSearch\n"));
		return FALSE;
		}
	BeceemAPI_SetTimeout(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT, u32NetSearchTimeoutMs);

	if (hAPI->eNetworkSearchState == NsIdle
		|| hAPI->eNetworkSearchState == NsAborted
		|| hAPI->eNetworkSearchState == NsDone)
		{
		BeceemAPI_ClearBSInformation(hAPI); 
		hAPI->eNetworkSearchState = NsSyncDown1;
		BeceemAPI_SyncDownRequest(hAPI);
		BeceemAPI_Activate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
		return TRUE;
		}
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Network search state is not 'NsIdle' in BeceemAPI_InitNetworkSearch\n"));
		return FALSE;
		}

	} /* BeceemAPI_InitNetworkSearch */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_ResetNetworkSearch
//
// - Resets the network search process
//
//   
void BeceemAPI_ResetNetworkSearch (BeceemAPI_t hAPI)
	{

	hAPI->eNetworkSearchState = NsIdle;
	memset(hAPI->hNetSearchParam, 0, sizeof(stNetSearchParameters_t));

	} /* ResetNetworkSearch */

// -----------------------------------------------------------------------------------------
// Get Option
//
void BeceemAPI_GetOption(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs)
	{

	PrintContext_t hPrint;
	UINT32 n;

	if (!hAPI)
		return;

	hPrint = hAPI->hPrintMisc;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Get Option: "));
	for (n=0; n<u32NumberOfArgs; n++)
		PrintDiag(DIAG_NORMAL,hAPI, B_SF S(" "), abszArgStrings[n]);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"));

	ServerSocketBeginPrint(hPrint);

	if (u32NumberOfArgs < 1)
		Options_GetOptionByName(hAPI->hOptionTable, NULL, hPrint);
	else if (u32NumberOfArgs == 1)
		Options_GetOptionByName(hAPI->hOptionTable, abszArgStrings[0], hPrint);
	else
		Print_Formatted(hPrint, S("ERROR: Extra arguments in command"));

	ServerSocketEndPrint(hAPI, hPrint, ID_MSG_GET);

	} /* BeceemAPI_GetOption */

// -----------------------------------------------------------------------------------------
// Set Option
//
void BeceemAPI_SetOption(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs)
	{

	UINT32 n, u32TextLength;
	STRING bszText, bszOptionName;
	BOOL bFreeBuffer;
	PrintContext_t hPrint;

	if (!hAPI)
		return;

	hPrint = hAPI->hPrintMisc;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Set Option: "));
	for (n=0; n<u32NumberOfArgs; n++)
		PrintDiag(DIAG_NORMAL,hAPI, B_SF S(" "), abszArgStrings[n]);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"));

	bFreeBuffer = FALSE;
	if (u32NumberOfArgs >= 2)
		{

		ServerSocketBeginPrint(hPrint);

		bszOptionName = Options_SetOptionByName(hAPI->hOptionTable, 
					            abszArgStrings[0], &abszArgStrings[1], u32NumberOfArgs - 1,
					            hPrint);

		BeceemAPI_UpdateOptions(hAPI);

		Print_Formatted(hPrint, S("set ") B_SF, bszOptionName == NULL ? abszArgStrings[0] : bszOptionName);
		for (n=1; n < u32NumberOfArgs; n++)
			Print_Formatted(hPrint, S(" ") B_SF, abszArgStrings[n]);
		Print_Formatted(hPrint, S(" ") B_SFNL, bszOptionName != NULL ? S("OK") : S("FAILED"));

		bszText = Print_GetText(hPrint, &u32TextLength);
		Print_Flush(hPrint);
		Print_SetBuffered(hPrint, FALSE);
		bFreeBuffer = TRUE;
		}

	else if (u32NumberOfArgs == 1)
		bszText = S("ERROR: Missing option value.\n");
	else 
		bszText = S("ERROR: Missing option name.\n");

	ServerSocketResponse(hAPI, ID_MSG_SET, bszText, FALSE);
	if (bFreeBuffer)
		WcmFree(bszText);

	} /* BeceemAPI_SetOption */

// -----------------------------------------------------------------------------------------
// Implements three sub-commands:
//      options file
//      options load
//      options save
//
void BeceemAPI_OptionsCommand(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs)
	{

	UINT32 n, u32TextLength, u32ErrorCount = 0;
	STRING bszText, bszInitFilePath;
	hOptionTable_t hNewOptionTable = NULL, hOldOptionTable = NULL;
	BOOL bFreeBuffer;
	BOOL bStatus;
	PrintContext_t hPrint;
	enum {OPTIONS_ERROR, OPTIONS_FILE, OPTIONS_LOAD, OPTIONS_SAVE} eSubCmd;

	if (!hAPI)
		return;

	hPrint = hAPI->hPrintMisc;

	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Options: "));
	for (n=0; n<u32NumberOfArgs; n++)
		PrintDiag(DIAG_NORMAL,hAPI, B_SF S(" "), abszArgStrings[n]);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"));

	bFreeBuffer = FALSE;

	// Determine the sub-command
	bszText = NULL;
	eSubCmd = OPTIONS_ERROR;
	if (u32NumberOfArgs < 1)
		bszText = S("ERROR: 'file', 'load', or 'save' required after 'options'.\n");
	else if (u32NumberOfArgs > 2)
		bszText = S("ERROR: Too many parameters for 'options' command.\n");
	else
		{
		if (BcsCmpNoCase(abszArgStrings[0], S("file")))
			eSubCmd = OPTIONS_FILE;
		else if (BcsCmpNoCase(abszArgStrings[0], S("load")))
			eSubCmd = OPTIONS_LOAD;
		else if (BcsCmpNoCase(abszArgStrings[0], S("save")))
			eSubCmd = OPTIONS_SAVE;
		else 
			bszText = S("ERROR: 'file', 'load', or 'save' required after 'options'.\n");
		}

	// Check that options file is defined
	if (bszText == NULL && u32NumberOfArgs == 1 && 
	   (hAPI->bszActConfFilePath == NULL || hAPI->bszActConfFilePath[0] == B_NUL))
		{
		if (eSubCmd == OPTIONS_FILE)
			bszText = S("options file is not defined");
		else
			bszText = S("ERROR: options file is not defined");
		}

	// Process the sub-command
	if (bszText == NULL)
		{
		ServerSocketBeginPrint(hPrint);

		switch (eSubCmd)
			{

			case OPTIONS_FILE:
				if (u32NumberOfArgs == 2)
					BeceemAPI_SetConfFilePath(hAPI, abszArgStrings[1]);
				Print_Formatted(hPrint, S("options file is '") B_SF S("'"), hAPI->bszActConfFilePath);
				break;

			case OPTIONS_LOAD:
				bszInitFilePath = u32NumberOfArgs == 2 ? abszArgStrings[1] : hAPI->bszActConfFilePath;
				hNewOptionTable = Options_CreateTable();
				if (hNewOptionTable == NULL)
					Print_Formatted(hPrint, S("\nERROR: Program error while creating new option table.\n"));
				else
					{
					bStatus = Options_LoadOptionsFromFile(hNewOptionTable, bszInitFilePath, hPrint, &u32ErrorCount);
					if (bStatus && u32ErrorCount == 0)
						{
						// Replace the internal option table with the new version
						hOldOptionTable = hAPI->hOptionTable;
						BeceemAPI_SetOptionsTable(hAPI, hNewOptionTable);
						BeceemAPI_UpdateOptions(hAPI);
						Options_DestroyTable(hOldOptionTable);
						Print_Formatted(hPrint, S("options load OK\n"));
						}
					else
						{
						Print_Formatted(hPrint, S("options load FAILED - options were NOT UPDATED\n"));
						Options_DestroyTable(hNewOptionTable);
						}
					}
				break;

			case OPTIONS_SAVE:
				bszInitFilePath = u32NumberOfArgs == 2 ? abszArgStrings[1] : hAPI->bszActConfFilePath;
				bStatus = Options_SaveOptionsToFile(hAPI->hOptionTable, bszInitFilePath, hPrint);
				Print_Formatted(hPrint, S("options save ") B_SFNL, bStatus ? S("OK") : S("FAILED"));
				break;

			default:
				break;
			}

		bszText = Print_GetText(hPrint, &u32TextLength);
		Print_Flush(hPrint);
		Print_SetBuffered(hPrint, FALSE);
		bFreeBuffer = TRUE;

		}

	ServerSocketResponse(hAPI, ID_MSG_OPTIONS, bszText, FALSE);
	if (bFreeBuffer)
		WcmFree(bszText);

	} /* BeceemAPI_OptionsCommand */


// =========================================================================================================
// Save a new configuration file path
// =========================================================================================================

void BeceemAPI_SetConfFilePath(BeceemAPI_t hAPI, STRING bszConfFilePath)
	{

	UINT32 u32WBufSize;

	if (hAPI->bszActConfFilePath != NULL)
		WcmFree(hAPI->bszActConfFilePath);

	if (bszConfFilePath == NULL || bszConfFilePath[0] == B_NUL)
		{
		hAPI->bszActConfFilePath = NULL;
		return;
		}

	u32WBufSize = (UINT32) b_strlen(bszConfFilePath) + 1;
	if ((hAPI->bszActConfFilePath = (STRING) WcmMalloc(BCHARS2BYTES(u32WBufSize))) == NULL)
		{
		hAPI->bszActConfFilePath = NULL;
		return;
		}

	b_strncpy(hAPI->bszActConfFilePath, bszConfFilePath, u32WBufSize);

	} /* BeceemAPI_SetConfFilePath */

// =========================================================================================================
// Initialize the Options table pointers
// =========================================================================================================

void BeceemAPI_SetOptionsTable(BeceemAPI_t hAPI, hOptionTable_t hTable)
	{

	hAPI->hOptionTable = hTable;
	if (hTable == NULL)
		return;

	hAPI->pOptSkipNetSearch					= Options_FindEntry(hTable, S("SkipNetSearch"));
	hAPI->pOptAutoReConnectEnabled			= Options_FindEntry(hTable, S("AutoReConnectEnabled"));
	hAPI->pOptAutoReConnectIntervalSec		= Options_FindEntry(hTable, S("AutoReConnectIntervalSec"));
	hAPI->pOptAutoReDisconnectEnabled		= Options_FindEntry(hTable, S("AutoReDisconnectEnabled"));
	hAPI->pOptAutoReDisconnectIntervalSec	= Options_FindEntry(hTable, S("AutoReDisconnectIntervalSec"));
	hAPI->pOptLinkStatusRequestPeriodSec	= Options_FindEntry(hTable, S("LinkStatusRequestPeriodSec"));
	hAPI->pOptCSCMDebugLogLevel				= Options_FindEntry(hTable, S("CSCMDebugLogLevel"));
	hAPI->pOptCSCMDebugLogFileName			= Options_FindEntry(hTable, S("CSCMDebugLogFileName"));
	hAPI->pOptCSCMDebugLogFileMaxSizeMB		= Options_FindEntry(hTable, S("CSCMDebugLogFileMaxSizeMB")); 

#ifdef LINUX
	// IP Refresh 
	hAPI->pOptIPRefreshCommand				= Options_FindEntry(hTable, S("IPRefreshCommand"));
	hAPI->pOptNetEntryIPRefreshEnabled		= Options_FindEntry(hTable, S("NetEntryIPRefreshEnabled"));
	hAPI->pOptTerminateDHCPClient			= Options_FindEntry(hTable, S("TerminateDHCPClient"));
#endif

	// Network search 
	hAPI->pOptArrayBandwidthMHz				= Options_FindEntry(hTable, S("BandwidthMHz"));
	hAPI->pOptScanMinFrequencyMHz			= Options_FindEntry(hTable, S("ScanMinFrequencyMHz"));
	hAPI->pOptScanMaxFrequencyMHz			= Options_FindEntry(hTable, S("ScanMaxFrequencyMHz"));
	hAPI->pOptScanStepFrequencyMHz			= Options_FindEntry(hTable, S("ScanStepFrequencyMHz"));
	hAPI->pOptArrayCenterFrequencyMHz		= Options_FindEntry(hTable, S("CenterFrequencyMHz"));
	hAPI->pOptNetworkSearchTimeoutSec		= Options_FindEntry(hTable, S("NetworkSearchTimeoutSec"));
	hAPI->pOptLPSearchInShutDownEnabled		= Options_FindEntry(hTable, S("LPSearchInShutDownEnabled"));

	// Network entry
	hAPI->pOptNetworkEntryTimeoutSec        = Options_FindEntry(hTable, S("NetworkEntryTimeoutSec"));
	hAPI->pOptNEToHighestCINRBaseStation    = Options_FindEntry(hTable, S("NEToHighestCINRBaseStation"));
		
	// Authentication
	hAPI->pOptAuthEnabled					= Options_FindEntry(hTable, S("AuthEnabled"));
	hAPI->pOptAuthEthernetToRADIUS			= Options_FindEntry(hTable, S("AuthEthernetToRADIUS"));	
	hAPI->pOptIdentity						= Options_FindEntry(hTable, S("UserIdentity"));
	hAPI->pOptPassword						= Options_FindEntry(hTable, S("UserPassword"));
	hAPI->pOptRADIUSIPAddress				= Options_FindEntry(hTable, S("RADIUSIPAddress"));
	hAPI->pOptRADIUSPort					= Options_FindEntry(hTable, S("RADIUSPort"));
	hAPI->pOptRADIUSSecret					= Options_FindEntry(hTable, S("RADIUSSecret"));
	hAPI->pOptAuthLogLevel					= Options_FindEntry(hTable, S("AuthLogLevel"));
	hAPI->pOptRADIUSClientLogLevel			= Options_FindEntry(hTable, S("RADIUSClientLogLevel"));
	hAPI->pOptAuthLogFileName				= Options_FindEntry(hTable, S("AuthLogFileName"));
	hAPI->pOptAuthLogFileMaxSizeMB			= Options_FindEntry(hTable, S("AuthLogFileMaxSizeMB"));
	hAPI->pOptAuthEnableSysLogPrints		= Options_FindEntry(hTable, S("EnableAuthSysLogPrints"));

	// EAP
	hAPI->pOptEAPMethod						= Options_FindEntry(hTable, S("EAPMethod"));
	hAPI->pOptCACertPath					= Options_FindEntry(hTable, S("CACertPath")); 
	hAPI->pOptCACertFileName				= Options_FindEntry(hTable, S("CACertFileName"));
	hAPI->pOptTTLSAnonymousIdentity			= Options_FindEntry(hTable, S("TTLSAnonymousIdentity"));
	hAPI->pOptValidateServerCert            = Options_FindEntry(hTable, S("ValidateServerCert"));
	hAPI->pOptTLSDeviceCertFileName			= Options_FindEntry(hTable, S("TLSDeviceCertFileName"));
	hAPI->pOptTLSDeviceSubCA1CertFileName	= Options_FindEntry(hTable, S("TLSDeviceSubCA1CertFileName"));
	hAPI->pOptTLSDeviceSubCA2CertFileName	= Options_FindEntry(hTable, S("TLSDeviceSubCA2CertFileName")); 
	hAPI->pOptTLSDevicePrivateKeyFileName	= Options_FindEntry(hTable, S("TLSDevicePrivateKeyFileName"));
	hAPI->pOptTLSDevicePrivateKeyPassword	= Options_FindEntry(hTable, S("TLSDevicePrivateKeyPassword"));
	hAPI->pOptPrivateKeyPasswordFormat      = Options_FindEntry(hTable, S("PrivateKeyPasswordFormat"));
	
	// Advanced 
	hAPI->pOptInvertMSKByteOrder			= Options_FindEntry(hTable, S("InvertMSKByteOrder"));
	hAPI->pOptAlwaysIncludeTLSLength		= Options_FindEntry(hTable, S("AlwaysIncludeTLSLength"));
	hAPI->pOptEAPFragmentMaxLength          = Options_FindEntry(hTable, S("EAPFragmentMaxLength"));
	hAPI->pOptEAPPollingLoopIntervalMs		= Options_FindEntry(hTable, S("EAPPollingLoopIntervalMs"));
	hAPI->pOptAuthenticationTimeoutSec      = Options_FindEntry(hTable, S("AuthenticationTimeoutSec"));

	hAPI->pOptFirmwareFileName				= Options_FindEntry(hTable, S("FirmwareFileName"));
	hAPI->pOptConfigFileName				= Options_FindEntry(hTable, S("ConfigFileName"));

	hAPI->pOptFirmwareRSAPrivateKeyEncrypt  = Options_FindEntry(hTable, S("FirmwareRSAPrivateKeyEncrypt"));
	hAPI->pOptFirmwarePrivateKeyBits        = Options_FindEntry(hTable, S("FirmwarePrivateKeyBits"));
	hAPI->pOptInnerNAIChange                = Options_FindEntry(hTable, S("InnerNAIChange"));
	hAPI->pOptBeceemEngineFileName   		= Options_FindEntry(hTable, S("BeceemEngineFileName"));
	hAPI->pOptEngineLoggingEnabled          = Options_FindEntry(hTable, S("EngineLoggingEnabled"));
	hAPI->pOptEngineLogFileName             = Options_FindEntry(hTable, S("EngineLogFileName"));
	hAPI->pOptEngineLogFileMaxSizeMB		= Options_FindEntry(hTable, S("EngineLogFileMaxSizeMB")); 
	
	} /* BeceemAPI_SetOptionsTable */

// =========================================================================================================
// Update BeceemWiMAX variables after the options file is read
// =========================================================================================================

void BeceemAPI_UpdateOptions(BeceemAPI_t hAPI)
	{

	UINT32 u32LogLevel, u32MaxLogFileSize;
	STRING bszFileName;

	if (!BeceemAPI_CheckOptionValues(hAPI))
		ServerSocketResponse(hAPI, ID_MSG_ERROR, S("ERROR: Error in option value(s).\n")
					S("Please see the CSCM Debug Log file for an explanation."), TRUE);

	// Network search
	hAPI->bSkipNetSearch = Options_GetBoolByEntry(hAPI->pOptSkipNetSearch, NULL);

	
	u32LogLevel = Options_GetUint32ByEntry(hAPI->pOptCSCMDebugLogLevel, NULL);
	bszFileName = Options_GetStringByEntry(hAPI->pOptCSCMDebugLogFileName, NULL);
	u32MaxLogFileSize = Options_GetUint32ByEntry(hAPI->pOptCSCMDebugLogFileMaxSizeMB, NULL);
	if (u32LogLevel > 0)
		Print_OpenLogFile(hAPI->hPrintDiag, bszFileName, u32MaxLogFileSize); // Just returns if the file name is unchanged
	else
		Print_CloseLogFile(hAPI->hPrintDiag);
	hAPI->u32CSCMDebugLogLevel = u32LogLevel;

	// Authentication
	hAPI->bAuthEnabled			 = Options_GetBoolByEntry(hAPI->pOptAuthEnabled, NULL);
	hAPI->bAuthEthernetToRADIUS  = Options_GetBoolByEntry(hAPI->pOptAuthEthernetToRADIUS, NULL);
	hAPI->i32AuthenticationState = hAPI->bAuthEnabled ? EAP_AUTH_OFF : EAP_AUTH_DISABLED;
	hAPI->bInvertMSKByteOrder    = Options_GetBoolByEntry(hAPI->pOptInvertMSKByteOrder, NULL);
	//delete the present auth log, so that supplicant starts with a new one.
	bszFileName = Options_GetStringByEntry(hAPI->pOptAuthLogFileName, NULL);
#ifdef WIN32
	DeleteFile(bszFileName);
#else
	remove(bszFileName);
#endif	 
	LogFile_deleteOldLog(bszFileName);
#ifdef LINUX
	// IP refresh
	hAPI->bNetEntryIPRefresh     = Options_GetBoolByEntry(hAPI->pOptNetEntryIPRefreshEnabled, NULL);
	hAPI->bTerminateDHCPClient   = Options_GetBoolByEntry(hAPI->pOptTerminateDHCPClient, NULL);
#endif

	} // BeceemAPI_UpdateOptions


BOOL BeceemAPI_CheckOptionValues(BeceemAPI_t hAPI)
	{

	BOOL bError = FALSE;

	// -----------------------------------------------------------------
	// Check file names
	// -----------------------------------------------------------------

	// CSCM debug logging 
	if (Options_IsEmptyStringByEntry(hAPI->pOptCSCMDebugLogFileName, &bError) &&
		Options_GetUint32ByEntry(hAPI->pOptCSCMDebugLogLevel, &bError) > 0)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: No CSCM Debug Logging file name specified\n"));
		bError = TRUE;
		}
		 

	if (hAPI->bAuthEnabled)
		{
		if (Options_IsEmptyStringByEntry(hAPI->pOptAuthLogFileName, &bError) &&
			!(Options_GetUint32ByEntry(hAPI->pOptAuthLogLevel, &bError) == 0 && 
			  Options_GetUint32ByEntry(hAPI->pOptRADIUSClientLogLevel, &bError) == 0))
			{
			PrintDiag(DIAG_NORMAL,hAPI, S("ERROR:No authentication log file name specified"));
			bError = TRUE;
			}
	
		if (Options_IsEmptyStringByEntry(hAPI->pOptCACertFileName, &bError)
			   && Options_IsEmptyStringByEntry(hAPI->pOptCACertPath, &bError)) 
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Neither CA Certificate path nor file name specified"));
			bError = TRUE;
			}

		if (Options_GetUint32ByEntry(hAPI->pOptEAPMethod, &bError) == EAP_METHOD_TLS)
			{
			if (Options_IsEmptyStringByEntry(hAPI->pOptTLSDeviceCertFileName, &bError))
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR: No Client Certificate file name specified for EAP-TLS"));
				bError = TRUE;
				}
			else 
				{ 
				STRING name = Options_GetStringByEntry(hAPI->pOptTLSDeviceCertFileName,NULL );
				if(Options_IsFileOnDeviceMem(name))
				{
					if(!Options_ValidateAndGetSlotNumber(name))
					{
						PrintDiag(DIAG_ERROR, hAPI, S("The Slot number mentioned for Client Certificate file is invalid. "));
						bError = TRUE;
					}
				}
				}

			if (Options_IsEmptyStringByEntry(hAPI->pOptTLSDevicePrivateKeyFileName, &bError))
				{
				PrintDiag(DIAG_ERROR, hAPI, S("ERROR: No Client Private Key file name specified for EAP-TLS"));
				bError = TRUE;
				}
			if (Options_IsEmptyStringByEntry(hAPI->pOptTLSDeviceSubCA1CertFileName, NULL)) 
			{
				PrintDiag(DIAG_ERROR, hAPI, S("WARNING: No 1st SubCA Certificate file name specified for EAP-TLS"));
			}	
			if (Options_IsEmptyStringByEntry(hAPI->pOptTLSDeviceSubCA2CertFileName, NULL))
			{
				PrintDiag(DIAG_ERROR, hAPI, S("WARNING: No 2nd SubCA certificate file name specified for EAP-TLS"));
			}	
			}
		}

	return !bError;

	} /* BeceemAPI_CheckOptionValues */

// -----------------------------------------------------------------------------------------
// Globally Disable and Enable timeouts
//
// Globally disable timeouts
// Return the pre-existing timeout status
BOOL BeceemAPI_DisableTimeouts(BeceemAPI_t hAPI)
	{
	BOOL bPreviousTimeoutStatus = hAPI->bEnableTimeouts;
	hAPI->bEnableTimeouts = FALSE;
	BeceemAPI_DeactivateCounters(hAPI);
	return bPreviousTimeoutStatus;
	} /* BeceemAPI_DisableTimeouts */

// -------------------------------------------------------------
// Globally enable timeouts
// Returns: Previous timeout status
BOOL BeceemAPI_EnableTimeouts(BeceemAPI_t hAPI)
	{
	BOOL bPreviousTimeoutStatus = hAPI->bEnableTimeouts;
	hAPI->bEnableTimeouts = TRUE;
	BeceemAPI_DeactivateCounters(hAPI);
	return bPreviousTimeoutStatus;
	} /* BeceemAPI_EnableTimeouts */

// -------------------------------------------------------------
// Reset all the timeout counters
void BeceemAPI_DeactivateCounters(BeceemAPI_t hAPI)
	{
	int n;
	for (n=0; n < ID_EVENT_INVALID; n++)
		hAPI->astEventData[n].u32WaitingMs = 0;

	} /* BeceemAPI_DeactivateCounters */

// -------------------------------------------------------------
// Set the timeout for a specific ID
void BeceemAPI_SetTimeout (BeceemAPI_t hAPI, UINT32 u32EventID, UINT32 u32TimeoutMs)
	{

	hAPI->astEventData[u32EventID].u32TimeoutMs = u32TimeoutMs;

	} /* BeceemAPI_SetTimeout */

// -------------------------------------------------------------
// Check if a specific timeout has expired
BOOL BeceemAPI_CheckTimeout(BeceemAPI_t hAPI, UINT32 u32EventID)
	{

	stTimeout_t *pstTimeout = &hAPI->astEventData[u32EventID];

	if (pstTimeout->bActive)
		{
		pstTimeout->u32WaitingMs += CHECK_TIMEOUTS_TIMER_INTERVAL_MS;
		if (pstTimeout->u32WaitingMs > pstTimeout->u32TimeoutMs)
			{
			BeceemAPI_Deactivate(hAPI, u32EventID);
			pstTimeout->u32EventCount++;
			return TRUE;
			}
		}
	else
		pstTimeout->u32WaitingMs = 0;

	return FALSE;

	} /* BeceemAPI_CheckTimeout */

// -------------------------------------------------------------
// Starts the wait for a specific timeout
void BeceemAPI_Activate(BeceemAPI_t hAPI, UINT32 u32EventID)
	{
	hAPI->astEventData[u32EventID].bActive	= TRUE;
	} /* BeceemAPI_Deactivate */

// -------------------------------------------------------------
// Resets a specific timeout
void BeceemAPI_Deactivate(BeceemAPI_t hAPI, UINT32 u32EventID)
	{
	hAPI->astEventData[u32EventID].bActive	    = FALSE;
	hAPI->astEventData[u32EventID].u32WaitingMs = 0;
	} /* BeceemAPI_Deactivate */

// -------------------------------------------------------------
// Starts the wait for a specific timeout
BOOL BeceemAPI_IsActivated(BeceemAPI_t hAPI, UINT32 u32EventID)
	{
	return hAPI->astEventData[u32EventID].bActive;
	} /* BeceemAPI_IsActivated */


void BeceemAPI_ResetStatistics (BeceemAPI_t hAPI, BOOL abUseRpt[]) // Reset the baseband statistics counter displays
	{
	UINT32 n;
	for (n=0; n<MAX_STATS_REPORT; n++)
		hAPI->hLinkStatsReport[n].bBBStatsOffset = abUseRpt[n];
	}

BOOL BeceemAPI_Authorizing (BeceemAPI_t hAPI)
	{
	return hAPI->bThreadAuthenticationExists /* TO DO: How to implement "&& threadAuthentication->IsAlive" */;
	}

BOOL BeceemAPI_IsLoaded(BeceemAPI_t hAPI) 
	{
#ifdef WIN32
	if (hAPI->hDLL != NULL) 
		return TRUE;
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: ") S(BECEEM_WIMAX_DLL_FILENAME) S(" is not loaded.\n"));
		return FALSE;
		}
#else
	return TRUE;
#endif
	} /* BeceemAPI_IsLoaded */

BOOL BeceemAPI_IsOpen(BeceemAPI_t hAPI) 
	{
	return hAPI->hDevice != INVALID_HANDLE_VALUE; 
	}

void BeceemAPI_ClearChipsetControlStatus(BeceemAPI_t hAPI)
	{
	hAPI->eChipsetControlStatus = ChipsetIdle;
	}

// -----------------------------------------------------------------------------------------
UINT32 BeceemAPI_GetResponseStatus(BeceemAPI_t hAPI) {

	// ---------------------------------------------------
	// Check for response timeouts

	UINT32 u32ResponseStatus = 0;

	if (!hAPI->bEnableTimeouts) 
		{
		BeceemAPI_DeactivateCounters(hAPI);
		return 0;
		}

	// Net search response timeout
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT))
		{
		BeceemAPI_AbortNetworkSearch(hAPI);
		if (hAPI->hNetSearchParam->bSearchMoreCF)
			{
			BeceemAPI_NetworkSearchRequest(hAPI);
			}
		else
			{
			hAPI->eNetworkSearchState = NsAborted;
			u32ResponseStatus |= (1 << ID_EVENT_NET_SEARCH_TIMEOUT);
			}
		}

	// Sync Up response timeout
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_SYNC_UP_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_SYNC_UP_TIMEOUT);

	// Sync Down response timeout
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT))
		{
		if (hAPI->eNetworkSearchState == NsSyncDown1 ||
			hAPI->eNetworkSearchState == NsSyncDown2)
			BeceemAPI_NetworkSearchRequest(hAPI);
		else
			{
			u32ResponseStatus |= (1 << ID_EVENT_SYNC_DOWN_TIMEOUT);
			BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT, 
				S("Base station sync down timeout"), S("Can't disconnect from the base station"));
			}
		}

	// Net Entry response timeout or DEREG received
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_NET_ENTRY_TIMEOUT);

	// Net Entry dereg received
	if (BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_FAILURE))
		{
		BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_FAILURE);
		BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
		u32ResponseStatus |= (1 << ID_EVENT_NET_ENTRY_FAILURE);
		}

	// Net Dereg response timeout
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_NET_DEREG_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_NET_DEREG_TIMEOUT);
		
	// Baseband Status response timeout
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_BASEBAND_STATUS_TIMEOUT);

	// Chipset reset timeout 
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_CHIPSET_RESET_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_CHIPSET_RESET_TIMEOUT);

	// Firmware download timeout 
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_FW_DOWNLOAD_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_FW_DOWNLOAD_TIMEOUT);

	// Shutdown timeout 
	if (BeceemAPI_CheckTimeout(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT))
		u32ResponseStatus |= (1 << ID_EVENT_SHUTDOWN_TIMEOUT);

	// return status
	return u32ResponseStatus;

	} // GetResponseStatus


// =========================================================================================
// Support functions
// =========================================================================================

// -----------------------------------------------------------------------------------------
// Used to write the "Waiting for card initialization ..." message 
void BeceemAPI_InitBasebandStatusMsgWrite(BeceemAPI_t hAPI) 
	{
	BeceemAPI_SetState(hAPI, STATE_WAITING_FOR_CARD); 
	}

// -----------------------------------------------------------------------------------------
// Used to clear the "Waiting for card initialization ..." message if link status request fails
void BeceemAPI_InitBasebandStatusMsgClear(BeceemAPI_t hAPI) 
	{
	if (hAPI->u32State == STATE_WAITING_FOR_CARD) {
		BeceemAPI_SetState(hAPI, STATE_LIBRARY_INIT); 
		if (hAPI->bMACLoggingEnabled)
			BeceemAPI_MACManagementMessageLoggingRequest(hAPI, TRUE);
		}
	}

// =========================================================================================================
// Inform the user about timeouts
// =========================================================================================================

void BeceemAPI_ShowTimeoutMessage(BeceemAPI_t hAPI, UINT32 u32EventID, STRING bszMessage, STRING bszExplanation) 
	{

	BOOL bError = FALSE;
	B_CHAR bszBuffer[128];

	if (!hAPI)
		return;

	if (u32EventID != ID_EVENT_INVALID)
		{
		if (bszExplanation != NULL)
			StrPrintf(bszBuffer, BCHARSOF(bszBuffer), B_SF S(" (%d times): ") B_SF, 
				bszMessage, hAPI->astEventData[u32EventID].u32EventCount, bszExplanation);
		else
			StrPrintf(bszBuffer, BCHARSOF(bszBuffer), B_SF S(" (%d times)"), 
				bszMessage, hAPI->astEventData[u32EventID].u32EventCount);
		}
	else
		{
		if (bszExplanation != NULL)
			StrPrintf(bszBuffer, BCHARSOF(bszBuffer), B_SF S(": ") B_SF, bszMessage, bszExplanation);
		else
			StrPrintf(bszBuffer, BCHARSOF(bszBuffer), B_SF, bszMessage);
		}

	BeceemAPI_ConnectionError(hAPI, bszBuffer);

	PrintDiag(DIAG_NORMAL,hAPI, S("========================================================\n"));
	PrintDiag(DIAG_NORMAL,hAPI, B_SFNL, bszBuffer);
	PrintDiag(DIAG_NORMAL,hAPI, S("========================================================\n"));

	// After each timeout, unlock the API
	BeceemAPI_Unlock(hAPI, &bError);

	} /* BeceemAPI_ShowTimeoutMessage */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_ConnectionError(void)
//
void BeceemAPI_ConnectionError(BeceemAPI_t hAPI, STRING bszMessage)
	{
	BeceemAPI_SetStateEx(hAPI, STATE_ERROR, bszMessage);
	} /* BeceemAPI_ConnectionError */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_ProgramError(void)
//
// Resets the state to OFF
// TO DO: Make this re-entrant

void BeceemAPI_ProgramError(BeceemAPI_t hAPI, STRING bszErrMsg, STRING bszFile, UINT32 u32Line)
	{

	static B_CHAR bszBuffer[128];

	// Assemble the message
	StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S("Program error: ") B_SF S(" in file: ") B_SF S(", line %d"),
					bszErrMsg, bszFile, u32Line);

	BeceemAPI_ConnectionError(hAPI, bszBuffer);

	} /* BeceemAPI_ProgramError */

// Client lock 
// Returns TRUE if lock successfully acquired
BOOL BeceemAPI_Lock(BeceemAPI_t hAPI, WcmSocket_t hSocket, BOOL *pbError)
	{

	BOOL bLocked = FALSE;
	BOOL bError  = FALSE;
	STRING bszStatus = EMPTY_STRING;
	UINT32 u32LockWaitTimeMs = 0;

	if (!hAPI)
		bError = TRUE;
	else if (hSocket == CSCM_INVALID_SOCKET)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Invalid socket in BeceemAPI_Lock()\n"));
		bError = TRUE;
		}
	else
		{
		while (u32LockWaitTimeMs < CLIENT_MUTEX_MAX_LOCK_WAIT_TIME_MS && !bLocked)
			{
			WcmMutex_Wait(hAPI->hMutexClientLock);
			if (!hAPI->bClientMutexLocked)
				{
				hAPI->socClient = hSocket;
				hAPI->bClientMutexLocked     = TRUE;
				hAPI->u32ClientMutexLockedMs = 0;
				bLocked = TRUE;
				}
			WcmMutex_Release(hAPI->hMutexClientLock);
			if (!bLocked)
				{
				SleepMs(CLIENT_MUTEX_LOCK_WAIT_TIME_INTERVAL_MS);
				u32LockWaitTimeMs += CLIENT_MUTEX_LOCK_WAIT_TIME_INTERVAL_MS;
				}
			}
		}

		if (bError)
			bszStatus = S("ERROR");
		else if (bLocked)
			bszStatus = S("SUCCESS");
		else
			bszStatus = S("TIME OUT");
		PrintDiag(DIAG_NORMAL, hAPI, S("Client socket %08X lock Beceem API: ") B_SF S(" (wait = %u ms)\n"), 
			hSocket, bszStatus, u32LockWaitTimeMs);

	// If lock fails, send back a server busy message
	if (!bLocked && !bError)
		{
		PrintDiag(DIAG_VERBOSE, hAPI, S("INFO: Server busy for socket = %08X\n"), hSocket);
		WcmMutex_Wait(hAPI->hMutexSocketSend);
		if (WcmSocket_WPrintf(hSocket, S(":%d:Server busy - please try again later\n"), ID_MSG_SERVER_BUSY) < 1 ||
			WcmSocket_WPrintf(hSocket, S(":%d:\n>"), ID_MSG_PROMPT) < 1)
			bError = TRUE;
		WcmMutex_Release(hAPI->hMutexSocketSend);
		}

	if (pbError)
		*pbError = bError;

	return bLocked;

	} /* BeceemAPI_Lock */

// Returns TRUE if unlock operation successful
BOOL BeceemAPI_Unlock(BeceemAPI_t hAPI, BOOL *pbError)
	{

	BOOL bError    = FALSE;
	BOOL bUnlocked = FALSE;
	STRING bszStatus = EMPTY_STRING;

	WcmSocket_t socClient;

	if (!hAPI)
		{
		if (pbError)
			*pbError = bError;
		return bUnlocked;
		}

	WcmMutex_Wait(hAPI->hMutexClientLock);
	socClient = hAPI->socClient;

	if (socClient == CSCM_INVALID_SOCKET)
		bError = TRUE;
	if (hAPI->bClientMutexLocked)
		{
		hAPI->socClient = CSCM_INVALID_SOCKET;
		hAPI->bClientMutexLocked     = FALSE;
		hAPI->u32ClientMutexLockedMs = 0;
		bUnlocked = TRUE;
		}
	WcmMutex_Release(hAPI->hMutexClientLock);

	if (bError)
		// Note: This message occurs in normal operation when autoreconnect is enabled
		//       Generally it can be ignored.
		PrintDiag(DIAG_VERBOSE, hAPI, S("INFO: No socket in BeceemAPI_Unlock()\n"));

	if (bUnlocked)
		bszStatus = S("Success");
	else
		bszStatus = S("Was not locked (this is not an error)");
	PrintDiag(DIAG_NORMAL, hAPI, S("Client socket %08X unlock Beceem API: ") B_SFNL, 
		socClient, bszStatus);

	if (pbError)
		*pbError = bError;

	return bUnlocked;

	} /* BeceemAPI_Unlock */

void ServerSocketWPrintf(BeceemAPI_t hAPI, STRING bszFormat, ...)
	{

	BOOL bError = FALSE;

	va_list args;

	va_start(args, bszFormat);

	if (hAPI->socClient != CSCM_INVALID_SOCKET)
		{
		WcmMutex_Wait(hAPI->hMutexSocketSend);
		if (WcmSocket_VWPrintf(hAPI->socClient, bszFormat, args) < 1)
			bError = TRUE;

		PrintDiag(DIAG_DEBUG, hAPI, S("SOCKET SEND START:\n"));
		BeceemAPI_PrintDiag_ArgPtr(DIAG_DEBUG, hAPI, bszFormat, args); 
		PrintDiag(DIAG_DEBUG, hAPI, S("SOCKET END.\n"));

		WcmMutex_Release(hAPI->hMutexSocketSend);
		if (bError)
			{
			hAPI->socClient = CSCM_INVALID_SOCKET;
			PrintDiag(DIAG_DEBUG, hAPI, S("ERROR: Socket error in ServerSocketWPrintf.\n"));
			}
		}

	return;

	} /* ServerSocketWPrintf */

void ServerSocketWPuts(BeceemAPI_t hAPI, STRING bszText)
	{

	BOOL bError = FALSE;

	if (hAPI->socClient != CSCM_INVALID_SOCKET)
		{
		WcmMutex_Wait(hAPI->hMutexSocketSend);
		if (WcmSocket_Send(hAPI->socClient, (const UINT8 *) bszText, (UINT32) BCHARS2BYTES(b_strlen(bszText) + 1)) < 1)
			bError = TRUE;

		PrintDiag(DIAG_DEBUG, hAPI, S("SOCKET SEND START:\n"));
		PrintDiag(DIAG_DEBUG, hAPI, bszText);
		PrintDiag(DIAG_DEBUG, hAPI, S("SOCKET END.\n"));

		WcmMutex_Release(hAPI->hMutexSocketSend);
		if (bError)
			{
			hAPI->socClient = CSCM_INVALID_SOCKET;
			PrintDiag(DIAG_DEBUG, hAPI, S("ERROR: Socket error in ServerSocketWPrintf.\n"));
			}
		}

	} /* ServerSocketWPuts */

void ServerSocketResponse(BeceemAPI_t hAPI, UINT32 u32CmdID, STRING bszText, BOOL bNewLine)
	{

	UINT32 u32TextLen = (UINT32) b_strlen(bszText);
	BOOL bError = FALSE;

	if (hAPI->socClient != CSCM_INVALID_SOCKET)
		{
		PrintDiag(DIAG_VERBOSE, hAPI, S("DEBUG: ServerSocketResponse for ID=%d Text chars=%d: ") B_SFNL, u32CmdID, u32TextLen, bszText);
		WcmMutex_Wait(hAPI->hMutexSocketSend);
		if (WcmSocket_WPrintf(hAPI->socClient, bNewLine ? S(":%d:\n") : S(":%d:"), u32CmdID) < 1 ||
			WcmSocket_Send(hAPI->socClient, (const UINT8 *) bszText, (UINT32) BCHARS2BYTES(u32TextLen + 1)) < 1 ||
			WcmSocket_WPrintf(hAPI->socClient, S(":%d:\n>"), ID_MSG_PROMPT) < 1)
				bError = TRUE;
		WcmMutex_Release(hAPI->hMutexSocketSend);
		if (bError)
			{
			hAPI->socClient = CSCM_INVALID_SOCKET;
			PrintDiag(DIAG_ERROR, hAPI, S("DEBUG: Socket error in ServerSocketResponse\n"));
			}
		}

	BeceemAPI_Unlock(hAPI, &bError);

	} /* ServerSocketResponse */

void ServerSocketPrompt(BeceemAPI_t hAPI)
	{
	ServerSocketWPrintf(hAPI, S(":%d:\n>"), ID_MSG_PROMPT);
	} /* ServerSocketPrompt */

void ServerSocketBeginPrint(PrintContext_t hPrint)
	{
	Print_Flush(hPrint);
	Print_SetBuffered(hPrint, TRUE);
	} /* ServerSocketBeginPrint */

void ServerSocketEndPrint (BeceemAPI_t hAPI, PrintContext_t hPrint, UINT32 u32ID)
	{

	UINT32 u32TextLength;
	STRING bszText; 

	bszText = Print_GetText(hPrint, &u32TextLength);
	Print_Flush(hPrint);
	Print_SetBuffered(hPrint, FALSE);

	ServerSocketResponse(hAPI, u32ID, bszText, TRUE);
	WcmFree(bszText);

	} /* ServerSocketEndPrint */

STRING BeceemAPI_DateTime(BeceemAPI_t hAPI)
	{

	return DateTimeStamp(hAPI->bszDateTime, BCHARSOF(hAPI->bszDateTime));

	} /* BeceemAPI_DateTime */


#ifdef MS_VISUAL_CPP
#pragma warning(push)
#pragma warning(disable: 41)
#endif


