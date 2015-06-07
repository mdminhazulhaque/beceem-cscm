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
 * Description	:	This includes CBeceemWiMAX class, which implements
 *					device and various message related functionality.
 * Author		:	Prashanth Gedde N
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h"
#include "OMADMTreeStruct.h"
#include "ProxyEndianHandler.h"
#include "vendorspecificextension.h"
#include "globalobjects.h"
extern UINT32 g_bNVMTypeIsFLASH;


#define     CFG_EEPROM_CMDQ_SPI_REG                               (0x0f003018)

#define     CFG_EEPROM_READDATA_SPI_REG                           (0x0f003020)

#define     CFG_EEPROM_WRITEDATA_SPI_REG                          (0x0f00301c)


#define SWAP_SHORT(DATA) ((DATA>>8)&0xff)+((DATA << 8)&0xff00)

 
/*#ifdef      MII

#define     CFG_EEPROM_START                                      4096

#else

#define     CFG_EERPOM_START                                      512

#endif*/


DEBUGPRINT *GTraceDebug;


const int SIZE_stCPStatistics = sizeof(stCPStatistics);
const int SIZE_stMPStatistics = sizeof(stMPStatistics);
const int SIZE_stDPStatistics= sizeof(stDPStatistics);
const int SIZE_stLinkStats = sizeof(stLinkStats);
const int SIZE_stLinkStatEx = sizeof(stLinkStatEx);
const int SIZE_stPhyStateInfo = sizeof(stPhyStateInfo);
const int SIZE_stHoEventInfo = sizeof(stHoEventInfo);
const int SIZE_stStatistics_SS = sizeof(stStatistics_SS);

const int SIZE_stMACMgmtMessage = sizeof(S_MAC_MGMTMESSAGE);

const int SIZE_WIMAX_MESSAGE = sizeof(WIMAX_MESSAGE);
const int SIZE_BSINFO = sizeof(BSINFO);
const int SIZE_BSLIST = sizeof(BSLIST);
const int SIZE_BSINFOEX = sizeof(BSINFOEX);
const int SIZE_LEADER = sizeof(LEADER);
const int SIZE_PACKETTOSEND = sizeof(PACKETTOSEND);
const int SIZE_PACKETTOSENDEXT = sizeof(PACKETTOSENDEXT);
const int SIZE_MAC_MGMT_MESSAGE = sizeof(MAC_MGMT_MESSAGE);
const int SIZE_FIRMWARE_INFO = sizeof(FIRMWARE_INFO);
const int SIZE_FIRMWARE_STATUS_INDICATION = sizeof(FIRMWARE_STATUS_INDICATION);

const int SIZE_CCPacketClassificationRuleSI = sizeof(CCPacketClassificationRuleSI);
const int SIZE_CPhsRuleSI = sizeof(CPhsRuleSI);
const int SIZE_CConvergenceSLTypes = sizeof(CConvergenceSLTypes);
const int SIZE_CServiceFlowParamSI = sizeof(CServiceFlowParamSI);
const int SIZE_LocalSFAddRequest = sizeof(stLocalSFAddRequest);
const int SIZE_SYSTEM_CONFIGURATION = sizeof(SYSTEM_CONFIGURATION);
const int SIZE_SUBSCRIBER_STATION_INFO = sizeof(SUBSCRIBER_STATION_INFO);
const int SIZE_LINK_STATUS = sizeof(LINK_STATUS);
const int SIZE_GPIO_INFO = sizeof(GPIO_INFO);
const int SIZE_GPIO_MULTI_INFO = sizeof(GPIO_MULTI_INFO);
const int SIZE_GPIO_MULTI_MODE = sizeof(GPIO_MULTI_MODE);

void my_fini(void);

VOID BeceemWiMAX_Signal_Handler(INT signum)
{
#if 0
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Test Tx Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, "Test Rx Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_NVM, "Test NVM Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RDM, "Test RDM Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_WRM, "Test WRM Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Test OTHER Path: %d", signum);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_ALL, "Test ALL Path: %d", signum);

    DPRINT_COMMONAPI(DEBUG_MESSAGE, "DPRINT_COMMONAPI: Test CAPI Path: %d", signum);
#endif

    DPRINT_OTHER("\n*App Aborted by signal [%d]. Cleaning up...*\n", signum);
    CloseWiMAXDevice();
    exit(1);

}

#ifndef WIN32
BOOL SignalHandling_Init(VOID)
{
    INT i;
    struct sigaction act;

#ifdef LINUX
    struct rlimit rlim;

    /* Setup the core dump resource limit to ensure that the core file is indeed dumped.
     * Equivalent to doing 'ulimit -c <size>' on the cmd-line.. */
    rlim.rlim_cur = CORE_FILE_SOFTLIMIT;
    rlim.rlim_max = CORE_FILE_HARDLIMIT;	// ceiling for soft limit
    if (setrlimit(RLIMIT_CORE, &rlim) < 0)
	{
	perror("setrlimit failed");
	DPRINT_OTHER("WARNING! setrlimit failed.\n");
    }
    //system ("ulimit -a|grep core");               // verify (dev)

#endif

    /* Setup signal handler */
    //DPRINT_OTHER ("Setting up signal handlers for SIGINT and SIGQUIT...\n");
    memset(&act, 0, sizeof(act));
    act.sa_handler = BeceemWiMAX_Signal_Handler;
    act.sa_flags = SA_RESTART | SA_NOCLDWAIT;
    sigemptyset(&act.sa_mask);


    for (i = 1; i <= SIGNALS_MAX; i++)
	{
	
		if(i == SIGINT || i == SIGQUIT || i == SIGTSTP)
		{
		    if (sigaction(i, &act, NULL) == -1)
			{
			    //perror ("sigaction");
			    DPRINT_OTHER("signal %d, sigaction failed.\n", i);
			}
		}	
	}
    return TRUE;
}
#elif WIN32
BOOL SignalHandling_Init(VOID)
{
    signal(SIGINT, BeceemWiMAX_Signal_Handler);
    signal(SIGABRT, BeceemWiMAX_Signal_Handler);
    signal(SIGTERM, BeceemWiMAX_Signal_Handler);
    DP;
    return TRUE;
}
#endif

/**
@ingroup Utility_Functions
*	Function		:	This will initialize all members declared in the BECEEMWIMAX structure
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS if initialization is successful.
	@retval	ERROR_INVALID_ARGUMENT if pArg is invalid.
	@retval ERROR_MALLOC_FAILED if malloc fails
*/
int BeceemWiMAX_InitializeMembers(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
    DP;
#if 1
	BECEEMCALLOC(GTraceDebug,DEBUGPRINT *,DEBUGPRINT,1)
#endif

	DeviceIO_Init(&(pWiMAX->m_Device));

	/* Callback thread */
	Thread_Init(&(pWiMAX->m_CallbackThread), pArg);
	Event_Init(&(pWiMAX-> m_objReadFileEvent));
    DP;
	DebugPrint_Init(GTraceDebug);


	Mutex_Init(&(pWiMAX->m_CloseDeviceRemovalEventMutex));
	Mutex_Create(&(pWiMAX->m_CloseDeviceRemovalEventMutex), NULL);
    DP;
	Mutex_Init(&(pWiMAX->m_objMutex));
	Mutex_Create(&(pWiMAX->m_objMutex), BECEEM_STATS_MUTEX_NAME);

	Mutex_Init(&(pWiMAX->m_ShMemMutex));
	Mutex_Create(&(pWiMAX->m_ShMemMutex), BECEEM_SHMEM_MUTEX_NAME);
	 
	Mutex_Init(&(pWiMAX->m_EEPROM_AcessMutex));
	Mutex_Create(&(pWiMAX->m_EEPROM_AcessMutex), BECEEM_MUTEX_EEPROM_ACCEESS);
    DP;
	Mutex_Init(&(pWiMAX->m_objCallbackMutex));
	Mutex_Create(&(pWiMAX->m_objCallbackMutex), NULL);

	Mutex_Init(&(pWiMAX->m_FirmwareDownloadMutex));
	Mutex_Create(&(pWiMAX->m_FirmwareDownloadMutex), BECEEM_MUTEX_FIRMWARE_DOWNLOAD);

	Mutex_Init(&(pWiMAX->m_MACMgmtMsg));
	Mutex_Create(&(pWiMAX->m_MACMgmtMsg), BECEEM_MUTEX_MAC_MGMT);

	/*For Firmware download*/
	Thread_Init(&(pWiMAX->m_FirmwareThread), pArg);

	/* Statistics Request */
	Thread_Init(&(pWiMAX->m_StatsThread), pArg);

	/* Host Statistics Request */
	Thread_Init(&(pWiMAX->m_LoggerThread), pArg);

	WiMAXLogger_Init(&(pWiMAX->m_IRLogger));
	WiMAXLogger_Init(&(pWiMAX->m_BRLogger));
	WiMAXLogger_Init(&(pWiMAX->m_PRLogger));
	WiMAXLogger_Init(&(pWiMAX->m_BWLLogger));
	WiMAXLogger_Init(&(pWiMAX->m_CLLogger));
	WiMAXLogger_Init(&(pWiMAX->m_OLLogger));
	WiMAXLogger_Init(&(pWiMAX->m_CIDLogger));
	WiMAXLogger_Init(&(pWiMAX->m_CQICHLogger));

	/* for net entry */
	Event_Init(&(pWiMAX->m_objNetEntryEvent));
	Event_Init(&(pWiMAX->m_objNetEntryEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_NetEntryThread), pWiMAX);

	/* for sync up */
	Event_Init(&(pWiMAX->m_objSyncUpEvent));
	Event_Init(&(pWiMAX->m_objSyncUpEventMsgRecvd));

	Thread_Init(&(pWiMAX->m_SyncUpThread), pWiMAX);

	/* for net search */
	Event_Init(&(pWiMAX->m_objNetSearchEvent));
	Event_Init(&(pWiMAX->m_objNetSearchEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_NetSearchThread), pWiMAX);
	
	Thread_Init(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor), pWiMAX);

	Thread_Init(&(pWiMAX->m_WiMAXCommonAPI_NwSearch),pWiMAX);

	/* for MAC address Write */
	Event_Init(&(pWiMAX->m_objMACaddressRespEvent));
	Thread_Init(&(pWiMAX->m_MACAddressWriteRespThread), pWiMAX);

	/* for WiMAX Options */
	Event_Init(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
	Event_Init(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_WIMAXOptionsRespThread), pWiMAX);

	/* for Shutdown Req */
	Event_Init(&(pWiMAX->m_objShutDownRespEvent));
	Event_Init(&(pWiMAX->m_objShutDownRespEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_ShutDownRespThread), pWiMAX);

	/* Antenna config req */
	Event_Init(&(pWiMAX->m_objAntennaConfigReqEvent));
	Event_Init(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));

	Thread_Init(&(pWiMAX->m_AntennaConfigRespThread), pWiMAX);
	

	/* Antenna Scanning req */
	Event_Init(&(pWiMAX->m_objAntennaScanningReqEvent));
	Event_Init(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_AntennaScanningRespThread), pWiMAX);
	

	/* Antenna Selection req */
	Event_Init(&(pWiMAX->m_objAntennaSelectionReqEvent));
	Event_Init(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd));
	Thread_Init(&(pWiMAX->m_AntennaSelectionRespThread), pWiMAX);
		

	
	/*Device Init Thread*/
	Thread_Init(&(pWiMAX->m_DeviceInitThread), pWiMAX);

	/*Idle mode wake up thread */
	Thread_Init(&(pWiMAX->m_IdleModeWakeupThread), pWiMAX);

#ifdef WIN32
    InterlockedExchange(&pWiMAX->m_iDeviceRemovalIssued,0);
	/*For Device Unload */
	Event_Init(&(pWiMAX->m_DriverUnloadEvent));
	Thread_Init(&(pWiMAX->m_DriverUnloadEventThread), pWiMAX);
#endif
	Event_Init(&(pWiMAX->m_CAPIOMAThreadEvent));
	Thread_Init(&(pWiMAX->m_CAPIOMAThread), pWiMAX);

    Mutex_Init(&(pWiMAX->m_DeviceRemovalIssuedMutex));
    Mutex_Create(&(pWiMAX->m_DeviceRemovalIssuedMutex), NULL);

	Thread_Init(&(pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread), pWiMAX);

	BeceemWiMAX_InitializeMIBTable(pWiMAX);
	pWiMAX->m_bStdConfigInit = 0;

	pWiMAX->m_bEnableMACLogging = FALSE;


	return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This will cleanup all members declared in the BECEEMWIMAX structure.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS if initialization is successful.
	@retval	ERROR_INVALID_ARGUMENT if pArg is invalid.
*/
int BeceemWiMAX_CleanupMembers(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	
	pWiMAX->m_bIdleMode = FALSE;
	pWiMAX->m_bModemInShutdown = FALSE;

	DeviceIO_Cleanup(&(pWiMAX->m_Device));

	/* Callback thread */
	Thread_Cleanup(&(pWiMAX->m_CallbackThread));
	Event_Cleanup(&(pWiMAX-> m_objReadFileEvent));

	DebugPrint_Cleanup(GTraceDebug);
	
	Mutex_Cleanup(&(pWiMAX->m_objCallbackMutex));
	Mutex_Cleanup(&(pWiMAX->m_CloseDeviceRemovalEventMutex));

	Mutex_Cleanup(&(pWiMAX->m_objMutex));
	Mutex_Cleanup(&(pWiMAX->m_ShMemMutex));
	Mutex_Cleanup(&(pWiMAX->m_MACMgmtMsg));

	Mutex_Cleanup(&(pWiMAX->m_EEPROM_AcessMutex));

	Mutex_Cleanup(&(pWiMAX->m_FirmwareDownloadMutex));
	
	Thread_Cleanup(&(pWiMAX->m_FirmwareThread));

	Thread_Cleanup(&(pWiMAX->m_StatsThread));

	Thread_Cleanup(&(pWiMAX->m_LoggerThread));

	WiMAXLogger_Cleanup(&(pWiMAX->m_IRLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_BRLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_PRLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_BWLLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_CLLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_OLLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_CIDLogger));
	WiMAXLogger_Cleanup(&(pWiMAX->m_CQICHLogger));

	Event_Cleanup(&(pWiMAX->m_objNetEntryEvent));
	Event_Cleanup(&(pWiMAX->m_objNetEntryEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_NetEntryThread));

	Event_Cleanup(&(pWiMAX->m_objSyncUpEvent));
	Event_Cleanup(&(pWiMAX->m_objSyncUpEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_SyncUpThread));

	Event_Cleanup(&(pWiMAX->m_objNetSearchEvent));
	Event_Cleanup(&(pWiMAX->m_objNetSearchEventMsgRecvd));

	Thread_Cleanup(&(pWiMAX->m_NetSearchThread));
	Thread_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_DeviceInsertMonitor));
	Thread_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_NwSearch));
	Event_Cleanup(&(pWiMAX->m_objMACaddressRespEvent));
	Event_Cleanup(&(pWiMAX->m_objMACaddressRespEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_MACAddressWriteRespThread));

	Thread_Cleanup(&(pWiMAX->m_IdleModeWakeupThread));
	
	Event_Cleanup(&(pWiMAX->m_objShutDownRespEvent));
	Event_Cleanup(&(pWiMAX->m_objShutDownRespEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_ShutDownRespThread));

	Event_Cleanup(&(pWiMAX->m_objSetWIMAXOptionsRespEvent));
	Event_Cleanup(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_WIMAXOptionsRespThread));
	

	/* Antenna config req */
	Event_Cleanup(&(pWiMAX->m_objAntennaConfigReqEvent));
	Event_Cleanup(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_AntennaConfigRespThread));
	
	/* Antenna Scanning req */
	Event_Cleanup(&(pWiMAX->m_objAntennaScanningReqEvent));
	Event_Cleanup(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_AntennaScanningRespThread));

	/* Antenna Selection req */
	Event_Cleanup(&(pWiMAX->m_objAntennaSelectionReqEvent));
	Event_Cleanup(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd));
	Thread_Cleanup(&(pWiMAX->m_AntennaSelectionRespThread));

	/* Device Init Thread */
	Thread_Cleanup(&(pWiMAX->m_DeviceInitThread));

