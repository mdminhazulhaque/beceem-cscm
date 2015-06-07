/* ---------------------------------------------------- */
/*	Beceem Client-Server Connection Manager             */
/*	Copyright 2006 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2006                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "BeceemAPI.h"
#include "BeceemCscmTop.h"
#include "BeceemCscmLogin.h"
#include "PrintContext.h"
#include "SysLogFunctions.h"
#include "Utility.h"
#include "Version.h"

extern stCmdInformation_t *g_astCmdInfo;


// -------------------------------------------------------------------------------------------
// Create the CSCM data structure
//
BeceemCSCM_t CSCM_Create(STRING bszConfFilePath, BOOL bDebugMode)
	{

	BeceemCSCM_t hCSCM;

	if ((hCSCM = WcmMalloc(sizeof(stBeceemCSCM_t))) == NULL)
		return NULL;

	// Create the Beceem API class
	hCSCM->hAPI = BeceemAPI_Create(bszConfFilePath, bDebugMode); 
	if (hCSCM->hAPI == NULL)
		return NULL; 

	// Create the Beceem API mutex
	hCSCM->mutexBeceemAPI = WcmMutex_Create();
	if (hCSCM->mutexBeceemAPI == NULL)
		return NULL;

	// Card initialization controls
	hCSCM->u32PostFormShownDelayMs         = 0;
	hCSCM->u32BasebandStatusRequestTimeMs  = 0;
	hCSCM->u32BasebandStatusRequestIntervalMs = 2000;
	hCSCM->u32CheckDeviceOpenTimeMs		   = 0;
	hCSCM->u32AutoReConnectTimeMs          = 0;
	hCSCM->u32AutoReConnectIntervalMs      = 0;
    hCSCM->u32AutoReDisconnectTimeMs       = 0;
	hCSCM->u32AutoReDisconnectIntervalMs   = 0;

	hCSCM->bInitialBasebandStatusRequestIssued = FALSE;
	hCSCM->bInitialBasebandStatusResponseWait  = FALSE;
	hCSCM->bAutonomousScanParamsInitialized 	   = FALSE;
	hCSCM->bReadingIniFile					   = TRUE;

	hCSCM->bReadingIniFile = FALSE;

	// Clear event counts
	CSCM_ClearAllCounts(hCSCM);

	// EAP supplicant state
	hCSCM->bAuthDone	    = TRUE;
	hCSCM->bAuthenticating  = FALSE;
	hCSCM->bDisableConnect  = FALSE;
	hCSCM->eDisableConnectReason = eNoReason;
	hCSCM->u32AuthenticatingISRCount = 0;

	// Other initializations
	hCSCM->bInvalidFWVersionWarningShown  = FALSE;
	hCSCM->bInvalidDLLVersionWarningShown = FALSE;
	hCSCM->pu8Buffer					  = NULL;

	hCSCM->bReadyToAutoConnect			  = FALSE;
	hCSCM->bSearchIsAsync                 = FALSE;
	hCSCM->bSearchInProgress			  = FALSE;

	hCSCM->bszIdentity = EMPTY_STRING;
	hCSCM->bszPassword = EMPTY_STRING;

	CSCM_UpdateOptions(hCSCM);

	// -------------------------------------------------------------------
	// Create timer ISR threads and events
	// -------------------------------------------------------------------

	hCSCM->eventExitTimerAuthentication = WcmEvent_Create();
	hCSCM->threadTimerAuthentication = 
		WcmThread_Create(
				&CSCM_ThreadTimerAuthentication,
				hCSCM,
				NULL);

	hCSCM->eventExitTimerCheckTimeouts = WcmEvent_Create();
	hCSCM->threadTimerCheckTimeouts = 
		WcmThread_Create(
				&CSCM_ThreadTimerCheckTimeouts,
				hCSCM,
				NULL);

	hCSCM->eventExitTimerPeriodicAction = WcmEvent_Create();
	hCSCM->threadTimerPeriodicAction = 
		WcmThread_Create(
				&CSCM_ThreadTimerPeriodicAction,
				hCSCM,
				NULL);

	if (!hCSCM->threadTimerAuthentication    ||
		!hCSCM->eventExitTimerAuthentication ||
		!hCSCM->threadTimerCheckTimeouts     ||
		!hCSCM->eventExitTimerCheckTimeouts  ||
		!hCSCM->threadTimerPeriodicAction    ||
		!hCSCM->eventExitTimerPeriodicAction)
		{
		PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Unable to initialize timer threads and events."));
		CSCM_Destroy(hCSCM);
		return NULL;
		}

	return hCSCM;

	} /* CSCM_Create */

// -------------------------------------------------------------------------------------------
// Destroy the CSCM data structure
//
void CSCM_Destroy(BeceemCSCM_t hCSCM)
	{
	
#ifdef LINUX
	BeceemAPI_t hAPI;
#endif

	if (!hCSCM)
		return;

	// Print to system
	SysLogWPrintf(SYSLOG_INFO, S("Starting Beceem CM Server shutdown ... "));

#ifdef LINUX
	hAPI = hCSCM->hAPI;
	if (hAPI != NULL && hAPI->bTerminateDHCPClient)
		{
		PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Terminating IP Refresh process(es).\n"));
		BeceemAPI_IPRefreshKillProcess(hAPI);
		}
#endif

	// Exit the various timer threads immediately
	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Terminating Authentication Timer thread.\n"));
	WcmEvent_Set(hCSCM->eventExitTimerAuthentication);
	WcmThread_Destroy(hCSCM->threadTimerAuthentication);
	WcmEvent_Destroy(hCSCM->eventExitTimerAuthentication);

	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Terminating Check Timeouts Timer thread.\n"));
	WcmEvent_Set(hCSCM->eventExitTimerCheckTimeouts);
	WcmThread_Destroy(hCSCM->threadTimerCheckTimeouts);
	WcmEvent_Destroy(hCSCM->eventExitTimerCheckTimeouts);

	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Terminating Periodic Action Timer thread.\n"));
	WcmEvent_Set(hCSCM->eventExitTimerPeriodicAction);
	WcmThread_Destroy(hCSCM->threadTimerPeriodicAction);
	WcmEvent_Destroy(hCSCM->eventExitTimerPeriodicAction);

	// Destroy other objects
	WcmMutex_Destroy(hCSCM->mutexBeceemAPI);
	BeceemAPI_CloseDevice(hCSCM->hAPI);
	BeceemAPI_Destroy(hCSCM->hAPI);

	// Free the structure
	WcmFree(hCSCM);

	SysLogWPrintf(SYSLOG_INFO, S("Beceem CM Server is stopped"));

	} /* CSCM_Destroy */

