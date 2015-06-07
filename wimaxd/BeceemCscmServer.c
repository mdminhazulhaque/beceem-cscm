/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "BeceemAPI.h"
#include "BeceemCscmServer.h"
#include "BeceemCscmTop.h"
#include "BeceemCscmCommon.h"
#include "Options.h"
#include "PrintContext.h"
#include "SysLogFunctions.h"
#include "Utility.h"
#include "Version.h"

#ifdef LINUX_OR_OSX
#include <unistd.h>
#include <time.h>
#endif

#define WARGV_LEN 10
#define WINDOWS_SERVICE_NAME		S("BeceemCSCMServer")

extern stCmdInformation_t *g_astCmdInfo;

static struct {
	STRING bszListenAddress;
	UINT32 u32ListenPort;
	STRING bszConfFilePath;
	UINT32 u32DebugLevel;
	BOOL   bDebugMode;
	} g_stCmdArgValues;

typedef struct {
	WcmSocket_t socClient;
	BeceemCSCM_t hCSCM;
	BeceemAPI_t hAPI;
	WcmMutex_t  mutexBeceemAPI;
	STRING      bszConfFilePath;
	} BeceemWcmConnArgs_t;

static CSCM_THREAD_FUNCTION BeceemWcmConnection (void *pvThreadParam);
static BOOL g_bClientTerminate = FALSE; 
static BOOL g_bSignalTerminate = FALSE;
static BOOL g_bTerminateCalled = FALSE;

// Current used
static void CSCM_DownloadFirmwareFile_Command(BeceemCSCM_t hCSCM);
static void CSCM_EventCounts_Command(BeceemCSCM_t hCSCM);
static void CSCM_GetServerInfo_Command(BeceemCSCM_t hCSCM);
static void CSCM_Help_Command(BeceemCSCM_t hCSCM);
static void CSCM_ShutDown_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs) ;
static void CSCM_SyncDown_Command(BeceemCSCM_t hCSCM);
static void CSCM_SyncUp_Command(BeceemCSCM_t hCSCM);

static void CSCM_DownloadConfigFile_Command(BeceemCSCM_t hCSCM);
static void CSCM_ResetBasebandChip_Command(BeceemCSCM_t hCSCM);
static void CSCM_GetEMSK_Command(BeceemCSCM_t hCSCM);
static void CSCM_IdleMode_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs);

static BOOL NotIdleOrShutdown(BeceemAPI_t hAPI, UINT32 u32MsgID, STRING bszType);

#define server_wprintf b_printf

static int    RunServer(void);
static void   TerminateServer(BeceemCSCM_t hCSCM, STRING bszExitType);
static void   SignalHandler(int signal);
static STRING ProcessClientCommand(BeceemAPI_t hAPI, BeceemWcmConnArgs_t *pstArgs, 
				UINT32 u32ID, STRING abszArgStrings[], UINT32 u32NumberOfArgs, UINT32 *pu32Status);

static BOOL DecodeLinkStatsArgs (BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs, 
								 BOOL abUseRpt[]);
static BOOL ProcessArgs(int argc, STRING argv[], BOOL *pbQuit);
static void ServerUsage(void);
static void FatalError(STRING bszMessage);

#ifdef WIN32

#include <windows.h>
SERVICE_STATUS			g_ServiceStatus;
SERVICE_STATUS_HANDLE	g_hServiceStatus;
void ServiceControlHandler(DWORD dwRequest);
void ServiceMain(int argc, char **argv);

#endif

static BeceemCSCM_t g_hCSCM = NULL;

// ============================================================================================
// Client connection handling
// ============================================================================================

// --------------------------------------------------------------------------------
// CSCM_THREAD_FUNCTION BeceemWcmConnection (void *pvThreadParam)
//
// Description: Thread function for client connection handling.
//
// Input parameters:
//		pvThreadParam = Pointer to a stBeceemWcmConnArgs structure;
//
// Returns: status code
//
// --------------------------------------------------------------------------------

