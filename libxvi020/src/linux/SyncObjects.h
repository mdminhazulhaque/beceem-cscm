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
 * Description	:	Synchronization primitives APIs for Linux.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef SYNC_OBJECTS
#define SYNC_OBJECTS

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "Typedefs.h"
#include "ErrorDefs.h"
#include "Sem.h"

#define INFINITE 0xFFFFFFFF
#define MAX_SEMNAME_SIZE 100
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
#define USING_SYS_V_SEM_IPC
#endif
/**************************** MUTEX - Binary semaphore ****************************/
#ifdef USING_PTHREAD_MUTEX
typedef struct  _MUTEX
{
	pthread_mutex_t m_Mutex;
}MUTEX, *PMUTEX;
#endif

void Mutex_Init(void *pArg);
void Mutex_Cleanup(void *pArg);

int Mutex_Create(void *pArg, const char *szName);
void Mutex_Lock(void *pArg);
void Mutex_Unlock(void *pArg);


/**************************** EVENTS ****************************/
#ifdef USING_SYS_V_SEM_IPC
typedef struct _ST_MUTEX_SYS_V
{
key_t key;
int semid;
int iIndex;
}ST_MUTEX_SYS_V,*P_STMUTEX_SYS_V;

#ifdef LINUX
union semun 
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};
#endif

#endif


#ifndef USING_PTHREAD_MUTEX
typedef struct _EVENT
{
    #ifdef MACOS
    	sem_t *m_EventSem;
    #else
    	sem_t m_EventSem;
    #endif
        pthread_mutex_t mutexobj;
    	char cSemName[MAX_SEMNAME_SIZE];
    	BOOL m_bValidSemaphore;
     #ifdef USING_SYS_V_SEM_IPC
         ST_MUTEX_SYS_V  stSem;     
     #endif 
    }EVENT, *PEVENT,MUTEX,*PMUTEX;
#else
    typedef struct _EVENT
    {
    #ifdef MACOS
    	sem_t *m_EventSem;
    #else
    	sem_t m_EventSem;
    #endif
    	char cSemName[MAX_SEMNAME_SIZE];
    	BOOL m_bValidSemaphore;
    }EVENT, *PEVENT;
#endif


void Event_Init(void *pArg);
void Event_Cleanup(void *pArg);

int Event_Create(void *pArg, BOOL bManualReset, BOOL bInitialState,	const char *szName);
int Event_Open(void *pArg, const char *szName);
int Event_Set(void *pArg);
int Event_Reset(void *pArg);
int Event_Wait(void *pArg, UINT32 ulWaitTimeOut);

UINT32 GetCurrentMicroSecs();
void SyS_V_Sem_Init(void *pArg);
void SyS_V_Sem_Cleanup(void *pArg);
void SyS_V_Sem_UnLock(void *pArg);
void SyS_V_Sem_Lock(void *pArg);
int Create_SyS_V_Sem(void *pArg, const int iKeyValue,int iNumSems);


#endif /* SYNC_OBJECTS */
