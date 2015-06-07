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
 * Description	:	Synchronization primitives for Linux.
 * Author		:
 * Reviewer		:
 *
 */
#include "BeceemWiMAX.h"
#include "SyncObjects.h"

#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
#include <sys/sem.h>
#endif



/* MUTEX for syncronizing shared data across threads */
extern UINT32 g_bCleanupSem;
void Mutex_Init(void *pArg)
{
	PMUTEX pMutex = (PMUTEX)pArg;
	if(!pMutex)
		return;
    Event_Init(pMutex);
	
#ifdef USING_SYS_V_SEM_IPC	
    SyS_V_Sem_Init(&pMutex->stSem);
#endif
    
}

void Mutex_Cleanup(void *pArg)
{
	PMUTEX pMutex = (PMUTEX)pArg;
	if(!pMutex)
		return;
    if(pMutex->cSemName)
        {
#ifdef USING_SYS_V_SEM_IPC        
			SyS_V_Sem_Cleanup(&pMutex->stSem);
#else
        	Event_Cleanup(pMutex);
#endif
        }
    else
        {
            pthread_mutex_destroy (&pMutex->mutexobj);
        }
}

int Mutex_Create(void *pArg, const char *szName)
{
    PMUTEX pMutex = (PMUTEX)pArg;
    if(!pMutex)
        return ERROR_INVALID_ARGUMENT;

    if(szName)
    {
#ifdef USING_SYS_V_SEM_IPC    
		INT iRet = 0;
		iRet =  Create_SyS_V_Sem(&pMutex->stSem,get_id_for_mutexName(szName), 1);
		if(!iRet)
			strncpy((char *) pMutex->cSemName, szName, MAX_SEMNAME_SIZE);
	
		return iRet;
#else
        return Event_Open(pArg,szName);
#endif
    }
    else
    {
        memset(pMutex->cSemName,0,MAX_SEMNAME_SIZE);
        pthread_mutex_init(&pMutex->mutexobj, NULL);
    	return SUCCESS;
    }

   	return ERROR_INVALID_ARGUMENT;
}

void Mutex_Lock(void *pArg)
{
	PMUTEX pMutex = (PMUTEX)pArg;
	if(!pMutex)
		return;
    if(strlen(pMutex->cSemName))
    {
#ifdef USING_SYS_V_SEM_IPC    
        SyS_V_Sem_Lock(&pMutex->stSem);
#else
        if(Event_Wait(pArg,INFINITE)  != SUCCESS)
        {
             DPRINT_OTHER("Event_Wait FAILED\n");
        }
#endif
    }       
   else
    {
	    pthread_mutex_lock (&pMutex->mutexobj);
    }
    
}

void Mutex_Unlock(void *pArg)
{
	PMUTEX pMutex = (PMUTEX)pArg;
	if(!pMutex)
		return;
    if(strlen(pMutex->cSemName))
    {
#ifdef USING_SYS_V_SEM_IPC    
        SyS_V_Sem_UnLock(&pMutex->stSem);
#else
        if(Event_Set(pArg) != SUCCESS)
    	{
    	    DPRINT_OTHER("Unlock: Event_Set failed...\n");
    	}
#endif
    }
    else
    {
        pthread_mutex_unlock (&pMutex->mutexobj);
    }

}


/* Event for asynv event handling */

void Event_Init(void *pArg)
{
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return;

	pEvent->m_bValidSemaphore = FALSE;
}

void Event_Cleanup(void *pArg)
{
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return;
	
#ifdef MACOS
	{
		char cSemNameWithPID[MAX_SEMNAME_SIZE] = {0};

		sprintf(cSemNameWithPID,"%s%d",pEvent->cSemName,getpid());
		sem_unlink(cSemNameWithPID);
		sem_close(pEvent->m_EventSem);
	}
#else
	sem_destroy(&(pEvent->m_EventSem));
#endif

	pEvent->m_bValidSemaphore = FALSE;
}

UINT32 GetCurrentMicroSecs()
{
	struct timeval now;
    UINT32 ulMicroSecs = 0;
	gettimeofday(&now, NULL);
    ulMicroSecs = now.tv_sec * 1000000;
	ulMicroSecs += now.tv_usec;

	return ulMicroSecs;
}

