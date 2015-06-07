/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "BeceemCscmCommon.h"
#include "Utility.h"

// Must appear after BeceemCscmCommon.h
#ifdef LINUX_OR_OSX
#include <signal.h>
#endif

static void Abort(int iStatus);

/* For reference only

typedef struct {
	UINT32 u32ID;
	STRING bszKeyword;
	UINT32 u32TimeoutCommandMs;
	UINT32 u32TimeoutEventMs;
	UINT32 u32TimeoutEventID;
	STRING bszArgHelp;
	} stCmdInformation_t;

*/

static stCmdInformation_t astCmdInfo[] = {

// Note: First timeout value is the client command timeout
//       Second timeout value is the API event timeout

	{ID_MSG_BYE,				S("bye"),			30000,	0,	ID_EVENT_INVALID,       S("Immediately exit\n")},
	{ID_MSG_HELP,				S("help"),		10000,	0,	ID_EVENT_INVALID,       S("Print command help summary\n")},
	{ID_MSG_SERVER,				S("server"),		10000,  0,	ID_EVENT_INVALID,	
		S("server      : Identify the Beceem CM Server\n")
		S("server stop : Terminate the server and the client\n")},
	{ID_MSG_VERSIONS,			S("versions"),	10000,  0,		ID_EVENT_INVALID,	S("Print software and IC versions\n\n")},

	{ID_MSG_GET,				S("get"),			10000,	0,	ID_EVENT_INVALID,				
		S("get          : Get the values of all options\n")
		S("get <name>   : Get the value of the option <name>\n")
		S("get <string> : Get the values of the options whose names\n")
		S("               contain the substring <string>\n")},
	{ID_MSG_SET,				S("set"),			10000,	0,	ID_EVENT_INVALID,												
		S("set <name> <value> ...   : Set the value(s) of the option <name>\n")
		S("set <string> <value> ... : Set the value(s) of the option whose name\n")
		S("                           contains the substring <string>\n")},
	{ID_MSG_OPTIONS,			S("options"),		10000,  0,  ID_EVENT_INVALID,				
		S("options file        : Display the default options file path\n")
		S("options file <file> : Change the default options file to <file path>\n")
		S("options load        : Load options from the default options file\n")
		S("options save        : Save options to the default options file\n")
		S("options load <file> : Load options from file <file path>\n")
		S("options save <file> : Save options to file <file path>\n\n")},

	// Note: Network search timeout for the server is set by the Options mechanism
	{ID_MSG_NETWORK_SEARCH,     S("search"),      300000,	0,      ID_EVENT_NET_SEARCH_TIMEOUT,   
		S("search              : Run a network search and wait for the results\n")
	    S("search wait         : Run a network search and wait for the results\n")
		S("search nowait       : Start a network search and don't wait for the results\n")
		S("                      Use 'search status' to retrieve the search results\n")
		S("search capl         : start a network search using CAPL parameters and wait for the results\n")
		S("search capl wait    : start a network search using CAPL parameters and wait for the results\n")
		S("search capl nowait  : start a network search using CAPL parameters and wait for the results\n")
		S("search stop         : Stop the current network search\n")
		S("search status       : Print the results of the last network search\n\n")},

	{ID_MSG_CONNECT,			S("connect"),		20000,  0,	    ID_EVENT_NET_ENTRY_TIMEOUT,	  
		S("connect <BS search index> : Connect to base station with search index <index>\n")
		S("connect <CF-MHz> <BW-MHz> : Connect to base station with the specified\n")
		S("                            center frequency and bandwidth, in MHz\n")},
	{ID_MSG_DISCONNECT,			S("disconnect"),	20000,  10000,	ID_EVENT_NET_DEREG_TIMEOUT,	 S("Disconnect from the current base station\n\n")},

	{ID_MSG_DHCP,				S("dhcp"),		70000,  60000,  ID_EVENT_INVALID,            S("Run IP refresh command (Linux only)\n\n")},

	{ID_MSG_BASEBAND_STATUS,	S("status"),		10000,  10000,  ID_EVENT_BASEBAND_STATUS_TIMEOUT, S("Print connection status\n")},
	{ID_MSG_CM_STATS,			S("cmstats"),		10000,	0,	    ID_EVENT_INVALID,  S("Print connection manager event counts\n")},
	{ID_MSG_LINK_STATS,			S("linkstats"),   10000,  0,		ID_EVENT_INVALID,            
		S("linkstats             : Print basic baseband link statistics\n")
		S("linkstats all         : Print all available baseband link statistics\n")
		S("linkstats <type1> ... : Print statistics for the listed types.\n")
		S("                        Valid types are\n")
		S("                             basic  : basic statistics\n")
		S("                             phy    : PHY statistics\n")
		S("                             sftype : service flow type statistics\n")},
	{ID_MSG_RESET_LINK_STATS,   S("resetstats"),  10000,  0,		ID_EVENT_INVALID,            
	    S("resetstats                : Reset basic baseband link statistics counters\n")
		S("resetstats all            : Reset all available statistics counters\n")
		S("resetstats <type1> ...    : Reset statistics for the listed types,\n")
		S("                            as defined for linkstats\n\n")},

	{ID_MSG_SYNC_UP,			S("syncup"),      10000,  30000,	ID_EVENT_SYNC_UP_TIMEOUT,    S("Acquire sync with a base station\n")},
	{ID_MSG_SYNC_DOWN,			S("syncdown"),    10000,  30000,	ID_EVENT_SYNC_DOWN_TIMEOUT,  S("End sync with the current base station\n")},
	{ID_MSG_IDLE_MODE,			S("idlemode"),    10000,  30000,  ID_EVENT_IDLEMODE_TIMEOUT,      
		S("idlemode enter : Send request to enter idle mode\n")
		S("idlemode leave : Leave idle mode\n")},
	{ID_MSG_SHUT_DOWN,			S("shutdown"),    10000,  30000,	ID_EVENT_SHUTDOWN_TIMEOUT,		
		S("shutdown				: Shut down the baseband chip immediately\n")
		S("shutdown	lpsearch	: Shut down the baseband and start the periodic low power scan\n\n")},

	{ID_MSG_RESET_CHIP,         S("resetchip"),   10000,  10000,  ID_EVENT_CHIPSET_RESET_TIMEOUT,	S("Reset baseband chip\n")},
	{ID_MSG_CF_DOWNLOAD,        S("cfdownload"),  70000,  60000,  ID_EVENT_FW_DOWNLOAD_TIMEOUT,	S("Download config file to baseband chip (Linux only)\n")},
	{ID_MSG_FW_DOWNLOAD,		S("fwdownload"),  70000,  60000,	ID_EVENT_FW_DOWNLOAD_TIMEOUT,	S("Download the firmware binary to baseband chip\n")},

	// End alphabetical order by keyword

	// Internal use only (starting at ID = 100)
	{ID_MSG_START_INTERNAL,     S(""),            10000,  0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Start internal IDs"},
	{ID_MSG_NOP,				S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: No operation"},
	{ID_MSG_INFO,               S(""),            10000,  0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Information message"}, 
	{ID_MSG_WARNING,			S(""),			10000,  0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Warning message"}, 
	{ID_MSG_ERROR,				S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Error message"},

	{ID_MSG_WELCOME,			S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Welcome message"},
	{ID_MSG_PROMPT,				S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Command prompt"},
	{ID_MSG_INVALID_COMMAND,	S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Invalid command"},
	{ID_MSG_INVALID_KEYWORD,	S(""),			10000,	0,	ID_EVENT_INVALID,	NULL},  // Internal use only: Invalid keyword"},
	{ID_MSG_SERVER_BUSY,		S(""),			10000,  0,  ID_EVENT_INVALID,   NULL},  // Internal use only: Sever busy message"},
	{ID_MSG_GET_EMSK,			S("emsk"),		10000,  0,  ID_EVENT_INVALID,   S("Get EMSK generated after last successful authentication\n")},  // GET EMSK"},
	{ID_LAST_ENTRY,				NULL,			0,		0,	ID_EVENT_INVALID,	NULL}   //  Internal use only: Last table entry
	};

stCmdInformation_t *g_astCmdInfo = astCmdInfo;

// --------------------------------------------------------------------------------
// bszCommand is modified by this function
UINT32 ParseCommandLine(STRING bszCommand, 
					STRING abszArgStrings[], UINT32 u32MaxNumArgs, UINT32 *pu32ActualNumArgs)
	{

	STRING bszToken, bszDelim;
	UINT32 n;
	int iDelimCnt;
	B_CHAR wcQuoteChar;
	UINT32 u32KeyWordID = ID_MSG_NOP;

	bszDelim = bszCommand;

	for (n=0; n<u32MaxNumArgs; n++)
		{

		// Skip any delimiters to find the start of the token
		if (bszDelim != NULL)
			{
			iDelimCnt = (int) b_strspn(bszDelim, DELIMITERS);
			bszToken = &bszDelim[iDelimCnt];
			}
		else
			bszToken = NULL;

		// Set the final arg count
		if (bszToken == NULL || *bszToken == B_NUL)
			{
			if (pu32ActualNumArgs != NULL)
				*pu32ActualNumArgs = n;	
			break;
			}

		// Quoted tokens
		else if (*bszToken == C('\'') || *bszToken == C('"'))
			{
			wcQuoteChar = *bszToken++;
			bszDelim = b_strchr(bszToken, wcQuoteChar);
			}

		// Non-quoted tokens
		else
			{
			if (n == 0)
				u32KeyWordID = CommandID(bszToken);
			bszDelim = b_strpbrk(bszToken, DELIMITERS);
			}

		// NUL terminate the token
		if (bszDelim != NULL)
			{
			*bszDelim = B_NUL;
			bszDelim++;
			}

		// Save the token
		if (abszArgStrings != NULL)
			abszArgStrings[n] = bszToken;

		} // for (n=0; ... 
	
	return u32KeyWordID;

	} /* ParseCommandLine */

// bszCommand is not modified by this function
UINT32 CommandID(const STRING bszCommand)
	{
	STRING bszToken;
	UINT32 n;
	B_CHAR *bszNextToken;
	BOOL bValidKeyWord = FALSE;
	UINT32 u32KeyWordID = ID_MSG_NOP;
	B_CHAR bszKeyword[MAX_KEYWORD_CHARS];

	// Check for NULL pointer
	if (bszCommand == NULL)
		return ID_MSG_INVALID_COMMAND;

	// Make a local copy
	b_bcsncpy(bszKeyword, bszCommand, MAX_KEYWORD_CHARS);
	bszKeyword[MAX_KEYWORD_CHARS-1] = B_NUL;

	// Process the keyword
	bszToken = b_strtok(bszKeyword, DELIMITERS, &bszNextToken);

	if (bszToken == NULL)

		u32KeyWordID = ID_MSG_INVALID_COMMAND;

	else
		{
		for (n=0; g_astCmdInfo[n].u32ID != ID_LAST_ENTRY && !bValidKeyWord; n++)
			if (BcsCmpNoCase(bszToken, g_astCmdInfo[n].bszKeyword))
				{
				bValidKeyWord = TRUE;
				u32KeyWordID = g_astCmdInfo[n].u32ID;
				}

		if (!bValidKeyWord)
			u32KeyWordID = ID_MSG_INVALID_KEYWORD;
		}

	return u32KeyWordID;

	} /* CommandID */

// --------------------------------------------------------------------------------
// stCmdInformation_t *GetCmdInfo(UINT32 u32ID)
//
// Description: Gets the command information structure for a particular ID
//
// Input parameters:
//		u32ID	- Command ID
//
// Returns: pointer to structure, if successful; NULL, on error
// --------------------------------------------------------------------------------

stCmdInformation_t *GetCmdInfo(UINT32 u32ID) 
	{

	int n;

	for (n=0; g_astCmdInfo[n].u32ID < ID_LAST_ENTRY; n++)
		{
		if (g_astCmdInfo[n].u32ID == u32ID)
			return &g_astCmdInfo[n];
		}

	return NULL;

	} /* GetCmdInfo */


#ifdef LINUX_OR_OSX

static void Abort(int iStatus)
	{
	exit(iStatus);
	}

void InstallSignalHandlers(void)
	{

	signal(SIGHUP,  SIG_IGN);
	signal(SIGINT,  Abort);
	signal(SIGTERM, Abort);

	} /* InstallSignalHandlers */

#endif