static CSCM_THREAD_FUNCTION BeceemWcmConnection (void *pvThreadParam)
	{

	BOOL bError = FALSE;
	UINT32 u32Status = CLIENT_NOP;
	UINT32 u32NumberOfArgs = 0;
	UINT8 * au8SendBuffer;
	UINT8 * au8RecvBuffer;
	STRING bszCommandLine;
	STRING abszArgStrings[MAX_NUMBER_OF_COMMAND_ARGS];
	UINT32 i32BytesReceived, u32BcharReceived, u32ID, u32PacketNumber = 0;
	UINT32 u32NxtCmdLnChr = 0, u32NxtRcvBufChr = 0, u32StrLen, u32CopyLen;
	UINT32 u32ErrorCode = 0;
	B_CHAR *pRcvStart, *pRcvEnd;
	BeceemWcmConnArgs_t stArgs, *pstThreadArgs;
	BeceemAPI_t hAPI;

	pstThreadArgs = (BeceemWcmConnArgs_t *) pvThreadParam;

	// Save a copy of the args & free the passed structure
	memcpy(&stArgs, pstThreadArgs, sizeof(stArgs));
	WcmFree(pstThreadArgs);

	hAPI = stArgs.hAPI; 

	PrintDiag(DIAG_NORMAL, hAPI, S("Socket is %08X\n"), stArgs.socClient);

	// Open the Beceem device
	// PrintDiag(DIAG_NORMAL, hAPI, S("Opening device\n"));
	//if (!BeceemAPI_IsOpen(hAPI))
	//	{
	//	PrintDiag(DIAG_NORMAL, hAPI, S("No device found - closing connection\n"));
	//	return WcmThread_Exit(1);
	//	}

	// Allocate the buffers
	if ((au8SendBuffer  = WcmMalloc(DEFAULT_SEND_BUFFER_SIZE)) == NULL ||
		(au8RecvBuffer  = WcmMalloc(DEFAULT_RECV_BUFFER_SIZE + sizeof(B_CHAR))) == NULL ||
		(bszCommandLine = WcmMalloc(BCHARS2BYTES(DEFAULT_CMD_LINE_CHAR_LEN))) == NULL)
		{	
		PrintDiag(DIAG_ERROR, hAPI, S("Can't allocate send & receive buffers\n"));
		return WcmThread_Exit(1);
		}

	if (!BeceemAPI_Lock(hAPI, stArgs.socClient, &bError) || bError)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("Socket %08X can't get Beceem API lock\n"), stArgs.socClient);
		return WcmThread_Exit(1);
		}
	ServerSocketWPrintf(hAPI, S(":%d:\nBeceem CM Server Version ") B_SFNL, ID_MSG_WELCOME, CSCM_PACKAGE_VERSION); 
	ServerSocketPrompt(hAPI);
	BeceemAPI_Unlock(hAPI, &bError);

	// Loop until BYE command is received
	u32NxtCmdLnChr = 0;
	while (u32Status == CLIENT_NOP && stArgs.socClient != CSCM_INVALID_SOCKET)
		{
		i32BytesReceived = WcmSocket_Receive(stArgs.socClient, au8RecvBuffer, DEFAULT_RECV_BUFFER_SIZE);

		// If server is terminating, exit immediately
		if (g_bTerminateCalled)
			{
			hAPI = NULL; // This prevents further diag printing here
			u32Status = OTHER_TERMINATE;
			break;
			}
		
		// Add a terminating B_NUL to stop wcscspn searches at the end of the received data
		au8RecvBuffer[i32BytesReceived]   = 0;
		au8RecvBuffer[i32BytesReceived+1] = 0;

		if (i32BytesReceived == CSCM_SOCKET_RECEIVE_ERROR || 
			i32BytesReceived > DEFAULT_RECV_BUFFER_SIZE)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Socket receive error.\n"));
			return WcmThread_Exit(1);
			}

		if (i32BytesReceived == 0)
			{
			PrintDiag(DIAG_NORMAL, hAPI, S("Connection closed by client.\n"));
			u32Status = CLIENT_QUIT;
			}

		u32PacketNumber++;

		// If server is terminating, exit immediately (2nd try)
		if (g_bTerminateCalled)
			{
			hAPI = NULL; // This prevents further diag printing here
			u32Status = OTHER_TERMINATE;
			break;
			}
		
		PrintDiag(DIAG_NORMAL, hAPI, S("Received %d bytes: \n"), i32BytesReceived);
		StringHexDump(DIAG_NORMAL, hAPI, TRUE, au8RecvBuffer, i32BytesReceived);

		// Get and process the next command line
		u32StrLen = 0;
		u32NxtRcvBufChr = 0;
		u32BcharReceived = BYTES2BCHARS(i32BytesReceived);
		while (u32NxtRcvBufChr < u32BcharReceived)
			{
			pRcvStart = (B_CHAR *) au8RecvBuffer + u32NxtRcvBufChr;
			u32StrLen = (UINT32) b_strcspn(pRcvStart, S(";\r\n\0")); // Look for the first ;, CR, LF, or NUL

			pRcvEnd = pRcvStart + u32StrLen;
			if (u32StrLen == 0)
				{
				u32NxtRcvBufChr++;      // Skip the terminator
				if (*pRcvEnd == C('\r'))  // Ignore any CR
					continue;
				}
			else
				{
				if (u32NxtCmdLnChr + u32StrLen >= DEFAULT_CMD_LINE_CHAR_LEN)
					u32CopyLen = DEFAULT_CMD_LINE_CHAR_LEN - u32NxtCmdLnChr - 1;
				else
					u32CopyLen = u32StrLen;
				if (u32CopyLen > 0)
					b_memcpy(bszCommandLine + u32NxtCmdLnChr, pRcvStart, u32CopyLen);
				u32NxtRcvBufChr += u32StrLen;
				u32NxtCmdLnChr  += u32CopyLen;
				}

			if (u32StrLen == 0) // True if we found a line terminator in this packet
				{
				if (u32NxtCmdLnChr >= DEFAULT_CMD_LINE_CHAR_LEN)
					u32NxtCmdLnChr = DEFAULT_CMD_LINE_CHAR_LEN - 1;
				bszCommandLine[u32NxtCmdLnChr] = B_NUL;
				u32NxtCmdLnChr = 0;
				u32ID = ParseCommandLine(bszCommandLine, abszArgStrings, MAX_NUMBER_OF_COMMAND_ARGS, &u32NumberOfArgs);
				ProcessClientCommand(hAPI, &stArgs, u32ID, &abszArgStrings[1], u32NumberOfArgs - 1, &u32Status);
				}
			}
		}

	// Close the socket
	if (WcmSocket_Close(stArgs.socClient, &u32ErrorCode) == CSCM_SOCKET_ERROR)
		{
#ifdef WIN32
#define NO_CLIENT_CODE 0x2749
#else
#define NO_CLIENT_CODE ENOTCONN //0x6B
#endif
		if (u32ErrorCode == NO_CLIENT_CODE)
			PrintDiag(DIAG_ERROR, hAPI, S("Client not found\n"));
		else
			PrintDiag(DIAG_ERROR, hAPI, S("Client socket close error = %08X\n"), u32ErrorCode);
		}
	WcmSocket_Destroy(stArgs.socClient);
	stArgs.socClient = CSCM_INVALID_SOCKET;

	// Free the buffers
	WcmFree(au8SendBuffer);
	WcmFree(au8RecvBuffer);
	WcmFree(bszCommandLine);

	// Terminate either the entire server process or just the connection.
	if (u32Status == CLIENT_TERMINATE)
		{
		g_bClientTerminate = 1;
		PrintDiag(DIAG_NORMAL, hAPI, S("Terminating server process.\n"));
		TerminateServer(stArgs.hCSCM, S("user termination"));
		return WcmThread_Exit(1); 
		}
	else
		{
		PrintDiag(DIAG_NORMAL, hAPI, S("Leaving BeceemWcmConnection thread\n"));
		return WcmThread_Exit(0);
		}

	} /* BeceemWcmConnection */

// -------------------------------------------------------------------------------------------
// Request chip reset
//
static void CSCM_ResetBasebandChip_Command(BeceemCSCM_t hCSCM) 
	{

	BeceemAPI_t hAPI;
	STRING bszMessage;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	// Dereg, if necessary
	if (hAPI->u32State == STATE_NET_ENTRY_DONE)
		BeceemAPI_Disconnect(hAPI);
	
	BeceemAPI_ChipsetResetRequest(hAPI);
	if (hAPI->eChipsetControlStatus == ChipsetIdle)
		bszMessage = S("Chip set reset request failed");
	else
		bszMessage = S("Baseband chip was reset");

	PrintDiag(DIAG_NORMAL, hAPI, bszMessage);
		
	} /* CSCM_ResetBasebandChip_Command */

// -------------------------------------------------------------------------------------------
// Download firmware file
//
static void CSCM_DownloadFirmwareFile_Command(BeceemCSCM_t hCSCM) {

	BeceemAPI_t hAPI;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

 	// Dereg, if necessary
	if (hAPI->u32State == STATE_NET_ENTRY_DONE)
		BeceemAPI_Disconnect(hAPI);
	
	CSCM_DownloadFirmwareOrConfigFile (hCSCM, IMAGE_FIRMWARE);

	}  /* CSCM_DownloadFirmware */

// -------------------------------------------------------------------------------------------
// Download config file
//
static void CSCM_DownloadConfigFile_Command(BeceemCSCM_t hCSCM) {

	BeceemAPI_t hAPI;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	// Dereg, if necessary
	if (hAPI->u32State == STATE_NET_ENTRY_DONE)
		BeceemAPI_Disconnect(hAPI);
	
	CSCM_DownloadFirmwareOrConfigFile (hCSCM, IMAGE_CONFIG_FILE);

	} /* CSCM_DownloadConfigFile_Command */

