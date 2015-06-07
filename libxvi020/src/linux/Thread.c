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
 * Description	:	Thread implementation for Linux.
 * Author		:
 * Reviewer		:
 *
 */
#include "BeceemWiMAX.h"

#include "Thread.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include<time.h>	
#include<stdlib.h>

/* private functions */
void thread_cleanup(void *pThread);

void Thread_Cleanup(void *pArg)
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return;

	pThread->m_bIsAlive = FALSE;
	sem_destroy(&pThread->m_WaitSem);   /* Event semaphore handle	*/
}

void Thread_Init(void *pArg, void *pvThreadParam )
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return;

	pThread->m_pvThreadParam = pvThreadParam;
	pThread->m_pThreadFunc = NULL;
	pThread->m_bIsAlive = FALSE;
	sem_init(&pThread->m_WaitSem, 0, 0);
}

int Thread_Run(void *pArg, PTHREADFUNC pThreadFunc, void *pvThreadParam/*=NULL*/)
{
    int iThreadStatus = 0;
	PTHREAD pThread = (PTHREAD)pArg;
    pthread_attr_t   attrs;

	memset(&attrs, 0, sizeof(pthread_attr_t));
	if(!pThread)
		return ERROR_THREAD_CREATION_FAILED;

	pThread->m_pvThreadParam = pvThreadParam;
	pThread->m_pThreadFunc = pThreadFunc;

	if(!pThread->m_pThreadFunc)
		return ERROR_NULL_THREAD_FUNC;

    pthread_attr_init( &attrs );
    pthread_attr_setdetachstate( &attrs, PTHREAD_CREATE_DETACHED );
	iThreadStatus = pthread_create(&pThread->m_Thread, &attrs, pThread->m_pThreadFunc, pThread->m_pvThreadParam);
    pthread_attr_destroy( &attrs );
	pThread->m_bIsAlive = (iThreadStatus==0) ? TRUE : FALSE;

	return Thread_IsValidThread(pArg) ? SUCCESS : ERROR_THREAD_CREATION_FAILED;
}

BOOL Thread_IsValidThread(void *pArg)
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return FALSE;

	return pThread->m_bIsAlive;
}

BOOL Thread_IsAlive(void *pArg)
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return FALSE;

	if(!Thread_IsValidThread(pArg))
		return FALSE;

	return pThread->m_bIsAlive;	
}

int Thread_Stop(void *pArg, PTHREADSTOPFUNC pStopFunc, void *pvStopParam/*=NULL*/)
{
	int i=0;
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return ERROR_THREAD_NOT_STOPPED;

	if(!pStopFunc)
		return ERROR_INVALID_STOP_FUNC;

	if(!pThread->m_bIsAlive)	
		return SUCCESS;

	pThread->m_pThreadStopFunc = pStopFunc;

	for(i=0; i<MAX_KILL_RETRY; i++)
	{
		if(pThread->m_pThreadStopFunc)
			pThread->m_pThreadStopFunc(pvStopParam);

		
		usleep(2* 1000);
#ifndef BCM_PTHREAD_CANCEL_NOT_SUPPORTED
		if(pthread_kill(pThread->m_Thread,0)==ESRCH)
	    	break;
 		else
        	Thread_Terminate(pThread);
#endif
	}
	
	if(i>=MAX_KILL_RETRY)
	{
		pThread->m_bIsAlive = TRUE;
		return ERROR_THREAD_NOT_STOPPED;
	}
	
	pThread->m_bIsAlive = FALSE;
	return SUCCESS;
}

int Thread_Terminate(void *pArg)
{
    int iRet = 0;
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return ERROR_THREAD_NOT_STOPPED;

	if(!pThread->m_bIsAlive)
		return ERROR_THREAD_NOT_RUNNING;
#ifndef BCM_PTHREAD_CANCEL_NOT_SUPPORTED
    iRet=pthread_kill(pThread->m_Thread,0);
    if(iRet==ESRCH)
    {
		return SUCCESS;
    }
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
    iRet = pthread_cancel(pThread->m_Thread);
	if(iRet == ESRCH || iRet == 0)
	{
		/* ESRCH - No thread exists with that id, means the thread already exited,
		 0 - Thread successfully stopped*/
		return SUCCESS;
	}
	/* Thread still running ???, retry killing*/
	return ERROR_THREAD_NOT_STOPPED;
#else
	return SUCCESS;

#endif

#else
	return SUCCESS;

#endif

}


void thread_cleanup(void *pArg)
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return;

	sem_post(&pThread->m_WaitSem);    /* Event semaphore Handle*/
	pThread->m_bIsAlive = FALSE;
}


/* Wait not working !!! - to be fixed -Prash */
int Thread_Wait(void *pArg, UINT32  dwWaitMilliSec)
{
	PTHREAD pThread = (PTHREAD)pArg;
	if(!pThread)
		return -1;

	if(dwWaitMilliSec == 0)
		return ERROR_INVALID_WAIT_TIME;

	if(dwWaitMilliSec == INFINITE)
	{
		pthread_cleanup_push(thread_cleanup, NULL);
		while(1)
		{
			if(sem_trywait(&pThread->m_WaitSem) == 0)
			{
				break;
			}
		}
		pthread_cleanup_pop(0);
	}
	else
	{
		UINT32 ulWaitMicroSec = dwWaitMilliSec*1000;
		UINT32 ulTotMicroSecs = 0;
		
		pthread_cleanup_push(thread_cleanup, NULL);
		while(1)
		{
			if(sem_trywait(&pThread->m_WaitSem) == 0)
			{
				break;
			}

			if(ulWaitMicroSec < ulTotMicroSecs)
				break;
			usleep(100); /* 100 micro seconds*/
			ulTotMicroSecs += 100;
		}
		pthread_cleanup_pop(0);
	

/*		struct timeval now;
   		struct timespec timeout;
   		gettimeofday(&now, NULL);

   		UINT32 ulRemNanoSec = (dwWaitMilliSec%1000)*(1000000);  Converting milli secs to nano secs, multiplying by 10^6
   		timeout.tv_sec = now.tv_sec + (dwWaitMilliSec/1000);
   		timeout.tv_nsec = now.tv_usec * 1000+ulRemNanoSec;
		sem_timedwait(&m_WaitSem, &timeout);
	*/
	}
	return SUCCESS;
}

int Thread_SetDebugName(const char* lpszDebugThreadName) 
{
	return SUCCESS;
}
