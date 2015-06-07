#ifdef WIN32

#include "BeceemWiMAX.h"
#include "WiMAXMonitor.h"
#include "BeceemWiMAXExport.h"
#include "Messages.h"

INT WiMAXMonitorService()
{
	SERVICE_TABLE_ENTRY   DispatchTable[] = 
	{ 
		{ "WiMaxMonitor", (LPSERVICE_MAIN_FUNCTION )BeceemWiMaxServiceStart}, 
		{ NULL, NULL } 
	}; 

	BeceemWiMaxServiceInitialization();

	if (!StartServiceCtrlDispatcher(DispatchTable)) 
	{
		SERVICE_DPRINT(DEBUG_ERROR, "Failed to start Service, Error : %d!", GetLastError());
		BeceemWiMaxServiceCleanup();
		return GetLastError();
	}
	return 0;
}

DWORD BeceemWiMaxServiceInitialization() 
{ 
	DebugPrint_Init(&SrvDebug);
	DebugPrint_Initialize(&SrvDebug, TRUE, LOG_ALL, "c:\\WiMAXMonitorService.log");

	hServHandle = INVALID_HANDLE_VALUE;

	hResetEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hHaltEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hServiceExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    return 0; 
}

DWORD BeceemWiMaxServiceCleanup() 
{ 
	if(hServHandle != INVALID_HANDLE_VALUE && hServHandle != NULL)
	{
		SERVICE_DPRINT(DEBUG_MESSAGE, "Closing the device handle...");
		local_CloseWiMAXDevice(hServHandle);
	}

	DebugPrint_Cleanup(&SrvDebug);

	CloseHandle(hResetEvent);
	CloseHandle(hHaltEvent);

    return 0; 
}

HANDLE local_OpenWiMAXDevice()
{
	return CreateFile( 
						BCM_SYMBOLIC_NAME,
						GENERIC_READ | GENERIC_WRITE ,/*| FILE_READ_ATTRIBUTES | SYNCHRONIZE, */
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
						NULL 
					);
}

void local_CloseWiMAXDevice(HANDLE hDevice)
{
	CloseHandle(hDevice);
}


