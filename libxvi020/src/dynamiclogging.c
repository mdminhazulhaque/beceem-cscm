
#include "BeceemWiMAX.h"
#include "globalobjects.h"
/**********************************************************************
 * DYNAMIC LOGGING Subsystem
 *********************************************************************/

INLINE_LINKAGE PST_SHM_HDR GetShmHdr(PBECEEMWIMAX pstBeceemWiMAX)
{
//      PST_SHM_HDR __pstHdr = NULL;
    if (!pstBeceemWiMAX)
	return NULL;

	if(!pstBeceemWiMAX->stLogging.bInited)
	return NULL;	
	
    return (PST_SHM_HDR) & pstBeceemWiMAX->stLogging.pstShmLog->stShmHdr;
}

/* Get the pointer to the shmem log record (for dynamic logging facility) for this process */
INLINE_LINKAGE PST_SHM_LOG_REC GetThisProcessShmLogRec(PBECEEMWIMAX pstBeceemWiMAX)
{
    PST_SHM_LOG_REC __pstRec = NULL;
    int index;
    intptr_t uiSharedMemLogBasePtr = (intptr_t) pstBeceemWiMAX->stLogging.pstShmLog;

    if (!pstBeceemWiMAX)
	return NULL;
	else if(!pstBeceemWiMAX->stLogging.bInited)
	return NULL;	
    else
	{
	    if (!pstBeceemWiMAX->stLogging.pstShmLog)
		return NULL;
	    index = pstBeceemWiMAX->stLogging.nIndexPos;
	    __pstRec =
		(PST_SHM_LOG_REC) (uiSharedMemLogBasePtr + sizeof(ST_SHM_LOG) +
				   (index * sizeof(ST_SHM_LOG_REC)));
	    return (PST_SHM_LOG_REC) __pstRec;
	}
}

/* Get the pointer to the shmem log record (for dynamic logging facility) for the given index (slot) */
INLINE_LINKAGE PST_SHM_LOG_REC GetShmLogRecByIndex(PBECEEMWIMAX pstBeceemWiMAX, UINT index)
{
    PST_SHM_LOG_REC __pstRec = NULL;

    if (!pstBeceemWiMAX)
	return NULL;
	if(!pstBeceemWiMAX->stLogging.bInited)
	return NULL;
    if ((index < 0) || (index >= DYN_LOGGING_MAX_PROCESSES))
	return NULL;
    if (!pstBeceemWiMAX->stLogging.pstShmLog)
	return NULL;
    __pstRec =
	(PST_SHM_LOG_REC) ((intptr_t) pstBeceemWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG) +
			   (index * sizeof(ST_SHM_LOG_REC)));
    return (PST_SHM_LOG_REC) __pstRec;
}
INLINE_LINKAGE INT DynLogging_FindProcessByPID(PBECEEMWIMAX pWiMAX, PID pid)
{
    PST_SHM_LOG_REC recs_start;
    int i;

    if (!pWiMAX)
	return -1;
	
	if(!pWiMAX->stLogging.bInited)
	return -1;

    recs_start = (PST_SHM_LOG_REC) ((intptr_t) pWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));
    for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
	{
	    if (pid == recs_start->nPid)
		return i;
	    recs_start++;
	}
    return -1;
}

#define	SHOW_ONLY_LIVE_PROCESSES	TRUE

#define	SHOW_SHM_LOG_RECS(pstBeceemWiMAX, bShowOnlyLivePrcs)	\
do {	\
	PST_SHM_LOG_REC recs_start;	\
	int i;	\
	if(STD_OUTPUT)\
				fprintf (STD_OUTPUT,"Index:   PID  :    Dbg Flags      Dbg Paths    Log Where\n");	\
	if (pstBeceemWiMAX->stLogging.bInited) {	\
		recs_start = (PST_SHM_LOG_REC)((intptr_t)pstBeceemWiMAX->stLogging.pstShmLog + sizeof (ST_SHM_LOG)); \
		for (i=0; i<DYN_LOGGING_MAX_PROCESSES; i++) {	\
			if (bShowOnlyLivePrcs && !recs_start->nPid) {		\
				recs_start ++;	\
				continue;	\
			}	\
			if(STD_OUTPUT)\
				fprintf (STD_OUTPUT," %02d:  %7d :       0x%02x       0x%08x       0x%02x\n", i, 	\
				recs_start->nPid, recs_start->uDebugFlag, recs_start->uDebugPath, recs_start->uLogWhere);	\
			recs_start ++;	\
		}	\
	}	\
} while (0)

