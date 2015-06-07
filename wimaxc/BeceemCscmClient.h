/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------------------------
// Header file for BeceemCscmClient.h, which is the main module for the
// Client application
// -------------------------------------------------------------------------

#define CLIENT_DEBUG_OFF   0
#define CLIENT_DEBUG_ERROR 1
#define CLIENT_DEBUG_INFO  2
#define CLIENT_DEBUG_ALL   3

#define CLIENT_DEFAULT_DEBUG_LEVEL   CLIENT_DEBUG_OFF
#define CLIENT_DEFAULT_SERVER_PORT   5200
#define CLIENT_DEFAULT_SERVER_IP     S("127.0.0.1")

typedef struct {
	BOOL u32DebugLevel;
	STRING bszServerIPAddress;
	UINT32 u32ServerPort;
	BOOL bNoServerStart;
	STRING bszServerConfigFile;
	} stWcmClientParam_t;

void ClientDebugPrint(int iDebugLevel, STRING bszFormat, ...);
void ClientReceivedPrint(int iDebugLevel, STRING bszMessage);


