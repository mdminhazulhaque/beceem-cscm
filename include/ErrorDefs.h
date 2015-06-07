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
 * Description	:	Beceem Error Definitions
 * Author		:
 * Reviewer		:
 *
 */


#ifndef WIMAX_ERROR_DEFS_H
#define WIMAX_ERROR_DEFS_H


#define SUCCESS							0
/**
*	These are the definitions of start numbers for different modules
*	And these can be reloacted as module error codes are being added.
**/

#define ERRORS_GENERAL_BASE						(-1)		
#define ERRORS_BECEEMWIMAX_BASE					(-10)		
#define ERRORS_THREAD_BASE						(-80)		
#define ERRORS_SYNC_OBJECTS_BASE				(-90)		
#define ERRORS_DEVCIE_IO_BASE					(-110)		
#define ERRORS_SOCKET_BASE						(-130)		
#define ERRORS_SHAREDMEM_BASE					(-150)		

#define ERRORS_GENERAL_BASE_EX						(-300)		
#define ERRORS_BECEEMWIMAX_BASE_EX					(-500)		
#define ERRORS_THREAD_BASE_EX						(-600)		
#define ERRORS_SYNC_OBJECTS_BASE_EX					(-900)		
#define ERRORS_DEVCIE_IO_BASE_EX					(-1000)		
#define ERRORS_SOCKET_BASE_EX						(-1200)		
#define ERRORS_SHAREDMEM_BASE_EX					(-1300)	

/**
*	Error codes which are common to all modules
**/

#define ERROR_INVALID_BUFFER					(ERRORS_GENERAL_BASE-0)
#define ERROR_INVALID_VALUE						(ERRORS_GENERAL_BASE-1)
#define ERROR_INVALID_BUFFER_LEN				(ERRORS_GENERAL_BASE-2)
#define ERROR_BUFFER_INSUFFICIENT				(ERRORS_GENERAL_BASE-3)
#define ERROR_INVALID_ARGUMENT					(ERRORS_GENERAL_BASE-4)
#define ERROR_MALLOC_FAILED						(ERRORS_GENERAL_BASE-5)
#define ERROR_HANDLE_INVALID					(ERRORS_GENERAL_BASE-6)
#define ERROR_DEVICE_OPENED_FOR_REMOTE_OPERATION (ERRORS_GENERAL_BASE-7)
#define ERROR_API_NOT_SUPPORTED					(ERRORS_GENERAL_BASE-8)
#define ERROR_COMMON_API_INTERFACE_ACTIVE		(ERRORS_GENERAL_BASE-9)
#define ERROR_FILE_OPERATION					(ERRORS_GENERAL_BASE_EX-0)
#define ERROR_SIGNAL_OPERATION					(ERRORS_GENERAL_BASE_EX-1)


/**
*	Error codes in BeceemWiMAX module
**/

