/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef BECEEM_API_H
#define BECEEM_API_H

// CscmString.h must come first to unset UNICODE and _UNICODE
// in narrow character builds. For convenience, it's also 
// included in CscmTypes.h.
#include "CscmString.h"

// These are WiMAX API library headers
#include "ErrorDefs.h"
#include "Messages.h"
#include "MibUserApi.h"
#include "Typedefs.h"
#include "cntrl_SignalingInterface.h"
// End WiMAX API library headers

#include "LibraryVersion.h"

#include "CscmTypes.h"
#include "PrintContext.h"
#include "AuxStruct.h"
#include "MsgQueue.h"
#include "Options.h"
#include "SocketFunctions.h"

#define MAC_LOGGING_SUPPORT

#define SWAP16(x) ( (UINT16) ( (((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF) ) )

// DLL version MUST match that given below
#define HOST_PROXY_DLL_FILENAME   "hstxvi010.dll"
#define BECEEM_WIMAX_DLL_FILENAME "libxvi010.dll"

// DLL Version must match exactly
#define TARGET_DLL_VERSION        CSCM_API_LIBRARY_VERSION
#define TARGET_DLL_VERSION_ALT    CSCM_API_LIBRARY_VERSION

// MSK len in bytes
#define MSK_LEN 64
#define EMSK_LEN 64
// Length of encryption key for private key stored in NVM
#define PRIV_KEY_AES_KEY_LEN 32

#define RSA_PRIVATE_KEY_ENABLE_OPTION          (1 << 4) 
#define RSA_PRIVATE_KEY_FULL_HOST_SUPPLICANT   0
#define RSA_PRIVATE_KEY_SPLIT_SUPPLICANT       1

#define OPEN_DEVICE_SUCCESS 0x00000000
#define OPEN_DEVICE_FAILURE 0xffffffff

// Initial number of entries in the BS data ArrayList (can grow)
#define INIT_BSDATA_ENTRIES     10

#define MAX_SEARCH_DONE_RETRIES 20

// Period for statistics requests (ms)
#define STATISTICS_PERIOD_MS    1000

// Interval for authentication timer interrupts (ms)
#define AUTHENTICATION_TIMER_INTERVAL_MS         100
#define AUTHENTICATION_WATCHDOG_INTERVAL_MS      10000
#define AUTHENTICATION_WATCHDOG_MAX_COUNT        (AUTHENTICATION_WATCHDOG_INTERVAL_MS/AUTHENTICATION_TIMER_INTERVAL_MS)

// Interval for process messages timer interrupts (ms)
#define PROCESS_MESSAGES_TIMER_INTERVAL_MS       100

// Interval for timeout check timer interrupt (ms)
#define CHECK_TIMEOUTS_TIMER_INTERVAL_MS		 500

// Interval for GUI update timer interrupt (ms)
#define PERIODIC_ACTION_TIMER_INTERVAL_MS		 100

#define CHECK_DEVICE_OPEN_INTERVAL_MS			500

// Watchdog timer for Client lock mutex
#define DEFAULT_CLIENT_MUTEX_LOCK_WATCHDOG_TIMEOUT_MS    60000

// Waiting for client mutex lock
#define CLIENT_MUTEX_MAX_LOCK_WAIT_TIME_MS        5000
#define CLIENT_MUTEX_LOCK_WAIT_TIME_INTERVAL_MS    500

#define RSA_PRIVATE_KEY_ENCRYPTION_TIMEOUT_MS      5000

#define DEFAULT_MAC_ADDRESS						"12:34:56:78:9A:BC"

// Wait a second before trying to initialize the card for the first time
#define POST_FORM_SHOWN_DELAY_MS 100 //reduce the delay to proceed with status checks immedeatly.

// BS with same preamble ID are considered duplicates if their RSSI's differ by <= 
// this constant
#define NET_SEARCH_DUP_MAX_RSSI_DIFF 5

// Identity and password lengths
#define MIN_IDENTITY_LENGTH 2
#define MIN_PASSWORD_LENGTH 2
#define MAX_IDENTITY_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

// EAP methods
#define EAP_METHOD_TTLS_MSCHAPV2                0
#define EAP_METHOD_TTLS_MSCHAPV2_DEVICE_CERT	1
#define EAP_METHOD_TTLS_CHAP					2
#define EAP_METHOD_TTLS_EAP_TLS					3
#define EAP_METHOD_TLS							4
#define EAP_METHOD_TTLS_CHAP_DEVICE_CERT        5
#define EAP_METHOD_TTLS_PAP                     6
#define EAP_METHOD_TTLS_PAP_DEVICE_CERT         7

// These use values between 0 and -9, inclusive
// MUST match those in eap_supplicant.h
#define EAP_SUPPLICANT_SUCCESS 						  0
#define EAP_SUPPLICANT_STARTED 						 -1
#define EAP_SUPPLICANT_TEST_EAPOL_FAILED			 -2
#define EAP_SUPPLICANT_SM_GET_KEY_FAILED			 -3
#define EAP_SUPPLICANT_TIMED_OUT                     -4
#define EAP_SUPPLICANT_RADIUS_ACCESS_REJECT_RECEIVED -5
#define EAP_SUPPLICANT_NUM_MPPE_MISMATCH			 -6
// End of eap_supplicant.h extract

