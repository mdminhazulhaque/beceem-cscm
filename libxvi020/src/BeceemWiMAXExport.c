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
 * Description	:	This exports following functionalities
 *						- Open device handle
 *						- Close device handle
 *						- Register application's callback function
 *						- Send messages to the driver
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAXExport.h"
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	#include "HostProxy_AsyncExport.h"
#endif
#endif
#include "BeceemWiMAX.h"

#include "WiMAXCommonAPI.h"
/* Global WiMAX object */
static BECEEMWIMAX GWiMAXObj;
BECEEMWIMAX *GpWiMAX = &GWiMAXObj;
#include "globalobjects.h"
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD

typedef INT (*LPOPENWIMAXDEVICE) (PSTPROXYPARAMETERS  ); 

typedef VOID (*LPCLOSECOMMUNICATIONPORT) ();

typedef INT  (*LPSENDDATAMESSAGE)(PUCHAR , UINT ,USHORT );

typedef INT (*LPREGISTERCALLBACKREMOTE)(PFNBCMCALLBACK );

typedef INT  (*LPRDMREMOTE)(UINT , UCHAR *, INT );

typedef INT  (*LPWRMREMOTE)(UINT , UCHAR *, INT );

BOOL g_bOverProxy = FALSE;
HMODULE hHostProxyDLL = NULL;

LPOPENWIMAXDEVICE lpOpenWiMAXDeviceRemote;

LPCLOSECOMMUNICATIONPORT lpCloseCommPortRemote;

LPSENDDATAMESSAGE lpSendDataMessageRemote;

LPREGISTERCALLBACKREMOTE lpRegisterCallBackRemote;

LPRDMREMOTE  lpRDMRemote;

LPWRMREMOTE lpWRMRemote;
#endif


/*
*	Function		:	DllMain
*	Description		:	This function will be invoked whenever this DLL is loaded or unloaded from an application.
*						All initialisation and cleanup tasks are performed here.
*	Return value	:	BOOL
*	OS				:	WIN32
*/
BOOL APIENTRY DllMain(
			HANDLE hModule, 
			DWORD  ul_reason_for_call, 
			LPVOID lpReserved
		)
{
	UNUSED_ARGUMENT(lpReserved);
	UNUSED_ARGUMENT(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		if(BeceemWiMAX_Init(GpWiMAX))
				return FALSE;
		
		/*For Common APIs*/
		if(WiMAXCommonAPI_Init(GpWiMAX)) 
				return FALSE;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		#ifdef PROXY_ENABLED_BUILD
		if(g_bOverProxy)
		{
			FreeLibrary(hHostProxyDLL);
			hHostProxyDLL = NULL;
			g_bOverProxy  = FALSE;
		}
        #endif
		
        WiMAXCommonAPI_Cleanup(GpWiMAX);
		BeceemWiMAX_Cleanup(GpWiMAX);		
		break;
	}
    return TRUE;
}

#else
UINT32 g_bCleanupSem;

/*
*	Function		:	my_init
*	Parameters		:	NONE
*	Description		:	This function will be invoked whenever this DLL is loaded from an application.
*						All initialisation tasks are performed here.			
*	Return value	:	VOID
*	OS				:	LINUX
*/
void my_init()
{
	if(BeceemWiMAX_Init(GpWiMAX))
		return;

	/*For Common APIs*/
	if(WiMAXCommonAPI_Init(GpWiMAX))
		return;
}

/*
*	Function		:	my_fini
*	Parameters		:	NONE
*	Description		:	This function will be invoked whenever this DLL is unloaded from an application.
*						All cleanup tasks are performed here.
*	Return value	:	VOID
*	OS				:	LINUX
*/
void my_fini()
{
    if(SharedMem_GetAttachCount(&GpWiMAX->m_shmDynLogging) <= 1)
    {
        g_bCleanupSem = TRUE;
    }
	WiMAXCommonAPI_Cleanup(GpWiMAX);
    BeceemWiMAX_Cleanup(GpWiMAX);
}

