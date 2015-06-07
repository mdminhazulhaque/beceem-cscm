/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "BeceemAPI.h"
#include "BeceemCscmCommon.h"
#include "Utility.h"
#include "Options.h"
#include "SysLogFunctions.h"

static UINT32 g_u32NumberOfOptions = 0;

static stOptionDef_t g_astOptionDefs[] = {

	// Network search parameters
	{S("BandwidthMHz"),					eARRAY_UINT32,	MAX_NUM_BANDWIDTHS_SUPPORTED},
	{S("ScanMinFrequencyMHz"),			eARRAY_UINT32,  1},
	{S("ScanMaxFrequencyMHz"),			eARRAY_UINT32,  1},
	{S("ScanStepFrequencyMHz"),			eARRAY_UINT32,  1},
	{S("CenterFrequencyMHz"),			eARRAY_UINT32,	MAX_NUM_DISCRETE_CF_SUPPORTED},	
	{S("NetworkSearchTimeoutSec"),		eUINT32,	0},
	{S("LPSearchInShutDownEnabled"),	eBOOL,		0},
	// Network entry timeout
	{S("NetworkEntryTimeoutSec"),       eUINT32,    0},
	{S("NEToHighestCINRBaseStation"),	eBOOL,		0},

	// Authentication
	{S("AuthEnabled"),					eBOOL,		0},
	{S("EAPMethod"),					eUINT32,    0},
	{S("UserIdentity"),					eSTRING,    0},
	{S("UserPassword"),					eSTRING,    0},
	{S("TTLSAnonymousIdentity"),		eSTRING,    0},
	{S("ValidateServerCert"),           eBOOL,      0},
	{S("CACertPath"),					eFILENAME,  0}, 
	{S("CACertFileName"),				eFILENAME,  0},
	{S("TLSDeviceCertFileName"),		eFILENAME,  0},
	{S("TLSDeviceSubCA1CertFileName"),	eFILENAME,	0}, 
	{S("TLSDeviceSubCA2CertFileName"), 	eFILENAME,	0},
	{S("TLSDevicePrivateKeyFileName"),	eFILENAME,  0},
	{S("TLSDevicePrivateKeyPassword"),	eSTRING,    0},
	{S("PrivateKeyPasswordFormat"),		eSTRING,    0},
	{S("AuthenticationTimeoutSec"),     eUINT32,    0},
	{S("InvertMSKByteOrder"),			eBOOL,      0},
	{S("AlwaysIncludeTLSLength"),		eBOOL,      0},
	{S("EAPFragmentMaxLength"),			eUINT32,	0},
	{S("EAPPollingLoopIntervalMs"),		eUINT32,    0},

	// Beceem cryptographic engine
	{S("FirmwareRSAPrivateKeyEncrypt"), eBOOL,		0},
	{S("FirmwarePrivateKeyBits"),       eUINT32,    0},
	{S("InnerNAIChange"),               eBOOL,      0},
	{S("BeceemEngineFileName"),			eFILENAME,	0},

	// Authentication test mode (over Ethernet)
	{S("AuthEthernetToRADIUS"),			eBOOL,      0},
	{S("RADIUSIPAddress"),				eSTRING,    0},
	{S("RADIUSPort"),					eUINT32,    0},
	{S("RADIUSSecret"),					eSTRING,    0},

	// Automatic connect and/or disconnect
	{S("AutoReConnectEnabled"),			eBOOL,		0},
	{S("AutoReDisconnectEnabled"),		eBOOL,      0},
	{S("SkipNetSearch"),				eBOOL,		0},
	{S("AutoReConnectIntervalSec"),		eUINT32,    0},
	{S("AutoReDisconnectIntervalSec"),	eUINT32,    0},

	// Link status request period
	{S("LinkStatusRequestPeriodSec"),	eUINT32,    0},

#ifdef LINUX
	// IP refresh (Linux only)
	{S("IPRefreshCommand"),				eSTRING,    0},
	{S("NetEntryIPRefreshEnabled"),		eBOOL,		0},
	{S("TerminateDHCPClient"),			eBOOL,		0},
#endif

	// Firmware and config files
	{S("FirmwareFileName"),				eFILENAME,  0},
	{S("ConfigFileName"),				eFILENAME,  0},

	// Log file control
	{S("CSCMDebugLogLevel"),			eUINT32,    0},
	{S("CSCMDebugLogFileName"),			eFILENAME,  0},
	{S("CSCMDebugLogFileMaxSizeMB"),	eUINT32,    0}, 

	{S("AuthLogLevel"),					eUINT32,    0},
	{S("AuthLogFileName"),				eFILENAME,  0},
	{S("EnableAuthSysLogPrints"), 		eBOOL,		0},
	{S("AuthLogFileMaxSizeMB"),			eUINT32,    0}, 
	{S("EngineLoggingEnabled"),         eBOOL,      0},
	{S("EngineLogFileName"),			eFILENAME,  0},
	{S("EngineLogFileMaxSizeMB"),		eUINT32,    0}, 
	{S("RADIUSClientLogLevel"),			eUINT32,    0},

	// End of option list
	{NULL,								eNONE,      0}
	};

// =========================================================================================
// Functions to create and destroy the Options table
// =========================================================================================

