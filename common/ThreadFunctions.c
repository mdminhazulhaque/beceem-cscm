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

// #define TEST_SYSTEM_FUNCTIONS

#include "CscmTypes.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
// The following is needed to enable CreateWaitableTimer() and its related functions
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>
#endif

#ifdef LINUX_OR_OSX
#define __BSD_SOURCE
#include <sys/time.h>
#include <unistd.h>
#endif

#include "ThreadFunctions.h"
#include "Utility.h"

// -----------------------------------------------------------------------------------------
// Mutex functions
// -----------------------------------------------------------------------------------------
WcmMutex_t WcmMutex_Create(void)
	{

#ifdef WIN32
	return CreateMutex( 
		NULL,             // default security attributes
		FALSE,            // initially not owned
		NULL);            // May be NULL for un-named mutex
#endif

#ifdef LINUX_OR_OSX
	WcmMutex_t hMutex;

	if ((hMutex = WcmMalloc(sizeof(pthread_mutex_t))) == NULL)
		return NULL;

	pthread_mutex_init(hMutex, NULL);

	return hMutex;
#endif

	} /* WcmMutex_Create */

void WcmMutex_Destroy(WcmMutex_t hMutex)
	{

#ifdef WIN32
	if (hMutex)
		CloseHandle(hMutex);
#endif

#ifdef LINUX_OR_OSX
	if (hMutex)
		{
		pthread_mutex_destroy(hMutex);
		WcmFree(hMutex);
		}
#endif

	} /* WcmMutex_Destroy */

void WcmMutex_Wait(WcmMutex_t hMutex)
	{

#ifdef WIN32
	// no timeout on wait
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);
#endif

#ifdef LINUX_OR_OSX
	if (hMutex)
		pthread_mutex_lock(hMutex);
#endif

	} /* WcmMutex_Wait */

BOOL WcmMutex_Release(WcmMutex_t hMutex)
	{

#ifdef WIN32
	// TO DO: Handle error returns
	if (hMutex)
		return ReleaseMutex(hMutex);
	else
		return FALSE;
#endif

#ifdef LINUX_OR_OSX
	if (hMutex)
		return pthread_mutex_unlock(hMutex) == 0;
	else
		return FALSE;
#endif

	} /* WcmMutex_Release */

// -----------------------------------------------------------------------------------------
// Semaphore functions
// -----------------------------------------------------------------------------------------
WcmSemaphore_t WcmSemaphore_Create(void)
	{
#ifdef WIN32
	return CreateSemaphore(NULL, 0, 1000, NULL);
#endif
#ifdef LINUX_OR_OSX
	WcmSemaphore_t hSem;
	if ((hSem = WcmMalloc(sizeof(stWcmSemaphore_t))) == NULL)
		return NULL;
	hSem->u32Count = 0;
	if(pthread_mutex_init(&hSem->hMutex, NULL) != 0)
	{
		WcmFree(hSem);
		return NULL;
	}
	if(pthread_cond_init(&hSem->condSem, NULL) != 0)
	{
		pthread_mutex_destroy(&hSem->hMutex);
		WcmFree(hSem);
		return NULL;
	}
	return hSem;
#endif
	} /* WcmSemaphore_Create */

void WcmSemaphore_Destroy(WcmSemaphore_t hSem)
	{
#ifdef WIN32
	if (hSem)
		CloseHandle(hSem);
#endif
#ifdef LINUX_OR_OSX
	if (hSem)
		{
		pthread_cond_destroy(&hSem->condSem);		
		pthread_mutex_destroy(&hSem->hMutex);
		WcmFree(hSem);
		}
#endif
	} /* WcmSemaphore_Destroy */

void WcmSemaphore_Wait(WcmSemaphore_t hSem)
	{
#ifdef WIN32
	if (hSem)
		WaitForSingleObject(hSem, INFINITE); // no timeout on wait
#endif

#ifdef LINUX_OR_OSX
	BOOL bWait = FALSE;
	if (hSem)
		{
		pthread_mutex_lock(&hSem->hMutex);
		bWait = hSem->u32Count == 0;
		if (bWait) 
			pthread_cond_wait(&hSem->condSem, &hSem->hMutex);
		if (hSem->u32Count != 0)	
			hSem->u32Count--;
		pthread_mutex_unlock(&hSem->hMutex);
		}
#endif
	
	} /* WcmSemaphore_Wait */