static void CSCM_EventCounts_Command(BeceemCSCM_t hCSCM)
	{

	BeceemAPI_t hAPI = NULL;
	PrintContext_t hPrint = NULL;
	INT32 i32OtherFailureCount = 0;

	#define EVENT_COUNT_FORMAT1 B_SF1(30) S(" %lu\n")
	#define EVENT_COUNT_FORMAT2 S("     ") B_SF1(25) S(" %lu\n")
	#define EVENT_COUNT_FORMAT3 S("          ") B_SF1(20) S(" %lu\n")

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	hPrint = hAPI->hPrintInfo;
	ServerSocketBeginPrint(hPrint);

	Print_Newline(hPrint);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT1, S("Connection attempts"), hCSCM->u32NumberOfConnectAttempts);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Conn OK"), hAPI->u32NetEntryCompletedCount);
	Print_Formatted(hPrint, S("     ") B_SF1(25) S(" %lu%%\n"), S("Percent OK"), 
		hCSCM->u32NumberOfConnectAttempts == 0 ? 0 :
		(100 * hAPI->u32NetEntryCompletedCount) / hCSCM->u32NumberOfConnectAttempts);

	Print_Newline(hPrint);
	Print_Formatted(hPrint, S("Authentication:\n"));
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Started"), hAPI->u32AuthStartCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Terminated"), 
		hAPI->u32AuthStartCount - hCSCM->u32NumberOfAuthCompleted);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Completed"), hCSCM->u32NumberOfAuthCompleted);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT3, S("Success"), hCSCM->u32NumberOfAuthOK);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT3, S("Timed out"), hCSCM->u32NumberOfAuthTimedOut);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT3, S("Other failure"), hCSCM->u32NumberOfAuthOtherFailure);

	Print_Formatted(hPrint, S("\nNetwork entry failure reasons:\n"));
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("DREG received"),
		hAPI->u32NetEntryDeregCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("PKM error"),
		hAPI->u32NetEntryPKMCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("SATEK error"),         
		hAPI->u32NetEntrySATEKCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Registration failed"),
		hAPI->astEventData[ID_EVENT_NET_ENTRY_FAILURE].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Network entry timeout"),
		hAPI->astEventData[ID_EVENT_NET_ENTRY_TIMEOUT].u32EventCount);
		
	i32OtherFailureCount = 
		  (INT32) hCSCM->u32NumberOfConnectAttempts - (INT32) hAPI->u32NetEntryCompletedCount
		- (INT32) hAPI->u32NetEntryDeregCount - (INT32) hAPI->u32NetEntryPKMCount - (INT32) hAPI->u32NetEntrySATEKCount
		- (INT32) hAPI->astEventData[ID_EVENT_NET_ENTRY_FAILURE].u32EventCount 
		- (INT32) hAPI->astEventData[ID_EVENT_NET_ENTRY_TIMEOUT].u32EventCount;
	if (i32OtherFailureCount < 0)
		i32OtherFailureCount = 0;
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Other"), (UINT32) i32OtherFailureCount);
		
	// Other DREG messages
	Print_Newline(hPrint);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT1, S("Other DREG received"), hAPI->u32OtherDeregRcvdCount);
	
	// Other timeout counts
	Print_Formatted(hPrint, S("\nOther Timeouts:\n"));
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Network search"),
		hAPI->astEventData[ID_EVENT_NET_SEARCH_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Sync up"),
		hAPI->astEventData[ID_EVENT_SYNC_UP_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Link status"),
		hAPI->astEventData[ID_EVENT_BASEBAND_STATUS_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Sync down"),
		hAPI->astEventData[ID_EVENT_SYNC_DOWN_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Deregistration"),
		hAPI->astEventData[ID_EVENT_NET_DEREG_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Chipset reset"),
		hAPI->astEventData[ID_EVENT_CHIPSET_RESET_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Firmware download"),
		hAPI->astEventData[ID_EVENT_FW_DOWNLOAD_TIMEOUT].u32EventCount);
	Print_Formatted(hPrint, EVENT_COUNT_FORMAT2, S("Shutdown"),
		hAPI->astEventData[ID_EVENT_SHUTDOWN_TIMEOUT].u32EventCount);

	ServerSocketEndPrint(hAPI, hPrint, ID_MSG_CM_STATS);

	} /* CSCM_EventCounts_Command */

// -------------------------------------------------------------------------------------------
// server
//
static void CSCM_GetServerInfo_Command(BeceemCSCM_t hCSCM)
	{

	BeceemAPI_t hAPI = NULL;
	B_CHAR bszLine[128];
	B_CHAR bszBuffer[512];

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	bszBuffer[0] = 0;

#ifdef ENABLE_WIDE_CHAR
#define WC_STATUS S("with")
#else
#define WC_STATUS S("without")
#endif

	StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
		S("Beceem CM Server ") B_SF S(" (Built ") B_NSF S(" ") B_NSF S(" ") B_SF S(" wchar_t support)\n"), 
		SERVER_VERSION, __DATE__, __TIME__, WC_STATUS);
	StrPrintf(bszLine, BCHARSOF(bszLine),
		S("Listening on IP address ") B_SF S(" port %d\n"), g_stCmdArgValues.bszListenAddress, g_stCmdArgValues.u32ListenPort);
	b_strncat(bszBuffer, bszLine, BCHARSOF(bszBuffer));

	if (hAPI->bszActConfFilePath == NULL || hAPI->bszActConfFilePath[0] == B_NUL)
		StrPrintf(bszLine, BCHARSOF(bszLine),
				S("Options initialized from internal default values\n"));
	else
		StrPrintf(bszLine, BCHARSOF(bszLine),
			S("Options initialized from file ") B_SFNL,
			hAPI->bszActConfFilePath);

	b_strncat(bszBuffer, bszLine, BCHARSOF(bszBuffer));

	StrPrintf(bszLine, BCHARSOF(bszLine),
		S("Running in ") B_SF S(" mode with server debug level %d\n"),
		g_stCmdArgValues.bDebugMode ? S("debug") : S("normal (daemon or service)"),
		g_stCmdArgValues.u32DebugLevel);

	b_strncat(bszBuffer, bszLine, BCHARSOF(bszBuffer));

	StrPrintf(bszLine, BCHARSOF(bszLine),
		S("Beceem WiMAX device ") B_SF S(" present\n"),
		BeceemAPI_IsOpen(hAPI) ? S("is") : S("is NOT"));

	b_strncat(bszBuffer, bszLine, BCHARSOF(bszBuffer));

	ServerSocketResponse(hAPI, ID_MSG_SERVER, bszBuffer, TRUE);

	} /* CSCM_GetServerInfo_Command */

// -------------------------------------------------------------------------------------------
// Shutdown
//

 static void CSCM_ShutDown_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs) 
 {
	BeceemAPI_t hAPI;
	STRING bszText;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	if (u32NumberOfArgs > 1)
		bszText = S("ERROR: Too many parameters for 'shutdown' command.\n");
	else if (u32NumberOfArgs == 0)
	{
		// Dereg, if necessary
		if (hAPI->u32State == STATE_NET_ENTRY_DONE)
			BeceemAPI_Disconnect(hAPI);
		
		BeceemAPI_ShutdownRequest(hAPI);
		bszText = S("shut down request sent.\n");
	}
	else
	{
		if (BcsCmpNoCase(abszArgStrings[0], S("lpsearch")))
		{
			if(hCSCM->bAutonomousScanParamsInitialized)
			{
				// Dereg, if necessary
				if (hAPI->u32State == STATE_NET_ENTRY_DONE)
					BeceemAPI_Disconnect(hAPI);
				BeceemAPI_ShutDownWithLPSM(hAPI);
				bszText = S("SUCCESS: Low Power Auto Search by Firmware started in shutdown mode.\n");
			}
			else
				bszText = S("ERROR: Low Power Auto Search by Firmware is Not Enabled/Configured.\n");
		}
		else 
			bszText = S("ERROR: 'lpsearch' required after 'syncdown'.\n");
	}
	 ServerSocketResponse(hAPI, ID_MSG_SHUT_DOWN, bszText, TRUE); 
	} /* CSCM_SyncDown_Command */

// -------------------------------------------------------------------------------------------
// Sync Down
//
 static void CSCM_SyncDown_Command(BeceemCSCM_t hCSCM) {

	BeceemAPI_t hAPI;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

    BeceemAPI_SyncDownRequest(hAPI);

	} /* CSCM_SyncDown_Command */


// -------------------------------------------------------------------------------------------
// Sync Up
//
static void CSCM_SyncUp_Command(BeceemCSCM_t hCSCM) {

	BeceemAPI_t hAPI;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

    BeceemAPI_SyncUpRequest(hAPI);

	} /* CSCM_SyncUp_Command */

static void CSCM_GetEMSK_Command(BeceemCSCM_t hCSCM)
{
	BeceemAPI_t hAPI;
	char bszMessage[256] = {0,};
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	if(hAPI->au32SessionKeyLength)
	{
		int i;
		char *pbuf;
		STRING bszSucc = S("SUCCESS:\t");
		int succLen = (int)strlen(bszSucc);
		memcpy(bszMessage, bszSucc, succLen);
		pbuf = &bszMessage[succLen];
		for (i = 0; i < EMSK_LEN; i++) 
		{
#if defined(LINUX_OR_OSX)		
			pbuf += sprintf(pbuf, "%02x", hAPI->au8MasterSessionKey[MSK_LEN+i]);
#else
			pbuf += _snprintf_s(pbuf, (255-(succLen+2*i)), _TRUNCATE, S("%02x"), hAPI->au8MasterSessionKey[MSK_LEN+i]);
#endif
		}
		
	}
	else
	{
		STRING bszFail = S("ERROR: EMSK not available");		
		memcpy(bszMessage, bszFail, strlen(bszFail));
	}
	ServerSocketResponse(hAPI, ID_MSG_GET_EMSK, bszMessage, TRUE); 
}

// -------------------------------------------------------------------------------------------
// Sync Up
//
static void CSCM_IdleMode_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs)
	{

	BeceemAPI_t hAPI;
	STRING bszMessage = NULL;
	BOOL bFormatError = FALSE;
	
	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;
	
	if (hAPI->u32State == STATE_SHUTDOWN)
		bszMessage = S("Device is shut down: idlemode command is not allowed");
	else if (u32NumberOfArgs != 1)
		bFormatError = TRUE;
	else if (BcsCmpNoCase(abszArgStrings[0],S("enter")))
		{
		if (hAPI->u32State == STATE_IDLE)
			bszMessage = S("Device is already in idle mode");
		else
			BeceemAPI_IdleModeRequest(hAPI, TRUE);
		}
	else if (BcsCmpNoCase(abszArgStrings[0],S("leave")))
		{
		if (hAPI->u32State != STATE_IDLE)
			bszMessage = S("Device is not in idle mode");
		else
			BeceemAPI_IdleModeRequest(hAPI, FALSE);
		}
	else
		bFormatError = TRUE;

	if (bFormatError)
		bszMessage = S("IdleMode command requires 'enter' or 'leave' as the only parameter");
	else if (bszMessage == NULL)
		bszMessage = S("Idle mode request submitted");

	ServerSocketResponse(hAPI, ID_MSG_IDLE_MODE, bszMessage, TRUE); 

	} /* CSCM_IdleMode_Command */

// --------------------------------------------------------------------------------
// Print a help summary
// --------------------------------------------------------------------------------

void CSCM_Help_Command(BeceemCSCM_t hCSCM)
	{

	int n;
	BeceemAPI_t hAPI = NULL;
	PrintContext_t hPrint = NULL;
	STRING bszKeyword, bszArgHelp, bszRest, bszDesc;
	B_CHAR *pwNL; 
	B_CHAR bszBuffer[512];

	if (!hCSCM || (hAPI = hCSCM->hAPI) == NULL)
		return;

	hPrint = hAPI->hPrintMisc;

	ServerSocketBeginPrint(hPrint);

	Print_Formatted(hPrint, S("\n--------------------\n")
							S("Command help summary\n")
		                    S("--------------------\n\n"));

	for (n=0; g_astCmdInfo[n].u32ID < ID_MSG_START_INTERNAL; n++)
		{

#ifdef WIN32
		if (g_astCmdInfo[n].u32ID == ID_MSG_CF_DOWNLOAD ||
			g_astCmdInfo[n].u32ID == ID_MSG_DHCP)
			continue;
#endif
#ifdef OSX
		if (g_astCmdInfo[n].u32ID == ID_MSG_DHCP)
			continue;
#endif

		bszKeyword = g_astCmdInfo[n].bszKeyword;
		bszArgHelp = g_astCmdInfo[n].bszArgHelp;
		if (bszArgHelp != NULL)
			{
		
			if (b_strchr(bszArgHelp, C(':')) == NULL)
				Print_Formatted(hPrint, B_SF1(10) S(" : ") B_SF, bszKeyword, bszArgHelp);
			else
				{
				Print_Formatted(hPrint, B_SF1(10) S("\n"), bszKeyword);
				b_strncpy(bszBuffer, bszArgHelp, BCHARSOF(bszBuffer));
				bszRest = bszBuffer;
				while (bszRest[0] != B_NUL)
					{
					pwNL = b_strchr(bszRest, C('\n'));
					if (pwNL == NULL)
						break;
					*pwNL  = B_NUL;
					bszDesc = bszRest;
					Print_Formatted(hPrint, S("   ") B_SFNL, bszDesc);
					bszRest = pwNL + 1;
					}
				}
			}
		}

	ServerSocketEndPrint(hAPI, hPrint, ID_MSG_HELP);

	} /* CSCM_Help_Command */

static BOOL NotIdleOrShutdown(BeceemAPI_t hAPI, UINT32 u32MsgID, STRING bszType)
	{

	STRING bszReason, bszRetVal;
	B_CHAR bszBuffer[128];

	if (hAPI->u32State == STATE_IDLE)
		bszReason = S("Idle");
	else if (hAPI->u32State == STATE_SHUTDOWN)
		bszReason = S("Shutdown");
	else if (hAPI->u32State == STATE_HMC_SHUTDOWN)
		bszReason = S("Half-mini card shutdown");
	else
		bszReason = NULL;

	if (bszReason != NULL)
		{
		bszRetVal = StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
			B_SF S(" are not available in ") B_SF S(" Mode"), bszType, bszReason);
		ServerSocketResponse(hAPI, u32MsgID, bszRetVal, TRUE);
		return FALSE;
		}
	else
		return TRUE;

	} // NotIdleOrShutdown