// -----------------------------------------------------------------------------------------
hOptionTable_t Options_CreateTable(void)
	{

	BOOL bError = FALSE;
	UINT32 n, u32Count, u32OptionCount, u32TableBytes, u32ValueBytes;
	hOptionTable_t hTable;
	stString_t *pstString;
	stArrayUint32_t  *pstArray;

	// Count the number of options
	u32OptionCount = 0;
	for (n = 0; g_astOptionDefs[n].eType != eNONE; n++)
		u32OptionCount++;
	g_u32NumberOfOptions = u32OptionCount;

	// Alocate the table
	u32TableBytes = u32OptionCount * sizeof(stOptionEntry_t);
	if ((hTable = WcmMalloc(u32TableBytes)) == NULL)
		return NULL;

	// Initialize the table, and allocate any necessary buffers
	memset(hTable, 0, u32TableBytes);
	for (n=0; n<u32OptionCount && !bError; n++)
		{
		hTable[n].bszName  = g_astOptionDefs[n].bszName;
		hTable[n].eType    = g_astOptionDefs[n].eType;
		hTable[n].bChanged = FALSE;

		bError = FALSE;
		switch (hTable[n].eType)
			{
			case eFILENAME:	
			case eSTRING:
				u32ValueBytes  = BCHARS2BYTES(DEFAULT_STRING_CHAR_SIZE + 1);
				pstString = &hTable[n].uVal.stString;
				if ((pstString->bszValue = WcmMalloc(u32ValueBytes)) == NULL)
					bError = TRUE;
				else
					{
					pstString->u32CurChar = 0;
					pstString->u32MaxChar = DEFAULT_STRING_CHAR_SIZE;
					memset(pstString->bszValue, 0, u32ValueBytes);
					}
				break;

			case eARRAY_UINT32:
				pstArray = &hTable[n].uVal.stArray;
				u32Count = g_astOptionDefs[n].u32Count;
				if ((pstArray->au32Values = WcmMalloc(u32Count * sizeof(UINT32))) == NULL)
					bError = TRUE;
				else
					{
					pstArray->u32CurLength = 0;
					pstArray->u32MaxLength = u32Count;
					memset(pstArray->au32Values, 0, u32Count * sizeof(UINT32));
					}
				break;

			case eBOOL:
			case eUINT32:
			default:
				break;
			}
		}

	// Set default values
	Options_SetDefaultOptionValues(hTable);

	return hTable;

	} /* Options_CreateTable */

// -----------------------------------------------------------------------------------------
void Options_DestroyTable(hOptionTable_t hTable)
	{

	UINT32 n;
	UINT32 u32Count = g_u32NumberOfOptions;

	if (!hTable)
		return;

	// Free string and array buffers
	for (n=0; n<u32Count; n++)
		{
		if (hTable[n].eType == eSTRING || hTable[n].eType == eFILENAME)
			{
			if (hTable[n].uVal.stString.bszValue != NULL)
				WcmFree(hTable[n].uVal.stString.bszValue);
			}
		else if (hTable[n].eType == eARRAY_UINT32)
			{
			if (hTable[n].uVal.stArray.au32Values != NULL)
				WcmFree(hTable[n].uVal.stArray.au32Values);
			}
		}

	WcmFree(hTable);

	} /* Options_DestroyTable */

// -----------------------------------------------------------------------------------------
void Options_SetDefaultOptionValues(hOptionTable_t hTable) 
	{

	UINT32 n;
	hNetSearchParameters_t hNetSearch;

	// -------------------------------------------------------------------
	// Option entry pointers (for quick access)

	// Network search 
	hNetSearch = NetSearchParameters_Create();
	Options_SetNetSearchParameters(hTable, hNetSearch);
	WcmFree(hNetSearch);
	Options_SetUint32ByName(hTable, S("NetworkSearchTimeoutSec"),		DEFAULT_NET_SEARCH_TIMEOUT_SEC);
	Options_SetBoolByName  (hTable, S("LPSearchInShutDownEnabled"),		FALSE);

	// Network entry
	Options_SetUint32ByName(hTable, S("NetworkEntryTimeoutSec"),		DEFAULT_NET_ENTRY_TIMEOUT_SEC);
	Options_SetUint32ByName(hTable, S("NEToHighestCINRBaseStation"),	FALSE);
	
	// Authentication
	Options_SetBoolByName  (hTable, S("AuthEnabled"),					TRUE);
	Options_SetUint32ByName(hTable, S("EAPMethod"),						0);
	Options_SetStringByName(hTable, S("UserIdentity"),					EMPTY_STRING);
	Options_SetStringByName(hTable, S("UserPassword"),					EMPTY_STRING);
	Options_SetStringByName(hTable, S("TTLSAnonymousIdentity"),			DEFAULT_ANONYMOUS_IDENTITY);
	Options_SetBoolByName  (hTable, S("ValidateServerCert"),			TRUE);
	Options_SetStringByName(hTable, S("CACertPath"),					EMPTY_STRING); 
	Options_SetStringByName(hTable, S("CACertFileName"),				EMPTY_STRING);
	Options_SetStringByName(hTable, S("TLSDeviceCertFileName"),			DEFAULT_DEVICE_CERT_FILEPATH);
	Options_SetStringByName(hTable, S("TLSDeviceSubCA1CertFileName"),	EMPTY_STRING); 
	Options_SetStringByName(hTable, S("TLSDeviceSubCA2CertFileName"),	EMPTY_STRING);
	Options_SetStringByName(hTable, S("TLSDevicePrivateKeyFileName"),	DEFAULT_DEVICE_PRIVATE_KEY_FILEPATH);
	Options_SetStringByName(hTable, S("TLSDevicePrivateKeyPassword"),	EMPTY_STRING);
	Options_SetStringByName(hTable, S("PrivateKeyPasswordFormat"),		DEFAULT_PRIVATE_KEY_FORMAT);
	Options_SetUint32ByName(hTable, S("AuthenticationTimeoutSec"),		DEFAULT_AUTHENTICATION_TIMEOUT_SEC);
	Options_SetBoolByName  (hTable, S("InvertMSKByteOrder"),			FALSE);
	Options_SetBoolByName  (hTable, S("AlwaysIncludeTLSLength"),		FALSE);
	Options_SetUint32ByName(hTable, S("EAPFragmentMaxLength"),			DEFAULT_EAP_FRAGMENT_MAX_LENGTH);
	Options_SetUint32ByName(hTable, S("EAPPollingLoopIntervalMs"),		DEFAULT_EAP_POLLING_INTERVAL_MS);
	
	// Beceem cryptographic engine
	Options_SetBoolByName  (hTable, S("FirmwareRSAPrivateKeyEncrypt"),	FALSE);
	Options_SetUint32ByName(hTable, S("FirmwarePrivateKeyBits"),	    DEFAULT_FIRMWARE_PRIVATE_KEY_BITS);
	Options_SetBoolByName  (hTable, S("InnerNAIChange"),				FALSE);
	Options_SetStringByName(hTable, S("BeceemEngineFileName"),			DEFAULT_BECEEM_ENGINE_FILEPATH);

	// Authentication test mode (over Ethernet)
	Options_SetBoolByName  (hTable, S("AuthEthernetToRADIUS"),			FALSE);
	Options_SetStringByName(hTable, S("RADIUSIPAddress"),				DEFAULT_RADIUS_IP_ADDRESS);
	Options_SetUint32ByName(hTable, S("RADIUSPort"),					DEFAULT_RADIUS_PORT);
	Options_SetStringByName(hTable, S("RADIUSSecret"),					EMPTY_STRING);

	// Automatic connect and/or disconnect

	Options_SetBoolByName  (hTable, S("AutoReConnectEnabled"),			FALSE);
	Options_SetBoolByName  (hTable, S("AutoReDisconnectEnabled"),		FALSE);
	Options_SetBoolByName  (hTable, S("SkipNetSearch"),					FALSE);
	Options_SetUint32ByName(hTable, S("AutoReConnectIntervalSec"),		DEFAULT_AUTO_CONNECT_INTERVAL_SEC);
	Options_SetUint32ByName(hTable, S("AutoReDisconnectIntervalSec"),	DEFAULT_AUTO_DISCONNECT_INTERVAL_SEC);

	// Link status request period
	Options_SetUint32ByName(hTable, S("LinkStatusRequestPeriodSec"),	DEFAULT_LINK_STATUS_INTERVAL_SEC);

#ifdef LINUX
	// IP refresh (Linux only)
	Options_SetStringByName(hTable, S("IPRefreshCommand"),				DEFAULT_IP_REFRESH_COMMAND);
	Options_SetBoolByName(hTable,   S("NetEntryIPRefreshEnabled"),		FALSE);
	Options_SetBoolByName(hTable,   S("TerminateDHCPClient"),			FALSE);
#endif	

	// Firmware and config files
	Options_SetStringByName(hTable, S("FirmwareFileName"),				DEFAULT_FIRMWARE_FILEPATH);
	Options_SetStringByName(hTable, S("ConfigFileName"),				DEFAULT_CONFIG_FILEPATH);

	// Log file control
	Options_SetUint32ByName(hTable, CSCM_DEBUG_LEVEL_OPTION_NAME,		DIAG_NORMAL);
	Options_SetStringByName(hTable, S("CSCMDebugLogFileName"),			DEFAULT_CSCM_DEBUG_LOG_FILEPATH);
	Options_SetUint32ByName(hTable, S("CSCMDebugLogFileMaxSizeMB"),		1); 	
	Options_SetUint32ByName(hTable, S("AuthLogLevel"),					0);
	Options_SetStringByName(hTable, S("AuthLogFileName"),				DEFAULT_AUTH_LOG_FILEPATH);
	Options_SetBoolByName  (hTable, S("EnableAuthSysLogPrints"),		FALSE);	
	Options_SetUint32ByName(hTable, S("AuthLogFileMaxSizeMB"),			1); 
	Options_SetBoolByName  (hTable, S("EngineLoggingEnabled"),			FALSE);
	Options_SetStringByName(hTable, S("EngineLogFileName"),				DEFAULT_ENGINE_LOG_FILEPATH);
	Options_SetUint32ByName(hTable, S("EngineLogFileMaxSizeMB"),		1); 
	Options_SetUint32ByName(hTable, S("RADIUSClientLogLevel"),			0);

	// Reset the 'changed' flags
	for (n=0; n<g_u32NumberOfOptions; n++)
		hTable[n].bChanged = FALSE;

	} /* Options_SetDefaultOptionValues */