INLINE_LINKAGE BOOL DYN_LOGGING_LOCK_SHMEM(PMUTEX pSem)
{
    if (!pSem)
    	return FALSE;
    Mutex_Lock(pSem);
     return TRUE;
}
INLINE_LINKAGE BOOL DYN_LOGGING_UNLOCK_SHMEM(PMUTEX pSem)
{
    if (!pSem)
    	return FALSE;
    Mutex_Unlock(pSem);
    return TRUE;
}

BOOL DynLogging_Logfile_CheckSize(PCHAR log_filename)
{
    
#ifndef WIN32
    struct stat st_stat;

    if (stat((PVOID) log_filename, &st_stat) < 0) 
	{
	    DPRINT_OTHER("errno=%d\n", errno);
	    return FALSE;
	}

    if (st_stat.st_size > DYN_LOGGING_LOGFILE_MAXSIZE)
	{
	    DPRINT_OTHER("WARNING! Logfile %s exceeds maximun allowable size, will now be truncated!\n",
			 log_filename);

	    //
	    // TODO ! Make a backup first!
	    //

	    if (truncate((PVOID) log_filename, 0) < 0)
		{
		    DPRINT_OTHER("WARNING! truncate(2) failed...errno=%d\n", errno);
		    unlink((PVOID) log_filename);
		    return FALSE;
		}
	}
	return TRUE;
#else
    DWORD dwFileSizeLo = 0,dwFileSizeHigh = 0; 
    BOOL bRetVal = FALSE;
    HANDLE hFile =   CreateFile(
					log_filename,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
				);
    if((INVALID_HANDLE_VALUE == hFile)&& (GetLastError() == ERROR_FILE_NOT_FOUND ))
        {
            bRetVal = TRUE;
            goto ret_path;
        }
    else
        {
            dwFileSizeLo = GetFileSize(hFile,&dwFileSizeHigh);
            if(INVALID_FILE_SIZE == dwFileSizeLo)
                {
                     DPRINT_OTHER("WARNING! GetFileSizefailed...errno=%d\n", GetLastError());
                     bRetVal = TRUE;
                     goto ret_path;

                }
            else if( DYN_LOGGING_LOGFILE_MAXSIZE < dwFileSizeLo)
                {
                     DPRINT_OTHER("WARNING! Logfile %s exceeds maximun allowable size, will now be truncated!\n",
			 log_filename);
                     CloseHandle(hFile);
                     if(!DeleteFile(log_filename))
                        {
                            DPRINT_OTHER("WARNING! Logfile %s could not be deleted error No %d",
                                log_filename,GetLastError() );
                        }
                     hFile = INVALID_HANDLE_VALUE;
                    bRetVal = TRUE;

                }
        }
ret_path:
    if(INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    return TRUE;

#endif

}

/* Loop over all records looking for invalid records;
   and get rid of 'em!
 */
#ifndef WIN32
VOID DynLogging_Shmem_FixInvalidSlots(VOID)
{
    PST_SHM_LOG_REC recs_start;
    int i;

	if(!GpWiMAX->stLogging.bInited)
	return;

    recs_start = (PST_SHM_LOG_REC) ((intptr_t) GpWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));
    for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
	{
	    if ((kill(recs_start->nPid, 0) < 0) && (errno == ESRCH))

		{		// found an invalid process in this slot!
		    QP("INVALID SLOT! #%d PID %d\n", i, recs_start->nPid);
		    recs_start->nPid = 0;
		}
	    recs_start++;
	}
}
#else
VOID DynLogging_Shmem_FixInvalidSlots(VOID)
{
    PST_SHM_LOG_REC recs_start;
    int i;
	PVOID hProcHandle = NULL;
	DWORD dwExitStatus = 0;

	if(!GpWiMAX->stLogging.bInited)
	return;
	
	

    recs_start = (PST_SHM_LOG_REC) ((intptr_t) GpWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));
    for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
	{
	    hProcHandle = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,recs_start->nPid);
		if(hProcHandle)
		{
			if(GetExitCodeProcess(hProcHandle,&dwExitStatus))
			{
				if(dwExitStatus != STILL_ACTIVE)
				{
					/* Found Invalid Process In Slot */
					QP("INVALID SLOT! #%d PID %d\n", i, recs_start->nPid);

					recs_start->nPid = 0;
				}
			}
		}
		else
		{
			if(GetLastError() == ERROR_INVALID_PARAMETER)
			{
				/* OpenProcess failed due to Invalid PID */
				recs_start->nPid = 0;
			}
		}
		
	    recs_start++;
	}
}
#endif
/* This function is called on init when a process instance begins using the library.  */
BOOL DynLogging_InitShmRecord(void)
{
    INT shmSize = sizeof(ST_SHM_LOG) + (sizeof(ST_SHM_LOG_REC) * DYN_LOGGING_MAX_PROCESSES);
    BOOLEAN first_time = FALSE;
    PST_SHM_LOG pstShmLog = NULL;
    PST_SHM_HDR pstShmHdr = NULL;
    PMUTEX pSem = NULL;
    BOOL bRetVal = TRUE;
	GpWiMAX->stLogging.bInited = TRUE;
    if (SharedMem_Open(&GpWiMAX->m_shmDynLogging, shmSize, DYN_LOGGING_SHM_KEY) == ERROR_SHARED_MEM_OPEN_FAILED)
	{			// very first process!
	    DPRINT_OTHER("SharedMem_Open failed, attempting to Create...(first process)\n");
	    first_time = TRUE;
	    // Initialize the shmem record
	    if (SharedMem_Create(&GpWiMAX->m_shmDynLogging, shmSize, DYN_LOGGING_SHM_KEY) < 0)
		{
		    DPRINT_OTHER("SharedMem_Create failed\n");
		    bRetVal = FALSE;
			GpWiMAX->stLogging.bInited = FALSE;
            goto ret_path;
		}

	    pstShmLog = (PST_SHM_LOG) GpWiMAX->m_shmDynLogging.pMappedAddress;
        memset(pstShmLog, 0, shmSize);
#ifndef WIN32
	     pstShmLog->stShmHdr.uNumProcessesAlive = SharedMem_GetAttachCount(&GpWiMAX->m_shmDynLogging);
#else
		/* First process to use shared memory */
         pstShmLog->stShmHdr.uNumProcessesAlive = 1;
#endif
	    if (pstShmLog->stShmHdr.uNumProcessesAlive < 0)
		pstShmLog->stShmHdr.uNumProcessesAlive = 0;
	    QP("attach count=num prcs = %d\n", pstShmLog->stShmHdr.uNumProcessesAlive);

	    pstShmLog->pstShmLogRec = (PST_SHM_LOG_REC) ((intptr_t) pstShmLog + sizeof(ST_SHM_LOG));

//              QP ("sizeof(ST_SHM_LOG) = %d, sizeof(MUTEX) = %d, shmSize = 0x%x (%d bytes), pstShmLogRec = 0x%x\n",
//                      sizeof(ST_SHM_LOG), sizeof(MUTEX), shmSize, shmSize, pstShmLog->pstShmLogRec);
	}

    // Named Semaphore creation
    GpWiMAX->stLogging.ptSemShmLock = (PMUTEX) calloc(1, sizeof(MUTEX));
    if (!GpWiMAX->stLogging.ptSemShmLock)
	{
	    DPRINT_OTHER("pEvent: out of memory\n");
		GpWiMAX->stLogging.bInited = FALSE;
        bRetVal = FALSE;
        goto ret_path;
	}
    if (!Mutex_Create(GpWiMAX->stLogging.ptSemShmLock, DYN_LOGGING_MUTEX) < 0)
	{
	    DPRINT_OTHER("EventOpen failed\n");
		GpWiMAX->stLogging.bInited = FALSE;
	    bRetVal = FALSE;
        goto ret_path;
	}
    pSem = GpWiMAX->stLogging.ptSemShmLock;
							
		if (!DYN_LOGGING_LOCK_SHMEM(pSem))
        {
            bRetVal = FALSE;
			GpWiMAX->stLogging.bInited = FALSE;
            goto ret_path;
	    }

    pstShmLog = (PST_SHM_LOG) GpWiMAX->m_shmDynLogging.pMappedAddress;
    GpWiMAX->stLogging.pstShmLog = (PST_SHM_LOG) GpWiMAX->m_shmDynLogging.pMappedAddress;
    //      QP("GpWiMAX->stLogging.pstShmLog = %x\n", GpWiMAX->stLogging.pstShmLog);
// From here on, use the GpWiMAX pointer to reference the shm log structures..

    pstShmHdr = (PST_SHM_HDR) GetShmHdr(GpWiMAX);
    if (!pstShmHdr)
	{
	    DPRINT_OTHER("%s: Failed retrieving shmem header\n", __FUNCTION__);
		GpWiMAX->stLogging.bInited = FALSE;
	    bRetVal = FALSE;
        goto ret_path;
	}
#ifndef WIN32
    pstShmHdr->iShmId = GpWiMAX->m_shmDynLogging.iShmId;
#endif

    /* ---Fix index position in array of log records in shmem */
    if (first_time)
	{
	    GpWiMAX->stLogging.nIndexPos = 0;
	    first_time = FALSE;
	}
    else
	{
#ifndef WIN32
        DynLogging_Shmem_FixInvalidSlots();
#endif
#ifndef WIN32
	    pstShmLog->stShmHdr.uNumProcessesAlive = SharedMem_GetAttachCount(&GpWiMAX->m_shmDynLogging);
#else
        pstShmLog->stShmHdr.uNumProcessesAlive = DynLogging_GetCountValidProcessShmLogRec()+1;
#endif
	    if (pstShmLog->stShmHdr.uNumProcessesAlive < 0)
    		pstShmLog->stShmHdr.uNumProcessesAlive = 0;
	    QP("attach count=num prcs = %d\n", pstShmLog->stShmHdr.uNumProcessesAlive);
	    if (pstShmHdr->uNumProcessesAlive > DYN_LOGGING_MAX_PROCESSES)
		{
		    DPRINT_OTHER("%s: MAX number of processes exceeded (%d)\n",
				 __FUNCTION__, pstShmLog->stShmHdr.uNumProcessesAlive);
		    DYN_LOGGING_UNLOCK_SHMEM(pSem);
		    bRetVal = FALSE;
			GpWiMAX->stLogging.bInited = FALSE;
            goto ret_path;
		}

	    // Loop over all records looking for a free slot
	    {
		PST_SHM_LOG_REC recs_start;
		int i;

		recs_start =
		    (PST_SHM_LOG_REC) ((intptr_t) GpWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));
		for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
		    {
			if (!recs_start->nPid)
			    {	// found a free slot!
				GpWiMAX->stLogging.nIndexPos = i;
				break;
			    }
			recs_start++;
		    }		// > max processes condition caught above, so no need to chk here again..
	    }
	}			// else

    QP("%s: PID %d. # processes alive = %d. Index pos=%d\n",
       __FUNCTION__, GetCurrentProcID(), pstShmHdr->uNumProcessesAlive, GpWiMAX->stLogging.nIndexPos);

    /* Access the relevant shmem log record & init it */
    {
	PST_SHM_LOG_REC pstRec;

	pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);
	if (!pstRec)
	    {
		DPRINT_OTHER("%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			     __FUNCTION__, GetCurrentProcID());
		DYN_LOGGING_UNLOCK_SHMEM(pSem);
		bRetVal = FALSE;
		GpWiMAX->stLogging.bInited = FALSE;
        goto ret_path;
	    }
	/*
	   QP ("pstShmLog = 0x%x. index slot=%d. pstRec = 0x%x [diff %d]\n",
	   (void *)GpWiMAX->stLogging.pstShmLog, GpWiMAX->stLogging.nIndexPos, pstRec, 
	   ((void *)pstRec-(void *)GpWiMAX->stLogging.pstShmLog));
	 */
	pstRec->nPid = GetCurrentProcID();
	pstRec->uDebugFlag = DEBUG_OFF;	//DEBUG_OFF;
	pstRec->uDebugPath = DEBUG_PATH_ALL;
	pstRec->uLogWhere = LOG_ALL;

	/*--- File logging, syslog setup.
	 *
	 * Design: we'll use a SINGLE logfile for all process instances 
	 * This enables us to easily truncate the file when it exceeds a limit. 
	 * This means all processes will append their logs to the SAME file; 
	 * in order for the user to differentiate which process logged what, we 
	 * also log the PID (2nd field, ':' delimited).
	 */
	DynLogging_Logfile_CheckSize((PCHAR) DYN_LOGGING_LOGFILENAME);

	if (pstRec->uDebugFlag)
	    {
		QP("LOGGING ON!\n");

		SetFile(GTraceDebug, (char *) DYN_LOGGING_LOGFILENAME);

		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
		       "************** PROCESS PID %d DYNAMIC LOGGING START **************\n", GETPID);
	    }
    }

    SHOW_SHM_LOG_RECS(GpWiMAX, !SHOW_ONLY_LIVE_PROCESSES);

    if (!DYN_LOGGING_UNLOCK_SHMEM(pSem))
        {
            bRetVal = FALSE;
			GpWiMAX->stLogging.bInited = FALSE;
            goto ret_path;
        }

	GpWiMAX->stLogging.bInited = TRUE;
	
