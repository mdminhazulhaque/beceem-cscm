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
 * Description	:	Synchronization object implementation for Windows.
 * Author		:
 * Reviewer		:
 *
 */


#include "SyncObjects.h"

//windows 2000 and greater supported.
#define _WIN32_WINNT 0x0500
#include <sddl.h>

//This is very important,  when using named events, it is possible for the default security
//on the event to block other users from opening it.
//
// This will fix the following scenarios
//
// Scenario 1#, WinXP.
// 1. A service is using this api and is the app on the system to run the api so it 
//    creates a shared event w/o fixing the dacl on it. (the system account runs in session0)
// 2. A 'regular user' account (i.e. non-admin) logs in on the Xp machine (if it is the first user account logged it it will also be 
//    running under session0).
// 3. The regular user will fail to open the events, because they will both be under session0\\evenname
//    and only an admin account would have permissions to open up an event created by a service.
//
//
// Scenario 2#, Windows Vista
// 1. An admin account is logged in w/ UAC turned on
// 2. An app is run w/ "Run As Admin" option and creates the named event.
// 3. An app is run as a under the same account w/o the "Run As Admin" option"
// 4. The app created in step 3 will fail to open the event w/ access denied.
//
// There are additional scenarios aswell.. esp if you choose to use the "Global\\" namespace
#define FIX_SECURITY_DACL 1

//Do we actually want these to be shared accross the system? .. or just the current logged in user...
//In windows, named events and objects are created in something called a namespace.  
//For example.
// 1.  An event is created w/ the name "TestEvent"
//      a.  This is equivelent to "Local\\TestEvent"
//      b.  Is also equivelent to "<TheCurrentSession#>\\TestEvent"..i.e "Session0\\TestEvent".. or "Session1\\TestEvent" depending 
//          on the user account calling it
//      c.  Is unique only in the current session,  other logged in users or services may have
//          a different event that is also named "TestEvent" but the two objects are not linked together
// 2.  An event is created w/ the name "Global\\TestEvent"
//      b.  Is also equivelent to "<TheCurrentSession#>\TestEvent"..i.e "Session0\TestEvent"
//
//
//Warning, if you choose to enable this,  for all of the objects to be linked together everyone must upgrade to the latest
//api.  Since "Local\\TestEvent", would be difference then "Global\\TestEvent"
//
#define FIX_NAMED_OBJECT_NAMESPACE 0

//special function to detect vista or better.
// these newer versions of the OS should also support 'integrity checking' on permissions
//
BOOL IsVistaOrBetter()
{
    // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
    // If that fails, try using the OSVERSIONINFO structure.
    OSVERSIONINFOEX osvi = {0};
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if( !GetVersionEx ((OSVERSIONINFO *) &osvi) )
    {
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
            return FALSE;
    }
    if(osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 0)
    {   
        return TRUE;
    }
    return FALSE;    
}






/********************************************************************************
*																				*
*						MUTEX FUNCTION IMPLEMENTATIONS							*
*																				*
********************************************************************************/

int Mutex_Init(PVOID pVoid)
{
	PMUTEX pMutex;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pMutex = (PMUTEX)pVoid;
	pMutex->hMutex = NULL;
	return SUCCESS;
}

int Mutex_Cleanup(PVOID pVoid)
{
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	return Mutex_Release(pVoid);
}