// -------------------------------------------------------------
// DecodeLinkStatsArgs: Decodes the command arguments for 
// the resetstats and linkstats commands
//
// Input args:
//		hAPI				- API handle
//		abszArgStrings		- array of command arguments
//		u32NumberOfArgs		- Number of command args
//
// Output args:
//		abUseRpt			- Array of indicators for each stats report type (size = MAX_STATS_REPORT)
//
// returns TRUE, if args are valid
//         FALSE, on invalid arg
//
static BOOL DecodeLinkStatsArgs (BeceemAPI_t hAPI, STRING abszArgStrings[], UINT32 u32NumberOfArgs, 
								 BOOL abUseRpt[])
	{

	UINT32 u32RptIdx, u32ArgIdx;
	BOOL bInvalidOption = FALSE;
	BOOL bFoundReport = FALSE;
	STRING bszArg;
			
	if (abUseRpt == NULL || abszArgStrings == NULL)
		return FALSE;

	memset(abUseRpt, 0, MAX_STATS_REPORT*sizeof(BOOL));

	if (u32NumberOfArgs < 1)
		abUseRpt[DEFAULT_STATS_REPORT_INDEX] = TRUE;
	else
		{
		for (u32ArgIdx = 0; u32ArgIdx < u32NumberOfArgs && !bInvalidOption; u32ArgIdx++)				
			{			
			bszArg = abszArgStrings[u32ArgIdx];

			// Search for the arg value
			for (u32RptIdx = 0; u32RptIdx < MAX_STATS_REPORT && !bInvalidOption; u32RptIdx++)
				{
				if (BcsCmpNoCase(bszArg, hAPI->hLinkStatsReport[u32RptIdx].bszStatsRptName))
					{
					if (abUseRpt[u32RptIdx])
						bInvalidOption = TRUE;
					else
						{
						abUseRpt[u32RptIdx] = TRUE;
						bFoundReport = TRUE;
						}
					break;
					}
				else if (BcsCmpNoCase(bszArg, S("all")))
					{
					if (abUseRpt[u32RptIdx])
						bInvalidOption = TRUE;
					else
						{
						abUseRpt[u32RptIdx] = TRUE;
						bFoundReport = TRUE;
						}
					}
				}
			}

		if (!bInvalidOption && !bFoundReport)
			bInvalidOption = TRUE;

		}

	if (bInvalidOption)
		memset(abUseRpt, 0, MAX_STATS_REPORT*sizeof(BOOL));

	return !bInvalidOption;

	} // DecodeLinkStatsArgs