ret_path:  
    QP("returning %d",bRetVal);
    return bRetVal;    
    
}

/*-----------------Shared Memory record functionality for dynamic logging---
 * This function is called on cleanup.
 */
BOOL DynLogging_CleanShmRecord(void)
{
    PST_SHM_LOG_REC pstRec = NULL;
    PST_SHM_LOG pstShmLog = NULL;
    PST_SHM_HDR pstShmHdr = NULL;
    PMUTEX pSem = NULL;
	BOOL bRetVal = TRUE;
    pSem = GpWiMAX->stLogging.ptSemShmLock;
    if(!GpWiMAX->stLogging.bInited)
		return bRetVal;
	DYN_LOGGING_LOCK_SHMEM(pSem);
    #ifndef WIN32
    DynLogging_Shmem_FixInvalidSlots();
    #endif

    pstShmHdr = (PST_SHM_HDR) GetShmHdr(GpWiMAX);
    if (!pstShmHdr)
	{
	    DPRINT_OTHER("%s: Failed retrieving shmem header. Aborting...\n", __FUNCTION__);
	    bRetVal= FALSE;
		goto ret_path;
	}

    pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);
    if (!pstRec)
	{
	    DPRINT_OTHER("%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			 __FUNCTION__, GetCurrentProcID());
	    DYN_LOGGING_UNLOCK_SHMEM(pSem);
	    bRetVal= FALSE;
		goto ret_path;
	}
    pstShmLog = (PST_SHM_LOG) GpWiMAX->stLogging.pstShmLog;

    pstRec->nPid = 0;

    
	#ifndef WIN32
	 pstShmLog->stShmHdr.uNumProcessesAlive = SharedMem_GetAttachCount(&GpWiMAX->m_shmDynLogging);
    #else
     pstShmLog->stShmHdr.uNumProcessesAlive = DynLogging_GetCountValidProcessShmLogRec();
	#endif
    
    if ((int)pstShmLog->stShmHdr.uNumProcessesAlive < 0)
	pstShmLog->stShmHdr.uNumProcessesAlive = 0;
    QP("attach count=num prcs = %d\n", pstShmLog->stShmHdr.uNumProcessesAlive);

    SHOW_SHM_LOG_RECS(GpWiMAX, !SHOW_ONLY_LIVE_PROCESSES);
	DP;
    SharedMem_Close(&GpWiMAX->m_shmDynLogging);
	GpWiMAX->stLogging.pstShmLog = NULL;
	DP;
ret_path:
	DP;	
    DYN_LOGGING_UNLOCK_SHMEM(pSem);
    DP;
	Mutex_Cleanup(pSem);
    DP;
	GpWiMAX->stLogging.bInited= FALSE;
    if(GpWiMAX->stLogging.ptSemShmLock)
        {
        free(GpWiMAX->stLogging.ptSemShmLock);
        GpWiMAX->stLogging.ptSemShmLock=NULL;
        }
    
	return bRetVal;
}