int Event_Create(
			void *pArg,
			BOOL bManualReset,  /* ignored on linux */
			BOOL bInitialState,
			const char *szName /* = NULL*/ /* ignored on linux */
		)
{
	int iInitalValue = 0;
	int iRet = 0;
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return ERROR_INVALID_ARGUMENT;

	if(bInitialState)
		iInitalValue = 1;

#ifndef MACOS
	iRet = sem_init(&(pEvent->m_EventSem), 0, iInitalValue);
	pEvent->m_bValidSemaphore = (iRet != -1) ? TRUE : FALSE;
#else
	{
		char cSemNameWithPID[MAX_SEMNAME_SIZE] = {0};
		
		memcpy(pEvent->cSemName,szName,strlen(szName)+1);
		sprintf(cSemNameWithPID,"%s%d",szName,getpid());
		
		pEvent->m_EventSem = sem_open(cSemNameWithPID, O_CREAT, 0777, 0);
		pEvent->m_bValidSemaphore = (pEvent->m_EventSem) ? TRUE : FALSE;
	}
#endif

	return iRet;
}

int Event_Open(
			void *pArg,
			const char *szName
		)
{
	sem_t *posix_sem;
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return ERROR_INVALID_ARGUMENT;

	pEvent->m_bValidSemaphore = TRUE;
    strncpy((char *) pEvent->cSemName, szName, MAX_SEMNAME_SIZE);
	QP(" # %s\n",pEvent->cSemName);
	
	if ((posix_sem = sem_open(szName, O_CREAT, 0664, 1)) == SEM_FAILED) {
		DP;
		perror ("sem_open fail");
		pEvent->m_bValidSemaphore = FALSE;
	}
	else {
        #ifdef MACOS
		pEvent->m_EventSem =  posix_sem;
        #else
        memcpy (&pEvent->m_EventSem, posix_sem, sizeof (sem_t));
        #endif
	}

	return pEvent->m_bValidSemaphore;
}
int Event_Set(void *pArg)
{
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return ERROR_INVALID_ARGUMENT;

	if(pEvent->m_bValidSemaphore)
#ifdef MACOS
		sem_post(pEvent->m_EventSem);	
#else
		sem_post(&(pEvent->m_EventSem));
#endif
	else
		return -1;
	
	return SUCCESS;
}

int Event_Reset(void *pArg)
{
	PEVENT pEvent = (PEVENT)pArg;
	INT iRet = 0;
	
	if(!pEvent)
		return ERROR_INVALID_ARGUMENT;

	if(!pEvent->m_bValidSemaphore)
		return ERROR_EVENT_NOT_INITIALIZED;
		
	/* destroy existing and initialize again with value 0 == not signaled */
    /*sem_destroy(&m_EventSem); */
	pEvent->m_bValidSemaphore = FALSE;
	Event_Cleanup(pEvent);

	iRet = Event_Create(pEvent, FALSE,FALSE,
#ifndef MACOS
	NULL
#else	
	pEvent->cSemName
#endif	
	);

	if(iRet == SUCCESS)
		pEvent->m_bValidSemaphore = TRUE;
		
	return SUCCESS; /* Is this needed for Linux, since the counter is automatically decremented by sem_trywait() ???*/
}

int Event_Wait(void *pArg, UINT32 ulWaitTimeOut/*= INFINITE*/)
{
	PEVENT pEvent = (PEVENT)pArg;
	if(!pEvent)
		return ERROR_INVALID_ARGUMENT;

	if(!pEvent->m_bValidSemaphore)
		return ERROR_EVENT_NOT_INITIALIZED;
		
	if(ulWaitTimeOut == 0)
		return ERROR_INVALID_WAIT_TIME;

	if(ulWaitTimeOut == INFINITE)
	{
#ifdef MACOS
		sem_wait(pEvent->m_EventSem);
#else
		sem_wait(&(pEvent->m_EventSem));
#endif		/*
		while(true)
		{
			if(sem_trywait(&m_EventSem) == 0)
				break;
		}
		*/
	}
	else
	{
		UINT32 ulWaitMicroSec = ulWaitTimeOut*1000;
		UINT32 ulStart = GetCurrentMicroSecs();
		while(1)
		{
		        UINT32 ulEnd =0;
#ifdef MACOS
			if(sem_trywait(pEvent->m_EventSem) == 0)
#else
			if(sem_trywait(&(pEvent->m_EventSem)) == 0)
#endif
				break;

			ulEnd = GetCurrentMicroSecs();
			if((ulEnd - ulStart) >= ulWaitMicroSec)
				return WAIT_TIMEOUT;
		
			usleep(10); /* 100 micro seconds */
		}
	}
	return SUCCESS;
}