// =========================================================================================================
// Open EAP supplicant and get username and password
// Also used to ready the supplicant for re-authentication
// Returns NULL on success; error message on failure
// =========================================================================================================
STRING CSCM_Authenticate(BeceemCSCM_t hCSCM, BOOL bReAuthenticate)
	{

	BeceemAPI_t hAPI;
	BOOL bError = FALSE;
	STRING bszErrorMsg = NULL;
	ST_EAP_INIT_PARAMS stEAPInitParams;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return S("Program ERROR: Null pointer in CSCM_Authenticate");

	if (!Options_GetBoolByEntry(hAPI->pOptAuthEnabled, NULL))
		return S("Authentication is disabled");

	if (!bReAuthenticate)
		{
		hCSCM->bAuthDone = FALSE; 
		if (hAPI->i32AuthenticationState != EAP_REAUTH_WAIT)
			hAPI->i32AuthenticationState = EAP_LOGIN_STARTED;
		}

	if (bReAuthenticate ||
		hAPI->i32AuthenticationState != EAP_REAUTH_WAIT)
		{
			
		if (hAPI->bEAPIsOpen)
			BeceemAPI_EAP_Close(hAPI);

		memset(&stEAPInitParams, 0, sizeof(stEAPInitParams));

		bError = FALSE;
		stEAPInitParams.u32EAPMethod              = Options_GetUint32ByEntry(hAPI->pOptEAPMethod, &bError);
		stEAPInitParams.bszCACertPath         	  = Options_GetStringByEntry(hAPI->pOptCACertPath, &bError); 
		stEAPInitParams.bszCACertFileName         = Options_GetStringByEntry(hAPI->pOptCACertFileName, &bError);
	    stEAPInitParams.bszTTLSAnonymousIdentity  = Options_GetStringByEntry(hAPI->pOptTTLSAnonymousIdentity, &bError);
	    stEAPInitParams.bszTLSDeviceCertFileName  = Options_GetStringByEntry(hAPI->pOptTLSDeviceCertFileName, &bError);
		stEAPInitParams.bszTLSDeviceSubCA1CertFileName  = Options_GetStringByEntry(hAPI->pOptTLSDeviceSubCA1CertFileName, NULL); 
		stEAPInitParams.bszTLSDeviceSubCA2CertFileName  = Options_GetStringByEntry(hAPI->pOptTLSDeviceSubCA2CertFileName, NULL); 
	    stEAPInitParams.bszTLSDevicePrivateKeyFileName = Options_GetStringByEntry(hAPI->pOptTLSDevicePrivateKeyFileName, &bError);
		stEAPInitParams.bszTLSDevicePrivateKeyPassword = Options_GetStringByEntry(hAPI->pOptTLSDevicePrivateKeyPassword, &bError);
		stEAPInitParams.bszPrivateKeyPasswordFormat    = Options_GetStringByEntry(hAPI->pOptPrivateKeyPasswordFormat, &bError);
		stEAPInitParams.bAuthEthernetOverRADIUS   = Options_GetBoolByEntry(hAPI->pOptAuthEthernetToRADIUS, &bError);
	    stEAPInitParams.bszRADIUSIPAddress        = Options_GetStringByEntry(hAPI->pOptRADIUSIPAddress, &bError);
	    stEAPInitParams.u32RADIUSPort             = Options_GetUint32ByEntry(hAPI->pOptRADIUSPort, &bError);
	    stEAPInitParams.bszRADIUSSecret           = Options_GetStringByEntry(hAPI->pOptRADIUSSecret, &bError);
	    stEAPInitParams.u32EloopPollingIntervalMs = Options_GetUint32ByEntry(hAPI->pOptEAPPollingLoopIntervalMs, &bError);
	    stEAPInitParams.u32AuthenticationTimeoutSec = Options_GetUint32ByEntry(hAPI->pOptAuthenticationTimeoutSec, &bError);
		stEAPInitParams.u32AuthLogLevel			  = Options_GetUint32ByEntry(hAPI->pOptAuthLogLevel, &bError);
	    stEAPInitParams.u32RADIUSClientLogLevel   = Options_GetUint32ByEntry(hAPI->pOptRADIUSClientLogLevel, &bError);
	    stEAPInitParams.bszAuthLogFileName        = Options_GetStringByEntry(hAPI->pOptAuthLogFileName, &bError);
	    stEAPInitParams.bAlwaysIncludeTLSLength   = Options_GetBoolByEntry(hAPI->pOptAlwaysIncludeTLSLength, &bError);
		stEAPInitParams.u32EAPFragmentMaxLength   = Options_GetUint32ByEntry(hAPI->pOptEAPFragmentMaxLength, &bError);
		stEAPInitParams.bFirmwareRSAPrivateKeyEncrypt = Options_GetBoolByEntry(hAPI->pOptFirmwareRSAPrivateKeyEncrypt, &bError);
		stEAPInitParams.u32FirmwarePrivateKeyBits = Options_GetUint32ByEntry(hAPI->pOptFirmwarePrivateKeyBits, &bError);
		stEAPInitParams.bszBeceemEngineFileName   = Options_GetStringByEntry(hAPI->pOptBeceemEngineFileName, &bError);
		stEAPInitParams.bEngineLoggingEnabled     = Options_GetBoolByEntry(hAPI->pOptEngineLoggingEnabled, &bError);
		stEAPInitParams.bszEngineLogFileName      = Options_GetStringByEntry(hAPI->pOptEngineLogFileName, &bError);
		stEAPInitParams.bValidateServerCert       = Options_GetBoolByEntry(hAPI->pOptValidateServerCert, &bError);
		stEAPInitParams.bEnableAuthSysLogPrints   = Options_GetBoolByEntry(hAPI->pOptAuthEnableSysLogPrints, &bError);
		hAPI->bInnerNAIChange                     = Options_GetBoolByEntry(hAPI->pOptInnerNAIChange, &bError);
		
		if (bError)
			bszErrorMsg = S("Program ERROR: Option value retrieval");

		if(!bError && stEAPInitParams.bValidateServerCert)
		{
			if((stEAPInitParams.bszCACertPath[0] == B_NUL 
				|| !b_strcmp(stEAPInitParams.bszCACertPath, EMPTY_STRING)) &&
				(stEAPInitParams.bszCACertFileName[0] == B_NUL 
				|| !b_strcmp(stEAPInitParams.bszCACertFileName, EMPTY_STRING)))
					bszErrorMsg = S("Program ERROR: Server Certificate validation is enabled but CA path/file-name not given");
		}
			
			
		BeceemAPI_EAP_Init(hAPI, &stEAPInitParams);
		}
	else
		memset(&stEAPInitParams, 0, sizeof(stEAPInitParams));

	if (!hAPI->bEAPIsOpen) 
		{
		bszErrorMsg = S("Failed to open EAP supplicant");
		PrintDiag(DIAG_ERROR, hAPI, B_SFNL, bszErrorMsg);
		}

	else
		{
		// Get the username and passowrd
		bError = FALSE;
		hCSCM->bszIdentity = Options_GetStringByEntry(hAPI->pOptIdentity, &bError);
		hCSCM->bszPassword = Options_GetStringByEntry(hAPI->pOptPassword, &bError);
		if (bError)
			bszErrorMsg = S("Program ERROR: Option value retrieval");
		hCSCM->bUsePasswordField = 
			stEAPInitParams.u32EAPMethod == EAP_METHOD_TTLS_MSCHAPV2 ||
			stEAPInitParams.u32EAPMethod == EAP_METHOD_TTLS_MSCHAPV2_DEVICE_CERT ||
			stEAPInitParams.u32EAPMethod == EAP_METHOD_TTLS_CHAP;
		if (!bReAuthenticate && hAPI->i32AuthenticationState != EAP_REAUTH_WAIT)
			{
			BOOL bPreviousEnableTimeouts = BeceemAPI_DisableTimeouts(hAPI);
			if (!Login_CheckValid(hCSCM))
				{
				bszErrorMsg = S("Invalid user identity or password length");
				PrintDiag(DIAG_ERROR, hAPI, B_SFNL, bszErrorMsg);
				SysLogWPrintf(SYSLOG_ERROR, bszErrorMsg);
				}
			if (bPreviousEnableTimeouts)
				BeceemAPI_EnableTimeouts(hAPI);
			}
			
		if (bszErrorMsg == NULL)
			{
			if (bszErrorMsg == NULL &&
				Login_IsIdentityValid(hCSCM) &&
				Login_IsPasswordValid(hCSCM))
				{
				BeceemAPI_EAP_Login (hAPI, hCSCM->bszIdentity, hCSCM->bszPassword);

				if (bReAuthenticate)
					{
					hAPI->i32AuthenticationState   = EAP_REAUTH_WAIT;
					hAPI->i32ReauthenticationState = STATE_REAUTH_WAITING;
					}
				else
					{
					hAPI->i32AuthenticationState = EAP_AUTH_WAIT_SUPPLICANT;
					hCSCM->bAuthenticating = TRUE;
					hCSCM->u32AuthenticatingISRCount = 0;
					}

				bError = FALSE;
				}
			else
				{
				bszErrorMsg = S("ERROR: No username or password provided");
				PrintDiag(DIAG_ERROR, hAPI, B_SFNL, bszErrorMsg);
				}
			}
		}

	// Clean up on error
	if (bszErrorMsg != NULL)
		{
		if (hAPI->bEAPIsOpen)
			BeceemAPI_EAP_Close(hAPI);
		hCSCM->bAuthDone = TRUE; 
		hAPI->i32AuthenticationState = hAPI->bAuthEnabled ? EAP_AUTH_OFF : EAP_AUTH_DISABLED;;
		return bszErrorMsg;
		} 
	else
		return NULL;

	} // Authenticate

