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
 * Description	:	This includes CThread class, wrapper on CreateThread
 * 					and WaitForSingleObject functions
 * Author		:
 * Reviewer		:
 *
 */


#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H


#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>

#include "ErrorDefs.h"
#include "Typedefs.h"

#ifndef PRIVATE_TYPEDEFS_H
typedef UCHAR BOOLEAN,BOOL;
#endif

typedef void *(*PTHREADFUNC)(void *lpThreadParameter);
typedef int (*PTHREADSTOPFUNC)(void *lpThreadParameter);

#ifndef BCM_PTHREAD_CANCEL_NOT_SUPPORTED
#define MAX_KILL_RETRY 10
#else
#define MAX_KILL_RETRY 1
#endif



typedef struct _THREAD
{
	pthread_t m_Thread;
	void *m_pvThreadParam;
	PTHREADFUNC m_pThreadFunc;
	PTHREADSTOPFUNC m_pThreadStopFunc;
	UINT32 m_dwThreadID;
	BOOL m_bIsAlive;
	int m_iThreadID;
	sem_t m_WaitSem;

}THREAD, *PTHREAD;

/* Constructors */
void Thread_Init(void *pThread, void *pvThreadParam);

/* Destructors */
void Thread_Cleanup(void *pThread);

/* Functions */
int Thread_Run(void *pThread, PTHREADFUNC pThreadFunc, void *pvThreadParam);
int Thread_Wait(void *pThread, UINT32  dwWaitMilliSec);
BOOL Thread_IsAlive(void *pThread);
int Thread_Stop(void *pThread, PTHREADSTOPFUNC pStopFunc, void *pvStopParam);
int Thread_Terminate(void *pThread);
BOOL Thread_IsValidThread(void *pThread);
int Thread_SetDebugName(const char* lpszDebugThreadName);


#endif /* THREAD_CLASS_H */