#define EAP_AUTH_SUCCESS	   EAP_SUPPLICANT_SUCCESS
#define EAP_AUTH_STARTING	   EAP_SUPPLICANT_STARTED
#define EAP_AUTH_INIT_ERROR    EAP_SUPPLICANT_TEST_EAPOL_FAILED
#define EAP_AUTH_KEY_ERROR     EAP_SUPPLICANT_SM_GET_KEY_FAILED
#define EAP_AUTH_TIMED_OUT	   EAP_SUPPLICANT_TIMED_OUT
#define EAP_AUTH_RADIUS_REJECT EAP_SUPPLICANT_RADIUS_ACCESS_REJECT_RECEIVED
#define EAP_AUTH_KEY_MISMATCH  EAP_SUPPLICANT_NUM_MPPE_MISMATCH

// Auth is disabled in the Options dialog
#define EAP_AUTH_DISABLED			-10
// Auth is OFF completely
#define EAP_AUTH_OFF			    -11

// Starting user login 
#define EAP_LOGIN_STARTED		    -12
// User has initiated auth -- waiting for supplicant thread to start
#define EAP_AUTH_WAIT_SUPPLICANT    -13
// Thread has started -- waiting for supplicant to initialize
#define EAP_AUTH_IN_PROGRESS		-14
// Thread has started -- but EAP is not open (program error)
#define EAP_AUTH_NOT_OPEN			-15
// Definition?
#define EAP_REAUTH_WAIT				-16

#define CSCM_ERROR_NULL_POINTER     -1001

// simplified (not very good) 7-bit random number
#define myRand() ((u32Seed = (4321*u32Seed + 1)) & 0x7F) 

// CSCM States
#define STATE_OFF					0
#define STATE_ERROR					1
#define STATE_IDLE					2
#define STATE_LEAVING_IDLE			3
#define STATE_SYNC_DOWN_SENT		4
#define STATE_UNLOADED				6
#define STATE_LIBRARY_INIT			7
#define STATE_LIBRARY_INIT_ERROR	8
#define STATE_CARD_INIT				9
#define STATE_CARD_INIT_ERROR		10
#define STATE_CLOSED				11
#define STATE_WAITING_FOR_CARD      12
#define STATE_NET_WAIT_PROXY_CONN   13

// 14 to 15 reserved
#define STATE_NET_SEARCH_DONE		16
#define STATE_WAIT_PHY_SYNC_CMD		17
#define STATE_PHY_SYNC_IN_PROG		18
#define STATE_SYNC_UP_DONE			19
#define STATE_WAIT_NET_ENTRY_CMD    20
#define STATE_NET_ENTRY_IN_PROG		21
#define STATE_NET_ENTRY_DONE		22
//#define STATE_NET_DEREG_DONE		23
#define STATE_SHUTDOWN				24
#define STATE_LEAVING_SHUTDOWN      25
#define STATE_SLEEP					26
#define STATE_HMC_SHUTDOWN			27
#define STATE_LEAVING_HMC_SHUTDOWN  28

#define BM_EVENT_NET_SEARCH_TIMEOUT		(1 << ID_EVENT_NET_SEARCH_TIMEOUT)
#define BM_EVENT_SYNC_UP_TIMEOUT		(1 << ID_EVENT_SYNC_UP_TIMEOUT)
#define BM_EVENT_SYNC_DOWN_TIMEOUT		(1 << ID_EVENT_SYNC_DOWN_TIMEOUT)
#define BM_EVENT_NET_ENTRY_TIMEOUT		(1 << ID_EVENT_NET_ENTRY_TIMEOUT)
#define BM_EVENT_NET_DEREG_TIMEOUT		(1 << ID_EVENT_NET_DEREG_TIMEOUT)
#define BM_EVENT_BASEBAND_STATUS_TIMEOUT (1 << ID_EVENT_BASEBAND_STATUS_TIMEOUT)
#define BM_EVENT_CHIPSET_RESET_TIMEOUT	(1 << ID_EVENT_CHIPSET_RESET_TIMEOUT)
#define BM_EVENT_FW_DOWNLOAD_TIMEOUT	(1 << ID_EVENT_FW_DOWNLOAD_TIMEOUT)
#define BM_EVENT_SHUTDOWN_TIMEOUT		(1 << ID_EVENT_SHUTDOWN_TIMEOUT)
#define BM_EVENT_NET_ENTRY_FAILURE		(1 << ID_EVENT_NET_ENTRY_FAILURE)
#define BM_EVENT_WIMAX_OPTIONS_TIMEOUT  (1 << ID_EVENT_WIMAX_OPTIONS_TIMEOUT)
#define BM_EVENT_EAP_ENC_PRIV_TIMEOUT   (1 << ID_EVENT_EAP_ENC_PRIV_TIMEOUT)

#define EAP_SUPPLICANT_START_TIMEOUT_MS 10000
#define EAP_SUPPLICANT_START_SLEEP_MS      50

#define EAP_POLLING_MIN_INTERVAL_MS 10
#define EAP_POLLING_MAX_INTERVAL_MS 1000

// Reauthentication states
#define STATE_REAUTH_OFF            0
#define STATE_REAUTH_WAITING        1
#define STATE_REAUTH_AUTHENTICATING 2
#define STATE_REAUTH_DONE           3

// CSCM debug level
#define DIAG_OFF		0
#define DIAG_ERROR		1
#define DIAG_NORMAL		2
#define DIAG_VERBOSE	3
#define DIAG_DEBUG      4

