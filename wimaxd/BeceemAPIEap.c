/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */


#ifdef WIN32
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "BeceemAPI.h"
#include "Utility.h"
#include "LogFile.h"

#include "CscmString.h"
#include <sys/types.h>
#include <sys/stat.h>

// various items extracted from common.h
//
// Note: The entire header file cannot be included because of 
// strong dependencies on various mingw-specific include files

#define ETH_ALEN 6
enum {
	HOSTAPD_LEVEL_DEBUG_VERBOSE, 
	HOSTAPD_LEVEL_DEBUG,
	HOSTAPD_LEVEL_INFO, 
	HOSTAPD_LEVEL_NOTICE, 
	HOSTAPD_LEVEL_WARNING};
enum { 
	MSG_MSGDUMP, 
    MSG_DEBUG, 
	MSG_INFO, 
	MSG_WARNING, 
	MSG_ERROR };
// end of common.h items

// Include files from wpa_supplicant directory
#ifdef LINUX_OR_OSX
#define EXTERN_C 
#endif

#include "config_ssid.h"
#include "eap_supplicant.h"
#include "base64.h"

// end wpa_supplicant include files

#define NAI_CHANGE_CIPHER_RULE S("AES256-SHA:AES128-SHA")
//#define NAI_CHANGE_CIPHER_RULE S("AES256-SHA")
//#define NAI_CHANGE_CIPHER_RULE S("AES128-SHA")

// These are local (unmanaged or native) status variables
static BOOL g_bAuthEnabled;
static BOOL g_bAuthEthernetToRADIUS;

#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST
static INT32 BeceemAPI_EAP_RSAPrivateEncrypt(
	void *context, 
	const UINT8 *au8RSAMsgIn,  UINT32 u32RSAMsgInLen,
	UINT8 *au8RSAMsgOut, UINT32 u32EAPMsgOutMaxLen);
#else
#define BeceemAPI_EAP_RSAPrivateEncrypt NULL
#endif

// NAI change constants shared with Beceem engine DLL
enum {ID_AES_128_KEY, ID_AES_256_KEY, ID_AES_128_IV, ID_AES_256_IV, 
      ID_SHA1_HMAC_SECRET, ID_SEQUENCE_NUMBER, ID_LAST_CRYPTO};

// Crypto field handling for Inner NAI change option
static INT32 BeceemAPI_EAP_SetCryptoField(void *context, UINT32 u32FieldID, const UINT8 *au8FieldData, UINT32 u32FieldLen);

#define LEN_AES_128_KEY  16
#define LEN_AES_128_IV   16
#define LEN_AES_256_KEY  32
#define LEN_AES_256_IV   16
#define LEN_HMAC_SECRET    20
#define LEN_SEQUENCE_NUMBER 8

const UINT32 u32CryptoFieldLengths[6] = 
	{LEN_AES_128_KEY, LEN_AES_256_KEY, LEN_AES_128_IV, LEN_AES_256_IV, LEN_HMAC_SECRET, LEN_SEQUENCE_NUMBER};

static struct {
	BOOL  bAES128KeySet;
	BOOL  bAES256KeySet;
	BOOL  bAES128IVSet;
	BOOL  bAES256IVSet;
	BOOL  bHMACSecretSet;
	BOOL  bSequenceNumberSet;
	UINT8 au8AES128Key[LEN_AES_128_KEY];
	UINT8 au8AES256Key[LEN_AES_256_KEY];
	UINT8 au8AES128IV[LEN_AES_128_IV];
	UINT8 au8AES256IV[LEN_AES_256_IV];
	UINT8 au8HMACSecret[LEN_HMAC_SECRET];
	UINT8 au8SequenceNumber[LEN_SEQUENCE_NUMBER];
	} stCryptoFields;

#define CLIENT_CERT_BLOB_NAME 			"ClientCert"
#define PRIVATE_KEY_BLOB_NAME 			"PrivateKey"
#define DEVICE_SUB_CA1_CERT_BLOB_NAME	"DeviceSubCA1Cert"
#define DEVICE_SUB_CA2_CERT_BLOB_NAME	"DeviceSubCA2Cert"

//Slot Id is calculated relative to CERTIFICATE1_DATA
#define GET_DATAFIELD_ID_FOR_SLOT(n) 		((CERTIFICATE1_DATA/2)*(n+1)) 
#define GET_LENFIELD_ID_FOR_SLOT(n) 		(GET_DATAFIELD_ID_FOR_SLOT(n)-1) 


//MOD-END
static void ZeroCryptoFields(void);


/* Read the Certs and Keys from the NVM. */

static INT32 BeceemAPI_EAP_GetSecCreds(void *context, const UINT32 uSecParamId, 
													char **au8CertKeyData)
{
	UINT32 u32OutLen = 0;
	BeceemAPI_t hAPI = (BeceemAPI_t) context;

	// Send the request
	BeceemAPI_EAP_GetSecCredsRequest (hAPI, uSecParamId);
	
	if(hAPI->u32SecCredsLength> 0 && hAPI->au8SecCredsData)
	{
		*au8CertKeyData = (char*) WcmMalloc(hAPI->u32SecCredsLength);
		if(!(*au8CertKeyData))
		{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: BeceemAPI_EAP_GetSecCreds malloc failed\n"));
			return 0;
		}
		memset(*au8CertKeyData, 0, hAPI->u32SecCredsLength);
		memcpy(*au8CertKeyData, hAPI->au8SecCredsData, hAPI->u32SecCredsLength);

		//Print_Formatted(hAPI->hPrintDiag, S("BeceemAPI_EAP_GetSecParam - Cert/keydata: ") B_SF,
		//			(char *) ((*au8CertKeyData)+1024));

		u32OutLen = hAPI->u32SecCredsLength;
		WcmFree(hAPI->au8SecCredsData);
		hAPI->au8SecCredsData = NULL;
		hAPI->u32SecCredsLength = 0;
	}
	else 
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: BeceemAPI_EAP_GetSecCredsRequest returned NULL/0 length.\n"));
	return u32OutLen;
} 



// Write the Engine configuration 
static void BeceemAPI_EngineConfig(
			   BeceemAPI_t hAPI,
			   ST_EAP_INIT_PARAMS *pstEAPInitParams)
	{

	static const STRING bszEngineConfig =
		S("engine=1\n")
		S("engine_id=\"opensc\"\n")
		S("key_id=\"0\"\n");

	UINT32 u32LogFileSizeMB = Options_GetUint32ByEntry(hAPI->pOptEngineLogFileMaxSizeMB, NULL);
	
	Print_Append(hAPI->hPrintEAPConfig, bszEngineConfig);
		
	Print_Formatted(hAPI->hPrintEAPConfig, S("pin=\"%p %p %p %d %d %d %d %d '"),
		BeceemAPI_EAP_RSAPrivateEncrypt, 
		BeceemAPI_EAP_SetCryptoField,
		hAPI, 
		pstEAPInitParams->bFirmwareRSAPrivateKeyEncrypt ? 1 : 0,
		hAPI->bInnerNAIChange ? 1 : 0,
		pstEAPInitParams->bEngineLoggingEnabled ? 1 : 0,
		pstEAPInitParams->u32FirmwarePrivateKeyBits,
		u32LogFileSizeMB);

	Print_Append(hAPI->hPrintEAPConfig, pstEAPInitParams->bszEngineLogFileName);
	Print_Append(hAPI->hPrintEAPConfig, S("'\"\n"));

	} // BeceemAPI_EngineConfig