// --------------------------------------------------------------------------------
// STRING ProcessClientCommand(BeceemWcmConnArgs_t pstArgs
//				UINT32 u32ID, STRING abszArgStrings[], UINT32 u32NumberOfArgs, BOOL *bQuit)
//
// Description: Processes a single client command
//
// Input parameters:
//		pstArgs         = Connection args structure
//		u32ID           = Command ID
//		abszArgStrings  = Array of command arguments
//	    u32NumberOfArgs = Number of command arguments
//
// Output parameter:
//		bQuit  = Thread should quit
//
// Returns: status code
// -------------------------------------------------------------------

static STRING ProcessClientCommand(BeceemAPI_t hAPI,
				BeceemWcmConnArgs_t *pstArgs,
				UINT32 u32ID, STRING abszArgStrings[], UINT32 u32NumberOfArgs, UINT32 *pu32Status)
	{
	BOOL bInvalidKeyword = FALSE;
	BOOL bError = FALSE;
	STRING bszRetVal, bszStatusText, bszReason;
	BeceemCSCM_t hCSCM = pstArgs->hCSCM;
	UINT32 u32TextLength = 0;
	stCmdInformation_t * pstCmdInfo;
	B_CHAR bszBuffer[128];

	pstCmdInfo = GetCmdInfo(u32ID);
	PrintDiag(DIAG_NORMAL, hAPI, S("Executing command '") B_SF S("'\n"), pstCmdInfo->bszKeyword);

	// Immediately handle commands that don't require an API lock, because they don't return
	// any output

	switch (u32ID)
		{
		case ID_MSG_BYE:
			*pu32Status = CLIENT_QUIT;
			return NULL;

		case ID_MSG_NOP:
			return NULL;

		default: // continue processing
			break;
		}

	// Get exclusive access to Beceem API
	if (!BeceemAPI_Lock(hAPI, pstArgs->socClient, &bError) || bError)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("Socket %08X can't get Beceem API lock\n"), pstArgs->socClient);
		return NULL;
		}

	if (!BeceemAPI_IsOpen(hAPI) && u32ID > ID_MSG_CM_STATS && u32ID < ID_MSG_START_INTERNAL)
		{
		ServerSocketResponse(hAPI, u32ID, S("Beceem WiMAX device is NOT present"), TRUE);
		return NULL;
		}

	switch (u32ID)
		{

		case ID_MSG_HELP:
			CSCM_Help_Command(hCSCM);
			break;

		case ID_MSG_SERVER:
			if (u32NumberOfArgs == 0)
				CSCM_GetServerInfo_Command(hCSCM);
			else if (u32NumberOfArgs == 1 && BcsCmpNoCase(abszArgStrings[0],S("stop")))
				{
				ServerSocketResponse(hAPI, ID_MSG_TERMINATE, S("Stopping the CM Server"), TRUE);
				*pu32Status = CLIENT_TERMINATE;
				}
			else
				ServerSocketResponse(hAPI, ID_MSG_SERVER, S("Invalid 'server' command parameters"), TRUE);
			break;

		case ID_MSG_GET:
			BeceemAPI_GetOption(hAPI, abszArgStrings, u32NumberOfArgs);
			break;

		case ID_MSG_SET:
		case ID_MSG_OPTIONS:
			if (u32ID == ID_MSG_SET)
				BeceemAPI_SetOption(hAPI, abszArgStrings, u32NumberOfArgs);
			else
				BeceemAPI_OptionsCommand(hAPI, abszArgStrings, u32NumberOfArgs);
			hCSCM->u32AutoReConnectIntervalMs         = 1000 * Options_GetUint32ByEntry(hAPI->pOptAutoReConnectIntervalSec, NULL);
			hCSCM->u32AutoReDisconnectIntervalMs      = 1000 * Options_GetUint32ByEntry(hAPI->pOptAutoReDisconnectIntervalSec, NULL);
			hCSCM->u32BasebandStatusRequestIntervalMs = 1000 * Options_GetUint32ByEntry(hAPI->pOptLinkStatusRequestPeriodSec, NULL);
			// Minimum value is 500 ms
			if (hCSCM->u32BasebandStatusRequestIntervalMs < 500)
				hCSCM->u32BasebandStatusRequestIntervalMs = 500;

			if(!Options_GetBoolByEntry(hAPI->pOptLPSearchInShutDownEnabled, NULL))
				hCSCM->bAutonomousScanParamsInitialized = FALSE;
				
			break;

		case ID_MSG_CM_STATS:
			CSCM_EventCounts_Command(hCSCM);
			break;

		case ID_MSG_NETWORK_SEARCH:
				CSCM_NetworkSearch_Command(hCSCM, abszArgStrings, u32NumberOfArgs);
			break;

		case ID_MSG_CONNECT:
			if (hCSCM->bDisableConnect)
				{
				switch (hCSCM->eDisableConnectReason)
					{ 
					case eAlreadyConnReason:  bszReason = S("already connected"); break;
					case eAuthInProgReason:   bszReason = S("authentication in progress"); break;
					case eSearchInProgReason: bszReason = S("network search in progress"); break;
					case eAutoReConnReason:   bszReason = S("auto-reconnect option is enabled"); break;
					case eNoReason:
					default:	
						bszReason = S("unknown reason");
						break;
					}
					bszRetVal = StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
						S("connect command is currently disabled: ") B_SF, bszReason);
					ServerSocketResponse(hAPI, ID_MSG_CONNECT, bszRetVal, TRUE);
				}
			else
				{
				bszRetVal = CSCM_SelectBSForConnect(hCSCM, abszArgStrings, u32NumberOfArgs, &bError);
				if (!bError)
					{
					if (u32NumberOfArgs > 0)
						CSCM_SyncDown_Command(hCSCM);
					CSCM_Connect_Command(hCSCM, TRUE);
					}
				else 
					{
					if (bszRetVal == NULL)
						bszRetVal = S("connect failed: Unknown error reason");
					ServerSocketResponse(hAPI, ID_MSG_CONNECT, bszRetVal, TRUE);
					}
				}
			break;

		case ID_MSG_DISCONNECT:
			CSCM_Disconnect_Command(hCSCM);
			break;

