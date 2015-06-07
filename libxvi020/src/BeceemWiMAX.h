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
 * Description	:	Library internal include header file.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_WIMAX
#define BECEEM_WIMAX


#include "PrivateTypedefs.h"
#include "ErrorDefs.h"
#include "Messages.h"
#include "StatisticsStruct.h"
#include "MibUserApi.h"
#include "OMADMUserApi.h"
#include "HostMIBSInterface.h"
#include "uti_EEPROMVSRODef.h"
#include "cntrl_SignalingInterface.h"
#include "VendorSpecificTypeDefs.h"
#include "OMADMTreeStruct.h"
#include "flashinfoaccess.h"

#include "DriverPacket.h"
#include "MibDefines.h"
#include "config_paramsSS.h"

#include "WiMAXMessage.h"
#include "Logger.h"
#include "uti_BeceemEEPROMCfg.h"


#include "libraryversion.h"

#ifdef WIN32
#include "win32/DebugPrint.h"
#include "win32/Thread.h"
#include "win32/DeviceIO.h"
#include "win32/SyncObjects.h"
#include "win32/IOCTLs.h"
#include "win32/SharedMemory.h"
#else
#include "linux/DebugPrint.h"
#include "linux/Thread.h"
#include "linux/DeviceIO.h"
#include "linux/SyncObjects.h"
#include "linux/IOCTLs.h"
#include "linux/SharedMemory.h"
#include <syslog.h>
#include <sys/resource.h>
#include <errno.h>
#endif

#include <signal.h>
#ifdef WIN32
#pragma warning( push ) 
#pragma warning(disable : 4214 4152)	/*To avoid nonstandard extension used : bit field types other than int */
#pragma warning(disable : 4055 4054)	/* To avoid functionpointer conversion waring */
#pragma warning( disable : 4127) 
#endif

#ifndef DEBUG
#ifdef WIN32
#pragma warning(disable : 4057 )
#endif
#endif
//Macros to be moved:

#define ALLOCATE_MEMORY(ptr,data_type,usize,uoldsize) do\
			{\
			    UINT uiMemSet = (ptr == NULL)? TRUE:FALSE;\
			    ptr=(data_type *)realloc(ptr,(size_t)usize);\
			    if(!ptr)\
                 {\
                    printf("\n realloc error:%s.%d: " ,__FUNCTION__, __LINE__);\
                 }\
                else if(uiMemSet)\
                {\
                memset(ptr,0,usize);\
                }\
                else if(uoldsize < usize)\
                {\
                    PUCHAR puchNewMem = (PUCHAR )ptr + uoldsize;\
                    memset(puchNewMem,0,usize - uoldsize);\
                }\
			}\
			while(0)


#define COPY_MEMORY(ptr_dest,ptr_src,length) do\
			{\
			    if(!ptr_dest)\
                 {\
                    printf("\n dest ptr is NULL @:%s.%d: " ,__FUNCTION__, __LINE__);\
                 }\
                 else if(!ptr_src)\
                    {\
                        printf("\n src ptr is NULL @:%s.%d: " ,__FUNCTION__, __LINE__);\
                    }\
                 if(length == 0)\
                    {\
                        printf("\n length is NULL @:%s.%d: " ,__FUNCTION__, __LINE__);\
                    }\
                   memcpy(ptr_dest ,ptr_src,length);\
			}\
			while(0)

#define FREE_MEMORY(ptr) do\
			{\
			    if(!ptr)\
                 {\
                    printf("\n dest ptr is NULL @:%s.%d: " ,__FUNCTION__, __LINE__);\
                 }\
                 else\
                    {\
                         free(ptr);\
                         ptr = NULL;\
                 }\
			}\
			while(0)

#define BECEEMCALLOC_NOT_RET(ptr,type,usize,no)\
		ptr = (type)calloc(sizeof(usize),no);\
		if(!ptr){DPRINT_OTHER("\nMemory allocation failed  @ function %s and %d",__FUNCTION__,__LINE__);return;}

#define BECEEMCALLOC_RET(ptr,type,usize,no,rettype)\
		ptr = (type)calloc(sizeof(usize),no);\
		if(!ptr){DPRINT_OTHER("\nMemory allocation failed @ function %s and %d",__FUNCTION__,__LINE__);return (rettype)ERROR_MALLOC_FAILED;}

#define BECEEMCALLOC(ptr,type,usize,no)\
		ptr = (type)calloc(sizeof(usize),no);\
		if(!ptr){DPRINT_OTHER("\nMemory allocation failed  @ function %s and %d",__FUNCTION__,__LINE__);return ERROR_MALLOC_FAILED;}
#define BECEEM_MEMFREE(ptr) if(ptr){free(ptr);ptr=NULL;}


#define VALIDATE_SEC_PARAM_ID(SecID)\
		if(SecID<ENCRYPTION_STATUS_FLAG || SecID>PRIVATE_KEY_DATA2) return ERROR_SEC_OBJECT_NOTFOUND;


/* Extremely usefully for tracking down problems when using visual studio */
#ifndef SET_THREAD_NAME
#if defined(_WIN32) || defined(WIN32)
#define SET_THREAD_NAME() Thread_SetDebugName(__FUNCTION__ )
#else
#define SET_THREAD_NAME() Thread_SetDebugName(__FUNCTION__ )
#endif
#endif

#ifndef min
#define min(a,b)	(((a)>(b))? (b):(a))
#endif
#ifndef max
#define max(a,b)	(((a)<(b))? (b):(a))
#endif


typedef intptr_t  UADDTYPE;

#define UNUSED_ARGUMENT(x) (void)x	//To avoid warning for unused variable


#define MAX_BUFFER_SIZE		3072	/*Pending Buffer for waiting DLL Notification Routinue */
#define MAX_KILL_RETRY		10	/* Max num of thread kill retries */
#define MAC_ADDRESS_LENGTH	6
#define MAX_FILENAME_LENGTH	250
#define MAX_BASE_STATIONS			80
#define SIGNALS_MAX			31
#ifdef LINUX
#define CORE_FILE_SOFTLIMIT			20*1024*1024
#define CORE_FILE_HARDLIMIT			RLIM_INFINITY	// ?
#endif

#define TIMEOUT_FOR_STATS_POINTER	1000
#define TIMEOUT_FOR_NET_ENTRY		20000
#define TIMEOUT_FOR_IDLEMODEWAKEUP_REQ	60000
#define TIMEOUT_FOR_SYNC_UP			10000
#define TIMEOUT_FOR_NET_SEARCH		160000
#define TIMEOUT_FOR_MAC_ADDRESS		60000
#define TIMEOUT_FOR_SHUTDOWN_REQ	20000
#define TIMEOUT_FOR_IDLEMODEWAKEUP_REQ	60000
#define TIMEOUT_FOR_ANTENNA_CONFIG_REQ	20000
#define TIMEOUT_FOR_ANTENNA_SCANNING_REQ	20000
#define TIMEOUT_FOR_ANTENNA_SELECTION_REQ	20000
#define TIMEOUT_FOR_WIMAX_OPTIONS	10000
#define WAIT_FOR_THREAD_TIMEOUT		200
#define PHY_SYNC_ACHIEVED			0xA5A5A5A5
#define PHY_TRYING_TO_SYNC			0xBBBBBBBB

#define PLL_LOCKED					0x0000CAFE
#define PLL_NOT_LOCKED				0x0000DEAF
#define TIMEOUT_CALL_BACK_SERIALIZATION_EVENT 1000
#define OMA_DM_MAJOR_VERSION_INTERNAL   4
#define OMA_DM_MINOR_VERSION_INTERNAL   0