//#define MAX_NUMBER_OF_SERVICE_FLOWS 16
#define MAX_NUMBER_OF_SERVICE_FLOWS 0

// Shorthand macros for printing text to print contexts
#define  PrintDiag     BeceemAPI_PrintDiag
#define  PrintStats    BeceemAPI_PrintStats
#define  PrintVersions BeceemAPI_PrintVersions
#define  PrintConStat  BeceemAPI_PrintConStat

struct stBinFileData {
	UINT32  u32Length; // File length in bytes
	UINT8 * pu8Buffer; // Statically allocated (with malloc) buffer
	}; 

typedef struct {
	UINT32 u32ID;
	BOOL   bActive;
	UINT32 u32TimeoutMs;
	UINT32 u32WaitingMs;
	UINT32 u32EventCount;
	} stTimeout_t;

// =========================================================================================
// Main Beceem WiMAX interface class
// =========================================================================================

typedef enum {ChipsetIdle, 
			  ChipsetResetInProgress,
			  ChipsetResetSuccess,
			  ChipsetConfigFileDownloadInProgress,
			  ChipsetConfigFileDownloadSuccess,
			  ChipsetConfigFileDownloadFailure,
			  ChipsetFirmwareDownloadInProgress, 
			  ChipsetFirmwareDownloadSuccess, 
			  ChipsetFirmwareDownloadFailure} tChipsetControlStatus;

typedef enum {NsIdle, 
    NsSyncDown1, NsSyncDown1_Wait_1, NsSyncDown1_Wait_2, 
	NsSyncDown2, NsSyncDown2_Wait_1, NsSyncDown2_Wait_2, 
	NsSearching, NsAborted, NsDone} tNsState;

typedef enum {IpIdle, IpRefreshRequested, IpSfAddUL, IpSfAddDL} tIpRefreshState;