#ifdef LINUX
		case ID_MSG_DHCP:
			BeceemAPI_RefreshIPAddress(hAPI);
			ServerSocketResponse(hAPI, ID_MSG_DHCP, S("IP refresh started"), TRUE); 
			break;
#endif

		case ID_MSG_RESET_LINK_STATS:
			{
			BOOL abUseRpt[MAX_STATS_REPORT];
			
			if (DecodeLinkStatsArgs(hAPI, abszArgStrings, u32NumberOfArgs, abUseRpt))
				{
				if (NotIdleOrShutdown(hAPI, ID_MSG_RESET_LINK_STATS, S("Reset statistics")))
					{
					BeceemAPI_ResetStatistics(hAPI, abUseRpt);
					BeceemAPI_GetStatisticsMIBsRequest(hAPI, abUseRpt);
					}
				}
			else
				ServerSocketResponse(hAPI, ID_MSG_RESET_LINK_STATS, S("ERROR: Invalid report type"), TRUE); 
			}
			break;

		case ID_MSG_LINK_STATS:
			{
			BOOL abUseRpt[MAX_STATS_REPORT];
			
			if (DecodeLinkStatsArgs(hAPI, abszArgStrings, u32NumberOfArgs, abUseRpt))
				{
				if (NotIdleOrShutdown(hAPI, ID_MSG_LINK_STATS, S("Link statistics")))
					BeceemAPI_GetStatisticsMIBsRequest(hAPI, abUseRpt);
				}
			else
				ServerSocketResponse(hAPI, ID_MSG_LINK_STATS, S("ERROR: Invalid report type"), TRUE); 
			}
			break;

		case ID_MSG_BASEBAND_STATUS:
			// Just return the last status received with periodic link requests
			WcmMutex_Wait(hAPI->hMutexStatusResponse);
			bszStatusText = Print_GetText(hAPI->hPrintConStat, &u32TextLength);
			ServerSocketResponse(hAPI, u32ID, bszStatusText, TRUE);
			WcmFree(bszStatusText);
			WcmMutex_Release(hAPI->hMutexStatusResponse);
			break;

		case ID_MSG_SYNC_UP:
			CSCM_SyncUp_Command(hCSCM);
			ServerSocketResponse(hAPI, ID_MSG_SYNC_UP, S("Sync up request sent"), TRUE); 
			break;

		case ID_MSG_SYNC_DOWN:
			CSCM_SyncDown_Command(hCSCM);
			ServerSocketResponse(hAPI, ID_MSG_SYNC_DOWN, S("Sync down request sent"), TRUE); 
			break;

		case ID_MSG_SHUT_DOWN:
			CSCM_ShutDown_Command(hCSCM, abszArgStrings, u32NumberOfArgs) ;
			break;

		case ID_MSG_RESET_CHIP:
			CSCM_ResetBasebandChip_Command(hCSCM); 
			ServerSocketResponse(hAPI, ID_MSG_RESET_CHIP, S("Chip reset request sent"), TRUE); 
			break;


#ifdef LINUX_OR_OSX
		case ID_MSG_CF_DOWNLOAD:
			CSCM_DownloadConfigFile_Command(hCSCM);
			break;
#endif

		case ID_MSG_FW_DOWNLOAD:
			CSCM_DownloadFirmwareFile_Command(hCSCM);
			break;

		case ID_MSG_VERSIONS:
			if (NotIdleOrShutdown(hAPI, ID_MSG_VERSIONS, S("Versions")))
				BeceemAPI_GetSSInfoRequest(hAPI);
			break;

		case ID_MSG_IDLE_MODE:
			CSCM_IdleMode_Command(hCSCM, abszArgStrings, u32NumberOfArgs);
			break;

		case ID_MSG_GET_EMSK:
			CSCM_GetEMSK_Command(hCSCM);
			break;
			
		default:
			BeceemAPI_Unlock(hAPI, &bError);
			bInvalidKeyword = TRUE;
			break;
		}


	if (bInvalidKeyword)
		ServerSocketResponse(hAPI, ID_MSG_ERROR, S("ERROR: Unknown command"), TRUE); 

	return NULL;

	} /* ProcessClientCommand */

// --------------------------------------------------------------------------------
// static int RunServer(void)
//
// Description: Server main entry point.
//
// Returns: status code
// -------------------------------------------------------------------

static int RunServer(void)
	{

	UINT32 u32ConnectionCount = 0;
	WcmSocket_t socListen, socAccept;
	B_CHAR bszRemoteAddress[64];
	BeceemWcmConnArgs_t *pstArgs;
	WcmThread_t threadBeceemConnection;
	UINT32 u32BeceemWcmConnThreadID;
	UINT32 u32ErrorCode = 0;
	BeceemCSCM_t hCSCM = NULL;
	BeceemAPI_t hAPI = NULL;

	// ----------------------------------------------------------------------------------------------
	// Create and initialize the CSCM object
	if (g_stCmdArgValues.bszConfFilePath != NULL && !DoesFileExist(g_stCmdArgValues.bszConfFilePath))
		FatalError(S("Can't open the CSCM configuration file specified by the -c switch"));
	hCSCM = CSCM_Create(g_stCmdArgValues.bszConfFilePath, g_stCmdArgValues.bDebugMode);
	g_hCSCM = hCSCM;

	if (!hCSCM)
		FatalError(S("CSCM Initialization Failed - See CSCM debug log for further explanation"));

	// ----------------------------------------------------------------------------------------------
	// Install the signal handler
	signal(SIGABRT, SignalHandler);
	signal(SIGINT,  SignalHandler);
	signal(SIGTERM, SignalHandler);

	// ----------------------------------------------------------------------------------------------
	// Set up listen socket
	if (!WcmSocket_Init())
		FatalError(S("Listen socket initialization failed"));

	socListen = WcmSocket_Create();
	if (socListen == CSCM_INVALID_SOCKET)
		FatalError(S("Listen socket creation failed"));
		
	if (!WcmSocket_Bind(socListen, g_stCmdArgValues.bszListenAddress, g_stCmdArgValues.u32ListenPort))
		FatalError(S("Listen socket bind failed"));

	// Save the pointers
	hAPI = hCSCM->hAPI;
	hCSCM->socketListen = socListen;	

	// ----------------------------------------------------------------------------------------------
	// Accept connections
	while (!g_bClientTerminate && !g_bSignalTerminate)
		{
		if ((socAccept = WcmSocket_Accept(socListen, bszRemoteAddress, BCHARSOF(bszRemoteAddress), &u32ErrorCode)) 
			   == CSCM_INVALID_SOCKET)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("Socket accept error #%08X\n"), u32ErrorCode);
			PrintDiag(DIAG_ERROR, hAPI, S("Sleeping for 200 ms\n"));		
			SleepMs(200);
			}
		else
			{

			u32ConnectionCount++;

			if ((pstArgs = WcmMalloc(sizeof(BeceemWcmConnArgs_t))) == NULL)
				{
				PrintDiag(DIAG_ERROR, hAPI, S("Unable to allocate connection args structure."));
				PrintDiag(DIAG_ERROR, hAPI, S("Out of memory? Terminating server.\n"));
				break;
				}

			pstArgs->socClient       = socAccept; // This is unique to each client
			pstArgs->hCSCM		     = hCSCM;
			pstArgs->hAPI            = hCSCM->hAPI;
			pstArgs->mutexBeceemAPI  = hCSCM->mutexBeceemAPI;
			pstArgs->bszConfFilePath = g_stCmdArgValues.bszConfFilePath;

			PrintDiag(DIAG_NORMAL, hAPI, S("Got connection %d from ") B_SFNL, u32ConnectionCount, bszRemoteAddress);

			threadBeceemConnection = 
				WcmThread_Create(
					&BeceemWcmConnection,
					pstArgs,
					&u32BeceemWcmConnThreadID);

			if (!threadBeceemConnection)
				{
				PrintDiag(DIAG_ERROR, hAPI, S("Unable to initialize BeceemWcmConnection thread."));
				}
			else
				{
				PrintDiag(DIAG_NORMAL, hAPI, S("Wcm connection handling thread launched successfully\n"));
				}
#ifdef WIN32
			CloseHandle(threadBeceemConnection);
#endif

			}

		}

	return 0; 

	} /* RunServer */