#define WIMAX_SET_OPTION_BIT0_SYNC	1
#define SERVING_BSID_BIT_POSITION   1

#define REUSE3_CINR_VALID_BIT_POSITION   3
#define REUSE1_CINR_VALID_BIT_POSITION    4


#define FREQBAND_2_3_RANGE_START	2300000
#define FREQBAND_2_3_RANGE_END		2400000
#define FREQBAND_2_5_RANGE_START	2485000
#define FREQBAND_2_5_RANGE_END		2700000
#define FREQBAND_3_5_RANGE_START	3300000
#define FREQBAND_3_5_RANGE_END		3700000

#define CONTROLMSGS_DEFAULT_MASK	0xFFFFF7FF	/* Default mask for control messages. 
							   Logging messages(0xAB swstatus) are disabled */

#define SWSTATUS_BIT_MASK			0x1F	/* This mask is used to identify control msg SwStatus */


#define BECEEM_SHAREDMEM_NAME		"BECEEM_SHARED_MEMORY"
#define BECEEM_SHMEM_MUTEX_NAME		"BECEEM_SHMEM_MUTEX"
#define BECEEM_STATS_MUTEX_NAME		"BECEEM_STATS_MUTEX_NAME"

#define BECEEM_PROPRIETARY_MUTEX		"BCM_PROP_API_ACCESS_MUTEX"
#define BECEEM_CAPI_CALLBACK_MUTEX		"BECEEM_CAPI_CALLBACK_MUTEX"
#define BECEEM_CAPI_INTERFACE_MUTEX     "BECEEM_CAPI_INTERFACE_MUTEX"

#define BECEEM_SHMEM_COMMON_API_MUTEX		"BECEEM_SHMEM_COMMON_API_MUTEX"
#define BECEEM_NWSEARCH_COMMON_API_MUTEX		"BCM_NWSEARCH_CAPI_MUTEX"


#ifdef WIN32
#define BECEEM_SHARED_MAC_MGMT		"BECEEM_SHARED_MAC_MGMT"
#else
#define BECEEM_SHARED_MAC_MGMT		100
#endif

#ifdef WIN32
#define BECEEM_SHARED_COMMON_API_DATA  "BECEEM_SHARED_COMMON_API_DATA"
#else
#define BECEEM_SHARED_COMMON_API_DATA  200
#endif

#define BECEEM_MUTEX_MAC_MGMT		"BECEEM_MUTEX_MAC_MGMT"
#define BECEEM_MUTEX_EEPROM_ACCEESS "BECEEM_MUTEX_EEPROM_ACCESS"
#define BECEEM_MUTEX_FIRMWARE_DOWNLOAD "BECEEM_MUTEX_FIRMWARE_DOWNLOAD"


#ifdef WIN32
#define SYS_CONFIG_FILE_NAME	"\\Drivers\\macxvi.cfg"
#else
#ifdef MACOS
#define LIB_PATH "/Library/Firmware/Beceem/"
#elif __ANDROID__
#define LIB_PATH "/system/etc/wimax/"
#else
#define LIB_PATH "/lib/firmware/"
#endif

#define SYS_CONFIG_FILE_NAME	"macxvi.cfg"
#define SYS_FW_FILE_NAME		"macxvi200.bin"
#define SYS_350_FW_FILE_NAME   "macxvi350.bin"
#endif
#ifdef LINUX
#define DYN_LOGGING_MUTEX	"/bcm_dyn_log_sem"
#else 
#define DYN_LOGGING_MUTEX	"BECEEM_LOGGING_MUTEX"
#endif
#define SWAP_INTEGER(DATA) ((DATA&0xff)<<24)+((DATA&0xff00)<<8)+((DATA&0xff0000)>>8)+((DATA>>24)&0xff)
#define SWAP_LONGLONG(DATA) ((DATA&0xff)<<56)|((DATA&0xff00)<<40)|((DATA&0xff0000)<<24)|((DATA&0xff000000LL)<<8)|((DATA&0xff00000000LL)>>8)|((DATA&0xff0000000000LL)>>24)|((DATA&0xff000000000000LL)>>40)|((DATA>>56)&0xff)
#define Pad4(usLen) (USHORT)(((usLen+3)/4)*4)

#define STATS_PTR_ADDRESS			0x1F60F02C
#define REG_BSINFO_ADDR				0x1F60FB00
#define FIRMWARE_VER_INT_ADDR		0xBFFFF480
#define	FIRMWARE_VER_FLOAT1_ADDR	0xBFFFF484
#define	FIRMWARE_VER_FLOAT2_ADDR	0xBFFFF488
#define MAC_ADDRESS_LOCATION		0x1F60D000
#define RF_CHIP_VERSION				0x1F60F148
#define CENTER_FREQ_LOCATION		0x1F60F138
#define REG_TEMPARATURE				0x1F60F154
#define NUM_BS_ADDR					0x1F60F3D0
#define CONFIG_STD_MIBS_ADDRESS		0X1F60AC80
#define MAC_MGMT_STATUS				0x1F60F024
#define POWER_LEVEL_ADDRS			0x1f60f200
#define	ANTENNA_VALUE				0x1f60f214
#define IDLE_MODE_INACITIVITY_ADDR	0xBFC02F98
#define PHY_STATE_ADDRS				0x1F60F01C
#define PLL_STATUS_ADDRS			0x1F60F050
#define FRAMES_ELAPSED_INFO_ADDR	0x1F60F030
#define BASEBAND_BW_INFO_ADDR		0x1F60F12C
#define BASEBAND_TX_PWR_INFO_ADDR	0x1F60F218
#define BASEBAND_ANTENNA_VAL_ADDR	0x1F60f214
#define BS_TRACK_NUM_OF_RECORDS_ADDR 0x1F60F3D0
#define BS_TRACK_DATA_INFO_ADDR		0x1F60FB00
#define BS_TRACK_VALID_INFO_ADDR	0x1F60F344
#define BS_TRACK_PREV_FREQ_ADDR		0x1F60F340
#define BS_TRACK_PREV_PREAMBLE_ADDR 0x1F60F3C4
#define BASEBAND_CHIP_VER_ADDR		0xAF000000
#define BASEBANSD_SYS_CONFIG_ADDR	0xAF000004
#define BASEBAND_REG_RESET_BER_ADDR	0x1F60F040	/* Register to reset counters */
#define BASEBAND_NAI_FIELD_SIZE		256
#define MS120						0xbece0120
#define MS121						0xbece0121
#define MS130						0xbece0130
#define MS200						0xbece0200
#define MS300						0xbece0300
#define MS210						0xbece0210
#define MS310						0xbece0310
#define MS3200						0xbece3200
#define MS3300						0xbece3300
#define MS3301						0xbece3301
#define MS3311						0xbece3311
#define MS3321						0xbece3321
#define SYMPHONY_CHIP_MASK			0xbece3500
      

#define EERPOM_VER_INT_ADDR			0x1f60ef10
#define EEPROM_SIZE_INT_ADDR		0x1f60ef08
#define EEPROM_CHECKSUM_INT_ADDR	0x1f60ef04
#define EEPROM_CALIB_BASE_ADDR		0x1f60d014	/* consider d016 addressed value */
#define	EEPROM_CHECKSUM_VALID_ADDR	0x1f60ef00
#define NVM_IDENTIFY_LOCATION		0x1fb00000
#define NVM_TYPE_FLASH_SIGNATURE	0xbeadbead

#define BASEBAND_DEVICE_NOT_FUNCTIONING	31
#define FIRMWARE_STATUS_CHECK_LOCATION 0xBFFFFC00
#define FIRMWARE_HEALTH_SIGNATURE 0xabbaabba
#define MAX_FLASH_MIB_UPDATES_DONE_AT_A_TIME 50