// =========================================================================================
// Functions to get option values
// =========================================================================================

// -----------------------------------------------------------------------------------------
BOOL Options_GetBoolByName(hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError)
	{

	BOOL bError = FALSE;
	BOOL bRetVal = FALSE;

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		return FALSE;
	else
		bRetVal = Options_GetBoolByEntry(pstEntry, pbError);

	if (pbError && bError)
		*pbError = TRUE;

	return bRetVal;

	} /* Options_GetBoolByName */

// -----------------------------------------------------------------------------------------
BOOL Options_GetBoolByEntry(stOptionEntry_t *pstEntry, BOOL *pbError)
	{

	BOOL bError  = FALSE;
	BOOL bRetVal = FALSE;

	if (!pstEntry)
		bError = TRUE;
	else if (pstEntry->eType != eBOOL)
		bError = TRUE;
	else
		bRetVal = pstEntry->uVal.bVal;

	if (pbError && bError)
		*pbError = TRUE;

	return bRetVal;

	} /* Options_GetBoolByEntry */

// -----------------------------------------------------------------------------------------
UINT32 Options_GetUint32ByName(hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError)
	{

	BOOL bError      = FALSE;
	UINT32 u32RetVal = (UINT32) -1;

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		bError = TRUE;
	else
		u32RetVal = Options_GetUint32ByEntry(pstEntry, &bError);

	if (pbError && bError)
		*pbError = TRUE;

	return u32RetVal;	

	} /* Options_GetUint32ByName */

// -----------------------------------------------------------------------------------------
UINT32 Options_GetUint32ByEntry(stOptionEntry_t *pstEntry, BOOL *pbError)
	{

	BOOL bError = FALSE;
	UINT32 u32RetVal = (UINT32) -1;

	if (!pstEntry)
		bError = TRUE;
	else if (pstEntry->eType != eUINT32)
		bError = TRUE;
	else
		u32RetVal = pstEntry->uVal.u32Val;

	if (pbError && bError)
		*pbError = TRUE;

	return u32RetVal;

	} /* Options_GetUint32ByEntry */

// -----------------------------------------------------------------------------------------
STRING Options_GetStringByName(hOptionTable_t hTable, const STRING bszFieldName, BOOL *pbError)
	{

	BOOL bError = FALSE;
	STRING bszRetVal = EMPTY_STRING;

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		bError = TRUE;
	else
		bszRetVal = Options_GetStringByEntry(pstEntry, &bError);

	if (pbError && bError)
		*pbError = TRUE;

	return bszRetVal;

	} /* Options_GetStringByName */

// -----------------------------------------------------------------------------------------
STRING Options_GetStringByEntry(stOptionEntry_t *pstEntry, BOOL *pbError)
	{

	BOOL bError = FALSE;
	STRING bszRetVal = EMPTY_STRING;

	if (!pstEntry)
		bError = TRUE;
	else if (pstEntry->eType != eSTRING && pstEntry->eType != eFILENAME)
		bError = TRUE;
	else
		bszRetVal = pstEntry->uVal.stString.bszValue;

	if ((pbError && bError) || bszRetVal == NULL)
		*pbError = TRUE;

	return bszRetVal;

	} /* Options_GetStringByEntry */