int Mutex_Create(PVOID pVoid, const char *szName)
{
	PMUTEX pMutex;
#if FIX_NAMED_OBJECT_NAMESPACE
    char szFixedUpNameBuffer[255] = {0};
#endif
    const char* lpszFixedObjectName = szName;
    LPSECURITY_ATTRIBUTES pSecurityAttributes = 0;

    SECURITY_DESCRIPTOR secDesc = {0};
    SECURITY_ATTRIBUTES attr = {0};
    PSECURITY_DESCRIPTOR pSD = NULL;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pMutex = (PMUTEX)pVoid;
	if(pMutex->hMutex != NULL)
		return SUCCESS;
  
    //if this is a named object then we are going to open up the permissions on it
    //so people in other accounts / sessions can also use the same shared named object.
    //Starting w/ vista services and the logged in users are no longer in the same session anymore aswell.
    if(szName)
    {
#if FIX_SECURITY_DACL
        if(InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION))
        {
            //Give a NULL DACL for all access to everyone
            if(SetSecurityDescriptorDacl(&secDesc, TRUE, NULL, FALSE))
            {
                attr.nLength = sizeof(attr);
                attr.lpSecurityDescriptor = &secDesc;
                pSecurityAttributes = &attr;

                //Vista introduces "integrity" check levels.  We are going to set this to "low"
                //to prevent us from locking out a low-integrity process if someone in 
                //a service uses the api to open the mutex first.
                if(IsVistaOrBetter())
                {
                    if(ConvertStringSecurityDescriptorToSecurityDescriptor( "S:(ML;;NW;;;LW)", // "low integrity"
                        SDDL_REVISION_1,
                        &pSD,
                        NULL))
                    {
                        PACL pSacl = NULL;                  // not allocated
                        BOOL fSaclPresent = FALSE;
                        BOOL fSaclDefaulted = FALSE;
                        GetSecurityDescriptorSacl(
                            pSD,
                            &fSaclPresent,
                            &pSacl,
                            &fSaclDefaulted);
                        //Set the SACL w/ low-integrity checks
                        SetSecurityDescriptorSacl(attr.lpSecurityDescriptor, TRUE, pSacl, FALSE);
                    }
                }
            }
        }
#endif        

#if FIX_NAMED_OBJECT_NAMESPACE
        //now fix up the namespace on the object...
        strcat(szFixedUpNameBuffer,"Global\\");
        strcat(szFixedUpNameBuffer,szName);
        lpszFixedObjectName = szFixedUpNameBuffer;
#endif
    }

	/* Create a mutex with specified params */
	pMutex->hMutex = CreateMutex(	
							pSecurityAttributes,
							FALSE,
							lpszFixedObjectName
						);

    //local free if needed
    if(pSD)
    {
        //preserve last error
        DWORD dwLastError = GetLastError();
        LocalFree((HLOCAL)pSD);
        SetLastError(dwLastError);
    }

	if(pMutex->hMutex == NULL)
		return ERROR_CREATE_MUTEX_FAILED;
	return SUCCESS;
}
void Mutex_Lock(PVOID pVoid)
{
	PMUTEX pMutex;
    DWORD dwWaitResult; 

	if(!pVoid)
		return;

	pMutex = (PMUTEX)pVoid;

    /*  Request ownership of mutex. */
     dwWaitResult = WaitForSingleObject( 
										pMutex->hMutex,			/*  handle to mutex */
										INFINITE			/*  Wait timeout  */
									  );   
	return;								
}
int Mutex_Unlock(PVOID pVoid)
{
	PMUTEX pMutex;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pMutex = (PMUTEX)pVoid;

	/* Release the mutex ownership */
	if(ReleaseMutex(pMutex->hMutex))
		return SUCCESS;
	return ERROR_RELEASE_MUTEX_FAILED;
}
int Mutex_Release(PVOID pVoid)
{
	PMUTEX pMutex;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;

	pMutex = (PMUTEX)pVoid;

	if(pMutex->hMutex != NULL)
	{
		/* If Mutex is not release yet!  */
		/* Release it and close the mutex handle */
		if(!ReleaseMutex(pMutex->hMutex))
		{
			if(CloseHandle(pMutex->hMutex))
			{
				pMutex->hMutex = NULL;
				return SUCCESS;
			}
			return ERROR_CLOSE_MUTEX_HANDLE_FAILED;
		}
		return ERROR_RELEASE_MUTEX_FAILED;
	}
	return SUCCESS;
}



/********************************************************************************
*																				*
*						EVENT FUNCTION IMPLEMENTATION							*
*																				*
********************************************************************************/

int Event_Init(PVOID pVoid)
{
	PEVENT pEvent;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;
	pEvent->hEvent = NULL;

	return 0;
}

int Event_Cleanup(PVOID pVoid)
{
	return Event_Release(pVoid);
}