#define MAX_CERTBUFF_LEN (10*1024) //Maximum size of security certificate possible (should always be less than 7K indeed) since sec section is 7K 


#define EEPROM_CHK_VAILIDITY(x,i) (((x) & (1 << i))?1:0)

#define AUTH_ID_OFFSET	10
//#define SWAP16(data) ((data << 8) | (data >> 8))
#define SWAP16(data)  htons(data)
#define MAX_READ_PACKETS 10
#define WIN_NUM_READS		4

#define BIT_MASK_AUTO_SCAN_ENABLE 0x1
#define BIT_POSITION_WIMAX_TRIGGER_TYPE 30
#define BIT_WIMAX_TRIGGER_TYPE_LOWER	30
#define BIT_WIMAX_TRIGGER_TYPE_UPPER	31
#include "dynamiclogging.h"


typedef struct _INPUT_BUFFER {
    UINT InputValue;
    UINT Register;

} INPUT_BUFFER, *PINPUT_BUFFER;


typedef struct _READ_PACKET {
    BOOL m_bPending;
    UCHAR m_aucMsg[MAX_BUFFER_SIZE];
} READ_PACKET, *PREAD_PACKET;


#define MAX_MULTIPLE_MIBS 50

typedef struct _MULTIPLE_MIB_OBJECTS_NOTIFY_ST {
    UINT32 u32NumMibObjs;
    UINT32 u32TimePeriod;
    UINT32 u32MIBsFragmentSize;
    UINT32 u32MIBUpdateFlag;
    MIB_OBJECT_INFO MibObjs[MAX_MULTIPLE_MIBS];
} MULTIPLE_MIB_OBJECTS_NOTIFY_ST, *PMULTIPLE_MIB_OBJECTS_NOTIFY_ST;


typedef struct _BECEEMWIMAX {
#if 1
    DEVICEIO m_Device;

    MUTEX m_objCallbackMutex;
    PFNBCMCALLBACK m_pAppCallback;
    PVOID m_pAppCallbackContext;

    THREAD m_CallbackThread;
    EVENT m_objReadFileEvent;
#endif

#if 1
    UCHAR m_szMsg[MAX_BUFFER_SIZE];
    DWORD m_dwBuffSize;
    UCHAR m_aucStatBuffer[2048];

    /* For Firmware download */
    THREAD m_FirmwareThread;
    ST_FIRMWARE_DNLD m_FirmwareInfo;
#if WIN32
    HANDLE m_hFileHandle;
    HANDLE m_hFileMap;
    HANDLE m_hMappedFile;
#else
    UINT32 m_u32FileDescriptor;
#endif

    PVOID pFirmwareFile;
    UINT32 m_u32FirmwareSize;

    /* Device Data for MRU */
    PVOID pDeviceMRUData;

    /* Device Data for MIB Cache */
    PUCHAR pStatsCacheData;
    S_MIBS_HOST_STATS_MIBS *pstHostStatsCache;
    PWIMAXMESSAGE pucMibOutBuffer;

    /*Statistics Request */
    THREAD m_StatsThread;
    UINT m_uiStatsPeriod;

    /* Statistics Pointer */
    UINT m_uiStatAddress;
    UINT m_uiStructLength;
    UINT m_bMultiMIBCall;
    /*Logger thread */
    THREAD m_LoggerThread;
    int m_iLoggerCount;

    WIMAXLOGGER m_IRLogger;
    WIMAXLOGGER m_BRLogger;
    WIMAXLOGGER m_PRLogger;
    WIMAXLOGGER m_BWLLogger;
    WIMAXLOGGER m_CLLogger;
    WIMAXLOGGER m_OLLogger;
    WIMAXLOGGER m_CIDLogger;
    WIMAXLOGGER m_CQICHLogger;

    int m_iIRPrevLogIndex;
    int m_iBRPrevLogIndex;
    int m_iPRPrevLogIndex;
    int m_iBWLPrevLogIndex;
    int m_iCLPrevLogIndex;
    int m_iOLPrevLogIndex;
    int m_iCIDPrevLogIndex;
    int m_iCQICHPrevLogIndex;
    int m_iSetWiMAXReqInfo;

#ifdef WIN32
    READ_PACKET m_aIRP[MAX_READ_PACKETS];
    OVERLAPPED m_aOverlappedArg[MAX_READ_PACKETS];
#endif

    /* NET_ENTRY */
    EVENT m_objNetEntryEvent;
    EVENT m_objNetEntryEventMsgRecvd;
    THREAD m_NetEntryThread;

    /* Sync Up */
    EVENT m_objSyncUpEvent;
    EVENT m_objSyncUpEventMsgRecvd;
    THREAD m_SyncUpThread;

    /* NET SEARCH */
    EVENT m_objNetSearchEvent;
    EVENT m_objNetSearchEventMsgRecvd;
    THREAD m_NetSearchThread;

    /*MAC Address write */
    EVENT m_objMACaddressRespEvent;
    EVENT m_objMACaddressRespEventMsgRecvd;
    THREAD m_MACAddressWriteRespThread;

    EVENT m_objSetWIMAXOptionsRespEvent;
    EVENT m_objSetWIMAXOptionsRespEventMsgRecvd;
    THREAD m_WIMAXOptionsRespThread;

    /* SHUTDOWN req */
    EVENT m_objShutDownRespEvent;
    EVENT m_objShutDownRespEventMsgRecvd;
    THREAD m_ShutDownRespThread;

    /* Antenna config req */
    EVENT m_objAntennaConfigReqEvent;
    EVENT m_objAntennaConfigReqEventMsgRecvd;
    THREAD m_AntennaConfigRespThread;

    /* Antenna Scanning req */
    EVENT m_objAntennaScanningReqEvent;
    EVENT m_objAntennaScanningReqEventMsgRecvd;
    THREAD m_AntennaScanningRespThread;

    /* Antenna Selection req */
    EVENT m_objAntennaSelectionReqEvent;
    EVENT m_objAntennaSelectionReqEventMsgRecvd;

    THREAD m_AntennaSelectionRespThread;


    /*Init Thread */
    THREAD m_DeviceInitThread;

    UINT32 m_iPropStatsIndex;
    UINT32 m_iStdStatsIndex;
    UINT32 m_iPropCfgIndex;
    UINT32 m_iStdCfgIndex;

    /* To make sure only 1 device removal notification is issued */
    volatile UINT32 m_iDeviceRemovalIssued;
    MUTEX m_DeviceRemovalIssuedMutex;

    S_MIB_INFO m_sMIBTable[BCM_PROPREITARY_STATS_MIB_BASE_END + 1];

    /* MUTEX for Shared memory read/writes */
    MUTEX m_ShMemMutex;

    PUCHAR pEEPROMContextTable;
    UINT32 u32EEPROMContextTableSize;

    /* for shared for WiMAX Common API */
    SHARED_MEM m_SharedMemCommonAPI;

    /*Shared Memory Common API mutex */
    MUTEX m_ShMemCommonAPIMutex;
    MUTEX m_CAPINWSearchMutex;

    EVENT m_WiMAXCommonAPI_NetworkEntry;
    EVENT m_WiMAXCommonAPI_WaitForFirmwareDownload;
    EVENT m_WiMAXCommonAPI_ModemSearchComplete;
    EVENT m_WiMAXCommonAPI_WaitForShutdownResponse;
    EVENT m_WiMAXCommonAPI_StartCaplSearchRequest;
    EVENT m_WiMAXCommonAPI_WaitForIdleModeResponse;
    EVENT m_WiMAXCommonAPI_WaitForAbortResponse;
    EVENT m_WiMAXCommonAPI_WaitForLinkStatusResponse;
    THREAD m_WiMAXCommonAPI_DeviceInsertMonitor;
    THREAD m_WiMAXCommonAPI_PeriodicMIBGetThread;

    /* Unload event shared with Driver */
    EVENT m_DriverUnloadEvent;
    THREAD m_DriverUnloadEventThread;

    EVENT m_CAPIOMAThreadEvent;
    THREAD m_CAPIOMAThread;
    /*search parsing CAPL and RAPL parameters */
    THREAD m_WiMAXCommonAPI_NwSearch;

    /*MUTEX EEPROM Access Protection */
    MUTEX m_EEPROM_AcessMutex;

    /* MUTEX for FirmwareDownload */
    MUTEX m_FirmwareDownloadMutex;

    /* Shared MAC Mgmt Message location */
    SHARED_MEM m_shmMACMgmtMsg;

    /* MUTEX for m_shmMACMgmtMsg Shared memory read/writes */
    MUTEX m_MACMgmtMsg;
    MUTEX m_CloseDeviceRemovalEventMutex;

    volatile BYTE m_bKillCallbackThread;
    volatile BYTE m_bStatThreadRunning;
    volatile BYTE m_bMACMgmtMsgsEnabled;
    volatile BYTE m_bKillLoggerThread;

    volatile BYTE m_bKillNetEntryThread;
    volatile BYTE m_bKillSyncUpThread;
    volatile BYTE m_bKillNetSearchThread;
    volatile BYTE m_bIdleMode;

    volatile BYTE m_bKillMACAddressRespThread;
    volatile BYTE m_bKillShutDownRespThread;
    volatile BYTE m_bKillAntennaConfigRespThread;
    volatile BYTE m_bKillAntennaScanningRespThread;

    volatile BYTE m_bKillAntennaSelectionRespThread;
    volatile BYTE m_bSetWiMAXInfoReqFlag;
    volatile BYTE m_bKillWimaxCommonAPI_SearhThread;
    volatile BYTE m_bWimaxCommonAPI_SearhThreadRunning;

    volatile BYTE m_bKillWimaxCommonAPI_PeriodicMIBRequests;
    volatile BYTE m_bKillWimaxCommonAPI_CAPIOMAThread;
    volatile BYTE m_bUnUsed[2];

    volatile BOOL m_bFirmwareDownloadDone;
    /* For authentication */
    volatile BOOL m_bInitEEPROM;
    volatile BOOL m_bNetEntryInProgress;
    volatile BOOL m_bSyncUpInProgress;

    volatile BOOL m_bNetSearchInProgress;
    volatile BOOL m_bUnloadNotified;
    volatile BOOL m_bStdConfigInit;
    volatile BOOL m_bMACAddressWriteInProgress;

    volatile BOOL m_bModemInShutdown;
    volatile BOOL m_bFirmwareDownloadInProgress;
    volatile BOOL m_bAntennaConfigInProgress;
    volatile BOOL m_bAntennaScanningInProgress;

    volatile BOOL m_bAntennaSelectionInProgress;
    volatile BOOL m_bWIMAXOptionsSetInProgress;
    volatile BOOL m_bKillWIMAXOptionsSetInProgress;

    volatile BOOL m_bEnableMACLogging;
    volatile BOOL m_bLibraryOperatingOVerProxy;	/*Valid for embeded platforms where tgtxvi020 is running */
    volatile BOOL m_bBigEndianMachine;
    volatile BOOL m_bKillWiMAXCommonAPI_DeviceInsertMonitor;

    volatile BOOL m_bEnablePHYLogging;
    volatile BOOL m_bDeviceInitInProgress;
    volatile BOOL m_bKillDeviceInitThread;
    volatile BOOL m_bKillIdleModeWakeupThread;

    volatile BOOL m_bStatsPointerFlag;
    volatile BOOL m_bCommonAPIIfActive;
    volatile BOOL m_bKillDriverUnloadEvent;
    volatile BOOL m_bKillDeviceRemovalThread;

    volatile BOOL m_bNSPSearchInProgress;
    volatile BOOL m_bWIMAXOptionsAllSetInProgress;
    BOOL m_bUnused[3];

    THREAD m_IdleModeWakeupThread;
    MUTEX m_objMutex;
    ST_WIMAX_OPTIONS_ALL stWiMAXOptionAll;
    BCM_INTERFACE_TYPE eBCM_INTERFACETYPE;
    UINT32 u32OffsetForUnalignedAccessCorrection;
    ST_TRIGGER_INFO stTriggerInformation;
    UINT32 m_uiFirmwareHaltedFlag;
#endif
    ST_FLASH_PARAM_ID_WRITES astFlashMIBWrites[MAX_FLASH_MIB_UPDATES_DONE_AT_A_TIME];
    UINT32 u32FlashMulitpleMIBWriteCount;
    UINT32 u32NVMIsFlashFlag;
#ifdef WIN32
    UINT32 pPlaceHodlerForFSRedirectionStatus;
#endif
    volatile BOOL m_bControlMessageLogging;
    UINT32 m_uiControlMsgEnableMask;
    DEVICE_DRIVER_INFO stDriverInfo;
    UINT32 u32MaxRDMBufferSize;
    UINT32 u32NVM_StartOffset;
	UINT32 u32DeviceStatus;
    /*Multiple Mib Timer Request */
    THREAD m_MultipleMibThread;
    volatile UINT32 m_bMultipleMibThreadRunning;
    MULTIPLE_MIB_OBJECTS_NOTIFY_ST stMultiMIBNotification;

	ST_LOGGING stLogging;
    SHARED_MEM m_shmDynLogging;

} BECEEMWIMAX, *PBECEEMWIMAX;

