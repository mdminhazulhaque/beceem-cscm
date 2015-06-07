/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------------------------
// This is the main module for the Client application
// -------------------------------------------------------------------------

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "CscmTypes.h"
#include "BeceemCscmClient.h"
#include "BeceemCscmCommon.h"
#include "SocketFunctions.h"
#include "Utility.h"
#include "Version.h"

#ifdef WIN32
#include <process.h>
#include <stdio.h>
#endif

#ifdef CALL_OMA_CLI
#include "Oma_relative.h"
#endif

//#define RUNNING_IN_IDE

static BOOL g_bInteractive;

extern stCmdInformation_t *g_astCmdInfo;


// -------------------------------------------------------------------------

static void ExecuteCommand(STRING bszCommand);
static void ClientInit(void);
static BOOL ProcessClientSwitches(int argc, STRING wargv[], INT32 *pi32NextArg, BOOL *pbQuit);
static void ClientUsage(void);
static BOOL ParseReceivedMessage(STRING bszBuffer, INT32 i32ByteCount, UINT32 u32SendCmdID, UINT32 *pu32LastID);
static void wprintf_ub(STRING bszFormat, ...);
#ifdef WIN32
static BOOL StartServerService(UINT32 u32ServerArgc, STRING *abszServerArgv, BOOL *pbAlreadyRunning);
static void SystemErrorMessage(STRING bszFunctionName);
#endif

stWcmClientParam_t g_stWcmClientParam;
STRING g_bszClientFilePath = NULL;

// -------------------------------------------------------------------------
//
// Beceem CM command line client
//
// -------------------------------------------------------------------------
int b_main(int argc, STRING b_argv[])

	{

	STRING bszServerCommand = NULL;
	INT32 i32NextArg = 0;
	BOOL bQuit = FALSE;

#ifdef ENABLE_WIDE_CHAR
#ifdef LINUX_OR_OSX
	// Convert to Unicode wargv[]
#define WARGV_LEN 100
	STRING b_wargv[WARGV_LEN];

	if (!WcsCreateWargv(b_wargv, WARGV_LEN, argc, b_argv))
		exit(1);
#else
#define b_wargv b_argv
#endif
#else
#define b_wargv b_argv
#endif

	g_bInteractive = FALSE;
	g_bszClientFilePath = b_wargv[0];

	// Initialize
	ClientInit();

	// Process the client switches
	if (argc < 2 || !ProcessClientSwitches(argc, b_wargv, &i32NextArg, &bQuit))
		{
		ClientUsage();
#ifdef RUNNING_IN_IDE
		PressEnterToContinue();
#endif
		return 1;
		}
	if (bQuit)
		return 0;

	// Build the server command line, if any
	if (i32NextArg > argc)
		return 0;
	bszServerCommand = BuildCommandLine(argc - i32NextArg, &b_wargv[i32NextArg]);
	if (bszServerCommand != NULL)
		ClientDebugPrint(CLIENT_DEBUG_INFO, S("Server command line = '") B_SF S("'\n"), bszServerCommand);

	// Run the command
	ExecuteCommand(bszServerCommand);

	// Clean up
	if (bszServerCommand != NULL)
		WcmFree(bszServerCommand);
#ifdef ENABLE_WIDE_CHAR
#ifdef LINUX_OR_OSX
	WcsDestroyWargv(argc, b_wargv);
#endif
#endif
#ifdef RUNNING_IN_IDE
	PressEnterToContinue();
#endif

	return 0;

	} /* wmain */

// -------------------------------------------------------------------------
//
// void ExecuteCommand (STRING bszCommand)
//
// Description: Executes the command on the server
//
// -------------------------------------------------------------------------