#ifdef WIN32
	/* Device Unload Event Thread */
	Event_Cleanup(&(pWiMAX->m_DriverUnloadEvent));
	Thread_Cleanup(&(pWiMAX->m_DriverUnloadEventThread));
#endif
	Event_Cleanup(&(pWiMAX->m_CAPIOMAThreadEvent));
	Thread_Cleanup(&(pWiMAX->m_CAPIOMAThread));

    Mutex_Cleanup(&(pWiMAX->m_DeviceRemovalIssuedMutex));

	Thread_Cleanup(&(pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread));
	/* Idle Mode Wakeup Thread */
	Thread_Cleanup(&(pWiMAX->m_IdleModeWakeupThread));
	
	if(GTraceDebug)
	{
		BECEEM_MEMFREE(GTraceDebug);
		GTraceDebug = NULL;
	}
	return SUCCESS;
}



/**
@ingroup Utility_Functions
*	Function		:	This will perform initialisation task for BECEEMWIMAX instance.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS if initialization is successful.
	@retval	ERROR_INVALID_ARGUMENT if pArg is invalid.
*/
int BeceemWiMAX_Init(PVOID pArg)
{
	INT32 iRet = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    DP;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    if (!SignalHandling_Init())
	return ERROR_SIGNAL_OPERATION;
    DP;
    BeceemWiMAX_ResetFlags(pWiMAX);
    DP;
    iRet = BeceemWiMAX_InitializeMembers(pWiMAX);
    DP;
    if (iRet)
	return iRet;
	

    DebugPrint_Initialize(GTraceDebug);
    DPRINT_OTHER("Constructing object..\n");

	

	/* For callback thread */
	Event_Create(&(pWiMAX->m_objReadFileEvent),TRUE,TRUE,
#ifdef MACOS
	"Beceem1"
#else
	NULL
#endif
);


	
	Event_Create(&(pWiMAX->m_objNetEntryEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem2"
#else
	NULL
#endif
);
		Event_Create(&(pWiMAX->m_objNetEntryEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem21"
#else
	NULL
#endif
);
	
	Event_Create(&(pWiMAX->m_objSyncUpEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem3"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objSyncUpEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem31"
#else
	NULL
#endif
);
	
	Event_Create(&(pWiMAX->m_objNetSearchEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem4"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objNetSearchEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem41"
#else
	NULL
#endif
);
	
	Event_Create(&(pWiMAX->m_objMACaddressRespEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem40"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objMACaddressRespEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem401"
#else
	NULL
#endif
);

	
	Event_Create(&(pWiMAX->m_objShutDownRespEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem5"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objShutDownRespEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem51"
#else
	NULL
#endif
);
	
	Event_Create(&(pWiMAX->m_objSetWIMAXOptionsRespEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem50"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem501"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objAntennaConfigReqEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem7"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem71"
#else
	NULL
#endif
);
	
	Event_Create(&(pWiMAX->m_objAntennaScanningReqEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem8"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem81"
#else
	NULL
#endif
);

	Event_Create(&(pWiMAX->m_objAntennaSelectionReqEvent),TRUE,FALSE,
#ifdef MACOS
	"Beceem9"
#else
	NULL
#endif
);
	Event_Create(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd),TRUE,TRUE,
#ifdef MACOS
	"Beceem91"
#else
	NULL
#endif
);
	
    DP;
	CheckMachineEndianness();	
    DP;

#ifndef DISABLE_DYN_LOGGING
    if (!DynLogging_InitShmRecord())
    {
		QP("DynLogging Initialization failed");
    }
#else
	pWiMAX->stLogging.bInited = FALSE;
#endif
    QP("SUCCESSFUL*****");
	return SUCCESS;
}
int BeceemWiMAX_ResetFlags(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

		/*Initialize MIB constants*/
	pWiMAX->m_iStdStatsIndex = 0;
	pWiMAX->m_iPropCfgIndex = 0;
	pWiMAX->m_iPropStatsIndex = 0;
	pWiMAX->m_iStdCfgIndex = 0;

	pWiMAX->m_pAppCallback = NULL;
	pWiMAX->m_pAppCallbackContext = NULL;

	/* For callback thread */
	pWiMAX->m_bKillCallbackThread = 0;

	pWiMAX->m_bKillAntennaSelectionRespThread = 0;

	pWiMAX->m_bKillDriverUnloadEvent = 0;
	pWiMAX->m_bKillDeviceRemovalThread = 0;
	
	pWiMAX->m_bIdleMode = FALSE;
	pWiMAX->m_bStatsPointerFlag = FALSE;
	pWiMAX->m_uiStatAddress = 0;
	pWiMAX->m_uiStructLength = 0;

	pWiMAX->m_bKillAntennaScanningRespThread = 0;
	pWiMAX->m_bAntennaSelectionInProgress = FALSE;

	pWiMAX->m_bKillAntennaConfigRespThread = 0;
	pWiMAX->m_bAntennaScanningInProgress = FALSE;

	pWiMAX->m_bAntennaConfigInProgress = FALSE;
	pWiMAX->m_bKillWIMAXOptionsSetInProgress = 0;

	pWiMAX->m_bKillShutDownRespThread = 0;
	pWiMAX->m_bWIMAXOptionsSetInProgress = FALSE;

	pWiMAX->m_bKillSyncUpThread = 0;
	pWiMAX->m_bNetSearchInProgress = FALSE;
    pWiMAX->m_bNSPSearchInProgress = FALSE;

	pWiMAX->m_bKillMACAddressRespThread = 0;
	pWiMAX->m_bModemInShutdown = FALSE;

	pWiMAX->m_bKillNetSearchThread = 0;
	pWiMAX->m_bMACAddressWriteInProgress = FALSE;

	pWiMAX->m_bKillNetEntryThread = 0;
	pWiMAX->m_bSyncUpInProgress = FALSE;

	pWiMAX->m_bFirmwareDownloadDone = TRUE;

	/*Stats Request */
	pWiMAX->m_uiStatsPeriod = 0;
	pWiMAX->m_bStatThreadRunning = FALSE;
	pWiMAX->m_bMACMgmtMsgsEnabled = 0;
	pWiMAX->m_bInitEEPROM = FALSE;
	pWiMAX->m_iIRPrevLogIndex = -1;
	pWiMAX->m_iBRPrevLogIndex = -1;
	pWiMAX->m_iPRPrevLogIndex = -1;
	pWiMAX->m_iBWLPrevLogIndex = -1;
	pWiMAX->m_iCLPrevLogIndex = -1;
	pWiMAX->m_iOLPrevLogIndex = -1;
	pWiMAX->m_iCIDPrevLogIndex = -1;
	pWiMAX->m_iCQICHPrevLogIndex = -1;

	pWiMAX->m_iLoggerCount = 0;
	pWiMAX->m_bKillLoggerThread = 0;

	pWiMAX->m_bNetEntryInProgress = FALSE;

	/*Control msg enable mask*/
	pWiMAX->m_uiControlMsgEnableMask = CONTROLMSGS_DEFAULT_MASK;

	return SUCCESS;

}

/*
*	Function		:	BeceemWiMAX_Cleanup
*	Parameters		:
*						PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*
*	Description		:	This will perform cleanup task for BECEEMWIMAX instance.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
/**
@ingroup Utility_Functions
*	Function		:	This will perform cleanup task for BECEEMWIMAX instance.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS if initialization is successful.
	@retval	ERROR_INVALID_ARGUMENT if pArg is invalid.
*/
int BeceemWiMAX_Cleanup(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    if (!DynLogging_CleanShmRecord())
	DPRINT_OTHER("Error encountered when cleaning dynamic logging shmem records.\n");
	BeceemWiMAX_CloseDevice(pWiMAX);
    DPRINT_OTHER("Destructing object.\n");

	BeceemWiMAX_CleanupMembers(pWiMAX);
	return SUCCESS;
}
/*
*	Function		:	BeceemWiMAX_OpenEEPROM
*	Parameters		:
*						PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*
*	Description		:	This function will initalize EEPROM for read/writes.
*	Return value	:	A TRUE on success, FALSE on failure.
*/
/**
@ingroup Utility_Functions
*	Function		:	This function will initalize EEPROM for read/writes.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	A TRUE on success, FALSE on failure.
*/
BOOL BeceemWiMAX_OpenEEPROM(PVOID pArg)
{

	int iRet = 0;	

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return FALSE;

	
	iRet = CFG_Init(&pWiMAX->m_Device, &pWiMAX->m_ShMemMutex, 
			(unsigned char*)pWiMAX->pEEPROMContextTable, pWiMAX->u32EEPROMContextTableSize);
	if (iRet <= 0)
		return FALSE;
	else
	{
		pWiMAX->m_bInitEEPROM = TRUE; 
		return pWiMAX->m_bInitEEPROM;
	}
}
void CheckIfNVMIsFlash(PBECEEMWIMAX pWiMAX)
{
	UINT32 au32SignatureContents[2] = {0};
	
	if(BeceemWiMAX_rdm(pWiMAX, NVM_IDENTIFY_LOCATION,au32SignatureContents, sizeof(UINT32)*2))
		{
	     DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "RDM failed CheckIfNVMIsFlash failed at location %x",
		   NVM_IDENTIFY_LOCATION);
		}
	

	if((au32SignatureContents[0] == NVM_TYPE_FLASH_SIGNATURE)&& 
		(au32SignatureContents[1] == NVM_TYPE_FLASH_SIGNATURE))
		pWiMAX->u32NVMIsFlashFlag = TRUE;
	else
		pWiMAX->u32NVMIsFlashFlag = FALSE;

	g_bNVMTypeIsFLASH = pWiMAX->u32NVMIsFlashFlag;
}

VOID BeceemWiMAX_FindInterfaceType(PVOID pArg,UINT32 u32ChipID,UINT32 u32SysConfigReg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, " ***Chip ID: x%x, SysConfig: x%x ***\n",
	   u32ChipID, u32SysConfigReg);


	switch(u32ChipID)
		{
		case MS120:
		case MS121:
			switch((u32SysConfigReg &0x8) >> 3)
				{
				case 0:
					pWiMAX->eBCM_INTERFACETYPE = BCM_PCMCIA;
					break;
				case 1:
					pWiMAX->eBCM_INTERFACETYPE = BCM_MII;
					break;
				}
			break;
		case MS130:
		case MS200:
		case MS210:
		case MS300: // T3 Chip Set
		case MS310:
			switch((u32SysConfigReg&0x1C)>>2)
				{
				case 0: 
					pWiMAX->eBCM_INTERFACETYPE = BCM_PCMCIA;
					break;
				case 2: 
					pWiMAX->eBCM_INTERFACETYPE = BCM_CARDBUS;
					break;
				case 4:
					pWiMAX->eBCM_INTERFACETYPE = BCM_SDIO;
					break;
				case 5: 
					pWiMAX->eBCM_INTERFACETYPE = BCM_USB;
					break;
				case 7:
					pWiMAX->eBCM_INTERFACETYPE = BCM_MII;
					break;
				}
			break;
		 case MS3200: // T3 LP
		 	switch((u32SysConfigReg&0x1C)>>2)
		 	{
		 	case 0:
				pWiMAX->eBCM_INTERFACETYPE = BCM_PCMCIA;
				break;
			case 2: 
			case 4: 
				pWiMAX->eBCM_INTERFACETYPE = BCM_SDIO;
				break;
			case 5: 
				pWiMAX->eBCM_INTERFACETYPE = BCM_USB;
					break;
			case 7:
					pWiMAX->eBCM_INTERFACETYPE = BCM_MII;
					break;
		 	}
			break;
		case MS3300: // T3 LP-B(UMA-B)
		case MS3301: // ADI T3 LP-BD
                case MS3311: // ADI T3 LP-BD
                case MS3321: // ADI T3 LP-BD
			switch((u32SysConfigReg&0x1C)>>2)
		 	{
		 	case 0:
			case 1:
				pWiMAX->eBCM_INTERFACETYPE = BCM_USB;
				break;
			case 2: 
			case 3: 
				pWiMAX->eBCM_INTERFACETYPE = BCM_SDIO;
				break;
			case 4:
			case 5: 
				pWiMAX->eBCM_INTERFACETYPE = BCM_MII;
					break;
			}
			break;
		}


}

/**
@ingroup Utility_Functions
*	Function		:	This will open the device handle.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param const char* szDeviceName -  Pointer to null terminated string which specifies the device name.
*	@retval	A valid device handle on success, error code on failure.
*/
HANDLE BeceemWiMAX_CreateDevice(PVOID pArg, const char* szDeviceName)
{
	int iRet = 0;
#ifdef WIN32	
    UINT32 u32Val = 0;
#endif    
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return INVALID_HANDLE_VALUE;
	
	if(DeviceIO_GetDeviceHandle(&(pWiMAX->m_Device))!= INVALID_HANDLE_VALUE)
		return DeviceIO_GetDeviceHandle(&(pWiMAX->m_Device));

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Trying to open device");
#ifdef WIN32
	
    u32Val = InterlockedCompareExchange(&pWiMAX->m_iDeviceRemovalIssued,0,1);

    if(u32Val != 0)
    {
		if (Thread_IsValidThread(&pWiMAX->m_DriverUnloadEventThread))
			Thread_Wait(&(pWiMAX->m_DriverUnloadEventThread),INFINITE,0);
		pWiMAX->m_iDeviceRemovalIssued = FALSE;
    }

#endif	

	if(!szDeviceName)
		return INVALID_HANDLE_VALUE;

	if(DeviceIO_OpenHandle(&(pWiMAX->m_Device),szDeviceName))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Invalid handle. Error while opening device");
		return INVALID_HANDLE_VALUE;
	}

    Mutex_Lock(&pWiMAX->m_objCallbackMutex);
    pWiMAX->m_pAppCallback = NULL;
	pWiMAX->m_pAppCallbackContext = NULL;
	Mutex_Unlock(&pWiMAX->m_objCallbackMutex);

	/* Kill callbackthread if running */
	if(Thread_Stop(&(pWiMAX->m_CallbackThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillCallbackThread, pWiMAX) != SUCCESS)
		Thread_Terminate(&(pWiMAX->m_CallbackThread));

	BeceemWiMAX_InitIRP(pWiMAX);
	BeceemWiMAX_ResetFlags(pWiMAX);

	/* Intialize the device removal flag */
	pWiMAX->m_iDeviceRemovalIssued = 0;
   
	/* Initialize the Callback thread flag */
	pWiMAX->m_bKillCallbackThread = 0;

	/* Register Data Notification routine */
	DeviceIO_RegisterNotifyCallback(&(pWiMAX->m_Device), BeceemWiMAX_NotificationRoutine, pWiMAX);

	/*
	 Launch a thread function to send packets to the application
	*/
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Launching Callback thread.");
	if(!Thread_IsAlive(&pWiMAX->m_CallbackThread))
	{
		if(Thread_Run(&(pWiMAX->m_CallbackThread),(PTHREADFUNC)BeceemWiMAX_CallbackThreadFunc, pWiMAX))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Launching Callback thread failed !!!");
			return INVALID_HANDLE_VALUE;
		}
	}
	pWiMAX->m_bUnloadNotified = FALSE;

	{
		/* Check Idle mode status and update the variable */
		LINK_STATE stLinkState = {0};
		BeceemWiMAX_GetLinkState(pArg, &stLinkState);

		pWiMAX->m_bIdleMode = FALSE;	
		pWiMAX->m_bModemInShutdown = FALSE;	
		if(stLinkState.bIdleMode == TRUE)
		{
			pWiMAX->m_bDeviceInitInProgress = TRUE;
			pWiMAX->m_bIdleMode = TRUE;	
		}
		
		if(stLinkState.bShutdownMode == TRUE)
		{
			pWiMAX->m_bDeviceInitInProgress = TRUE;
			pWiMAX->m_bModemInShutdown = TRUE;	
		}
	}

	pWiMAX->pEEPROMContextTable = (PUCHAR)calloc(1,((sizeof(CFG_CONTEXTTABLE) + sizeof(CFG_LEGACYCONTEXTTABLE))));

	if(!pWiMAX->pEEPROMContextTable)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Memory Allocation failed");
		return INVALID_HANDLE_VALUE;
	}
	
	pWiMAX->u32EEPROMContextTableSize = (sizeof(CFG_CONTEXTTABLE) + sizeof(CFG_LEGACYCONTEXTTABLE));
	memset(pWiMAX->pEEPROMContextTable, 0, pWiMAX->u32EEPROMContextTableSize);
	pWiMAX->m_bInitEEPROM = FALSE;
	
	iRet = SharedMem_Open(&(pWiMAX->m_shmMACMgmtMsg), sizeof(int), BECEEM_SHARED_MAC_MGMT);

	if(iRet)
	{
		int iValue = 0;			
		SharedMem_Create(&(pWiMAX->m_shmMACMgmtMsg), sizeof(int), BECEEM_SHARED_MAC_MGMT);
		SharedMem_WriteData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iValue, sizeof(int), 0);
	}
	
	if(pWiMAX->m_bMACMgmtMsgsEnabled == 1)
	{
		ToggleMACMgmtMsg(pWiMAX, TRUE);
	}

	pWiMAX->m_bDeviceInitInProgress = TRUE;
	pWiMAX->m_bKillDeviceInitThread = 0;
	
	BeceemWiMax_GetDriverInfoSettings(pWiMAX);

#ifdef LAUNCH_CFG_THREAD
	if(Thread_Run(&(pWiMAX->m_DeviceInitThread),(PTHREADFUNC)BeceemWiMAX_DeviceInitThreadFunc,pWiMAX))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Launching Device Init THREAD failed !!!");
		return INVALID_HANDLE_VALUE;
	}
#else
	if(BeceemWiMAX_DeviceInitThreadFunc(pWiMAX))
		return INVALID_HANDLE_VALUE;
#endif
	
#ifdef WIN32
	if(BeceemWiMAX_DeviceUnloadShareEvent(pWiMAX))
		{
			return INVALID_HANDLE_VALUE;
		}
#endif

	iRet = BeceemWiMAX_VendorSpecificInitFunc(pWiMAX);

	if(iRet)
		{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Vendor Specific Initialization failed ");
			return INVALID_HANDLE_VALUE;
		}


	pWiMAX->pStatsCacheData=NULL;
	pWiMAX->pucMibOutBuffer=NULL;
    pWiMAX->pstHostStatsCache=NULL;
	
	return DeviceIO_GetDeviceHandle(&(pWiMAX->m_Device));
}


/**
@ingroup Utility_Functions
*	Function		:	This will check whether the device handle is valid or not.
*	@param PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	TRUE if valid, FALSE otherwise.
*/
BOOL BeceemWiMAX_IsValidHandle(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return FALSE;

	return DeviceIO_IsValidHandle(&(pWiMAX->m_Device)); 
}


/*		vsprintf(pPrintBuffer+strlen(pPrintBuffer), szFmt, argp);			
	va_list argp;			\
	va_start(argp, szFmt);			\
		va_end(argp);			\
*/


int ToggleMACMgmtMsg(PVOID pArg, BOOL bToggleMsg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	int iMACMgmtStatus = 0;
	UINT uiMACRegValue;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	Mutex_Lock(&pWiMAX->m_MACMgmtMsg);	
	SharedMem_ReadData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iMACMgmtStatus, sizeof(int), 0);


    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Number clients receiving MAC Msgs : %d", iMACMgmtStatus);

	if(BeceemWiMAX_rdmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT)))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Reading MAC MGMT Register failed!");
		Mutex_Unlock(&pWiMAX->m_MACMgmtMsg);
		return ERROR_RDM_FAILED;
	}

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Current MAC MGMT STATUS Register : %X", uiMACRegValue);

	if(bToggleMsg)
	{
	    ++iMACMgmtStatus;
		uiMACRegValue = uiMACRegValue & 0xFFFFFFFE;
	}
	else
	{
		--iMACMgmtStatus;
		if(iMACMgmtStatus == 0)
			uiMACRegValue = uiMACRegValue | 0x01;
	}

	
	BeceemWiMAX_wrmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT));
	SharedMem_WriteData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iMACMgmtStatus, sizeof(int), 0);
	Mutex_Unlock(&pWiMAX->m_MACMgmtMsg);
	return SUCCESS;
}

