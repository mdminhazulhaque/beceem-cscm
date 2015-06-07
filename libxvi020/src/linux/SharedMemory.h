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
 * Description	:	This includes _SHARED_MEM structure declarations, 
 *					wrapper for Shared memory creation and accessing.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef SHARED_MEM_H
#define SHARED_MEM_H



#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#ifndef BCM_SHARED_MEM_NOT_SUPPORTED
#include <sys/shm.h>
#endif
#include "Typedefs.h"
#include "ErrorDefs.h"

typedef struct _SHARED_MEM
{
    INT32	iShmId;
	PVOID	pMappedAddress;

}SHARED_MEM, *PSHARED_MEM;

int SharedMem_Open(PSHARED_MEM pSharedMem, UINT uiNumBytes, int ShmKey);
int SharedMem_Create(PSHARED_MEM pSharedMem, UINT uiNumBytes, int ShmKey);
int SharedMem_GetAttachCount (PSHARED_MEM pSharedMem);
int SharedMem_Close(PSHARED_MEM pSharedMem);
int SharedMem_ReadData(PSHARED_MEM pSharedMem, PUCHAR pucData, UINT uiLength, UINT uiOffset);
int SharedMem_WriteData(PSHARED_MEM pSharedMem, const PUCHAR pucData, UINT uiLength, UINT uiOffset);
int MapSharedMem(PSHARED_MEM pSharedMem);
UINT GetCurrentProcID();

#endif /* SHARED_MEM_H */

