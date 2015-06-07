/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef CSCM_TYPES_H
#define CSCM_TYPES_H

#include "CscmString.h"

#ifdef LINUX
#define LINUX_OR_OSX
#else
#ifdef  OSX
#define LINUX_OR_OSX
#endif
#endif

#ifdef LINUX_OR_OSX
typedef	unsigned char  BOOL;
#endif
#ifdef WIN32
typedef          int   BOOL;
#endif

#ifndef BCM_TYPEDEFS_H
typedef signed   int   INT32;
typedef unsigned int   UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;
typedef unsigned long  ULONG;
#endif

typedef signed short   INT16;
typedef UINT8		   u8;   // for supplicant only
typedef UINT32		   u32;

#define TRUE 1
#define FALSE 0

enum {
	CLIENT_NOP,
	CLIENT_QUIT,
	CLIENT_TERMINATE,
	OTHER_TERMINATE
	};

enum {
	// This first group does not require a Beceem device present
	ID_MSG_BYE = 0,				
	ID_MSG_HELP,
	ID_MSG_SERVER,
	ID_MSG_GET,
	ID_MSG_SET,
	ID_MSG_CM_STATS,

	// Starting here, a device is required
	ID_MSG_NETWORK_SEARCH = 20,	
	ID_MSG_CONNECT,
	ID_MSG_DISCONNECT,
	ID_MSG_DHCP,
	ID_MSG_LINK_STATS,		
	ID_MSG_RESET_LINK_STATS,

	ID_MSG_BASEBAND_STATUS,
	ID_MSG_SYNC_UP,
	ID_MSG_SYNC_DOWN,		
	ID_MSG_SHUT_DOWN,
	ID_MSG_RESET_CHIP,
	ID_MSG_CF_DOWNLOAD,
	ID_MSG_FW_DOWNLOAD,

	ID_MSG_VERSIONS,
	ID_MSG_IDLE_MODE,
	ID_MSG_OPTIONS,

	// All the following are for internal use only 
	ID_MSG_START_INTERNAL = 100,

	ID_MSG_NOP,	    
	ID_MSG_INFO,
	ID_MSG_WARNING,
	ID_MSG_ERROR,

	ID_MSG_WELCOME,			
	ID_MSG_PROMPT,
	ID_MSG_INVALID_COMMAND,
	ID_MSG_INVALID_KEYWORD,

	ID_MSG_SERVER_BUSY,

	// Returned by the server when 'server stop' is received
	ID_MSG_TERMINATE,
	ID_MSG_GET_EMSK, //to get EMSK.
	ID_LAST_ENTRY			
	};

// Response status masks
enum {
	ID_EVENT_NET_SEARCH_TIMEOUT = 0,
	ID_EVENT_SYNC_UP_TIMEOUT,
	ID_EVENT_SYNC_DOWN_TIMEOUT,
	ID_EVENT_NET_ENTRY_TIMEOUT,
	ID_EVENT_NET_DEREG_TIMEOUT,
	ID_EVENT_BASEBAND_STATUS_TIMEOUT,
	ID_EVENT_CHIPSET_RESET_TIMEOUT,
	ID_EVENT_FW_DOWNLOAD_TIMEOUT,
	ID_EVENT_SHUTDOWN_TIMEOUT,
	ID_EVENT_NET_ENTRY_FAILURE,
	ID_EVENT_WIMAX_OPTIONS_TIMEOUT,
	ID_EVENT_EAP_ENC_PRIV_TIMEOUT,
	ID_EVENT_IDLEMODE_TIMEOUT,
	ID_EVENT_INVALID};

typedef struct {
	UINT32 u32ID;
	STRING bszKeyword;
	UINT32 u32TimeoutCommandMs;
	UINT32 u32TimeoutEventMs;
	UINT32 u32TimeoutEventID;
	STRING bszArgHelp;
	} stCmdInformation_t;

#endif