typedef struct {

	// -------------------------------------------------------------------

	PrintContext_t hPrintConStat;    // Print context handle for connection status
	PrintContext_t hPrintStats;	  // Print context handle for link statistics
	PrintContext_t hPrintInfo;		  // Print contect handle for connection manager event counts
	PrintContext_t hPrintSearch;	  // Print context handle for net search results
	PrintContext_t hPrintVersions;   // Print context handle for versions
	PrintContext_t hPrintDiag;	      // Print context handle for CSCM debug log
	PrintContext_t hPrintEAPConfig;  // Print context handle for EAP configuration string
	PrintContext_t hPrintMisc;		  // Print context handle for miscellaneous uses (such as option get / set)

#ifdef WIN32
	HINSTANCE hDLL;		    // Handle to DLL
#endif

	void * hDevice;		    // Handle to Beceem WiMAX device

	BOOL bDebugMode;		// Debug mode operation

    UINT32 u32Seed;			// "Random" number generator seed (not a real RNG!)
	UINT32 u32StatisticsInterval;  // Interval for periodic statistics reports

	// Request wait indicators
	tNsState eNetworkSearchState;

	// Timeout elapsed time counter (in ms)
	BOOL   bEnableTimeouts;       // Global timeout enable

	// ----------------------------------------
	// Timeout data
	stTimeout_t astEventData[ID_EVENT_INVALID];

	// Mutexese and Events
	WcmMutex_t hMutexSendWiMAXMessage;		// Serializes access to SendWiMAXMessage()
	WcmMutex_t hMutexSocketSend;			// Serializes access to SocketSend() (data transfers to client)
	WcmMutex_t hMutexStatusResponse;		// Locks API during status response indications
	WcmMutex_t hMutexClientLock;			// Serializes access to API lock Boolean flag & counter
	WcmMutex_t hMutexEAPSupplicant;			// Serializes access to EAP supplicant open/init/close functions
	WcmMutex_t hMutexEAPTraffic;			// Serializes access to EAP supplicant data transfer functions

	// Watchdog timer for MutexClientLock
	BOOL   bClientMutexLocked;
	UINT32 u32ClientMutexLockedMs;
	UINT32 u32ClientMutexLockedTimeoutMs;

	// Options
	hOptionTable_t hOptionTable;

	// CSCM Debug logging
	UINT32 u32CSCMDebugLogLevel;	// Debug log level

	// MAC management message logging 
	BOOL bMACLoggingEnabled;		// Is MAC message logging enabled
	hLogFile_t hMACLogFile;			// MAC log file stream (= NULL if closed)
	STRING bszMACLogFileName;		// MAC management message log file name

	// Baseband statistics counters, offset values, and report definition
	hBBStatisticsCounters_t hBBStatsOffset; 
	hLinkStatsData_t hLinkStatsReport;

	// Network search parameter pointer
	hNetSearchParameters_t hNetSearchParam;

	// MIB list for Statistics Report
	hMibData_t hMibData;

	// IP Refresh variables
	tIpRefreshState eIPRefreshState;
	BOOL bNetEntryIPRefresh;
	BOOL bTerminateDHCPClient;

	// Link status
	UINT32 u32LastLinkStatus;

	// -------------------------------------------------------------------
	// Current EAP parameters				
	BOOL		 bRADIUSIsOpen;		
	BOOL		 bInnerNAIChange;
	B_CHAR		 bszCurrentIdentity[128];
	B_CHAR		 bszCurrentPassword[128];
	STRING		 bszCurrentRADIUSIPAddress;
	UINT32		 u32CurrentRADIUSPort;
	STRING		 bszCurrentRADIUSSecret;
	UINT32		 u32EAPMethod;
	STRING		 bszCACertFilename;
	STRING		 bszTTLSAnonymousIdentity;
	STRING		 bszTLSDeviceCertFilename;
	STRING		 bszTLSDevicePrivateKeyFilename;
	STRING		 bszTLSDevicePrivateKeyPassword;
	STRING		 bszPrivateKeyPasswordFormat;
	BOOL		 bThreadAuthenticationExists;
	WcmThread_t	 threadAuthentication;
	BOOL         bProcessMesssagesThreadRunning;
	UINT32       u32ProcessMesssagesThreadID;
	WcmThread_t  threadProcessMessages;

	stBSInformation_t stConnectBS;		// Base station used for Connect command, when user manually specifies
										// frequency and bandwidth
	STRING	bszBaseStationID;			// Base station MAC address
	BOOL	bIsNetworkSearchDone;		// Are we done with network search?
	BOOL	bInvertMSKByteOrder;		// Invert the MSK byte order after the supplicant exits?
	BOOL	bIgnoreWaitNetEntryCmd;		// Are we ignoring WAIT_NET_ENTRY_CMD indications?
	BOOL	bSkipNetSearch;				// Are we skipping network search
	BOOL	bAuthEnabled;				// Authentication enabled?
	BOOL	bAuthEthernetToRADIUS;		// Use direct Ethernet connection to RADIUS server
	INT32	i32AuthenticationState;		// Return code from authentication function
	INT32	i32ReauthenticationState;	// Reauthentication status
	UINT32	u32AuthStartCount;			// Number of times the authentication thread was started
	UINT32	u32NetEntryCompletedCount;	// Number of times network entry succeeded
	UINT32	u32NetEntryDeregCount;		// Number of DREG received during network entry
	UINT32	u32NetEntryPKMCount;		// Number of PKM failures during network entry
	UINT32	u32NetEntrySATEKCount;		// Number of SATEK failures during network entry
	UINT32	u32NetEntryRegCount;		// Number of registration failures during network entry
	UINT32	u32OtherDeregRcvdCount;		// Number of other DREG received (not during net entry)

	// Read-only properties

	// Current RSSI and CINR
	INT32 s32RSSI;  // Received signal strength indication
	INT16 s16CINR;  // Carrier to interference and noise ratio

	// ----------------------------------------------
	// Firmware download status
	tChipsetControlStatus eChipsetControlStatus;

	// ---------------------------------------------
	// Library, driver and firmware versions

#define FWVERSION_LEN  64
#define DLLVERSION_LEN 64
#define DRVVERSION_LEN 64
	B_CHAR bszActualDLLVersion[DLLVERSION_LEN];

	// Valid library and firmware version
	BOOL bValidDLLVersion;
	BOOL bValidFWVersion;

	// Driver version as reported by SS Info response
	B_CHAR bszActualDriverVersion[DRVVERSION_LEN];

	// Firwmare version as reported by SS Info response
	B_CHAR bszActualFWVersion[FWVERSION_LEN];

	// ----------------------------------------
	//Network search method
	BOOL bUseCAPLForNetworkSearch;
	// Results returned by network search
#define MAX_NET_SEARCH_RESULTS 50
	UINT32 u32BSInfoCount;
	hBSInformation_t hSelectedBS;
	hBSInformation_t ahBSInfo[MAX_NET_SEARCH_RESULTS];
	
	// -----------------------------------------
	// Various status indicators

	// Current CSCM connection state
	UINT32 u32State;

    // -----------------------------------------
	// EAP Authentication

	BOOL bEAPIsOpen;
	BOOL bAuthExchangeActive;  // Is authentcation exchange active?
	UINT32 au32SessionKeyLength; 
	UINT8  au8MasterSessionKey[MSK_LEN+EMSK_LEN];
	UINT8  au8PrivKeyAESEncryptionKey[PRIV_KEY_AES_KEY_LEN];
	UINT8  *au8EncMsgDigest;
	UINT32 u32EncMsgDigestLength;

	WcmEvent_t hEventRSAPrivEncryptResponse;
	B_CHAR  *au8SecCredsData;
	UINT32 u32SecCredsLength;

	// -------------------------------------------------------------------
	// Option entry pointers (for quick access)


	// Network search 
	stOptionEntry_t *pOptArrayBandwidthMHz;
	stOptionEntry_t *pOptScanMinFrequencyMHz;
	stOptionEntry_t *pOptScanMaxFrequencyMHz;
	stOptionEntry_t *pOptScanStepFrequencyMHz;
	stOptionEntry_t *pOptArrayCenterFrequencyMHz;
	stOptionEntry_t *pOptNetworkSearchTimeoutSec;
	stOptionEntry_t *pOptLPSearchInShutDownEnabled;
	// Network entry
	stOptionEntry_t *pOptNetworkEntryTimeoutSec;
	stOptionEntry_t *pOptNEToHighestCINRBaseStation;

	// Authentication
	stOptionEntry_t *pOptAuthEnabled;
	stOptionEntry_t *pOptEAPMethod;
	stOptionEntry_t *pOptIdentity;
	stOptionEntry_t *pOptPassword;
	stOptionEntry_t *pOptTTLSAnonymousIdentity;
	stOptionEntry_t *pOptValidateServerCert;
	stOptionEntry_t *pOptCACertPath; 
	stOptionEntry_t *pOptCACertFileName;
	stOptionEntry_t *pOptTLSDeviceCertFileName;
	stOptionEntry_t *pOptTLSDeviceSubCA1CertFileName;
	stOptionEntry_t *pOptTLSDeviceSubCA2CertFileName;
	stOptionEntry_t *pOptTLSDevicePrivateKeyFileName;
	stOptionEntry_t *pOptTLSDevicePrivateKeyPassword;
	stOptionEntry_t *pOptPrivateKeyPasswordFormat;
	stOptionEntry_t *pOptAuthenticationTimeoutSec;
	stOptionEntry_t *pOptInvertMSKByteOrder;
	stOptionEntry_t *pOptAlwaysIncludeTLSLength;
	stOptionEntry_t *pOptEAPFragmentMaxLength;
	stOptionEntry_t *pOptEAPPollingLoopIntervalMs;
	// Beceem cryptographic engine
	stOptionEntry_t *pOptFirmwareRSAPrivateKeyEncrypt;
	stOptionEntry_t *pOptFirmwarePrivateKeyBits;
	stOptionEntry_t *pOptInnerNAIChange;
	stOptionEntry_t *pOptBeceemEngineFileName;

	// Authentication test mode (over Ethernet)
	stOptionEntry_t *pOptAuthEthernetToRADIUS;	
	stOptionEntry_t *pOptRADIUSIPAddress;
	stOptionEntry_t *pOptRADIUSPort;
	stOptionEntry_t *pOptRADIUSSecret;

	// Automatic connect and/or disconnect
	stOptionEntry_t *pOptAutoReConnectEnabled;	
	stOptionEntry_t *pOptAutoReDisconnectEnabled;
	stOptionEntry_t *pOptSkipNetSearch;
	stOptionEntry_t *pOptAutoReConnectIntervalSec;
	stOptionEntry_t *pOptAutoReDisconnectIntervalSec;

	// Link status request period
	stOptionEntry_t *pOptLinkStatusRequestPeriodSec;

#ifdef LINUX
	// IP refresh (Linux only)
	stOptionEntry_t *pOptIPRefreshCommand;
	stOptionEntry_t *pOptNetEntryIPRefreshEnabled;
	stOptionEntry_t *pOptTerminateDHCPClient;
#endif

	// Firmware and config files
	stOptionEntry_t *pOptFirmwareFileName;
	stOptionEntry_t *pOptConfigFileName;

	// Log file control
	stOptionEntry_t *pOptCSCMDebugLogLevel;
	stOptionEntry_t *pOptCSCMDebugLogFileName;
	stOptionEntry_t *pOptCSCMDebugLogFileMaxSizeMB; 
	stOptionEntry_t *pOptAuthLogLevel;
	stOptionEntry_t *pOptAuthLogFileName;
	stOptionEntry_t *pOptAuthLogFileMaxSizeMB;
	stOptionEntry_t *pOptAuthEnableSysLogPrints;
	stOptionEntry_t *pOptEngineLoggingEnabled;
	stOptionEntry_t *pOptEngineLogFileName;
	stOptionEntry_t *pOptEngineLogFileMaxSizeMB;
	stOptionEntry_t *pOptRADIUSClientLogLevel;

	// Actual config file used (NULL = default values)
	STRING bszActConfFilePath;

	// ---------------------------------------------------------------------

	// Message Queue
	hQueueEntry_t pQueueStart;
    hQueueEntry_t pQueueEnd;
    HANDLE hQueueMutex;
    HANDLE hQueueSemaphore;

	// Client communication socket
	WcmSocket_t socClient;

	// Date-time buffer
	B_CHAR bszDateTime[64];

	// Elapsed time variables
	UINT32 u32TotalSeconds;
	time_t timeServerStartSeconds;
	UINT32 u32ConnStartSeconds;

	// Link state buffer
	B_CHAR bszLinkStatus[128];

	// String function buffer pool
	int iStringNxtBufBchar;
#define MAX_STR_RESULT_SIZE 2048
    B_CHAR bszBufferPool[MAX_STR_RESULT_SIZE];

	} stBeceemAPI_t;