// =========================================================================================================
// CSCM_AbortAuthentication
// Also used to ready the supplicant for re-authentication
// =========================================================================================================

void CSCM_AbortAuthentication(BeceemCSCM_t hCSCM, BOOL bSendEAPComplete)
	{

	BeceemAPI_t hAPI;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	BeceemAPI_AbortAuth(hAPI, bSendEAPComplete);
	hAPI->bAuthExchangeActive = FALSE; 
	hCSCM->bAuthDone = TRUE;
	hCSCM->bAuthenticating = FALSE;
	PrintDiag(DIAG_NORMAL,hAPI, S("\n=============================\n"));
	PrintDiag(DIAG_NORMAL,hAPI,   S("Authentication thread stopped\n"));
	PrintDiag(DIAG_NORMAL,hAPI,   S("=============================\n"));

	} /* CSCM_AbortAuthentication */

static void CSCM_PrintSearchResults(BeceemCSCM_t hCSCM)
	{

	BeceemAPI_t hAPI;
	PrintContext_t hPrint;
	hBSInformation_t hInfo;
	INT32 s32MaxRSSI = -999, s32RSSI_dBm, s32MaxCINR = -999;
	int n, iSlectedBSIndex = -1;
	BOOL bUseCINR = FALSE;
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	bUseCINR = Options_GetBoolByEntry(hAPI->pOptNEToHighestCINRBaseStation, NULL);
	hPrint = hAPI->hPrintSearch;
	ServerSocketBeginPrint(hPrint);

	Print_Formatted(hPrint, S("Network search returned %d base station") B_SF S(".\n"), 
		hAPI->u32BSInfoCount, hAPI->u32BSInfoCount == 1 ? S("") : S("s"));	
	if (hAPI->u32BSInfoCount > 0)
		Print_Formatted(hPrint, S("Idx BSID                    Pre      Freq      BW  RSSI  CINR\n"));
	
	for (n = 0; n < (int) hAPI->u32BSInfoCount; n++)
		{
		
		hInfo = hAPI->ahBSInfo[n];
		s32RSSI_dBm = hInfo->s32RelativeSignalStrength;

		if(bUseCINR)
		{
			if(s32MaxCINR < hInfo->s32CINR || 
				((s32MaxCINR == hInfo->s32CINR) && (s32MaxRSSI < s32RSSI_dBm)))
			{
				iSlectedBSIndex = n;
				s32MaxCINR = hInfo->s32CINR;
				s32MaxRSSI = s32RSSI_dBm;
			}
		}
		else
		{
			if (s32RSSI_dBm > s32MaxRSSI) 
			{
				iSlectedBSIndex = n;
				s32MaxRSSI	  = s32RSSI_dBm;
			}
			}
		
		Print_Formatted(hPrint, S("%2d  %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x"),
			n,
			hInfo->au8BSID[0], hInfo->au8BSID[1], hInfo->au8BSID[2], hInfo->au8BSID[3], 
			hInfo->au8BSID[4], hInfo->au8BSID[5], hInfo->au8BSID[6], hInfo->au8BSID[7]);
		Print_Formatted(hPrint, S(" 0x%02X%9.3f%8.3f%6d%6d\n"),
			hInfo->u32PreambleIndex,
			hInfo->u32CenterFrequency/1000.0,
			hInfo->u32Bandwidth/1000.0,
			hInfo->s32RelativeSignalStrength,
			hInfo->s32CINR);

		}

	if (iSlectedBSIndex >= 0)
		hAPI->hSelectedBS = hAPI->ahBSInfo[iSlectedBSIndex];

	ServerSocketEndPrint(hAPI, hPrint, ID_MSG_NETWORK_SEARCH);

	// Wait until response sent back to client before allowing another network search
	BeceemAPI_ResetNetworkSearch(hAPI);

	} /* CSCM_PrintSearchResults */

// =========================================================================================================
// Timer interrupt service routine for Authentication
// =========================================================================================================

CSCM_THREAD_FUNCTION CSCM_ThreadTimerAuthentication(void *pvThreadParam)
	{
	
	BeceemCSCM_t hCSCM = (BeceemCSCM_t) pvThreadParam;

	if (!hCSCM)
		return WcmThread_Exit(1);

	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Timer Authentication thread has started.\n"));

	while (!WcmEvent_Wait(hCSCM->eventExitTimerAuthentication, AUTHENTICATION_TIMER_INTERVAL_MS))
		CSCM_AuthenticationTimerISR(hCSCM);

	PrintDiag(DIAG_NORMAL, hCSCM->hAPI, S("Timer Authentication thread has finished.\n"));

	return WcmThread_Exit(0);

	} /* CSCM_ThreadTimerAuthentication */

CSCM_THREAD_FUNCTION CSCM_ThreadTimerCheckTimeouts(void *pvThreadParam)
	{

	BeceemCSCM_t hCSCM = (BeceemCSCM_t) pvThreadParam;

	if (!hCSCM)
		return WcmThread_Exit(1);

	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Timer Check Timeouts thread has started.\n"));

	while (!WcmEvent_Wait(hCSCM->eventExitTimerCheckTimeouts, CHECK_TIMEOUTS_TIMER_INTERVAL_MS))
		CSCM_CheckTimeoutsTimerISR(hCSCM);

	PrintDiag(DIAG_NORMAL, hCSCM->hAPI, S("Timer Check Timeouts thread has finished.\n"));

	return WcmThread_Exit(0);

	} /* CSCM_ThreadTimerCheckTimeouts */

CSCM_THREAD_FUNCTION CSCM_ThreadTimerPeriodicAction(void *pvThreadParam)
	{

	BeceemCSCM_t hCSCM = (BeceemCSCM_t) pvThreadParam;

	if (!hCSCM)
		return WcmThread_Exit(1);

	if(hCSCM->u32AutoReConnectIntervalMs >= 1000) //first attempt to connect should be sooner than 
		hCSCM->u32AutoReConnectTimeMs =  hCSCM->u32AutoReConnectIntervalMs - 300; // subsequent ones...
	PrintDiag(DIAG_NORMAL,hCSCM->hAPI, S("Timer Periodic Action thread has started.\n"));

	while (!WcmEvent_Wait(hCSCM->eventExitTimerPeriodicAction, PERIODIC_ACTION_TIMER_INTERVAL_MS))
		CSCM_PeriodicActionTimerISR(hCSCM);

	PrintDiag(DIAG_NORMAL, hCSCM->hAPI, S("Timer Periodic Action thread has finished.\n"));

	return WcmThread_Exit(0);

	} /* CSCM_ThreadTimerPeriodicAction */