// Write TLS certificate configuration
static void BeceemAPI_CertConfig(
			   BeceemAPI_t hAPI,
			   BOOL bUseEngine,
			   ST_EAP_INIT_PARAMS *pstEAPInitParams)
	{

	B_CHAR wc;
	BOOL bHexFormat = FALSE;
	int n, i32Len;

	// cert location or blob name
	/*Pass the certificates as blob if they are on the device NVM.	*/
	if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceCertFileName))
		Print_Formatted(hAPI->hPrintEAPConfig, S("client_cert=\"blob://" S(CLIENT_CERT_BLOB_NAME) "\"\n"));
	else
		Print_Formatted(hAPI->hPrintEAPConfig, S("client_cert=\"") B_SF S("\"\n"),
						StringTrim(hAPI, pstEAPInitParams->bszTLSDeviceCertFileName));

	if(!Options_IsEmptyStringByEntry(hAPI->pOptTLSDeviceSubCA1CertFileName, NULL))
	{		
		if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceSubCA1CertFileName))
			Print_Formatted(hAPI->hPrintEAPConfig, S("device_subca1_cert=\"blob://" 
									S(DEVICE_SUB_CA1_CERT_BLOB_NAME) "\"\n"));		
		else
			Print_Formatted(hAPI->hPrintEAPConfig, S("device_subca1_cert=\"") B_SF S("\"\n"),
						StringTrim(hAPI, pstEAPInitParams->bszTLSDeviceSubCA1CertFileName));	
	}
	if(!Options_IsEmptyStringByEntry(hAPI->pOptTLSDeviceSubCA2CertFileName, NULL))	
	{
		if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceSubCA2CertFileName))
			Print_Formatted(hAPI->hPrintEAPConfig, S("device_subca2_cert=\"blob://" 
										S(DEVICE_SUB_CA2_CERT_BLOB_NAME) "\"\n"));		
		else
			Print_Formatted(hAPI->hPrintEAPConfig, S("device_subca2_cert=\"") B_SF S("\"\n"),
						StringTrim(hAPI, pstEAPInitParams->bszTLSDeviceSubCA2CertFileName));	
	}
	// Check private key format
	wc = pstEAPInitParams->bszPrivateKeyPasswordFormat[0];
	if (wc == C('A') || wc == C('a'))
		bHexFormat = FALSE;
	else if (wc == C('H') || wc == C('h'))
		bHexFormat = TRUE;
	else
		{
		bHexFormat = bUseEngine;
		PrintDiag(DIAG_ERROR, hAPI, 
			S("\n***** ERROR: Invalid private key format specifier: %lc - using ") B_SF S(" format"), 
			wc, bHexFormat ? S("Hex") : S("ASCII"));
		}

	// Entries for cryptographic engine
	if (bUseEngine)
		{
		i32Len = (INT32) b_strlen(pstEAPInitParams->bszTLSDevicePrivateKeyPassword);
		memset(hAPI->au8PrivKeyAESEncryptionKey, 0, sizeof(hAPI->au8PrivKeyAESEncryptionKey));
		if (bHexFormat)
			{
			if (HexStringToUint8Array(pstEAPInitParams->bszTLSDevicePrivateKeyPassword, 
					hAPI->au8PrivKeyAESEncryptionKey, sizeof(hAPI->au8PrivKeyAESEncryptionKey)) < 0)
				PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Invalid private key AES password hex string\n")); 
			else if (i32Len > 2*PRIV_KEY_AES_KEY_LEN)
				PrintDiag(DIAG_ERROR, hAPI, S("\n***** WARNING: Private key password is longer than ")
					S("%d hex digits - will be truncated\n"), 2*PRIV_KEY_AES_KEY_LEN);
			}
		else
			{
			if (i32Len > PRIV_KEY_AES_KEY_LEN)
				PrintDiag(DIAG_ERROR, hAPI, S("\n***** WARNING: Private key password is longer than ")
					S("%d characters - will be truncated\n"), PRIV_KEY_AES_KEY_LEN);
			for (n=0; n < sizeof(hAPI->au8PrivKeyAESEncryptionKey) && n < i32Len; n++)
				hAPI->au8PrivKeyAESEncryptionKey[n] = (UINT8) pstEAPInitParams->bszTLSDevicePrivateKeyPassword[n];
			}
		}
	else 
		{
		if (bHexFormat)
			PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Hex format not allowed for private key password\n"));

		/*Pass the key as a blob if it is on the device NVM.	*/
		if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDevicePrivateKeyFileName))
			Print_Formatted(hAPI->hPrintEAPConfig, S("private_key=\"blob://" S(PRIVATE_KEY_BLOB_NAME) "\"\n"));
		else
			Print_Formatted(hAPI->hPrintEAPConfig, S("private_key=\"") B_SF S("\"\n"),
						StringTrim(hAPI, pstEAPInitParams->bszTLSDevicePrivateKeyFileName));

		Print_Formatted(hAPI->hPrintEAPConfig,
			S("private_key_passwd=\"") B_SF S("\"\n"),
			StringTrim(hAPI, pstEAPInitParams->bszTLSDevicePrivateKeyPassword));
		}

	} /* BeceemAPI_CertConfig */


//write the cert blobs to config string.
static BOOL BeceemAPI_CertBlobConfig(BeceemAPI_t hAPI, STRING fileName, STRING blobName)
{	
	BOOL ret = FALSE;
	char *blobLen = NULL, *blobData = NULL;
	UINT32 len = 0;
	UINT32 enc_len = 0;
	UINT32 slot = Options_ValidateAndGetSlotNumber(fileName);
	if(slot > 0 )
	{
		len = BeceemAPI_EAP_GetSecCreds(hAPI, GET_LENFIELD_ID_FOR_SLOT(slot), &blobLen);
		if(len == 2 && (blobLen[1] > 0 || blobLen[0] > 0))
		{
			len = BeceemAPI_EAP_GetSecCreds(hAPI, GET_DATAFIELD_ID_FOR_SLOT(slot), &blobData);
			if(len > 0)
			{
				unsigned char* enc_blobData = base64_encode((unsigned char*)blobData, len, &enc_len);
				if(enc_blobData)
				{
					UINT32 write_len = enc_len;
					Print_Formatted(hAPI->hPrintEAPConfig, S("blob-base64-") B_SF 
												S("=\n"), blobName);
					while(write_len)
					{
						unsigned char enc_data[PRINT_CONTEXT_PRINTF_BUFFER_SIZE] = {0,};

						PrintDiag(DIAG_NORMAL, hAPI, S("remaining length of data to write into config blob=%d\n"), write_len);
						if(write_len <= (PRINT_CONTEXT_PRINTF_BUFFER_SIZE-4))
						{
							memcpy(enc_data, enc_blobData+(enc_len - write_len), write_len);
							write_len = 0;
						}
						else
						{
							memcpy(enc_data, enc_blobData+(enc_len - write_len), PRINT_CONTEXT_PRINTF_BUFFER_SIZE-4);
							write_len-=(PRINT_CONTEXT_PRINTF_BUFFER_SIZE-4);
						}
						Print_Formatted(hAPI->hPrintEAPConfig, B_SF, enc_data);
					} 	
					Print_Formatted(hAPI->hPrintEAPConfig, S("\n}\n"));
					
					/*On windows, calling free from here causes a crash as it is allocated by the dll. 
					     Hence calling this wpa_suppliant interface to free memory */
					base64_free_mem(enc_blobData);
					ret = TRUE;
				}
				else
					PrintDiag(DIAG_NORMAL, hAPI, S("BLOB %s base64 encoding failed!!!\n"), blobName);
			}
			else
				PrintDiag(DIAG_NORMAL, hAPI, S("BLOB %s data field could not be read@slotID%d, #%d\n"), 
												blobName, GET_DATAFIELD_ID_FOR_SLOT(slot), slot);
			if(blobData)
				WcmFree(blobData);
		}
		else
		{
			PrintDiag(DIAG_NORMAL, hAPI, S("BLOB %s has length field=%d, for slotID%d, #%d\n"), 
									blobName, len, GET_LENFIELD_ID_FOR_SLOT(slot), slot);
		}
		if(blobLen)
			WcmFree(blobLen);
	}
	if(ret == TRUE)
		PrintDiag(DIAG_NORMAL, hAPI, S("BLOB %s [length=%d] configured successfuly\n"), blobName, enc_len);
	else
		PrintDiag(DIAG_ERROR, hAPI, S("WARNING: error reading BLOB %s. [length=%d] ")
									S("It could not be configured\n"), blobName, enc_len);
	return ret;
}
	