typedef stBeceemAPI_t *BeceemAPI_t;

// -------------------------------------------------------------------
// Structure initialization and de-initialization
// Note: The structure handle is a global variable hAPI
BeceemAPI_t BeceemAPI_Create(STRING bszConfFilePath, BOOL bDebugMode);
void BeceemAPI_Destroy(BeceemAPI_t hAPI);

// -------------------------------------------------------------------
// Read config file for CSCM over remote proxy
BOOL BeceemAPI_ReadProxyConfigFile(PSTPROXYPARAMETERS pstProxyParameters);

// -------------------------------------------------------------------
// Private request functions
void BeceemAPI_EAPCompleteRequest(BeceemAPI_t hAPI, UINT32 u32Status, UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len);

void BeceemAPI_SetState(BeceemAPI_t hAPI, UINT32 u32NewState);
void BeceemAPI_SetStateEx(BeceemAPI_t hAPI, UINT32 u32NewState, STRING bszMessage);
void BeceemAPI_SetStatusStrip(BeceemAPI_t hAPI, UINT32 u32Command, STRING bszLabel);
BOOL BeceemAPI_IsConnectedState(BeceemAPI_t hAPI);
//delegate void BeceemAPI_StatusStripCommandCallback(BeceemAPI_t hAPI, UINT32 u32Command, STRING bszAddText);