BOOL DebugPrint_Print(PVOID pArg, unsigned char iCode, INT iPath, const char *szFmt, ...)
{
    PCHAR pszMsg = NULL;
    UINT szMsglen = MAX_DPRINT_BUFFER_SIZE + MAX_TIMESTAMP_BUFFER_SIZE;
    CHAR sDtTime[MAX_TIMESTAMP_BUFFER_SIZE];
   #ifdef WIN32
    CHAR __tmstamp[MAX_TIMESTAMP_BUFFER_SIZE];
#endif    
    PST_SHM_LOG_REC pstRec = NULL;
    PDEBUGPRINT pDPrint = (PDEBUGPRINT) pArg;

    if (!pDPrint)
	return FALSE;

    pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);
    if (!pstRec)
	{
#ifdef ENABLE_FALLBACK_PRINTS
	    //DPRINT_OTHER ("%s: Failed retrieving shmem log record for PID %d.\n", __FUNCTION__, GetCurrentProcID());

	    // Since the "regular" debug print failed, fallback to DPRINT_OTHER ...
	    pszMsg = (PCHAR) calloc(1, szMsglen);
	    if (NULL == pszMsg)
		{
		    printf("\nDebugPrint_Print: Memory allocation failed \n");
		    return FALSE;
		}

        SECURE_PRINT_FN((PVOID) pszMsg, szMsglen, "<Fallback Print>: %s", szFmt);
        
	    DPRINT_OTHER("%s", pszMsg);
	    if (pszMsg)
		free(pszMsg);
#endif
	    return FALSE;
	}
