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
 * Description	:	Thread implementation for Windows.
 * Author		:
 * Reviewer		:
 *
 */


#include "Thread.h"
#include <stdio.h>
#include "BeceemWiMAX.h"
#include "globalobjects.h"
#define MAX_TIMEOUT_TO_LAUNCH_THREAD 100000
typedef BOOL (__stdcall *lpIsWow64Process)(HANDLE,PBOOL);
typedef BOOL (__stdcall *lpWow64DisableWow64FsRedirection) (PVOID  ); 
typedef BOOL (__stdcall *lpWow64RevertWow64FsRedirection) (PVOID );

int Thread_Init(PVOID pVoid, void *pvThreadParam)
{
	PTHREAD pThread;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pThread = (PTHREAD)pVoid;
	pThread->hThread = INVALID_HANDLE_VALUE;
	pThread->pvThreadParam = pvThreadParam;
	pThread->pThreadFunc = NULL;
	pThread->dwThreadID = 0;

	return 0;
}

int Thread_Cleanup(PVOID pVoid)
{
	PTHREAD pThread;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pThread = (PTHREAD)pVoid;

	if(Thread_IsValidThread(pThread))
    {
		CloseHandle(pThread->hThread);
        pThread->hThread = INVALID_HANDLE_VALUE;
    }

	return 0;
}

int Thread_Run(PVOID pVoid, PTHREADFUNC pThreadFunc, void *pvThreadParam)
{
	PTHREAD pThread;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pThread = (PTHREAD)pVoid;

	pThread->pvThreadParam = pvThreadParam;
	pThread->pThreadFunc = pThreadFunc;

	if(!pThread->pThreadFunc)
		return ERROR_NULL_THREAD_FUNC;

    if(Thread_IsValidThread(pThread))
    {
        if(Thread_Wait(pThread,MAX_TIMEOUT_TO_LAUNCH_THREAD,FALSE) == WAIT_TIMEOUT)
	        return ERROR_THREAD_CREATION_FAILED;
	    else
        {
            CloseHandle(pThread->hThread);
            pThread->hThread = INVALID_HANDLE_VALUE;
        }
    }

	pThread->hThread = CreateThread(
								NULL,
								0,
								pThread->pThreadFunc,
								pThread->pvThreadParam,
								0,
								&pThread->dwThreadID
							);

	
	return Thread_IsValidThread(pThread) ? SUCCESS : ERROR_THREAD_CREATION_FAILED;
}

BOOL Thread_IsValidThread(PVOID pVoid)
{
	PTHREAD pThread;

	if(!pVoid)
		return FALSE;

	pThread = (PTHREAD)pVoid;

	if(pThread->hThread == INVALID_HANDLE_VALUE || pThread->hThread == NULL)
		return FALSE;

	return TRUE;
}

DWORD Thread_Wait(PVOID pVoid, DWORD dwWaitMilliSec, BOOL bAlertable)
{
	PTHREAD pThread;

	if(!pVoid)
		return FALSE;

	pThread = (PTHREAD)pVoid;
	
    if(!Thread_IsValidThread(pThread))
        return ERROR_INVALID_HANDLE;
	if(!bAlertable)
		return WaitForSingleObject(pThread->hThread, dwWaitMilliSec);
	else
		return WaitForSingleObjectEx(pThread->hThread, dwWaitMilliSec, TRUE);
}

BOOL Thread_IsAlive(PVOID pVoid)
{
	PTHREAD pThread = NULL;

	if(!pVoid)
		return FALSE;

	pThread = (PTHREAD)pVoid;

	if(!Thread_IsValidThread(pThread))
		return FALSE;

    if(WaitForSingleObject(pThread->hThread,0) == WAIT_TIMEOUT)
        return TRUE;
    else 
        return FALSE;
}

int Thread_Stop(PVOID pVoid, PTHREADSTOPFUNC pStopFunc, void *pvStopParam)
{
	int i = 0;
	PTHREAD pThread;
	
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pThread = (PTHREAD)pVoid;

	if(!pStopFunc)
		return ERROR_INVALID_STOP_FUNC;

	pThread->pThreadStopFunc = pStopFunc;

	for(i=0; i<MAX_KILL_RETRY; i++)
	{
		if(!Thread_IsAlive(pThread))
		{
			if(Thread_IsValidThread(pThread))
				CloseHandle(pThread->hThread);
			pThread->hThread = INVALID_HANDLE_VALUE;
			return SUCCESS;
		}
		pThread->pThreadStopFunc(pvStopParam);
        WaitForSingleObject(pThread->hThread,500);
	}
	return ERROR_THREAD_NOT_STOPPED;
}

