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
 * Description	:	Shared memory implementation for Linux.
 * Author		:
 * Reviewer		:
 *
 */



#include "SharedMemory.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
#include <sys/shm.h>
#endif
#include "BeceemWiMAX.h"
#ifdef MACOS
#include <sys/ipc.h>
#endif

int SharedMem_Open(PSHARED_MEM pSharedMem, UINT uiNumBytes, int ShmKey)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;
 #ifndef BCM_SHARED_MEM_NOT_SUPPORTED
   
	pSharedMem->iShmId = shmget(
								  ShmKey,
								  uiNumBytes,
								  S_IRUSR |S_IWUSR |S_IRGRP|S_IWGRP |S_IROTH |S_IWOTH 
								  );
 
	if(pSharedMem->iShmId < 0)
		return ERROR_SHARED_MEM_OPEN_FAILED;
	else
		return MapSharedMem(pSharedMem);
#else
    return 0;
#endif


}

int SharedMem_Create(PSHARED_MEM pSharedMem, UINT uiNumBytes, int ShmKey)
{ 
    
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

 #ifndef BCM_SHARED_MEM_NOT_SUPPORTED

	pSharedMem->iShmId = shmget(
								  ShmKey,
								  uiNumBytes,
								  IPC_CREAT|S_IRUSR |S_IWUSR |S_IRGRP|S_IWGRP |S_IROTH |S_IWOTH 						  
								);

	if(pSharedMem->iShmId < 0)
	{
		DPRINT_OTHER ("%s: Shared memory creation failed\n", __FUNCTION__);
		return ERROR_SHARED_MEM_OPEN_FAILED;
	}	
	else
	{
		return MapSharedMem(pSharedMem);
	}
#else
    return 0;
#endif
}

int MapSharedMem(PSHARED_MEM pSharedMem)
{
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

//	DPRINT_OTHER ("%s: Mapping shared memory\n", __FUNCTION__);

	pSharedMem->pMappedAddress = shmat(pSharedMem->iShmId,NULL,0);

	if (pSharedMem->pMappedAddress == (void *)-1)
    {
       	pSharedMem->pMappedAddress = NULL;
       	DPRINT_OTHER("\n%s:  mmap failed, Error : %d\n", __FUNCTION__, errno);
 		return ERROR_SHARED_MEM_MAPPING_FAILED;
    }


	return SUCCESS;
#else
	UNUSED_ARGUMENT(pSharedMem);

 	return ERROR_SHARED_MEM_OPEN_FAILED;
#endif
}

int SharedMem_GetAttachCount (PSHARED_MEM pSharedMem)
{
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
    struct shmid_ds stShmds;
	if (shmctl (pSharedMem->iShmId, IPC_STAT, &stShmds) < 0) {
		DPRINT_OTHER ("WARNING! shmctl failed.\n");
		return -1;
	}
	return stShmds.shm_nattch;
#else
    return 0;
#endif
}
int SharedMem_Close(PSHARED_MEM pSharedMem)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
	if(pSharedMem->pMappedAddress)
	{
	    int GetAttachCount=0;
	    QP ("pSharedMem->pMappedAddress = %p\n", pSharedMem->pMappedAddress);

        GetAttachCount=SharedMem_GetAttachCount(pSharedMem);
        DPRINT_OTHER("\n #of process Alive : %d with ShmID: %d",GetAttachCount,pSharedMem->iShmId);
        if( GetAttachCount==1)
        {
            if (shmctl(pSharedMem->iShmId, IPC_RMID, 0) < 0)
    		{
    		    DPRINT_OTHER("%s: shmctl fail.\n", __FUNCTION__);
    		    return ERROR_SHARED_MEM_CLOSE_FAILED;
    		}
        }
        else
        {
            shmdt(pSharedMem->pMappedAddress);
        }
	    pSharedMem->pMappedAddress = NULL;
	}
#endif
	return SUCCESS;
}

int SharedMem_ReadData(PSHARED_MEM pSharedMem, PUCHAR pucData, UINT uiLength, UINT uiOffset)
{
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED

	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

	if(pSharedMem->pMappedAddress == NULL)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

	memcpy(pucData, ((UCHAR *)pSharedMem->pMappedAddress)+uiOffset, uiLength);
#endif
	return SUCCESS;
}

int SharedMem_WriteData(PSHARED_MEM pSharedMem, const PUCHAR pucData, UINT uiLength, UINT uiOffset)
{
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED

	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

	if(pSharedMem->pMappedAddress == NULL)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

	memcpy(((UCHAR *)pSharedMem->pMappedAddress)+uiOffset, pucData, uiLength);
#endif
	return SUCCESS;
}
UINT GetCurrentProcID()
{
    return (UINT)getpid();
}