//      QP ("pstShmLog = 0x%x. index slot=%d. pstRec = 0x%x [diff %d]\n",
//              (void *)GpWiMAX->stLogging.pstShmLog, index, pstRec, ((void *)pstRec-(void *)GpWiMAX->stLogging.pstShmLog));

    /* Do we log it? */
    if (!(pstRec->uDebugFlag) || !(pstRec->uLogWhere) || !(pstRec->uDebugPath))
	return FALSE;

//      QP("iPath = %d. pstRec->uDebugPath = %d\n", iPath, pstRec->uDebugPath );
    /* Note! 
     * 1. debug paths are mutually exclusive!
     * 2. maintain this order of evaluation (order in which the DEBUG_PATH_XXX 's are defined).
     */
    if (!(DEBUG_PATH_ALL == iPath))
	{
	    if ((DEBUG_PATH_RX == iPath) && !((pstRec->uDebugPath & DEBUG_PATH_RX) == DEBUG_PATH_RX))
		return FALSE;
	    else if ((DEBUG_PATH_TX == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_TX) == DEBUG_PATH_TX))
		return FALSE;
	    else if ((DEBUG_PATH_NVM == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_NVM) == DEBUG_PATH_NVM))
		return FALSE;
	    else if ((DEBUG_PATH_CAPI == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_CAPI) == DEBUG_PATH_CAPI))
		return FALSE;
	    else if ((DEBUG_PATH_RDM == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_RDM) == DEBUG_PATH_RDM))
		return FALSE;
	    else if ((DEBUG_PATH_WRM == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_WRM) == DEBUG_PATH_WRM))
		return FALSE;
	    else if ((DEBUG_PATH_OTHER == iPath) &&
		     !((pstRec->uDebugPath & DEBUG_PATH_OTHER) == DEBUG_PATH_OTHER))
		return FALSE;
	}