typedef struct _ST_BCM_LIB_SHARED_EVENTS {
    UINT nTotalEvents;
    HANDLE arrhEvents[10];
} ST_BCM_LIB_SHARED_EVENTS, *PST_BCM_LIB_SHARED_EVENTS;

int ConvertToHostEndian(UCHAR * pucData, int iSizeOfData, int iType);
int BeceemWiMAX_DeviceUnloadShareEvent(PVOID pArg);

int BeceemWiMAX_Init(PVOID pArg);
int BeceemWiMAX_InitIRP(PVOID pArg);
int BeceemWiMAX_Cleanup(PVOID pArg);
HANDLE BeceemWiMAX_CreateDevice(PVOID pArg, const char *szDeviceName);
int BeceemWiMAX_CloseDevice(PVOID pArg);
BOOL BeceemWiMAX_IsValidHandle(PVOID pArg);
void *BeceemWiMAX_CallbackThreadFunc(void *pParam);

int BeceemWiMAX_KillAllThreads(PVOID pArg);
void BeceemWiMAX_KillCallbackThread(void *pParam);
int BeceemWiMAX_ValidateWiMaXMessage(PWIMAX_MESSAGE pMsg, UINT uiLength);
BOOL BeceemWiMAX_OpenEEPROM(PVOID pArg);
void BeceemWiMAX_KillDeviceRemovalThread(void *pParam);
void HexDump(void *pBuffer, UINT32 u32Length,PCHAR pFunName,UINT uLineNo);

HANDLE BeceemWiMAX_GetDeviceHandle(PVOID pArg);
int BeceemWiMAX_RegisterAppCallback(PVOID pArg, PVOID pAppCallback, PVOID pCallbackContext);
int BeceemWiMAX_CallAppCallback(PVOID pArg, PWIMAXMESSAGE pWMMsg);