// DHCP Refresh
void BeceemAPI_RefreshIPAddress(BeceemAPI_t hAPI);

// ------------------------------------------------------------------
// Response status display functions
STRING BeceemAPI_DecodeState(BeceemAPI_t hAPI);
STRING BeceemAPI_DecodeLinkStatus(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize);
STRING BeceemAPI_DecodeDeviceStatus(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize);
STRING BeceemAPI_DecodeConnectionProgress(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize);
STRING BeceemAPI_DecodeLinkStatusReason(INT32 i32Status, STRING bszBuffer, UINT32 u32BcharSize);
void   BeceemAPI_DisplayBasebandStatus(BeceemAPI_t hAPI, BOOL bConnected, 
								       STRING bszStatusLabel, STRING bszStatusValue1, STRING bszStatusValue2);
void   BeceemAPI_DisplayBaseInfo(BeceemAPI_t hAPI, PrintContext_t hPrint, PBSINFOEX pBaseInfo);

// -------------------------------------------------------------------
// Utility functions
void BeceemAPI_ProgramError(BeceemAPI_t hAPI, STRING ErrMsg, STRING sFile, UINT32 u32Line);

void BeceemAPI_SetTimeout (BeceemAPI_t hAPI, UINT32 u32EventID, UINT32 u32TimeoutMs);
BOOL BeceemAPI_CheckTimeout(BeceemAPI_t hAPI, UINT32 u32EventID);
void BeceemAPI_Activate   (BeceemAPI_t hAPI, UINT32 u32EventID);
void BeceemAPI_Deactivate (BeceemAPI_t hAPI, UINT32 u32EventID);
BOOL BeceemAPI_IsActivated(BeceemAPI_t hAPI, UINT32 u32EventID);

void BeceemAPI_DeactivateCounters(BeceemAPI_t hAPI);

// Functions defined in BeceemWiMAXEAP.cpp 
void BeceemAPI_EAP_FromCSCMToSupplicant (BeceemAPI_t hAPI, const UINT8 *EAP_msg, size_t EAP_msg_len);
void BeceemAPI_EAP_FromBaseStationToSupplicant (BeceemAPI_t hAPI, const UINT8 *msg, size_t msg_len);

#ifdef WIN32

// ------------------------------ START libxvi010.dll Interface --------------------------
// OpenWiMAXDevice ...
typedef void * (__cdecl * LPFNDLL1) (void);
// CloseWiMAXDevice ...
typedef VOID   (__cdecl * LPFNDLL3) (void *pvHandle);
// RegisterCallBack ...
typedef UINT32 (__cdecl * LPFNDLL4) (VOID *pvHandle, 
									 void *pfnCallbackFn,
									 VOID *pvCallbackContext);
// BeceemAPI_SendWiMAXMessage ...
typedef UINT32 (__cdecl * LPFNDLL5) (VOID *pvHandle, 
									 UINT8 *pu8SendBuffer, 
									 UINT32 u32Length);
// Note rdm() and wrm() are not used in this application

LPFNDLL1 lpfnDllOpenWiMAXDevice;    // Pointer to: void * OpenWiMAXDevice(BeceemAPI_t hAPI);
LPFNDLL3 lpfnDllCloseWiMAXDevice;	// Pointer to: void CloseWiMAXDevice(void *pvHandle)
LPFNDLL4 lpfnDllRegisterCallback;   // Pointer to: UINT32 RegisterCallback (void *pvHandle, 
									//									    void *pCallbackFnPtr,
									//										void *pvCallbackContext);

LPFNDLL5 lpfnDllSendWiMAXMessage;   // Pointer to: UINT32 BeceemAPI_SendWiMAXMessage (void *pvHandle,
									//										UINT8 *pu8SendBuffer,
									//										UINT32 u32Length);

// ------------------------------ END libxvi010.dll Interface --------------------------

#else

void * OpenWiMAXDevice (void);
void CloseWiMAXDevice (void *pvHandle);
UINT32 RegisterCallback (void *pvHandle, 
						 void *pfnCallbackFn,
						 void *pvCallbackContext);
UINT32 SendWiMAXMessage (VOID *pvHandle, 
						 UINT8 *pu8SendBuffer, 
						 UINT32 u32Length);

#endif

UINT32 BeceemAPI_OpenDevice (BeceemAPI_t hAPI);		 // Open WiMAX device
void BeceemAPI_CloseDevice (BeceemAPI_t hAPI);		 // Close WiMAX device

CSCM_THREAD_FUNCTION BeceemAPI_ProcessMessages (void *pvThreadParam);	 // Message processor - runs in its own thread
void BeceemAPI_ResetStatistics (BeceemAPI_t hAPI, BOOL abUseRpt[]);		 // Reset the baseband statistics counter displays