#define ERROR_INVALID_NOTIFY_ROUTINE			(ERRORS_BECEEMWIMAX_BASE-0)
#define ERROR_INVALID_STATS_LEN					(ERRORS_BECEEMWIMAX_BASE-1)
#define ERROR_NO_STATISTIC_RESPONSE				(ERRORS_BECEEMWIMAX_BASE-2)
#define ERROR_INVALID_WIMAX_MESSAGE				(ERRORS_BECEEMWIMAX_BASE-3)
#define ERROR_INVALID_STATISTICS				(ERRORS_BECEEMWIMAX_BASE-4)
#define ERROR_RESET_FAILED						(ERRORS_BECEEMWIMAX_BASE-5)
#define ERROR_FIRMWAREDNLD_FAILED				(ERRORS_BECEEMWIMAX_BASE-6)
#define ERROR_INVALID_CALLBACK_ADDRESS			(ERRORS_BECEEMWIMAX_BASE-7)
#define ERROR_INVALID_STATSPTR_RESPONSE			(ERRORS_BECEEMWIMAX_BASE-8)
#define ERROR_FIRMWARE_DOWNLOAD_INPROGRESS		(ERRORS_BECEEMWIMAX_BASE-9)
#define ERROR_SYSCONFIG_GET_FAILED				(ERRORS_BECEEMWIMAX_BASE-10)
#define ERROR_SYSCONFIG_OPEN_FAILED				(ERRORS_BECEEMWIMAX_BASE-11)
#define ERROR_INVALID_GPIO_NUMBER				(ERRORS_BECEEMWIMAX_BASE-12)
#define ERROR_LOGGER_ENABLED					(ERRORS_BECEEMWIMAX_BASE-13)
#define ERROR_INVALID_LOGGER_TYPE				(ERRORS_BECEEMWIMAX_BASE-14)
#define ERROR_INVALID_LOGGER_INDEX				(ERRORS_BECEEMWIMAX_BASE-15)
#define ERROR_INVALID_STATADDRESS				(ERRORS_BECEEMWIMAX_BASE-16)
#define ERROR_AUTH_CRC_MISMATCH					(ERRORS_BECEEMWIMAX_BASE-17)
#define ERROR_NET_ENTRY_INPROGRESS				(ERRORS_BECEEMWIMAX_BASE-18)
#define ERROR_SYNC_UP_INPROGRESS				(ERRORS_BECEEMWIMAX_BASE-19)
#define ERROR_INVALID_MIB_OBJ_SIZE				(ERRORS_BECEEMWIMAX_BASE-20)
#define ERROR_MIB_OBJ_READONLY					(ERRORS_BECEEMWIMAX_BASE-21)
#define ERROR_INVALID_MIB_OBJ_ID				(ERRORS_BECEEMWIMAX_BASE-22)
#define ERROR_INVALID_SUBTYPE					(ERRORS_BECEEMWIMAX_BASE-23)
#define ERROR_EEPROM_NOTINITIALIZED				(ERRORS_BECEEMWIMAX_BASE-24)
#define ERROR_RDM_FAILED        				(ERRORS_BECEEMWIMAX_BASE-25)
#define ERROR_INVALID_SF_SUBTYPE				(ERRORS_BECEEMWIMAX_BASE-26)
#define ERROR_LIB_INITIALIZATION				(ERRORS_BECEEMWIMAX_BASE-27)
#define ERROR_MIB_OBJECT_NOTFOUND				(ERRORS_BECEEMWIMAX_BASE-28)
#define ERROR_INVALID_LOGGER_LENGTH				(ERRORS_BECEEMWIMAX_BASE-29)
#define ERROR_NET_SEARCH_INPROGRESS				(ERRORS_BECEEMWIMAX_BASE-30)
#define ERROR_SS_IS_INSYNC						(ERRORS_BECEEMWIMAX_BASE-31)
#define ERROR_INVALID_MIB_ATTRIBUTE				(ERRORS_BECEEMWIMAX_BASE-32)
#define ERROR_EEPROM_WRITE_FAILED				(ERRORS_BECEEMWIMAX_BASE-33)
#define ERROR_FREQUENCY_LIST_NOT_SORTED			(ERRORS_BECEEMWIMAX_BASE-34)
#define ERROR_TX_POWER_OUT_OF_RANGE				(ERRORS_BECEEMWIMAX_BASE-35)
#define ERROR_TX_POWER_SET_FAILED				(ERRORS_BECEEMWIMAX_BASE-36)
#define ERROR_WRITE_MAC_ADDRESS_FAILED			(ERRORS_BECEEMWIMAX_BASE-37)
#define ERROR_SS_IS_LINKED_UP					(ERRORS_BECEEMWIMAX_BASE-38)
#define ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS		(ERRORS_BECEEMWIMAX_BASE-39)
#define ERROR_INVALID_SLEEP_MODE_TYPE   		(ERRORS_BECEEMWIMAX_BASE-40)
#define ERROR_IDLE_MODE_INACTIVITY_TIMER_SET_FAILED	(ERRORS_BECEEMWIMAX_BASE-41)
#define ERROR_SHUTDOWN_MODE_RESP_TIMEOUT		(ERRORS_BECEEMWIMAX_BASE-42)
#define ERROR_INVALID_ANTENNA_MODE_TYPE			(ERRORS_BECEEMWIMAX_BASE-43)
#define ERROR_WIMAX_OPTIONS_SET_IN_PROGRESS		(ERRORS_BECEEMWIMAX_BASE-44)
#define	ERROR_ANTENNA_CONFIG_RESP_TIMEOUT		(ERRORS_BECEEMWIMAX_BASE-45)
#define	ERROR_ANTENNA_SCANNING_RESP_TIMEOUT		(ERRORS_BECEEMWIMAX_BASE-46)
#define	ERROR_ANTENNA_SELECTION_RESP_TIMEOUT	(ERRORS_BECEEMWIMAX_BASE-47)
#define	ERROR_SET_WIMAX_OPTIONS_RESP_FAILED		(ERRORS_BECEEMWIMAX_BASE-48)
#define	ERROR_DEVICE_NOT_IN_IDLE_MODE			(ERRORS_BECEEMWIMAX_BASE-49)
#define	ERROR_INVALID_ADDRS_INPUT				(ERRORS_BECEEMWIMAX_BASE-50)
#define ERROR_MODEM_IN_SHUT_DOWN_MODE			(ERRORS_BECEEMWIMAX_BASE-51)
#define ERROR_MODEM_IN_IDLE_MODE				(ERRORS_BECEEMWIMAX_BASE-52)
#define ERROR_EEPROM_INIT_FAILED		   		(ERRORS_BECEEMWIMAX_BASE-53)
#define ERROR_EEPROM_NOT_ACCESSIBLE				(ERRORS_BECEEMWIMAX_BASE-54)
#define ERROR_OMADM_OBJECT_NOTFOUND				(ERRORS_BECEEMWIMAX_BASE-55)
#define ERROR_SEC_OBJECT_NOTFOUND				(ERRORS_BECEEMWIMAX_BASE-56)
#define OMA_DM_ERROR_OUT_OF_BOUND_ARRAY_ACCESS  (ERRORS_BECEEMWIMAX_BASE-57)
#define OMA_DM_ERROR_READ_BUFFER_TOO_SMALL      (ERRORS_BECEEMWIMAX_BASE-58)
#define OMA_DM_ERROR_WRITE_BUFFER_TOO_LARGE     (ERRORS_BECEEMWIMAX_BASE-59)
#define OMA_DM_ERROR_INVALID_VERSION			(ERRORS_BECEEMWIMAX_BASE-60)
#define ERROR_NO_MIB_SUPPORT					(ERRORS_BECEEMWIMAX_BASE-61)
#define ERROR_OMA_DM_INIT_FALIED				(ERRORS_BECEEMWIMAX_BASE-62)
#define ERROR_SEC_OBJECT_ZERO_LENGTH			(ERRORS_BECEEMWIMAX_BASE-63)
#define ERROR_MIB_FOR_DIAGNOSTIC_ONLY			(ERRORS_BECEEMWIMAX_BASE-64)
#define ERROR_MRU_OBJECT_NOTFOUND				(ERRORS_BECEEMWIMAX_BASE-65)
#define ERROR_MRU_CHANNEL_TABLE_IS_EMPTY		(ERRORS_BECEEMWIMAX_BASE-66)
#define ERROR_RDM_IN_IDLEMODE					(ERRORS_BECEEMWIMAX_BASE-67)
#define ERROR_DISCRETEFREQS_NOT_IN_SAME_RANGE   (ERRORS_BECEEMWIMAX_BASE-68)
#define ERROR_SEC_OBJECT_LENGTH_INVALID         (ERRORS_BECEEMWIMAX_BASE-69)

