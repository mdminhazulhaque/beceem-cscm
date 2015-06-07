/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "CscmTypes.h"
#include "AuxStruct.h"
#include "PrintContext.h"

// Interval for periodic link status requests (ms)
#define DEFAULT_LINK_STATUS_INTERVAL_SEC		2

// Intervals for auto connect and disconnect
#define DEFAULT_AUTO_CONNECT_INTERVAL_SEC       20
#define DEFAULT_AUTO_DISCONNECT_INTERVAL_SEC    20

#define DEFAULT_RADIUS_IP_ADDRESS				S("192.168.100.1")
#define DEFAULT_RADIUS_PORT						1812

#define DEFAULT_ANONYMOUS_IDENTITY				S("anonymous")
#define DEFAULT_EAP_POLLING_INTERVAL_MS			50
#define DEFAULT_AUTHENTICATION_TIMEOUT_SEC		10
#define DEFAULT_EAP_FRAGMENT_MAX_LENGTH			1398

#define	DEFAULT_NET_SEARCH_TIMEOUT_SEC			60
#define DEFAULT_NET_ENTRY_TIMEOUT_SEC           10

#define DEFAULT_FIRMWARE_PRIVATE_KEY_BITS       2048

#ifdef WIN32

#define DEFAULT_CA_CERT_PATH           		S("C:\\Beceem\\Certs") 
#define DEFAULT_CA_CERT_FILEPATH            S("C:\\Beceem\\Certs\\ca_cert.pem")
#define DEFAULT_DEVICE_CERT_FILEPATH        S("C:\\Beceem\\Certs\\device_cert.pem")
#define DEFAULT_DEVICE_PRIVATE_KEY_FILEPATH S("C:\\Beceem\\Certs\\device_private_key.pem")

#define DEFAULT_BECEEM_ENGINE_FILEPATH		S("C:\\Beceem\\CM\\App\\Server\\engine_beceem.dll")
#define DEFAULT_IP_REFRESH_COMMAND			EMPTY_STRING
#define DEFAULT_FIRMWARE_FILEPATH			EMPTY_STRING
#define DEFAULT_CONFIG_FILEPATH				EMPTY_STRING

#define DEFAULT_CSCM_DEBUG_LOG_FILEPATH     S("C:\\Beceem\\CM\\CM_Server_Debug.log")
#define DEFAULT_AUTH_LOG_FILEPATH           S("C:\\Beceem\\CM\\CM_Auth.log")
#define DEFAULT_ENGINE_LOG_FILEPATH			S("C:\\Beceem\\CM\\CM_Engine.log")
#define DEFAULT_MAC_LOG_FILEPATH            S("C:\\Beceem\\CM\\CM_MacMsg.dat")

#endif
#ifdef LINUX_OR_OSX

#define DEFAULT_CA_CERT_PATH            	S("/lib/firmware")	
#define DEFAULT_CA_CERT_FILEPATH            S("/lib/firmware/ca_cert.pem")
#define DEFAULT_DEVICE_CERT_FILEPATH        S("/lib/firmware/device_cert.pem")
#define DEFAULT_DEVICE_PRIVATE_KEY_FILEPATH S("/lib/firmware/device_private_key.pem")

#define DEFAULT_BECEEM_ENGINE_FILEPATH		S("/lib/engine_beceem.so")
#define DEFAULT_IP_REFRESH_COMMAND			S("udhcpc --now --quit -i veth0 &")
#define DEFAULT_FIRMWARE_FILEPATH			S("/lib/firmware/macxvi200.bin")
#define DEFAULT_CONFIG_FILEPATH				S("/lib/firmware/macxvi.cfg")

#define DEFAULT_CSCM_DEBUG_LOG_FILEPATH     S("/tmp/CM_Server_Debug.log")
#define DEFAULT_AUTH_LOG_FILEPATH           S("/tmp/CM_Auth.log")
#define DEFAULT_ENGINE_LOG_FILEPATH			S("/tmp/CM_Engine.log")
#define DEFAULT_MAC_LOG_FILEPATH            S("/tmp/CM_MacMsg.dat")

#endif

#define DEFAULT_PRIVATE_KEY_FORMAT			S("Ascii")

#define CSCM_DEBUG_LEVEL_OPTION_NAME  S("CSCMDebugLogLevel")

#define DEVICE_MEM_SLOT_NAME_STRING				"DeviceMemSlot" 
#define strlenof_DEVICE_MEM_SLOT_NAME_STRING 	b_strlen(DEVICE_MEM_SLOT_NAME_STRING)
#define DEIVCE_MEM_SLOT_ID_MIN_VALUE			1
#define DEIVCE_MEM_SLOT_ID_MAX_VALUE			4