void ExecuteCommand (STRING bszCommand)
	{

	BOOL bConnected, bPrompt, bDone = FALSE, bValidCommand = FALSE;
	INT32 i32ByteCount = 0, i32SendLength = 0, i32SocketCount = 0;
	UINT32 u32CmdCount = 0;
	UINT32 u32SendCmdIndex, u32TimeoutSec = 0, u32TimeoutMs = 0;
	UINT32 u32ErrorCode = 0;
	UINT32 u32SendCmdID = ID_MSG_NOP, u32RecvCmdID = ID_MSG_NOP;
	WcmSocket_t socConnect;
	B_CHAR bszBuffer[1025];
	B_CHAR bszInteractiveCommand[2048];
	int n, iStart;

#ifdef WIN32
	UINT32 u32ServiceArgc = 0;
	B_CHAR bszSmallBuffer[10];
	BOOL bAlreadyRunning = FALSE;
	STRING abszServerArgv[9] = 
		{NULL, NULL, NULL, 
		 NULL, NULL, NULL, 
		 NULL, NULL, NULL};
#endif

	// Open connection to server
	if (!WcmSocket_Init())
		return;

	socConnect = WcmSocket_Create();
	if (socConnect == CSCM_INVALID_SOCKET)
		return;
	bConnected = WcmSocket_Connect(socConnect, 
		g_stWcmClientParam.bszServerIPAddress, g_stWcmClientParam.u32ServerPort);

#ifdef WIN32
	// If can't connect to the server, try starting the service
	if (!bConnected && !g_stWcmClientParam.bNoServerStart)
		{

		abszServerArgv[0] = S("-a");
		abszServerArgv[1] = g_stWcmClientParam.bszServerIPAddress;
		abszServerArgv[2] = S("-p");
		abszServerArgv[3] = StrPrintf(bszSmallBuffer, BCHARSOF(bszSmallBuffer), S("%lu"), g_stWcmClientParam.u32ServerPort);

		if (g_stWcmClientParam.bszServerConfigFile != NULL)
			{
			u32ServiceArgc = 6;
			abszServerArgv[4] = S("-c");
			abszServerArgv[5] = g_stWcmClientParam.bszServerConfigFile;
			}
		else
			u32ServiceArgc = 4;

		wprintf_ub(S("Starting Beceem Connection Manager service ... "));
		if (StartServerService(u32ServiceArgc, abszServerArgv, &bAlreadyRunning))
			{
			bConnected = WcmSocket_Connect(socConnect, 
				g_stWcmClientParam.bszServerIPAddress, g_stWcmClientParam.u32ServerPort);
			wprintf_ub(B_SFNL, bConnected ? S("OK") : S("FAIL"));
			}
		else if (bAlreadyRunning)
			{
			wprintf_ub(S("\nServer appears to be running, but it's not responding.\n")
					   S("Please double check the IP address and port on which the\n")
					   S("CM Server is currently listening.\n"));
			bConnected = FALSE;
			}
		else
			wprintf_ub(S("FAIL\n"));

		}
#endif

	// Use the connection
	u32SendCmdIndex = 0; // Assume BYE by default
	for (u32CmdCount = 0; bConnected && !bDone; u32CmdCount++)
		{		

		// Wait for the prompt
		bPrompt = FALSE;
		while (!bPrompt)
			{

			// ----------------------------------------------------
			// Receive the next chunk of data, subject to a timeout
			// ----------------------------------------------------
			for (n = 0; g_astCmdInfo[n].u32ID != ID_LAST_ENTRY; n++)
				if (g_astCmdInfo[n].u32ID == u32SendCmdID)
					{
					u32SendCmdIndex = n;
					break;
					}

			u32TimeoutSec = g_astCmdInfo[u32SendCmdIndex].u32TimeoutCommandMs / 1000;
			u32TimeoutMs  = g_astCmdInfo[u32SendCmdIndex].u32TimeoutCommandMs % 1000;
			i32SocketCount = WcmSocket_WaitReceive(socConnect, u32TimeoutSec, u32TimeoutMs);		
			if (i32SocketCount == 0)
				{
				i32ByteCount = 0;
				bszBuffer[0] = B_NUL;
				}
			else
				{
				i32ByteCount = WcmSocket_Receive(socConnect, (UINT8 *) bszBuffer, sizeof(bszBuffer) - sizeof(B_CHAR));
				bszBuffer[i32ByteCount/sizeof(B_CHAR)] = B_NUL; // Just in case
				}

			// Check for timeouts
			if (i32SocketCount == 0 && i32ByteCount == 0)
				{
				wprintf_ub(S("ERROR: No response from server to '") B_SF S("' command in %0.1f s\n"), 
					g_astCmdInfo[u32SendCmdIndex].bszKeyword, u32TimeoutSec + u32TimeoutMs/1000.0);
				if (g_bInteractive)
					{
					bDone   = FALSE;
					bPrompt = TRUE;
					}
				else
					{
					bDone = TRUE;
					break;
					}
				}

			// Zero byte count, means connection has been closed by the server
			// In that case, exit the loop
			else if (i32ByteCount <= 0)
				{
				//wprintf_ub(S("Connection reset by server\n"));
				if (i32ByteCount == 0)
					; // wprintf_ub(S("Connection reset by server\n"));
				if (i32ByteCount == CSCM_INVALID_SOCKET)
					wprintf_ub(S("ERROR: Invalid receive socket.\n"));
				else if (i32ByteCount == CSCM_SOCKET_RECEIVE_ERROR)
					wprintf_ub(S("ERROR: Receive socket error.\n"));
				else if (i32ByteCount < 0)
					wprintf_ub(S("ERROR: Other receive error.\n"));
				bDone = TRUE;
				break;
				}
		
			// Parse the received command
			else
				{
				bPrompt = ParseReceivedMessage(bszBuffer, i32ByteCount, u32SendCmdID, &u32RecvCmdID);	
				if (u32RecvCmdID == ID_MSG_TERMINATE)
					bDone = TRUE;
#ifdef CALL_OMA_CLI
				else if(u32RecvCmdID == ID_MSG_DISCONNECT || u32RecvCmdID == ID_MSG_SYNC_DOWN || u32RecvCmdID == ID_MSG_SHUT_DOWN) {
					wprintf_ub(S("get u32RecvCmdID is %d\n"), u32RecvCmdID);
					call_OMA_cli(7549, 6);
				}
#endif

			}
				

		} /* while (!bPrompt) */

		// Exit loop if bDone is true or, in interactive mode, if the bye has been accepted
		if (bDone || (u32SendCmdID == ID_MSG_BYE && bPrompt))
			{
			bDone = TRUE;
			break;
			}

		// Get the next command
		bValidCommand = FALSE;
		bDone         = FALSE;
		while (!bValidCommand && !bDone)
			{
			if (g_bInteractive)
				{
				if (!(u32CmdCount == 0 && bszCommand != NULL && bszCommand[0] != B_NUL))
					{
					b_fputs(S("\n> "), stdout);
					b_fgets(bszInteractiveCommand, BCHARSOF(bszInteractiveCommand), stdin);
					// Get rid of newline character
					bszInteractiveCommand[b_strlen(bszInteractiveCommand)-1] = B_NUL;
					iStart = (int) b_strspn(bszInteractiveCommand, S(" \t"));
					bszCommand = &bszInteractiveCommand[iStart];
					if (bszCommand[0] == B_NUL)
						continue;
					}
				}
			else
				// Send BYE to end the session
				{
				if (u32CmdCount == 1 || bszCommand == NULL || bszCommand[0] == B_NUL ||
					u32RecvCmdID == ID_MSG_SERVER_BUSY)
					{
					bszCommand = S("BYE");
					bDone = TRUE;
					}
				}

			// Parse the command to get the MSG ID
			u32SendCmdID = CommandID(bszCommand);
			if (u32SendCmdID == ID_MSG_INVALID_COMMAND)
				{
				wprintf_ub(S("ERROR: Invalid command\n"));
				bszCommand = NULL;
				}

			else if (u32SendCmdID == ID_MSG_INVALID_KEYWORD)
				{
				wprintf_ub(S("ERROR: Invalid command keyword\n"));
				bszCommand = NULL;
				}
			
			else
				{
				// Send the command, including the terminating B_NUL
				ClientDebugPrint(CLIENT_DEBUG_INFO, S("Sending command = '") B_SF S("'\n"), bszCommand);
				i32SendLength = (UINT32) BCHARS2BYTES(b_strlen(bszCommand) + 1);
				i32ByteCount = WcmSocket_Send(socConnect, (UINT8 *) bszCommand, i32SendLength);
				if (i32ByteCount == CSCM_SOCKET_SEND_ERROR)
					{
					wprintf_ub(S("ERROR: Socket send error\n"));
					bDone = TRUE;
					}
				else
					bValidCommand = TRUE;
				}

			} // while (!bValidCommand) 

		} /* for (u32CmdCount=0; ... */

	// Close connection to server (shuts down and closes socket)
	if (WcmSocket_Close(socConnect, &u32ErrorCode) == CSCM_SOCKET_ERROR)
		{
#ifdef WIN32
#define NO_SERVER_CODE 0x2749
#else
#define NO_SERVER_CODE ENOTCONN //0x6B
#endif
		if (u32ErrorCode == NO_SERVER_CODE)
			wprintf_ub(S("Server not found\n"));
		else
			wprintf_ub(S("Socket close error = %08X\n"), u32ErrorCode);
		}

	WcmSocket_Destroy(socConnect); // Redundant ?
 
	wprintf_ub(S("\n"));

	} /* Execute Command */