/*
 * fork() case  ??
 * If process forks, the child will inherit the parent's settings (shmem ref cnt will go up);
 * this default seems reasonable. If we don't want the child to log, we can look for the case
 * by checking for it's record, saved Pid == GetCurrentProcID() ? if false, it's a child..
 * [nothing implemented yet...]
 */

    pszMsg = (PCHAR) calloc(1, szMsglen);
    if (NULL == pszMsg)
	{
	    DPRINT_OTHER("\nDebugPrint_Print: Memory allocation failed \n");
	    return FALSE;
	}

    // Update: we now append the PID as well...
#ifndef WIN32
    snprintf((PVOID) sDtTime, sizeof(sDtTime), "[%s]:%d", (PCHAR) GET_TIMESTAMP(TM_FMT_COOKED), GETPID);
#else
    GET_TIMESTAMP((PUCHAR) __tmstamp);
    sprintf_s(sDtTime, sizeof(sDtTime), "[%s]:%d", __tmstamp, GETPID);
#endif

    switch (iCode)
	{
	  case DEBUG_MESSAGE:
	      SECURE_PRINT_FN((PVOID) pszMsg, szMsglen, "%s:WiMAX MESSAGE: %s", sDtTime, szFmt);
	      break;

	  case DEBUG_ERROR:
	      SECURE_PRINT_FN((PVOID) pszMsg, szMsglen, "%s:WiMAX *ERROR*: %s", sDtTime, szFmt);
	      break;

	  case DEBUG_WARNING:
	      SECURE_PRINT_FN((PVOID) pszMsg, szMsglen, "%s:WiMAX *WARNING*: %s", sDtTime, szFmt);
	      break;

	  default:
	      strncat((char *) pszMsg, szFmt, szMsglen);
	}

    /* Log Where... */
    
    if (pstRec->uLogWhere & LOG_FILE)
	{
	#ifndef WIN32
        if(pDPrint->m_iFile == INVALID_FILE_DESCRIPTOR)
    #else
        if(!pDPrint->pFptr)
    #endif
        {
            SetFile(GTraceDebug, (char*) DYN_LOGGING_LOGFILENAME);
        }
#ifndef WIN32
	    write(pDPrint->m_iFile, pszMsg, strlen((PVOID) pszMsg));
#else

		if(pDPrint->pFptr)
		{
	        fprintf(pDPrint->pFptr,pszMsg);
		}
#endif
	}

    if (pstRec->uLogWhere & LOG_STD_SCR)
	printf("%s", pszMsg);
    

    if ((pstRec->uLogWhere & BECEEM_LOG_SYSLOG) || (iCode == DEBUG_ERROR))
	{
	    DebugPrint_syslog(pszMsg);
	}
    if (pszMsg)
	{
	    free(pszMsg);
	}

    return TRUE;
}