#define BW5MHZ     5000
#define BW7MHZ     7000
#define BW8P75MHZ  8750
#define BW10MHZ   10000

#define DEFAULT_STRING_CHAR_SIZE 128
#define MAX_NUMBER_OF_COMMAND_ARGS 100

typedef enum {eNONE, eBOOL, eUINT32, eARRAY_UINT32, eSTRING, eFILENAME, eSTRUCT} eOptionType_t;

typedef struct {
	UINT32 u32MaxChar;
	UINT32 u32CurChar;
	STRING bszValue;
	} stString_t;

typedef struct {
	UINT32 u32MaxLength;
	UINT32 u32CurLength;
	UINT32 *au32Values;
	} stArrayUint32_t;

typedef union {
	BOOL			bVal;
	UINT32			u32Val;
	stString_t		stString;
	stArrayUint32_t stArray;
	} uOptionValue_t;

typedef struct {
	STRING			bszName;
	eOptionType_t   eType;
	BOOL			bChanged;
	uOptionValue_t  uVal;
	} stOptionEntry_t;

typedef struct {
	STRING			bszName;
	eOptionType_t	eType;
	UINT32			u32Count;
	} stOptionDef_t;

typedef stOptionEntry_t *hOptionTable_t;

hOptionTable_t Options_CreateTable(void);
void Options_DestroyTable(hOptionTable_t hTable);
void Options_SetDefaultOptionValues(hOptionTable_t hTable);

stOptionEntry_t * Options_FindEntry(hOptionTable_t hTable, const STRING bszFieldName);

BOOL Options_LoadOptionsFromFile(hOptionTable_t hTable, const STRING bszInitFilePath, PrintContext_t hPrint,
								 UINT32 *pu32ErrorCount);
BOOL Options_SaveOptionsToFile(hOptionTable_t hTable, const STRING bszInitFilePath, PrintContext_t hPrint);
STRING Options_SetOptionByName(hOptionTable_t hTable, 
							 const STRING bszFieldName, STRING bszArgs[], UINT32 u32NumberOfArgs,
							 PrintContext_t hPrint);
BOOL Options_GetOptionByName(hOptionTable_t hTable, const STRING bszFieldName, PrintContext_t hPrint);

BOOL   Options_GetBoolByEntry  (stOptionEntry_t *pstEntry, BOOL *pbError);
UINT32 Options_GetUint32ByEntry(stOptionEntry_t *pstEntry, BOOL *pbError);
STRING Options_GetStringByEntry(stOptionEntry_t *pstEntry, BOOL *pbError);
BOOL   Options_IsEmptyStringByEntry(stOptionEntry_t *pstEntry, BOOL *pbError);
UINT32 *Options_GetUint32ArrayByEntry(stOptionEntry_t *pstEntry, UINT32 *pu32Size, BOOL *pbError);

BOOL   Options_GetBoolByName   (hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError);
UINT32 Options_GetUint32ByName (hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError);
STRING Options_GetStringByName (hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError);
UINT32 *Options_GetUint32ArrayByName(hOptionTable_t hTable, const STRING bszFieldName, 
							 UINT32 *pu32Size, BOOL *pbError);

BOOL Options_SetBoolByEntry  (stOptionEntry_t *pstEntry, BOOL bValue);
BOOL Options_SetUint32ByEntry(stOptionEntry_t *pstEntry, UINT32 u32Value);
BOOL Options_SetStringByEntry(stOptionEntry_t *pstEntry, STRING bszValue);
BOOL Options_SetUint32ArrayByEntry(stOptionEntry_t *pstEntry, UINT32 au32Values[], UINT32 u32Size);

BOOL Options_SetBoolByName   (hOptionTable_t hTable, const STRING bszFieldName, BOOL bValue);
BOOL Options_SetUint32ByName (hOptionTable_t hTable, const STRING bszFieldName, UINT32 u32Value);
BOOL Options_SetStringByName (hOptionTable_t hTable, const STRING bszFieldName, STRING bszValue);
BOOL Options_SetUint32ArrayByName(hOptionTable_t hTable, const STRING bszFieldName, 
					UINT32 au32Values[], UINT32 u32Size);

BOOL Options_SetNetSearchParameters(hOptionTable_t hTable, hNetSearchParameters_t hNetSearch);
BOOL Options_GetNetSearchParameters(hOptionTable_t hTable, hNetSearchParameters_t hNetSearch);

BOOL Options_IsFileOnDeviceMem(STRING name);
UINT32 Options_ValidateAndGetSlotNumber(STRING name);

// Not currently used
BOOL Options_IsValidIPAddress (STRING bszField);


#endif

