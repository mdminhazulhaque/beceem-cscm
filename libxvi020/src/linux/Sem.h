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
 * Description	:	Semaphore implementation API for Linux.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef _SEM_H
#define _SEM_H

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "ErrorDefs.h"

#define MAX_SEMAPHORE_LENGTH 512

typedef struct _SEMAPHORE
{
	sem_t   *pSemaphore;
	char 	szName[MAX_SEMAPHORE_LENGTH];
}SEMAPHORE, *PSEMAPHORE;

int Semaphore_Create(void *pArg, const char* szName, int iInitialValue);
int Semaphore_Wait(void *pArg);
int Semaphore_Post(void *pArg);
int Semaphore_Close(void *pArg);


#endif /* _SEM_H */