// --------------------------------------------------------------------------------
// static void SignalHandler(int signal)
//
// Description: Handle signals
//
// -------------------------------------------------------------------

static void SignalHandler(int signal)
	{

	STRING bszType;

	g_bSignalTerminate = TRUE;

	if (signal == SIGABRT ||
		signal == SIGINT  ||
		signal == SIGTERM)
		{
		switch (signal)
			{
			case SIGABRT: bszType = S("SIGABRT"); break;
			case SIGINT:  bszType = S("SIGINT"); break;
			case SIGTERM: bszType = S("SIGTERM"); break;
			default:      bszType = S("unknown signal"); break;
			}
		TerminateServer(g_hCSCM, bszType);
		}

	} /* SignalHandler */

// --------------------------------------------------------------------------------
// static void TerminateServer(BeceemCSCM_t hCSCM)
//
// Description: Terminate the server
//
// -------------------------------------------------------------------

static void TerminateServer(BeceemCSCM_t hCSCM, STRING bszExitType)
	{

	UINT32 u32ErrorCode = (UINT32) -1;
	WcmSocket_t socketListen = hCSCM->socketListen;

	if (!hCSCM)
		return;

	// Notify the connection handler that the server is terminating
	g_bTerminateCalled = TRUE;

	// Clean up the listen socket
	WcmSocket_Close(socketListen, &u32ErrorCode);
	WcmSocket_Destroy(socketListen);
	WcmSocket_DeInit();

	// Clean up the CSCM
	CSCM_Destroy(hCSCM);
	// Write to system log
	SysLogWPrintf(SYSLOG_INFO, S("Beceem CM Server v") B_SF S(" exit: ") B_SF S("."), 
			SERVER_VERSION, bszExitType);
	SysLogClose();

	// For IDE debugging only
	PressEnterToContinue();

	} /* TerminateServer */

// --------------------------------------------------------------------------------
// ProcessArgs(int argc, STRING argv[])
//
// Description: Process command line arguments.
//
// Input parameters:
//		argc	- Command line argument count
//		argv[]	- Array of command line argument strings
//
// Returns: TRUE, if successful; FALSE, on error
// --------------------------------------------------------------------------------

static BOOL ProcessArgs(int argc, STRING wargv[], BOOL *pbQuit)
	{

	int n;
	BOOL bError = FALSE;
	FILE *fInfile = NULL;
	B_CHAR wc;
	
	// Set default values
	g_stCmdArgValues.bszListenAddress	= DEFAULT_LISTEN_ADDRESS;
	g_stCmdArgValues.u32ListenPort		= DEFAULT_LISTEN_PORT;
	g_stCmdArgValues.bszConfFilePath    = NULL;
	g_stCmdArgValues.u32DebugLevel      = 0;
	g_stCmdArgValues.bDebugMode			= FALSE;
	
	*pbQuit = FALSE;

	// Process command line (skip wargv[0])
	n = 1;
	while (n < argc )
		{
		// server_wprintf(S("argv[%2d]: ") B_SFNL, n, wargv[n]);
		if (b_strlen(wargv[n]) == 2 && wargv[n][0] == C('-'))
			{
			wc = wargv[n][1];
			switch (wc)
				{
				case C('a'):
				case C('c'):
				case C('d'):
				case C('p'):
					if (n+1 >= argc)
						return FALSE;

					if (wc == C('a'))
						g_stCmdArgValues.bszListenAddress = wargv[n+1];
					else if (wc == C('c'))
						g_stCmdArgValues.bszConfFilePath = wargv[n+1];
					else if (wc == C('d'))
						g_stCmdArgValues.u32DebugLevel = BcsToInt32(wargv[n+1], &bError);
					else if (wc == C('p'))
						g_stCmdArgValues.u32ListenPort = BcsToInt32(wargv[n+1], &bError);

					//ClientDebugPrint(CLIENT_DEBUG_INFO, S("wargv[%d] = ") B_SFNL, n, wargv[n]);
					//ClientDebugPrint(CLIENT_DEBUG_INFO, S("wargv[%d] = ") B_SFNL, n+1, wargv[n+1]);
					
					if (bError)
						n++;
					else
						n += 2;
					break;

				case C('D'):  // debug mode (do not fork as daemon)
					g_stCmdArgValues.bDebugMode = TRUE;
					n++;
					break;

				case C('v'):
					server_wprintf(S("Beceem CM Server ") B_SF S(" (Built ") B_NSF S(" ") B_NSF S(")\n"),
                    	SERVER_VERSION, __DATE__, __TIME__);
					n++;
					*pbQuit = TRUE;
					break;

				default:
					bError = TRUE;
					n++;
					break;
				}
			}
		else
			{
			bError = TRUE;
			n++;
			}
		}

	// Check to see if the config file exists and is readable
	if (g_stCmdArgValues.bszConfFilePath != NULL)
		{
		if ((fInfile = BcsFopen(g_stCmdArgValues.bszConfFilePath, S("rt"))) == NULL)
			{
			server_wprintf(S("ERROR: Can't find file '") B_SF S("'\n"),
				g_stCmdArgValues.bszConfFilePath);
			SysLogWPrintf(SYSLOG_ERROR, S("Can't find file '") B_SF S("'\n"),
				g_stCmdArgValues.bszConfFilePath);
			bError = TRUE;
			}
		else
			fclose(fInfile);
		}

	return !bError;

	} /* ProcessArgs */

// -------------------------------------------------------------------------
//
// void ServerUsage(void)
//
// Description: Print the usage help message.
//
// -------------------------------------------------------------------------
static void ServerUsage(void)
	{

	server_wprintf(
		    S("Usage:\n")
			S("   wimaxd <options>\n\n")
			S("Options:\n")
#ifdef LINUX_OR_OSX
			S("   -D                     Run as a foreground console app (do not run as a daemon)\n")
#endif
#ifdef WIN32
			S("   -D                     Run as a foreground console app (do not run as a service)\n")
#endif
			S("   -c <config file path>  Specify CM Server configuration file location\n")
			S("   -a <IP Address>        Specify CM Server's IP address (default = localhost)\n"));
	server_wprintf(
			S("   -p <port>              Specify CM Server's port number (default = %d)\n"), DEFAULT_LISTEN_PORT);
	server_wprintf(
			S("   -d <level>             Specify the debug message level\n")
			S("   -v                     Print version and quit\n")
			);

	} /* ClientUsage */