BOOL GetMACMgmtMsgStatus(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	int iMACMgmtStatus;
	UINT uiMACRegValue;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	Mutex_Lock(&pWiMAX->m_MACMgmtMsg);
	SharedMem_ReadData(&pWiMAX->m_shmMACMgmtMsg, (PUCHAR)&iMACMgmtStatus, sizeof(int), 0);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Number clients receiving MAC Msgs : %d", iMACMgmtStatus);

	if(BeceemWiMAX_rdmBulk(pWiMAX, MAC_MGMT_STATUS, (UCHAR *)&uiMACRegValue, sizeof(UINT)))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Reading MAC MGMT Register failed!");
		Mutex_Unlock(&pWiMAX->m_MACMgmtMsg);
		return ERROR_RDM_FAILED;
	}

	Mutex_Unlock(&pWiMAX->m_MACMgmtMsg);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Current MAC MGMT STATUS Register : %X", uiMACRegValue);

	if(uiMACRegValue&1)
	    return FALSE;
	else
		return TRUE;
}

/**
@ingroup Utility_Functions
*	Function		:	This will perform cleanup tasks like 
					- closing the device handle
					- stopping all the threads running
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_CloseDevice(PVOID pArg)
{
	DWORD dwDummyBuffer = 0;

#ifdef WIN32
	UINT32 u32Val = 0;
#endif		
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
    DP;
	/* Stop MAC Mgmt Messages*/
	if(pWiMAX->m_bMACMgmtMsgsEnabled)
	{
		ToggleMACMgmtMsg(pArg, FALSE);
		pWiMAX->m_bMACMgmtMsgsEnabled = FALSE;
	}
	DP;
	/* Perform vendor specific tasks before closing device */
	BeceemWiMAX_VendorSpecificExitFunc(pWiMAX);
#ifdef MACOS	
	DeviceIO_SendOnly(&(pWiMAX->m_Device), IOCTL_CLOSE_NOTIFICATION, (PVOID)&dwDummyBuffer, sizeof(dwDummyBuffer));
#else
	UNUSED_ARGUMENT(dwDummyBuffer);
#endif
	DeviceIO_CloseHandle(&(pWiMAX->m_Device));
	DP;
	Mutex_Lock(&pWiMAX->m_objCallbackMutex);
    pWiMAX->m_pAppCallback = NULL;
    pWiMAX->m_pAppCallbackContext = NULL;
    Mutex_Unlock(&pWiMAX->m_objCallbackMutex);
	DP;
#ifdef WIN32
    u32Val = InterlockedCompareExchange(&pWiMAX->m_iDeviceRemovalIssued,0,1);
    if(u32Val != 0)
    {
		if (Thread_IsValidThread(&pWiMAX->m_DriverUnloadEventThread))
			Thread_Wait(&(pWiMAX->m_DriverUnloadEventThread),INFINITE,0);
	}