void CSCM_AuthenticationTimerISR(BeceemCSCM_t hCSCM) 
	{

	BeceemAPI_t hAPI;
	STRING bszRetVal = NULL;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	// ---------------------------------------------------
	// Authentication
	//PrintDiag(DIAG_NORMAL,hAPI, S("***** DEBUG AUTH ***** authenticating = %d - active = %d - authorizing = %d - auth state = %d\n"), 
	//	hCSCM->bAuthenticating, hAPI->bAuthExchangeActive, !BeceemAPI_Authorizing(hAPI),
	//	hAPI->i32AuthenticationState);

	if (!BeceemAPI_IsOpen(hAPI))
		{
		hCSCM->bAuthenticating = FALSE;
		hAPI->bAuthExchangeActive = FALSE; 
		hAPI->i32ReauthenticationState = STATE_REAUTH_OFF;
		BeceemAPI_EAP_Close(hAPI);
		hCSCM->bAuthDone = TRUE;
		}
	
	else if ((hCSCM->bAuthenticating && hAPI->bAuthExchangeActive && !BeceemAPI_Authorizing(hAPI)) ||
			  (hAPI->i32ReauthenticationState == STATE_REAUTH_AUTHENTICATING && 
						   (hAPI->i32AuthenticationState == SUCCESS || 
							hAPI->i32AuthenticationState == EAP_AUTH_TIMED_OUT ||
							hAPI->i32AuthenticationState == EAP_AUTH_KEY_ERROR ||
							hAPI->i32AuthenticationState == EAP_AUTH_KEY_MISMATCH)))
		{
		// Save a copy of the the authentication state before calling EAP_Close()
		INT32 i32AuthenticationState = hAPI->i32AuthenticationState;

		hAPI->bAuthExchangeActive = FALSE; 
		hAPI->i32ReauthenticationState = STATE_REAUTH_DONE;

		hCSCM->bAuthenticating = FALSE;

		BeceemAPI_EAP_Complete(hAPI);

		BeceemAPI_EAP_Close(hAPI); 

		// After authentication fails, send a SYNC DOWN request
		if (!hAPI->bAuthEthernetToRADIUS &&
			 i32AuthenticationState != SUCCESS)
			BeceemAPI_SyncDownRequest(hAPI);

		PrintDiag(DIAG_NORMAL,hAPI, S("\n --------------------- EAP Authentication ------------------ \n"));
		PrintDiag(DIAG_NORMAL,hAPI, B_SFNL, BeceemAPI_DecodeAuthState(i32AuthenticationState));
		if (i32AuthenticationState == SUCCESS) // SUCCESS = EAP_AUTH_SUCCESS = 0
			{
				PrintDiag(DIAG_NORMAL,hAPI, S("Master Session Key:\n"));
				StringHexDump(DIAG_NORMAL, hAPI, FALSE, hAPI->au8MasterSessionKey, MSK_LEN);
				PrintDiag(DIAG_NORMAL,hAPI, S(" ----------------------------------------------------------- \n"));

				if(Options_GetUint32ByEntry(hAPI->pOptEAPMethod, NULL) == EAP_METHOD_TLS ||
					Options_GetUint32ByEntry(hAPI->pOptEAPMethod, NULL) == EAP_METHOD_TTLS_EAP_TLS)
				{
					PrintDiag(DIAG_NORMAL,hAPI, S("EXTENDED Master Session Key:\n"));
					StringHexDump(DIAG_NORMAL, hAPI, FALSE, &hAPI->au8MasterSessionKey[MSK_LEN], EMSK_LEN);
				}
			}
		PrintDiag(DIAG_NORMAL,hAPI, S(" ----------------------------------------------------------- \n"));
		
		// Update connection counts
		if (i32AuthenticationState != EAP_AUTH_OFF && i32AuthenticationState != EAP_AUTH_DISABLED)
			{
			hCSCM->u32NumberOfAuthCompleted++;
			if (i32AuthenticationState == SUCCESS)
				hCSCM->u32NumberOfAuthOK++;
			else if (i32AuthenticationState == EAP_AUTH_TIMED_OUT)
				hCSCM->u32NumberOfAuthTimedOut++;
			else 
				hCSCM->u32NumberOfAuthOtherFailure++;
			}

		BeceemAPI_EnableTimeouts(hAPI);
		hCSCM->bAuthDone = TRUE;

		if ((bszRetVal = CSCM_Authenticate(hCSCM, TRUE)) != NULL)
			{
			PrintDiag(DIAG_ERROR, hAPI, bszRetVal);
			}

		}

	// Trap 'zombie' authentications that don't get terminated properly
	else if (!hCSCM->bAuthenticating)
		{
		hCSCM->u32AuthenticatingISRCount = 0;
		}
	else if (hCSCM->bAuthenticating && !hAPI->bAuthExchangeActive && !BeceemAPI_Authorizing(hAPI))
		{
		hCSCM->u32AuthenticatingISRCount++;
		if (hCSCM->u32AuthenticatingISRCount > AUTHENTICATION_WATCHDOG_MAX_COUNT)
			{
			CSCM_AbortAuthentication(hCSCM, FALSE);
			PrintDiag(DIAG_NORMAL, hAPI, S("WARNING: Authentication watchdog activated\n"));
			}
		}

	} /* AuthenticationTimerISR */

