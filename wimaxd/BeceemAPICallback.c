/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "BeceemAPI.h"
#include "Utility.h"
#include "SysLogFunctions.h"

#define RESP_MSG(x) 
#define CALLBACK_CONTEXT ((void *) 0x31415922)

#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST
// ----------------------------------------------------------------------------------------------
// RSA private encryption messages require immediate handling, to avoid a deadlock in ProcessMessages()
static void Process_RSAPrivateEncryptResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMaxMsg, UINT32 u32Length)
	{		

	PST_EAP_RSA_PRIVATE pstEAPRsaPrivate = (PST_EAP_RSA_PRIVATE) pstWiMaxMsg->szData;

	if(!pstEAPRsaPrivate)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: EAP Encrypt Private Indication has NULL buffer pointer.\n"));
	else if (pstEAPRsaPrivate->u32DataLen == 0)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: EAP Encrypt Private Indication has 0 data length.\n"));
	else if ((hAPI->au8EncMsgDigest = WcmMalloc(pstEAPRsaPrivate->u32DataLen)) == NULL)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: EAP Encrypt Private Indication memory allocation failed.\n"));
	else
		{
		memcpy(hAPI->au8EncMsgDigest, pstEAPRsaPrivate->u8Data, pstEAPRsaPrivate->u32DataLen);
		hAPI->u32EncMsgDigestLength = pstEAPRsaPrivate->u32DataLen;
		WcmEvent_Set(hAPI->hEventRSAPrivEncryptResponse);  // Let the function know results are ready
		PrintDiag(DIAG_NORMAL, hAPI, S("***** Received EAP Encrypt Private Indication (type %u subtype %u) - %u bytes (") B_SF S(")\n"),
			pstWiMaxMsg->usType, pstWiMaxMsg->usSubType, u32Length, BeceemAPI_DateTime(hAPI));
		}

	} /* Process_RSAPrivateEncryptResponse */
#endif

/* Copy the Certs/Key data. */
static void Process_ReadNVMSecCredsResponse(BeceemAPI_t hAPI, PWIMAX_MESSAGE pstWiMaxMsg, UINT32 u32Length)
{		

	PSEC_OBJECT_INFO pSecData = (PSEC_OBJECT_INFO) pstWiMaxMsg->szData;

	if(!pSecData)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Security Get Response has NULL buffer pointer.\n"));
	else if (pSecData->s8ErrorCode < 0)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Security Get Response has returned error Code:%d\n"), pSecData->s8ErrorCode);
	else if (pSecData->u32SecObjSize == 0)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Security Get Response has returned 0 data length. \n"));
	else if ((hAPI->au8SecCredsData = WcmMalloc(pSecData->u32SecObjSize)) == NULL)
		PrintDiag(DIAG_ERROR, hAPI, S("\n***** ERROR: Process_ReadNVMSecCredsResponse memory allocation failed.\n"));
	else
	{
		memcpy(hAPI->au8SecCredsData, pSecData->au8SecObjVal, pSecData->u32SecObjSize);
		hAPI->u32SecCredsLength= pSecData->u32SecObjSize;
		PrintDiag(DIAG_NORMAL, hAPI, S("***** Received Security Get Response (type %u subtype %u) - %u bytes (") B_SF S(")\n"),
			pstWiMaxMsg->usType, pstWiMaxMsg->usSubType, u32Length, BeceemAPI_DateTime(hAPI));
	}

} /* Process_RSAPrivateEncryptResponse */


// ----------------------------------------------------------------------------------------------
// Messages are just queued for later disposition by the main app. This function is called
// from a separate thread, so it's treated as an "interrupt" service routine, with no 
// GUI interaction.

void CSCMCallback (void *hAPI, void * pBCMMessage, UINT32 ulBCMMessageLength)
{
	PWIMAX_MESSAGE pWiMaxMsg = (PWIMAX_MESSAGE) pBCMMessage;
	// UINT8 * pBuf = (UINT8 *) pBCMMessage; // For debugging only
#ifdef SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST
	if (pWiMaxMsg->usType == TYPE_AUTHENTICATION && 
		pWiMaxMsg->usSubType == SUBTYPE_EAP_ENCRYPT_PRIVATE_RESPONSE)
		Process_RSAPrivateEncryptResponse((BeceemAPI_t) hAPI, pBCMMessage, ulBCMMessageLength);
	else
#endif
/* Copy the Certs/Key data. */
	if (pWiMaxMsg->usType == TYPE_SECURITY && 
		pWiMaxMsg->usSubType == SUBTYPE_SECURITY_GET_RESP)
			Process_ReadNVMSecCredsResponse(hAPI, pBCMMessage, ulBCMMessageLength);
		// Create a new queue entry 
	else
		Enqueue (hAPI, eFromWiMAXAPI, pBCMMessage, ulBCMMessageLength);
	
	return;

} // CSCMCallback

// ----------------------------------------------------------------------------------------------
BOOL InitQueue (void *pvAPI) {

	BeceemAPI_t hAPI = (BeceemAPI_t) pvAPI;

	if (!hAPI)
		return FALSE;

	hAPI->pQueueStart      = NULL;
	hAPI->pQueueEnd        = NULL;
	hAPI->hQueueMutex      = WcmMutex_Create();
	hAPI->hQueueSemaphore  = WcmSemaphore_Create();
	return hAPI->hQueueMutex != NULL && hAPI->hQueueSemaphore != NULL;
	}