#endif        
	/* Kill all threads */
	BeceemWiMAX_KillAllThreads(pWiMAX);

	DP;
	BECEEM_MEMFREE(pWiMAX->pStatsCacheData);
	BECEEM_MEMFREE(pWiMAX->pucMibOutBuffer);
    BECEEM_MEMFREE(pWiMAX->pstHostStatsCache);
    
	BECEEM_MEMFREE(pWiMAX->pEEPROMContextTable);
	pWiMAX->pEEPROMContextTable = NULL;
	DP;
	SharedMem_Close(&(pWiMAX->m_shmMACMgmtMsg));
		
	DP;
	
    return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This will stop all the threads running.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_KillAllThreads(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	/* Stop Periodic Statstics thread */
	if(Thread_Stop(&(pWiMAX->m_StatsThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillStatsThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Stats thread!!!");
		Thread_Terminate(&(pWiMAX->m_StatsThread)); 
	}


	if(Thread_Stop(&(pWiMAX->m_MultipleMibThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillMultipleMibThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Multple Mib thread!!!");
		Thread_Terminate(&(pWiMAX->m_MultipleMibThread)); 
	}

	/* Stop Logger thread */
	if(Thread_Stop(&(pWiMAX->m_LoggerThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillLoggerThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Logger thread!!!");
		Thread_Terminate(&(pWiMAX->m_LoggerThread)); 
	}

	/* Stop NetEntry thread */
	if(Thread_Stop(&(pWiMAX->m_NetEntryThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetEntryThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop NetEntry thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_NetEntryThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objNetEntryEventMsgRecvd));
        }
	}

	if(Thread_Stop(&(pWiMAX->m_WiMAXCommonAPI_NwSearch), (PTHREADSTOPFUNC)BeceemWiMAX_KillCommonAPI_NwSearchThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop NetEntry thread!!!");
		Thread_Terminate(&(pWiMAX->m_WiMAXCommonAPI_NwSearch)); 
	}

	/* Stop SyncUp thread */
	if(Thread_Stop(&(pWiMAX->m_SyncUpThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillSyncUpThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop SyncUp thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_SyncUpThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objSyncUpEventMsgRecvd));
        }
	}

	/* Stop NetSearch thread */
	if(Thread_Stop(&(pWiMAX->m_NetSearchThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillNetSearchThread, pWiMAX) != SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop NetSearch thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_NetSearchThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objNetSearchEventMsgRecvd));
        }
	}
	

	/* Stop Callback thread */
	if(Thread_Stop(&(pWiMAX->m_CallbackThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillCallbackThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Callback thread!!!");
		Thread_Terminate(&(pWiMAX->m_CallbackThread));
	}
	
	/* Stop MAC address response thread*/
	if(Thread_Stop(&(pWiMAX->m_MACAddressWriteRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillMACAddressWriteThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop MAC address thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_MACAddressWriteRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objMACaddressRespEventMsgRecvd));
        }
	}
	
	if(Thread_Stop(&(pWiMAX->m_IdleModeWakeupThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillIdleModeWakeupThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Idle mode wakeup thread!!!");
		Thread_Terminate(&(pWiMAX->m_IdleModeWakeupThread));
	}
	
	/*Stop WiMAX options response thread*/
	if(Thread_Stop(&(pWiMAX->m_WIMAXOptionsRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillWiMAXOptionsThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Callback thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_WIMAXOptionsRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objSetWIMAXOptionsRespEventMsgRecvd));
        }
	}

	/* Stop Shutdown response thread*/
	if(Thread_Stop(&(pWiMAX->m_ShutDownRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillShutDownRespThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Shutdown Response thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_ShutDownRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objShutDownRespEventMsgRecvd));
        }
	}

	/* Antenna config req */
	if(Thread_Stop(&(pWiMAX->m_AntennaConfigRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillAntennaConfigRespThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Antenna config req thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_AntennaConfigRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objAntennaConfigReqEventMsgRecvd));
        }
	}


	/* Antenna Scanning req */
	if(Thread_Stop(&(pWiMAX->m_AntennaScanningRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillAntennaScanningRespThread, pWiMAX)!= SUCCESS)
	{
	    DPRINT_OTHER("Failed to stop Antenna Scanning req thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_AntennaScanningRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objAntennaScanningReqEventMsgRecvd));
        }
	}

	/* Antenna Selection req */
	if(Thread_Stop(&(pWiMAX->m_AntennaSelectionRespThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillAntennaSelectionRespThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT_OTHER("Failed to stop Antenna selection req thread!!!");
		if(Thread_Terminate(&(pWiMAX->m_AntennaSelectionRespThread)) == SUCCESS)
        {
            //so we can recover if we terminate..
            Event_Set(&(pWiMAX->m_objAntennaSelectionReqEventMsgRecvd));
        }
	}

	/* Device Init req */
	if(Thread_Stop(&(pWiMAX->m_DeviceInitThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillDeviceInitThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT_OTHER("Failed to stop Device Init thread!!!");
		Thread_Terminate(&(pWiMAX->m_DeviceInitThread));
	}
#ifdef WIN32
	/* Device Unload Event req */
	if(Thread_Stop(&(pWiMAX->m_DriverUnloadEventThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillDeviceUnloadEventThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT_OTHER("Failed to stop Device Unload Event thread!!!");
		Thread_Terminate(&(pWiMAX->m_DriverUnloadEventThread));
	}
#endif
	if(Thread_Stop(&(pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillCAPIPeriodicMIBGetRequestThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT_OTHER("Failed to stop Periodic MIB get Request thread!!!");
		Thread_Terminate(&(pWiMAX->m_WiMAXCommonAPI_PeriodicMIBGetThread));
		
	}

	if(Thread_Stop(&(pWiMAX->m_CAPIOMAThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillCAPIOMAMonitorThread, pWiMAX)!= SUCCESS)
		
	{
	    DPRINT_OTHER("Failed to stop Periodic MIB get Request thread!!!");
		Thread_Terminate(&(pWiMAX->m_CAPIOMAThread));
		
	}
		
	return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This will stop call-back thread.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*/
void BeceemWiMAX_KillCallbackThread(void *pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "--------Start KillCallBackThread--------");
    pWiMAX->m_bKillCallbackThread = 1;
	Event_Set(&(pWiMAX->m_objReadFileEvent));
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------End KillCallBackThread---------");

	return;
}


/**
@ingroup Utility_Functions
*	Function		:	This will store the application's callback function address in a member function pointer.
						This will also launch a thread to read data from driver and supply it to application by calling application's callback function.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
	@param	PFNBCMCALLBACK pAppCallback - function pointer to application's callback function.
	@param	PVOID pCallbackContext - Void pointer pointing to a context object.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_RegisterAppCallback(PVOID pArg, PVOID pAppCallback, PVOID pCallbackContext)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!BeceemWiMAX_IsValidHandle(pWiMAX))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "Invalid device handle. Callback registeration aborted");
		return ERROR_INVALID_DEVICE_HANDLE;
	}

    Mutex_Lock(&pWiMAX->m_objCallbackMutex);

	pWiMAX->m_pAppCallback =(PFNBCMCALLBACK) pAppCallback;
    pWiMAX->m_pAppCallbackContext = pCallbackContext;

	

    Mutex_Unlock(&pWiMAX->m_objCallbackMutex);

	return SUCCESS;
}

#ifdef WIN32

void CALLBACK ReadNotificaitonRoutine(
								DWORD dwErrorCode,                /* completion code*/
								DWORD dwNumberOfBytesTransfered,  /* number of bytes transferred*/
								LPOVERLAPPED lpOverlapped         /* I/O information buffer*/
								)
{
	PREAD_PACKET pReadPkt = (PREAD_PACKET)lpOverlapped->hEvent;
	memcpy(GpWiMAX->m_szMsg, pReadPkt->m_aucMsg, sizeof(pReadPkt->m_aucMsg));
	if(GpWiMAX->m_Device.hDevice != INVALID_HANDLE_VALUE)
	{
		BeceemWiMAX_NotificationRoutine(GpWiMAX, dwErrorCode, dwNumberOfBytesTransfered);
		pReadPkt->m_bPending = TRUE;
	}
	else
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Callback called on a invalid handle ");
			pReadPkt->m_bPending = FALSE;
	}
}

#endif


/*
*	Function		:	BeceemWiMAX_CallbackThreadFunc
*	Parameters		:	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	Description		:	Thread function for reading data from the driver.
*						This will read the data from the driver and call BeceemWiMAX_NotificationRoutine() function (ProcessResponse.c)
*	Return value	:	0
*/

/**
@ingroup Utility_Functions
*	Function		:	Thread function for reading data from the driver.
						This will read the data from the driver and call BeceemWiMAX_NotificationRoutine() function (ProcessResponse.c)
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@retval	SUCCESS(0)
*/
void *BeceemWiMAX_CallbackThreadFunc(void *pArg)
{
	
#ifdef WIN32
	int i=0, j=0;
#endif
	UINT32 bWhileLoop=1;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();
    if(!pWiMAX)
		return NULL;

	while (bWhileLoop) 
	{
		/*
		 Check to kill this thread
		*/
		if (pWiMAX->m_bKillCallbackThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			   "Breaking out of loop, for callback thread.");
			break;
		}


#ifdef WIN32

		for(i=0, j=0; i<WIN_NUM_READS; i++)
		{
			if(pWiMAX->m_aIRP[i].m_bPending)
			{
				BOOL bStatus = TRUE;
				pWiMAX->m_aIRP[i].m_bPending = FALSE;
				memset(&(pWiMAX->m_aOverlappedArg[i]), 0, sizeof(OVERLAPPED));
				pWiMAX->m_aOverlappedArg[i].hEvent = &(pWiMAX->m_aIRP[i]);
				/* it assures us that a read is ONLY issued
				 on a valid handle */
                bStatus = DeviceIO_ReadOverlapped(
                    &pWiMAX->m_Device,
                    pWiMAX->m_aIRP[i].m_aucMsg,
                    MAX_BUFFER_SIZE,
                    &(pWiMAX->m_aOverlappedArg[i]),
                    (LPOVERLAPPED_COMPLETION_ROUTINE)ReadNotificaitonRoutine
                    );


				
		//		DPRINT(DEBUG_MESSAGE,"\n bStatus # %d",bStatus);
                if(!bStatus)
                {
                    DWORD dwLastError = GetLastError();
					if(dwLastError == ERROR_DEVICE_NOT_CONNECTED)
					{
						DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,"Received ERROR_DEVICE_NOT_CONNECTED from driver.");
						Event_Set(&(pWiMAX->m_DriverUnloadEvent));
					}						
                    else if(pWiMAX->m_bFirmwareDownloadInProgress && (dwLastError == ERROR_GEN_FAILURE))
                    {
                    	pWiMAX->m_aIRP[i].m_bPending = TRUE;
                        Sleep(100);
                        continue;
                    }
                }
				else
					j++;
			}
		}

        {
            DWORD dwRet = 0;
            /*DPRINT(DEBUG_MESSAGE, "Wait - Start, Reads sent : %d",j);*/
            dwRet = WaitForSingleObjectEx(
                GetCurrentThread(),
                WAIT_FOR_THREAD_TIMEOUT,
                TRUE
                );
			/*DPRINT(DEBUG_MESSAGE, "Wait returned : %d",dwRet);*/
        }

#else
		memset(pWiMAX->m_szMsg, 0, sizeof(pWiMAX->m_szMsg));
		if(DeviceIO_Receive(&(pWiMAX->m_Device), pWiMAX->m_szMsg, sizeof(pWiMAX->m_szMsg)) < 0)
		{
		    DPRINT(DEBUG_WARNING, DEBUG_PATH_OTHER, "Reading from device failed.");
			if (errno == ENODEV || errno == -ENODEV)
				BeceemWiMAX_ProcessUnloadDevResponse(pWiMAX);
		}
#endif
		/*
		 Check to kill this thread
		*/
		if (pWiMAX->m_bKillCallbackThread)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			   "Breaking out of loop for callback thread.");
			break;
		}
#ifndef WIN32
		usleep(100);
#endif
	}
	if(!pWiMAX->m_bFirmwareDownloadInProgress)
			DeviceIO_CloseHandle(&pWiMAX->m_Device); 
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Exiting callback thread.");
	return SUCCESS; 
}

/**
@ingroup Utility_Functions
*	Function		:	This is to validate the Wimax Message.	
*	@param	PWIMAX_MESSAGE pMsg - is a pointer to WIMAX_MESSAGE buffer.
*	@param	UINT uiLength - is the length of the above buffer.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ValidateWiMaXMessage(PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int SIZE_WIMAX_HEADER = SIZE_WIMAX_MESSAGE - MAX_VARIABLE_LENGTH;

	if(!pMsg)
    {   
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "No input buffer for sending message !!!");
        return ERROR_INVALID_BUFFER;
    }

    if(uiLength < (UINT)SIZE_WIMAX_HEADER || uiLength > (UINT)SIZE_WIMAX_MESSAGE)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "No input buffer for sending message !!!");
		return ERROR_INVALID_BUFFER_LEN;
	}
    return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This is to retrieve the device handle.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiLength - is the length of the above buffer.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
HANDLE BeceemWiMAX_GetDeviceHandle(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return INVALID_HANDLE_VALUE;

	return DeviceIO_GetDeviceHandle(&(pWiMAX->m_Device));
}


/**
@ingroup Utility_Functions
*	Function		:	This will call the registered application's callback function.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	PWIMAX_MESSAGE pMsg - is a pointer to WIMAX_MESSAGE buffer.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_CallAppCallback(PVOID pArg, PWIMAXMESSAGE pWMMsg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PFNBCMCALLBACK  pAppCallback = NULL;
    PVOID pBuffer = NULL;
    UINT uiMessageLength = 0;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    Mutex_Lock(&pWiMAX->m_objCallbackMutex);
	pAppCallback = pWiMAX->m_pAppCallback;
	Mutex_Unlock(&pWiMAX->m_objCallbackMutex);
    if(!pAppCallback)
		return ERROR_INVALID_CALLBACK_ADDRESS;
    pBuffer = WiMAXMessage_GetMessage(pWMMsg);
    uiMessageLength = WiMAXMessage_GetMessageLength(pWMMsg);
    HEX_DUMP(pBuffer,uiMessageLength);

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_RX, " Calling App registered callback with  T#%d and ST#%d",
			pWMMsg->stMessage.usType,
			pWMMsg->stMessage.usSubType);
	
    pAppCallback(pWiMAX->m_pAppCallbackContext, (UCHAR *) pBuffer,uiMessageLength);

	return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This will write bulk data(multiples of 4 bytes) into registers.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddress - address of register to be written.
*	@param	UCHAR *pucValueArray - pointer pointing to the data to be written.
*	@param	INT iSize - Size of buffer.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_wrmBulk(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iSize)
{
	INT iBytes2Write;
	UINT uiAddr;
	UCHAR *pucValue;
	INT iRet=0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iSize <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;


	iBytes2Write=(int)pWiMAX->u32MaxRDMBufferSize-(2*sizeof(UINT));
	uiAddr = uiAddress;
	pucValue = pucValueArray;
	
	while(iSize)
	{
		if (iBytes2Write > iSize)
          iBytes2Write = iSize;
		iRet = BeceemWiMAX_wrmbulk_ioctlhandler(pWiMAX, uiAddr,pucValue, iBytes2Write);
		
		if(iRet)
		{ 
				return iRet;
		}

		iSize -= iBytes2Write;
		
		uiAddr += iBytes2Write;
		pucValue += iBytes2Write;
	}

	return SUCCESS;
}
/**
@ingroup Utility_Functions
*	Function		:	This will write bulk data(multiples of 4 bytes) into registers when device is in idle mode.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddress - address of register to be written.
*	@param	UCHAR *pucValueArray - pointer pointing to the data to be written.
*	@param	INT iSize - Size of buffer.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_wrmBulkIdle(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iSize)
{
	INT iBytes2Write;
	UINT uiAddr;
	UCHAR *pucValue;
	INT iRet=0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iSize <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;


	iBytes2Write=(int)pWiMAX->u32MaxRDMBufferSize-(2*sizeof(UINT));
	uiAddr = uiAddress;
	pucValue = pucValueArray;
	
	while(iSize)
	{
		if (iBytes2Write > iSize)
          iBytes2Write = iSize;
		iRet = BeceemWiMAX_wrmbulkIdle_ioctlhandler(pWiMAX, uiAddr,pucValue, iBytes2Write);
		
		if(iRet)
		{ 
				return iRet;
		}
		iSize -= iBytes2Write;
		
		uiAddr += iBytes2Write;
		pucValue += iBytes2Write;
	}
	return SUCCESS;
}

/*
*	Function		:	BeceemWiMAX_wrm
*	Parameters		:
*						PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*						UINT uiAddr - address of register to be written.
*						UINT uiValue - pointer pointing to the data to be written.
*
*	Description		:	This will write data(4 bytes) into registers.
*	Return value	:	SUCCESS(0) on success, error code on failure.
*/
/**
@ingroup Utility_Functions
*	Function		:	This will write data(4 bytes) into registers.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be written.
*	@param	UINT uiValue - pointer pointing to the data to be written.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_wrm(PVOID pArg, UINT uiAddr, UINT uiValue)
{
	PVOID pvInBuffer = 0;
	int iInBuffLen = 0;
    int iRet = 0;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

#ifdef WIN32
	{
	INPUT_BUFFER stInputStruct;

	stInputStruct.Register = uiAddr;
	stInputStruct.InputValue = uiValue;

 	pvInBuffer = (void *)&stInputStruct;
	iInBuffLen = sizeof(INPUT_BUFFER);
	}
#else
	{
	WRM_BUFFER WrmBuffer;



	WrmBuffer.ulRegister = uiAddr;
	WrmBuffer.ulLength = sizeof(UINT);
	memcpy(WrmBuffer.aucData, (PUCHAR)&uiValue, sizeof(uiValue));

	pvInBuffer = (void *)&WrmBuffer;
	iInBuffLen = sizeof(WRM_BUFFER);
	}
#endif

	iRet = DeviceIO_SendOnly(
				&(pWiMAX->m_Device),
				IOCTL_BCM_REGISTER_WRITE,
				pvInBuffer,
				iInBuffLen
			);
    if(iRet)
	DPRINT(DEBUG_ERROR, DEBUG_PATH_WRM, "WRM failed, error : %d ", iRet);
    return iRet;

}

/**
@ingroup Utility_Functions
*	Function		:	This will write data(4K bytes) to the specified registers.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be write.
*	@param	UINT *puiOutVal - write data(4k bytes) will be stored in this address.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_wrmbulk_ioctlhandler(PVOID pArg,UINT uiAddr, UCHAR *pucValue,INT iBytes2Write)
{

	PVOID pvInBuffer = 0;
	int iInBuffLen = 0;
    int iRet = 0;
	PBULKWRM_BUFFER pwrmBuffer=NULL;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	BECEEMCALLOC(pwrmBuffer,PBULKWRM_BUFFER,UCHAR,(sizeof(BULKWRM_BUFFER)+iBytes2Write-sizeof(UINT)))
	


		
	pwrmBuffer->Register = uiAddr;
	pwrmBuffer->SwapEndian = 1;

	memcpy((UCHAR *)&pwrmBuffer->Values, pucValue, iBytes2Write);
 	pvInBuffer = (void *)pwrmBuffer;

	iInBuffLen = sizeof(BULKWRM_BUFFER)-sizeof(UINT)+iBytes2Write;
	iRet = DeviceIO_SendOnly(
				&(pWiMAX->m_Device),
				IOCTL_BCM_BULK_WRM,
				pvInBuffer,
				iInBuffLen
			);	




    if(iRet)
		DPRINT(DEBUG_ERROR, DEBUG_PATH_WRM,"WRM failed, error : %d Add=%08X Len=%d  @ function %s and %d", iRet,
	       (UINT) pwrmBuffer->Register, iInBuffLen, __FUNCTION__, __LINE__);
	else
		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_WRM, "WRM @ %08X with Len %d", (UINT) pwrmBuffer->Register,iInBuffLen);
BECEEM_MEMFREE(pwrmBuffer)

return iRet;

}

/**
@ingroup Utility_Functions
*	Function		:	This will write data(4K bytes) to the specified registers when device in idle mode.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be write.
*	@param	UINT *puiOutVal - write data(4k bytes) will be stored in this address.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_wrmbulkIdle_ioctlhandler(PVOID pArg,UINT uiAddr, UCHAR *pucValue,INT iBytes2Write)
{

	PVOID pvInBuffer = 0;
	int iInBuffLen = 0;
    int iRet = 0;
	PBULKWRM_BUFFER pwrmBuffer=NULL;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	BECEEMCALLOC(pwrmBuffer,PBULKWRM_BUFFER,UCHAR,(sizeof(BULKWRM_BUFFER)+iBytes2Write-sizeof(UINT)))
	


		
	pwrmBuffer->Register = uiAddr;
	pwrmBuffer->SwapEndian = 1;

	memcpy((UCHAR *)&pwrmBuffer->Values, pucValue, iBytes2Write);
 	pvInBuffer = (void *)pwrmBuffer;

	iInBuffLen = sizeof(BULKWRM_BUFFER)-sizeof(UINT)+iBytes2Write;
	iRet = DeviceIO_SendOnly(
				&(pWiMAX->m_Device),
				IOCTL_BCM_REGISTER_WRITE_PRIVATE,
				pvInBuffer,
				iInBuffLen
			);	




    if(iRet)
	DPRINT(DEBUG_ERROR, DEBUG_PATH_WRM,
	       "WRM failed, error : %d Add=%08X Len=%d  @ function %s and %d", iRet,
	       (UINT) pwrmBuffer->Register, iInBuffLen, __FUNCTION__, __LINE__);
	else
		DPRINT(DEBUG_MESSAGE, DEBUG_PATH_WRM, "WRM @ %08X with Len %d", (UINT)pwrmBuffer->Register,iInBuffLen);
BECEEM_MEMFREE(pwrmBuffer)

return iRet;

}

/**
@ingroup Utility_Functions
*	Function		:	This will read data(4 bytes) from the specified registers.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@param	UINT *puiOutVal - read data(4 bytes) will be stored in this address.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_rdm(PVOID pArg, UINT uiAddr, UINT *puiOutVal, INT iBytesTobeRead)
{
	DWORD dwBytesReturned = 0;
	void *pvInBuffer = 0;
	int iInputBuffLen = 0;
    int iRet = 0;
 #ifndef WIN32
	RDM_BUFFER stRDMBuffer;
	
#endif
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	
	
#ifdef WIN32 
	pvInBuffer = &uiAddr;
	iInputBuffLen = iBytesTobeRead;
#else
	
	memset(&stRDMBuffer, 0, sizeof(RDM_BUFFER));
	
	stRDMBuffer.ulRegister = uiAddr;
	stRDMBuffer.ulLength = iBytesTobeRead;

	pvInBuffer = (void *)&stRDMBuffer;
	iInputBuffLen = sizeof(RDM_BUFFER);
   
#endif


	iRet = DeviceIO_Send(
					&(pWiMAX->m_Device),
					IOCTL_BCM_REGISTER_READ,
					pvInBuffer,
					#ifdef WIN32
                    sizeof(UINT)
                    #else
					iInputBuffLen
                    #endif
					,
					puiOutVal,
					iBytesTobeRead,
					&dwBytesReturned
   				);

    
	if(iRet)
	{
#ifdef WIN32
         DPRINT(DEBUG_ERROR, DEBUG_PATH_RDM, "RDM failed for register %08x! Error : %d ",uiAddr, iRet);
#else
         DPRINT(DEBUG_ERROR, DEBUG_PATH_RDM, "RDM failed for register %08x! Error : %d ", (UINT)stRDMBuffer.ulRegister, iRet);
#endif
	}
    return iRet;
}
/**
@ingroup Utility_Functions
*	Function		:	This will read data(4 bytes) from the specified registers when device in idle mode.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@param	UINT *puiOutVal - read data(4 bytes) will be stored in this address.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_rdmIdle(PVOID pArg, UINT uiAddr, UINT *puiOutVal, INT iBytesTobeRead)
{
	DWORD dwBytesReturned = 0;
	void *pvInBuffer = 0;
	int iInputBuffLen = 0;
    int iRet = 0;
 #ifndef WIN32
	RDM_BUFFER stRDMBuffer;
#endif
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;


	
#ifdef WIN32 
	pvInBuffer = &uiAddr;
	iInputBuffLen = iBytesTobeRead;
#else
	
	memset(&stRDMBuffer, 0, sizeof(RDM_BUFFER));
	
	stRDMBuffer.ulRegister = uiAddr;
	stRDMBuffer.ulLength = iBytesTobeRead;

	pvInBuffer = (void *)&stRDMBuffer;
	iInputBuffLen = sizeof(RDM_BUFFER);
   
#endif


	iRet = DeviceIO_Send(
					&(pWiMAX->m_Device),
					IOCTL_BCM_REGISTER_READ_PRIVATE,
					pvInBuffer,
					#ifdef WIN32
                    sizeof(UINT)
                    #else
					iInputBuffLen
                    #endif
					,
					puiOutVal,
					iBytesTobeRead,
					&dwBytesReturned
   				);

    
	if(iRet)
	{
#ifdef WIN32
         DPRINT(DEBUG_ERROR, DEBUG_PATH_RDM, "RDM failed for register %08x! Error : %d ",uiAddr, iRet);
#else
         DPRINT(DEBUG_ERROR, DEBUG_PATH_RDM, "RDM failed for register %08x! Error : %d ", (UINT) stRDMBuffer.ulRegister, iRet);
#endif
	}



    return iRet;
}

/**
@ingroup Utility_Functions
*	Function		:	This will read data(multiple of 4 bytes) starting from the specified register address.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@param	UCHAR *pucValueArray - Pointer to the buffer where the read data is stored after reading.
*	@param	INT iBytesTobeRead - size of the above buffer, also the number of bytes to be read.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_rdmBulk(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iBytesTobeRead)
{
	UINT uiAddr;
	UCHAR *pucValue;
	int iBytestoRead = 0;
	int iRet = 0;


	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iBytesTobeRead <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;



	iBytestoRead =(int)pWiMAX->u32MaxRDMBufferSize;
	uiAddr = uiAddress;
	pucValue = pucValueArray;	
	
	while(iBytesTobeRead)
	{
		if (iBytestoRead > iBytesTobeRead )
          iBytestoRead = iBytesTobeRead;

		iRet = BeceemWiMAX_rdm(pWiMAX, uiAddr, (UINT *)pucValue, iBytestoRead);
		if(iRet)
			return iRet;

		iBytesTobeRead -= iBytestoRead;
		
		uiAddr += iBytestoRead;
		pucValue += iBytestoRead;
	}

	return SUCCESS;
}
/**
@ingroup Utility_Functions
*	Function		:	This will read data(multiple of 4 bytes) starting from the specified register address when device in idle mode.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@param	UCHAR *pucValueArray - Pointer to the buffer where the read data is stored after reading.
*	@param	INT iBytesTobeRead - size of the above buffer, also the number of bytes to be read.
*	@retval	SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_rdmBulkIdle(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iBytesTobeRead)
{
	UINT uiAddr;
	UCHAR *pucValue;
	int iBytestoRead = 0;
	int iRet = 0;


	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iBytesTobeRead <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;



	iBytestoRead =(int)pWiMAX->u32MaxRDMBufferSize;
	uiAddr = uiAddress;
	pucValue = pucValueArray;	
	
	while(iBytesTobeRead)
	{
		if (iBytestoRead > iBytesTobeRead )
          iBytestoRead = iBytesTobeRead;

		iRet = BeceemWiMAX_rdmIdle(pWiMAX, uiAddr, (UINT *)pucValue, iBytestoRead);
		if(iRet)
			return iRet;

		iBytesTobeRead -= iBytestoRead;
		
		uiAddr += iBytestoRead;
		pucValue += iBytestoRead;
	}

	return SUCCESS;
}


/**
@ingroup Utility_Functions
*	Function		:	This will read data(4 bytes) from the specified register address.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@retval		Read value.

*/
UINT BeceemWiMAX_rdmsf(PVOID pArg, UINT uiAddr)
{
	UINT uiOutBuffer = 0;
	DWORD BytesReturned = 0;
	int iStatus = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return (UINT)ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return (UINT)ERROR_HANDLE_INVALID;

	iStatus =  DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_REGISTER_READ,
				&uiAddr,
				sizeof(UINT),
				&uiOutBuffer,
				sizeof(UINT),
				&BytesReturned
			);

	if(iStatus)
		return iStatus;

	return uiOutBuffer;
}

/**
@ingroup Utility_Functions
*	Function		:	This will read the statistics doing series of RDMs.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	UINT uiAddr - address of register to be read.
*	@retval		SUCCESS(0) on success, error code on failure.
*/
int BeceemWiMAX_ReadStatistics(PVOID pArg)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if (pWiMAX->m_bStatsPointerFlag == FALSE)
	{
		iRet = BeceemWiMAX_GetStatisticsAddress(pWiMAX, &(pWiMAX->m_uiStatAddress));
		if(iRet)
			return iRet;

		iRet = BeceemWiMAX_ReadStatisticsLength(pWiMAX, pWiMAX->m_uiStatAddress, &(pWiMAX->m_uiStructLength));
		if(iRet)
			return iRet;
		pWiMAX->m_bStatsPointerFlag = TRUE;
	}

	if(pWiMAX->m_uiStructLength > sizeof(pWiMAX->m_aucStatBuffer))
		return ERROR_BUFFER_INSUFFICIENT; 

	iRet = BeceemWiMAX_ReadStats(pWiMAX, pWiMAX->m_uiStatAddress, pWiMAX->m_aucStatBuffer, pWiMAX->m_uiStructLength);
	if(iRet)
		return iRet;

	/* Update BSID */
	UpdateBSID((stStatistics_SS *)pWiMAX->m_aucStatBuffer);

	return pWiMAX->m_uiStructLength;
}

void UpdateBSID(stStatistics_SS *pStats)
{
	/*
	B_UINT8		u8BSID_MSB_4[4];					B_UINT8		au8BSID[6];
	B_UINT8		u8Unused_2[2];			---->		B_UINT8		u8Unused2[2];
	B_UINT8		u8BSID_LSB_2[2];
	*/
	USHORT usLSB = 0;
	UINT uiMSB = 0;

	if(GpWiMAX->m_bBigEndianMachine)
		return;

	usLSB = *(USHORT *)pStats->tLinkStatsEx.u8Unused2;
	uiMSB = *(UINT *)pStats->tLinkStatsEx.au8BSID;

	*(UINT *)pStats->tLinkStatsEx.au8BSID = htonl(uiMSB);
	*(USHORT *)(((UCHAR *)pStats->tLinkStatsEx.au8BSID)+4) = htons(usLSB);
}

int BeceemWiMAX_GetStatisticsAddress(PVOID pArg, UINT32 *puiStatAddress)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !puiStatAddress)
		return ERROR_INVALID_ARGUMENT;

	*puiStatAddress = 0;
	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, puiStatAddress, sizeof(UINT32));
	if(iRet)
		return iRet;

	if(!*puiStatAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

	return SUCCESS;
}

int BeceemWiMAX_ReadStatisticsLength(PVOID pArg, UINT32 uiStatsAddress, UINT32 *puiStatsLength)
{
	int iRet = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	*puiStatsLength = 0;
	iRet = BeceemWiMAX_rdm(pWiMAX, uiStatsAddress, puiStatsLength, sizeof(UINT32));
	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Reading statistics length failed !!!");
		return iRet;
	}

	/* Check for invalid length of stats */
	if(*puiStatsLength < 12)
		return ERROR_INVALID_STATS_LEN;

	*puiStatsLength = sizeof(stStatistics_SS);

	/* WRAP for multiple of 4 */
	if((*puiStatsLength%4) != 0 )
		*puiStatsLength = 4*((*puiStatsLength+3)/4);

	return SUCCESS;
}

int BeceemWiMAX_ReadStats(PVOID pArg, UINT32 uiStatsAddress, PUCHAR pucStats, UINT32 uiStatsLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !pucStats || !uiStatsAddress || !uiStatsLength)
		return ERROR_INVALID_ARGUMENT;

	memset(pucStats, 0, uiStatsLength);
	return BeceemWiMAX_rdmBulk(pWiMAX, uiStatsAddress, pucStats, uiStatsLength);
}

int BeceemWiMX_GetBasebandTemperature(PVOID pArg, UINT *puiTemperature)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	return BeceemWiMAX_rdm(pWiMAX, REG_TEMPARATURE, puiTemperature, sizeof(UINT32));
}