VOID BeceemWiMaxServiceStart(DWORD argc, LPTSTR *argv) 
{ 
	int i = 0;
	BOOL bRet = 0;
	BeceemWiMaxServiceStatus.dwServiceType        = SERVICE_WIN32; 
	BeceemWiMaxServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
	BeceemWiMaxServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE; 
	BeceemWiMaxServiceStatus.dwWin32ExitCode      = 0; 
	BeceemWiMaxServiceStatus.dwServiceSpecificExitCode = 0; 
	BeceemWiMaxServiceStatus.dwCheckPoint         = 0; 
	BeceemWiMaxServiceStatus.dwWaitHint           = 0; 

	SERVICE_DPRINT(DEBUG_MESSAGE, "BeceemWiMaxServiceStart...");

	BeceemWiMaxServiceStatusHandle = RegisterServiceCtrlHandler( 
																"WiMaxMonitor", 
																(LPHANDLER_FUNCTION)BeceemWiMaxServiceCtrlHandler
																); 
 
	if (BeceemWiMaxServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
	{
		SERVICE_DPRINT(DEBUG_ERROR, "RegisterServiceCtrlHandler failed %d", GetLastError());
		BeceemWiMaxServiceCleanup();
		return; 
	}
	SERVICE_DPRINT(DEBUG_MESSAGE, "BeceemWiMaxServiceStart...SERVICE_RUNNING");

	BeceemWiMaxServiceStatus.dwCurrentState = SERVICE_RUNNING; 

	if (!SetServiceStatus (BeceemWiMaxServiceStatusHandle, &BeceemWiMaxServiceStatus)) 
		SERVICE_DPRINT(DEBUG_ERROR, "SetServiceStatus failed %d", GetLastError());
 

    /* This is where the service does its work. */
    /* The worker loop of a service */


    while (BeceemWiMaxServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		ResetEvent(hResetEvent);
		ResetEvent(hHaltEvent);
		ResetEvent(hServiceExitEvent);

		if(hServHandle == INVALID_HANDLE_VALUE || hServHandle == NULL)
		{
			hServHandle = local_OpenWiMAXDevice();
			SERVICE_DPRINT(DEBUG_MESSAGE, "BeceemWiMaxServiceStart...Opening device");
			
			if(hServHandle != INVALID_HANDLE_VALUE && hServHandle != NULL)
			{

				HANDLE ahEvents[] = {hResetEvent, hHaltEvent};

				/* UCHAR ucBuff[sizeof(HANDLE)*2]; */
				DWORD dwByterReturned = 0;

				DWORD dwRetWait = 0;

				SERVICE_DPRINT(DEBUG_MESSAGE, "Opening handle to WiMAX Device, SUCCESS : 0x0%x", hServHandle);
				/*
				*(HANDLE *)ucBuff = hResetEvent;
				*(HANDLE *)(ucBuff+sizeof(HANDLE)) = hHaltEvent;
				*/
				bRet = DeviceIoControl(
							hServHandle,
							IOCTL_BCM_REGISTER_EVENT,
							(LPVOID)ahEvents,
							sizeof(HANDLE)*2,
							NULL,
							0,
							&dwByterReturned,
							NULL
							);
				
				if(bRet == 0)
				{
					SERVICE_DPRINT(DEBUG_MESSAGE, "Failed to register the EVENT HANDLES with driver!");
				}
				SERVICE_DPRINT(DEBUG_MESSAGE, "Waitig for some nasty thing to happen!");
				
				SERVICE_DPRINT(DEBUG_MESSAGE, "ResetEvent : %x HaltEvent : %x \n",hResetEvent,hHaltEvent);
				
				local_CloseWiMAXDevice(hServHandle);
				hServHandle = INVALID_HANDLE_VALUE;
				
				dwRetWait = WaitForMultipleObjects(2, ahEvents, FALSE, INFINITE);
				switch(dwRetWait)
				{
				case WAIT_OBJECT_0:
					/* Reset Event Set*/
					SERVICE_DPRINT(DEBUG_MESSAGE, "Device needs reset event set...");
					ReloadDevice("Beceem");
					break;

				case WAIT_OBJECT_0+1:
					/* Halt Event Set*/
					SERVICE_DPRINT(DEBUG_MESSAGE, "DEVICE-HALT Event received...");
					break;

				default:
					SERVICE_DPRINT(DEBUG_MESSAGE, "Wait returned with unknown return value! (%d)",dwRetWait);
					break;
				}
			}
		}
#ifndef WIN32
		usleep(1000000);
#else
		Sleep(1000);
#endif
	}

	SERVICE_DPRINT(DEBUG_MESSAGE, "End ServiceMain");
	BeceemWiMaxServiceCleanup();
    return; 
} 

VOID BeceemWiMaxServiceCtrlHandler(DWORD Opcode) 
{ 
    switch(Opcode) 
    { 
        case SERVICE_CONTROL_PAUSE: 
        /*  Do whatever it takes to pause here.  */
            BeceemWiMaxServiceStatus.dwCurrentState = SERVICE_PAUSED; 
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
        /*  Do whatever it takes to continue here.  */
            BeceemWiMaxServiceStatus.dwCurrentState = SERVICE_RUNNING; 
            break; 
 
        case SERVICE_CONTROL_STOP: 
        /*  Do whatever it takes to stop here.  */
			SetEvent(hServiceExitEvent);

            BeceemWiMaxServiceStatus.dwWin32ExitCode = 0; 
            BeceemWiMaxServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
            BeceemWiMaxServiceStatus.dwCheckPoint    = 0; 
            BeceemWiMaxServiceStatus.dwWaitHint      = 0; 
 
            if (!SetServiceStatus (BeceemWiMaxServiceStatusHandle, &BeceemWiMaxServiceStatus))
				SERVICE_DPRINT(DEBUG_ERROR, "SetServiceStatus failed %d", GetLastError());

			SERVICE_DPRINT(DEBUG_MESSAGE, "Leaving BeceemWiMaxService");
			BeceemWiMaxServiceCleanup();

            return; 
 
        case SERVICE_CONTROL_INTERROGATE: 
        /*  Fall through to send current status.  */
            break; 
 
        default: 
			SERVICE_DPRINT(DEBUG_WARNING, "Unrecognized opcode %ld", Opcode);
    } 
 
    /*  Send current status.  */
    if (!SetServiceStatus (BeceemWiMaxServiceStatusHandle,  &BeceemWiMaxServiceStatus)) 
		SERVICE_DPRINT(DEBUG_WARNING, "SetServiceStatus error %ld",GetLastError());

    return; 
}

BOOL SERVICE_DPRINT(UCHAR iCode, const char *szFmt, ...)
{
	BOOL bRet = TRUE;
	
#ifdef WIMAX_DEBUG

	CHAR caBuffer[2048]={0};
	va_list argp;
	va_start(argp, szFmt);

	
	vsprintf(caBuffer, szFmt, argp);
	strcat(caBuffer, "\r\n");
	bRet = DebugPrint_Print(&SrvDebug, iCode, caBuffer);

	va_end(argp);
#endif
	return bRet;
}

/* Application callback function */
void ServiceCallback(PVOID context,PVOID pBCMMessage,ULONG ulBCMMessageLength)
{
	PWIMAX_MESSAGE pWiMaxMsg = (PWIMAX_MESSAGE)pBCMMessage;
    if(!pBCMMessage)
    {
        return;
    }
    switch(pWiMaxMsg->usType)
    {
		case DEVICE_RESET_UNLOAD_TYPE:
			SERVICE_DPRINT(DEBUG_MESSAGE, "DEVICE_RESET_UNLOAD_TYPE received");
			hServHandle = INVALID_HANDLE_VALUE;
			break;

       default:
			break;
    }
}

#endif