// ----------------------------------------------------------------------------------------------
BOOL IsQueueEmpty (void *pvAPI) 
	{
	BOOL bIsEmpty;
	BeceemAPI_t hAPI = (BeceemAPI_t) pvAPI;

	WcmMutex_Wait(hAPI->hQueueMutex);
	bIsEmpty = hAPI->pQueueStart == NULL;
	WcmMutex_Release(hAPI->hQueueMutex);	// Should always return TRUE, but we don't test that condition here

	return bIsEmpty;
	}

// ----------------------------------------------------------------------------------------------
BOOL Enqueue (void *pvAPI, eSourceType eSource, void * pu8Message, UINT32 u32Length) 
	{

	hQueueEntry_t hEntry;
	BeceemAPI_t hAPI = (BeceemAPI_t) pvAPI;

	// DEBUG USE ONLY (to facilitate watches)
	// UINT16 * pMsg = (UINT16 *) pu8Message;
	// END DEBUG

	// Wait for access to the queue
	WcmMutex_Wait(hAPI->hQueueMutex);

	// Create the queue entry
	hEntry = CreateEntry (
		pvAPI,
		eSource, 
		pu8Message != NULL ? QUEUE_ENTRY_OK : QUEUE_ENTRY_NULL_BUFFER, 
		pu8Message, 
		u32Length);

	if (hEntry == NULL)
		{
		WcmMutex_Release(hAPI->hQueueMutex);
		return FALSE;
		}

	// Link the new entry into the queue
	if (hAPI->pQueueStart == NULL)	  // Queue is empty
		{
		hAPI->pQueueStart = hEntry;
		hAPI->pQueueEnd   = hEntry;
		}
	else if (hAPI->pQueueEnd == NULL)  // This should never happen
		{
		WcmMutex_Release(hAPI->hQueueMutex);
		return FALSE;
		}
	else
		{
		hAPI->pQueueEnd->pNextEntry = (void *) hEntry;
		hAPI->pQueueEnd = hEntry;
		}

	// Tell the Process Messages thread that a message is available
	WcmSemaphore_Release(hAPI->hQueueSemaphore);

	// release queue access mutex
	if (WcmMutex_Release(hAPI->hQueueMutex) != 0)
		{
		return TRUE;  // Should always return TRUE 
		}
	else
		return FALSE;

	} // Enqueue (...

// ----------------------------------------------------------------------------------------------
hQueueEntry_t Dequeue (void *pvAPI)
	{

	hQueueEntry_t hEntry;
	BeceemAPI_t hAPI = (BeceemAPI_t) pvAPI;

	// Block the calling thread until there is
	// an item in the queue (lock the mutex)
	WcmSemaphore_Wait(hAPI->hQueueSemaphore);

	// Wait for access to the queue
	WcmMutex_Wait(hAPI->hQueueMutex);

	if (hAPI->pQueueStart == NULL)
		hEntry = NULL;
	
	else
		{

		hEntry = hAPI->pQueueStart;

		hAPI->pQueueStart = (hQueueEntry_t) hAPI->pQueueStart->pNextEntry;
		if (hAPI->pQueueStart == NULL)
			hAPI->pQueueEnd = NULL;

		}

	if (WcmMutex_Release(hAPI->hQueueMutex))
		{
		return hEntry;
		}
	else
		return NULL;			// This should never happen

	} // Dequeue (...

// ----------------------------------------------------------------------------------------------
BOOL FreeEntry (hQueueEntry_t hEntry) {
	
	if (hEntry == NULL)
		return FALSE;

	WcmFree(hEntry->pu8Message);
	WcmFree(hEntry);

	return TRUE;

	} // FreeEntry (...

// ----------------------------------------------------------------------------------------------
hQueueEntry_t CreateEntry (
    void	  * pvAPI,
	eSourceType eSource,		// indicates message origin: WiMAXAPI DLL or eap_supplicant DLL.
	UINT8       u8Status,
	void      * pu8Message,
	UINT32      u32Length) {

	hQueueEntry_t hEntry;
	UINT8 * pu8Buffer;

	if ((hEntry = WcmMalloc(sizeof(stQueueEntry_t))) == NULL)
		return NULL;
		
	// Set the contents
	hEntry->pvAPI     = pvAPI;
	hEntry->eSource   = eSource;
	hEntry->u8Status  = u8Status;
	hEntry->u32Length = u32Length;
	if (pu8Message != NULL)
		{
		if ((pu8Buffer = WcmMalloc(u32Length)) == NULL)
			{
			WcmFree(hEntry);
			return NULL;
			}
#ifdef MS_VISUAL_CPP
		memcpy_s(pu8Buffer, u32Length, pu8Message, u32Length);
#else
		memcpy(pu8Buffer, pu8Message, u32Length);
#endif
		hEntry->pu8Message = pu8Buffer;
		}
	else
		hEntry->pu8Message = NULL;
	hEntry->pNextEntry = NULL;
	
	return hEntry;

	} // CreateEntry (...