// -------------------------------------------------------------------------
//
// static BOOL ParseReceivedMessage(STRING bszBuffer, 
//					INT32 i32ByteCount, UINT32 u32SendCmdID, UINT32 *pu32LastID)
//
// Description: Parse a message received from the server.
//
// -------------------------------------------------------------------------

static BOOL ParseReceivedMessage(STRING bszBuffer, INT32 i32ByteCount, UINT32 u32SendCmdID, UINT32 *pu32RecvCmdID)
	{

	BOOL bPrompt = FALSE;
	B_CHAR wcCurrChar = B_NUL, wcLastChar = B_NUL;
	B_CHAR bszID[MAX_KEYWORD_CHARS];
	BOOL bInsideID = FALSE;
	static UINT32 u32RecvCmdID = ID_MSG_NOP;

	B_CHAR *wcpEnd;

	int nID = 0;
	UINT32 n, u32CharCount, u32LastID = ID_MSG_NOP;

	// ----------------------------------------------------
	// Parse the message
	// ----------------------------------------------------

	u32CharCount = BYTES2BCHARS(i32ByteCount);
	for (n=0; n<u32CharCount; n++)
		{
		wcCurrChar = bszBuffer[n];

		// If this is the start of a received packet, which doesn't start with the
		// message ID string, assume that we are still in the response to the last
		// valid message ID
		if (n==0 && wcCurrChar != C(':'))
			{
			bInsideID = FALSE;
			ClientReceivedPrint(CLIENT_DEBUG_INFO, bszBuffer);
			if (u32RecvCmdID == ID_MSG_WELCOME     ||
				u32RecvCmdID == ID_MSG_INFO        ||
				u32RecvCmdID == ID_MSG_WARNING     ||
				u32RecvCmdID == ID_MSG_ERROR       ||
				u32RecvCmdID == ID_MSG_SERVER_BUSY ||
				u32RecvCmdID == ID_MSG_TERMINATE   ||
				u32RecvCmdID == u32SendCmdID)

				wprintf_ub(B_SF, bszBuffer);
			}

		// Look for and isolate the message ID string, which has the format :<n>:, 
		// where <n> is an integer
		else if (wcLastChar == B_NUL && wcCurrChar == C(':'))
			{
			bInsideID = TRUE;
			nID = 0;
			ClientReceivedPrint(CLIENT_DEBUG_INFO, &bszBuffer[n]);
			}
		else if (bInsideID)
			{
			if (wcCurrChar == C(':'))
				{
				bInsideID    = FALSE;
				bszID[nID]   = B_NUL;
				u32LastID = b_strtol(bszID, &wcpEnd, 10); 
				if (u32LastID != ID_MSG_PROMPT)
					{
					u32RecvCmdID = u32LastID;
					if (u32RecvCmdID == ID_MSG_WELCOME     ||
						u32RecvCmdID == ID_MSG_INFO        ||
						u32RecvCmdID == ID_MSG_WARNING     ||
						u32RecvCmdID == ID_MSG_ERROR       ||
						u32RecvCmdID == ID_MSG_SERVER_BUSY ||
						u32RecvCmdID == ID_MSG_TERMINATE   ||
						u32RecvCmdID == u32SendCmdID)

						// Here we skip any line termination characters, if present,
						// and then print the next response string.
						{

						if (u32RecvCmdID == ID_MSG_WARNING)
							wprintf_ub(S("WARNING: "));
						else if (u32RecvCmdID == ID_MSG_ERROR)
							wprintf_ub(S("ERROR: "));
				
						n++;
						for (;n < u32CharCount; n++)
							{
							if (!(bszBuffer[n] == L'\r' || 
								  bszBuffer[n] == L'\n' || 
								  bszBuffer[n] == '\0'))
								{
								ClientReceivedPrint(CLIENT_DEBUG_INFO, &bszBuffer[n]);
								// We must use B_SF here to avoid running into problems with format
								// specifier length maximums.
								wprintf_ub(B_SF, &bszBuffer[n]);
								break;
								}
							}
						}
					}
				}
			else if (nID < BCHARSOF(bszID))
				bszID[nID++] = wcCurrChar;
			}

		// Stop processing Received characters, when we've seen the prompt string
		// after (1) welcome message, (2) error message, (3) server busy message,
		// or (4) the response to the last command we sent. Ignore anything else.
		else if (wcCurrChar == B_NUL && u32LastID == ID_MSG_PROMPT &&
			(u32RecvCmdID == ID_MSG_WELCOME     ||
			 u32RecvCmdID == ID_MSG_ERROR       ||
			 u32RecvCmdID == ID_MSG_SERVER_BUSY ||
			 u32RecvCmdID == ID_MSG_TERMINATE   ||
			 u32RecvCmdID == u32SendCmdID) )
			bPrompt = TRUE;

		wcLastChar = wcCurrChar;

		}

	if (pu32RecvCmdID)
		*pu32RecvCmdID = u32RecvCmdID;

	return bPrompt;

	} /* ParseReceivedMessage */