/* SendMessage funcs */
int BeceemWiMAX_SendMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendNetworkSearchRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendNetEntryRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendLinkStateRequest(PVOID pArg);
int BeceemWiMAX_SendShutdownModeStatusRequest(PVOID pArg);
BOOL BeceemWiMAX_CheckForSyncFail(PVOID pArg, UCHAR ucLinkStatusError);
int BeceemWiMAX_GetLinkState(PVOID pArg, PLINK_STATE pLinkState);
BOOL BeceemWiMAX_IsSyncAcheived(PVOID pArg);

BOOL BeceemWiMAX_IsSyncInProgress(PVOID pArg);
BOOL BeceemWiMAX_IsNetworkSearchInProgress(PVOID pArg);
BOOL BeceemWiMAX_IsNetworkEntryInProgress(PVOID pArg);
VOID BeceemWiMAX_FindInterfaceType(PVOID pArg, UINT32 u32ChipID, UINT32 u32SysConfigReg);



int BeceemWiMAX_SendLinkDownRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendShutDownORHibernateRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendAppSSInforesponse(PVOID pArg, PWIMAX_MESSAGE pMsg);
int BeceemWiMAX_SendChipsetControlMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_ChipReset(PVOID pArg);
int BeceemWiMAX_SendFirmwareDnldIndication(PVOID pArg, int iStatus);
int BeceemWiMAX_FirmwareDownloadThreadFunc(void *pParam);
int BeceemWiMAX_StartFirmware(PVOID pArg);

int BeceemWiMAX_SendAuthMessage(PVOID pArg, PVOID pvMsg, UINT uiLength);
int BeceemWiMAX_AuthPacketTransferRequest(PVOID pArg, PWIMAX_MESSAGE pWMsg);
int BeceemWiMAX_EncryptPrivateRequest(PVOID pArg, PWIMAX_MESSAGE pWMsg);
int BeceemWiMAX_ProcessAuthMessageResponse(PVOID pArg);

int BeceemWiMAX_SendIdleModeRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendStatisticsRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);

void *BeceemWiMAX_StatsThreadFunc(void *pParam);
void BeceemWiMAX_KillStatsThread(void *pParam);


int BeceemWiMAX_GetStatisticsReal(PVOID pArg, UCHAR * pucStatsBuff, int iStatsBuffSize);
int BeceemWiMAX_ReadStatistics(PVOID pArg);
void UpdateBSID(stStatistics_SS * pStats);
int BeceemWiMAX_GetStatisticsAddress(PVOID pArg, UINT32 * puiStatAddress);
int BeceemWiMAX_ReadStatisticsLength(PVOID pArg, UINT32 uiStatsAddress, UINT32 * puiStatsLength);
int BeceemWiMAX_ReadStats(PVOID pArg, UINT32 uiStatsAddress, PUCHAR pucStats, UINT32 uiStatsLength);

int BeceemWiMAX_SendMACMgmtMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_PowerSavingModeStatusCheck(PVOID pArg, PVOID pvMsg);
int GetEEPROMDetailedInformation(PBECEEMWIMAX pWiMAX,
				 PST_SS_EEPROM_DETAILED_INFO pstEEPROMDetailedInfo);
int BeceemWiMAX_HalfMiniCardMessage(PVOID pArg, PVOID pMsg, UINT uiLength);
int GetCalibInfoFromCalibBaseAddress(PVOID pArg);
int BeceemWiMAX_LoggingRequestMessage(PBECEEMWIMAX pWiMAX, PWIMAX_MESSAGE pMsg, UINT32 uiLength);
int BeceemWiMAX_SelectControlMsgSubscription(PVOID pArg, PWIMAX_MESSAGE pMsg);
void BeceemWiMAX_CheckAndSwapStatistics(PBECEEMWIMAX pWiMAX, stStatistics_SS * pStatistics);

/* MIB API */
int GetDlUlZoneInformation(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			   UINT * puiOutDataLen);
int BeceemWiMAX_AddConfigMIBMember(UINT32 uiAttribute, UINT32 uiOffset, UINT32 uiFieldSize);
int BeceemWiMAX_AddStatMIBMember(UINT32 uiAttribute, UINT32 uiOffset, UINT32 uiFieldSize);
int BeceemWiMAX_AddStatMIBMemberTable(UINT32 uiAttribute, UINT32 uiTableOffset,	/* offset of table */
				      UINT32 uiTableRowSize,	/* size of single row */
				      UINT32 uiNumRows,	/* num of rows */
				      UINT32 uiTableEntryOffset,	/* offset of table entry */
				      UINT32 uiTableEntrySize	/* size of this field */
    );
VOID BeceemWiMAX_InitializeMIBTable(PVOID);

int BeceemWiMAX_MIBReqMessage(PVOID pVoid, PWIMAX_MESSAGE pMsg, UINT uiLength);
int ValidateOID(UINT uiOID);
int BeceemWiMAX_ProcessMIBGetReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMsg);
int BeceemWiMAX_ProcessMIBSetReq(PVOID pVoid, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_MIBWriteConfig(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj);
int ReadBCMPropreitaryConfig(int iOffset, UCHAR * pucOutData, UINT * puiOutDataLen);
int ReadStandardStats(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * aucOutBuff, int *piOutLen);
int ReadStandardHostStats(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * aucOutBuff,
			  int *piOutLen);
int ReadBCMPropreitaryStats(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * aucOutBuff,
			    int *piOutLen);
int GetMIBData(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData, UINT * puiOutDataLen);
int GetMRUData(PBECEEMWIMAX pWiMAX, UCHAR * pucOutData, UINT * puiOutDataLen);
int GetRxTxPacketHistorgram(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			    UINT * puiOutDataLen);
int GetPacketsOrBytesDroppedPerSF(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
				  UINT * puiOutDataLen);
int SearchStatsData(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
		    UINT * puiOutDataLen);
int ReadStandardConfigMIB(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * aucOutBuffer,
			  int *piOutLen);
int ReadStdHostStatsFromIOCTL(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			      UINT * puiOutDataLen);
int GetTypeSpecificData(UCHAR * pucOutData, const UCHAR * pucData, UINT uiSize, UINT uiType);
int WriteStandardStatsMIB(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj);

int BeceemWiMAX_MACAddress(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_WriteMACAddress(PVOID pArg, UCHAR * pucMACAddress);
int GetNeighbourInfoFromBSLogger(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
				 UINT * puiOutDataLen);
int GetPduRcvStatInformation(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			     UINT * puiOutDataLen);
int GetPduSndStatInformation(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			     UINT * puiOutDataLen);
int GetMcsStatInformation(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			  UINT * puiOutDataLen);
int GetDlZoneStatInformation(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			     UINT * puiOutDataLen);
int GetTriggerStatusInfo(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
			 UINT * puiOutDataLen);
int GetNumberOfControlMessagesDropped(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData,
				      UINT * puiOutDataLen);
void BeceemWiMAX_CheckAndSwapMIBData(PBECEEMWIMAX pWiMAX, PUCHAR pucData, INT32 iOID);

/* Multiple MIB Get/Set related */
int BeceemWiMAX_ProcessMultipleMIBGetReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECTS pMsg, UINT32 bFrmThread);
int BeceemWiMAX_ProcessMultiplePeriodicIndicationReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECTS_TIMER pMsg);
int BeceemWiMAX_ProcessMultipleMIBSetReq(PVOID pVoid, PWIMAX_MESSAGE pMsg, UINT uiLength);

int BeceemWiMAX_SendServiceFlowMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendSleepModeMsg(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);

/*Sleep mode related*/
int BeceemWiMAX_ProcessSleepModeResponse(PVOID pArg, UINT uiNumBytesRecvd);
void ConvertSLeepModeEndianess(ST_SLEEPMODE_MSG * pstSleepMode, int iType);