int GetMACAddress(PVOID pArg, UCHAR *pucMACAddress)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UCHAR ucMACAddress[8] = {0};
	UCHAR ucTmpMACAddress[8] = {0};

	if(!pWiMAX || !pucMACAddress)
		return ERROR_INVALID_ARGUMENT;

	BeceemWiMAX_rdmBulk(pWiMAX, MAC_ADDRESS_LOCATION, ucMACAddress, 8);
	
	*(UINT *)ucTmpMACAddress = htonl(*(UINT *)ucMACAddress);
	*(UINT *)(ucTmpMACAddress+4) = htonl(*(UINT*)(ucMACAddress + 4));
	
	memcpy(pucMACAddress,ucTmpMACAddress,6);
	return SUCCESS;
}
int ConvertToLittleEndianForProxy(PVOID pArg, UCHAR *pucData, int iSizeOfData, int iType)
{
	int i = 0;
	UCHAR *pucTemp = NULL;
	UNUSED_ARGUMENT(pArg);

	/*
		iType == 0 FOR SHORT
		iType == 1 FOR INT
	*/
	
	for(i = 0,pucTemp = pucData; i < iSizeOfData;)
	{
		if(iType == 0)
		{
			*(USHORT *)pucTemp = Swap_Short(*(USHORT *)pucTemp);
			pucTemp +=sizeof(USHORT);
			i +=sizeof(USHORT);
		}
		else if(iType == 1)
		{
			*(UINT *)pucTemp = Swap_Int(*(UINT *)pucTemp);
			pucTemp +=sizeof(UINT);
			i +=sizeof(UINT);
		}
		else
		{
			return ERROR_INVALID_ARGUMENT;
		}
	}
	return SUCCESS;
}

// 4-byte number
int Swap_Int(int i)
{
    return SWAP_INTEGER(i);
}
ULONG64 SWAP_Long64(ULONG64 i)
{
	return SWAP_LONGLONG(i);
}

USHORT Swap_Short(USHORT i)
{	

   return SWAP_SHORT(i);

}


int ConvertToHostEndian(UCHAR *pucData, int iSizeOfData, int iType)
{
	int i;
	UCHAR *pucTemp;
	if(!pucData || iSizeOfData == 0)
		return ERROR_INVALID_ARGUMENT;


	/*
		iType == 0 FOR SHORT
		iType == 1 FOR INT
	*/
	
	for(i = 0,pucTemp = pucData; i < iSizeOfData;)
	{
		if(iType == 0)
		{
			*(USHORT *)pucTemp = htons(*(USHORT *)pucTemp);
			pucTemp +=sizeof(USHORT);
			i +=sizeof(USHORT);
		}
		else if(iType == 1)
		{
			*(UINT *)pucTemp = htonl(*(UINT *)pucTemp);
			pucTemp +=sizeof(UINT);
			i +=sizeof(UINT);
		}
		else
		{
			return ERROR_INVALID_ARGUMENT;
		}
	}
	return SUCCESS;
}
/**
iType = 0 htonl
iType = 1 ntohl
iType = 2 Simply SWAP
*/
UINT ConvertLongEndian(UINT uiValue, int iType)
{
		switch(iType)
			{
			case 0:
				return htonl(uiValue);
			case 1:
				return ntohl(uiValue);
			case 2:
				return Swap_Int(uiValue);
			default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "WRONG iTYPE passed to ConvertLongEndian");
				break;
			}
		return uiValue;
		
}

//#define ConvertLongEndian(uiValue, iType) ((iType == 0 )? htonl(uiValue): ntohl(uiValue))
//#define ConvertShortEndian(usValue, iType)	((iType == 0 )? htons(usValue): ntohs(usValue))