#endif
#ifdef PROXY_ENABLED_BUILD
PVOID OpenWiMAXDeviceRemote(      
									PSTPROXYPARAMETERS pstProxyParameters
									)
{
#ifdef WIN32 
	if(!g_bOverProxy)
	{
		hHostProxyDLL = LoadLibrary(HOST_PROXY_DLL_NAME );

		lpOpenWiMAXDeviceRemote = (LPOPENWIMAXDEVICE)GetProcAddress(hHostProxyDLL,"OpenCommunicatonPort");

		lpCloseCommPortRemote = (LPCLOSECOMMUNICATIONPORT)GetProcAddress(hHostProxyDLL,"CloseCommunicationPort");

		lpSendDataMessageRemote = (LPSENDDATAMESSAGE )GetProcAddress(hHostProxyDLL,"SendDataMessage");

		lpRegisterCallBackRemote = (LPREGISTERCALLBACKREMOTE)GetProcAddress(hHostProxyDLL,"RegisterCallbackForProxy");

		lpRDMRemote = (LPRDMREMOTE)GetProcAddress(hHostProxyDLL,"rdmremote");

		lpWRMRemote = (LPWRMREMOTE)GetProcAddress(hHostProxyDLL,"wrmremote");

		g_bOverProxy = TRUE;
	}
	return (PVOID)(ULONG64)lpOpenWiMAXDeviceRemote(pstProxyParameters);
#else
	GpWiMAX->m_bLibraryOperatingOVerProxy = TRUE;
/* 
- Force endian to be LE if this is sent for all the data to/from the library
*/
return NULL;

#endif

}

#endif 
/*
*	Function		:	OpenWiMAXDevice
*	Parameters		:	NONE
*	Description		:	Exported function to open the BcmPCMCIACard device handle.
*	Return value	:	A valid handle on success, error code(<=0) on failure.
*/
PVOID OpenWiMAXDevice()
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
		
		return (PVOID)(ULONG64)ERROR_DEVICE_OPENED_FOR_REMOTE_OPERATION;
	}
#endif   
#endif
	if(!GpWiMAX)
		return (PVOID)(ULONG64)(ERROR_LIB_INITIALIZATION);

	if(GpWiMAX->m_bCommonAPIIfActive)
		return (PVOID)(ULONG64)(ERROR_COMMON_API_INTERFACE_ACTIVE);

	return (PVOID)BeceemWiMAX_CreateDevice(GpWiMAX, BCM_SYMBOLIC_NAME);
}

/*
*	Function		:	CloseWiMAXDevice
*	Parameters		:	NONE
*	Description		:	Exported function to close the BcmPCMCIACard device handle.
*	Return value	:	VOID
*/
VOID CloseWiMAXDevice()
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
		lpCloseCommPortRemote();
		 return;
	}
#endif
#endif

	if(!GpWiMAX)
		return;

	BeceemWiMAX_CloseDevice(GpWiMAX);
}

