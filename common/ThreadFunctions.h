/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------
// System-dependent functions for threads, events, 
// semaphores, and mutexes: WIN32
// -------------------------------------------------------


#ifndef THREAD_FUNCTIONS_H
#define THREAD_FUNCTIONS_H

#define CSCM_EVENT_MAX_WAIT 0x7FFFFFFF

// ---------------------------------------------------------------------
// WIN32 definitions
// ---------------------------------------------------------------------

#ifdef WIN32

// The following is needed to enable CreateWaitableTimer() and its related functions
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>

typedef  HANDLE WcmMutex_t;
typedef  HANDLE WcmSemaphore_t;
typedef  HANDLE WcmEvent_t;
typedef  HANDLE WcmThread_t;
typedef  ULONG  WcmThreadReturn_t;

#define  CSCM_THREAD_FUNCTION ULONG __stdcall
typedef  ULONG  (__stdcall *WcmThreadProc_t) (void *pvThreadParameter);
#endif

#ifdef LINUX_OR_OSX

// ---------------------------------------------------------------------
// LINUX/OSX definitions
// ---------------------------------------------------------------------

#include <semaphore.h>
#include <pthread.h>

typedef  pthread_t        WcmThread_t;
typedef  pthread_mutex_t *WcmMutex_t;
typedef  struct {
	pthread_mutex_t	mutexEvent;
	pthread_cond_t	condEvent;
	} stWcmEvent_t;
typedef stWcmEvent_t *WcmEvent_t;
typedef  struct {
	UINT32     		u32Count;
	pthread_mutex_t hMutex;
	pthread_cond_t	condSem;
	} stWcmSemaphore_t;
typedef stWcmSemaphore_t *WcmSemaphore_t;
typedef void * WcmThreadReturn_t;

#define  CSCM_THREAD_FUNCTION  void *
typedef  void * (*WcmThreadProc_t) (void *pvThreadParameter);

#endif

// Mutexes
// bszName may be NULL
WcmMutex_t WcmMutex_Create(void);
void WcmMutex_Destroy(WcmMutex_t hMutex);
void WcmMutex_Wait(WcmMutex_t hMutex);
BOOL WcmMutex_Release(WcmMutex_t hMutex);

// Semaphores
WcmSemaphore_t WcmSemaphore_Create(void);
void WcmSemaphore_Destroy(WcmSemaphore_t hSem);
void WcmSemaphore_Wait(WcmSemaphore_t hSem);
BOOL WcmSemaphore_Release(WcmSemaphore_t hSem);

// Events
WcmEvent_t WcmEvent_Create(void);
void WcmEvent_Destroy(WcmEvent_t hEvent);
BOOL WcmEvent_Set(WcmEvent_t hEvent);
BOOL WcmEvent_Wait(WcmEvent_t hEvent, UINT32 u32MillisecondTimeout);

// Threads
WcmThread_t WcmThread_Create(WcmThreadProc_t pfThreadFunction, void *pvThreadParam, UINT32 *pu32ThreadID);
void   WcmThread_Destroy(WcmThread_t hThread);
WcmThreadReturn_t WcmThread_Exit(UINT32 u32Status);
void   WcmThread_WaitForThreadExit (WcmThread_t hThread);

// Date-time stamp
STRING DateTimeStamp(STRING bszBuffer, UINT32 u32BcharCount);

// Sleep
void SleepMs(UINT32 u32Milliseconds);

#endif