// =========================================================================================================
// Timer interrupt service routine for Timeouts
// =========================================================================================================
void CSCM_CheckTimeoutsTimerISR(BeceemCSCM_t hCSCM) 
	{

	BOOL bClientMutexLocked = FALSE;
	BOOL bError = FALSE;
	BeceemAPI_t hAPI;
	UINT32 u32ResponseStatus = 0;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	// ---------------------------------------------------
	// Watchdog timer for Client Mutex Lock
	WcmMutex_Wait(hAPI->hMutexClientLock);
	bClientMutexLocked = hAPI->bClientMutexLocked;
	WcmMutex_Release(hAPI->hMutexClientLock);
	if (bClientMutexLocked)
		{
		hAPI->u32ClientMutexLockedMs += CHECK_TIMEOUTS_TIMER_INTERVAL_MS;
		if (hAPI->u32ClientMutexLockedMs > hAPI->u32ClientMutexLockedTimeoutMs)
			{
			BeceemAPI_Unlock(hAPI, &bError);
			CSCM_AbortAuthentication(hCSCM, FALSE);
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Client lock watchdog timer expired after %u sec.\n"),
				hAPI->u32ClientMutexLockedTimeoutMs/1000);
			}
		}

	// ---------------------------------------------------
	// Chipset reset 
	if (hAPI->eChipsetControlStatus != ChipsetIdle)
		{
		if (hAPI->eChipsetControlStatus == ChipsetResetSuccess)
			{
			BeceemAPI_ClearChipsetControlStatus(hAPI);
			}

		if (hAPI->eChipsetControlStatus == ChipsetConfigFileDownloadSuccess ||
			hAPI->eChipsetControlStatus == ChipsetConfigFileDownloadFailure ||
			hAPI->eChipsetControlStatus == ChipsetFirmwareDownloadSuccess   ||
			hAPI->eChipsetControlStatus == ChipsetFirmwareDownloadFailure)
			{
			BeceemAPI_ClearChipsetControlStatus(hAPI);
			}
		}

	// ---------------------------------------------------
	// Check for response timeouts and other conditions
	u32ResponseStatus = BeceemAPI_GetResponseStatus(hAPI);

	// Net search response timeout
	if (u32ResponseStatus & BM_EVENT_NET_SEARCH_TIMEOUT) 
		{
		hCSCM->bSearchInProgress = FALSE;
		hCSCM->bSearchIsAsync    = FALSE;
		//We cannot call ShowTimeoutMessage in this case, because the BeceemAPI_Unlock() function called inside of it
		//sets the client socket to INVALID_SOCKET, which means that no response gets back to the client.
		///BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT, S("Network search timeout"), S("Can't find any base stations"));
		}

	// Sync Up response timeout
	if (u32ResponseStatus & BM_EVENT_SYNC_UP_TIMEOUT)
		{		
		CSCM_AbortAuthentication(hCSCM, FALSE);
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_SYNC_UP_TIMEOUT, S("Base station sync up timeout"), S("Can't sync to the requested base station"));		
		}

	// Sync Down response timeout
	if (u32ResponseStatus & BM_EVENT_SYNC_DOWN_TIMEOUT)
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_SYNC_DOWN_TIMEOUT, S("Base station sync down timeout"), S("Can't disconnect from the base station"));

	// Net Entry failure (PKM, SATEK, REG, and DREG failures)
	if (u32ResponseStatus & BM_EVENT_NET_ENTRY_FAILURE) 
		CSCM_AbortAuthentication(hCSCM, FALSE);

	// Net Entry response timeout
	if (u32ResponseStatus & BM_EVENT_NET_ENTRY_TIMEOUT) 
		{
		// After authentication times out, send a SYNC DOWN request
		if (!Options_GetBoolByEntry(hAPI->pOptAuthEthernetToRADIUS, NULL))
			{
			BeceemAPI_SyncDownRequest(hAPI);		
			CSCM_AbortAuthentication(hCSCM, FALSE);
			BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT, S("Net entry timeout"), S("Can't join the network"));
			}
		}

	// Net Dereg response timeout
	if (u32ResponseStatus & BM_EVENT_NET_DEREG_TIMEOUT)	
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_NET_DEREG_TIMEOUT, S("Network deregistration request timeout"), NULL);

	// link Status response timeout
	if ((u32ResponseStatus & BM_EVENT_BASEBAND_STATUS_TIMEOUT))
		{
		if (hCSCM->bInitialBasebandStatusResponseWait)
			{
			hCSCM->bInitialBasebandStatusResponseWait = FALSE;
			BeceemAPI_InitBasebandStatusMsgClear(hAPI);
			}
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_BASEBAND_STATUS_TIMEOUT, S("Link status response timeout"), NULL);
		}

	// Chipset reset 
	if ((u32ResponseStatus & BM_EVENT_CHIPSET_RESET_TIMEOUT))
		{
		BeceemAPI_ConnectionError(hAPI, S("Chipset reset timeout"));	
		BeceemAPI_ClearChipsetControlStatus(hAPI);
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_CHIPSET_RESET_TIMEOUT, S("Chipset reset timeout"), NULL);
		}

	// Firmware download 
	if ((u32ResponseStatus & BM_EVENT_FW_DOWNLOAD_TIMEOUT))
		{
		BeceemAPI_ConnectionError(hAPI, S("Firmware download timeout"));
		BeceemAPI_ClearChipsetControlStatus(hAPI);
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_FW_DOWNLOAD_TIMEOUT, S("Firmware download timeout"), NULL);
		}
		
	// Shutdown
	if ((u32ResponseStatus & BM_EVENT_SHUTDOWN_TIMEOUT))
		BeceemAPI_ShowTimeoutMessage(hAPI, ID_EVENT_SHUTDOWN_TIMEOUT, S("Shutdown timeout"), NULL);

	// Try to open the device if it's not already open
	if (!BeceemAPI_IsOpen(hAPI))
		{
		hCSCM->u32CheckDeviceOpenTimeMs += CHECK_TIMEOUTS_TIMER_INTERVAL_MS;
		if (hCSCM->u32CheckDeviceOpenTimeMs >= CHECK_DEVICE_OPEN_INTERVAL_MS)
			{
			hCSCM->u32CheckDeviceOpenTimeMs = 0;
			BeceemAPI_OpenDevice(hAPI);
			}
		}

	// Check for card removed
	if (hAPI->u32State == STATE_UNLOADED)
	{
		BeceemAPI_CloseDevice(hAPI);
		hCSCM->bAutonomousScanParamsInitialized = FALSE;	
	}

	// Invalid Library versions
	if (!hAPI->bValidDLLVersion && !hCSCM->bInvalidDLLVersionWarningShown)
		{
		hCSCM->bInvalidDLLVersionWarningShown = TRUE;
		PrintDiag(DIAG_ERROR, hAPI, S("Invalid Beceem WiMAX API library version.\n") 
			S("The CSCM requires library version = ") B_SFNL
			S("but the actual library version   = ") B_SFNL
			S("WARNING: The Beceem CSCM may not work properly with this library version.\n"),
			TARGET_DLL_VERSION, hAPI->bszActualDLLVersion);
		}

	// Invalid firmware version
	if (!hAPI->bValidFWVersion && !hCSCM->bInvalidFWVersionWarningShown)
		{
		hCSCM->bInvalidFWVersionWarningShown = TRUE;
		PrintDiag(DIAG_ERROR, hAPI, 
			S("WARNING: Invalid Beceem firmware version: ") B_SFNL
			S("Please check that the Beceem driver software is properly installed and configured.\n") ,
			hAPI->bszActualFWVersion);
		}

	} // CheckTimeoutsTimerISR

// =========================================================================================================
// Process link status requests, automatic connect/disconnect, and other tasks
// =========================================================================================================