// ---------------------------------------
// Connection request functions
BOOL BeceemAPI_InitNetworkSearch (BeceemAPI_t hAPI);				// Prepare for a network search operation
BOOL BeceemAPI_AbortNetworkSearch (BeceemAPI_t hAPI);			    // Abort a network search
void BeceemAPI_ResetNetworkSearch (BeceemAPI_t hAPI);				// Resets the network search state
BOOL BeceemAPI_NetworkSearchRequest (BeceemAPI_t hAPI);				// network search for base stations
void BeceemAPI_SyncUpRequest (BeceemAPI_t hAPI);					// PHY sync 
void BeceemAPI_SyncDownRequest (BeceemAPI_t hAPI);					// Sync down
INT32 BeceemAPI_NetworkEntryRequest (BeceemAPI_t hAPI);				// Network entry
void BeceemAPI_NetworkDeregRequest (BeceemAPI_t hAPI);				// Network exit
void BeceemAPI_EAPTransferRequest(BeceemAPI_t hAPI, const UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len);
#ifdef SUBTYPE_EAP_SPOOF_INFO_REQ
void BeceemAPI_EAPNaiSpoofInfoRequest(BeceemAPI_t hAPI, const UINT8 *u8EAP_msg, UINT32 u32EAP_msg_len);
#endif
void BeceemAPI_AbortAuth(BeceemAPI_t hAPI, BOOL bSendEAPComplete);
void BeceemAPI_SetWiMAXOptionsRequest (BeceemAPI_t hAPI, UINT32 u32OptionSpecifier, UINT32 u32Payload);
void BeceemAPI_EAPEncryptPrivateRequest (BeceemAPI_t hAPI, 	
			UINT32 u32Padding, const UINT8 u8AESKey[32], const UINT8 u8Data[], UINT32 u32DataLen);
void BeceemAPI_Disconnect(BeceemAPI_t hAPI);

void BeceemAPI_ClearBSInformation (BeceemAPI_t hAPI);
void BeceemAPI_EAP_GetSecCredsRequest(BeceemAPI_t hAPI, UINT32 uSecParamId); 

// DHCP client termination
#ifdef LINUX
void BeceemAPI_IPRefreshKillProcess(BeceemAPI_t hAPI);
#endif

// ---------------------------------------
// Information request functions
void BeceemAPI_BasebandStatusRequest (BeceemAPI_t hAPI);
void BeceemAPI_IdleModeRequest(BeceemAPI_t hAPI, BOOL bEnterIdleMode);
void BeceemAPI_IdleModeStatusRequest(BeceemAPI_t hAPI);
void BeceemAPI_LinkStatusRequest(BeceemAPI_t hAPI);
void BeceemAPI_ShutdownStatusRequest(BeceemAPI_t hAPI);
void BeceemAPI_HalfMiniCardStatusRequest(BeceemAPI_t hAPI);
void BeceemAPI_GetSSInfoRequest(BeceemAPI_t hAPI);
void BeceemAPI_GetStatisticsMIBsRequest(BeceemAPI_t hAPI, BOOL abUseRpt[]);
void BeceemAPI_MACManagementMessageLoggingRequest(BeceemAPI_t hAPI, BOOL bStart);

// ---------------------------------------
// Option functions
void BeceemAPI_GetOption(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs);
void BeceemAPI_SetOption(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs);
void BeceemAPI_OptionsCommand(BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs);
void BeceemAPI_SetConfFilePath(BeceemAPI_t hAPI, STRING bszConfFilePath);
void BeceemAPI_UpdateOptions(BeceemAPI_t hAPI);
void BeceemAPI_SetOptionsTable(BeceemAPI_t hAPI, hOptionTable_t hTable);
BOOL BeceemAPI_CheckOptionValues(BeceemAPI_t hAPI);

// ---------------------------------------
// Timeout Check
BOOL BeceemAPI_DisableTimeouts(BeceemAPI_t hAPI);
BOOL BeceemAPI_EnableTimeouts(BeceemAPI_t hAPI);
UINT32 BeceemAPI_GetResponseStatus(BeceemAPI_t hAPI);
void BeceemAPI_InitBasebandStatusMsgWrite(BeceemAPI_t hAPI);
void BeceemAPI_InitBasebandStatusMsgClear(BeceemAPI_t hAPI);

// ---------------------------------------
// Baseband chip operations
void BeceemAPI_ChipsetResetRequest(BeceemAPI_t hAPI);
void BeceemAPI_FirmwareDownloadRequest(BeceemAPI_t hAPI, UINT32 u32ImageType, STRING bszFilePath); 

// ---------------------------------------
// Shutdown request
void BeceemAPI_ShutdownRequest(BeceemAPI_t hAPI);
BOOL BeceemAPI_ShutDownWithLPSM(BeceemAPI_t hAPI);
BOOL BeceemAPI_ConfigLPSMParamsRequest(BeceemAPI_t hAPI);