// --------------------------------------------------------------
// Build the EAP Supplicant configuration string
//
// NOTE: This function returns a string buffer (created with malloc)
//       that must be freed when no longer needed.
static STRING BeceemAPI_EAPSupplicantConfig(
			    BeceemAPI_t hAPI,
				ST_EAP_INIT_PARAMS *pstEAPInitParams
				)
	{

#define KEYWORD_LEN 32
	STRING bszLine    = EMPTY_STRING;
	B_CHAR  bszKeyword[KEYWORD_LEN];
	int i, n;
	B_CHAR *pbcEquals, *pwc;
	STRING bszRetVal = NULL;
	STRING bszPhase2Proto = NULL;

	static const STRING abszEAPSupplicantConf[] = {
	
	S("eapol_version=1"),
	S("ap_scan=0"),
	S("fast_reauth=1"),

	S("network={"),
	
		S("ssid=\"AuthTest\""),
		S("identity="), // will be added later
		S("password="), // will be added later
		S("key_mgmt=IEEE8021X"),
		S("priority=5"),
		S("eapol_flags=0"),
		S("fragment_size="), // will be added later
		NULL
		};

	STRING bszTemp = EMPTY_STRING;
	UINT32 u32EAPMethod = pstEAPInitParams->u32EAPMethod; // local copy

	BOOL bHaveEngineFileName = pstEAPInitParams->bszBeceemEngineFileName && pstEAPInitParams->bszBeceemEngineFileName[0];

	BOOL bUseDeviceCert = 
		u32EAPMethod == EAP_METHOD_TTLS_MSCHAPV2_DEVICE_CERT ||
		u32EAPMethod == EAP_METHOD_TTLS_CHAP_DEVICE_CERT     ||
		u32EAPMethod == EAP_METHOD_TTLS_PAP_DEVICE_CERT      ||
		u32EAPMethod == EAP_METHOD_TTLS_EAP_TLS              ||
		u32EAPMethod == EAP_METHOD_TLS;

	BOOL bPKEncrypt = pstEAPInitParams->bFirmwareRSAPrivateKeyEncrypt && bUseDeviceCert;
	
	BOOL bNAIChange = hAPI->bInnerNAIChange && (
	    u32EAPMethod == EAP_METHOD_TTLS_CHAP                 ||
		u32EAPMethod == EAP_METHOD_TTLS_CHAP_DEVICE_CERT     ||
		u32EAPMethod == EAP_METHOD_TTLS_PAP                  ||
		u32EAPMethod == EAP_METHOD_TTLS_PAP_DEVICE_CERT);

	BOOL bUseEngine =  (bPKEncrypt || bNAIChange) && bHaveEngineFileName;

	// Start from a clean slate
	Print_Clear(hAPI->hPrintEAPConfig);
	Print_SetBuffered(hAPI->hPrintEAPConfig, TRUE);

	// Enter engine path name
	if (bUseEngine)
		{
		bszTemp = StringTrim(hAPI, pstEAPInitParams->bszBeceemEngineFileName);
		for (i=0; bszTemp[i] != B_NUL; i++)
			if (bszTemp[i] == C('\\'))
				bszTemp[i] = C('/');
		Print_Formatted(hAPI->hPrintEAPConfig, S("opensc_engine_path=") B_SFNL, bszTemp);
		}

	// Start 'network' block
	for (i = 0; abszEAPSupplicantConf[i] != NULL; i++)
		{
		bszLine = abszEAPSupplicantConf[i];
		Print_Append(hAPI->hPrintEAPConfig, bszLine);
		pbcEquals = b_strchr(bszLine, C('='));
		if (pbcEquals == NULL)
			bszKeyword[0] = B_NUL;
		else
			{
			pwc = bszLine;
			for (n=0; n < KEYWORD_LEN; n++)
				{
				if (pwc == pbcEquals)
					bszKeyword[n] = B_NUL;
				else
					bszKeyword[n] = *pwc++;
				}
			}
		if (!b_strcmp(bszKeyword, S("identity")) ||
			!b_strcmp(bszKeyword, S("password")))
			Print_Append(hAPI->hPrintEAPConfig, S("\"                                                                  \""));
		else if (!b_strcmp(bszKeyword, S("fragment_size")))
			Print_Formatted(hAPI->hPrintEAPConfig, S("%d"), pstEAPInitParams->u32EAPFragmentMaxLength);
		Print_Append(hAPI->hPrintEAPConfig, S("\n"));
		}

	// Validate server certificate
	// If server cert validation is turned off, omit the "ca_cert" parameter altogether
	if (pstEAPInitParams->bValidateServerCert)
	{
		if(!Options_IsEmptyStringByEntry(hAPI->pOptCACertFileName, NULL))
			Print_Formatted(hAPI->hPrintEAPConfig, S("ca_cert=\"") B_SF S("\"\n"), StringTrim(hAPI, pstEAPInitParams->bszCACertFileName));
		if(!Options_IsEmptyStringByEntry(hAPI->pOptCACertPath, NULL))
			Print_Formatted(hAPI->hPrintEAPConfig, S("ca_path=\"") B_SF S("\"\n"), StringTrim(hAPI, pstEAPInitParams->bszCACertPath));		
	}
	// Limit SSL cipher suites
	if (bNAIChange)
		Print_Formatted(hAPI->hPrintEAPConfig, 
			S("cipher_rule=\"") B_SF S("\"\n"), NAI_CHANGE_CIPHER_RULE);

	// Phase 1
	if (u32EAPMethod == EAP_METHOD_TLS)
		{
		Print_Append(hAPI->hPrintEAPConfig,
			S("# ---- EAP-TLS -----\n")
			S("eap=TLS\n"));
		}
	else // EAP-TTLS
		{
		Print_Formatted(hAPI->hPrintEAPConfig, 
			S("# ---- EAP-TTLS -----\n")
			S("eap=TTLS\n")
			S("anonymous_identity=\"") B_SF S("\"\n"),
			StringTrim(hAPI, pstEAPInitParams->bszTTLSAnonymousIdentity));
		}

	// Add engine PIN
	if (bUseEngine)
		BeceemAPI_EngineConfig(hAPI, pstEAPInitParams);

	// Add certificate & private key configuration
	if (bUseDeviceCert)
		BeceemAPI_CertConfig(hAPI, bUseEngine, pstEAPInitParams);

	// TLS length
	if (pstEAPInitParams->bAlwaysIncludeTLSLength)
		Print_Append(hAPI->hPrintEAPConfig, 
			S("phase1=\"include_tls_length=1\"\n"));

	// Set the phase 2 protocol
	switch (pstEAPInitParams->u32EAPMethod)
		{
		case EAP_METHOD_TTLS_MSCHAPV2:
		case EAP_METHOD_TTLS_MSCHAPV2_DEVICE_CERT:
			bszPhase2Proto = S("MSCHAPV2");
			break;
		case EAP_METHOD_TTLS_CHAP:
		case EAP_METHOD_TTLS_CHAP_DEVICE_CERT:
			bszPhase2Proto = S("CHAP");
			break;
		case EAP_METHOD_TTLS_PAP:
		case EAP_METHOD_TTLS_PAP_DEVICE_CERT:
			bszPhase2Proto = S("PAP");
			break;
		default:
			bszPhase2Proto = NULL;
			break;
		}

	if (bszPhase2Proto != NULL)
		Print_Formatted(hAPI->hPrintEAPConfig, 
			S("phase2=\"auth=") B_SF S("\"\n"), bszPhase2Proto);
		
	// End the network block
	Print_Append(hAPI->hPrintEAPConfig, S("}\n"));

	// read certs/key from NVM, Write to blob object. Start config blobs with "//blob-base64-"
	if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceCertFileName))
		BeceemAPI_CertBlobConfig(hAPI, pstEAPInitParams->bszTLSDeviceCertFileName, CLIENT_CERT_BLOB_NAME);
	if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceSubCA1CertFileName))
		BeceemAPI_CertBlobConfig(hAPI, pstEAPInitParams->bszTLSDeviceSubCA1CertFileName, DEVICE_SUB_CA1_CERT_BLOB_NAME);
	if(Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDeviceSubCA2CertFileName))
		BeceemAPI_CertBlobConfig(hAPI, pstEAPInitParams->bszTLSDeviceSubCA2CertFileName, DEVICE_SUB_CA2_CERT_BLOB_NAME);
	if(!bUseEngine && Options_IsFileOnDeviceMem(pstEAPInitParams->bszTLSDevicePrivateKeyFileName))
		BeceemAPI_CertBlobConfig(hAPI, pstEAPInitParams->bszTLSDevicePrivateKeyFileName, PRIVATE_KEY_BLOB_NAME);

	// End config blobs
	// End the configuration string
	bszRetVal = Print_GetText(hAPI->hPrintEAPConfig, NULL);
	Print_SetBuffered(hAPI->hPrintEAPConfig, FALSE);
	
	return bszRetVal;

	} /* BeceemAPI_EAPSupplicantConfig */