static void FatalError(STRING bszMessage)
	{

	if (bszMessage)
		{
		server_wprintf(S("FATAL ERROR: ") B_SFNL, bszMessage);
		SysLogWPrintf(SYSLOG_FATAL_ERROR, bszMessage);
		SysLogClose();
		}

#ifdef WIN32
	if (g_hServiceStatus != INVALID_HANDLE_VALUE)
		{
        g_ServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
        g_ServiceStatus.dwWin32ExitCode      = 1; 
        SetServiceStatus(g_hServiceStatus, &g_ServiceStatus); 
		}
	else
#endif
		exit(1);

	} /* Fatal_Error */

#ifdef WIN32

// ============================================================================================
// Main Program for windows service
// ============================================================================================

void ServiceControlHandler(DWORD dwRequest)
	{

    switch(dwRequest) 
    { 
        case SERVICE_CONTROL_STOP: 
        case SERVICE_CONTROL_SHUTDOWN: 
			if (g_hCSCM)
				TerminateServer(g_hCSCM, S("service shutdown"));

            g_ServiceStatus.dwWin32ExitCode = 0; 
            g_ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
            break; 
 
        default:
            break;
    } 
 
    // Report current status
    SetServiceStatus (g_hServiceStatus,  &g_ServiceStatus);
 
    return; 

	} /* ServiceControlHandler */

void ServiceMain(int argc, char* argv[]) 
	{ 

	BOOL bQuit = FALSE;
	int n;

    g_ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    g_ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    g_ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    g_ServiceStatus.dwWin32ExitCode      = 0; 
    g_ServiceStatus.dwServiceSpecificExitCode = 0; 
    g_ServiceStatus.dwCheckPoint         = 0; 
    g_ServiceStatus.dwWaitHint           = 0; 

    g_hServiceStatus = RegisterServiceCtrlHandler(
		WINDOWS_SERVICE_NAME, 
		(LPHANDLER_FUNCTION) ServiceControlHandler); 
    if (g_hServiceStatus == (SERVICE_STATUS_HANDLE) 0 ) 
		{ 
        // Registering Control Handler failed
        return; 
		}  

#if 1
	SysLogWPrintf(SYSLOG_INFO, S("ServiceMain - argc = %d"), argc);
	for (n=0; n<argc; n++)
		SysLogWPrintf(SYSLOG_INFO, S("ServiceMain - arg %d: ") B_SF, n, argv[n]);
#endif

    // Process command line args 
    if (!ProcessArgs(argc, (B_CHAR **) argv, &bQuit))
		{
		// ServerUsage();
		SysLogWPrintf(SYSLOG_ERROR, S("Invalid command line arguments"));
		}
	else if (!bQuit)
		{
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
		SetServiceStatus (g_hServiceStatus, &g_ServiceStatus);
	 
		// Run the server here
		RunServer();
		}

	// Notify that it's stopped
    g_ServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
    g_ServiceStatus.dwWin32ExitCode      = 1; 
    SetServiceStatus(g_hServiceStatus, &g_ServiceStatus); 

    return; 

} /* ServiceMain */

#endif

// ============================================================================================
// Main Program
// ============================================================================================

// --------------------------------------------------------------------------------
// wmain(int argc, STRING argv[])
// or 
// main(int argc, char *argv[])
//
// Description: Beceem CM Server main program
//
// Input parameters:
//		argc	- Command line argument count
//		argv[]	- Array of command line argument strings
//
// Returns: status code
// --------------------------------------------------------------------------------
#ifdef WIN32
int b_main (int argc, STRING b_argv[])
	{

	INT32 i32ErrorCode;
	STRING bszErrorMessage = NULL;
	SERVICE_TABLE_ENTRY ServiceTable[2];
	BOOL bQuit = FALSE;
	int n;

	g_hServiceStatus = INVALID_HANDLE_VALUE;

	ServiceTable[0].lpServiceName = WINDOWS_SERVICE_NAME;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION) ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;

	SysLogOpen();
	SysLogWPrintf(SYSLOG_INFO, S("Starting Beceem CM Server v") B_SF S(" (") B_NSF S(" ") B_NSF S(")."), 
		SERVER_VERSION, __DATE__, __TIME__);

#if 1
	SysLogWPrintf(SYSLOG_INFO, S("wmain - argc = %d"), argc);
	for (n=0; n<argc; n++)
		SysLogWPrintf(SYSLOG_INFO, S("wmain - arg %d: ") B_SF, n, b_argv[n]);
#endif

	// Process the command line arguments
	if (!ProcessArgs(argc, b_argv, &bQuit))
		{
		ServerUsage();
		SysLogWPrintf(SYSLOG_ERROR, S("Invalid command line arguments"));
		exit(1);
		}
	if (bQuit)
		return 0;

	// Run as a program
	if (g_stCmdArgValues.bDebugMode)
		return RunServer();

	// or as a service
	else
		{
		if (!StartServiceCtrlDispatcher(ServiceTable))
			{
			i32ErrorCode = GetLastError();
			server_wprintf(S("ERROR: "));
			switch (i32ErrorCode)
				{
				case ERROR_SERVICE_ALREADY_RUNNING:
					bszErrorMessage = S("Service is already running.");
					break;
				case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
					bszErrorMessage = S("Use the -D switch to run Beceem CM Server from the command line.");
					break;
				default:
					bszErrorMessage = S("Unknown error code");
					break;
				}
			server_wprintf(B_SFNL, bszErrorMessage);
			SysLogWPrintf(SYSLOG_ERROR, bszErrorMessage);
			SysLogWPrintf(SYSLOG_INFO, S("Beceem WCM Server stopping."));
			SysLogClose();
			return 1;
			}
		else
			return 0;
		}

	} /* wmain */
#endif

#ifdef LINUX_OR_OSX
int b_main (int argc, STRING b_argv[])
	{

#ifdef ENABLE_WIDE_CHAR
	STRING b_wargv[WARGV_LEN];
#endif
	BOOL bQuit = FALSE;
	int iRetCode;

	InstallSignalHandlers();
	{
		time_t rawtime;
		struct tm *timeinfo;
		char buffer[120];

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 120, "********** CSCM Server Started ********** %x %X", timeinfo);
		server_wprintf(B_SFNL, buffer);
	}
	
	SysLogOpen();
	SysLogWPrintf(SYSLOG_INFO, S("Starting Beceem CM Server v") B_SF S(" (") B_NSF S(" ") B_NSF S(")."), 
		SERVER_VERSION, __DATE__, __TIME__);

#ifdef ENABLE_WIDE_CHAR
    if (!WcsCreateWargv(b_wargv, WARGV_LEN, argc, b_argv))
		FatalError(S("Command line argument processing failed"));
#else
#define b_wargv b_argv
#endif

	// ----------------------------------------------------------------------------------------------
	// Process the command line arguments
	if (!ProcessArgs(argc, b_wargv, &bQuit))
		{
		ServerUsage();
		SysLogWPrintf(SYSLOG_ERROR, S("Invalid command line arguments"));
		iRetCode = 1;
		bQuit = TRUE;
		}

	// Run as a daemon
	if (!bQuit)
		{
		if (!g_stCmdArgValues.bDebugMode && daemon(0,0) != 0)
			{
			FatalError(S("Deamon startup failed"));
			iRetCode = 1;
			}
		else
			{
			SysLogWPrintf(SYSLOG_INFO, S("Beceem CM Server started."));
			iRetCode = RunServer();
			}
		}

#ifdef ENABLE_WIDE_CHAR
	WcsDestroyWargv(argc, b_wargv);
#endif

	return iRetCode;
	
	} /* main */

#endif