// -------------------------------------------------------------------------
//
// void ClientInit(void)
//
// Description: Initializes the client's global parameter structure
//
// -------------------------------------------------------------------------

void ClientInit(void)
	{

	memset (&g_stWcmClientParam, 0, sizeof(g_stWcmClientParam));

	g_stWcmClientParam.u32DebugLevel       = CLIENT_DEFAULT_DEBUG_LEVEL;
	g_stWcmClientParam.u32ServerPort       = CLIENT_DEFAULT_SERVER_PORT;
	g_stWcmClientParam.bszServerIPAddress  = CLIENT_DEFAULT_SERVER_IP;
	g_stWcmClientParam.bszServerConfigFile = NULL;
	g_stWcmClientParam.bNoServerStart      = FALSE;

	} /* ClientInit */

// -------------------------------------------------------------------------
//
// void ClientUsage(void)
//
// Description: Print the usage help message.
//
// -------------------------------------------------------------------------
static void ClientUsage(void)
	{

	b_printf(S("Usage:\n")
			S("   wimaxc <options> <server command> <server arg1> ...\n\n")
			S("Options:\n")
			S("   -i                Run in interactive mode\n")
#ifdef WIN32
			S("   -n                Do not start the Beceem CM service (overrides -c)\n")
			S("   -c <config>       Use <config> as the service's config file path\n")
#endif
			S("   -a <IP Address>   Specify CM Server's IP address (default = localhost)\n"));
	b_printf(
			S("   -p <port>         Specify CM Server's port number (default = %d)\n"),
			CLIENT_DEFAULT_SERVER_PORT);
	b_printf(
			S("   -d <level>        Generate debugging output at the specified level (0-2)\n")
			S("   -v                Print version and quit (server command ignored)\n"));

	} /* ClientUsage */