// ================================================================

// --------------------------------------------------------------
// EAP_FromCSCMToSupplicant
//
//    is called by the CSCM whenever it wishes to send
//    an EAP packet to the EAP supplicant.
//
void BeceemAPI_EAP_FromCSCMToSupplicant (BeceemAPI_t hAPI, const UINT8 *EAP_msg, size_t EAP_msg_len)
{

	WcmMutex_Wait(hAPI->hMutexEAPTraffic);

	g_bAuthEthernetToRADIUS = hAPI->bAuthEthernetToRADIUS;
	g_bAuthEnabled          = hAPI->bAuthEnabled && BeceemAPI_Authorizing(hAPI) && hAPI->bEAPIsOpen;

	if (g_bAuthEnabled)
		ieee802_1x_EAP_packet_received (FALSE, (u8*) EAP_msg, EAP_msg_len);

	WcmMutex_Release(hAPI->hMutexEAPTraffic);
}

// --------------------------------------------------------------
// EAP_FromBaseStationToSupplicant
//
//    is called by the CSCM whenever it wishes to forward 
//    an EAP packet from the base station to the EAP supplicant.
//
// This function is called from EAPResponse(...) in BeceemWiMAXResponse.cpp
void BeceemAPI_EAP_FromBaseStationToSupplicant (BeceemAPI_t hAPI, const UINT8 *EAP_msg, size_t EAP_msg_len)
{

	WcmMutex_Wait(hAPI->hMutexEAPTraffic);

	g_bAuthEthernetToRADIUS = hAPI->bAuthEthernetToRADIUS;
	g_bAuthEnabled          = hAPI->bAuthEnabled && BeceemAPI_Authorizing(hAPI) && hAPI->bEAPIsOpen;

	if (g_bAuthEnabled && !g_bAuthEthernetToRADIUS)
		ieee802_1x_EAP_packet_received (FALSE, (u8*) EAP_msg, EAP_msg_len);

	WcmMutex_Release(hAPI->hMutexEAPTraffic);
}

// --------------------------------------------------------------
// BeceemAPI_EAP_FromRADIUSServerToSupplicant
//
//    is called by the RADIUS server whenever it wishes to send
//    an EAP packet to the EAP supplicant.
//
#ifdef MS_VISUAL_CPP
#pragma warning (disable: 4100)
#endif

static void BeceemAPI_EAP_FromRADIUSServerToSupplicant (void *context, const u8 *EAP_msg, size_t EAP_msg_len)
{

	struct eapol_test_data *pData = (struct eapol_test_data *) context;

	BeceemAPI_t hAPI = (BeceemAPI_t) pData->hBeceemAPI;

	WcmMutex_Wait(hAPI->hMutexEAPTraffic);

	if (g_bAuthEnabled && g_bAuthEthernetToRADIUS)
		ieee802_1x_EAP_packet_received (TRUE, (u8*) EAP_msg, EAP_msg_len);

	WcmMutex_Release(hAPI->hMutexEAPTraffic);

}

#ifdef MS_VISUAL_CPP
#pragma warning (default: 4100)
#endif

// --------------------------------------------------------------
// BeceemAPI_EAP_IsEAPApplicationData
//
//    Parses an EAP message to determine if it contains an 
//    Application Data TLS record.
//
#define EAP_RESPONSE_CODE            2
#define EAP_TYPE_TTLS               21
#define TLS_TYPE_APPLICATION_DATA   23
BOOL BeceemAPI_EAP_IsEAPApplicationData(const UINT8 *au8EAPMessage, UINT32 u32MsgLength)
	{

	typedef struct {
		UINT8 u8EAPCode;
		UINT8 u8EAPIdentifier;
		UINT8 u8EAPLength[2];
		UINT8 u8EAPType;
	} stEAPHeader_t;
	const stEAPHeader_t *pstEAPHeader = (stEAPHeader_t *) au8EAPMessage;
	const UINT32 u32EAPHeaderSize = sizeof(stEAPHeader_t);
	const UINT8 * au8TLSHeader;
	UINT8 u8TLSFlags, u8TLSRecContentType;

	// Test the EAP Header
	if (u32MsgLength < u32EAPHeaderSize + 2 ||
		pstEAPHeader->u8EAPCode != EAP_RESPONSE_CODE || 
		pstEAPHeader->u8EAPType != EAP_TYPE_TTLS)
		return FALSE;

	// Decode the TLS Header
	au8TLSHeader = &au8EAPMessage[u32EAPHeaderSize];
	u8TLSFlags = au8TLSHeader[0];

	if (u8TLSFlags & 0x80)
		{
		if (u32MsgLength <= u32EAPHeaderSize + 6)
			return FALSE;
		u8TLSRecContentType = au8TLSHeader[5];
		}
	else
		{
		u8TLSRecContentType = au8TLSHeader[1];
		}
			
	return u8TLSRecContentType == TLS_TYPE_APPLICATION_DATA;

	} // BeceemAPI_EAP_IsEAPApplicationData

static UINT32 BuildTLV(UINT8 *au8Buffer, UINT8 u8Type, UINT32 u32Length, UINT8 *au8Value)
	{

	au8Buffer[0] = u8Type;
	au8Buffer[1] = (UINT8) u32Length;
	memcpy(&au8Buffer[2], au8Value, u32Length);

	return 2 + u32Length;

	} // BuildTLV