BOOL WcmSemaphore_Release(WcmSemaphore_t hSem)
	{

	BOOL bRetVal = FALSE;
	
	if (!hSem)
		return FALSE;

#ifdef WIN32
	bRetVal = ReleaseSemaphore(hSem, 1, NULL) != 0;
#endif
#ifdef LINUX_OR_OSX
	pthread_mutex_lock(&hSem->hMutex);
	hSem->u32Count++;
	if (hSem->u32Count == 1)
		pthread_cond_signal(&hSem->condSem);
	pthread_mutex_unlock(&hSem->hMutex);
	bRetVal = TRUE;
#endif

	return bRetVal;

	} /* WcmSemaphore_Release */

// -----------------------------------------------------------------------------------------
// Event functions
// -----------------------------------------------------------------------------------------
WcmEvent_t WcmEvent_Create(void)
	{

#ifdef WIN32
	return CreateEvent(
		NULL,   // Use default security permissions
		FALSE,  // Use automatic reset
		FALSE,  // Initialize to non-signaled
		NULL    // No name specified
		);
#endif

#ifdef LINUX_OR_OSX
	WcmEvent_t hEvent;

	if ((hEvent = WcmMalloc(sizeof(stWcmEvent_t))) == NULL)
		return NULL;

	if (pthread_mutex_init(&hEvent->mutexEvent, NULL) != 0 ||
		pthread_cond_init(&hEvent->condEvent, NULL)   != 0)
		return NULL;

	return hEvent;
#endif

	} /* WcmEvent_Create */

void WcmEvent_Destroy(WcmEvent_t hEvent)
	{

#ifdef WIN32
	if (hEvent)
		CloseHandle(hEvent);
#endif

#ifdef LINUX_OR_OSX
	if (hEvent)
		{
		pthread_cond_destroy(&hEvent->condEvent);
		pthread_mutex_destroy(&hEvent->mutexEvent);
		WcmFree(hEvent);
		}
#endif
	
	} /* WcmEvent_Destroy */

BOOL WcmEvent_Set(WcmEvent_t hEvent)
	{
#ifdef WIN32
	return SetEvent(hEvent) != 0;
#endif

#ifdef LINUX_OR_OSX
	pthread_mutex_lock(&hEvent->mutexEvent);
	pthread_cond_signal(&hEvent->condEvent);
	pthread_mutex_unlock(&hEvent->mutexEvent);
	return TRUE;
#endif

	} /* WcmEvent_Set */

// Returns TRUE if the event occurred; FALSE, on timeout or error
BOOL WcmEvent_Wait(WcmEvent_t hEvent, UINT32 u32MillisecondTimeout)
	{
	BOOL bHasOccurred = FALSE;

#ifdef WIN32
	bHasOccurred = WaitForSingleObject(hEvent, u32MillisecondTimeout) == WAIT_OBJECT_0;
#endif

#ifdef LINUX_OR_OSX
#define ONE_E9  1000000000
	struct timespec ts;
#if defined(__APPLE__)
	struct timeval tv;
#endif
	UINT32 u32Sec;
	UINT32 u32Ms;
	pthread_mutex_lock(&hEvent->mutexEvent);
	if (u32MillisecondTimeout != CSCM_EVENT_MAX_WAIT)
		{

		// Compute the absolute time at the end of the wait period
#if !defined(__APPLE__)
		clock_gettime(CLOCK_REALTIME, &ts);
#else
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000;
#endif
		u32Sec = u32MillisecondTimeout / 1000;
		u32Ms  = u32MillisecondTimeout % 1000;

		ts.tv_nsec += u32Ms * 1000000;
		if (ts.tv_nsec > ONE_E9)
			{
			ts.tv_sec++;
			ts.tv_nsec -= ONE_E9;
			}
		ts.tv_sec += u32Sec;


		// Wait
		bHasOccurred = pthread_cond_timedwait(&hEvent->condEvent, &hEvent->mutexEvent, &ts) == 0;
		}
	else
		bHasOccurred = pthread_cond_wait(&hEvent->condEvent, &hEvent->mutexEvent) == 0;
	pthread_mutex_unlock(&hEvent->mutexEvent);
#endif

	return bHasOccurred;

	} /* WcmEvent_Wait */

