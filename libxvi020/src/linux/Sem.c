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
 * Description	:	Semaphore implementation for Linux.
 * Author		:
 * Reviewer		:
 *
 */


#include "Sem.h"

int Semaphore_Create(void *pArg, const char* szName, int iInitialValue)
{
	PSEMAPHORE pSemObj = (PSEMAPHORE)pArg;
	if(!pSemObj)
		return ERROR_INVALID_ARGUMENT;;
	
	memset(pSemObj->szName, 0, sizeof(pSemObj->szName));
	strcpy(pSemObj->szName, szName);
	pSemObj->pSemaphore = sem_open(pSemObj->szName, O_CREAT, 0777, iInitialValue); 

	if(pSemObj->pSemaphore == (void*) -1)
		return ERROR_SEMAPHORE_INIT_FAILED;
	
	return SUCCESS;
}

int Semaphore_Wait(void *pArg)
{
	PSEMAPHORE pSemObj = (PSEMAPHORE)pArg;
	if(!pSemObj)
		return ERROR_INVALID_ARGUMENT;

	if(sem_wait(pSemObj->pSemaphore) == -1)
		return ERROR_SEMAPHORE_LOCK_FAILED;

	return SUCCESS;
}

int Semaphore_Post(void *pArg)
{
	PSEMAPHORE pSemObj = (PSEMAPHORE)pArg;
	if(!pSemObj)
		return ERROR_INVALID_ARGUMENT;

	if(sem_post(pSemObj->pSemaphore) == -1)
		return ERROR_SEMAPHORE_POST_FAILED;

	return SUCCESS;
}

int Semaphore_Close(void *pArg)
{
	PSEMAPHORE pSemObj = (PSEMAPHORE)pArg;
	if(!pSemObj)
		return ERROR_INVALID_ARGUMENT;

	sem_close(pSemObj->pSemaphore);
	return SUCCESS;
}