// --------------------------------------------------------------
// BeceemAPI_EAP_BuildNaiSpoofInfoRequestPayload
//
//    Builds the payload for an NAI Spoof Info request message
//
//  Returns:
//      TRUE  - success
//      FALSE - error
BOOL BeceemAPI_EAP_BuildNaiSpoofInfoRequestPayload(BeceemAPI_t hAPI,
			UINT8 *au8Payload, UINT32 u32PayloadMaxSize, UINT16 *pu16PayloadLength, 
			const UINT8 *au8EAPMessage, UINT32 u32EAPMsgLength)
	{
	
	UINT32 u32KeyLength = 0, u32IVLength = 0;
	UINT32 u32PayloadLength = 0;
	UINT8 *au8Buffer, *au8Key, *au8IV;

	// Initialize 0 length for error return
	if (pu16PayloadLength)
		*pu16PayloadLength = 0;

#define AES_KEY_TYPE     0x01
#define AES_IV_TYPE      0x02
#define MAC_SECRET_TYPE  0x03
#define SEQ_NUMBER_TYPE  0x04
#define EAP_PAYLOAD_TYPE 0xA0

	// Determine encryption type
	if (stCryptoFields.bAES128KeySet && stCryptoFields.bAES128IVSet)
		{
		u32KeyLength = LEN_AES_128_KEY;
		u32IVLength  = LEN_AES_128_IV;
		au8Key = stCryptoFields.au8AES128Key;
		au8IV  = stCryptoFields.au8AES128IV;
		}

	else if (stCryptoFields.bAES256KeySet && stCryptoFields.bAES256IVSet)
		{
		u32KeyLength = LEN_AES_256_KEY;
		u32IVLength  = LEN_AES_256_IV;
		au8Key = stCryptoFields.au8AES256Key;
		au8IV  = stCryptoFields.au8AES256IV;
		}
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: AES key and/or IV are not present for NAI change.\n"));
		return FALSE;
		}

	// Check for MAC secret and sequence number
	if (!(stCryptoFields.au8HMACSecret && stCryptoFields.bSequenceNumberSet))
		{
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: MAC secret and/or sequence number are not present for NAI change.\n"));
		return FALSE;
		}

	// Compute and check length
	u32PayloadLength = 
		(2 + u32KeyLength) +
		(2 + u32IVLength) +
		(2 + LEN_HMAC_SECRET) +
		(2 + LEN_SEQUENCE_NUMBER) +
        (3 + u32EAPMsgLength);       // 2 byte length
	if (u32PayloadLength > u32PayloadMaxSize)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Too large NAI change payload length = %d (max = %d)\n"),
			u32PayloadLength, u32PayloadMaxSize);
		return FALSE;
		}
	
	// Add the TLVs
	au8Buffer = au8Payload;
	au8Buffer += BuildTLV(au8Buffer, AES_KEY_TYPE,    u32KeyLength, au8Key);
	au8Buffer += BuildTLV(au8Buffer, AES_IV_TYPE,     u32IVLength,  au8IV);
	au8Buffer += BuildTLV(au8Buffer, MAC_SECRET_TYPE, LEN_HMAC_SECRET,     stCryptoFields.au8HMACSecret);
	au8Buffer += BuildTLV(au8Buffer, SEQ_NUMBER_TYPE, LEN_SEQUENCE_NUMBER, stCryptoFields.au8SequenceNumber);

	// Add the EAP packet
	au8Buffer[0] = EAP_PAYLOAD_TYPE;
	*((UINT16 *) &au8Buffer[1]) = SWAP16(u32EAPMsgLength);
	memcpy(&au8Buffer[3], au8EAPMessage, u32EAPMsgLength);

	// Debug prints
	PrintDiag(DIAG_VERBOSE, hAPI, S("AES Key: (len = 0x%02X)"), u32KeyLength);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, au8Key, u32KeyLength);
	PrintDiag(DIAG_VERBOSE, hAPI, S("AES IV: (len = 0x%02X)"), u32IVLength);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, au8IV, u32IVLength);
	PrintDiag(DIAG_VERBOSE, hAPI, S("HMAC Secret: (len = 0x%02X)"), LEN_HMAC_SECRET);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, stCryptoFields.au8HMACSecret, LEN_HMAC_SECRET);
	PrintDiag(DIAG_VERBOSE, hAPI, S("Sequence Number: (len = 0x%02X)"), LEN_SEQUENCE_NUMBER);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, stCryptoFields.au8SequenceNumber, LEN_SEQUENCE_NUMBER);
	PrintDiag(DIAG_VERBOSE, hAPI, S("EAP Packet: (len = 0x%04x)"), u32EAPMsgLength);
	StringHexDump(DIAG_VERBOSE, hAPI, TRUE, au8EAPMessage, u32EAPMsgLength);

	if (pu16PayloadLength)
		*pu16PayloadLength = (UINT16) u32PayloadLength;

	return TRUE;

	} // BeceemAPI_EAP_BuildNaiSpoofInfoRequestPayload

// --------------------------------------------------------------
// BeceemAPI_EAP_FromEAPSupplicant
//
//    is called by the EAP supplicant whenever it wishes to transmit 
//    an EAP packet to the base station or RADIUS server.

static void BeceemAPI_EAP_FromEAPSupplicant (void *context, const u8 *EAP_msg, size_t EAP_msg_len)
{	

	struct eapol_test_data *pData = (struct eapol_test_data *) context;

	BeceemAPI_t hAPI = (BeceemAPI_t) pData->hBeceemAPI;

	// Received EAP message from EAP supplicant
	if (!g_bAuthEnabled)
		{
		//PrintDiag(DIAG_NORMAL,"Ignored EAP message from EAP supplicant: Authentication is not enabled in the Options.\n");
		return;
		}
	else if (g_bAuthEthernetToRADIUS)
		{
		// Forwarding EAP message to RADIUS server over Ethernet
		WcmMutex_Wait(hAPI->hMutexEAPSupplicant);
		ieee802_1x_encapsulate_radius(pData, (u8*) EAP_msg, EAP_msg_len);
		WcmMutex_Release(hAPI->hMutexEAPSupplicant);
		}
	else
		{
		// Forwarding EAP message to base station;
		// Create a new queue entry 
		Enqueue (pData->hBeceemAPI, eFromEAPSupplicant, (void *) EAP_msg, (UINT32) EAP_msg_len);
		}

} // BeceemAPI_EAP_FromEAPSupplicant

#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST

static INT32 BeceemAPI_EAP_RSAPrivateEncrypt(void *context, 
											const UINT8 *au8RSAMsgIn,  UINT32 u32RSAMsgInLen,
											      UINT8 *au8RSAMsgOut, UINT32 u32EAPMsgOutMaxLen)
	{

	UINT32 u32OutLen;
	BeceemAPI_t hAPI = (BeceemAPI_t) context;

	// Send the request
	BeceemAPI_EAPEncryptPrivateRequest (hAPI, 	
			RSA_NO_PADDING, hAPI->au8PrivKeyAESEncryptionKey, au8RSAMsgIn, u32RSAMsgInLen);

	// Wait for the response
	if (WcmEvent_Wait(hAPI->hEventRSAPrivEncryptResponse, RSA_PRIVATE_KEY_ENCRYPTION_TIMEOUT_MS))
		{
		// Copy and free the response buffer
		u32OutLen = hAPI->u32EncMsgDigestLength;
		if (u32OutLen > u32EAPMsgOutMaxLen)
			{
			PrintDiag(DIAG_ERROR, hAPI, S("ERROR: Encrypted message digest longer than output buffer:")
				S("msg len = %d buffer len = %d\n"),
				u32OutLen, u32EAPMsgOutMaxLen);
			u32OutLen = u32EAPMsgOutMaxLen;
			}
		memcpy(au8RSAMsgOut, hAPI->au8EncMsgDigest, u32OutLen);
		WcmFree(hAPI->au8EncMsgDigest);
		return u32OutLen;
		}

	// no response
	else
		{
		PrintDiag(DIAG_ERROR, hAPI, S("ERROR: RSA private key encryption timeout\n"));
		memset(au8RSAMsgOut, 0, u32EAPMsgOutMaxLen);
		return -1;
		}

	} // BeceemAPI_EAP_RSAPrivateEncrypt