/* Antenna Switching & Selection related */
int BeceemWiMax_AntennaReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);

/* idle mode related */
void ConvertIdleModeEndianess(ST_IDLEMODE_INDICATION * pstIdleMode);

/* Antenna scanning & switching related */
void ConvertAntennaConfigRespEndianess(ST_GPIO_SETTING_INFO * pstGpioSettingInfo);
void ConvertAntennaScanningRespEndianess(ST_RSSI_INFO_IND * pstRssiInfoInd);

UINT GetSystemDirectoryEx(char *lpszBuffer, UINT uiSize);
int OpenSysConfigFile(FILE ** fp, const char *szMode);
int CloseSysConfigFile(FILE ** fp);

int BeceemWiMAX_SendGPIOMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int GetMACAddress(PVOID pArg, UCHAR * pucMACAddress);

/* rdm & wrm */
int BeceemWiMAX_wrmBulk(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iSize);
int BeceemWiMAX_wrmBulkIdle(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iSize);
int BeceemWiMAX_wrmbulk_ioctlhandler(PVOID pArg, UINT uiAddr, UCHAR * pucValue, INT iBytes2Write);
int BeceemWiMAX_wrmbulkIdle_ioctlhandler(PVOID pArg, UINT uiAddr, UCHAR * pucValue, INT iBytes2Write);

int BeceemWiMAX_wrm(PVOID pArg, UINT uiAddr, UINT uiValue);
int BeceemWiMAX_wrmIdle(PVOID pArg, UINT uiAddr, UINT uiValue);

int BeceemWiMAX_rdm(PVOID pArg, UINT uiAddr, UINT * puiOutVal, INT iBytesTobeRead);
int BeceemWiMAX_rdmIdle(PVOID pArg, UINT uiAddr, UINT * puiOutVal, INT iBytesTobeRead);
int BeceemWiMAX_rdmBulk(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iBytesTobeRead);
int BeceemWiMAX_rdmBulkIdle(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iBytesTobeRead);
UINT BeceemWiMAX_rdmsf(PVOID pArg, UINT uiAddr);

int BeceemWiMAX_eepromrdmBulk(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iBytesTobeRead);
int BeceemWiMAX_eepromwrmBulk(PVOID pArg, UINT uiAddress, UCHAR * pucValueArray, INT iSize);

/* Logger */
int BeceemWiMAX_SendLogReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_LaunchLoggerThread(PVOID pArg);
void *BeceemWiMAX_LoggerThreadFunc(void *pParam);
int BeceemWiMAX_ProcessAndSendLoggerData(void *pArg, PWIMAXLOGGER pLogger, int iPrevIndex);
int BeceemWiMAX_KillLoggerThread(void *pvParam);
int BeceemWiMAX_SendLogStopReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);

BOOL BeceemWiMAX_GetDLLVersion(UINT * puiVersion);

/* Vendor specific info */
int BeceemWiMAX_SendVendorSpecificInfoMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_ProcessVSAReadParam(PVOID pArg, PVENDOR_SPECIFIC_PARAM pstVendorSpecificParam);
int BeceemWiMAX_ProcessVSAWriteParam(PVOID pArg, PVENDOR_SPECIFIC_PARAM pstVendorSpecificParam);
int BeceemWiMAX_ProcessVSARegionSizeParam(PVOID pArg);

/* Process Response */
void BeceemWiMAX_NotificationRoutine(PVOID pContext, DWORD dwErrorCode,	/* completion code */
				     DWORD dwBytesTransfered	/* number of bytes transferred */
    );

int BeceemWiMAX_ProcessUnloadDevResponse(PVOID pArg);
int BeceemWiMAX_SendAppDeviceUnload(PVOID pArg);
int BeceemWiMAX_ProcessLinkControlResponse(PVOID pArg, UINT uiNumBytesRecvd);
int BeceemWiMAX_SendAppLinkSyncStatus(PVOID pArg, USHORT usSubType, PST_SYNC_STATUS pstSyncStatus);
int BeceemWiMAX_GetLinkErrorCode(UCHAR ucLinkFailReason);
int BeceemWiMAX_SendAppIdleresponse(PVOID pArg, PUCHAR pucPayloadData);
int BeceemWiMAX_GetBaseInfoEx(PVOID pArg, PBSINFOEX pBaseInfo, int iStatus);
int BeceemWiMAX_SendAppLinkStatus(PVOID pArg, UINT uiLinkStatus, UCHAR ucLinkErrorCode,
				  PVENDOR_SPECIFIC_INFO pVendorInfo);
int BeceemWiMAX_SendAppWakeupStatus(PVOID pArg, UCHAR ucLinkErrorCode, PBSLIST pstBSInfo);
int BeceemWiMAX_ProcessMgmtInfoResponse(PVOID pArg, UINT uiNumBytesRecvd);

int BeceemWiMAX_ProcessServiceFlowResponse(PVOID pArg, UINT uiNumBytesRecvd);
int BeceemWiMAX_SetRssiCinrResponse(PVOID pArg, PUCHAR pPayload);
int BeceemWiMAX_ProcessControlMsgResponse(PVOID pArg, DWORD dwBytesTransfered);

/* Netwok entry */
PVOID BeceemWiMAX_NetEntryThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillNetEntryThread(PVOID pArg);
VOID BeceemWiMAX_KillCommonAPI_NwSearchThread(PVOID pArg);

/* Sync Up */
PVOID BeceemWiMAX_SyncUpThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillSyncUpThread(PVOID pArg);

/* Netwotk Search */
int BeceemWiMAX_NetworkSearchRequest(PVOID pArg, PST_NETWORK_SEARCH_PARAMS pstNetSearchParams);
int BeceemWiMAX_NetworkNSPSearchRequest(PVOID pArg, PVOID pPayload);
int BeceemWiMAX_SetLowPowerScanModeRequest(PVOID pArg, PVOID pPayload);

PVOID BeceemWiMAX_NetSearchThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillNetSearchThread(PVOID pArg);
VOID BeceemWiMAX_KillDeviceInsertThread(PVOID pArg);
int BeceemWiMAX_NetworkSearchAbort(PVOID pArg);
int BeceemWiMAX_NetworkSearchResults(PVOID pArg);
int BeceemWiMAX_SendAppNetSearchresponse(PVOID pArg, PBSLIST pstBsList);
int BeceemWiMAX_SendAppNetSearchresponsePerAntenna(PVOID pArg, PST_BSLIST_PER_ANTENNA pstBsList);
int BeceemWiMAX_SendAppLowPowerModeResponse(PVOID pArg, PST_SCAN_CONFIG_PARAM pScanConfigParam,
					    INT status);
int BeceemWiMAX_CaplBasedSearchRequest(PVOID pArg, PST_CAPL_BASED_SEARCH_PARAMS pstCaplSearchParams,USHORT usSubType);
int BeceemWiMAX_HandleCaplResponse(PVOID pArg, PST_CAPL_SEARCH_RSP pstCaplRsp);
int BeceemWiMAX_HandleNSPSearchResponse(PVOID pArg, PBSNSPLIST pBSInfoList);

/* Antenna Config Req */
PVOID BeceemWiMAX_AntennaConfigRespThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillAntennaConfigRespThread(PVOID pArg);
int BeceemWiMAX_SendAppAntennaConfigresponse(PVOID pArg, PST_GPIO_SETTING_INFO pstGpioSettingInfo);

/* Antenna Scanning Req */
PVOID BeceemWiMAX_AntennaScanningRespThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillAntennaScanningRespThread(PVOID pArg);
int BeceemWiMAX_SendAppAntennaScanningresponse(PVOID pArg, PST_RSSI_INFO_IND pstRssiInfoInd);

