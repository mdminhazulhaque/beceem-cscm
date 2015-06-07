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
 * Description	:	Shared memory implementation for Windows.
 * Author		:
 * Reviewer		:
 *
 */


#include "SharedMemory.h"
#define UNUSED_ARGUMENT(x) (void)x  //To avoid warning for unused variable


//See the descrption in "SyncObjects.h" about FIX_SECURITY_DACL
#define FIX_SECURITY_DACL 1

// WARNING!!! File mappings are special objects on windows.  Only an administrators can map a file mapping
// into the global object namespace. This poses a problem for us since we cannot share this mapped memory
// between all users unless an elevated user creates it first and keeps it open.  If we keep it in local
// then everyone will be able to use it however it will not actually be shared accross the entire system.
#define FIX_NAMED_OBJECT_NAMESPACE 0


int SharedMem_Open(PSHARED_MEM pSharedMem, UINT uiNumBytes, const char *lpcszName)
{
	
#if FIX_NAMED_OBJECT_NAMESPACE

  char szFixedUpNameBuffer[255] = {0};
#endif
    const char* lpszFixedObjectName = lpcszName;
	UNUSED_ARGUMENT(uiNumBytes);

    if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;
	
    /* if you are specifying a new size for uiNumBytes than the file mapping was originially created
       with, then you would have a problem here.
       the file would not grow.. you could however call create again and that would grow the file
       but I do not believe the data would be re-mapped into existing process's.. this could be problematic
       if you wanted to grow the file past 1k.
    */

#if FIX_NAMED_OBJECT_NAMESPACE

        //now fix up the namespace on the object...
        //and give it a BECEEM_ prefix to make sure we never collide w/ someone 
        //elses name... 
        strcat(szFixedUpNameBuffer,"Global\\BECEEM_");
        strcat(szFixedUpNameBuffer,lpcszName);
        lpszFixedObjectName = szFixedUpNameBuffer;
   
#endif


	pSharedMem->hSharedMemory = OpenFileMapping(
												FILE_MAP_ALL_ACCESS,	/* read/write permission	*/
												FALSE,					/* Do not inherit the name	*/
												lpszFixedObjectName				/* name						*/
												);
	if(!pSharedMem->hSharedMemory)
		return ERROR_SHARED_MEM_OPEN_FAILED;

	return MapSharedMem(pSharedMem);
}

int MapSharedMem(PSHARED_MEM pSharedMem)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

    /* considuer using max size here. */
	pSharedMem->pMappedAddress = MapViewOfFile(
												pSharedMem->hSharedMemory,	/* handle to mapping object */
												FILE_MAP_ALL_ACCESS,		/* read/write permission */
												0,							/* max. object size */
												0,							/* size */
												0							/* map entire file */
												);
	 
	if (!pSharedMem->pMappedAddress) 
		return ERROR_SHARED_MEM_MAPPING_FAILED;

	return SUCCESS;
}

int SharedMem_Create (PSHARED_MEM pSharedMem, UINT uiNumBytes, const char *lpcszName)
{
    SECURITY_DESCRIPTOR secDesc = {0};
    SECURITY_ATTRIBUTES attr = {0};
    LPSECURITY_ATTRIBUTES pSecurityAttributes = 0;
	const char* lpszFixedObjectName = lpcszName;
	
#if FIX_NAMED_OBJECT_NAMESPACE
	char szFixedUpNameBuffer[255] = {0};
#endif

    if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;
	

#if FIX_SECURITY_DACL
    //give everyone access.
    if(InitializeSecurityDescriptor(&secDesc,SECURITY_DESCRIPTOR_REVISION))
    {
        //all access to this object for everyone
        if(SetSecurityDescriptorDacl(&secDesc, TRUE, NULL, FALSE))
        {
            attr.nLength = sizeof(attr);
            attr.lpSecurityDescriptor = &secDesc;
            pSecurityAttributes = &attr;
        }
    }
#endif
#if FIX_NAMED_OBJECT_NAMESPACE

        //now fix up the namespace on the object...
        //and give it a BECEEM_ prefix to make sure we never collide w/ someone 
        //elses name... 
        strcat(szFixedUpNameBuffer,"Global\\BECEEM_");
        strcat(szFixedUpNameBuffer,lpcszName);
		lpszFixedObjectName = szFixedUpNameBuffer;
  
#endif
	pSharedMem->hSharedMemory = CreateFileMapping(
													INVALID_HANDLE_VALUE,		/* current file handle */
													pSecurityAttributes,			/* default security  */
													PAGE_READWRITE,	/* read/write permission */
													0,				/* max. object size */
													uiNumBytes,		/* size */
													lpszFixedObjectName		/* name of mapping object */
													);
	if(!pSharedMem->hSharedMemory)
	{
		return ERROR_SHARED_MEM_OPEN_FAILED;
	}

	pSharedMem->uiNumBytes = uiNumBytes;

	return MapSharedMem(pSharedMem);
}

int SharedMem_Close(PSHARED_MEM pSharedMem)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;


	if(pSharedMem->pMappedAddress)
	{
		UnmapViewOfFile(pSharedMem->pMappedAddress); 
		pSharedMem->pMappedAddress = NULL;
	}

	/* Close the process's handle to the file-mapping object. */
	if(pSharedMem->hSharedMemory)
	{
		CloseHandle(pSharedMem->hSharedMemory); 
		pSharedMem->hSharedMemory = NULL;
	}

	return SUCCESS;
}

int SharedMem_ReadData(PSHARED_MEM pSharedMem, PUCHAR pucData, UINT uiLength, UINT uiOffset)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

	if(!pSharedMem->hSharedMemory)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

    /* consider checking against the file size */
	memcpy(pucData, ((UCHAR *)pSharedMem->pMappedAddress)+uiOffset, uiLength);

	return SUCCESS;
}

int SharedMem_WriteData(PSHARED_MEM pSharedMem, const PUCHAR pucData, UINT uiLength, UINT uiOffset)
{
	if(!pSharedMem)
		return ERROR_INVALID_ARGUMENT;

	if(!pSharedMem->hSharedMemory)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

    /* consider checking against the max size of the file size */
	memcpy(((UCHAR *)pSharedMem->pMappedAddress)+uiOffset, pucData, uiLength);

	return SUCCESS;
}

UINT GetCurrentProcID()
{
    return (UINT)GetCurrentProcessId();
}