// -------------------------------------------------------------------------
//
// BOOL ProcessClientSwitches(int argc, STRING wargv[], INT32 *pi32NextArg, BOOL *pbQuit)
//
// Description: Process the command line arguments
//
// Input parameters:
//		argc	 = Number of arguments
//		wargv[]  = Argument wide string array
//
// Output parameters:
//		pi32NextArg = Returns index of last arg consumed by this
//                        function
//		pbQuit      = Returns TRUE if the -v switch was found.
//
// -------------------------------------------------------------------------

static BOOL ProcessClientSwitches(int argc, STRING wargv[], INT32 *pi32NextArg, BOOL *pbQuit)
	{

	int n;
	B_CHAR wc, *wcpEnd;

	if (!pi32NextArg)
		return FALSE;

	ClientDebugPrint(CLIENT_DEBUG_INFO, S("argc = %d\n"), argc);
	*pbQuit = FALSE;
	n = 1;
	while (n<argc && wargv[n][0] == C('-'))
		{
		wc = wargv[n][1];
		switch (wc)
			{
			case C('a'):
			case C('d'):
			case C('p'):
				if (n+1 >= argc)
					return FALSE;

				if (wc == C('a'))
					g_stWcmClientParam.bszServerIPAddress = wargv[n+1];
				else if (wc == C('d')) 
					g_stWcmClientParam.u32DebugLevel = b_strtol(wargv[n+1], &wcpEnd, 10);
				else if (wc == C('p'))
					g_stWcmClientParam.u32ServerPort = b_strtol(wargv[n+1], &wcpEnd, 10);

				ClientDebugPrint(CLIENT_DEBUG_INFO, S("wargv[%d] = ") B_SFNL, n, wargv[n]);
				ClientDebugPrint(CLIENT_DEBUG_INFO, S("wargv[%d] = ") B_SFNL, n+1, wargv[n+1]);

				n += 2;
				break;

			case C('i'):
				g_bInteractive = TRUE;
				n++;
				break;

			case C('n'):
				g_stWcmClientParam.bNoServerStart = TRUE;
				n++;
				break;

			case C('c'):
				g_stWcmClientParam.bszServerConfigFile = wargv[n+1];
				n += 2;
				break;

			case C('v'):
				wprintf_ub(S("Beceem CSCM Command Line Client ") B_SF S(" (Built ") B_NSF S(" ") B_NSF S(")\n"),
					CLIENT_VERSION, __DATE__, __TIME__);
				ClientDebugPrint(CLIENT_DEBUG_INFO, S("wargv[%d] = ") B_SFNL, n, wargv[n]);
				n++;
				*pbQuit = TRUE;
				break;
			default:
				wprintf_ub(S("ERROR: Unknown option = ") B_SFNL, wargv[n]);
				return FALSE;
			}
		}

	*pi32NextArg = n;

	return TRUE;

	} /* ProcessClientSwitches */