#ifdef USING_SYS_V_SEM_IPC

/* Adding system V IPC semaphores since sem_open is not supported in 5350 */

int Create_SyS_V_Sem(void *pArg, const int iKeyValue,int iNumSems)
{
	int iRet = SUCCESS,i=0;
    P_STMUTEX_SYS_V pMutex = (P_STMUTEX_SYS_V)pArg;

    union semun arg = {0};
    struct semid_ds buf;
    struct sembuf sb = {0};
    const int MAX_RETRIES = 10;
    iNumSems = 1;
    
	memset(&buf,0,sizeof(struct semid_ds));

	if(!pMutex)
    {
    	iRet = ERROR_INVALID_ARGUMENT;
		goto ret_path;
    }	
    
	if ((pMutex->key = ftok(LIB_PATH, iKeyValue)) == -1) 
    {
		perror("ftok");
        pMutex->semid = -1;
        iRet = ERROR_SEMAPHORE_INIT_FAILED;
        goto ret_path;
        
    }
    
  	pMutex->semid = semget(pMutex->key, iNumSems, IPC_CREAT | IPC_EXCL | 0666);
    if (pMutex->semid >= 0) /* we got it first */
    {
        sb.sem_op = 1; sb.sem_flg = 0;
	    arg.val = 1;
    	for(sb.sem_num = 0; sb.sem_num < iNumSems; sb.sem_num++) 
            { 
    		/* do a semop() to "free" the semaphores. */
    		/* this sets the sem_otime field, as needed below. */
	    		if (semop(pMutex->semid , &sb, 1) == -1) 
                {
                    int e = errno;
                    semctl(pMutex->semid, 0, IPC_RMID); /* clean up */
                    errno = e;
                    pMutex->semid  = -1; /* error, check errno */
                }
            }
    }
    else if (errno == EEXIST) 
    { 
        /* someone else got it first */
    		int ready = 0;

            pMutex->semid = semget(pMutex->key, iNumSems, 0); /* get the id */

        if (pMutex->semid < 0) 
            {
             iRet = ERROR_SEMAPHORE_INIT_FAILED;
            goto ret_path;
            }

        /* wait for other process to initialize the semaphore: */
         arg.buf = &buf;
        for(i = 0; i < MAX_RETRIES && !ready; i++) 
        {
               semctl(pMutex->semid, 0, IPC_STAT, arg);
             if (arg.buf->sem_otime != 0) 
            {
                ready = 1;
            } 
            else 
            {
            	sleep(1);
            }
        }
        if (!ready) 
        {
        	errno = ETIME;
        	pMutex->semid = -1;
        }

    }
    
ret_path:
    return iRet;
}

void SyS_V_Sem_Lock(void *pArg)
{
        P_STMUTEX_SYS_V pMutex = (P_STMUTEX_SYS_V)pArg;

        struct sembuf sb = {0};
		sb.sem_num = 0;
    	sb.sem_op = -1;  /* set to allocate resource */
    	sb.sem_flg = SEM_UNDO;
        if(pMutex->semid == -1)
            return;

        if (semop(pMutex->semid , &sb, 1) == -1) 
        {
            perror("semop");
        }
}
void SyS_V_Sem_UnLock(void *pArg)
{
        P_STMUTEX_SYS_V pMutex = (P_STMUTEX_SYS_V)pArg;
        struct sembuf sb = {0};
		sb.sem_num = 0;
    	sb.sem_op = 1;  /* set to allocate resource */
    	sb.sem_flg = SEM_UNDO;
        if(pMutex->semid == -1)
            return;

        if (semop(pMutex->semid , &sb, 1) == -1) 
        {
            perror("semop");
        }
        
}
void SyS_V_Sem_Init(void *pArg)
{
    P_STMUTEX_SYS_V pMutex = (P_STMUTEX_SYS_V)pArg;

    pMutex->semid = -1;

}
void SyS_V_Sem_Cleanup(void *pArg)
{
    P_STMUTEX_SYS_V pMutex = (P_STMUTEX_SYS_V)pArg;
    if(pMutex->semid == -1)
        return;
    
    if(g_bCleanupSem)
        {
            semctl(pMutex->semid, 0, IPC_RMID);
        }

    pMutex->semid = -1;
    pMutex->key = 0;
}
#endif