void CSCM_PeriodicActionTimerISR(BeceemCSCM_t hCSCM) 
	{

	BeceemAPI_t hAPI;
	BOOL bError = FALSE;
	BOOL bAutoReConnectEnabled,
		 bAutoReDisconnectEnabled,
		 bAuthEnabled,
		 bSkipNetSearch,
		 bAuthEthernetToRADIUS;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	// ---------------------------------------------------
	// Get various option values
	// ---------------------------------------------------
	bAutoReConnectEnabled       = Options_GetBoolByEntry(hAPI->pOptAutoReConnectEnabled, &bError);
	bAutoReDisconnectEnabled    = Options_GetBoolByEntry(hAPI->pOptAutoReDisconnectEnabled, &bError);
	bAuthEnabled                = Options_GetBoolByEntry(hAPI->pOptAuthEnabled, &bError);
	bSkipNetSearch              = Options_GetBoolByEntry(hAPI->pOptSkipNetSearch, &bError);
	bAuthEthernetToRADIUS       = Options_GetBoolByEntry(hAPI->pOptAuthEthernetToRADIUS, &bError);

	// ---------------------------------------------------
	// Automatic Link Status requests
	// ---------------------------------------------------
	// Issue the first Link Status request 
	if (!hCSCM->bInitialBasebandStatusRequestIssued && BeceemAPI_IsOpen(hAPI))
		{
		if (hCSCM->u32PostFormShownDelayMs < POST_FORM_SHOWN_DELAY_MS)
			hCSCM->u32PostFormShownDelayMs += PERIODIC_ACTION_TIMER_INTERVAL_MS;
		else
			{
			BeceemAPI_InitBasebandStatusMsgWrite(hAPI);
			hCSCM->bInitialBasebandStatusRequestIssued = TRUE;
			hCSCM->bInitialBasebandStatusResponseWait  = TRUE;
			hCSCM->u32BasebandStatusRequestTimeMs      = 0;
			BeceemAPI_IdleModeStatusRequest(hAPI);
			BeceemAPI_ShutdownStatusRequest(hAPI);
//For embedded platforms, the halfminicard status check is not required.
#if !defined(PLATFORM_BCS5350)  && !defined(PLATFORM_BCS5200)
			BeceemAPI_HalfMiniCardStatusRequest(hAPI);
#endif
			BeceemAPI_BasebandStatusRequest(hAPI);
			}
		}

	// ---------------------------------------------------
	// Send periodic link status requests
	if (BeceemAPI_IsOpen(hAPI))
		{
		hCSCM->u32BasebandStatusRequestTimeMs += PERIODIC_ACTION_TIMER_INTERVAL_MS;
		if (hCSCM->u32BasebandStatusRequestTimeMs > hCSCM->u32BasebandStatusRequestIntervalMs &&
			hCSCM->u32BasebandStatusRequestIntervalMs > 0)
			{
			hCSCM->bInitialBasebandStatusResponseWait = FALSE;
			hCSCM->u32BasebandStatusRequestTimeMs = 0;
			BeceemAPI_BasebandStatusRequest(hAPI);
			}
		}

	// ---------------------------------------------------
	// Check and configure the LPSM params if LPSM is enabled.
	if (BeceemAPI_IsOpen(hAPI) && Options_GetBoolByEntry(hAPI->pOptLPSearchInShutDownEnabled, NULL) 
									&& !hCSCM->bAutonomousScanParamsInitialized)
	{
		BeceemAPI_ClearBSInformation(hAPI);
		BeceemAPI_ConfigLPSMParamsRequest(hAPI);
		hCSCM->bAutonomousScanParamsInitialized = TRUE;
	}

	// ---------------------------------------------------
	// Check for network search response
	if (hAPI->bIsNetworkSearchDone) 
	{

		if (!hCSCM->bSearchIsAsync)
			CSCM_PrintSearchResults(hCSCM);

		hAPI->bIsNetworkSearchDone = FALSE;

		if (hAPI->u32BSInfoCount > 0 && bAutoReConnectEnabled)	
			{			
			hCSCM->bReadyToAutoConnect = TRUE;
			}

		hCSCM->bSearchInProgress = FALSE;
		hCSCM->bSearchIsAsync = FALSE;

	}

	// ---------------------------------------------------
	// Do auto connect and disconnect 

	if (!(BeceemAPI_IsLoaded(hAPI) && BeceemAPI_IsOpen(hAPI)))
		{
		//Do nothing
		}
	else if (BeceemAPI_IsConnectedState(hAPI))
		{

		hCSCM->bDisableConnect       = TRUE;
		hCSCM->eDisableConnectReason = eAlreadyConnReason;

		// Do the periodic auto re-disconnect 
		if (bAutoReDisconnectEnabled)
			{
			hCSCM->u32AutoReDisconnectTimeMs += PERIODIC_ACTION_TIMER_INTERVAL_MS;
			if (hCSCM->u32AutoReDisconnectTimeMs > hCSCM->u32AutoReDisconnectIntervalMs)
				{
				hCSCM->u32AutoReDisconnectTimeMs = 0;
				CSCM_Disconnect_Command(hCSCM);
				}
			}
		else 
			hCSCM->u32AutoReDisconnectTimeMs = 0;
		}
	else
		{

		// Clear connection manager statistics when device is out
		if (hAPI->u32State == STATE_WAITING_FOR_CARD)
			CSCM_ClearAllCounts(hCSCM);

		// Disable if authentication is running
		if (bAuthEnabled && !hCSCM->bAuthDone)
			{
			hCSCM->bDisableConnect       = TRUE;
			hCSCM->eDisableConnectReason = eAuthInProgReason;
			}

		// Disable if net search is running
		else if (hCSCM->bSearchInProgress)
			{
			hCSCM->bDisableConnect       = TRUE;
			hCSCM->eDisableConnectReason = eSearchInProgReason;
			}

		// Do the periodic auto re-connect 
		else if (bAutoReConnectEnabled)
			{

			hCSCM->bDisableConnect = TRUE;
			hCSCM->eDisableConnectReason = eAutoReConnReason;

			if (hAPI->bAuthExchangeActive || hAPI->u32State == STATE_NET_ENTRY_DONE
				|| (hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD && BeceemAPI_IsActivated(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT)) 
					|| (hAPI->u32State == STATE_WAIT_PHY_SYNC_CMD && BeceemAPI_IsActivated(hAPI, ID_EVENT_SYNC_UP_TIMEOUT)))
				{
				hCSCM->u32AutoReConnectTimeMs = 0;
				hCSCM->bReadyToAutoConnect = FALSE;
				}
			else if (hCSCM->bReadyToAutoConnect && 
				(hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD ||
				 hAPI->u32State == STATE_WAIT_PHY_SYNC_CMD))
				{
				hCSCM->u32AutoReConnectTimeMs = 0;
				hCSCM->bReadyToAutoConnect = FALSE;
				CSCM_Connect_Command(hCSCM, FALSE);
				}
			else
				{
				hCSCM->u32AutoReConnectTimeMs += PERIODIC_ACTION_TIMER_INTERVAL_MS;
				if (hCSCM->u32AutoReConnectTimeMs > hCSCM->u32AutoReConnectIntervalMs)
					{
					if (bSkipNetSearch)
						{
						if (hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD ||
							hAPI->u32State == STATE_WAIT_PHY_SYNC_CMD)
							{
							hCSCM->u32AutoReConnectTimeMs = 0;
							hCSCM->bReadyToAutoConnect = FALSE;
							CSCM_Connect_Command(hCSCM, FALSE);
							}
						}
					else
						{
						if (hAPI->u32State == STATE_ERROR ||
							hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD ||
							hAPI->u32State == STATE_WAIT_PHY_SYNC_CMD)
							{
							hCSCM->u32AutoReConnectTimeMs = 0;
							hCSCM->bReadyToAutoConnect = FALSE;
							CSCM_NetworkSearch_Command(hCSCM, NULL, 0);
							BeceemAPI_LinkStatusRequest(hAPI); //to hasten the search
							}
						}
					}
				}
			}

		// Otherwise enable
		else
			{
			hCSCM->bDisableConnect = FALSE;
			hCSCM->eDisableConnectReason = eNoReason;
			}

		}

	} // PeriodicActionTimerISR(...)} 

// =========================================================================================================
// Download firmware or config file
// =========================================================================================================

void CSCM_DownloadFirmwareOrConfigFile (BeceemCSCM_t hCSCM, UINT32 u32ImageType)
	{

	BeceemAPI_t hAPI;
	STRING bszImageFileName;
	STRING bszImageDescription;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	if (hAPI->u32State == STATE_NET_ENTRY_DONE)
		BeceemAPI_Disconnect(hAPI);

	// Check that files exist and are readable
	if (u32ImageType == IMAGE_FIRMWARE)
		{
		bszImageFileName = Options_GetStringByEntry(hAPI->pOptFirmwareFileName, NULL);
		bszImageDescription = S("Firmware");
		}
	else
		{
		bszImageFileName = Options_GetStringByEntry(hAPI->pOptConfigFileName, NULL);
		bszImageDescription = S("Config");
		}
	
	if (bszImageFileName == NULL || !bszImageFileName[0])
		{
		PrintDiag(DIAG_ERROR, hAPI, S("Please specify the ") B_SF S(" file name in the wimaxd options\n"),
			bszImageDescription);
		return;
		}

	if (!DoesFileExist(bszImageFileName))
		{
		PrintDiag(DIAG_ERROR, hAPI, S("Missing or invalid ") B_SF S(" file: ") B_SF,
			bszImageDescription, bszImageFileName);
		return;
		}

	BeceemAPI_FirmwareDownloadRequest(hAPI, u32ImageType, bszImageFileName);
	if (hAPI->eChipsetControlStatus == ChipsetIdle)
		{
		PrintDiag(DIAG_ERROR, hAPI, B_SF S(" file download request failed"),
			bszImageDescription);
		}

	} // DownloadFirmwareOrConfigFile(...)

// =========================================================================================================
// StartNetworkSearch
// Start a new network search
// =========================================================================================================	