/**
iType = 0 htons
iType = 1 ntohs
iType = 2 Simply SWAP
*/
USHORT ConvertShortEndian(USHORT usValue, int iType)
{
		switch(iType)
			{
			case 0:
				return htons(usValue);
			case 1:
				return ntohs(usValue);
			case 2:
				return Swap_Short(usValue);
			default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "WRONG iTYPE passed to ConvertShortEndian");
				break;
			}
		return usValue;
		
}
void ConvertSFParams(CServiceFlowParamSI *pSFParams, int iType)
{
	int i;
	if(!pSFParams)
		return;

	pSFParams->u32SFID = ConvertLongEndian(pSFParams->u32SFID, iType);
	pSFParams->u16CID = ConvertShortEndian(pSFParams->u16CID, iType);
	pSFParams->u32MaxSustainedTrafficRate = ConvertLongEndian(pSFParams->u32MaxSustainedTrafficRate, iType);
	pSFParams->u32MaxTrafficBurst = ConvertLongEndian(pSFParams->u32MaxTrafficBurst, iType);
	pSFParams->u32MinReservedTrafficRate = ConvertLongEndian(pSFParams->u32MinReservedTrafficRate, iType);
	pSFParams->u32ToleratedJitter = ConvertLongEndian(pSFParams->u32ToleratedJitter, iType);
	pSFParams->u32MaximumLatency = ConvertLongEndian(pSFParams->u32MaximumLatency, iType);
	pSFParams->u16TargetSAID = ConvertShortEndian(pSFParams->u16TargetSAID, iType);
	pSFParams->u16ARQWindowSize = ConvertShortEndian(pSFParams->u16ARQWindowSize, iType);
	pSFParams->u16ARQRetryTxTimeOut = ConvertShortEndian(pSFParams->u16ARQRetryTxTimeOut, iType);
	pSFParams->u16ARQRetryRxTimeOut = ConvertShortEndian(pSFParams->u16ARQRetryRxTimeOut, iType);
	pSFParams->u16ARQBlockLifeTime = ConvertShortEndian(pSFParams->u16ARQBlockLifeTime, iType);
	pSFParams->u16ARQSyncLossTimeOut = ConvertShortEndian(pSFParams->u16ARQSyncLossTimeOut, iType);
	pSFParams->u16ARQRxPurgeTimeOut = ConvertShortEndian(pSFParams->u16ARQRxPurgeTimeOut, iType);
	pSFParams->u16ARQBlockSize = ConvertShortEndian(pSFParams->u16ARQBlockSize, iType);
	pSFParams->u16SDUInterArrivalTime = ConvertShortEndian(pSFParams->u16SDUInterArrivalTime, iType);
	pSFParams->u16TimeBase = ConvertShortEndian(pSFParams->u16TimeBase, iType);
	pSFParams->u16UnsolicitedGrantInterval = ConvertShortEndian(pSFParams->u16UnsolicitedGrantInterval, iType);
	pSFParams->u16UnsolicitedPollingInterval = ConvertShortEndian(pSFParams->u16UnsolicitedPollingInterval, iType);
	pSFParams->u16MacOverhead = ConvertShortEndian(pSFParams->u16MacOverhead, iType);

	for(i = 0; i < MAX_CLASSIFIERS_IN_SF; i++)
	{
		pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16UserPriority = ConvertShortEndian(pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16UserPriority, iType);
		pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16VLANID = ConvertShortEndian(pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16VLANID, iType);
		pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16PacketClassificationRuleIndex = ConvertShortEndian(pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16PacketClassificationRuleIndex, iType);
		pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16ClassificationsValid = ConvertShortEndian(pSFParams->cConvergenceSLTypes[i].cCPacketClassificationRule.u16ClassificationsValid, iType);

	}
}

void ConvertAddIndicationEndianess(stLocalSFAddIndication *pDSAInd, int iType)
{
	if(!pDSAInd)
		return;

	pDSAInd->u16TID = ConvertShortEndian(pDSAInd->u16TID, iType);
	pDSAInd->u16CID = ConvertShortEndian(pDSAInd->u16CID, iType);
	pDSAInd->u16VCID = ConvertShortEndian(pDSAInd->u16VCID, iType);

	ConvertSFParams(&pDSAInd->sfActiveSet, iType);
	ConvertSFParams(&pDSAInd->sfAdmittedSet, iType);
	ConvertSFParams(&pDSAInd->sfAuthorizedSet, iType);
}

void ConvertAddRequestEndianess(stLocalSFAddRequest *pDSAReq, int iType)
{
	if(!pDSAReq)
		return;

	pDSAReq->u16TID = ConvertShortEndian(pDSAReq->u16TID, iType);
	pDSAReq->u16CID = ConvertShortEndian(pDSAReq->u16CID, iType);
	pDSAReq->u16VCID = ConvertShortEndian(pDSAReq->u16VCID, iType);

	ConvertSFParams(&pDSAReq->sfParameterSet, iType);
}
void ConvertSLeepModeEndianess(ST_SLEEPMODE_MSG *pstSleepMode, int iType)
{
    unsigned int i, j;
    if(!pstSleepMode)
		return;
    for(i=0;i<pstSleepMode->numOfDefinitions;i++)
        {
            pstSleepMode->PSC_Defn[i].enActivity=ConvertLongEndian(pstSleepMode->PSC_Defn[i].enActivity, iType);
            pstSleepMode->PSC_Defn[i].u32NoOfCids=ConvertLongEndian(pstSleepMode->PSC_Defn[i].u32NoOfCids,iType);
            pstSleepMode->PSC_Defn[i].u16FinalSleepWindowBase=ConvertShortEndian(pstSleepMode->PSC_Defn[i].u16FinalSleepWindowBase, iType);
			for (j=0;j<pstSleepMode->PSC_Defn[i].u32NoOfCids;j++)
				pstSleepMode->PSC_Defn[i].aCIDs[j]=ConvertShortEndian(pstSleepMode->PSC_Defn[i].aCIDs[j], iType);
        }
    pstSleepMode->numOfDefinitions=ConvertLongEndian(pstSleepMode->numOfDefinitions,iType);
    
	
}

void ConvertIdleModeEndianess(ST_IDLEMODE_INDICATION *pstIdleMode)
{
	if(!pstIdleMode)
		return;

	pstIdleMode->u32IdleModePeriod = SWAP_ENDIAN32(pstIdleMode->u32IdleModePeriod);
	pstIdleMode->s32RSSI = ntohl(pstIdleMode->s32RSSI);
	pstIdleMode->s32CINR = ntohl(pstIdleMode->s32CINR);	    
	pstIdleMode->u32PreambleIndex = SWAP_ENDIAN32(pstIdleMode->u32PreambleIndex);
	//pstIdleMode->u32VendorSpecificInformationLength = SWAP_ENDIAN32(pstIdleMode->u32VendorSpecificInformationLength);
}

UINT SWAP_ENDIAN32(UINT data)
{
		return ntohl(data);
}

void ConvertAntennaConfigRespEndianess(ST_GPIO_SETTING_INFO *pstGpioSettingInfo)
{
	int i = 0, size = 0;
    unsigned int * response;
	if(!pstGpioSettingInfo)
		return;
	size = sizeof(ST_GPIO_SETTING_INFO)/4;
    response = (unsigned int*)pstGpioSettingInfo;

	for(i=0; i< size; i++)
		response[i] =  SWAP_ENDIAN32(response[i]);
}

void ConvertAntennaScanningRespEndianess(ST_RSSI_INFO_IND *pstRssiInfoInd)
{
	int i = 0, size = 0;
	unsigned int* response;


	if(!pstRssiInfoInd)
		return;
    size = (sizeof(ST_RSSI_INFO_IND)/4);
	response = (unsigned int*)pstRssiInfoInd;
    for(i=0;i<size;i++) 
	{
      response[i] = SWAP_ENDIAN32(response[i]);
	} 
}

void q_sort(UINT numbers[], UINT left, UINT right)
{
	UINT pivot, l_hold, r_hold;

	l_hold = left;
	r_hold = right;
	pivot = numbers[left];
	while (left < right)
	{
		while ((numbers[right] >= pivot) && (left < right))
			right--;
		if (left != right)
		{
			numbers[left] = numbers[right];
			left++;
		}
		while ((numbers[left] <= pivot) && (left < right))
			left++;
		if (left != right)
		{
			numbers[right] = numbers[left];
			right--;
		}
	}
	numbers[left] = pivot;
	pivot = left;
	left = l_hold;
	right = r_hold;

	if (left < pivot)
	q_sort(numbers, left, pivot-1);
	
	if (right > pivot)
	q_sort(numbers, pivot+1, right);
}

void QuickSort(UINT numbers[], int array_size)
{
  q_sort(numbers, 0, array_size - 1);
}

INT BeceemWiMAX_NVMwrm(UINT32 uiOffset, PUCHAR pBuffer,UINT32 uiSize)
{
	
	NVM_READWRITE stNVMReadWrite = {0};

	INT iRet = 0;
	UINT32 u32SwapWrite = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	if(!pWiMAX )
		return -1;
	if(!pBuffer || !uiSize) //validity check for offset ?
		return ERROR_INVALID_ARGUMENT;
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM WRM with offset %d and size %d", uiOffset, uiSize);
	if(uiSize == 4)
	{
		u32SwapWrite = SWAP_ENDIAN32( *((UINT32*)pBuffer));
	
	}
	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
		Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));

		uiOffset = pWiMAX->u32NVM_StartOffset  + uiOffset;
		stNVMReadWrite.bVerify = TRUE;
		if(uiSize == 4)	
		{
			stNVMReadWrite.pBuffer = &u32SwapWrite;
		}
		else
		{
		stNVMReadWrite.pBuffer = pBuffer;
		}
		stNVMReadWrite.uiNumBytes = uiSize;
		stNVMReadWrite.uiOffset = uiOffset;
		
		iRet = DeviceIO_SendOnly(
				&(pWiMAX->m_Device),
				IOCTL_BCM_NVM_WRITE,
				(PVOID)&stNVMReadWrite,
				sizeof(NVM_READWRITE)
			);
    	if(iRet)
    		{
    		  uiSize =(UINT32)ERROR_EEPROM_WRITE_FAILED;
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "NVM WRM failed with error : %d ", iRet);
    		}
		else 
			uiSize = 4;
		/*DPRINT(DEBUG_MESSAGE, "NVM WRM data START @ offset %x!!!",stNVMReadWrite.uiOffset);
			HexDump(stNVMReadWrite.pBuffer, stNVMReadWrite.uiNumBytes);
		DPRINT(DEBUG_MESSAGE, "NVM WRM data END!!!");*/

		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
		
    return uiSize;

}
INT BeceemWiMAX_GetNVMSize(PUINT puiNVMSize)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	INT iRet = 0;
	DWORD iBufferReturned = 0;
	if(!pWiMAX || !puiNVMSize)
		return ERROR_INVALID_ARGUMENT;

	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_GET_NVM_SIZE,
				NULL,0,
				(PVOID)puiNVMSize,
				sizeof(UINT32),
				&iBufferReturned
			);
    	if(iRet)
	DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "NVM size read failed with error : %d ", iRet);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM size %d", *puiNVMSize);
	return SUCCESS;
}
INT BeceemWiMAX_NVMrdm(UINT32 uiOffset, PUCHAR pBuffer,UINT32 uiSize)
{
	
	PNVM_READWRITE pstNVMReadWrite = NULL;
	PUCHAR pContiguousMemory = NULL;
	INT iRet = 0;
	DWORD iBufferReturned;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	if(!pWiMAX)
		return -1;
	
	if(!pBuffer || !uiSize)
		return ERROR_INVALID_ARGUMENT;
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM rdm with offset %d and size %d", uiOffset, uiSize);
	
	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
		Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));

		uiOffset = pWiMAX->u32NVM_StartOffset  + uiOffset;
		BECEEMCALLOC(pContiguousMemory,PUCHAR,NVM_READWRITE,1)
		pstNVMReadWrite = (PNVM_READWRITE)pContiguousMemory;
			
		pstNVMReadWrite->uiNumBytes = uiSize;
		pstNVMReadWrite->uiOffset = uiOffset;
		pstNVMReadWrite->pBuffer = (PUCHAR)pBuffer;

		iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_NVM_READ,
				(PVOID)pstNVMReadWrite,
				sizeof(NVM_READWRITE),
				#ifdef WIN32
				NULL
				#else
				(PVOID)pstNVMReadWrite
				#endif
				,
				#ifdef WIN32
				0
				#else
				sizeof(NVM_READWRITE)
				#endif
				,
				&iBufferReturned
			);
	
		if(uiSize == 4)
		{
			*((UINT32*)pstNVMReadWrite->pBuffer) = SWAP_ENDIAN32( *((UINT32*)pstNVMReadWrite->pBuffer));
		}
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Bytes returned after NVM read %ul",
	   (UINT) iBufferReturned);

		/*DPRINT(DEBUG_MESSAGE, "NVM RDM data START @ offset %x!!!",stNVMReadWrite.uiOffset);
			HexDump(stNVMReadWrite.pBuffer, stNVMReadWrite.uiNumBytes);
		DPRINT(DEBUG_MESSAGE, "NVM RDM data END!!!");*/
    	if(iRet)
    		{
			  uiSize =(UINT32) ERROR_READ_FAILED;
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "NVM rdm failed with error : %d ", iRet);
    		}
		else
			{
			uiSize = 4;
			}
		BECEEM_MEMFREE(pContiguousMemory);
		
		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

    return uiSize;

}
INT BeceemWiMAX_eepromwrm(UINT uiAddr, INT uiValue)
{
	PVOID pvInBuffer = 0;
	int iInBuffLen = 0;
    int iRet = 0;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

#ifdef WIN32
	{
	INPUT_BUFFER stInputStruct;

	stInputStruct.Register = uiAddr;
	stInputStruct.InputValue = uiValue;

 	pvInBuffer = (void *)&stInputStruct;
	iInBuffLen = sizeof(INPUT_BUFFER);
	}
#else
	{
	WRM_BUFFER WrmBuffer;

	WrmBuffer.ulRegister = uiAddr;
	WrmBuffer.ulLength = sizeof(UINT);
	memcpy(WrmBuffer.aucData, (PUCHAR)&uiValue, sizeof(UINT));

	pvInBuffer = (void *)&WrmBuffer;
	iInBuffLen = sizeof(WRM_BUFFER);
	}
#endif

	iRet = DeviceIO_SendOnly(
				&(pWiMAX->m_Device),
				IOCTL_BCM_EEPROM_REGISTER_WRITE,
				pvInBuffer,
				(DWORD)iInBuffLen
			);
    if(iRet)
		DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "WRM failed, error : %d ", iRet);
    return iRet;

}


INT BeceemWiMAX_eepromrdm(UINT uiAddr, INT *puiOutVal)
{
	DWORD dwBytesReturned = 0;
	void *pvInBuffer = 0;
	int iInputBuffLen = 0;
    int iRet = 0;
    
#ifndef WIN32
	RDM_BUFFER stRDMBuffer;
	
#endif
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) GpWiMAX;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

#ifdef WIN32 
	pvInBuffer = &uiAddr;
	iInputBuffLen = sizeof(UINT);
#else
	
	memset(&stRDMBuffer, 0, sizeof(RDM_BUFFER));
	
	stRDMBuffer.ulRegister = uiAddr;
	stRDMBuffer.ulLength = sizeof(UINT);

	pvInBuffer = (void *)&stRDMBuffer;
	iInputBuffLen = sizeof(RDM_BUFFER);
    
#endif

	iRet = DeviceIO_Send(
					&(pWiMAX->m_Device),
					IOCTL_BCM_EEPROM_REGISTER_READ,
					pvInBuffer,
					iInputBuffLen,
					puiOutVal,
					sizeof(UINT),
					&dwBytesReturned
   				);

    if(iRet)
	{
#ifdef WIN32
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "RDM failed for register %08x! Error : %d ", uiAddr,
		   iRet);
#else
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "RDM failed for register %08x! Error : %d ",
		   (UINT) stRDMBuffer.ulRegister, iRet);
#endif
	}

    return iRet;
} 

 
/*  ========================================================================== */
/*  -------------------------------- EEPROM Bulk Write ---------------------------- */
/*  ========================================================================== */
#ifdef NVM_NOT_SUPPORTED

