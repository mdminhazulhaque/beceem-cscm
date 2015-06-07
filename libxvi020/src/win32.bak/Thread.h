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
 * Description	:	This includes stThread structure definition, 
 *					wrapper on CreateThread and WaitForSingleObject
 *					functions.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef THREAD_CLASS_H
#define THREAD_CLASS_H

#pragma once

#include "ErrorDefs.h"
#include "Typedefs.h"
#define _WIN32_WINNT 0x0501
#include <Windows.h>

typedef unsigned long (__stdcall *PTHREADFUNC)(void *lpThreadParameter );
typedef void (*PTHREADSTOPFUNC)(void *lpThreadParameter);

#define WAIT_INFINITE 0xFFFFFFFF
#define MAX_KILL_RETRY 10

typedef struct _THREAD
{
	HANDLE hThread;
	void *pvThreadParam;
	PTHREADFUNC pThreadFunc;
	PTHREADSTOPFUNC pThreadStopFunc;
	DWORD dwThreadID;
}THREAD, *PTHREAD;


int Thread_Init(PVOID pThread, void *pvThreadParam);
int Thread_Cleanup(PVOID pThread);
int Thread_Run(PVOID pThread, PTHREADFUNC pThreadFunc, void *pvThreadParam);
BOOL Thread_IsValidThread(PVOID pThread);
DWORD Thread_Wait(PVOID pThread, DWORD dwWaitMilliSec, BOOL bAlertable);
BOOL Thread_IsAlive(PVOID pThread);
int Thread_Stop(PVOID pThread, PTHREADSTOPFUNC pStopFunc, void *pvStopParam);
int Thread_Terminate(PVOID pThread);

//windows only special api.
//Call if from any given thread and it will set the name of that thread visible in the debugger.
int Thread_SetDebugName(const char* lpszDebugThreadName);
BOOL  Is32bitProcessRunningonWOW64();
VOID CheckAndDisableFileSystemRedirection(PVOID pVoiD);
VOID CheckAndRestoreFileSystemRedirection(PVOID pVoiD);


#endif /* THREAD_CLASS_H */
