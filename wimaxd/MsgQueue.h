/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

// =========================================================================================
// Message Queue
// This bridges the DLL (native) to the rest of the program (CLR)
typedef enum {eFromWiMAXAPI, eFromEAPSupplicant, eFromCSCM, eAbortThread} eSourceType;
typedef struct {
	void	  * pvAPI;			// Must be cast to BeceemAPI_t
	eSourceType eSource;		// indicates message origin: WiMAXAPI DLL or eap_supplicant DLL.
	UINT8       u8Status;
	void      * pu8Message;
	UINT32      u32Length;
	void      * pNextEntry;
	} stQueueEntry_t;
typedef stQueueEntry_t *hQueueEntry_t;
void CSCMCallback (void *pvAPI, void *pBCMMessage, UINT32 ulBCMMessageLength);
BOOL InitQueue (void *pvAPI);
BOOL IsQueueEmpty (void *pvAPI);
BOOL Enqueue (void *pvAPI, eSourceType eSource, void *pBCMMessage, UINT32 ulBCMMessageLength);
hQueueEntry_t Dequeue (void *pvAPI);

hQueueEntry_t CreateEntry (
    void      * pvAPI,
	eSourceType eSource,
	UINT8       u8Status,
	void      * pu8Message,
	UINT32      u32Length);
BOOL FreeEntry (hQueueEntry_t hEntry);

#define QUEUE_ENTRY_OK			0
#define QUEUE_ENTRY_NULL_BUFFER 1

#endif

