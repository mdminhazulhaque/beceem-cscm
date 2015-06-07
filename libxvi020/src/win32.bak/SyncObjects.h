/*
 * (C) Copyright 2010. Beceem Communications Inc. All rights reserved.
 * 
 * Disclaimer :
 *
 * EXCEPT AS SET FORTH IN BECEEM'S TERMS AND CONDITIONS OF SALE, BECEEM
 * ASSUMES NO LIABILITY WHATSOEVER AND DISCLAIMS ANY EXPRESS, IMPLIED OR
 * STATUTORY WARRANTY RELATING TO ITS PRODUCTS INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL BECEEM BE LIABLE FOR
 * ANY DIRECT, INDIRECT, CONSEQUENTIAL, PUNITIVE, SPECIAL OR INCIDENTAL
 * DAMAGES (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
 * BUSINESS INTERRUPTION OR LOSS OF INFORMATION) ARISING OUT OF THE USE
 * OR INABILITY TO USE THIS DOCUMENT, EVEN IF BECEEM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES. 
 * 
 * Trademarks :
 *
 * Beceem, Beceem Communications, and Mobilizing Broadband are registered
 * trademarks of Beceem Communications Inc.  The Beceem logo is a
 * trademark of Beceem Communications Inc.
 *
 * Description	:	Synchronization object APIs for Windows.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef SYNC_OBJECTS
#define SYNC_OBJECTS

#pragma once
#include <windows.h>
#include "ErrorDefs.h"

/********************************************************************************
*																				*
*						MUTEXT STRUCTURE DEFINITION								*
*																				*
********************************************************************************/

typedef struct _MUTEX
{
	HANDLE hMutex;		/* Mutex Handle */
} MUTEX, *PMUTEX;


int Mutex_Init(PVOID pVoid);
int Mutex_Cleanup(PVOID pVoid);
int Mutex_Create(PVOID pVoid, const char *szName);
void Mutex_Lock(PVOID pVoid);
int Mutex_Unlock(PVOID pVoid);
int Mutex_Release(PVOID pVoid);
int Event_Open(void *pArg, const char *szName);



/********************************************************************************
*																				*
*						EVENT STRUCTURE DEFINITIONS								*
*																				*
********************************************************************************/

typedef struct _EVENT
{
	HANDLE hEvent;		/* CEvent_ Handle */
}EVENT, *PEVENT;


int Event_Init(PVOID pVoid);
int Event_Cleanup(PVOID pVoid);
int Event_Create(
					PVOID pVoid,
					BOOL bManualReset,
					BOOL bInitialState,
					const char *szName
			   );
int Event_Set(PVOID pVoid);
int Event_Reset(PVOID pVoid);
DWORD Event_Wait(PVOID pVoid, DWORD dwWaitTimeOut);
int Event_Release(PVOID pVoid);


#endif /* SYNC_OBJECTS */