/*
 * Get/Set API.
 * Call Graph: 
 *  BeceemWiMAXTestApp->DynamicLogging()->WrapperSendWiMAXMessage()->SendMessage.c:BeceemWiMAX_DynLoggingHandler()->BeceemWiMAX.c:DynLoggingAttr_[G|S]et()
 *
 */
BOOLEAN DynLoggingAttr_Set(PUCHAR payload)
{
    PST_DYNAMIC_LOGGING pstDynLogging = NULL;	// mapped to the payload (sent by (test) app)
    PST_SHM_LOG_REC pstRec = NULL;	// maps to the shmem record being processed
    INT i = 0, index = 0;
    PMUTEX pSem = NULL;

    pSem = GpWiMAX->stLogging.ptSemShmLock;
    if (!DYN_LOGGING_LOCK_SHMEM(pSem))
	return FALSE;

    DynLogging_Shmem_FixInvalidSlots();
   
    pstDynLogging = (PST_DYNAMIC_LOGGING) payload;

    switch (pstDynLogging->nLogFor)
	{
	  case PROCESS_SELF:
	      pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);
	      if (!pstRec)
		  {
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			     "%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			     __FUNCTION__, GetCurrentProcID());
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }

#if 1
	      // Let child process use parent's record. ???

	      if ((pstDynLogging->uiPid != GetCurrentProcID()) 
                #ifndef WIN32 
                && (pstDynLogging->uiPid != getppid())
                #endif
                )
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
#endif
	      // The 'settings payload' is in record #0 ...
	      pstRec->uDebugFlag = pstDynLogging->arrShmLogRecs[0].uDebugFlag;
	      pstRec->uDebugPath = pstDynLogging->arrShmLogRecs[0].uDebugPath;
	      pstRec->uLogWhere = pstDynLogging->arrShmLogRecs[0].uLogWhere;
	      break;

	  case PROCESS_BY_PID:
	      index = DynLogging_FindProcessByPID(GpWiMAX, pstDynLogging->uiPid);
	      if (-1 == index)
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
//                      QP("Found PID %d! slot=%d\n", pstDynLogging->uiPid, index);

	      pstRec = GetShmLogRecByIndex(GpWiMAX, index);
	      if (!pstRec)
		  {
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			     "%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			     __FUNCTION__, pstDynLogging->uiPid);
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
	      if (pstRec->nPid != pstDynLogging->uiPid)
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }

	      // The 'settings payload' is in record #0 ...
	      pstRec->uDebugFlag = pstDynLogging->arrShmLogRecs[0].uDebugFlag;
	      pstRec->uDebugPath = pstDynLogging->arrShmLogRecs[0].uDebugPath;
	      pstRec->uLogWhere = pstDynLogging->arrShmLogRecs[0].uLogWhere;
	      break;

	  case PROCESS_ALL:

	      if(!GpWiMAX->stLogging.bInited)
		      return FALSE;
		      	
	      pstRec = (PST_SHM_LOG_REC) ((intptr_t) GpWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));	// start of shmem recs

	      for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
		  {
		      if (pstRec->nPid)
			  {
			      // The 'settings payload' is in record #0 ...
			      pstRec->uDebugFlag = pstDynLogging->arrShmLogRecs[0].uDebugFlag;
			      pstRec->uDebugPath = pstDynLogging->arrShmLogRecs[0].uDebugPath;
			      pstRec->uLogWhere = pstDynLogging->arrShmLogRecs[0].uLogWhere;
			  }
		      pstRec++;
		  }
	      break;
	  default:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Invalid 'nLogFor' value!\n");
	      break;
	}			// switch

    //------------- Open / Close the logfile depending upon whether debug flag is On / Off.
    if (pstRec)
	{
	    //CHAR sLogfileName[256];
	    //snprintf ((PVOID)sLogfileName, sizeof (sLogfileName), "%d_XVI.log", GetCurrentProcID());
	    if (pstDynLogging->arrShmLogRecs[0].uDebugFlag)
		{
		    QP("LOGGING ON!\n");
		    if (SetFile(GTraceDebug, (char*) DYN_LOGGING_LOGFILENAME))
			QP("Logfile \"%s\" ready.\n", DYN_LOGGING_LOGFILENAME);
		}
	    else
		{
		    PDEBUGPRINT pDPrint = (PDEBUGPRINT) GTraceDebug;

		    if (pDPrint &&
#ifndef WIN32
			(pDPrint->m_iFile != INVALID_FILE_DESCRIPTOR)
#else
			(pDPrint->pFptr)
#endif
			)
			{
#ifndef WIN32
			    fflush(fdopen(pDPrint->m_iFile, "a+"));
			    close(pDPrint->m_iFile);
			    pDPrint->m_iFile = INVALID_FILE_DESCRIPTOR;
			    closelog();	// for syslog
#else
#endif
			    QP("Logfile \"%s\" & syslog closed for THIS process PID %d.\n",
			       DYN_LOGGING_LOGFILENAME, GETPID);
			}
		    QP("LOGGING OFF!\n");
		}
	}

    if (!DYN_LOGGING_UNLOCK_SHMEM(pSem))
	return FALSE;
    return TRUE;
}