// -------------------------------------------------------------------------
//
// void ClientDebugPrint(int iDebugLevel, STRING bszFormat, ...)
//
// Description: Print debug output to the console
//
// Input parameters:
//		iDebugLevel	= Client debug print level (0-3)
//		bszFormat   = Format as wide string
//      ...         = Printf arguments
//
// -------------------------------------------------------------------------
void ClientDebugPrint(int iDebugLevel, STRING bszFormat, ...)
	{

	va_list args;

	if (iDebugLevel <= g_stWcmClientParam.u32DebugLevel)
		{
		b_printf(S("+++ DEBUG[%d] "), iDebugLevel);
		va_start(args, bszFormat);
		b_vprintf(bszFormat, args);
		fflush(stdout);
		}

	} /* ClientDebugPrint */

// -------------------------------------------------------------------------
//
// void ClientReceivedPrint(int iDebugLevel, STRING bszMessage)
//
// Description: Print a received message to the console
//
// Input parameters:
//		iDebugLevel	= Client debug print level (0-3)
//		bszMessage  = Message as wide string
//
// -------------------------------------------------------------------------
void ClientReceivedPrint(int iDebugLevel, STRING bszMessage)
	{

	if (iDebugLevel <= g_stWcmClientParam.u32DebugLevel)
		wprintf_ub(S("\n=======================================================\n")
			    S("Received = '") B_SF S("'")
				S("\n=======================================================\n"), 
				bszMessage);

	} /* ClientDebugPrint */

// -------------------------------------------------------------------------
//
// void wprintf_ub(STRING bszFormat, ...)
//
// Description: Unbuffered b_printf (flush the buffer after each b_printf)
//
// Input parameters:
//		bszFormat   = Format as wide string
//      ...         = Printf arguments
//
// -------------------------------------------------------------------------
static void wprintf_ub(STRING bszFormat, ...)
	{

	va_list args;

	va_start(args, bszFormat);
	b_vprintf(bszFormat, args);

	fflush(stdout);

	} /* ClientDebugPrint */

#ifdef WIN32
// -------------------------------------------------------------------------
//
// void StartServerService(void)
//
// Description: Starts the Beceem server as a Windows service
//
// Source URL: http://msdn.microsoft.com/en-us/library/ms686315(VS.85).aspx
//
// -------------------------------------------------------------------------