// ---------------------------------------
// Functions defined in BeceemWiMAXEAP.cpp 
typedef struct {
	UINT32 u32EAPMethod;
	STRING bszCACertPath; 
	STRING bszCACertFileName;
	STRING bszTTLSAnonymousIdentity;
	STRING bszTLSDeviceCertFileName;
	STRING bszTLSDeviceSubCA1CertFileName;		   
	STRING bszTLSDeviceSubCA2CertFileName;
	STRING bszTLSDevicePrivateKeyFileName;
	STRING bszTLSDevicePrivateKeyPassword;
	STRING bszPrivateKeyPasswordFormat;
	BOOL   bAuthEthernetOverRADIUS;
	STRING bszRADIUSIPAddress;
	UINT32 u32RADIUSPort;
	STRING bszRADIUSSecret;
	UINT32 u32EloopPollingIntervalMs;
	UINT32 u32AuthenticationTimeoutSec;
	UINT32 u32AuthLogLevel;
	UINT32 u32RADIUSClientLogLevel;
	STRING bszAuthLogFileName;
	BOOL   bAlwaysIncludeTLSLength;
	UINT32 u32EAPFragmentMaxLength;
	BOOL   bFirmwareRSAPrivateKeyEncrypt;
	UINT32 u32FirmwarePrivateKeyBits;
	STRING bszBeceemEngineFileName;
	BOOL   bEngineLoggingEnabled;
	STRING bszEngineLogFileName;
	BOOL   bValidateServerCert;
	BOOL   bEnableAuthSysLogPrints;
	} ST_EAP_INIT_PARAMS;

STRING BeceemAPI_DecodeAuthState (INT32 i32AuthState);
INT32 BeceemAPI_EAP_Init (
    BeceemAPI_t hAPI,
	ST_EAP_INIT_PARAMS *pstEAPInitParams
);
void BeceemAPI_EAP_Login (BeceemAPI_t hAPI, STRING bszIdentity, STRING bszPassword);
CSCM_THREAD_FUNCTION BeceemAPI_EAP_Authenticate(void *pvThreadParam);
void BeceemAPI_EAP_Complete(BeceemAPI_t hAPI);
void BeceemAPI_EAP_Close(BeceemAPI_t hAPI);
BOOL BeceemAPI_EAP_StartThread(BeceemAPI_t hAPI);
BOOL BeceemAPI_EAP_IsEAPApplicationData(const UINT8 *au8EAPMessage, UINT32 u32MsgLength);
BOOL BeceemAPI_EAP_BuildNaiSpoofInfoRequestPayload(BeceemAPI_t hAPI,
	UINT8 *au8Payload, UINT32 u32PayloadMaxSize, UINT16 *pu16PayloadLength, 
	const UINT8 *au8EAPMessage, UINT32 u32EAPMsgLength);
BOOL BeceemAPI_EnqueueEAPRequestIdentityPacket(BeceemAPI_t hAPI);

// ---------------------------------------
// Connection state error reporting function
void BeceemAPI_ShowTimeoutMessage(BeceemAPI_t hAPI, UINT32 u32ID, STRING bszMessage, STRING bszExplanation);
void BeceemAPI_ConnectionError(BeceemAPI_t hAPI, STRING bszMsg);		// Connection error

BOOL BeceemAPI_Authorizing(BeceemAPI_t hAPI);

// Returns TRUE if the BeceemWiMAX DLL is loaded; otherwise, FALSE
BOOL BeceemAPI_IsLoaded(BeceemAPI_t hAPI); 

// Returns TRUE if the WiMAX device is open; otherwise, FALSE
BOOL BeceemAPI_IsOpen(BeceemAPI_t hAPI); 

void BeceemAPI_ClearChipsetControlStatus(BeceemAPI_t hAPI);

void BeceemAPI_PrintDiag(UINT32 u32Level, BeceemAPI_t hAPI, STRING bszFormat, ...);
void BeceemAPI_PrintDiag_ArgPtr(UINT32 u32Level, BeceemAPI_t hAPI, STRING bszFormat, va_list args);
void BeceemAPI_PrintStats(BeceemAPI_t hAPI, STRING bszFormat, ...);
void BeceemAPI_PrintVersions(BeceemAPI_t hAPI, STRING bszFormat, ...);
void BeceemAPI_PrintConStat(BeceemAPI_t hAPI, STRING bszFormat, ...);

// Client lock
BOOL BeceemAPI_Lock(BeceemAPI_t hAPI, WcmSocket_t hSocket, BOOL *pbError);
BOOL BeceemAPI_Unlock(BeceemAPI_t hAPI, BOOL *pbError);

// Server socket printing
void ServerSocketWPrintf(BeceemAPI_t hAPI, STRING bszFormat, ...);
void ServerSocketWPuts(BeceemAPI_t hAPI, STRING bszText);
void ServerSocketResponse(BeceemAPI_t hAPI, UINT32 u32CmdID, STRING bszMessage, BOOL bNewLine);
void ServerSocketPrompt(BeceemAPI_t hAPI);
void ServerSocketBeginPrint(PrintContext_t hPrint);
void ServerSocketEndPrint(BeceemAPI_t hAPI, PrintContext_t hPrint, UINT32 u32ID);

// String functions
STRING StringRepChar(BeceemAPI_t hAPI, B_CHAR c, UINT32 u32Length);
STRING StringPadLeft(BeceemAPI_t hAPI, const STRING ws, int iSize, B_CHAR c);
STRING StringSubstring(BeceemAPI_t hAPI, const STRING ws, int iStart, int iLen);
STRING StringTrim(BeceemAPI_t hAPI, const STRING ws);
STRING StringToScaledFixedPoint(BeceemAPI_t hAPI, const STRING ws, int iRightOfDecimal);
void   StringHexDump(UINT32 u32DebugLevel, BeceemAPI_t hAPI, BOOL bPrintAddress, 
					 const UINT8 *au8Buffer, UINT32 u32ByteLength);

STRING BeceemAPI_DateTime(BeceemAPI_t hAPI);

#define STRING_SUBSTR_TO_END   -1

#endif