#endif

// This function is called by the Beceem engine to save a cryptographic field
static INT32 BeceemAPI_EAP_SetCryptoField(void *context, UINT32 u32FieldID, const UINT8 *au8FieldData, UINT32 u32FieldLen)
	{
									  
	BeceemAPI_t hAPI = (BeceemAPI_t) context;

	if (hAPI == NULL ||
		u32FieldID >= ID_LAST_CRYPTO ||
		u32CryptoFieldLengths[u32FieldID] != u32FieldLen)
		return -1;

	switch (u32FieldID)
		{
		case ID_AES_128_KEY:
		    stCryptoFields.bAES128KeySet = TRUE;
			memcpy(&stCryptoFields.au8AES128Key, au8FieldData, u32FieldLen);
			break;

		case ID_AES_256_KEY:
		    stCryptoFields.bAES256KeySet = TRUE;
			memcpy(&stCryptoFields.au8AES256Key, au8FieldData, u32FieldLen);
			break;

		case ID_AES_128_IV:
			stCryptoFields.bAES128IVSet = TRUE;
			memcpy(&stCryptoFields.au8AES128IV, au8FieldData, u32FieldLen);
			break;

		case ID_AES_256_IV:
			stCryptoFields.bAES256IVSet = TRUE;
			memcpy(&stCryptoFields.au8AES256IV, au8FieldData, u32FieldLen);
			break;

		case ID_SHA1_HMAC_SECRET:
			stCryptoFields.bHMACSecretSet = TRUE;
			memcpy(&stCryptoFields.au8HMACSecret, au8FieldData, u32FieldLen);
			break;

		case ID_SEQUENCE_NUMBER:
			stCryptoFields.bSequenceNumberSet = TRUE;
			memcpy(&stCryptoFields.au8SequenceNumber, au8FieldData, u32FieldLen);
			break;

		default:
			return -1;
		}

	return 0;

	} // BeceemAPI_EAP_SetCryptoField

void BeceemAPI_AbortAuth(BeceemAPI_t hAPI, BOOL bSendEAPComplete)
	{

	BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_TIMEOUT);
	BeceemAPI_Deactivate(hAPI, ID_EVENT_NET_ENTRY_FAILURE);
	if (bSendEAPComplete)
		BeceemAPI_EAP_Complete(hAPI); 

	BeceemAPI_EAP_Close(hAPI);

	} /* AbortAuth */

// -------------------------------------------------------------------------
//
// STRING BeceemAPI_DecodeAuthState (INT32 i32AuthState)
//
// Description: Decodes the authentication state;
//
// Returns: Authentication state description, as a string stored in a static buffer.
//
// WARNING: This function is not re-entrant.
// TO DO: Make this re-entrant
// -------------------------------------------------------------------------

STRING BeceemAPI_DecodeAuthState (INT32 i32AuthState)
	{

	STRING bszRet = NULL; 
	static B_CHAR bszBuffer[128];

	switch (i32AuthState)
		{

		// -----------------------------------------------
		// Authentication is off
		case EAP_AUTH_DISABLED:
			bszRet = S("Authentication is disabled");
			break;

		case EAP_AUTH_OFF:
			bszRet = S("EAP supplicant is OFF");
			break;

		// -------------------------------------------------
		// Authentication progress steps
		case EAP_LOGIN_STARTED:
			bszRet = S("Login has started");
			break;

		case EAP_AUTH_WAIT_SUPPLICANT:
			bszRet = S("Waiting for EAP supplicant initialization");
			break;

		case EAP_AUTH_STARTING:
			bszRet = S("EAP supplicant is starting");
			break;

		case EAP_AUTH_IN_PROGRESS:
			bszRet = S("Authorization in progress (thread started)");
			break;

		// ----------------------------------------
		// Authentication Results
		case EAP_AUTH_SUCCESS:
			bszRet = S("SUCCESS");
			break;

		case EAP_AUTH_INIT_ERROR:
			bszRet = S("EAP supplicant initialization error");
			break;

		case EAP_AUTH_KEY_ERROR:
			bszRet = S("Authentication failed (key generation)");
			break;

		case EAP_AUTH_TIMED_OUT:
			bszRet = S("Authentication timed out");
			break;

		case EAP_AUTH_RADIUS_REJECT:
			bszRet = S("AAA server rejected access request");
			break;

		case EAP_AUTH_KEY_MISMATCH:
			bszRet = S("MSK vs AAA server key mismatch");
			break;

		case EAP_AUTH_NOT_OPEN:  // Program error!
			bszRet = S("EAP supplicant is not open (Program ERROR)");
			break;

		case EAP_REAUTH_WAIT:
			bszRet = S("Waiting for EAP re-authentication");
			break;

		default:
			
			break;
		}

	if (bszRet)
		return StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
				bszRet);
	else 
		return StrPrintf(bszBuffer, BCHARSOF(bszBuffer), 
				S("Unknown return code (%d)"), i32AuthState);

	} /* BeceemAPI_DecodeAuthState */