/* Antenna Selection Req */
PVOID BeceemWiMAX_AntennaSelectionRespThreadFunc(PVOID pParam);
VOID BeceemWiMAX_KillAntennaSelectionRespThread(PVOID pArg);
int BeceemWiMAX_SendAppAntennaSelectionresponse(PVOID pArg, UINT * puiAntSelResp);

/* Kill Device Init Thread */
VOID BeceemWiMAX_KillDeviceInitThread(PVOID pArg);
VOID BeceemWiMAX_KillDeviceUnloadEventThread(PVOID pArg);

/* Device Config requests*/
int SendDeviceConfigRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SendResetStatistics(PVOID pArg, UINT uiEnableReset);
int BeceemWiMAX_ToggleResetStatistics(PVOID pArg, UINT uiEnableReset);
int BeceemWiMX_SetBasebandTransmitPower(PVOID pArg, UINT uiTxReferencePowerInDB);
int BeceemWiMX_SendBasebandTemperature(PVOID pArg);
int BeceemWiMX_GetBasebandTemperature(PVOID pArg, UINT * puiTemperature);
int SetPowerLevel(PVOID pArg, int iPowerLevel);
void ConvertAddIndicationEndianess(stLocalSFAddIndication * pDSAInd, int iType);
void ConvertAddRequestEndianess(stLocalSFAddRequest * pDSAReq, int iType);
void QuickSort(UINT numbers[], int array_size);
int CheckForCalibrationStatus(PVOID pArg, UINT uiChipID);
int SendDeviceConfigRequest(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_NetworkSearchSetThresholdRssiCinr(PVOID pArg, PVOID pPayload);

#ifdef WIN32
BOOL ReloadDevice(LPTSTR HardwareId);
#endif
/*To increment/Decrement the MAC mgmt message enabled count*/
int ToggleMACMgmtMsg(PVOID pArg, BOOL bToggleMsg);
BOOL GetMACMgmtMsgStatus(PVOID pArg);

/*MAC address write */
int SendMACAddressRequest(PVOID pArg, UCHAR * pucMACAddress);
PVOID BeceemWiMAX_MACAddressWriteThread(PVOID pParam);

PVOID BeceemWiMAX_IdleModeWakeupThread(PVOID pArg);

VOID BeceemWiMAX_KillMACAddressWriteThread(PVOID pArg);
int BeceemWiMAX_SendMACAddressWriteResponse(PVOID pArg, PUCHAR pPayload);
int BeceemWiMAX_ProcessStatusResponseMessage(PVOID pArg,DWORD dwBytesTransferred);

/*MAC Logger notifications*/
int BeceemWiMAX_ProcessMACLOggingNotification(PVOID pArg, DWORD dwBytesTransferred);

/*Phy Logger notifications*/
int BeceemWiMAX_ProcessCIDLoggingNotification(PVOID pArg, DWORD dwBytesTransferred);

/* Shutdown Response */
PVOID BeceemWiMAX_ShutDownRespThread(PVOID pParam);
VOID BeceemWiMAX_KillShutDownRespThread(PVOID pArg);

/*EEPROM ACCESS FROM HOST FUNCTIONS*/
INT BeceemWiMAX_eepromwrm(UINT uiAddr, INT uiValue);
INT BeceemWiMAX_eepromrdm(UINT uiAddr, INT * puiOutVal);
int BeceemWiMAX_HostEEPROMWrite(PVOID pArg, unsigned int u32Offset, unsigned int u32Data);
int BeceemWiMAX_HostEEPROMBulkWrite(PVOID pArg, unsigned int u32Offset, UCHAR * pu8Buffer, INT u32Size);
int BeceemWiMAX_HostEEPROMRead(PVOID pArg, unsigned int u32Offset, UINT32 * pu32SrcData);

//Firmware download 
int BeceemWiMAX_FirmwareDownload(PVOID pArg);
int BeceemWiMAX_StartReq(PVOID pArg);
int BeceemWiMAX_StopReq(PVOID pArg);
int SendDownloadRequest(PVOID pArg, PFIRMWARE_INFO pFirmwareDownload);
BOOL OpenFileForDwnload(PVOID * pArg);
BOOL CloseDownloadFile(PVOID * pArg);


int BeceemWiMAX_ControlMACLogging(PVOID pArg, PVOID pvMsg, UINT uiLength);
int BeceemWiMAX_ControlPHYLogging(PVOID pArg, PVOID pvMsg, UINT uiLength);
int BeceemWiMAX_ControlDrvLogging(PVOID pArg, PVOID pvMsg, UINT uiLength);

/* Wimax options write*/
int BeceemWiMAX_SendWimaxOptionsRequest(PVOID pArg, PWIMAX_MESSAGE pMsg);
int BeceemWiMAX_HandleWiMAXOptionsReq(PVOID pArg, PWIMAX_MESSAGE pMsg);
int BeceemWiMAX_SendWiMAXGetAllMessage(PVOID pArg);
int BeceemWiMAX_SendWiMAXSetAllMessage(PVOID pArg, PWIMAX_MESSAGE pMsg);
INT BeceemWiMAX_SendAppWiMAXOptionsAll(PVOID pArg, PUCHAR pPayload, UCHAR uchOptionSpecifer,
				       INT32 i32Status, USHORT usSubType);

int BeceemWiMAX_SendWimaxOptionsResponse(PVOID pArg, PUCHAR pPayload, UINT uiLength);
PVOID BeceemWiMAX_WiMAXOptionsMonitor(PVOID pArg);
PVOID BeceemWiMAX_WiMAXOptionsAllMonitor(PVOID pArg);
VOID BeceemWiMAX_KillWiMAXOptionsThread(PVOID pArg);

PVOID BeceemWiMAX_DeviceRemovalThread(PVOID pArg);
PINT BeceemWiMAX_DeviceInitThreadFunc(PVOID pArg);
PINT BeceemWiMAX_DeviceUnloadEventThreadFunc(PVOID pArg);

int BeceemWiMAX_ChipResetStatus(PVOID pArg);
int BeceemWiMAX_ConfigFileReDownload(PVOID pArg,PUCHAR pPayload);

/*Handling Endianness issue while operating over proxy*/
USHORT Swap_Short(USHORT i);
int Swap_Int(int i);
ULONG64 SWAP_Long64(ULONG64 i);
int ConvertToLittleEndianForProxy(PVOID pArg, UCHAR * pucData, int iSizeOfData, int iType);
UINT ConvertLongEndian(UINT uiValue, int iType);
USHORT ConvertShortEndian(USHORT usValue, int iType);
UINT SWAP_ENDIAN32(UINT data);
VOID BeceemWiMAX_KillIdleModeWakeupThread(PVOID pArg);


INT BeceemWiMAX_ValidateRdmAddress(UINT32 uiAddr);

/* OMA DM related */
int GetOMADMData(PBECEEMWIMAX pWiMAX, POMA_DM_NODE_OFFSETINFO pOMADMObj, UCHAR * pucOutData,
		 UINT * puiOutDataLen);

int GetSecData(PBECEEMWIMAX pWiMAX, PSEC_OBJECT_INFO pSecObj, UCHAR * pucOutData, UINT * puiOutDataLen);
int BeceemWiMAX_ProcessOMADMGetReq(PBECEEMWIMAX pWiMAX, POMADM_OBJECT_INFO pOMADMObj);
INT BeceemWiMAX_GetIMEIInfo(PBECEEMWIMAX pWiMAX, PUCHAR pData);
int BeceemWiMAX_ProcessSecGetReq(PBECEEMWIMAX pWiMAX, PSEC_OBJECT_INFO pSecObj);
int ValidateID(UINT uiOID);
int BeceemWiMAX_OMADMReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_SecReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_ProcessOMADMSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_ProcessSecSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
int BeceemWiMAX_NetworkSearchUsingOMADM(PVOID pArg);

void GetOMADM_Offset(OMA_DM_TREE_LEVEL * pTree, UINT32 u32Levels,
		     POMA_DM_NODE_OFFSETINFO pOMADMOffsetInfo, UINT32 u32BufferSize,
		     UCHAR bValidateBufferSize);


int BeceemWiMAX_ProcessOMADMGetParamSize(PBECEEMWIMAX pWiMAX, POMADM_PARAMETER_INFO pOMADMParamInfo);
INT BeceemWiMAX_ProcessOMADMVersionCheck(PBECEEMWIMAX pWiMAX,
					 POMADM_PARAMETER_VERSION_CHECK pOMADMVersionCheck);

INT GetRealmInfo(PBECEEMWIMAX pWiMAX, UCHAR * pucOutData, UINT * puiOutDataLen);
int BeceemWiMAX_OMADMCalibReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
VOID BeceemWiMAX_KillCAPIPeriodicMIBGetRequestThread(PVOID pArg);
VOID BeceemWiMAX_KillCAPIOMAMonitorThread(PVOID pArg);
PVOID GetDeviceDataMRUPointer();
UINT32 BeceemWiMAX_SendNSPLinkupRequest(PVOID pArg, PWIMAX_MESSAGE pMsg);
INT BeceemWiMAX_SendAppModemStatus(PVOID pArg, PST_MODEM_STATE_INFO pstModemStatus);
INT BeceemWiMAX_ProcessOMADMTreeUpdate(PBECEEMWIMAX pWiMAX, PST_OMA_DM_TREE_INFO pOMADMTreeInfo);
INT BeceemWiMAX_DownloadOMADMTree(PBECEEMWIMAX pWiMAX, PUCHAR pOMADMTree, UINT32 u32Size);
int BeceemWiMAX_ResetFlags(PVOID pArg);
INT BeceemWiMAX_GetNVMSize(PUINT puiNVMSize);
INT BeceemWiMAX_NVMrdm(UINT32 uiOffset, PUCHAR pBuffer, UINT32 uiSize);
INT BeceemWiMAX_NVMwrm(UINT32 uiOffset, PUCHAR pBuffer, UINT32 uiSize);
BOOL BeceemWiMAX_EnsureEEPROMInitialized(PVOID pArg);
int SetMRUData(PBECEEMWIMAX pWiMAX, PST_MRU_ADD_INFO pMruAddInfo);
int GetHARQRetriesExceeded(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
			   UCHAR * pucOutData, UINT * puiOutDataLen);
int ReadDataFromstatstoOffset(PBECEEMWIMAX pWiMAX, PUINT pu32Offset1, PUINT pu32Offset2);
int GetDLHARQsInError(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
		      UCHAR * pucOutData, UINT * puiOutDataLen);
int GetHARQRetryInError(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
			UCHAR * pucOutData, UINT * puiOutDataLen);
int CacheMulitpleMIBData(PBECEEMWIMAX pWiMAX, UINT8 * pBuffer, UINT32 u32Size, UINT32 u32MIBValue);
void CheckIfNVMIsFlash(PBECEEMWIMAX pWiMAX);
int GetNumTransmittedAndReceivedPkts(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
				     UCHAR * pucOutData, UINT * puiOutDataLen);
int BeceemWiMAX_neighbour_stats_wrapper(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
					UCHAR * pucOutData, UINT * puiOutDataLen);

INT BeceemWiMAX_FlashAPIHandler(PBECEEMWIMAX pWiMAX, PVOID pPayloadData, UINT uiLength);
INT BeceemWiMAX_GetFlashVersionInfo(PBECEEMWIMAX pWiMAX, PDYNAMIC_FLASH_CS_INFO pFlashControlSection,
				    UINT uiLength);
INT BeceemWiMAX_ReadFlashSectionReq(PBECEEMWIMAX pWiMAX, PDYNAMIC_FLASH_READWRITE_ST pstFlashReadsect,
				    UINT uiLength);
INT BeceemWiMAX_WriteFlashSectionReq(PBECEEMWIMAX pWiMAX, PDYNAMIC_FLASH_READWRITE_ST pstFlashReadsect,
				     UINT uiLength);
INT BeceemWiMAX_GetflashSectionBitMap(PBECEEMWIMAX pWiMAX, PST_PFLASH2X_BITMAP pstFlashSectionBitMap,
				      UINT uiLength);
INT BeceemWiMAX_SetActiveSection(PBECEEMWIMAX pWiMAX, PE_DYNAMIC_SECTION_VAL pDynamicSectionVal,
				 UINT uiLength);
INT BeceemWiMAX_SelectDSD(PBECEEMWIMAX pWiMAX, PE_DYNAMIC_SECTION_VAL pDynamicSectionVal,
			  UINT uiLength);
INT BeceemWiMAX_IdentifyActiveSection(PBECEEMWIMAX pWiMAX, PUCHAR pData, UINT uiLength);
INT BeceemWiMAX_CopySection(PBECEEMWIMAX pWiMAX, PST_FLASH2X_COPY_SECTION pstCopySection,
			    UINT uiLength);
INT BeceemWiMAX_FlashRawRead(PBECEEMWIMAX pWiMAX, PST_FLASH_RAW_ACCESS pstFlashRawAccess);
BOOL BeceemWiMAX_IsSafeToAccessContextTabel();
UINT32 BeceemWiMAX_GetTimeSinceNetEntry(PVOID pArg, PWIMAX_MESSAGE pMsg);
VOID BeceemWiMax_GetDriverInfoSettings(PBECEEMWIMAX pWiMAX);
VOID BeceemWiMAX_CheckAndSwapFlashInfo(PBECEEMWIMAX pWiMAX, PUCHAR pucFlashControlSection,
				       PUCHAR pucFlash_2X_CS_Info);
int BeceemWiMAX_GetStatsBufferChunkFrmDDR(PBECEEMWIMAX pWiMAX);
void BeceemWiMAX_KillMultipleMibThread(void *pParam);
INT BeceemWiMAX_StatsUpdate();
BOOL DynLogging_InitShmRecord(void);
BOOL DynLogging_CleanShmRecord(void);
int DynLogging_GetCountValidProcessShmLogRec(void);
INT BeceemWiMAX_DynLoggingHandler(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiMsgLen);
BOOLEAN DynLoggingAttr_Set(PUCHAR payload);
BOOLEAN DynLoggingAttr_Get(PUCHAR payload);
BOOL DebugPrint_Print(PVOID pArg, unsigned char iCode, INT iPath, const char *szFmt, ...);
PST_SHM_LOG_REC GetThisProcessShmLogRec(PBECEEMWIMAX pstBeceemWiMAX);

int BeceemWiMAX_GetDeviceIoctlFailureReason();


INT BeceemWiMAX_Resync_CID_LogHandler(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);
INT BeceemWiMAX_Resync_CID_Response(PVOID pArg, PUCHAR pPayload,DWORD dwBytesTransferred,USHORT usSwStatus);

INT BeceemWiMAX_ProximitySensorHandler(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength);

INT BeceemWiMAX_ProximitySensor_Response(PVOID pArg, PUCHAR pPayload,UINT uiBytesTransferred);
/*
 * below function provides name to index association for creating SyS V Semaphore. It should be in the same order to keep backward compatibility.
 * When a new mutex is added should be added at the end.
*/
int get_id_for_mutexName(const char *pMutexName);
#endif				/* BECEEM_WIMAX */