BOOL Options_IsEmptyStringByEntry(stOptionEntry_t *pstEntry, BOOL *pbError)
	{

	STRING bszRetVal = Options_GetStringByEntry(pstEntry, pbError);

	if (pbError && *pbError)
		return TRUE;
	else if (bszRetVal[0] == B_NUL)
		return TRUE;
	else
		return FALSE;

	} /* Options_IsNullOrEmptyStringByEntry */

// -----------------------------------------------------------------------------------------
UINT32 *Options_GetUint32ArrayByName(hOptionTable_t hTable, const STRING bszFieldName, 
					UINT32 *pu32Size, BOOL *pbError)
	{

	BOOL bError = FALSE;
	UINT32 *pu32Array = NULL;

	stOptionEntry_t *pstEntry;

	if (pu32Size)
		*pu32Size = 0;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		bError = TRUE;
	else
		pu32Array = Options_GetUint32ArrayByEntry(pstEntry, pu32Size, &bError);

	if (pbError && bError)
		*pbError = TRUE;

	return pu32Array;

	} /* Options_GetUint32ArrayByEntry */

// -----------------------------------------------------------------------------------------
UINT32 *Options_GetUint32ArrayByEntry(stOptionEntry_t *pstEntry, UINT32 *pu32Size, BOOL *pbError)
	{

	BOOL bError = FALSE;
	UINT32 *pu32Array = NULL;

	if (pu32Size)
		*pu32Size = 0;

	if (!pstEntry || !pu32Size)
		bError = TRUE;
	else if (pstEntry->eType != eARRAY_UINT32)
		bError = TRUE;
	else {
		*pu32Size = pstEntry->uVal.stArray.u32CurLength;
		pu32Array = pstEntry->uVal.stArray.au32Values;
		}

	if (pbError && bError)
		*pbError = TRUE;

	return pu32Array;

	} /* Options_GetUint32ArrayByEntry */

// =========================================================================================
// Functions to set option values
// =========================================================================================

// -----------------------------------------------------------------------------------------
BOOL Options_SetBoolByName(hOptionTable_t hTable, const STRING bszFieldName, BOOL bValue)
	{

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		return FALSE;

	return Options_SetBoolByEntry(pstEntry, bValue);

	} /* Options_SetBoolByName */

// -----------------------------------------------------------------------------------------
BOOL Options_SetBoolByEntry(stOptionEntry_t *pstEntry, BOOL bValue)
	{

	if (pstEntry == NULL || pstEntry->eType != eBOOL)
		return FALSE;

	pstEntry->bChanged			  = TRUE;
	pstEntry->uVal.bVal			  = bValue;

	return TRUE;

	} /* Options_SetBoolByEntry */

// -----------------------------------------------------------------------------------------
BOOL Options_SetUint32ByName(hOptionTable_t hTable, const STRING bszFieldName, UINT32 u32Value)
	{

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		return FALSE;
	else
		return Options_SetUint32ByEntry(pstEntry, u32Value);

	} /* Options_NameSetUInt32 */

// -----------------------------------------------------------------------------------------
BOOL Options_SetUint32ByEntry(stOptionEntry_t *pstEntry, UINT32 u32Value)
	{

	if (pstEntry == NULL || pstEntry->eType != eUINT32)
		return FALSE;

	pstEntry->bChanged			  = TRUE;
	pstEntry->uVal.u32Val		  = u32Value;

	return TRUE;

	} /* Options_SetUint32ByEntry */

// -----------------------------------------------------------------------------------------
BOOL Options_SetStringByName(hOptionTable_t hTable, const STRING bszFieldName, STRING bszValue)
	{

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		return FALSE;
	else
		return Options_SetStringByEntry(pstEntry, bszValue);

	} /* Options_SetString */

// -----------------------------------------------------------------------------------------
BOOL Options_SetStringByEntry(stOptionEntry_t *pstEntry, STRING bszValue)
	{

	stString_t *pstString;
	UINT32 u32Len;

	if (!pstEntry || !bszValue)
		return FALSE;

	pstString = &pstEntry->uVal.stString;

	u32Len = (UINT32) b_strlen(bszValue);

	// If new value is longer than current buffer size, WcmFree the old buffer	
	// and allocate a new one
	if (u32Len > pstString->u32MaxChar && pstString->u32MaxChar != 0)
		{

		if (pstString->bszValue != NULL)
			WcmFree(pstString->bszValue);
		pstString->u32MaxChar = 0;
		pstString->u32CurChar = 0;
		
		if ((pstString->bszValue = WcmMalloc(BCHARS2BYTES(u32Len + 1))) == NULL)
			return FALSE;
		else
			{
			pstString->u32CurChar       = 0;
			pstString->u32MaxChar       = u32Len;
			pstString->bszValue[u32Len] = B_NUL;
			}
		}

	// Copy the value
	b_strncpy(pstString->bszValue, bszValue, pstString->u32MaxChar);

	pstString->u32CurChar = u32Len;
	pstEntry->bChanged = TRUE;

	return TRUE;

	} /* Options_SetStringByEntry */

// -----------------------------------------------------------------------------------------
BOOL Options_SetUint32ArrayByName(hOptionTable_t hTable, const STRING bszFieldName, 
					UINT32 au32Values[], UINT32 u32Size)
	{

	stOptionEntry_t *pstEntry;

	if ((pstEntry = Options_FindEntry(hTable, bszFieldName)) == NULL)
		return FALSE;
	else
		return Options_SetUint32ArrayByEntry(pstEntry, au32Values, u32Size);

	} /* Options_SetUint32ArrayByName */

// -----------------------------------------------------------------------------------------
BOOL Options_SetUint32ArrayByEntry(stOptionEntry_t *pstEntry, UINT32 au32Values[], UINT32 u32Size)
	{

	stArrayUint32_t *pstArray;

	if (!pstEntry || !au32Values)
		return FALSE;

	pstArray = &pstEntry->uVal.stArray;

	if (u32Size > pstArray->u32MaxLength)
		return FALSE; // Arrays cannot grow beyond the initially allocated size

	memcpy(pstArray->au32Values, au32Values, u32Size*sizeof(UINT32));
	// Remove duplicates and sort values in ascending order
	pstArray->u32CurLength = ArraySortUint32(pstArray->au32Values, u32Size);

	return TRUE;

	} /* Options_SetUint32ArrayByEntry */