// ----------------------------------------------------------------------
// Initialize the EAP Supplicant interface
//
int BeceemAPI_EAP_Init (
	BeceemAPI_t hAPI,
	ST_EAP_INIT_PARAMS *pstEAPInitParams)
{
	
	int ret;
	struct eapol_debug_print_control opt;
	char * wpa_supplicant_conf;
	char * beceem_engine_file_path;
	STRING bszConfig;
	BOOL bTemp;

	// Zero out the cryptographic structure
	ZeroCryptoFields();

	// Set supplicant type 
	bTemp = Options_GetBoolByEntry(hAPI->pOptFirmwareRSAPrivateKeyEncrypt, NULL);
	BeceemAPI_SetWiMAXOptionsRequest(hAPI, 
		RSA_PRIVATE_KEY_ENABLE_OPTION,
		bTemp ? RSA_PRIVATE_KEY_SPLIT_SUPPLICANT : RSA_PRIVATE_KEY_FULL_HOST_SUPPLICANT);

	// wpa_debug_level values are:
	//   MSG_MSGDUMP = 0
    //   MSG_DEBUG
    //	 MSG_INFO
    //	 MSG_WARNING
	//   MSG_ERROR 	 = 4
    //   >4 is OFF
	// opt.wpa_debug_level      = MSG_INFO;
	// opt.wpa_debug_level      = MSG_MSGDUMP;
#define clip(var, lower, upper) (var > upper ? upper : (var < lower ? lower : var))
	opt.wpa_debug_level		 = 5 - pstEAPInitParams->u32AuthLogLevel;
	opt.wpa_debug_level      = clip(opt.wpa_debug_level, 0, 5);

	// wpa_debug_show_keys values are 0=no and 1=yes
	opt.wpa_debug_show_keys  = opt.wpa_debug_level <= MSG_MSGDUMP ? 1 : 0;           

	// hostapd_logger_level values are:
	//  HOSTAPD_LEVEL_DEBUG_VERBOSE = 0 (includes message dumps)
	//  HOSTAPD_LEVEL_DEBUG
	//  HOSTAPD_LEVEL_INFO
	//  HOSTAPD_LEVEL_NOTICE
	//  HOSTAPD_LEVEL_WARNING		= 4
    //  >4 is OFF
 	// opt.hostapd_logger_level = HOSTAPD_LEVEL_INFO;  
 	// opt.hostapd_logger_level = HOSTAPD_LEVEL_DEBUG_VERBOSE;  
	opt.hostapd_logger_level  = 5 - pstEAPInitParams->u32RADIUSClientLogLevel;
	opt.hostapd_logger_level  = clip(opt.hostapd_logger_level, 0, 5);

	if(pstEAPInitParams->bEnableAuthSysLogPrints) //if the config param EnableAuthSyslogPrints is enabled.
		opt.wpa_debug_syslog = 1;
	else
		opt.wpa_debug_syslog = 0;

	// Make a local copy
	g_bAuthEnabled          = hAPI->bAuthEnabled;
	g_bAuthEthernetToRADIUS = hAPI->bAuthEthernetToRADIUS;

	// Initialize the EAP supplicant
	hAPI->bEAPIsOpen    = FALSE;
	if (pstEAPInitParams->bszAuthLogFileName == NULL || *pstEAPInitParams->bszAuthLogFileName == B_NUL)
		{
		opt.wpa_debug_level      = 5;
		opt.hostapd_logger_level = 5;
		opt.debug_filename       = NULL;
		}
	else
		{
		UINT32 u32MaxLogFileSize = ONE_MEGA_BYTES * Options_GetUint32ByEntry(hAPI->pOptAuthLogFileMaxSizeMB, NULL);
		struct stat buffer;
		INT32 i32statRet;

		opt.debug_filename = StringToCharStar(pstEAPInitParams->bszAuthLogFileName);
		i32statRet= stat(opt.debug_filename, &buffer);
		if(i32statRet == 0 && u32MaxLogFileSize > 0 && (UINT32)buffer.st_size >= u32MaxLogFileSize)
			LogFile_RenameAsOldLog(opt.debug_filename);
		}
	
	// Note: The result of this call must be free's when no longer
	//       needed.
	bszConfig = 
		BeceemAPI_EAPSupplicantConfig(
			hAPI,
			pstEAPInitParams);
	wpa_supplicant_conf = StringToCharStar(bszConfig);
	WcmFree(bszConfig);
	beceem_engine_file_path = StringToCharStar(pstEAPInitParams->bszBeceemEngineFileName);

	if (opt.debug_filename      == NULL  ||
		wpa_supplicant_conf     == NULL ||
		beceem_engine_file_path == NULL)
		{
		ret = EAP_SUPPLICANT_MALLOC_FAILED;
		}
	else
		{
		WcmMutex_Wait(hAPI->hMutexEAPSupplicant);
		ret = eap_supplicant_open(
								wpa_supplicant_conf,
								pstEAPInitParams->u32EloopPollingIntervalMs,
								pstEAPInitParams->u32AuthenticationTimeoutSec,
								&opt,
								(void *) hAPI,
								BeceemAPI_EAP_FromEAPSupplicant,
								beceem_engine_file_path,
								BeceemAPI_EAP_RSAPrivateEncrypt
								);
		WcmMutex_Release(hAPI->hMutexEAPSupplicant);
		}

	WcmFree(opt.debug_filename);
	WcmFree(wpa_supplicant_conf);
	WcmFree(beceem_engine_file_path);

	if (ret)
		return ret;
	hAPI->bEAPIsOpen = TRUE;

	// Intialize the RADIUS server connection
	hAPI->bRADIUSIsOpen = FALSE;
	if (pstEAPInitParams->bAuthEthernetOverRADIUS &&
		b_strlen(pstEAPInitParams->bszRADIUSIPAddress) > 0 && b_strlen(pstEAPInitParams->bszRADIUSSecret) > 0)
		{
		char * szIP = StringToCharStar(pstEAPInitParams->bszRADIUSIPAddress);
		char * szSecret = StringToCharStar(pstEAPInitParams->bszRADIUSSecret);
		if (szIP != NULL && szSecret != NULL)
			{
			WcmMutex_Wait(hAPI->hMutexEAPSupplicant);
			eap_radius_init(
				szIP,
				(int) pstEAPInitParams->u32RADIUSPort, 
				szSecret,
				BeceemAPI_EAP_FromRADIUSServerToSupplicant, 
				opt.hostapd_logger_level);
			WcmMutex_Release(hAPI->hMutexEAPSupplicant);

			hAPI->bRADIUSIsOpen = TRUE;
			}
		WcmFree(szIP);
		WcmFree(szSecret);
		}

	return 0;

	} /* BeceemAPI_EAP_Init */

// ----------------------------------------------------------------------
// Set the user's Identity and Password
//
void BeceemAPI_EAP_Login (BeceemAPI_t hAPI, STRING bszIdentity, STRING bszPassword)
	{

	char * szIdentity;
	char * szPassword;

	if (!hAPI->bEAPIsOpen)
		return;

	b_strncpy(hAPI->bszCurrentIdentity, bszIdentity, BCHARSOF(hAPI->bszCurrentIdentity));
	b_strncpy(hAPI->bszCurrentPassword, bszPassword, BCHARSOF(hAPI->bszCurrentPassword));

	szIdentity = StringToCharStar (bszIdentity);
	szPassword = StringToCharStar (bszPassword);

	if (szIdentity == NULL || szPassword == NULL)
		{
		PrintDiag(DIAG_ERROR, hAPI, S("Memory allocation failed in BeceemAPI_EAP_Login\n"));
		}
	else
		{
		WcmMutex_Wait(hAPI->hMutexEAPSupplicant);
		eap_supplicant_identity(szIdentity, szPassword);
		WcmMutex_Release(hAPI->hMutexEAPSupplicant);
		}

	WcmFree(szIdentity);
	WcmFree(szPassword);

	} /* BeceemAPI_EAP_Login */

// ----------------------------------------------------------------------
// BeceemAPI_EAP_Authenticate() 
// Note: This function is run as a separate thread
//
CSCM_THREAD_FUNCTION BeceemAPI_EAP_Authenticate(void *pvThreadParam)
	{
	int i32WiMAXMode;
	int i32GenFakeRequestIdentity = 0;
	int key_len;
	BeceemAPI_t hAPI = (BeceemAPI_t) pvThreadParam;

	if (!hAPI)
		return (WcmThreadReturn_t) 1;

	i32WiMAXMode = hAPI->bAuthEthernetToRADIUS ? 0 : 1;

	if (hAPI->bEAPIsOpen)
		{

		UINT8 *pu8MSK0 = &hAPI->au8MasterSessionKey[0];
		hAPI->bThreadAuthenticationExists = TRUE;

		hAPI->au32SessionKeyLength = 0;

		if (hAPI->i32ReauthenticationState == STATE_REAUTH_WAITING)
			hAPI->i32ReauthenticationState = STATE_REAUTH_AUTHENTICATING;

		hAPI->i32AuthenticationState = EAP_AUTH_IN_PROGRESS;

		// Must NOT use the EAP Supplicant Mutex here, so that EAP_Close() can
		// forcefully terminate the authentication. Otherwise, the CSCM will
		// hang. 

		key_len = MSK_LEN + EMSK_LEN;
		hAPI->i32AuthenticationState = eap_supplicant_authenticate(pu8MSK0, key_len, 
			i32WiMAXMode, i32GenFakeRequestIdentity);
		hAPI->bThreadAuthenticationExists = FALSE;
		return 0;
		}
	else
		hAPI->i32AuthenticationState = EAP_AUTH_NOT_OPEN;

	hAPI->bThreadAuthenticationExists = FALSE;
	return (WcmThreadReturn_t) 1;

	} /* BeceemAPI_EAP_Authenticate */