// -----------------------------------------------------------------------------------------
// Thread functions
// -----------------------------------------------------------------------------------------

WcmThread_t WcmThread_Create(WcmThreadProc_t pfThreadFunction, void *pvParam, UINT32 *pu32ThreadID)
	{

	WcmThread_t hThread;

#ifdef WIN32
	ULONG ulThreadID;

	hThread = CreateThread(
		NULL,
		0,
		pfThreadFunction,
		pvParam,
		0,
		&ulThreadID);

	if (pu32ThreadID)
		*pu32ThreadID = (UINT32) ulThreadID;
#endif

#ifdef LINUX_OR_OSX

	int iRetVal;

	iRetVal = pthread_create(
		&hThread,
		NULL,
		pfThreadFunction,
		pvParam);
	// pthread_detach allows thread stack to be released when the thread exits
	pthread_detach(hThread);

	if (iRetVal)
		hThread = 0;

#endif

	return hThread;

	} /* WcmThread_Create */

void WcmThread_Destroy(WcmThread_t hThread)
	{

	WcmThread_WaitForThreadExit(hThread);

#ifdef WIN32
	if (hThread)
		CloseHandle(hThread);
#endif

	} /* WcmThread_Destroy */

// Called by the thread itself to clean up
WcmThreadReturn_t WcmThread_Exit(UINT32 u32Status)
	{

#ifdef WIN32
	if (u32Status != 0xFFFFFFFF) // Used only to eliminate gratuitous warnings
		ExitThread(u32Status);
	return u32Status;			 // Used only to eliminate gratuitous warnings
#endif

#ifdef LINUX_OR_OSX
	if (u32Status != 0xFFFFFFFF) // Used only to eliminate gratuitous warnings
		pthread_exit(NULL);		 // TO DO: return status possible?
	return NULL;
#endif

	} /* WcmThread_Exit */

void WcmThread_WaitForThreadExit(WcmThread_t hThread)
	{

	ULONG ulExitCode;

	if (!hThread)
		return;

#ifdef WIN32

	ulExitCode = STILL_ACTIVE;

	GetExitCodeThread(hThread, &ulExitCode);
	while (ulExitCode == STILL_ACTIVE)
		{
		Sleep(20);
		GetExitCodeThread(hThread, &ulExitCode);
		}

#endif
#ifdef LINUX_OR_OSX

	// Note: Since all threads have been detached, this call to 
	// pthread_join is superfluous. It may be removed in future
	// CSCM versions.
	ulExitCode = pthread_join(hThread, NULL);

#endif

	} /* WcmThread_WaitForThreadExit */

// -----------------------------------------------------------------------------------------
// Date time stamp
// -----------------------------------------------------------------------------------------
STRING DateTimeStamp(STRING bszBuffer, UINT32 u32BcharCount)
	{

#ifdef WIN32
	SYSTEMTIME st;

	GetLocalTime(&st);

	return StrPrintf(bszBuffer, u32BcharCount, 
		S("%4d/%02d/%02d %02d:%02d:%02d.%03d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	return StrPrintf(bszBuffer, u32BcharCount, 
		S("%4d/%02d/%02d %02d:%02d:%02d.%03d"),
		tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, 
		tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
#endif

	} // DateTimeStamp

void SleepMs(UINT32 u32Milliseconds)
	{

#ifdef WIN32
	Sleep(u32Milliseconds);
#else
	usleep(1000 * u32Milliseconds);
#endif

	}

#ifdef TEST_SYSTEM_FUNCTIONS

#define test_wprintf b_printf

void main(void)
	{

	test_wprintf(S("\nTesting system functions:\n\n"));
	
	test_wprintf(S("Date-time stamp: ") B_SFNL, DateTimeStamp());

	PressEnterToContinue();

	} /* main */

#endif