#define ERROR_DEVICE_NOT_IN_HMC_SHUTDOWN		(ERRORS_BECEEMWIMAX_BASE_EX-1)
#define ERROR_NVM_READ_FAILED                   (ERRORS_BECEEMWIMAX_BASE_EX-2)
#define ERROR_NVM_WRITE_FAILED                   (ERRORS_BECEEMWIMAX_BASE_EX-3)
#define ERROR_DEVICE_NOT_IN_LINKUP				(ERRORS_BECEEMWIMAX_BASE_EX-4)
#define ERROR_DEVICE_IS_IN_RESET_MODE			(ERRORS_BECEEMWIMAX_BASE_EX-5)
/**
*	Error codes in Thread module
**/

#define ERROR_NULL_THREAD_FUNC					(ERRORS_THREAD_BASE-0)
#define ERROR_INVALID_STOP_FUNC					(ERRORS_THREAD_BASE-1)
#define ERROR_THREAD_NOT_RUNNING				(ERRORS_THREAD_BASE-2)
#define ERROR_TERMINATION_FAILED				(ERRORS_THREAD_BASE-3)
#define ERROR_THREAD_TERMINATED					(ERRORS_THREAD_BASE-4)
#define ERROR_THREAD_CREATION_FAILED			(ERRORS_THREAD_BASE-5)
#define ERROR_THREAD_NOT_STARTED				(ERRORS_THREAD_BASE-6)
#define ERROR_THREAD_NOT_STOPPED				(ERRORS_THREAD_BASE-7)