static BOOL StartServerService(UINT32 u32ServerArgc, STRING *abszServerArgv, BOOL *pbAlreadyRunning)
	{

	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS_PROCESS ssStatus; 
	DWORD dwBytesNeeded;
	DWORD dwOldCheckPoint; 
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
	BOOL  bRetVal = FALSE;

	if (pbAlreadyRunning != NULL)
		*pbAlreadyRunning = FALSE;

	// Open the SCM database
	hSCManager = OpenSCManager(NULL, NULL, GENERIC_READ /* or SERVICE_ALL_ACCESS */);
    if (hSCManager == NULL)
		{
		SystemErrorMessage(S("OpenSCManager"));
		return FALSE;
		}

	// Open the service itself
    hService = OpenService( 
        hSCManager,					// SCM database 
        BECEEM_CSCM_SERVICE_NAME,   // name of service 
        SERVICE_ALL_ACCESS);		// full access 
    if (hService == NULL)
		{ 
        SystemErrorMessage(S("OpenService"));
        CloseServiceHandle(hSCManager);
        return FALSE;
		}    

    // Check the status in case the service is not stopped. 
    if (!QueryServiceStatusEx( 
            hService,                       // handle to service 
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ) )              // size needed if buffer is too small
		{
        SystemErrorMessage(S("QueryServiceStatusEx @1"));
        CloseServiceHandle(hService); 
        CloseServiceHandle(hSCManager);
        return FALSE; 
		}

    // Check if the service is already running.  
    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
		{
		if (pbAlreadyRunning != NULL)
			*pbAlreadyRunning = TRUE;
        CloseServiceHandle(hService); 
        CloseServiceHandle(hSCManager);
        return FALSE;
		}

	// Wait for the service to stop, if necessary
    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
		{

        // Save the tick count and initial checkpoint.

        dwStartTickCount = GetTickCount();
        dwOldCheckPoint = ssStatus.dwCheckPoint;

        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep( dwWaitTime );

        // Check the status until the service is no longer stop pending. 
 
        if (!QueryServiceStatusEx( 
                hService,                       // handle to service 
                SC_STATUS_PROCESS_INFO,         // information level
                (LPBYTE) &ssStatus,             // address of structure
                sizeof(SERVICE_STATUS_PROCESS), // size of structure
                &dwBytesNeeded ) )              // size needed if buffer is too small
			{
            SystemErrorMessage(S("QueryServiceStatusEx @2"));
            CloseServiceHandle(hService); 
            CloseServiceHandle(hSCManager);
            return FALSE; 
			}

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
			{
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
			}
        else
			{
            if (GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
				{
				wprintf_ub(S("ERROR: Timeout waiting for CSCM service to stop\n"));
                CloseServiceHandle(hService); 
                CloseServiceHandle(hSCManager);
                return FALSE; 
				}
			}
    }

	// Attempt to start the service.
    if (!StartService(
            hService,				// handle to service 
            u32ServerArgc,			// number of arguments, NOT including the service name
            abszServerArgv) )       // arg strings
		{
        SystemErrorMessage(S("StartService"));
        CloseServiceHandle(hService); 
        CloseServiceHandle(hSCManager);
        return FALSE; 
		}

    // Check the status until the service is no longer start pending. 
    if (!QueryServiceStatusEx( 
            hService,                       // handle to service 
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ))              // if buffer too small
		{
        SystemErrorMessage(S("QueryServiceStatusEx @3"));
        CloseServiceHandle(hService); 
        CloseServiceHandle(hSCManager);
        return FALSE; 
		}

    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
		{ 

        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 
 
        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep (dwWaitTime);

        // Check the status again. 
 
        if (!QueryServiceStatusEx( 
            hService,                       // handle to service 
            SC_STATUS_PROCESS_INFO,         // info level
            (LPBYTE) &ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded ))              // if buffer too small
			{
            SystemErrorMessage(S("QueryServiceStatusEx @4"));
            break; 
			}
 
        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
			{
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
			}
        else
			{
            if (GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
				{
                // No progress made within the wait hint.
                break;
				}
        }
    } 

    bRetVal = ssStatus.dwCurrentState == SERVICE_RUNNING;

    CloseServiceHandle(hService); 
    CloseServiceHandle(hSCManager);

	return bRetVal;

	} /* StartServerService */

// -------------------------------------------------------------------------
//
// void SystemErrorMessage
//
// Description: Prints the message for the last system error
//
// -------------------------------------------------------------------------
static void SystemErrorMessage(STRING bszFunctionName)
	{

	STRING bszBuffer = NULL;

	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL, 
                       GetLastError(),
                       0,
                       (LPTSTR) &bszBuffer, 
                       0, 
                       NULL))
		{
		wprintf_ub(S("ERROR: FormatMessage failed with 0x%08X\n"), GetLastError());
        return;
		}
	else
		{
		wprintf_ub(S("\nERROR: ") B_SF S(" returned '"), bszFunctionName);
		if (bszBuffer != NULL)
			wprintf_ub(bszBuffer);
		wprintf_ub(S("'\n"));
		LocalFree(bszBuffer);
		}

	} /* SystemErrorMessage */

#endif