BOOLEAN DynLoggingAttr_Get(PUCHAR payload)
{
    PST_DYNAMIC_LOGGING pstDynLogging = NULL;	// mapped to the payload from (test) app)
    PST_SHM_LOG_REC pstRec = NULL;	// maps to the shmem record being processed
    INT index = 0;
    PMUTEX pSem = NULL;

    pSem = GpWiMAX->stLogging.ptSemShmLock;
    if (!DYN_LOGGING_LOCK_SHMEM(pSem))
	return FALSE;

    DynLogging_Shmem_FixInvalidSlots();
   
    pstDynLogging = (PST_DYNAMIC_LOGGING) payload;

    switch (pstDynLogging->nLogFor)
	{
	  case PROCESS_SELF:
	      pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);
	      if (!pstRec)
		  {
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			     "%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			     __FUNCTION__, GetCurrentProcID());
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }


	      // Let child process use parent's record. ???
#ifndef WIN32
	      if ((pstDynLogging->uiPid != GetCurrentProcID()) && (pstDynLogging->uiPid != GetCurrentProcID()))
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
#endif

	      // Place the 'get return' in record #0 ...
	      pstDynLogging->arrShmLogRecs[0].uDebugFlag = pstRec->uDebugFlag;
	      pstDynLogging->arrShmLogRecs[0].uDebugPath = pstRec->uDebugPath;
	      pstDynLogging->arrShmLogRecs[0].uLogWhere = pstRec->uLogWhere;
	      break;

	  case PROCESS_BY_PID:
	      index = DynLogging_FindProcessByPID(GpWiMAX, pstDynLogging->uiPid);
	      if (-1 == index)
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
//                      QP("Found PID %d! slot=%d\n", pstDynLogging->uiPid, index);

	      pstRec = GetShmLogRecByIndex(GpWiMAX, index);
	      if (!pstRec)
		  {
		      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			     "%s: Failed retrieving shmem log record for PID %d. Aborting...\n",
			     __FUNCTION__, pstDynLogging->uiPid);
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }
	      if (pstRec->nPid != pstDynLogging->uiPid)
		  {
		      DYN_LOGGING_UNLOCK_SHMEM(pSem);
		      return FALSE;
		  }

	      // Place the 'get return' in record #0 ...
	      pstDynLogging->arrShmLogRecs[0].uDebugFlag = pstRec->uDebugFlag;
	      pstDynLogging->arrShmLogRecs[0].uDebugPath = pstRec->uDebugPath;
	      pstDynLogging->arrShmLogRecs[0].uLogWhere = pstRec->uLogWhere;
	      break;

	  case PROCESS_ALL:
	      SHOW_SHM_LOG_RECS(GpWiMAX, SHOW_ONLY_LIVE_PROCESSES);
	      break;

	  default:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Invalid 'nLogFor' value!\n");
	      break;
	}			// switch

    payload = (PUCHAR) pstDynLogging;
    if (!DYN_LOGGING_UNLOCK_SHMEM(pSem))
	return FALSE;
    return TRUE;
}

INT DynLogging_GetCountValidProcessShmLogRec()
{
	PST_SHM_LOG_REC recs_start;
	INT i = 0, iCount = 0;

	if(!GpWiMAX->stLogging.bInited)
		return 0;
	
	recs_start = (PST_SHM_LOG_REC) ((intptr_t) GpWiMAX->stLogging.pstShmLog + sizeof(ST_SHM_LOG));

	DynLogging_Shmem_FixInvalidSlots();
	
    for (i = 0; i < DYN_LOGGING_MAX_PROCESSES; i++)
	{
		if(recs_start->nPid)
			iCount++;
		
	    recs_start++;
	}	
	return iCount;
}