// ----------------------------------------------------------------------
// BeceemAPI_EAP_StartThread() 
// Starts the authentication thread
//
// returns TRUE, if thread started successfully; otherwise, FALSE
BOOL BeceemAPI_EAP_StartThread(BeceemAPI_t hAPI)
	{	

	BOOL bStarted = FALSE;
	UINT32 u32ThreadID;
	int n;

	PrintDiag(DIAG_NORMAL,hAPI, S("Starting EAP supplicant thread.\n"));

	hAPI->threadAuthentication = WcmThread_Create(BeceemAPI_EAP_Authenticate, hAPI, &u32ThreadID);
	hAPI->u32AuthStartCount++;
	
	for (n=0; 
		n<(EAP_SUPPLICANT_START_TIMEOUT_MS/EAP_SUPPLICANT_START_SLEEP_MS);
		n++)
		{
		// Handle authentication abort request
		if (!hAPI->bEAPIsOpen)
			break;

		SleepMs(EAP_SUPPLICANT_START_SLEEP_MS);
		// Do not use the EAP Supplicant mutex here, because this is called
		// during authentication.
		if (is_ieee802_1x_rx_ready())
			{
			bStarted = TRUE;
			break;
			}
		}

	if (!hAPI->bEAPIsOpen)
		PrintDiag(DIAG_NORMAL, hAPI, S("EAP Thread start process was aborted\n"));
	else if (bStarted)
		PrintDiag(DIAG_NORMAL, hAPI, S("EAP Supplicant is running.\n"));
	else
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: EAP Supplicant did not start in %d ms\n"), EAP_SUPPLICANT_START_TIMEOUT_MS);

	return bStarted;

	} /* BeceemAPI_EAP_StartThread */

// ----------------------------------------------------------------------
// BeceemAPI_EAP_Complete() 
//
void BeceemAPI_EAP_Complete(BeceemAPI_t hAPI)
	{
	
	
	if (!hAPI->bEAPIsOpen)
		return;

	if (hAPI->i32AuthenticationState == EAP_AUTH_SUCCESS)
		{
		UINT8 pu8MSK0[MSK_LEN];
		memcpy(pu8MSK0, hAPI->au8MasterSessionKey, MSK_LEN);
		if (hAPI->bAuthEthernetToRADIUS)
			// The following line is used for debug purposes only
			BeceemAPI_SetState(hAPI, STATE_WAIT_NET_ENTRY_CMD);
		else
			{
			if (hAPI->bInvertMSKByteOrder)
				{
				UINT32 i, j;
				UINT8 u8Temp;
				for (i=0; i<MSK_LEN / 2; i++)
					{
					j = MSK_LEN - i - 1;
					u8Temp   = pu8MSK0[i];
					pu8MSK0[i] = pu8MSK0[j];
					pu8MSK0[j] = u8Temp;
					}
				}

			BeceemAPI_EAPCompleteRequest(hAPI, 0, pu8MSK0, MSK_LEN);
			hAPI->au32SessionKeyLength = MSK_LEN + EMSK_LEN;
			}
		}
	else
		{
		INT16 i16AuthRetCode = (INT16) hAPI->i32AuthenticationState;
		if (hAPI->bAuthEthernetToRADIUS)
			// The following line is used for debug purposes only
			BeceemAPI_SetState(hAPI, STATE_WAIT_NET_ENTRY_CMD);
		else
			BeceemAPI_EAPCompleteRequest(hAPI, 1, (UINT8 *) &i16AuthRetCode, 2);
		}

	} /* BeceemAPI_EAP_Complete */

// ----------------------------------------------------------------------
// Close the EAP supplicant and RADIUS interfaces
//
void BeceemAPI_EAP_Close(BeceemAPI_t hAPI)
	{

	BOOL bRADIUSIsOpen_local;
	BOOL bEAPIsOpen_local;

	// Wait for all pending EAP supplicant traffic to finish
	WcmMutex_Wait(hAPI->hMutexEAPTraffic);

	bRADIUSIsOpen_local = hAPI->bRADIUSIsOpen;
	bEAPIsOpen_local    = hAPI->bEAPIsOpen;
	hAPI->bEAPIsOpen    = FALSE;
	hAPI->bRADIUSIsOpen = FALSE;

	// Wait for access to the EAP supplicant's functions
	WcmMutex_Wait(hAPI->hMutexEAPSupplicant);

	if (bRADIUSIsOpen_local)
		eap_radius_close();
		
	hAPI->bszCurrentRADIUSIPAddress = EMPTY_STRING;
	hAPI->u32CurrentRADIUSPort	    = 0;
	hAPI->bszCurrentRADIUSSecret	= EMPTY_STRING;

	if (bEAPIsOpen_local) 
		eap_supplicant_close();

	hAPI->i32AuthenticationState = hAPI->bAuthEnabled ? EAP_AUTH_OFF : EAP_AUTH_DISABLED;
	hAPI->i32ReauthenticationState = STATE_REAUTH_OFF;
	hAPI->bszCurrentIdentity[0] = B_NUL;
	hAPI->bszCurrentPassword[0] = B_NUL;

	// Wait for thread to exit and then kill it
	if (bEAPIsOpen_local)
		WcmThread_Destroy(hAPI->threadAuthentication);
	hAPI->threadAuthentication = (WcmThread_t) NULL;

	WcmMutex_Release(hAPI->hMutexEAPTraffic);
	WcmMutex_Release(hAPI->hMutexEAPSupplicant);

	// Zero crypto storage
	ZeroCryptoFields();

	} /* BeceemAPI_EAP_Close */

// ----------------------------------------------------------------------
// Enqueue a CSCM-generated EAP Request Identity packet, as if it had been
// sent by the base station.
//
BOOL BeceemAPI_EnqueueEAPRequestIdentityPacket(BeceemAPI_t hAPI)
	{

	WIMAX_MESSAGE sWiMAXMessage;
	PEAP_PAYLOAD pstEAPPayload;

	sWiMAXMessage.usType    = TYPE_AUTHENTICATION;
	sWiMAXMessage.usSubType = SUBTYPE_EAP_TRANSFER_INDICATION;
	memset(sWiMAXMessage.szData, 0, MAX_VARIABLE_LENGTH);

	pstEAPPayload = (PEAP_PAYLOAD) sWiMAXMessage.szData;

	pstEAPPayload->usEAPPayloadLength = (UINT16) build_EAP_request_identity_packet(pstEAPPayload->aucEAPPayload, 
		(UINT16) (MAX_VARIABLE_LENGTH - sizeof(pstEAPPayload->usEAPPayloadLength)));

	if (pstEAPPayload->usEAPPayloadLength > 0)
		{
		Enqueue(hAPI, eFromCSCM, (void *) &sWiMAXMessage, 
			sizeof(sWiMAXMessage) + sizeof(pstEAPPayload->usEAPPayloadLength) + pstEAPPayload->usEAPPayloadLength
			- MAX_VARIABLE_LENGTH);
		return TRUE;
		}
	else
		return FALSE;

	} /* BeceemAPI_EnqueueEAPRequestIdentityPacket */

static void ZeroCryptoFields(void)
	{

	memset(&stCryptoFields, 0, sizeof(stCryptoFields));

	} /* ZeroCryptoFields */