int Event_Create(
						PVOID pVoid,
						BOOL bManualReset,
						BOOL bInitialState,
						const char *szName
					)
{

	PEVENT pEvent;

#if FIX_NAMED_OBJECT_NAMESPACE

    char szFixedUpNameBuffer[255] = {0};
#endif
    const char* lpszFixedObjectName = szName;
    LPSECURITY_ATTRIBUTES pSecurityAttributes = 0;
    SECURITY_DESCRIPTOR secDesc = {0};
    SECURITY_ATTRIBUTES attr = {0};
    PSECURITY_DESCRIPTOR pSD = NULL;

	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;

	if(pEvent->hEvent != NULL)
		return SUCCESS;


    //if this is a named object then we are going to open up the permissions on it
    //so people in other accounts / sessions can also use the same shared named object.
    //Starting w/ vista services and the logged in users are no longer in the same session anymore aswell.
    if(szName)
    {
#if FIX_SECURITY_DACL
        if(InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION))
        {
            //all access to this object for everyone
            if(SetSecurityDescriptorDacl(&secDesc, TRUE, NULL, FALSE))
            {
                attr.nLength = sizeof(attr);
                attr.lpSecurityDescriptor = &secDesc;
                pSecurityAttributes = &attr;

                //Vista introduces "integrity" check levels.  We are going to set this to "low"
                //to prevent us from locking out a low-integrity process if someone in 
                //a service uses the api to open the mutex first.
                if(IsVistaOrBetter())
                {
                    if(ConvertStringSecurityDescriptorToSecurityDescriptor( "S:(ML;;NW;;;LW)", // "low integrity"
                        SDDL_REVISION_1,
                        &pSD,
                        NULL))
                    {
                        PACL pSacl = NULL;                  // not allocated
                        BOOL fSaclPresent = FALSE;
                        BOOL fSaclDefaulted = FALSE;
                        GetSecurityDescriptorSacl(
                            pSD,
                            &fSaclPresent,
                            &pSacl,
                            &fSaclDefaulted);
                        SetSecurityDescriptorSacl(attr.lpSecurityDescriptor, TRUE, pSacl, FALSE);
                    }
                }
            }
        }
#endif        
#if FIX_NAMED_OBJECT_NAMESPACE
        //For creating the event in the global namespace
        strcat(szFixedUpNameBuffer,"Global\\");
        strcat(szFixedUpNameBuffer,szName);
        lpszFixedObjectName = szFixedUpNameBuffer;
#endif
    }

    pEvent->hEvent = CreateEvent(
							pSecurityAttributes,    /*  no security attributes */
							bManualReset,			/*  manual-reset event flag */
							bInitialState,			/*  initial state flag */
							lpszFixedObjectName			/*  object name */
						  ); 
    //local free if needed
    if(pSD)
    {
        //preserve last error
        DWORD dwLastError = GetLastError();
        LocalFree((HLOCAL)pSD);
        SetLastError(dwLastError);
    }

	if(pEvent->hEvent == NULL)
		return ERROR_CREATE_EVENT_FAILED;

	return SUCCESS;
}
int Event_Set(PVOID pVoid)
{
	BOOL bSet;
	PEVENT pEvent;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;

	/* Set the event */
	bSet = SetEvent(pEvent->hEvent);

	if(bSet)
		return SUCCESS;

	return ERROR_SET_EVENT_FAILED;
}
int Event_Reset(PVOID pVoid)
{
	BOOL bReset;
	PEVENT pEvent;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;

	/* Reset the event */
	bReset = ResetEvent(pEvent->hEvent);

	if(bReset)
		return SUCCESS;

	return ERROR_RESET_EVENT_FAILED;
}
DWORD Event_Wait(PVOID pVoid, DWORD dwWaitTimeOut)
{
	PEVENT pEvent;
	if(!pVoid)
		return (DWORD)ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;

	if(pEvent->hEvent != NULL)
		return WaitForSingleObject(pEvent->hEvent, dwWaitTimeOut);
	return (DWORD)ERROR_INVALID_EVENT_HANDLE;
}
int Event_Release(PVOID pVoid)
{
	PEVENT pEvent;
	if(!pVoid)
		return ERROR_INVALID_ARGUMENT;
	pEvent = (PEVENT) pVoid;

	/* changed to check for null, previously we were never closing any handles. */
	if(pEvent->hEvent == NULL)
		return SUCCESS;

	if(CloseHandle(pEvent->hEvent) == FALSE)
		return ERROR_CLOSE_EVENT_HANDLE_FAILED;

	pEvent->hEvent = NULL;
	return SUCCESS;
}