/*
*	Function		:	RegisterCallback
*	Parameters		:	
*						PVOID pvHandle - Valid handle returned by OpenWiMAXDevice().
*						PVOID pCallbackFnPtr - Application's callback function address.
*						PVOID pvCallbackContext - Pointer to the context object.
*	Description		:	Exported function for registering the application's callback function.
*						A callback function has to be registered with the interface,
*						so that all asynchronous events can be notified to the application through this. 
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
UINT RegisterCallback(PVOID pvHandle, PVOID pCallbackFnPtr, PVOID pvCallbackContext)
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
		lpRegisterCallBackRemote(pCallbackFnPtr);
		return SUCCESS;
	}
#endif
#endif

	if(!GpWiMAX)
		return (UINT)ERROR_LIB_INITIALIZATION;

    if(BeceemWiMAX_GetDeviceHandle(GpWiMAX) != (HANDLE)pvHandle)
        return (UINT)ERROR_INVALID_DEVICE_HANDLE;

	if(!pCallbackFnPtr)
		return (UINT)ERROR_INVALID_NOTIFY_ROUTINE;

    return BeceemWiMAX_RegisterAppCallback((PVOID)GpWiMAX,pCallbackFnPtr, pvCallbackContext);
}

BOOLEAN g_CallByTestApp = TRUE;
VOID VerifiedWhoCall(BOOLEAN who)
{
	g_CallByTestApp = who;
	return;
}

/*
*	Function		:	SendWiMAXMessage
*	Parameters		:	
*						PVOID pvHandle - Valid handle returned by OpenWiMAXDevice().
*						PUCHAR pucSendBuffer - Data to be sent to the device(driver).
*						UINT uiLength - lenth of the above buffer.
*
*	Description		:	Exported function for sending the data to the device(driver).
*
*						This function has to be called by the application when
*						it needs to request information from the baseband modem 
*						or when it needs to invoke a particular functionality in the modem.
*
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
INT SendWiMAXMessage(PVOID pvHandle, PUCHAR pucSendBuffer, UINT uiLength)
{
    INT iRetVal = SUCCESS;
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD

	if(g_bOverProxy)
	{
		iRetVal = lpSendDataMessageRemote(pucSendBuffer,uiLength,COMMAND_MSG);
		
		return iRetVal;

	}
#endif
#endif
	
	if(!GpWiMAX)
		return ERROR_LIB_INITIALIZATION;

	if(BeceemWiMAX_GetDeviceHandle(GpWiMAX) != (HANDLE)pvHandle)
		return ERROR_INVALID_DEVICE_HANDLE;

    if(!pucSendBuffer)
		return ERROR_INVALID_BUFFER;

	if(uiLength == 0 || (INT)uiLength > SIZE_WIMAX_MESSAGE)
        return ERROR_INVALID_BUFFER_LEN;
    HEX_DUMP(pucSendBuffer,uiLength);
    iRetVal = BeceemWiMAX_SendMessage(GpWiMAX, (PWIMAX_MESSAGE) pucSendBuffer, uiLength);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "ret val for T#%d and ST#%d is %d",
            ((PWIMAX_MESSAGE) pucSendBuffer)->usType, 
            ((PWIMAX_MESSAGE) pucSendBuffer)->usSubType,iRetVal);
	return iRetVal;
}

/*
*	Function		:	rdm
*	Parameters		:	
*						UINT uiAddr - Address of the register to be read.
*						UCHAR *pucValue - Buffer where the read value will be stored.
*						INT uiSize - Size of the above buffer, also indicates the number of bytes to be read.

*	Description		:	Exported function for performing register read.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
INT rdm(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
		return lpRDMRemote(uiAddr,pucValue,uiSize);
		
	}
#endif
#endif

	if(!GpWiMAX)
		return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_rdmBulk(GpWiMAX, uiAddr, pucValue, uiSize);
}

/*
*	Function		:	rdmidle
*	Parameters		:	
*						UINT uiAddr - Address of the register to be read.
*						UCHAR *pucValue - Buffer where the read value will be stored.
*						INT uiSize - Size of the above buffer, also indicates the number of bytes to be read.

*	Description		:	Exported function for performing register read when device is in idle mode.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
INT rdmidle(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
		return lpRDMRemote(uiAddr,pucValue,uiSize);
		
	}
#endif
#endif

	if(!GpWiMAX)
		return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_rdmBulkIdle(GpWiMAX, uiAddr, pucValue, uiSize);
}

/*
*	Function		:	wrm
*	Parameters		:	
*						UINT uiAddr - Address of the register to be written.
*						UCHAR *pucValue - Pointer to the buffer, where the data to be written is supplied.
*						INT uiSize - Size of the above buffer, also indicates the number of bytes to be written.
*
*	Description		:	Exported function for performing register write.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
INT wrm(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
			
		return lpWRMRemote(uiAddr,pucValue,uiSize);
		
	}
#endif
#endif

	if(!GpWiMAX)
		return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_wrmBulk(GpWiMAX, uiAddr, pucValue, (INT)uiSize);
}

/*
*	Function		:	wrmidle
*	Parameters		:	
*						UINT uiAddr - Address of the register to be written.
*						UCHAR *pucValue - Pointer to the buffer, where the data to be written is supplied.
*						INT uiSize - Size of the above buffer, also indicates the number of bytes to be written.
*
*	Description		:	Exported function for performing register write when device is in idle mode.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
INT wrmidle(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
#ifdef WIN32
#ifdef PROXY_ENABLED_BUILD
	if(g_bOverProxy)
	{
			
		return lpWRMRemote(uiAddr,pucValue,uiSize);
		
	}
#endif
#endif

	if(!GpWiMAX)
		return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_wrmBulkIdle(GpWiMAX, uiAddr, pucValue, (INT)uiSize);
}


INT eepromrdm(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
	if(!GpWiMAX)
	return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_eepromrdmBulk(GpWiMAX, uiAddr, pucValue, uiSize);
}

INT eepromwrm(UINT uiAddr, UCHAR *pucValue, INT uiSize)
{
	if(!GpWiMAX)
	return ERROR_LIB_INITIALIZATION;

	return BeceemWiMAX_eepromwrmBulk(GpWiMAX, uiAddr, pucValue, (INT)uiSize);
}