void CSCM_NetworkSearch_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs)
	{

	BeceemAPI_t hAPI;
	STRING bszText;
	enum {SEARCH_ERROR, SEARCH_SYNC, SEARCH_ASYNC, SEARCH_CAPL_SYNC, SEARCH_CAPL_ASYNC,
		SEARCH_STOP, SEARCH_STATUS} eSubCmd;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

#if 0
	PrintDiag(DIAG_NORMAL,hAPI, S("\n***** Search: "));
	for (n=0; n<u32NumberOfArgs; n++)
		PrintDiag(DIAG_NORMAL,hAPI, B_SF S(" "), abszArgStrings[n]);
	PrintDiag(DIAG_NORMAL,hAPI, S("\n"));
#endif

	// Determine the sub-command
	bszText = NULL;
	eSubCmd = SEARCH_ERROR;
	hAPI->bUseCAPLForNetworkSearch = FALSE;
	if (u32NumberOfArgs > 2)
		bszText = S("ERROR: Too many parameters for 'search' command.\n");
	else if (u32NumberOfArgs == 0)
		eSubCmd = SEARCH_SYNC;
	else
		{
		if (BcsCmpNoCase(abszArgStrings[0], S("wait")))
			eSubCmd = SEARCH_SYNC;
		else if (BcsCmpNoCase(abszArgStrings[0], S("nowait")))
			eSubCmd = SEARCH_ASYNC;
		else if (BcsCmpNoCase(abszArgStrings[0], S("capl")))
			{
				hAPI->bUseCAPLForNetworkSearch = TRUE;
				if(u32NumberOfArgs == 1)
					eSubCmd = SEARCH_CAPL_SYNC;
				else if (BcsCmpNoCase(abszArgStrings[1], S("wait")))
					eSubCmd = SEARCH_CAPL_SYNC;
				else if (BcsCmpNoCase(abszArgStrings[1], S("nowait")))
					eSubCmd = SEARCH_CAPL_ASYNC;
				else
					bszText = S("ERROR: 'wait', 'nowait', required after 'search capl'.\n");
			}
		else if (BcsCmpNoCase(abszArgStrings[0], S("stop")))
			eSubCmd = SEARCH_STOP;
		else if (BcsCmpNoCase(abszArgStrings[0], S("status")))
			eSubCmd = SEARCH_STATUS;
		else 
			bszText = S("ERROR: 'wait', 'nowait', 'capl', 'capl wait', 'capl nowait', 'stop', or 'status' required after 'search'.\n");
		}

	if(bszText==NULL && ((hAPI->u32State == STATE_SHUTDOWN) || (hAPI->u32State == STATE_HMC_SHUTDOWN)) && 
				(eSubCmd == SEARCH_ASYNC || eSubCmd == SEARCH_SYNC))
	{	
		BeceemAPI_ClearBSInformation(hAPI); 
		if(!BeceemAPI_NetworkSearchRequest(hAPI))
		{
			bszText = S("ERROR: Couldn't start search in shutdown mode.\n");
			ServerSocketResponse(hAPI, ID_MSG_NETWORK_SEARCH, bszText, FALSE);
		}
		else
		{
			hCSCM->bDisableConnect		 = TRUE;
			hCSCM->eDisableConnectReason = eSearchInProgReason;
			hCSCM->bSearchIsAsync		 = eSubCmd == SEARCH_ASYNC;
			hCSCM->bSearchInProgress	 = TRUE;
			hAPI->eNetworkSearchState = NsSearching;
			BeceemAPI_Activate(hAPI, ID_EVENT_NET_SEARCH_TIMEOUT);
			if (eSubCmd == SEARCH_ASYNC)
			{
				bszText = S("Starting network search");
				ServerSocketResponse(hAPI, ID_MSG_NETWORK_SEARCH, bszText, FALSE);
			}
		}
		return;
	}
	
	// Process the sub-command
	if (bszText == NULL)
		{

		switch (eSubCmd)
			{

			case SEARCH_SYNC:
			case SEARCH_ASYNC:
			case SEARCH_CAPL_SYNC:
			case SEARCH_CAPL_ASYNC:
				if (hCSCM->bSearchInProgress)
					bszText = S("Search in progress");
				else
					{
					hCSCM->bDisableConnect       = TRUE;
					hCSCM->eDisableConnectReason = eSearchInProgReason;
					hCSCM->bSearchIsAsync        = eSubCmd == SEARCH_ASYNC || eSubCmd == SEARCH_CAPL_ASYNC ;
					hCSCM->bSearchInProgress     = TRUE;

					if (!BeceemAPI_InitNetworkSearch(hAPI))
					{
						hCSCM->bDisableConnect       = FALSE;
						hCSCM->eDisableConnectReason = eNoReason;
						hCSCM->bSearchInProgress     = FALSE;	
						bszText = S("Unable to start network search - see error message in CSCM Debug Log file");
					}
					else if (eSubCmd == SEARCH_ASYNC || eSubCmd == SEARCH_CAPL_ASYNC)
						bszText = S("Starting network search");

					}
				break;

			case SEARCH_STOP:
				if (hCSCM->bSearchInProgress)
					{
					BeceemAPI_AbortNetworkSearch(hAPI);
					bszText = S("Stopping network search");
					}
				else
					bszText = S("Network search is not running");
				break;

			case SEARCH_STATUS:
				if (hCSCM->bSearchInProgress)
					bszText = S("Network search is running");
				else
					CSCM_PrintSearchResults(hCSCM);
				break;

			default:
				break;
			}
		}

	if (bszText != NULL)
		ServerSocketResponse(hAPI, ID_MSG_NETWORK_SEARCH, bszText, FALSE);

} /* CSCM_NetworkSearch_Command */

// =========================================================================================================
// Select the base station to which to connect
// =========================================================================================================
#define ERR_MSG_RETURN(msg) \
	{						\
	if (pbError)			\
		*pbError = TRUE;	\
	return msg;				\
	}

STRING CSCM_SelectBSForConnect(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs, BOOL *pbError)
	{
	
	BeceemAPI_t hAPI;
	B_CHAR *wcpend;
	double dCenterFreqMHz, dBandwidthMHz;

	long idx;
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		ERR_MSG_RETURN(S("PROGRAM ERROR: Invalid pointer"))

	if (BeceemAPI_IsConnectedState(hAPI))
		ERR_MSG_RETURN(S("ERROR: Already connected"))

	if (u32NumberOfArgs == 1)
		{
		idx = b_strtol(abszArgStrings[0], &wcpend, 10);
		if (Options_GetBoolByEntry(hAPI->pOptSkipNetSearch, NULL))
			ERR_MSG_RETURN(S("ERROR: Search is disabled by option SkipNetSearch = Yes"))
		else if (*wcpend != B_NUL || idx < 0 || idx >= (long) hAPI->u32BSInfoCount)
			ERR_MSG_RETURN(S("ERROR: Invalid BS selection index"))
		else
			hAPI->hSelectedBS = (hBSInformation_t) hAPI->ahBSInfo[idx]; // Or use the selected one
		}
	else if (u32NumberOfArgs == 2)
		{
		dCenterFreqMHz = b_strtod(abszArgStrings[0], &wcpend);
		if (*wcpend != B_NUL || dCenterFreqMHz < 2300.0 || dCenterFreqMHz > 3700.0)
			ERR_MSG_RETURN(S("ERROR: Invalid center frequency (MHz)"))

		dBandwidthMHz = b_strtod(abszArgStrings[1], &wcpend);
		if (*wcpend != B_NUL || dBandwidthMHz < 4.9 || dBandwidthMHz > 10.1)
			ERR_MSG_RETURN(S("ERROR: Invalid bandwidth (MHz)"))

		BSInformation_SetDefault(&hAPI->stConnectBS);
		hAPI->stConnectBS.u32CenterFrequency = (UINT32) (1000.0 * dCenterFreqMHz + 0.5);
		hAPI->stConnectBS.u32Bandwidth       = (UINT32) (1000.0 * dBandwidthMHz + 0.5);
		hAPI->hSelectedBS = &hAPI->stConnectBS;
		}
	else if (u32NumberOfArgs != 0)  // For u32NumberOfArgs == 0, just use the previous selection
		ERR_MSG_RETURN(S("Too many command arguments"));

	if (pbError)
		*pbError = FALSE;

	return EMPTY_STRING;

	} /* CSCM_SelectBSForConnect */