// =========================================================================================
// Function to load option values from a file
// =========================================================================================

// -----------------------------------------------------------------------------------------
BOOL Options_LoadOptionsFromFile(hOptionTable_t hTable, const STRING bszInitFilePath, PrintContext_t hPrint, 
								 UINT32 *pu32ErrorCount)
	{

	B_CHAR  bszLine[1024];
	STRING abszArgs[MAX_NUMBER_OF_COMMAND_ARGS];
	UINT32 u32CharCount, u32CmdID, u32ActualNumArgs, u32ErrorCount = 0;
	INT32  i32BlockLevel = 0;
	BOOL bSkipCommands = FALSE, bStatus = FALSE;
	B_CHAR bcQuoteChar = B_NUL;
	int n, iLen;
	FILE *hInfile;
	B_CHAR bc;

	if ((hInfile = BcsFopen(bszInitFilePath, S("rt"))) == NULL)
		{
		Print_Formatted(hPrint, S("ERROR: Can't read options file '") B_SF S("'\n"), bszInitFilePath);
		u32ErrorCount++;
		SysLogWPrintf(SYSLOG_ERROR, S("Can't read options file '") B_SF S("'"), bszInitFilePath);
		if (pu32ErrorCount)
			*pu32ErrorCount = u32ErrorCount;
		return bStatus;
		}
	
	Print_Formatted(hPrint, S("Reading CM Server options file '") B_SF S("'\n"), bszInitFilePath);
	while (b_fgets(bszLine, BCHARSOF(bszLine), hInfile) != NULL)
		{

		// Delete any terminating CR or LF
		n = (INT32) b_strlen(bszLine) - 1;
		while (n >= 0 && (bszLine[n] == C('\n') || bszLine[n] == C('\r')))
			bszLine[n--] = B_NUL;
		iLen = n + 1;

		// Find the first # that is not inside a quoted string
		// and truncate the line at that point
		bcQuoteChar = B_NUL;
		for (n=0; n < iLen; n++)
			{
			bc = bszLine[n];
			if (bcQuoteChar == B_NUL)
				{
				// Not inside a quoted string
				if (bc == '#')
					{
					bszLine[n] = B_NUL;
					break;
					}
				else if (bc == C('\'') || bc == C('"'))
					{
					bcQuoteChar = bc;
					}
				}
			// Look for the matching quote character
			else if (bc == bcQuoteChar)
				{
				bcQuoteChar = B_NUL;
				}
			}
		if (bcQuoteChar != B_NUL)
			{
			Print_Formatted(hPrint, S("Unterminated quoted string:") B_SFNL, bszLine);
			u32ErrorCount++;
			}

		// Skip any blank lines
		u32CharCount = (UINT32) b_strspn(bszLine, DELIMITERS);
		bc = bszLine[u32CharCount];
		if (bc == B_NUL)
			continue;

		// Parse the command line
		u32CmdID = ParseCommandLine(bszLine, 
			abszArgs, MAX_NUMBER_OF_COMMAND_ARGS, &u32ActualNumArgs);

		// Process begin .. end pairs
		if (BcsCmpNoCase(abszArgs[0], S("begin")))
			{
			i32BlockLevel++;
			if (i32BlockLevel > 1)
				{
				Print_Formatted(hPrint, S("ERROR: Invalid BEGIN ... END nesting\n"));
				u32ErrorCount++;
				}
			else if (u32ActualNumArgs != 2)
				{
				Print_Formatted(hPrint, S("ERROR: Invalid BEGIN command\n"));
				u32ErrorCount++;
				}
			else
#ifdef WIN32
				if (!(BcsCmpNoCase(abszArgs[1], S("windows")) ||
					  BcsCmpNoCase(abszArgs[1], S("win32"))))
					bSkipCommands = TRUE;
#endif
#ifdef LINUX
				if (!BcsCmpNoCase(abszArgs[1], S("linux")))
					bSkipCommands = TRUE;
#endif
#ifdef OSX
				if (!BcsCmpNoCase(abszArgs[1], S("osx")))
					bSkipCommands = TRUE;
#endif
			}
		else if (BcsCmpNoCase(abszArgs[0], S("end")))
			{
			i32BlockLevel--;
			if (i32BlockLevel != 0)
				{
				Print_Formatted(hPrint, S("ERROR: Invalid BEGIN ... END nesting.\n"));
				u32ErrorCount++;
				}
			else if (u32ActualNumArgs != 1)
				{
				Print_Formatted(hPrint, S("ERROR: Invalid END command\n"));
				u32ErrorCount++;
				}
			bSkipCommands = FALSE;
			}

		// Checking for argument count
		else if (u32ActualNumArgs < 2)
			{
			Print_Formatted(hPrint, S("ERROR: No value given for option '") B_SF S("'\n"), abszArgs[1]);
			u32ErrorCount++;
			}
		else if (u32ActualNumArgs > 2 && 
			!(BcsCmpNoCase(abszArgs[0], S("BandwidthMHz")) || BcsCmpNoCase(abszArgs[0], S("CenterFrequencyMHz"))))
			{
			Print_Formatted(hPrint, S("ERROR: Option line contains invalid text '") B_SF S("'\n"), abszArgs[2]);
			u32ErrorCount++;
			}

		// skipping
		else if (bSkipCommands)
			{
			// Print_Formatted(hPrint, S("Skipping: ") B_SF S(" ") B_SFNL, abszArgs[0], abszArgs[1]);
			}

		// Process option setting commands
		else
			{
			// Set the option
			if (Options_SetOptionByName(hTable, 
					abszArgs[0], &abszArgs[1], u32ActualNumArgs - 1,
					hPrint) == NULL)
				u32ErrorCount++;
			}

		}

	if (i32BlockLevel > 0)
		{
		Print_Formatted(hPrint, S("ERROR: %d end commands missing from options file"),
			i32BlockLevel);
		u32ErrorCount++;
		}
	else if (i32BlockLevel < 0)
		{
		Print_Formatted(hPrint, S("ERROR: %d umatched end commands found in options file"),
			i32BlockLevel);
		u32ErrorCount++;
		}
	else if (ferror(hInfile))
		{
		Print_Formatted(hPrint, S("ERROR: I/O error while reading file '") B_SF S("'"), bszInitFilePath);
		u32ErrorCount++;
		}
	else
		bStatus = TRUE;

	fclose(hInfile);

	if (pu32ErrorCount)
		*pu32ErrorCount = u32ErrorCount;

	if (u32ErrorCount > 0)
		Print_Formatted(hPrint, S("%u errors found while reading options file.\n"), u32ErrorCount);

	return bStatus;

	} /* Options_LoadOptionsFromFile */