/**
*	Error codes in SyncObjects
**/

#define ERROR_INVALID_WAIT_TIME					(ERRORS_SYNC_OBJECTS_BASE-0)
#define ERROR_INVALID_MUTEX_HANDLE				(ERRORS_SYNC_OBJECTS_BASE-1)
#define	ERROR_CREATE_MUTEX_FAILED				(ERRORS_SYNC_OBJECTS_BASE-2)
#define ERROR_RELEASE_MUTEX_FAILED				(ERRORS_SYNC_OBJECTS_BASE-3)
#define ERROR_CLOSE_MUTEX_HANDLE_FAILED			(ERRORS_SYNC_OBJECTS_BASE-4)
#define ERROR_INVALID_EVENT_HANDLE				(ERRORS_SYNC_OBJECTS_BASE-5)
#define ERROR_CREATE_EVENT_FAILED				(ERRORS_SYNC_OBJECTS_BASE-6)
#define ERROR_SET_EVENT_FAILED					(ERRORS_SYNC_OBJECTS_BASE-7)
#define ERROR_RESET_EVENT_FAILED				(ERRORS_SYNC_OBJECTS_BASE-8)
#define ERROR_CLOSE_EVENT_HANDLE_FAILED			(ERRORS_SYNC_OBJECTS_BASE-9)
#define ERROR_EVENT_NOT_INITIALIZED				(ERRORS_SYNC_OBJECTS_BASE-10)
#define ERROR_SEMAPHORE_INIT_FAILED				(ERRORS_SYNC_OBJECTS_BASE-11)
#define ERROR_SEMAPHORE_LOCK_FAILED				(ERRORS_SYNC_OBJECTS_BASE-12)
#define ERROR_SEMAPHORE_POST_FAILED				(ERRORS_SYNC_OBJECTS_BASE-13)

/**
*	Error codes in Device IO
**/

#define ERROR_INVALID_DEVICE_HANDLE				(ERRORS_DEVCIE_IO_BASE-0)
#define ERROR_DEVICE_DETTACHED					(ERRORS_DEVCIE_IO_BASE-1)
#define ERROR_INVALID_DEVICE_NAME				(ERRORS_DEVCIE_IO_BASE-2)
#define ERROR_INVALID_PKT_TO_SEND				(ERRORS_DEVCIE_IO_BASE-3)
#define ERROR_NO_DEVICE_IO_RESPONSE				(ERRORS_DEVCIE_IO_BASE-4)
#define ERROR_DEVICEIOCONTROL_FAILED			(ERRORS_DEVCIE_IO_BASE-5)
#define ERROR_READ_ABORTED						(ERRORS_DEVCIE_IO_BASE-6)
#define ERROR_CLOSE_DEVICE						(ERRORS_DEVCIE_IO_BASE-7)
#define ERROR_CREATEFILE_FAILED					(ERRORS_DEVCIE_IO_BASE-8)
#define ERROR_INVALID_READBUFFER				(ERRORS_DEVCIE_IO_BASE-9)
#define ERROR_READ_FAILED						(ERRORS_DEVCIE_IO_BASE-10)
#define ERROR_INVALID_DEVICENAME				(ERRORS_DEVCIE_IO_BASE-11)

/**
*	Error codes in Socket
**/

#define ERROR_INVALID_SOCKET_HEADER				(ERRORS_SOCKET_BASE-0)
#define ERROR_INVALID_SOCKET					(ERRORS_SOCKET_BASE-1)


/**
*	Error codes in Shared Memory
**/
#define ERROR_SHARED_MEM_OPEN_FAILED			(ERRORS_SHAREDMEM_BASE-0)
#define ERROR_SHARED_MEM_MAPPING_FAILED			(ERRORS_SHAREDMEM_BASE-1)
#define ERROR_SHARED_MEM_NOT_INITIALIZED		(ERRORS_SHAREDMEM_BASE-2)
#define ERROR_SHARED_MEM_CLOSE_FAILED           (ERRORS_SHAREDMEM_BASE-3)

#ifndef WIN32
#define WAIT_TIMEOUT				(-112)
#endif


#endif /* WIMAX_ERROR_DEFS_H */