int BeceemWiMAX_HostEEPROMBulkWrite(PVOID pArg,unsigned int u32Offset, UCHAR * pu8Buffer, INT u32Size)
{
	unsigned int u32VerifyData = 0;
	unsigned int uiSPIReg = 0;
	unsigned int uiSPIOffset = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
	return -1;
	
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
#ifndef MACOS
//	u32Data = SWAP_ENDIAN32(u32Data);
#endif
	
	uiSPIReg = 0x06000000;
	uiSPIOffset = ((pWiMAX->u32NVM_StartOffset  + u32Offset)|0xFA000000);



	if(!BeceemWiMAX_eepromwrm(CFG_EEPROM_CMDQ_SPI_REG, uiSPIReg) &&
		/*  Data to be written */
		!BeceemWiMAX_eepromwrmBulk(pArg, CFG_EEPROM_WRITEDATA_SPI_REG, pu8Buffer, u32Size) &&
		/*  Dst address */
		!BeceemWiMAX_eepromwrm(CFG_EEPROM_CMDQ_SPI_REG, uiSPIOffset)) {
#ifndef WIN32
		usleep(300);
#else
		Sleep(4);
#endif

		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

		return 0;
		/* read back the data and verfiy */
		/*
		if(BeceemWiMAX_HostEEPROMRead(pWiMAX,u32Offset, &u32VerifyData) && u32VerifyData == u32Data) {
			return 0x4;
		}
		*/
	}
	else
		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

	return ERROR_EEPROM_WRITE_FAILED;
}

/*  ========================================================================== */
/*  -------------------------------- EEPROM Write ---------------------------- */
/*  ========================================================================== */
int BeceemWiMAX_HostEEPROMWrite(PVOID pArg,unsigned int u32Offset, unsigned int u32Data)
{
	unsigned int u32VerifyData = 0;
	unsigned int uiSPIReg = 0;
	unsigned int uiSPIOffset = 0;
	unsigned int u32TempData = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
	return -1;
	
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));

	u32Data = SWAP_ENDIAN32(u32Data);

	
	uiSPIReg = 0x06000000;
	uiSPIOffset = ((pWiMAX->u32NVM_StartOffset  + u32Offset)|0x3A000000);

	

	if(!BeceemWiMAX_eepromwrm(CFG_EEPROM_CMDQ_SPI_REG, uiSPIReg) &&
		/*  Data to be written */
		!BeceemWiMAX_eepromwrm(CFG_EEPROM_WRITEDATA_SPI_REG, u32Data) &&
		/*  Dst address */
		!BeceemWiMAX_eepromwrm(CFG_EEPROM_CMDQ_SPI_REG, uiSPIOffset)) {
#ifndef WIN32
		usleep(300);
#else
		Sleep(3);
#endif

		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

		/* read back the data and verfiy */
		if(BeceemWiMAX_HostEEPROMRead(pWiMAX,u32Offset, &u32VerifyData)) {// && u32VerifyData == u32Data) {
			//u32VerifyData
			//CFG_Swap((CFG_UINT8 *)&u32VerifyData, sizeof(u32VerifyData), sizeof(u32VerifyData));
			u32TempData = u32VerifyData >> 24 & 0x000000FF;
			u32TempData |= u32VerifyData >> 8 & 0x0000FF00;
			u32TempData |= u32VerifyData << 8 & 0x00FF0000;
			u32TempData |= u32VerifyData << 24  & 0xFF000000;
			if(u32TempData  == u32Data)
				return 0x4;
		}		
	}
	else
		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

	return ERROR_EEPROM_WRITE_FAILED;
}

#endif

/*  ========================================================================== */
/*  -------------------------------- EEPROM Read ----------------------------- */
/*  ========================================================================== */
int BeceemWiMAX_HostEEPROMRead(PVOID pArg, unsigned int u32Offset,UINT32 *pu32SrcData)
{
	unsigned int u32Data[4] = {0};
	unsigned int uiSPIOffset = 0;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return -1;

	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	
	uiSPIOffset = ((pWiMAX->u32NVM_StartOffset + u32Offset)|0x3B000000);

	/*  Dst address */
	if(!BeceemWiMAX_eepromwrm(CFG_EEPROM_CMDQ_SPI_REG, uiSPIOffset)) {
#ifndef WIN32
		usleep(200);
#else
		Sleep(2);
#endif
		/*SERIAL PERIPHERAL INTERFACE REGISTER  READ OPERATION*/
		/*  Data to be read */
		if(!BeceemWiMAX_eepromrdm(CFG_EEPROM_READDATA_SPI_REG,(INT *) &u32Data[0])    &&
			  !BeceemWiMAX_eepromrdm(CFG_EEPROM_READDATA_SPI_REG,(INT *) &u32Data[1]) &&
			  !BeceemWiMAX_eepromrdm(CFG_EEPROM_READDATA_SPI_REG,(INT *) &u32Data[2]) &&
			  !BeceemWiMAX_eepromrdm(CFG_EEPROM_READDATA_SPI_REG,(INT *) &u32Data[3])) {
				
			  *pu32SrcData = u32Data[3]|(u32Data[2] << 8)|(u32Data[1] << 16)|(u32Data[0] << 24);
			  //*pu32SrcData = u32Data[0]|(u32Data[1] << 8)|(u32Data[2] << 16)|(u32Data[3] << 24);
			  Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
			  return 0x4;
		}
	}
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return 0;
}


int BeceemWiMAX_eepromrdmBulk(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iBytesTobeRead)
{
	int iBytes2Read = 0;
	UINT uiSizeOfINT;
	UINT uiAddr;
	UCHAR *pucValue;
	UINT32 bWhileLoop=1;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iBytesTobeRead <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	iBytes2Read = iBytesTobeRead;
	uiSizeOfINT = sizeof(UINT);
	uiAddr = uiAddress;
	pucValue = pucValueArray;

/* #ifdef WIN32	 */
	while(bWhileLoop)
	{
		UINT uiValue = 0;
		int iRet = 0;
	
		iRet = BeceemWiMAX_eepromrdm(uiAddr,(INT *) &uiValue);
		if(iRet)
			return iRet;

		if(iBytes2Read > (INT)uiSizeOfINT)
			memcpy(pucValue, &uiValue, uiSizeOfINT);
		else
			memcpy(pucValue, (UCHAR *)&uiValue, iBytes2Read);

		iBytes2Read -= uiSizeOfINT;

		if(iBytes2Read <= 0)
			break;

		uiAddr += 4;/*4 is assumed to be register length*/
		pucValue+=uiSizeOfINT;
	}
/* 
#else
	{
		RDM_BUFFER stRDMBuffer;
		int iRet = 0;
		memset(&stRDMBuffer, 0, sizeof(RDM_BUFFER));

		stRDMBuffer.ulRegister = uiAddress;
		stRDMBuffer.ulLength = iBytesTobeRead;

		iRet =  DeviceIO_Send(
					&(pWiMAX->m_Device),
					IOCTL_BCM_REGISTER_READ,
					(void *)&stRDMBuffer,
					sizeof(RDM_BUFFER),
					pucValueArray,
					iBytesTobeRead,
					NULL
   				);
		return iRet;
	}
#endif
*/
	return SUCCESS;
}

int BeceemWiMAX_eepromwrmBulk(PVOID pArg, UINT uiAddress, UCHAR *pucValueArray, INT iSize)
{
	INT iBytes2Write;
	UINT uiSizeOfINT;
	UINT uiAddr;
	UCHAR *pucValue;
	UINT32 bWhileLoop=1;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	if(!pucValueArray)
		return ERROR_INVALID_BUFFER;

	if(iSize <=0)
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	iBytes2Write = iSize;
	uiSizeOfINT = sizeof(UINT);
	uiAddr = uiAddress;
	pucValue = pucValueArray;

	while(bWhileLoop)
	{
		UINT uiValue = 0;
		INT iRet = 0;

		if(iBytes2Write > (INT)uiSizeOfINT)
			memcpy(&uiValue,pucValue,uiSizeOfINT);
		else
			memcpy(&uiValue,pucValue,iBytes2Write);

		iRet = BeceemWiMAX_eepromwrm(uiAddr,uiValue);
		if(iRet)
			return iRet;

		iBytes2Write -= uiSizeOfINT;
	
		if(iBytes2Write <= 0)
			break;

		//uiAddr += 4;/*4 is assumed to be register length	*/
		pucValue+=uiSizeOfINT;
	}
	return SUCCESS;
}

int BeceemWiMAX_InitIRP(PVOID pArg)
{
#ifdef WIN32
	int i = 0;
#endif
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

#ifdef WIN32
	for(i=0; i<MAX_READ_PACKETS; i++)
	{
		memset(&(pWiMAX->m_aIRP[i]), 0, sizeof(READ_PACKET));
		pWiMAX->m_aIRP[i].m_bPending = TRUE;
	}
#endif

	return SUCCESS;
}

void HexDump(void *pBuffer, UINT32 u32Length,PCHAR pFunName,UINT uLineNo)
{
    UINT32 n, u32Start, u32End;
    const int HEX_DUMP_BYTES_PER_LINE = 16;
	PCHAR pPrintDebug = NULL;
    UINT8 *pByte = (UINT8 *) pBuffer;

	BECEEMCALLOC_NOT_RET(pPrintDebug,PCHAR,char,8098)
  
    for (u32Start = 0; u32Start < u32Length; u32Start += HEX_DUMP_BYTES_PER_LINE)
    {
		u32End = u32Start + HEX_DUMP_BYTES_PER_LINE;
	    sprintf((char *) pPrintDebug, "%04X: %s %d ", u32Start,pFunName,uLineNo);
	 
		for (n = u32Start; n < u32End; n++)
		{
			if (n < u32Length)
				sprintf((char*)(pPrintDebug + strlen((char*)pPrintDebug)),"%02X ", pByte[n]);
			else
				sprintf((char*)(pPrintDebug + strlen((char*)pPrintDebug)),"   ");
		}
	    DPRINT_HEX_DUMP(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "%s", pPrintDebug);
	}
	BECEEM_MEMFREE(pPrintDebug);
} /* HexDump */

PINT BeceemWiMAX_DeviceInitThreadFunc(PVOID pArg)
{
	int iRet = 0;
	BOOL bRet = FALSE;
	UINT32 u32ChipVersion = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	ST_TRIGGER_INFO stTriggerInfo = {0};
	LINK_STATE stLinkState = {0};
	INT32 iTriggerThreshold = 0, iTriggerType = 0, iTriggerThresholdOffset = 0, iTriggerTypeOffset = 0;
	FILE *fp = NULL;
	#ifdef LAUNCH_CFG_THREAD
	SET_THREAD_NAME();
	#endif
	
	
	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;


	BeceemWiMAX_GetLinkState(pWiMAX, &stLinkState);	
    pWiMAX->m_bIdleMode=stLinkState.bIdleMode;
	pWiMAX->m_bModemInShutdown=stLinkState.bShutdownMode;

	if (pWiMAX->m_bIdleMode == TRUE)
	{
		PWIMAXMESSAGE pOWMMsg = NULL;
		ST_IDLEMODE_INDICATION stIdlemodeIndication = {0};
		BECEEMCALLOC_RET(pOWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PINT)
		
		WiMAXMessage_Init(
			pOWMMsg,
			TYPE_IDLE_MODE, 
			SUBTYPE_IDLE_MODE_INDICATION, 
			(UCHAR*)&stIdlemodeIndication,
			sizeof(ST_IDLEMODE_INDICATION)
			);
		BeceemWiMAX_CallAppCallback(pWiMAX, pOWMMsg);
		BECEEM_MEMFREE(pOWMMsg);
		return SUCCESS;
	}

	if (pWiMAX->m_bModemInShutdown == TRUE)
	{
		PWIMAXMESSAGE pOWMMsg = NULL;
		UINT32 u32ShutdownModeStatus = !pWiMAX->m_bModemInShutdown;
		
		BECEEMCALLOC_RET(pOWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1,PINT)
		
				
		WiMAXMessage_Init(
						pOWMMsg,
						TYPE_SHUTDOWN, 
						SUBTYPE_SHUTDOWN_MODE_STATUS_RESP,
						(UCHAR*)&u32ShutdownModeStatus,
						sizeof(UINT32)
						);
		
		BeceemWiMAX_CallAppCallback(pWiMAX, pOWMMsg);
		BECEEM_MEMFREE(pOWMMsg);
		return SUCCESS;
	}


	/* Initializing WiMAX trigger information structure */
	iTriggerTypeOffset = (UADDTYPE)&((SYSTEM_CONFIGURATION *)NULL)->HostDrvrConfig4;
	iTriggerThresholdOffset = (UADDTYPE)&((SYSTEM_CONFIGURATION *)NULL)->HostDrvrConfig5;

	iRet = OpenSysConfigFile(&fp, "r");
	if(iRet)
		return (PINT)ERROR_FILE_OPERATION;
	iRet = fseek(fp, iTriggerTypeOffset, SEEK_SET);
	if(iRet)
		return (PINT)ERROR_FILE_OPERATION;

	fread(&iTriggerType,1,sizeof(UINT),fp);
	
	*(UINT *)&iTriggerType = htonl(*(UINT *)&iTriggerType) >> BIT_POSITION_WIMAX_TRIGGER_TYPE;
	iRet = fseek(fp, iTriggerThresholdOffset, SEEK_SET);
	if(iRet)
		return (PINT)ERROR_FILE_OPERATION;

	fread(&iTriggerThreshold,1,sizeof(UINT),fp);

	*(UINT *)&iTriggerThreshold = htonl(*(UINT *)&iTriggerThreshold);

	if(iTriggerType == TRIGGER_TYPE_RSSI || iTriggerType == TRIGGER_TYPE_CINR)
		stTriggerInfo.eTriggerType = iTriggerType;
	else 
		stTriggerInfo.eTriggerType = TRIGGER_TYPE_NOT_DEFINED;
	
	stTriggerInfo.s32TriggerThreshold = iTriggerThreshold;
	memcpy(&pWiMAX->stTriggerInformation,&stTriggerInfo,sizeof(stTriggerInfo));
	CloseSysConfigFile(&fp);

	
	
		
	
	BeceemWiMAX_rdm(pWiMAX, BASEBAND_CHIP_VER_ADDR, &u32ChipVersion, sizeof(UINT32));
	
	if (!pWiMAX->m_bInitEEPROM)
	{
		bRet = BeceemWiMAX_OpenEEPROM(pArg);
		if (bRet == TRUE)
		{
			pWiMAX->m_bDeviceInitInProgress = FALSE;
		}
		else if ((u32ChipVersion != 0) && (u32ChipVersion <= 0xBECE0121))
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "EEPROM Initialization Failed\n");
		}
		else
		{
			int i =0;
			UINT32 u32ChipVersion = 0;            
            int iWaitCount = 0;
            UINT32 nSleepTime = 0;
			
#ifdef LAUNCH_CFG_THREAD
			/* If in thread mode.. then it's the original wait times */
			iWaitCount = 1000;
			nSleepTime = 1000;
#else
			iWaitCount = 10;
			nSleepTime = 100;
#endif
			for (i=0; i<=iWaitCount; i++)
			{
				if (pWiMAX->m_bKillDeviceInitThread)
					break;
		
				iRet = BeceemWiMAX_rdm(pWiMAX, BASEBAND_CHIP_VER_ADDR, &u32ChipVersion, sizeof(UINT32));
				if (iRet < 0)
				{
#ifndef WIN32
	usleep(nSleepTime * 1000);
#else
	Sleep(nSleepTime);
#endif
				    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
					   "BeceemWiMAX_DeviceInitThreadFunc: Still RDMs are failing. Wait for sometime to complete CFG_Init.\n");
				}
				else
				{
				    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
					   "BeceemWiMAX_DeviceInitThreadFunc: Can GoAhead with CFG_Init.\n");
					break;
				}
			}

			bRet = BeceemWiMAX_OpenEEPROM(pArg);
			if (bRet == TRUE)
			{
				pWiMAX->m_bDeviceInitInProgress = FALSE;
			}
		}
	}
	else
	{
	   pWiMAX->m_bDeviceInitInProgress = FALSE;
	}
	return SUCCESS;
}