// =========================================================================================================
// Connect to the base station
// =========================================================================================================
void CSCM_Connect_Command(BeceemCSCM_t hCSCM, BOOL bSendResponseToClient)
	{

	BeceemAPI_t hAPI;
	BOOL bAuthEnabled;
	B_CHAR bszBuffer[256], bszSmallBuffer0[20], bszSmallBuffer1[20];
	STRING bszResponse;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	if (BeceemAPI_IsConnectedState(hAPI))
		{
		ServerSocketResponse(hAPI, ID_MSG_CONNECT, S("Device is already connected"), TRUE); 
		return; 
		}

	hCSCM->u32NumberOfConnectAttempts++;

	bAuthEnabled = Options_GetBoolByEntry(hAPI->pOptAuthEnabled, NULL);
	if (bAuthEnabled && hCSCM->bAuthDone) {
		if ((bszResponse = CSCM_Authenticate(hCSCM, FALSE)) != NULL)
			{
			if (bSendResponseToClient)
				{
				if (Options_GetBoolByEntry(hAPI->pOptAutoReConnectEnabled, NULL))
					bszResponse = S("Cannot connect manually while AutoReconnectEnabled is 'Yes'");
				PrintDiag(DIAG_NORMAL, hAPI, B_SFNL, bszResponse);
				ServerSocketResponse(hAPI, ID_MSG_CONNECT, bszResponse, TRUE);
				}
			return;	
			}
		}
	else if (bAuthEnabled)
		PrintDiag(DIAG_NORMAL, hAPI, S("WARNING: Not able to call CSCM_Authenticate: enabled = %d done = %d\n"),
			bAuthEnabled, hCSCM->bAuthDone);
	else
		PrintDiag(DIAG_NORMAL, hAPI, S("NOTE: Authentication is disabled\n"));

	if (bAuthEnabled && Options_GetBoolByEntry(hAPI->pOptAuthEthernetToRADIUS, NULL))
		BeceemAPI_EnqueueEAPRequestIdentityPacket(hAPI);
	else
		{	
		
		if (hAPI->u32State == STATE_WAIT_NET_ENTRY_CMD)
			{
			if (BeceemAPI_NetworkEntryRequest(hAPI) == ERROR_NET_ENTRY_INPROGRESS)
				{
				CSCM_AbortAuthentication(hCSCM, FALSE);
				ServerSocketResponse(hAPI, ID_MSG_CONNECT, 
					S("Adapter is still completing last step\nPlease wait 10 seconds and try again"), TRUE); 
				hAPI->bIgnoreWaitNetEntryCmd = TRUE;
				return;
				}
			hAPI->bIgnoreWaitNetEntryCmd = TRUE;
			}
		else
			BeceemAPI_SyncUpRequest(hAPI);

		}

	if (bSendResponseToClient)
		{
		if (hAPI->hSelectedBS == NULL)
			{
			if (hAPI->bAuthEthernetToRADIUS && hAPI->bAuthEnabled)
				StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
					S("Started authentication test over Ethernet to RADIUS server"));
			else
				StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
					S("Connect request submitted for:\n")
					S("     Center frequency    From config file\n")
					S("     Bandwidth           From config file\n")
					S("     Preamble index      0xFF"));
			}
		else
			StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
				S("Connect request submitted for:\n")
				S("     Center frequency    ") B_SF S(" MHz\n")
				S("     Bandwidth           ") B_SF S(" MHz\n")
				S("     Preamble index      0x%02X"),
				StrKHzToMHz(bszSmallBuffer0, BCHARSOF(bszSmallBuffer0), hAPI->hSelectedBS->u32CenterFrequency),
				StrKHzToMHz(bszSmallBuffer1, BCHARSOF(bszSmallBuffer1), hAPI->hSelectedBS->u32Bandwidth),
				hAPI->hSelectedBS->u32PreambleIndex);
		ServerSocketResponse(hAPI, ID_MSG_CONNECT, bszBuffer, TRUE);
		}

	} /* CSCM_Connect_Command */

// =========================================================================================================
// Disconnect from the base station
// =========================================================================================================
void CSCM_Disconnect_Command(BeceemCSCM_t hCSCM)
	{

	BeceemAPI_t hAPI;

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	if (!BeceemAPI_IsConnectedState(hAPI))
		{
		ServerSocketResponse(hAPI, ID_MSG_DISCONNECT, S("Not connected"), TRUE); 
		return; 
		}

	BeceemAPI_Disconnect(hAPI);
	CSCM_AbortAuthentication(hCSCM, FALSE);
	ServerSocketResponse(hAPI, ID_MSG_DISCONNECT, S("Disconnected"), TRUE); 

	} /* CSCM_Disconnect_Command */

// --------------------------------------------------------------------------------
// Clear all CSCM event counts
// --------------------------------------------------------------------------------
void CSCM_ClearAllCounts(BeceemCSCM_t hCSCM)
	{

	int n;
	BeceemAPI_t hAPI = hCSCM->hAPI;

	if (!hCSCM)
		return;

	if (hAPI != NULL)
		{
		hAPI->u32NetEntryCompletedCount = 0;
		hAPI->u32AuthStartCount         = 0;
		hAPI->u32NetEntryDeregCount     = 0;     
		hAPI->u32NetEntryPKMCount       = 0;      
		hAPI->u32NetEntrySATEKCount     = 0;    
		hAPI->u32NetEntryRegCount	    = 0;  
		hAPI->u32OtherDeregRcvdCount	= 0;
		for (n=0; n < ID_EVENT_INVALID; n++)
			hAPI->astEventData[n].u32EventCount = 0;
		}

	hCSCM->u32NumberOfConnectAttempts     = 0;
	hCSCM->u32NumberOfAuthCompleted		  = 0;
    hCSCM->u32NumberOfAuthOK			  = 0;
	hCSCM->u32NumberOfAuthTimedOut        = 0;
	hCSCM->u32NumberOfAuthOtherFailure    = 0;

	} /* CSCM_ClearAllCounts */

void CSCM_UpdateOptions(BeceemCSCM_t hCSCM)
	{

	BeceemAPI_t hAPI;

	if (!hCSCM || !hCSCM->hAPI)
		return;

	hAPI = hCSCM->hAPI;

	hCSCM->u32AutoReConnectIntervalMs         = 1000 * Options_GetUint32ByEntry(hAPI->pOptAutoReConnectIntervalSec, NULL);
	hCSCM->u32AutoReDisconnectIntervalMs      = 1000 * Options_GetUint32ByEntry(hAPI->pOptAutoReDisconnectIntervalSec, NULL);
	hCSCM->u32BasebandStatusRequestIntervalMs = 1000 * Options_GetUint32ByEntry(hAPI->pOptLinkStatusRequestPeriodSec, NULL);

//Autoreconnect time is set to 500ms if the input value is 0.
	if(hCSCM->u32AutoReConnectIntervalMs == 0)
		hCSCM->u32AutoReConnectIntervalMs = 500;
	// Minimum value is 500 ms
	if (hCSCM->u32BasebandStatusRequestIntervalMs < 500)
		hCSCM->u32BasebandStatusRequestIntervalMs = 500;

	}