// =========================================================================================
// Function to save option values to a file
// =========================================================================================

BOOL Options_SaveOptionsToFile(hOptionTable_t hTable, const STRING bszInitFilePath, PrintContext_t hPrint)
	{

	UINT32 n, nVal;
	stArrayUint32_t *pstArray;
	B_CHAR bszBuffer[64];

	FILE *hOutfile;

	if ((hOutfile = BcsFopen(bszInitFilePath, S("wt"))) == NULL)
		{
		Print_Formatted(hPrint, S("ERROR: Can't write options file '") B_SF S("'\n"), bszInitFilePath);
		SysLogWPrintf(SYSLOG_ERROR, S("Can't write options file '") B_SF S("'"), bszInitFilePath);
		return FALSE;
		}

	Print_Formatted(hPrint, S("Writing CM Server options file '") B_SF S("'\n"), bszInitFilePath);

	b_fprintf(hOutfile, S("# Options saved by CM Server on ") B_SFNL, DateTimeStamp(bszBuffer, BCHARSOF(bszBuffer)));

	for (n=0; n<g_u32NumberOfOptions; n++)
		{
		switch (hTable[n].eType)
			{
			case eFILENAME:	
			case eSTRING:
				b_fprintf(hOutfile, B_SF1(35) S("'") B_SF S("'\n"), hTable[n].bszName, 
					hTable[n].uVal.stString.bszValue != NULL ? hTable[n].uVal.stString.bszValue : EMPTY_STRING);
				break;

			case eARRAY_UINT32:
				pstArray = &hTable[n].uVal.stArray;
				if (pstArray->u32CurLength > 0)
					{
					b_fprintf(hOutfile, B_SF1(35), hTable[n].bszName);
					for (nVal=0; nVal < pstArray->u32CurLength; nVal++)
						b_fprintf(hOutfile, B_SF S(" "), 
							StrKHzToMHz(bszBuffer, BCHARSOF(bszBuffer), pstArray->au32Values[nVal]));
					b_fprintf(hOutfile, S("\n"));
					}
				break;

			case eBOOL:
				b_fprintf(hOutfile, B_SF1(35) S("%d\n"), hTable[n].bszName, hTable[n].uVal.bVal ? 1 : 0);
				break;

			case eUINT32:
				b_fprintf(hOutfile, B_SF1(35) S("%d\n"), hTable[n].bszName, hTable[n].uVal.u32Val);
				break;

			default:
				break;
			}
		}

	if (ferror(hOutfile))
		Print_Formatted(hPrint, S("ERROR: I/O error while writing file '") B_SF S("'"), bszInitFilePath);

	fclose(hOutfile);

	return TRUE;

	} /* Options_SaveOptionsToFile */

// -----------------------------------------------------------------------------------------
STRING Options_SetOptionByName(hOptionTable_t hTable, 
							   const STRING bszFieldName, STRING bszArgs[], UINT32 u32NumberOfArgs,
							   PrintContext_t hPrint)
	{

	BOOL bProgramError = FALSE, bInvalidValue = FALSE;
	STRING bszType = NULL;
	stOptionEntry_t *pstEntry;
	STRING bszParam;
	INT32 i32Value;
	UINT32 n, u32NumberOfOptions, *au32Values;
	B_CHAR *pwcEndPtr = NULL;
#ifdef _MAX_PATH
	UINT32 u32Length;
#endif

	// Find the entry (if unique)
	pstEntry = NULL;
	u32NumberOfOptions = 0;
	for (n = 0; n < g_u32NumberOfOptions; n++)
		{ 
		if (BcsCmpNoCase(hTable[n].bszName, bszFieldName))
			{
			pstEntry = &hTable[n];
			u32NumberOfOptions++;
			}
		}

	if (u32NumberOfOptions == 0)
		{
		Print_Formatted(hPrint, S("Unknown option name: '") B_SF S("'\n"), bszFieldName);
		return NULL;
		}

	else if (u32NumberOfOptions > 1)
		{
		Print_Formatted(hPrint, S("More than one matching option:\n"));
		for (n = 0; n < g_u32NumberOfOptions; n++)
			{ 
			if (BcsCmpNoCase(hTable[n].bszName, bszFieldName))
				Print_Formatted(hPrint, S("     ") B_SFNL, hTable[n].bszName);
			}
		return NULL;
		}

	// Store the value
	bProgramError = FALSE;
	bszParam = bszArgs[0];
	switch (pstEntry->eType)
		{

		// Valid values are TRUE/FALSE, T/F, or 1/0
		case eBOOL:
			bszType = S("BOOL");
			if (BcsCmpNoCase(bszParam, S("TRUE")) || BcsCmpNoCase(bszParam, S("T")) || BcsCmpNoCase(bszParam, S("1")) ||
				BcsCmpNoCase(bszParam, S("Yes")) || BcsCmpNoCase(bszParam, S("Y")))
				bProgramError = !Options_SetBoolByEntry(pstEntry, TRUE);
			else if (BcsCmpNoCase(bszParam, S("FALSE")) || BcsCmpNoCase(bszParam, S("F")) || BcsCmpNoCase(bszParam, S("0")) ||
				BcsCmpNoCase(bszParam, S("No")) || BcsCmpNoCase(bszParam, S("N")))
				bProgramError = !Options_SetBoolByEntry(pstEntry, FALSE);
			else
				bInvalidValue = TRUE;
			break;

		case eUINT32: 
			bszType = S("UINT32");
			i32Value = b_strtol(bszParam, &pwcEndPtr, 10);
			if (*pwcEndPtr == B_NUL && i32Value >= 0)
				bProgramError = !Options_SetUint32ByEntry(pstEntry, i32Value);
			else
				bInvalidValue = TRUE;
			break;
			
		case eARRAY_UINT32:
			bszType = S("UINT32 Array");
			if (u32NumberOfArgs > pstEntry->uVal.stArray.u32MaxLength)
				Print_Formatted(hPrint,	S("Given %d values exceed %d values allowed for option '") B_SF S("' with ") B_SF S(" type\n"),
					u32NumberOfArgs, pstEntry->uVal.stArray.u32MaxLength,
					bszParam, bszFieldName, bszType);
			else if ((au32Values = WcmMalloc(u32NumberOfArgs * sizeof(UINT32))) == NULL)
					Print_Formatted(hPrint, S("Array memory allocation failed for option '") B_SF S("'\n"),
						bszFieldName);
			else
				{
				for (n=0; n<u32NumberOfArgs; n++)
					{
					bszParam = bszArgs[n];
					// Convert from MHz to kHz
					i32Value = (INT32) (1000.0 * b_strtod(bszParam, &pwcEndPtr) + 0.5);
					au32Values[n] = 0;
					if (*pwcEndPtr != B_NUL || i32Value < 0)
						{
						bInvalidValue = TRUE;
						break;
						}
					else
						au32Values[n] = (UINT32) i32Value;
					}
				if (!bInvalidValue)
					bProgramError = !Options_SetUint32ArrayByEntry(pstEntry, au32Values, u32NumberOfArgs);
				WcmFree(au32Values);
				}
			break;

		case eFILENAME:
#ifdef _MAX_PATH
			u32Length = (UINT32) b_strlen(bszParam);
			if (u32Length > _MAX_PATH)
				{
				Print_Formatted(hPrint, 
					S("ERROR: File name length (%lu) is greater than system's maximum path length (%lu)\n"),
					u32Length, _MAX_PATH);
				return NULL;
				}
			else
#endif
				bProgramError = !Options_SetStringByEntry(pstEntry, bszParam);
			break;

		case eSTRING:
			bProgramError = !Options_SetStringByEntry(pstEntry, bszParam);
			break;
			
		case eSTRUCT:
		default:
			bProgramError = TRUE;
			Print_Formatted(hPrint, S("PROGRAM ERROR: Invalid option type in 'Options_SetOption'\n"));
			break;
			
		}

	if (bInvalidValue)
		{
		Print_Formatted(hPrint,	S("Invalid value '") B_SF S("' for option '") B_SF S("' with ") B_SF S(" type\n"), 
					bszParam, bszFieldName, bszType);
		return NULL;
		}

	else if (bProgramError)
		{
		Print_Formatted(hPrint, S("PROGRAM ERROR: While setting '") B_SF S("'\n"), bszFieldName);
		return NULL;
		}
		
	return pstEntry->bszName;

	} /* Options_SetOption */