VOID BeceemWiMAX_KillDeviceInitThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "--------Start KillDeviceSyncThread--------");
	pWiMAX->m_bKillDeviceInitThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------End KillDeviceSyncThread---------");

	return;
}

PVOID BeceemWiMAX_DeviceRemovalThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------Device Removal Thread Start---------------");
	/* Reset the statistics pointer flag */
	pWiMAX->m_bStatsPointerFlag = FALSE;
	BeceemWiMAX_ProcessUnloadDevResponse(pWiMAX);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------Device Removal Thread End---------------");
		
	return SUCCESS;
}



INT BeceemWiMAX_ValidateRdmAddress(UINT32 uiAddr)
{
	UINT uiAddrMask = 0x0F000000;


	if ((uiAddr & uiAddrMask) != uiAddrMask)
	{
#ifdef WIN32
		DWORD dwRetVal = GetLastError();

	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "WRONG RDM Address : 0x%x, ErrorCode is: [%d]\n",
		   uiAddr, dwRetVal);
#else
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "WRONG RDM Address : 0x%x: [%d]\n", uiAddr, uiAddr);
#endif
		return ERROR_INVALID_ADDRS_INPUT;
	}
	return SUCCESS;
}

#ifdef WIN32
int BeceemWiMAX_DeviceUnloadShareEvent(PVOID pArg)
{
	int iRet = 0, iStatus = 0;
	DWORD dwLastError = 0;
	HANDLE hEvent = 0;
	ST_BCM_LIB_SHARED_EVENTS stDriverSharedEvent = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "BeceemWiMAX_DeviceUnloadShareEvent is called.");

    
   	Mutex_Lock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
   	
	pWiMAX->m_DriverUnloadEvent.hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "BcmDeviceUnloadEvent");
	hEvent = pWiMAX->m_DriverUnloadEvent.hEvent;	
	dwLastError = GetLastError();
	if(dwLastError == ERROR_FILE_NOT_FOUND)
	{

		iRet = Event_Create(&(pWiMAX->m_DriverUnloadEvent),TRUE,FALSE,"BcmDeviceUnloadEvent");
        
		if (iRet != SUCCESS)
		{
			Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
			return iRet;
		}

	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
		   "BeceemWiMAX_DeviceUnloadShareEvent initialization started.\n");
		if (hEvent != pWiMAX->m_DriverUnloadEvent.hEvent)
        {
			if (hEvent)
            {
				CloseHandle(hEvent); 
            }
        }
	
		if (pWiMAX->m_DriverUnloadEvent.hEvent != INVALID_HANDLE_VALUE && pWiMAX->m_DriverUnloadEvent.hEvent != NULL)
		{
			stDriverSharedEvent.nTotalEvents = 1;
			stDriverSharedEvent.arrhEvents[0] = pWiMAX->m_DriverUnloadEvent.hEvent;

			iStatus =  DeviceIO_SendOnly(
					&(pWiMAX->m_Device),
					IOCTL_BCM_LIB_SHAREDEVENTS,
					&stDriverSharedEvent,
					sizeof(stDriverSharedEvent)
					);

			if(iStatus)
			{
				Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
				
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
				   "IOCTL_BCM_LIB_SHAREDEVENTS failed with status %d", iStatus);
				pWiMAX->m_bStatsPointerFlag = FALSE;
				BeceemWiMAX_ProcessUnloadDevResponse(pWiMAX);			
				return iStatus;
			}

		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
			   "BeceemWiMAX_DeviceUnloadShareEvent is initialized.\n");
		}
		else
		{
			Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
			return ERROR_INVALID_HANDLE;
		}
		Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
	}
	else
	{
		Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
	}

#ifdef WIN32
	if(Thread_Run(&(pWiMAX->m_DriverUnloadEventThread),(PTHREADFUNC)BeceemWiMAX_DeviceUnloadEventThreadFunc,pWiMAX))
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "Launching Device Unload Shared Event THREAD failed !!!");
		return -1;
	}
#endif

	
	return SUCCESS;
}

PINT BeceemWiMAX_DeviceUnloadEventThreadFunc(PVOID pArg)
{

	UINT32 u32Val = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	DWORD dwRetVal = 0;
	BOOLEAN bDeviceRemovalEvent = FALSE;
    SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	for (;;)
	{
		if(pWiMAX->m_bKillDriverUnloadEvent == 1)
			break;
	
        if(pWiMAX->m_DriverUnloadEvent.hEvent == NULL)
        {
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "DriverUnloadEvent has been closed.");
        	break;
        }

        /* consider a method where we know for sure we are no longer waiting on this handle
           before closing it in the DeviceIO_Close or simple
           switching to the method of not using a shared event for this

           also changed wait time,  since 2000 seconds would be alot of time
           trying to kill the thread. down.
	    */
		dwRetVal = Event_Wait(&(pWiMAX->m_DriverUnloadEvent),100);
		if (dwRetVal == WAIT_OBJECT_0)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "DriverUnloadEvent Wait Object set.");
			DeviceIO_CloseHandle(&pWiMAX->m_Device);
			bDeviceRemovalEvent = TRUE;
			break;
		}
		else if(dwRetVal == WAIT_ABANDONED)
		{
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "DriverUnloadEvent Wait Abandoned.");
			break;
		}
	}
	Mutex_Lock(&pWiMAX->m_CloseDeviceRemovalEventMutex);
	if (pWiMAX->m_DriverUnloadEvent.hEvent)
		CloseHandle(pWiMAX->m_DriverUnloadEvent.hEvent);
	pWiMAX->m_DriverUnloadEvent.hEvent = NULL;
	Mutex_Unlock(&pWiMAX->m_CloseDeviceRemovalEventMutex);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Acquired the m_DeviceRemovalIssuedMutex !!!");
	
	Mutex_Lock(&GpWiMAX->m_DeviceRemovalIssuedMutex);
	u32Val = InterlockedCompareExchange(&GpWiMAX->m_iDeviceRemovalIssued,1,0);
	if((u32Val == 0)&&(bDeviceRemovalEvent))
	{
		if (pWiMAX->m_bKillCallbackThread == 0)
		{

		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "BeceemWiMAX_DeviceRemovalThread ENTRY");
		   	BeceemWiMAX_DeviceRemovalThread(GpWiMAX);
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "BeceemWiMAX_DeviceRemovalThread EXIT");
			GpWiMAX->m_iDeviceRemovalIssued = TRUE;
            
        }
    }
	Mutex_Unlock(&GpWiMAX->m_DeviceRemovalIssuedMutex);
	
	return SUCCESS;
}

VOID BeceemWiMAX_KillDeviceUnloadEventThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "--------Start KillDeviceUnloadEventThread --------");
	pWiMAX->m_bKillDriverUnloadEvent = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------End KillDeviceUnloadEventThread ---------");

	return;
}
#endif

VOID BeceemWiMAX_KillCAPIPeriodicMIBGetRequestThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
	   "--------Start KillCAPIPeriodicMIBGetRequestThread --------");
	pWiMAX->m_bKillWimaxCommonAPI_PeriodicMIBRequests = 1;
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
	   "---------End KillCAPIPeriodicMIBGetRequestThread ---------");
}

VOID BeceemWiMAX_KillCAPIOMAMonitorThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
	   "--------Start KillCAPIPeriodicMIBGetRequestThread --------");
	pWiMAX->m_bKillWimaxCommonAPI_CAPIOMAThread = 1;
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
	   "---------End KillCAPIPeriodicMIBGetRequestThread ---------");
}

VOID BeceemWiMAX_KillDeviceRemovalThread(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "--------Start KillDeviceRemovalThread --------");
	pWiMAX->m_bKillDeviceRemovalThread = 1;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "---------End KillDeviceRemovalThread ---------");

	return;
}

PVOID GetDeviceDataMRUPointer()
{
	return GpWiMAX->pDeviceMRUData;		
}

BOOL BeceemWiMAX_EnsureEEPROMInitialized(PVOID pArg)
{
    PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

    /* Check Idle mode status and update the variable */
    if(pWiMAX->m_bDeviceInitInProgress == TRUE || pWiMAX->m_bInitEEPROM == FALSE)
    {
        LINK_STATE stLinkState = {0};
        BeceemWiMAX_GetLinkState(pArg, &stLinkState);

        if(stLinkState.bIdleMode != TRUE && stLinkState.bShutdownMode != TRUE)
        {
            //init device false eh?
            if(!pWiMAX->m_bInitEEPROM)
            {
                if(BeceemWiMAX_OpenEEPROM(pWiMAX) == TRUE)
                {
                    pWiMAX->m_bDeviceInitInProgress = FALSE;
                }
                else
                {
                    pWiMAX->m_bDeviceInitInProgress = FALSE;
                    return FALSE;
                }
            }
            else
            {
                pWiMAX->m_bDeviceInitInProgress = FALSE;
                //eep rom already inited 
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
    
}
BOOL BeceemWiMAX_IsSafeToAccessContextTabel()
{
	return GpWiMAX->m_bInitEEPROM;
}
VOID BeceemWiMax_GetDriverInfoSettings(PBECEEMWIMAX pWiMAX)
{
	INT iRet = 0,iBufferReturned = 0;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return;

	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_GET_DEVICE_DRIVER_INFO,
				NULL,
				0,
				(PVOID)&pWiMAX->stDriverInfo	,
				sizeof(DEVICE_DRIVER_INFO),
				(LPDWORD)&iBufferReturned
			);
	if(iRet)
	{
		
		UINT32 u32HIBRetVal = 0, u32ChipVersion = 0;
		
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_GET_DEVICE_DRIVER_INFO failed with status %d\n", iRet);

		CheckIfNVMIsFlash(pWiMAX);

		BeceemWiMAX_rdm(pWiMAX, BASEBAND_CHIP_VER_ADDR, &u32ChipVersion, sizeof(UINT32));
		BeceemWiMAX_rdm(pWiMAX, BASEBANSD_SYS_CONFIG_ADDR, &u32HIBRetVal, sizeof(UINT32));
		BeceemWiMAX_FindInterfaceType(pWiMAX,u32ChipVersion,u32HIBRetVal);

		if(pWiMAX->eBCM_INTERFACETYPE==BCM_MII)		
			pWiMAX->u32NVM_StartOffset =4096;
		else
			pWiMAX->u32NVM_StartOffset =512;		
		pWiMAX->u32MaxRDMBufferSize=4096;


		pWiMAX->u32DeviceStatus =DEV_UNKNOWN_DRIVER_STATE;
			
	}
	else
	{
		
		pWiMAX->u32OffsetForUnalignedAccessCorrection = pWiMAX->stDriverInfo.u32RxAlignmentCorrection;
		
		pWiMAX->eBCM_INTERFACETYPE =pWiMAX->stDriverInfo.u32InterfaceType;
		if(pWiMAX->stDriverInfo.u32NVMType==NVM_FLASH)
			pWiMAX->u32NVMIsFlashFlag = TRUE;
		else
			pWiMAX->u32NVMIsFlashFlag = FALSE;

		g_bNVMTypeIsFLASH = pWiMAX->u32NVMIsFlashFlag;

		pWiMAX->u32MaxRDMBufferSize=pWiMAX->stDriverInfo.MaxRDMBufferSize;
		pWiMAX->u32NVM_StartOffset =pWiMAX->stDriverInfo.u32DSDStartOffset;
		pWiMAX->u32DeviceStatus =pWiMAX->stDriverInfo.u32DeviceStatus;
		
	}

	if(g_bNVMTypeIsFLASH)
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM Type=%d \n", NVM_FLASH);
	else
	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM Type=%d \n", NVM_EEPROM);

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Interface Type=%d \n", pWiMAX->eBCM_INTERFACETYPE);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM Start Offset=%d \n", pWiMAX->u32NVM_StartOffset);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "MAX RDM Buffer Size=%d \n", pWiMAX->u32MaxRDMBufferSize);

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Device Status=%02X \n",pWiMAX->u32DeviceStatus);


}

INT BeceemWiMAX_GetDeviceIoctlFailureReason()
{
	INT iRetVal = 0;

	DeviceIO_GetDeviceStatus(GpWiMAX);
	
	if(GpWiMAX->u32DeviceStatus==DEV_IN_RESET)
	{
		iRetVal = ERROR_DEVICE_IS_IN_RESET_MODE;
		goto retCode;
	}
	else
	{
		/* Check for Idle mode/shutdown mode status */
		
		LINK_STATE stLinkState = {0};

		iRetVal = DeviceIO_GetLinkState(GpWiMAX , &stLinkState);
		if(iRetVal)
		{
			DPRINT(DEBUG_MESSAGE,DEBUG_PATH_TX,  "DeviceIO_GetLinkState returned %d !!!",iRetVal);
			goto retCode;
        }
		
		if(stLinkState.bIdleMode)
		{
	        GpWiMAX->m_bIdleMode = TRUE;
          	iRetVal = ERROR_MODEM_IN_IDLE_MODE;
			goto retCode;
		}
		else if (stLinkState.bShutdownMode)
		{
			GpWiMAX->m_bModemInShutdown = TRUE;
			iRetVal = ERROR_MODEM_IN_SHUT_DOWN_MODE;
			goto retCode;
		}
	}

	iRetVal = ERROR_DEVICEIOCONTROL_FAILED;
	
retCode:
	return iRetVal;

}
int get_id_for_mutexName(const char *pMutexName)
{
    int iIndex = 0;
    if(!pMutexName)
        return -1;
    
        if(!strcmp(BECEEM_SHMEM_MUTEX_NAME,pMutexName))
        {
            
            goto ret_path;
        }
        iIndex++;
       if(!strcmp(BECEEM_STATS_MUTEX_NAME,pMutexName))
        {
            goto ret_path;
        }
        iIndex++;
       if(!strcmp(BECEEM_PROPRIETARY_MUTEX,pMutexName))
        {
            goto ret_path;
        }
        iIndex++;

        if(!strcmp(BECEEM_CAPI_CALLBACK_MUTEX,pMutexName))
        {
            goto ret_path;
        }
       iIndex++;
     if(!strcmp(BECEEM_CAPI_INTERFACE_MUTEX ,pMutexName))
        {
            goto ret_path;
        }
       iIndex++;
     if(!strcmp(BECEEM_MUTEX_FIRMWARE_DOWNLOAD,pMutexName))
        {
            goto ret_path;
        }
      iIndex++;
     if(!strcmp(BECEEM_MUTEX_EEPROM_ACCEESS,pMutexName))
        {
            goto ret_path;
        }
       iIndex++;
     if(!strcmp(BECEEM_MUTEX_MAC_MGMT,pMutexName))
        {
            goto ret_path;
        }

     iIndex++;
     if(!strcmp(BECEEM_SHMEM_COMMON_API_MUTEX,pMutexName))
        {
            goto ret_path;
        }
       iIndex++;
     if(!strcmp(BECEEM_NWSEARCH_COMMON_API_MUTEX,pMutexName))
        {
            goto ret_path;
        }
       iIndex++;
	 if(!strcmp(DYN_LOGGING_MUTEX,pMutexName))
        {
            goto ret_path;
        }  

 ret_path:
    
 DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Mutex Name#%s index#%d\n", pMutexName,iIndex);
     return iIndex;
}