int Thread_Terminate(PVOID pVoid)
{
	int iRet;
	PTHREAD pThread;
	
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pThread = (PTHREAD)pVoid;

	if(!Thread_IsAlive(pThread))
	{
		return ERROR_THREAD_NOT_RUNNING;
	}
	iRet = TerminateThread(pThread->hThread, (DWORD)ERROR_THREAD_TERMINATED) ? SUCCESS : ERROR_TERMINATION_FAILED;
	return iRet;
}

//==================================================
//WHEN use w/ visual studio this will allow you
// to have named threads in the debuggers threads window
//==================================================
typedef struct _THREADNAME_INFO
{
    UINT32 dwType;
    char *pszName;
    UINT32 dwThreadID;
    UINT32 dwFlags;
}THREADNAME_INFO;

int Thread_SetDebugName(const char* lpszDebugThreadName) 
{
    UINT32 dwSuccess = 1;
    THREADNAME_INFO tni = {0};
    tni.dwType = 0x1000;
    tni.pszName = (char*)lpszDebugThreadName;
    tni.dwThreadID = (UINT32)-1;
    __try {
        RaiseException( 0x406d1388, 0,sizeof(tni),(ULONG_PTR*)&tni);
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        dwSuccess = 0;
    };
    return dwSuccess;
}
VOID CheckAndDisableFileSystemRedirection(PVOID pVoiD)
{

   lpWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"CheckAndDisableFileSystemRedirection==>");
   if(!Is32bitProcessRunningonWOW64())
   	{
   		return;
   	}
   
		Wow64DisableWow64FsRedirection = (lpWow64DisableWow64FsRedirection) GetProcAddress(
   	     GetModuleHandle(TEXT("kernel32")),"Wow64DisableWow64FsRedirection");
		if(Wow64DisableWow64FsRedirection == NULL)
			{
				 DPRINT (DEBUG_ERROR, DEBUG_PATH_OTHER,"Wow64DisableWow64FsRedirection doesn't exist");
				 return;
			}
		DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"CheckAndDisableFileSystemRedirection Before");
		if(Wow64DisableWow64FsRedirection(pVoiD) == FALSE)
			{
				 DPRINT (DEBUG_ERROR, DEBUG_PATH_OTHER,"Wow64DisableWow64FsRedirection failed last error is %d",GetLastError());
			}
   	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"CheckAndDisableFileSystemRedirection<==");

}

VOID CheckAndRestoreFileSystemRedirection(PVOID pVoiD)
{

   lpWow64RevertWow64FsRedirection Wow64RevertWow64FsRedirection;
	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"CheckAndRestoreFileSystemRedirection ==>");
   if(!Is32bitProcessRunningonWOW64())
   	{
   		return;
   	}
   
		Wow64RevertWow64FsRedirection = (lpWow64RevertWow64FsRedirection) GetProcAddress(
   	     GetModuleHandle(TEXT("kernel32")),"Wow64RevertWow64FsRedirection");
		
		if( Wow64RevertWow64FsRedirection == NULL)
			{
				 DPRINT (DEBUG_ERROR, DEBUG_PATH_OTHER,"Wow64RevertWow64FsRedirection doesn't exist");
				 return;
			}
		
		if(Wow64RevertWow64FsRedirection(pVoiD) == FALSE)
			{
				 DPRINT (DEBUG_ERROR, DEBUG_PATH_OTHER,"Wow64DisableWow64FsRedirection failed last error is %d",GetLastError());
			}
   	
	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"<==CheckAndRestoreFileSystemRedirection");
}

BOOL  Is32bitProcessRunningonWOW64()
{
	 BOOL bIsWow64 = FALSE;
	 
	lpIsWow64Process fnIsWow64Process;
	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"Is32bitProcessRunningonWOW64==>");
    fnIsWow64Process = (lpIsWow64Process) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

  	
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
          DPRINT (DEBUG_ERROR, DEBUG_PATH_OTHER,"\nIs a WOW 64 process failed !!");
        }
	}
	DPRINT (DEBUG_MESSAGE, DEBUG_PATH_OTHER,"<==Is32bitProcessRunningonWOW64 %d",bIsWow64);
	return bIsWow64;
}