// =========================================================================================
// Function to print one, some, or all of the option values
// =========================================================================================

BOOL Options_GetOptionByName(hOptionTable_t hTable, const STRING bszFieldName, PrintContext_t hPrint)
	{

	UINT32 n, nArray;
	stArrayUint32_t *pstArray;
	BOOL bPrintAll = FALSE, bFound = FALSE;

	if (!hPrint)
		return FALSE;

	bPrintAll = bszFieldName == NULL || bszFieldName[0] == B_NUL;

	for (n = 0; n < g_u32NumberOfOptions; n++)
		{

		if (!bPrintAll && !BcsStrStrNoCase(hTable[n].bszName, bszFieldName))
			continue;

		bFound = TRUE;

		Print_Formatted(hPrint, B_SF1(30) S("   "), hTable[n].bszName);

		switch (hTable[n].eType)
			{
			case eBOOL:
				Print_Formatted(hPrint, B_SFNL,  hTable[n].uVal.bVal ? S("Yes") : S("No"));
				break;

			case eUINT32:
				Print_Formatted(hPrint, S("%lu\n"),  hTable[n].uVal.u32Val);
				break;

			case eSTRING:
			case eFILENAME:
				Print_Formatted(hPrint, S("'") B_SF S("'\n"), hTable[n].uVal.stString.bszValue);
				break;

			case eARRAY_UINT32: // Convert values from kHz to MHz on output
				pstArray = &hTable[n].uVal.stArray;
				for (nArray = 0; nArray < pstArray->u32CurLength; nArray++)
					{
					Print_Formatted(hPrint, S("%0.3f "), 
						hTable[n].uVal.stArray.au32Values[nArray] / 1000.0);
					if (nArray > 0 && nArray % 10 == 0 && nArray != pstArray->u32CurLength-1)
						Print_Formatted(hPrint, S("\n") B_SF1(30) S("   "), S(""));
					}
				if (pstArray->u32CurLength == 0)
					Print_Formatted(hPrint, S("<Not Set>\n"));
				else
					Print_Formatted(hPrint, S("\n"));
				break;

			default:
				Print_Formatted(hPrint, B_SFNL, S("ERROR: Unknown type"));
				return FALSE;

			}
		}

	if (!bFound)
		{
		Print_Formatted(hPrint, S("ERROR: Unknown option name '") B_SF S("'\n"), bszFieldName);
		return FALSE;
		}
	else
		return TRUE;

	} /* Options_GetAll */

BOOL Options_GetNetSearchParameters(hOptionTable_t hTable, hNetSearchParameters_t hNetSearch)
	{

	UINT32 u32MaxSize, u32Size, *pu32Array;
	BOOL bError = FALSE;

	if (!hTable || !hNetSearch)
		return FALSE;

	pu32Array  = Options_GetUint32ArrayByName(hTable, S("BandwidthMHz"), &u32Size, &bError);
	u32MaxSize = sizeof(hNetSearch->u32BandwidthKHz)/sizeof(UINT32);
	memset(hNetSearch->u32BandwidthKHz, 0, u32MaxSize * sizeof(UINT32));
	if (u32Size <= u32MaxSize)
		{
		memcpy(hNetSearch->u32BandwidthKHz, pu32Array, u32Size * sizeof(UINT32));
		hNetSearch->u32NumberOfBandwidths = u32Size;
		}
	else
		{
		bError = TRUE;
		hNetSearch->u32NumberOfBandwidths = 0;
		}

	pu32Array = Options_GetUint32ArrayByName(hTable, S("ScanMinFrequencyMHz"),  &u32Size, &bError);
	if (u32Size != 1)
		bError = TRUE;
	hNetSearch->u32ScanMinFrequencyKHz = pu32Array[0];
	pu32Array = Options_GetUint32ArrayByName(hTable, S("ScanMaxFrequencyMHz"),  &u32Size, &bError);
	if (u32Size != 1)
		bError = TRUE;
	hNetSearch->u32ScanMaxFrequencyKHz = pu32Array[0];
	pu32Array = Options_GetUint32ArrayByName(hTable, S("ScanStepFrequencyMHz"), &u32Size, &bError);
	if (u32Size != 1)
		bError = TRUE;
	hNetSearch->u32ScanStepFrequencyKHz = pu32Array[0];

	pu32Array  = Options_GetUint32ArrayByName(hTable, S("CenterFrequencyMHz"), &u32Size, &bError);
	u32MaxSize = sizeof(hNetSearch->u32CenterFrequencyKHz)/sizeof(UINT32);
	memset(hNetSearch->u32CenterFrequencyKHz, 0, u32MaxSize * sizeof(UINT32));
	if (u32Size == 1 && pu32Array[0] == 0)
		hNetSearch->u32NumberOfFrequencies = 0;
	else if (u32Size <= u32MaxSize)
		{
		memcpy(hNetSearch->u32CenterFrequencyKHz, pu32Array, u32Size * sizeof(UINT32));
		hNetSearch->u32NumberOfFrequencies = u32Size;
		}
	else
		{
		hNetSearch->u32NumberOfFrequencies = 0;
		bError = TRUE;
		}

	return !bError;

	} /* Options_GetNetSearchParameters */

BOOL Options_SetNetSearchParameters(hOptionTable_t hTable, hNetSearchParameters_t hNetSearch)
	{

	BOOL bRetVal = TRUE;

	if (!hTable || !hNetSearch)
		return FALSE;

	bRetVal &= Options_SetUint32ByName(hTable, S("NumberOfBandwidths"),   hNetSearch->u32NumberOfBandwidths);
	bRetVal &= Options_SetUint32ArrayByName(hTable, S("BandwidthMHz"),    
		hNetSearch->u32BandwidthKHz, hNetSearch->u32NumberOfBandwidths);
	bRetVal &= Options_SetUint32ArrayByName(hTable, S("ScanMinFrequencyMHz"),  &hNetSearch->u32ScanMinFrequencyKHz,  1);
	bRetVal &= Options_SetUint32ArrayByName(hTable, S("ScanMaxFrequencyMHz"),  &hNetSearch->u32ScanMaxFrequencyKHz,  1);
	bRetVal &= Options_SetUint32ArrayByName(hTable, S("ScanStepFrequencyMHz"), &hNetSearch->u32ScanStepFrequencyKHz, 1);
	bRetVal &= Options_SetUint32ByName(hTable, S("NumberOfFrequencies"),  hNetSearch->u32NumberOfFrequencies);
	bRetVal &= Options_SetUint32ArrayByName(hTable, S("CenterFrequencyMHz"), 
		hNetSearch->u32CenterFrequencyKHz, hNetSearch->u32NumberOfFrequencies);

	return bRetVal;

	}  /* SetOption for cNetSearchParameters */

// TO DO: Use this in wimaxd.conf validation
static BOOL Options_IsValidUint32 (STRING bszField, UINT32 u32MaxValue)
	{

	INT32 i32Value = 0;
	B_CHAR *pwcEndPtr = NULL;

	i32Value = b_strtol(bszField, &pwcEndPtr, 10);
	if (*pwcEndPtr == B_NUL && i32Value >= 0 && i32Value <= (INT32) u32MaxValue)
		return TRUE;
	else
		return FALSE;

	} /* Options_IsValidUint32 */

// TO DO: Use this in wimaxd.conf validation
// Checks for valid IP address as string in the format 255.255.255.255
BOOL Options_IsValidIPAddress (STRING bszField)
	{

	int n;
	B_CHAR *wcp;
	STRING bszNum, bszRest = bszField;

	// Trim leading whitespace
	n = (int) b_strspn(bszRest, S("\t "));
	bszRest = &bszRest[n];

	// TO DO: Trim trailing whitespace?

	for (n=0; n<3; n++)
		{
		if ((wcp = b_strchr(bszRest, C('.'))) != NULL)
			{
			*wcp = B_NUL;
			bszNum = bszRest;
			if (!Options_IsValidUint32(bszNum, 255))
				return FALSE;
			bszRest = wcp + 1;
			}
		else
			return FALSE;
		}

	if (!Options_IsValidUint32(bszRest, 255))
		return FALSE;
	else
		return TRUE;

	} /* Options_IsValidIPAddress */

// Find a field name in the option definition table
stOptionEntry_t * Options_FindEntry(hOptionTable_t hTable, const STRING bszFieldName)
	{

	int n;

	if (!hTable || !bszFieldName)
		return NULL;

	for (n=0; g_astOptionDefs[n].eType != eNONE; n++)
		{
		if (BcsCmpNoCase(g_astOptionDefs[n].bszName, bszFieldName))
			{
			hTable[n].bszName = g_astOptionDefs[n].bszName;
			hTable[n].eType   = g_astOptionDefs[n].eType;
			return &hTable[n];
			}
		}

	return NULL;

	} /* Options_FindEntry */

//checks if the filename parameter is specifying a slot in NVM.
BOOL Options_IsFileOnDeviceMem(STRING name)
{	
	return BcsnCmpNoCase(name, DEVICE_MEM_SLOT_NAME_STRING, (UINT32)strlenof_DEVICE_MEM_SLOT_NAME_STRING);
}

//validate and get the slot number - returns (-1) if slot is not valid.
UINT32 Options_ValidateAndGetSlotNumber(STRING name)
{
	if(b_strlen(name) > strlenof_DEVICE_MEM_SLOT_NAME_STRING)
	{
		UINT32 slotId = atoi(name+strlenof_DEVICE_MEM_SLOT_NAME_STRING); 
		if(slotId >= DEIVCE_MEM_SLOT_ID_MIN_VALUE && slotId <= DEIVCE_MEM_SLOT_ID_MAX_VALUE)
			return slotId;
	}
	return 0;
}
