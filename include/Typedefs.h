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
 * Description	:	Beceem defines and structure definitions.
 * Author		:
 * Reviewer		:
 *
 */



/**
@file Typedefs.h
*/

#ifndef BCM_TYPEDEFS_H
#define BCM_TYPEDEFS_H


/**
Maximum number of neighbour base stations
*/
#define    DM_MAX_NBRS    24

/********************************************************************/
/*						BASIC TYPE DEFINIIONS						*/
/********************************************************************/
#ifdef WIN32
#include <windows.h>
#else

#include <netinet/in.h>

typedef void VOID;
typedef void *PVOID;
typedef void *LPVOID;

typedef signed char CHAR;
typedef signed char* PCHAR;

typedef unsigned char UCHAR;
typedef unsigned char *PUCHAR;
typedef unsigned char BYTE;

typedef signed short SHORT;
typedef signed short *PSHORT;

typedef unsigned short USHORT;
typedef unsigned short *PUSHORT;

typedef signed int INT, INT32;
typedef signed int *PINT;

typedef unsigned int UINT;
typedef unsigned int *PUINT;

typedef long long ULONG64;

typedef unsigned int DWORD;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;

typedef float FLOAT;

typedef void *HANDLE;


typedef double DOUBLE;

#define INFINITE 0xFFFFFFFF
#define INVALID_HANDLE_VALUE (void *)-1
#define TRUE 1
#define FALSE 0

#endif

typedef UCHAR MACADDRESS[6];
/****************************************************************************************/
/*										Basic typedefs									*/
/****************************************************************************************/
typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef signed short	SINT16;
typedef unsigned short	UINT16;
typedef signed int		SINT32;
typedef unsigned int	UINT32;

typedef SINT8			B_SINT8;
typedef UINT8			B_UINT8;
typedef SINT16			B_SINT16;
typedef UINT16			B_UINT16;
typedef SINT32			B_SINT32;
typedef UINT32			B_UINT32;

#ifndef WIN32
typedef pid_t			PID;
#else
typedef DWORD           PID;
#endif
#define BECEEM_SIGNALLING_INTERFACE_API

#define MAX_VARIABLE_LENGTH /*2048*/10240
#define MAX_BUFFER_LENGTH	/*2000*/10240
/**
Payload length for EAP packets
*/
#define MAX_EAP_PACKET_LENGTH (2048 - 16)
/**
Max number of discrete frequencies
*/
#define MAX_NUM_DISCRETE_CF_SUPPORTED 120
/**
Max number of bandwidths
*/
#define MAX_NUM_BANDWIDTHS_SUPPORTED 4
#define MAX_STRING_LEN		20
#define NO_OF_QUEUES		16

/**
Max WiMAX Options bits 
*/
#define MAX_WIMAX_OPTIONS_POSSIBLE 8
/**
Max number of Tree depth possible for accessing the OMA-DM object inside the tree.
*/
#define MAX_OMA_DM_TREE_LEVEL_DEPTH 10

/**
Max key size for certificate
*/
#define MAX_KEY_SIZE_PER_CERTIFICATE 20

/**
Size of IMEI field
*/
#define MAX_IMEI_SIZE		24
/**
Max DL Zone Types
*/
#define NUMBER_OF_DL_ZONE_TYPES 10
/**
Max UL Zone Types
*/
#define NUMBER_OF_UL_ZONE_TYPES 3
/**
Num of DL Zones
*/
#define NUM_DL_ZONES			7

/**
NUMBER_OF_DL_UL_SF_INFO
*/
#define NUMBER_OF_DL_UL_SF_INFO 5

/**
MAX_NUM_SF_SUPPORTED
*/
#define MAX_NUM_SF_SUPPORTED 16

/**
MAX_DL_OR_UL_SFS is equal to #MAX_NUM_SF_SUPPORTED/2
*/
#define MAX_DL_OR_UL_SFS MAX_NUM_SF_SUPPORTED/2


/**
Max FEC Types 
*/
#define NUMBER_OF_FEC_TYPES    29

#define WIMAX_SET_OPTION_BIT0	(1<<0)
#define WIMAX_SET_OPTION_BIT1	(1<<1)
#define WIMAX_SET_OPTION_BIT2	(1<<2)
#define WIMAX_SET_OPTION_BIT3	(1<<3)
#define WIMAX_SET_OPTION_BIT4	(1<<4)
#define WIMAX_SET_OPTION_BIT5	(1<<5)
#define WIMAX_SET_OPTION_BIT6	(1<<6)
/**
BS Logger Bit defines
*/

#define BSLOGGER_CURRENTLY_TRACKING_BIT			(1<<0)
#define BSLOGGER_SERVING_BS_INDICATOR			(1<<1)
#define BSLOGGER_PREVIOUS_SERVING_BS_INDICATOR 	(1<<2)
#define BSLOGGER_R1_CINR_VALID_BIT				(1<<4)


/**
*/
#define MAX_NSP_LIST_LENGTH 10
/**
*/
#define NUM_BS_PER_NAPID 20

#define MAX_EAP_ENCRYPT_PAYLOAD_LENGTH	1300
/**
Max BSLOGGING Records
*/
#define MAX_BS_TRACKING_RECORDS 65
/**
Max Classifiers Per Service Flow
*/
#define MAX_CLASSIFIERS_PER_SF  16
/**
Max PHS Lengths
*/
#define MIBS_MAX_PHS_LENGTHS	 255
/**
Max Protocol Length
*/
#define MIBS_MAX_PROTOCOL_LENGTH   32
/**
Modem Halted States
*/
#define MODEM_HALTED_DUE_TO_T3_MODEM			0xdead0001
#define MODEM_HALTED_BADCALIBRATION_CONTENTS	0xdead0002

/**
	Max number of NAP-ID 
*/
#define MAX_NUM_NAP_ID      6
/**
	Preamble array size
*/
#define PREAMBLE_ARRAY_SIZE 4
/**
	Max number of channels
*/
#define MAX_NUM_CHANNELS    120
/**
	Maximum number of BS
*/
#define MAX_NUM_BS 60

/* 
	Maximum length of mac management message 
*/
#define MAX_MGMT_MESSAGE_LENGTH		2048	

/****************************************************************************************/
/*							Callback function pointer typedef							*/
/****************************************************************************************/
/**
Callback function pointer prototype
*/
typedef VOID (*PFNBCMCALLBACK)(PVOID pContext, PVOID pBuffer, UINT32 u32Length);


/****************************************************************************************/
/*									WiMAX Message structure								*/
/****************************************************************************************/



#pragma pack (push,1)
/**
Ethernet Parameters for Proxy
*/
typedef struct _ETH_PARAMS_ST
{
 CHAR   szIpAddress[128];/**<IP address */
 UINT32 u32Port;/**< Port number*/
}ETH_PARAMS_ST;
/**
Serial Parameters for Proxy
*/
typedef struct _SERIAL_PARAMS_ST
{
CHAR szPortName[128];/**<Serial port name*/
UINT32 u32BaudRate;/**< Baudrate for communication*/
} SERIAL_PARAMS_ST;
/**
Proxy parameters
*/
typedef union _PROXY_PARAM
{
 SERIAL_PARAMS_ST SerialParams;/**< structure for serial connection information #SERIAL_PARAMS_ST*/
 ETH_PARAMS_ST	EthParams;/**< structure for Ethernet connection information #ETH_PARAMS_ST*/
}PROXY_PARAM;

/**
Connection type and Proxy parameter structure
*/
typedef struct _STPROXYPARAMETERS
{
   UINT8 u8ConnectionType; /**< connection type (1-Ethernet, 2-serial)*/
   PROXY_PARAM eProxyParams;/**< Proxy parameter settings for perticular connection type #PROXY_PARAM*/
   
}STPROXYPARAMETERS,*PSTPROXYPARAMETERS;

/**
Wimax Message structure used by SendWiMAXMessage API
*/
typedef struct _WIMAX_MESSAGE
{
	
	USHORT	usType;							/**< 16 bit value indicating the type/category of message*/
	USHORT	usSubType;						/**< 16 bit value indicating the actual message in the category*/
	UCHAR	szData[MAX_VARIABLE_LENGTH];	/**< variable sized array that contains the payload for the message.*/

}WIMAX_MESSAGE, *PWIMAX_MESSAGE;

#define WIMAX_HEADER_LEN 2*sizeof(USHORT)

/****************************************************************************************/
/*								Base Station Information								*/
/****************************************************************************************/


/**
BS Info structure
*/

typedef struct _BSINFO
{
/**
 *<table border="1">
 *   <tr>
 *       <td><strong>ByteInfo</strong></td>
 *       <td><strong>Description</strong></td>
 *   </tr>
 *   <tr>
 *       <td>0</td>
 *       <td>Indicate whether compressed map or normal map</td>
 *   </tr>
 *   <tr>
 *       <td>1</td>
 *       <td>Un used</td>
 *   </tr>
 *   <tr>
 *      <td>2:7</td>
 *       <td>BSID<table border="1">
 *           <tr>
 *               <td>2:4</td>
 *               <td>Operator Identifier (NAP ID)</td>
 *           </tr>
 *           <tr>
 *               <td>5:7</td>
 *               <td>Base station identifier</td>
 *           </tr>
 *       </table>
 *       </td>
 *   </tr>
 *</table>
 */
	UCHAR	aucBSID[8]; 
/** Preamble index */
	UINT32	u32PreambleIndex; 	
/** Centre frequency (in kHz) */
	UINT32	u32CenterFrequency;
/** Bandwidth (in kHz) */
	UINT32	u32Bandwidth; 	
/** RSSI level in dBm */
	SINT32	s32RelativeSignalStrength; 	
/** Instantaneous CINR level in units of dB*/
	SINT32	s32CINR; 	
/** DCD change count*/
    UINT32  u32DcdChangeCount;
/** NSP change count*/
    UINT32  u32NspChangeCount;

}BSINFO, *PBSINFO;

/****************************************************************************************/
/*								Base Station List								*/
/****************************************************************************************/

/**
Payload format for BS List structure
*/
typedef struct _BSLIST
{
	/** Number of elements valid in the following array. 
		The maximum no. of base stations that will be reported is 80. 
	*/
	UINT32 u32NumBaseStationsFound;	
	/** Indicates that the search is complete.
		- 0 - Search is in progress
		- 1 - Search is complete
		- 2 - Search timed out
	*/
	UINT32 u32SearchCompleteFlag;
	/** An array of structures containing information about enumerated base stations. 
		The BSINFO is the structure of the form #_BSINFO 
	*/
	BSINFO stBSInformation[1];
}BSLIST,*PBSLIST;

/**
* Info of BS detected with per antenna reporting
*/
typedef struct _ST_BSINFO_PER_ANTENNA
{
	UCHAR	aucBSID[8]; /**< BSID. 
						The first byte will indicate whether it is compressed map or not. 
						Byte aucBSID[1] is unused.
						BSID is [2:7], of which first 3 bytes [2:4] contains the base station 
						identifier and lower 3 bytes [5:7] is operator identifier (NAP ID).*/
	UINT32	u32PreambleIndex; /**< Preamble index */
	UINT32	u32CenterFrequency; /**< Centre frequency (in kHz) */
	UINT32	u32Bandwidth; /**< Bandwidth (in kHz) */
	SINT32	s32RelativeSignalStrength; /**< RSSI level in dBm */
	SINT32	s32CINR; /**< Instantaneous CINR level in units of dB*/
    UINT32 u32DcdChangeCount;/**< DCD change count*/
    UINT32 u32NspChangeCount;/**< NSP change count*/
	SINT32	s32PerAntennaRSSI[2]; /**< RSSI Per Antenna in dBm*/
	SINT32	s32PerAntennaCINR[2]; /**< CINR Per Antenna */
}ST_BSINFO_PER_ANTENNA, *PST_BSINFO_PER_ANTENNA;


/**
Payload format for BS List structure Per Antenna Reporting
*/
typedef struct _ST_BSLIST_PER_ANTENNA
{
	/** Number of elements valid in the following array. 
		The maximum no. of base stations that will be reported is 80. 
	*/
	UINT32 u32NumBaseStationsFound;	
	/** Indicates that the search is complete.
		0 - Search is in progress
		1 - Search is complete
		2 - Search timed out
	*/
	UINT32 u32SearchCompleteFlag;
	/** An array of structures containing information about enumerated base stations. 
		The BSINFO Per Antenna is the structure of the form #_ST_BSINFO_PER_ANTENNA
	*/
	ST_BSINFO_PER_ANTENNA stBSInformation[1];
}ST_BSLIST_PER_ANTENNA,*PST_BSLIST_PER_ANTENNA;
/****************************************************************************************/
/*								Base Station Information Ex								*/
/****************************************************************************************/

/**
Payload format for BS Info
*/
typedef struct _BSINFOEX
{
	UINT8	au8BSID[6];				/**< Base station ID*/
	UINT8	u8ULCellID;				/**< Uplink cell ID*/
	UINT8	u8PreambleIndex;		/**< Preamble index*/
	SINT16 	s16CINRMean;			/**< 2Rx combined CINR and the units are in dB. Range is -10dB to +35dB*/
	SINT16	s16CINRDeviation;		/**< CINR standard deviation reported in dB*/
	UINT32	u32Duration;			/**< Duration.*/
	SINT32	s32MeanRssiReport;			/**< Combined signal strength on both antennas and units are in 0.25 dBm. Range is 0 to -110.*/
	UINT32	u32StdDeviationRssiReport;	/**< not populated*/
	UINT32	u32UlCenterFrequency;		/**< Uplink center frequency in MHz*/
	UINT32	u32DlCenterFrequency;		/**< Downlink center frequency in MHz*/
}BSINFOEX, *PBSINFOEX;


/**
Image download Enum
*/
typedef enum _EN_DOWNLOAD_IMAGE
{
IMAGE_CONFIG_FILE,
IMAGE_FIRMWARE
}EN_DOWNLOAD_IMAGE,*PEN_DOWNLOAD_IMAGE;
/**
Firmware Download structure
*/
typedef struct _ST_FIRMWARE_DNLD
{
    UINT32 u32ImageType;  	/**<Image to be downloaded #_EN_DOWNLOAD_IMAGE*/
   CHAR szImageName[128]; /**<contains path of the downloadable image*/
} ST_FIRMWARE_DNLD,*PST_FIRMWARE_DNLD;
/**
Config File Redownload structure
*/
typedef struct _ST_CONFIGFILE_DNLD
{
  CHAR szImageName[256]; /**<Contains path to config file*/
} ST_CONFIGFILE_DNLD,*PST_CONFIGFILE_DNLD;

/**
	Firmware information structure
*/

/****************************************************************************************/
/*									FIRMWAR STATUS INDICATION 							*/
/****************************************************************************************/
/**
Firmware status indication structure
*/
typedef struct _FIRMWARE_STATUS_INDICATION
{
	/**
		0 = success 
		ERROR_FIRMWAREDNLD_FAILED = If the attempt to download Firmware from host fails.
	*/
	UINT32	u32FirmwareDownloadStatus;	/**<Download status of the Firmware*/
	ST_FIRMWARE_DNLD stFirmWareInfo;
}FIRMWARE_STATUS_INDICATION, *PFIRMWARE_STATUS_INDICATION;
#pragma pack(pop)
/****************************************************************************************/
/*									SUBSCRIBER STATION INFO								*/
/****************************************************************************************/
/**
Calibration Status Information enum.
*/
typedef enum _CALIB_STATUS_INFO
{
	NOT_CALIBRATED = 0,/**< MS is not calibrated.*/
	CARD_CALIBRATED,/**< MS is calibrated, In this you will get the calibrated value 
					     describes the Band Information which are valid
						 Value is decoded as below:

						 -  Bit0 indicates that at least one band has been calibrated.
						 -	Bit1 indicates if Band 0 has been calibrated.
						 -	Bit2 indicates if Band 1 has been calibrated.
						 -	Bit3 indicates if Band 2 has been calibrated.*/
	CALIBRATION_STATUS_NOT_KNOWN = 0xFE/**< MS Calibration Status is not known.*/
}CALIB_STATUS_INFO;

/**
Subscriber Station Information structure.
*/
typedef struct _SUBSCRIBER_STATION_INFO
{
	/** Version of the Firmware in use*/
	UINT32 u32FirmwareVersion[3];		/* @ 0xBFFFF480... */
	/** Driver Version in use*/
	UCHAR uchDriverVersion[128];		/* IOCTL */
	/** Library version in use*/
	UINT32 u32LibraryVersion[3];		/* DLL version */
	/** The baseband chip version*/
	UINT32 BasebandChipVersion;			/* @ 0x0f000000 */
	/** The RF chip version*/
	UINT32 RFChipVersion;				/* @ 0x1f60f148 */
	/** EEPROM Version */
	UINT32 u32EEPROMVersion[2];				/* @ 0x1f60ef10 & 0x1f60ef12 */
	/** The Mac address of the card*/
	UINT8 u8MacAddress[6];				/* @ 0xBFFFF490 */
	/**Padding extra bytes for 32bit boundary*/
	UINT8 u8Pad[2];
	/** Flag to indicate status of card calibration*/
	UINT8 u8Calibrated;					/* @ Refer #_CALIB_STATUS_INFO for 
											calibration status information. */
}SUBSCRIBER_STATION_INFO,*PSUBSCRIBER_STATION_INFO;


/**
SS EEPROM detailed information
*/
typedef struct _ST_SS_EEPROM_DETAILED_INFO {

UINT32 u32EEPROM_RFSectionVersion; /**< Version of RF section*/
UINT32 u32EEPROM_PHYSectionVersion; /**< Version of PHY section*/
UINT32 u32EEPROM_MACSectionVersion; /**< Version of MAC section*/
UINT32 u32EEPROM_CALSectionVersion; /**< Version of Calibration section*/
UINT32 u32EEPROM_HWSectionVersion; /**< Version of Hardware section*/
UINT32 u32EEPROM_MIBSectionVersion; /**< Version of MIB section */
UINT32 u32EEPROM_SecuritySectionVersion; /**< Version of Security section */
UINT32 u32EEPROM_OMADMSectionVersion; /**< Version of OMADM section */
UINT32 u32EEPROM_MRUSectionVersion; /**< Version of MRU section */
UINT32 u32EEPROM_Reserved1SectionVersion; /**< Reserved field */
UINT32 u32EEPROM_Reserved2SectionVersion; /**< Reserved field */
UINT32 u32EEPROM_Reserved3SectionVersion; /**< Reserved field */
UINT32 u32EEPROM_Reserved4SectionVersion; /**< Reserved field */
UINT32 u32EEPROM_VSASectionVersion; /**< Version of VSA section*/

}ST_SS_EEPROM_DETAILED_INFO, *PST_SS_EEPROM_DETAILED_INFO;



/****************************************************************************************/
/*									MAC MANAGEMENT MESSAGE								*/
/****************************************************************************************/

#pragma pack(push,1)
/**
MAC Management structure
*/
typedef struct _S_MAC_MGMTMESSAGE
{
	UINT8 u8Direction;				/**< - 0 for BS to MS
										 - 1 for MS to BS*/
	UINT32 u32FrameNumber;			/**< frame number in which message was sent/received. */
	UINT32 u32MsgID;				/**< Mgmt msg type as per 802.16e standard */
	UINT32 u32LengthOfMessage;		/**< size of aMessageBinaryData*/
	UINT	u32CID;					/**< CID on which the message arrived.*/
	UINT8	u8CmacOk;				/**< flag indicating whether the CMAC check was successful. (u8CmacOk = 1 for successfull)*/
	UINT8 aMessageBinaryData[1];	/**< payload*/
}S_MAC_MGMTMESSAGE, *PS_MAC_MGMTMESSAGE;

/****************************************************************************************/
/*									NETWORK ENTRY STATUS								*/
/****************************************************************************************/

/**
Indicates the status of the link
*/
typedef enum _LINK_ERROR_CODES
{
	LINKUP_ACHIEVED = 0,/**< Modem link Status is Linkup Achieved.*/
	PHY_SYNC_ERROR,/**< MS could not find the BS.*/
	MAC_SYNC_ERROR, /**< There is no valid maps or DCD/UCD from the BS.*/
	HOST_INITIATED_SYNC_DOWN, /**< Host Initiated Sync Down to the SS. */
	RANGING_FAILURE,/**< Ranging procedure with the BS failed.*/
	SBC_FAILURE,/**< Basic capability negotiation with the BS failed.*/
	PKM_FAILURE,/**< Security authentication failed.*/
	REGISTRATION_FAILURE,/**< Registration procedure with the BS failed.*/
	DREG_RECEIVED,/**< BS sent de-registration message to the SS.*/
	RESET_RECEIVED,/**< BS sent reset message to the SS.*/
	IN_IDLE_MODE,/**< SS is in idle mode.*/
	IN_SLEEP_MODE,/**< SS is in sleep mode cycle.*/
	NETWORK_ENTRY_IN_PROGRESS,/**< Another network entry operation is in progress.*/
	WAIT_FOR_NETWORK_ENTRY_CMD,/**< Modem is expecting a network entry command.*/
	WAIT_FOR_PHY_SYNC_CMD,/**< Modem is expecting a sync up command.*/
	SHUTDOWN_RECEIVED_CMD,/**< Modem has received a shutdown command, and cannot process  network entry request.*/
	SATEK_FAILURE,/**< Security Association TEK with the BS failed.*/
	LINK_STATUS_NORMAL, /**< Modem link Status is Normal. */
	INVALID_REASON_CODE_RECEIVED, /**< Invalid Reason Code Received from BS */
	LINK_DOWN_HOST_INITIATED, /**< Modem link down initiated by host. */
	MAC_VERSION_SWITCH, /**< Modem switched the MAC version  */
	SHUTDOWN_RESYNC_RECEIVED, /**< Shutdown resync message received*/
	IDLE_MODE_FAILURE_CODE_RECEIVED, /**< Idle mode failure code received*/
	LOCATION_UPDATE_WITH_POWER_DOWN_SUCCESS_RECEIVED, /**< Location update with powerdown success message received*/
	BS_OUT_OF_REACH,/**< Sync is lost as BS is out of reach. Base band modem tries to recover from here.*/
	IP_RETENTION_TIMER_EXPIRED, /**< IP retention timer expired. IP can be released now*/
	ASR_STATE_ENTER,/**<Baseband modem entered Active State Retention(ASR) state*/
	ASR_STATE_EXIT, /**<Baseband modem exited Active State Retention(ASR) state*/
	INNER_NAI_MATCH_FAILED, /**<Inner NAI match has failed*/
	BELOW_THRESHOLD ,  /**<Notification that modem received signal strength is below threshold. Please refer to #BCM_CFG_WIMAX_TRIGGER_THRESHOLD setting threshold*/
	ABOVE_THRESHOLD   /**<Notification that modem again started getting signal strength above threshold, after going below the threshold. Please refer to #BCM_CFG_WIMAX_TRIGGER_THRESHOLD on setting threshold*/
	/* to add more error codes here */
}LINK_ERROR_CODES;

/**
The Link Status structure used
*/
typedef struct _LINK_STATUS 
{
	LINK_ERROR_CODES  eStatusCode;		/**< indicates the status of the link. See #_LINK_ERROR_CODES */
	/**
		1 - Indicates valid
		0 - Indicates inValid.
		This is valid when the status indicates LINKUP_ACHIEVED.
	*/
	UINT32  BsInfoIsValidFlag ;			
	
	BSINFOEX BsInfo;	/**< contains valid information only if uiLinkStatus = LINKUP_ACHIEVED*/
	UINT32 u32ActionCode; /**< Action Code - Valid only when eStatusCode is DREG_RECEIVED*/ 
	UINT32 u32VendorSpecificInformationLength; /**< Length of the following vendor-specific information array – should be between 0 and 255.*/
	UINT8 au8VendorSpecificInformation[1];	/**< Vendor-specific information – Beceem software acts only as a conduit for this information and no processing is done on this optional information exchanged between subscriber station and base station. Endian conversion not handled for the vendor-specific information.*/
} LINK_STATUS, *PLINK_STATUS;

/**
Device Status enum.
*/
typedef enum _DEV_STATUS
{
    DEV_UNINITIALIZED,      /**< Device is uninitialized */
    DEV_RF_OFF_HW_SW,		/**< Device RF Off(both H/W and S/W) */
    DEV_RF_OFF_HW,          /**< Device RF Off(via H/W switch) */
    DEV_RF_OFF_SW,          /**< Device RF Off(via S/W switch) */
    DEV_READY,              /**< Device is ready */
    DEV_SCANNING,           /**<  Device is scanning */
    DEV_CONNECTING,         /**< Connection in progress */
    DEV_DATA_CONNECTED      /**<  Layer 2 connected */
}DEV_STATUS,*PDEV_STATUS;

/**
Device Connection progress information enum
*/
typedef enum _DEV_CONNECTION_PROGRESS_INFO
{
    WAIT_FOR_SYNC = 1,							/**< Waiting for Sync */
    PHY_SYNC_ACHIVED,							/**< Phy Sync Achieved */
    LINKUP_IN_PROGRESS,							/**< Network Entry In Progress */
    LINKUP_DONE,								/**< Network Entry Achieved*/
    DREG_RECIEVED,								/**< DREG Received */
	RESET_RECIEVED,								/**< RESET Received */
	PERIODIC_WAKE_UP_FROM_SHUTDOWN,				/**< Periodic Wake Up From Shutdown */
	SHUTDOWN_REQ_FROM_FIRMWARE,					/**< Shutdown Notification  */	
	COMPLETE_WAKE_UP_FROM_SHUTDOWN,				/**< Complete Wake Up From Shutdown */
	RANGING_IN_PROGRESS,						/**< Ranging In Progress */	
	SBC_IN_PROGRESS,							/**< SBC In Progress */
	EAP_AUTH_DEVICE_IN_PROGRESS,				/**< Device Authentication In Progress */
    EAP_AUTH_USER_IN_PROGRESS,					/**< User Authentication In Progress */
    SATEK_IN_PROGRESS,							/**< SATEK In Progress */
    REGISTRATION_IN_PROGRESS,					/**< Registration In Progress */
    HMC_SHUTDOWN_REQ_FROM_FIRMWARE,				/**< Half Mini Card shutdown indication received */
    HMC_WAKE_UP_NOTIFICATION_FRM_FW,				/**< Half Mini Card wakeup notification received */
    BS_OUT_OF_COVERAGE,         					/**< Out of coverage notification and base band modem would try resync*/
    ASR_ENTER_FRM_FW,
    ASR_EXIT_FRM_FW,
    SHUTDOWN_REQ_FRM_FW_STANDBY_TIMER,				/**< Shutdown occurred due to standby timer */
    SHUTDOWN_REQ_FRM_FW_HIBERNATION_BUTTON_PRESS,	/**< Hibernation Notification */
    COMPLETE_WAKE_UP_FROM_HIBERNATION				/**< Complete Wake Up From hibernation */
}DEV_CONNECTION_PROGRESS_INFO;

/*
Device Link Status Reason
*/
typedef enum _LINK_STATUS_REASON
{
	STATUS_NORMAL           = 0xA,				/**< Link Status Normal*/
	PHY_SYNC_FAIL			= 1,				/**< Phy Sync Failed */
	MAC_SYNC_FAIL			= 2,				/**< Mac Sync Failed */
	HOST_INITIATED			= 3,				/**< Host Initiated Sync Down */	
	RETAIN_IP_ADDR_TIMEOUT = 4,		/**< IP retention timer expired. IP can be released now*/

	RANGE_FAIL				= 0xB,				/**< Ranging Failed */
	SBC_FAIL				= 0xC,				/**< SBC Failed */
	PKM_FAIL				= 0xD,				/**< PKM Failed */
	SATEK_FAIL				= 0xE,				/**< SATEK Failed */
	REG_FAIL				= 0x0F,				/**< Registration Failed */
	/*The codes below are not just for mapping LINK_ERROR_CODES.
	Used just as a reference in DLL. So do not use for any other*/
	/*purpose*/
	IDLE_MODE_CODE			= 0xF6,				/**< Device In Idle Mode */
	SLEEP_MODE_CODE			= 0xF7,				/**< Device In Sleep Mode  */
	DREG_RECEIVED_CODE		= 0xF1,				/**< DREG Received */
	RESET_RECEIVED_CODE		= 0xF2,				/**< RESET Received */
	NET_ENTRY_PROGRESS_CODE	= 0xF3,				/**< Network Entry In Progress */
	WAIT_NET_ENTRY_CMD_CODE	= 0xF4,				/**< Wating for Network Entry */
	WAIT_PHY_SYNC_CMD_CODE	= 0xF5,				/**< Wating for Phy Sync */
	SHUTDOWN_RECEIVED_CODE = 0xF8,				/**< Shutdown Command Received */	
	INVALID_REASON_VALUE = 0xFF,					/**< Invalid Reason Value */	
	MAC_VERSION_SWITCH_CODE = 0x11,      /**< MAC version switch*/
	HOST_INITIATED_LINK_DOWN = 0x12,   	 /**< Host initiated link down */
	IDLE_MODE_FAILURE 	= 0xFA, 			 /**< Idle mode failure */	
	SHUTDOWN_RESYNC		= 0xFB,			 /**< Shutdown resync message received*/
	LOCATION_UPDATE_SUCCESS	= 0xFE,		 /**< Location update success*/
	BS_NOT_INCOVERAGE = 0x13, /**< Not able to sync to BS as BS looks to be Out of coverage*/
	ASR_ENTER_FRM_FW_CODE = 0x14,
	ASR_EXIT_FRM_FW_CODE = 0x15,
	INNER_NAI_MATCH_FAILURE_CODE = 0x16,
	BELOW_THRESHOLD_CODE  = 0x18, /**< Below THRESHOLD */
	ABOVE_THRESHOLD_CODE=0x19 /**Above THRESHOLD*/
}LINK_STATUS_REASON;

/****************************************************************************************/
/*									GPIO INFO											*/
/****************************************************************************************/

/**
GPIO Information structure
*/
typedef struct _GPIO_INFO 
{
	UINT uiGPIONumber ;		/**< valid numbers 0-16. But only few GPIO can be programmed*/
	UINT uiGPIOValue;		/**< - 1 : set 
								 - 0 : not set
								 */
}GPIO_INFO, *PGPIO_INFO;

/**
Array index for #GPIO_MULTI_INFO and #GPIO_MULTI_MODE types
when used with #SUBTYPE_GPIO_MULTI_REQ and #SUBTYPE_GPIO_MODE_REQ
respectively.
*/
typedef enum _E_ARRAY_GPIO_INDEX{
  WIMAX_IDX=0, /**<To access WiMAX chip GPIOs */
  HOST_IDX,    /**<To access Host chip GPIOs */
  WIMAX_SPIO_IDX,/**To access WiMAX chip SPIO's for GPIO_MULTI_INFO orGPIO_MULTI_MODE*/
  MAX_IDX /**<Max number of GPIO Array Index*/
}E_ARRAY_GPIO_INDEX;

/**
  Multiple GPIO output get and set structure.
  The structures must be used as array of #MAX_IDX.
*/
typedef struct _GPIO_MULTI_INFO
{
  UINT uiGPIOCommand; /**< 32 bit field, bit 0 to 31 maps to GPIO 0 to GPIO31. Bit command value 1 for set GPIO output and 0 for get GPIO pin state*/
  UINT uiGPIOMask;    /**< 32 bit field, bit 0 to 31 maps to GPIO 0 to GPIO31. Set the correspondig bits to 1 to access GPIO for both set and get*/
  UINT uiGPIOValue;   /**< 32 bit field, bit 0 to 31 maps to GPIO 0 to GPIO31. Bit values 0 or 1; When #uiGPIOMask is set and #uiGPIOCommand is 1 Value will be set to GPIO pin output, else when #uiGPIOMask is set the pin state will be returned*/
}GPIO_MULTI_INFO , *PGPIO_MULTI_INFO;

/**
Array of multi info GPIO structure
*/
typedef struct _ST_ARRAY_GPIO_MULTI_INFO_
{
	GPIO_MULTI_INFO aMultiInfo[MAX_IDX]; /**< Refer to #_GPIO_MULTI_INFO and #MAX_IDX */
}ST_ARRAY_GPIO_MULTI_INFO,*PST_ARRAY_GPIO_MULTI_INFO;

/**
MULTI GPIO mode set structure
*/
typedef struct _GPIO_MULTI_MODE
{
  UINT uiGPIOMode;    /**< 32 bit field, bit 0 to 31 maps to GPIO 0 to GPIO31. 1 to set GPIO mode to OUT, 0 for IN mode*/
  UINT uiGPIOMask;    /**< 32 bit field, bit 0 to 31 maps to GPIO 0 to GPIO31. Set the correspondig bits to 1 to change the GPIO mode, When all bits of #uiGPIOMask set to zero returns the current mode of all GPIOs*/
}GPIO_MULTI_MODE , *PGPIO_MULTI_MODE;
/**
Array of multi mode GPIO structure
*/
typedef struct _ST_ARRAY_GPIO_MULTI_MODE_
{
	GPIO_MULTI_MODE astMultiMode[MAX_IDX]; /**< Refer to #_GPIO_MULTI_MODE and #MAX_IDX */
}ST_ARRAY_GPIO_MULTI_MODE,*PST_ARRAY_GPIO_MULTI_MODE;

/****************************************************************************************/
/*									LED STATE INFO											*/
/****************************************************************************************/

/**
Enumerated LED colors 
*/
typedef enum _LED_COLORS{
	RED_LED = 1,		/**< Red LED type*/
	BLUE_LED = 2,		/**< Blue LED type*/
	YELLOW_LED = 3,		/**< Yellow LED type*/
	GREEN_LED = 4		/**< Green LED type*/
} LED_COLORS;

/**
Enumerated Baseband states - For more details please refer to Target Parameter Configuration document from Beceem.
*/
typedef enum _LED_EVENTINFO {
	DRIVER_INIT = 0x1,		/**< Baseband in Init state*/
	FW_DOWNLOAD = 0x2,		/**< Baseband in firmware download state*/
	FW_DOWNLOAD_DONE = 0x4,	/**< Baseband  in firmware download done state*/
	NO_NETWORK_ENTRY = 0x8,	/**< Baseband in no network entry state*/
	NORMAL_OPERATION = 0x10,	/**< Baseband in normal transmit/receive state*/
	IDLEMODE_ENTER = 0x20,	/**< Baseband in idlemode entry state - LED Cannot be configured for this state - This state is only to stop all the LED activities prior to enter Idlemode*/
	IDLEMODE_CONTINUE = 0x40, /**<Not Supported*/
	IDLEMODE_EXIT = 0x80		/**< Baseband in idlemode exit state -  LED Cannot be configured for this state - This state is for resuming all the LED activities after Idlemode*/
} LED_EVENTINFO;

/**
LED state information structure
*/
typedef struct _LED_STATE_INFO
{
	UCHAR ucLedType;	 /**<  Specify LED type number. Use 0x7F if not used. Polarity of the LED represents the LED logic
								for turn on and off. For normal polarity, write 1 for turn LED on and 0 for off. For reverse
								polarity, write 0 for turn on and 1 for turn off. Application informs to baseband for the polarity
								in bit 8.
								- Bit 8 is unset for normal polarity and set for reverse polarity.
								- Bits 1 to 7 is used for representing #_LED_COLORS.*/
	UCHAR ucLedOnState;  /**<  States of Baseband modem for which the above LED is associated. 
							Refer to #_LED_EVENTINFO for different states of Baseband modem.
						 */
	UCHAR ucLedBlinkState;  /**<  
							Baseband states for which the LEDs should blink. 
							Refer to #_LED_EVENTINFO for different states of Baseband modem. 
							*/
}LED_STATE_INFO, *pLED_STATE_INFO;

/**
LED information structure
*/
typedef struct _LED_INFORMATION
{
	UINT32  bSaveToConfigFile;	 /**< LED Configuration to be used for future session.
								  - 1 : Save the settings to config file.
								  - 0 : Do not save the settings.
							  */
	LED_STATE_INFO LedInfo[4];	 /**< Array of #_LED_STATE_INFO settings information for each LED*/
}LED_INFORMATION;

/****************************************************************************************/
/*									LED STATE INFO											*/
/****************************************************************************************/



/****************************************************************************************/
/*									Message format for IR, BR, PR Log					*/
/****************************************************************************************/
/**
Initial Ranging requests structure
*/
typedef struct _RANGE_CH_TRACK
{
    UINT32    u32FrameNum;		/**< Frame number*/
    UINT32    u32Code;			/**< Code*/
    UINT32    u32Subchannel;	/**< Sub Channel*/
    UINT32    u32Symbol;		/**< Symbol*/
}RANGE_CH_TRACK, *PRANGE_CH_TRACK;

/****************************************************************************************/
/*									Message format BWL Log								*/
/****************************************************************************************/
/**
BW Log structure
*/
typedef struct _BW_LOG
{
 UINT16   u16PduLen;			/**< PDU length in bytes*/
 UINT16   u16AllocationRecvd;	/**< Allocation received in bytes*/

 UINT16   u16Vcid;				/**< VC ID*/
 UINT16   u16Cid;				/**< C ID*/

 UINT16   u16QueueDepth;		/**< Queue depth in bytes*/
 UINT8     u8BWReq;				/**< 1=BWREQ , 2=PDU*/
 UINT8     u8SchType;			/**< BE=2,NRTPS=3,RTPS=4,ERTPS=5,UGS=6*/

 UINT32   u32FrameNumber;		/**< Frame number*/
}BW_LOG, *PBW_LOG;

/****************************************************************************************/
/*									Message format OL, CL Log								*/
/****************************************************************************************/

/**
	Power adjustment log structure
*/
typedef struct _POWER_ADJ_TRACK
{
    SINT32    s32PwrAdj;		/**< Deviding this value by 4 gives power adjustment in dBm*/
    UINT32    u32FrameOffset;	/**< Correction should be done based on Tx power this many frames earlier */
    UINT32    u32FrameNum;		/**< Frame number when correction was received */
    SINT32    s32Power;			/**< New power after correction */
}POWER_ADJ_TRACK, *PPOWER_ADJ_TRACK;

/****************************************************************************************/
/*									Message format CID Log								*/
/****************************************************************************************/

/**
	Power adjustment log structure
*/

typedef struct _CID_LOG
{
	UINT16      u16Vcid;	/**< VC ID */
	UINT16      u16Cid;		/**< C ID */
	
	UINT16      u16bArq;	/**< Boolean = ARQ enabled or not*/
	UINT16      u16Dir;		/**< 0= DL, 1 = UL, 2 = BI*/
	
}CID_LOG, *PCID_LOG;

/****************************************************************************************/
/*									Vendor speific info for Net entry					*/
/****************************************************************************************/
/**
Vendor specific information structure
*/
typedef struct _VENDOR_SPECIFIC_INFO
{
	UINT32 uiLength;		/**< Length of the TLV data in bytes. 0 indicates empty buffer*/
	UCHAR aucData[256];	/**< TLV data*/
}VENDOR_SPECIFIC_INFO, *PVENDOR_SPECIFIC_INFO;

/****************************************************************************************/
/*									Vendor Specific Param for READ/WRITE APIs							*/
/****************************************************************************************/

/****************************************************************************************/
/*									Vendor Specific Param								*/
/****************************************************************************************/

/**
Vendor specific parameter structure
*/
typedef struct _VENDOR_SPECIFIC_PARAM
{
	UINT16 u16Offset;				/**< Offset from where the parameter starts within the Read-Only/Read-Write Region*/
	UINT16 u16Size;					/**< size of the parameter */
	UINT8  au8VSAData[1];   		/**< Parameter value */
}VENDOR_SPECIFIC_PARAM, *PVENDOR_SPECIFIC_PARAM;


/****************************************************************************************/
/*									MIBVendor Specific Param for READ/WRITE APIs		*/
/****************************************************************************************/
/**
MIB structure
*/
struct _MIB_OBJECT_INFO
{
	UINT32	u32MibObjId;	/**< These are constant defined values.*/
	UINT8	u8NumKeys;		/**< Number of indexes for Table MIBS*/
	UINT32	au32Key[4];		/**< Keys to be used for table MIBS */
	UINT32	u32MibObjSize;	/**< Size of the Object ID as mentioned in the 802.16f standard. Does not include other members in this structure.*/
	SINT8   s8ErrorCode;	/**< The Error values (currently this is either Failed(1)/Success(0)); Please set this to zero for Set requests.*/
	UINT8   au8MibObjVal[1];	/**< The value of the Object. The data format should follow 802.16f standard based on the Object Id.*/
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
;

typedef struct _MIB_OBJECT_INFO MIB_OBJECT_INFO, *PMIB_OBJECT_INFO;
/****************************************************************************************/
/*									MIBVendor Specific Param for Multiple READ/WRITE APIs		*/
/****************************************************************************************/
/**
Multiple MIB structure
*/
typedef struct _MIB_OBJECTS
{
	UINT16	u16NumMibObjs;	/**< Number of MibObjects for Get/Set Request/Response.*/
	MIB_OBJECT_INFO   MibObjs[1];	/**< The Information of the Mib Object. Variable Size depending on u16NumMibObjs.*/
}MIB_OBJECTS, *PMIB_OBJECTS;

/**
Multiple periodic MIB indication structure
*/
typedef struct _MULTI_MIB_INDICATION_ST_
{
	UINT16	u16NumMibObjs;	/**< Number of MIBObjects requested for periodic indication.*/
	UINT16  u16TimePeriod;	/**< period in secs for which notification is requested for the fragmant size. When set to zero stops the periodic
                                                        notification */
	UINT16  u16MIBFragmentSize; /**< Fragment size of MIB for each notification. u16MIBFragmentSize <= u16NumMibObjs*/
	MIB_OBJECT_INFO   MibObjs[1];	/**< The Information of the MIB Object. Variable Size depending on u16NumMibObjs.*/
}MIB_OBJECTS_TIMER, *PMIB_OBJECTS_TIMER;


/****************************************************************************************/
/*							Set Baseband Transmit Power Response 						*/
/****************************************************************************************/

/**
TX Power response structure
*/
typedef struct _S_SET_TX_POWER_RESPONSE
{
	/**
	Status of the Set Tx Power request.
	0 – Failure
	1 – Success
	*/
	UINT16 u16Status;
	UINT8  u8TxPower;	/**< Actual Tx power in dBm of baseband after set.*/
	UINT8  u8Reserved;	/**< Reserved field */
} S_SET_TX_POWER_RESPONSE, *PS_SET_TX_POWER_RESPONSE;

/****************************************************************************************/
/*									Idle Mode Indication				 				*/
/****************************************************************************************/

/**
Idle mode indication structure.
*/
typedef struct _ST_IDLEMODE_INDICATION
{
	/**
		- 0  : Indicates the baseband has come out of Idle mode.
		- >0 : Indicates the baseband is going into Idle Mode. 
		The value in this field is the planned total time in milliseconds that the baseband shall remain idle
		
	*/
	UINT32 u32IdleModePeriod;
	
	/** 
		BSID : Base station ID the modem connected to.
	*/
	UINT8  auBSID[6];

	/** 
	 	RSSI : RSSI as reported by Modem in 0.25 dBm units.
	 	- User has to Multiply the RSSI by 0.25,
		to get the actual RSSI reported by Modem.
	*/
	SINT32 s32RSSI;
	
	/** 
	  	CINR : CINR as reported by Modem
	
	*/
	SINT32 s32CINR;	
	
	/** 
	  	Preamble Index : Base station ID Preamble Index modem is connected to.
	
	*/
	UINT32 u32PreambleIndex;

	/**
		Length of the following vendor-specific information array – should be between 0 
		and 255.
	*/
	UINT32 u32VendorSpecificInformationLength;

	/**
	Vendor-specific information – Beceem software acts only as a conduit for this information 
	and no processing is done on this optional information exchanged between subscriber station 
	and base station.
	*/
	UINT8 au8VendorSpecificInformation[1];
}ST_IDLEMODE_INDICATION, *PST_IDLEMODE_INDICATION;


/****************************************************************************************/
/*									VENDOR SPECIFIC LINK PARAMS				 			*/
/****************************************************************************************/

/**
Vendor-Specific Parameter Structure
*/
typedef struct _VENDOR_SPECIFIC_LINKPARAMS
{
	UINT32 u32VendorSpecificInformationLength; /**< Length of the following vendor-specific information array – should be between 0 and 255.*/ 
	UINT8 au8VendorSpecificInformation[256];		/**< Vendor specific information.*/
}VENDOR_SPECIFIC_LINKPARAMS, *PVENDOR_SPECIFIC_LINKPARAMS;

/****************************************************************************************/
/*									LOGSTRUCT								 			*/
/****************************************************************************************/
/**
Logger payload structure
*/
typedef struct _LOGSTRUCT 
{
	UINT16 u16PayloadSizeInBytes;/**< Payload length in bytes*/
	UINT8 au8LogBytes[1];/**<Logger data*/
}LOGSTRUCT, *PLOGSTRUCT;

/****************************************************************************************/
/*								HANDOFF INFORMATION STRUCTURE			 				*/
/****************************************************************************************/

/**
Hand over response structure
*/
typedef struct _S_HOINFO
{
	UINT8 u8hoSuccessFailure;	/**< Status of handoff is indicated by this field,
										1 – Success
										0 – Failure
										*/
	UINT8  u8DHCPRefresh; /**< Indicates if there is a DHCP refresh that needs to be done
							  0 - NO DHCP refresh
							  1 - DHCP refresh */
	UINT8  u8CurrentBSID[6]; /**< Current Base station ID MS is listenting to */
	UINT8  u8LastBSID[6]; /**< Last Base station ID MS was listenting to before Handover */
	UINT32 u32NoOfRangingFailure; /**< Total number of ranging failure during this handover */
	UINT32 u32HandoverLatency; /**< Handover latency in ms */
	UINT32 u32CurrentPreambleID; /**< Preamble ID of the Current BS to which MS is listening to */
	UINT32 u32LastPreambleID; /**< Preamble ID of the Last BS to which MS was listening to */
	UINT32 u32VendorSpecificInformationLength; /**< Length of the following vendor-specific information array – should be between 0 and 255.*/
	UINT8 au8VendorSpecificInformation[1];	/**< Vendor-specific information – Beceem software acts only as a conduit for this information and no processing is done on this optional vendor specific information.*/
}S_HOINFO, *PS_HOINFO;

/****************************************************************************************/
/*								DISCRETE CENTER FREQ INPUT LIST			 				*/
/****************************************************************************************/
/**
Structure with the list of frequencies to scan. 
*/
typedef struct _ST_DISCRETE_CENTERFREQ_INPUTLIST
{
	/**Number of discrete frequencies (Maximum of #MAX_NUM_DISCRETE_CF_SUPPORTED )*/
	UINT32 u32NumDiscreteFreq;
	/**Array of discrete frequencies (in kHz). A maximum of 120 discrete 
	frequencies can be specified. It is assumed that the application provides 
	the list of centre frequencies sorted in ascending order. */
	UINT32 au32CenterFreqList[MAX_NUM_DISCRETE_CF_SUPPORTED];
}ST_DISCRETE_CENTERFREQ_INPUTLIST, *PST_DISCRETE_CENTERFREQ_INPUTLIST;

/****************************************************************************************/
/*								CONTINUOUS CENTER FREQ INPUT RANGE		 				*/
/****************************************************************************************/

/**
Structure with the frequency range to scan 
*/
typedef struct _ST_CONTINUOUS_CENTERFREQ_INPUTRANGE
{
	/**Start center frequency (in kHz)*/
	UINT32 u32CenterFreqStart;       
	/**Center frequency step (in multiples of 250 kHz)*/
	UINT32 u32CenterFrequencyStep;   
	/**End center frequency (in kHz)*/
	UINT32 u32CenterFrequencyEnd;    
}ST_CONTINUOUS_CENTERFREQ_INPUTRANGE, *PST_CONTINUOUS_CENTERFREQ_INPUTRANGE;

/****************************************************************************************/
/*								CONTINUOUS CENTER FREQ INPUT RANGE		 				*/
/****************************************************************************************/

/**
Structure with the list of Bandwidth
*/

typedef struct _ST_BANDWIDTH_INPUTLIST
{
	/**No. of bandwidths to scan (Maximum of #MAX_NUM_BANDWIDTHS_SUPPORTED)*/
	UINT32 u32NumOfBwsToScan;       
	/**Array of bandwidths to scan (in kHz) */
	UINT32 au32BandwidthList[MAX_NUM_BANDWIDTHS_SUPPORTED];   
} ST_BANDWIDTH_INPUTLIST, *PST_BANDWIDTH_INPUTLIST;

/****************************************************************************************/
/*								NETWORK SEARCH PARAMS					 				*/
/****************************************************************************************/
/**
Payload Parameter for network search request
*/
typedef struct _ST_NETWORK_SEARCH_PARAMS
{
	/**Duration in seconds for the entire search. 
	The Baseband should abort the scan once this timer expires. 
	The user can specify an infinite timeout by setting this to 0. 
	In this case, the search will end only once the entire CF list is 
	exhausted or a scan failure occurs.*/
	UINT32 u32Duration; 
	/**Specifies whether the BS wants to report BS-by-BS as and when each BS is detected or a consolidated 
	list of BSes at the end of the search.
	- u32ReportType = 1 to provide BS by BS list followed by a consolidated list at
						the end of the search (to indicate the end of search). 
						The results (per BS report and consolidated list) can be 
						differentiated by the u32SearchCompleteFlag flag in the response. 
						This is set for consolidated list at end of search and reset for the 
						per BS response.

	- u32ReportType = 2 to provide consolidated list */
	UINT32  u32ReportType;
	/**Structure with the list Bandwidth. */
	ST_BANDWIDTH_INPUTLIST stBandwidthList;
	/**Structure with the frequency range to scan */
	ST_CONTINUOUS_CENTERFREQ_INPUTRANGE stContCFRange;
	/**Structure with the list of frequencies to scan. Can have 0 frequencies. */
	ST_DISCRETE_CENTERFREQ_INPUTLIST stDiscreteCFList;	
}ST_NETWORK_SEARCH_PARAMS, *PST_NETWORK_SEARCH_PARAMS;

/****************************************************************************************/
/*								NETWORK SEARCH PARAMS					 				*/
/****************************************************************************************/
/**
Syncup request param structure
*/
typedef struct _ST_SYNCUP_REQ_PARAMS
{
	UINT32	u32CenterFreq; /**< Center frequency in kHz (optional – Set to 0 if not specified) 
						   - Range: 2.3 GHz to 3.7 GHz*/
	UINT32	u32Bandwidth; /**< Bandwidth in kHz (if set to 0, the default bandwidth will be selected) 
						  - Supported bandwidth are:
                                                        - 3.5 MHz
                                                        - 5 MHz
                                                        - 7 MHz
                                                        - 8.75 MHz 
                                                        - 10 MHz
						        */
	UINT32	u32PreambleId; /**< Preamble ID (optional – Set to 255 if not specified) */
	UCHAR	aucBSID[8];	/**< BSID (optional – set to 0 if not specified) */
}ST_SYNCUP_REQ_PARAMS, *PST_SYNCUP_REQ_PARAMS;


/****************************************************************************************/
/*								NETWORK SEARCH PARAMS					 				*/
/****************************************************************************************/
/**
Setting default RSSI/CINR for Network Search
*/
typedef struct _ST_SET_RSSI_CINR
{
	SINT32 s32RSSI; /**< RSSI in dBm*/
	SINT32 s32CINR; /**< CINR in dB*/

}ST_SET_RSSI_CINR, *PST_SET_RSSI_CINR;


/****************************************************************************************/
/*									SYNC STATUS								 			*/
/****************************************************************************************/

/**
Sync status structure
*/
typedef struct  _ST_SYNC_STATUS
{
	/**
		Sync status. 
		- PHY_SYNC_ERROR (1) : If unable to sync
		- SYNC_UP		 (0) : If sync successful.
	*/
	UINT32	u32SyncStatus;	
	UCHAR	aucBSID[8];		/**< BSID */
	UINT32	u32PreambleId;	/**< Preamble ID*/
	SINT32	s32CINRMean;	/**< 2Rx combined CINR and the units are in dB. Range is -10dB to +35dB*/
	SINT32	s32CINRDev;		/**< CINR standard deviation reported in dB*/
	SINT32	s32RSSIMean;	/**< Combined signal strength on both antennas and units are in dBm. Range is 0 to -110.*/
	SINT32	s32RSSIDev;		/**< RSSI standard deviation and units are in dB.*/
	UINT32	u32CenterFreq;	/**< Center Frequency in kHz
							- Range: 2.3 GHz to 3.7GHz*/
	UINT32	u32Bandwidth;	/**< Bandwidth in kHz
							- Supported bandwidth are:
                                                                - 3.5 MHz
                                                                - 5 MHz
                                                                - 7 MHz
                                                                - 8.75 MHz 
                                                                - 10 MHz
                                                        */
	UINT32 u32DCDChangeCount;/**< DCD change count */
	UINT32 u32NSPChangeCount;/**< NSP change count */ 
	SINT32 s32PerAntCINR[2]; /**< CINR Per Antenna, Valid only when Per Antenna Reporting is Enabled */
    SINT32 s32PerAntRSSI[2]; /**< RSSI Per Antenna in dBm, Valid only when Per Antenna Reporting is Enabled */
}ST_SYNC_STATUS, *PST_SYNC_STATUS;

/****************************************************************************************/
/*									EAP PAYLOAD											*/
/****************************************************************************************/

/**
EAP Payload structure
*/
typedef struct _EAP_PAYLOAD
{
	USHORT	usEAPPayloadLength;	/**< length of EAPPayload (below)*/
	UCHAR	aucEAPPayload[MAX_EAP_PACKET_LENGTH]; /**< EAP Payload data
													See #MAX_EAP_PACKET_LENGTH*/
}EAP_PAYLOAD, *PEAP_PAYLOAD;

/**
EAP Complete info structure
*/
typedef struct _EAP_COMPLETE_INFO
{
	UCHAR ucStatus;				/**<	- Success = 0  payload =  Session Key (MSK in case of EAP AKA)
										- Failure = 1  payload = Failure cause (2 bytes)
										(EAP AKA error code, or as per any other requirements)
								*/
	USHORT usEAPCompletePayloadLength;	/**< Length of payload*/
	UCHAR aucEAPCompletePayload[MAX_EAP_PACKET_LENGTH]; /**< Payload See #MAX_EAP_PACKET_LENGTH*/

}EAP_COMPLETE_INFO, *PEAP_COMPLETE_INFO;

/**
BSINFO structure
*/
typedef  struct _ST_BSINFO
{
     UINT32 u32PreambleIndex; /**< Preamble index*/
     UINT32 u32ChannelBandwidth; /**< Channel bandwidth Of Neighbour BS identified by PreambleIndex If the bandwidth for the BS could not be retrieved this value is 0.*/
     UINT32 u32CentreFreq;  /**< Frequency of BS in 125kHz steps.*/
	 UINT32 u32BSIDLsb;		/**< The Lower 32 bits of the base station ID*/
     UINT32 u32BSIDMsb;		/**< The Upper 32 bits of the base station ID*/
     SINT32 s32Rssi;		/**< Current RSSI level in units of 0.25 dBm*/
     SINT32 s32Cinr;		/**< Instantaneous CINR level in units of dB*/
}ST_BSINFO, *PST_BSINFO;

/**
Neighbour BS structure
*/
typedef struct ST_NEIGHBOUR_BWINFO
{
     UINT32 u32BSCount; /**< Total number of BS’s whose bandwidth info can be retrieved from following array astBwInfo. */
     ST_BSINFO stNbrInfo[32]; /**< Array containing the bandwidth info of each neighbour BS. See #_ST_BSINFO*/
}ST_NEIGHBOUR_BWINFO, *PST_NEIGHBOUR_BWINFO;

/**
DIUC Mapping structure
*/
typedef struct _ST_DIUC_MAPPING
{
    UINT32 u32Diuc;			/**< Downlink Interval Usage Code */
    UINT32 u32FecCodeType;	/**< FEC code Type*/
}ST_DIUC_MAPPING;

/**
Downlink Burst Profile  from DCD.
*/
typedef struct _ST_BCMBASEBAND_DL_BURSTPROFILE
{
  UINT32 u32NumProfiles;				/**< The total number of valid DIUC mapping structures available in the following array.*/
  ST_DIUC_MAPPING astDiucMapping[16];	/**< Array of #_ST_DIUC_MAPPING structures.*/
}ST_BCMBASEBAND_DL_BURSTPROFILE;

/**
UIUC Mapping structure
*/
typedef struct _ST_UIUC_MAPPING
{
    UINT32 u32Uiuc;					/**< Uplink Interval Usage Code */
    UINT32 u32FecCodeType;			/**< FEC code Type*/
}ST_UIUC_MAPPING;
/**
Downlink Burst Profile  from DCD.
*/
typedef struct _ST_BCMBASEBAND_UL_BURSTPROFILE
{
  UINT32 u32NumProfiles;					/**< The total number of valid UIUC mapping structures available in the following array.*/
  ST_UIUC_MAPPING astUiucMapping[16];		/**< Array of ST_UIUC_MAPPING structures.*/
}ST_BCMBASEBAND_UL_BURSTPROFILE;

/**
SBC Connections params structure.
*/
typedef struct _ST_BCM_SBC_CONNECTIONPARAMS
{
	UINT16         u16AuthSupport;		/**< Authentication support - indicates 
										authorization policy used by the MS and BS 
										to negotiate and synchronize.*/
    UINT16  u16SsTransitionGaps;		/**< SS Transition gaps. - indicates 
										the transition speed SSTTG(min. transmit to 
										receive turnaround gap) and SSRTG (min. receive 
										to transmit turnaround gap) for SS.*/
    UINT16  u16OfdmaSsMimoDemod;		/**< SS MIMO Demod. - indicates the different MIMO 
										options supported by a WirelessMAN-OFDMA PHY SS 
										in the Downlink.*/
	UINT16  u16OfdmaMsCsit;				/**< DMA MS CSIT. - indicates MS capability of 
										supporting CSIT (UL sounding)*/
	UINT16  u16OfdmaSsDemod;			/**< DMA SS Demod. - indicates the different 
										demodulator options supported by a 
										WirelessMAN-OFDMA PHY SS for DL reception.*/
    UINT16  u16HarqIncrRedundancyBufferCap;/**< HARQ Incr Redundancy buffer capability.*/
    UINT16  u16HarqChaseCombining_CC_IR_BufferCap;/**< HARQ chase combining CC IR buffer 
												capability.*/
	UINT8   u8MsFFTSizes;/**< MS FFT Sizes.*/
    UINT8   u8OfdmaSsMod;/**< DMA SS Mod. -  indicates the different modulator options 
						 supported by a WirelessMAN-OFDMA PHY SS for UL transmission.*/
    UINT8   u8OfdmaSsPermutationSupport;/**< DMA SS Permutation support. - indicates the 
										different optional OFDMA permutation modes 
										(optional PUSC, optional FUSC, and AMC) supported 
										by a WirelessMAN-OFDMA SS*/
    UINT8   u8OfdmaSsMimoUplink;/**< DMA SS MIMO uplink. - indicates the different MIMO 
								options supported by a WirelessMAN-OFDMA PHY SS in 
								the Uplink.*/
    SINT8   s8OfdmaSsCinrMeas;/**< DMA SS CINR Meas.*/
    UINT8   u8OfdmaMapCapability;/**< DMA Capability. - indicates the different MAP 
								 options supported by a WirelessMANOFDMA PHY
								 (like HARQ map, Sub MAP).*/
    UINT8   u8UplinkControlChannelSupport;/**< Uplink control channel support. - indicates 
										  the different Uplink control channels supported 
										  by a WirelessMAN-OFDMA PHY MS for UL transmission.
										  (like DIUC-CQI fast feedback,  UL ACK, 
										  3 bit MIMO fast feedback).*/
    UINT8   u8HarqMaxBurstPerFrame;/**< MAX Burst perframe.*/
    UINT8   u8OfdmaSsModMimoSupport;/**< DMA SS mod MIMO support. - indicates the MIMO 
									capability of OFDMA SS modulator.*/
    UINT8   u8OfdmaMutipleDownlinkBurstProfile;/**< DMA multiple downlink burst profile. - 
											   indicates DL/UL Burst Profile that shall 
											   be used for MS and BS.*/
	UINT32  u32MaxTxPower;	/**< MAX TX Power.*/
} ST_BCM_SBC_CONNECTIONPARAMS;

/**
UL BW request information 
*/
typedef  struct _ST_UL_BW_REQ_INFO
{
    UINT16 u16Cid;				/**< UL CID */
    UINT16 u16Reserved;			/**< Reserved*/
    UINT32 u32NumBwRequests;	/**< Number of BW requests sent on this CID */
    UINT32 u32TotalBwRequested; /**< Total BW request (Bytes) sent for this CID*/
}ST_UL_BW_REQ_INFO;

/**
Total allocation granted to this MS
*/
typedef  struct  _ST_UL_BW_ALLOC_INFO
{
    UINT32 u32NumBwAllocations;		/**< Number of BW allocations received on Basic CID */
    UINT32 u32TotalBwAllocations;	/**< Total BW allocation (Bytes) sent for this MS */
}ST_UL_BW_ALLOC_INFO;

/**
Classifier Information Structure
*/
typedef struct _S_CLASSIFIER
{
	UINT32 u32ClasifierID;
	UINT32 u32PHSRuleID;
}S_CLASSIFIER;

/**
SFID Information structure
*/
typedef struct _SFID_INFO
{
    UINT32			u32SFID;					/**< SF ID*/
	UINT32			u32NumClassifierPHS; /**< Total Number of Classifier/PHS Filled for this SF ID*/
	S_CLASSIFIER	au32ClassifierIdList[MAX_CLASSIFIERS_PER_SF]; /**< Classifier ID List of size #MAX_CLASSIFIERS_PER_SF*/
}SFID_INFO;
/**
SFID List
*/
typedef struct _SFID_LIST
{
    UINT32 u32NumActiveSFIDs;	/**< Number of SFIDs*/
    SFID_INFO sfidList[16];		/**< Array of #_SFID_INFO structures*/
}SFID_LIST;
/**
Maximum number of PSC definitions supported.
*/
#define MAX_NUM_OF_SLEEP_MODE_DEFNS  4
#define MAX_NUM_CIDS 16

/**
Enumerated Activity type for the underlying Sleep mode message. The activity 
indicates the purpose of request of the message from Host to Target. If it is 
from Target to Host, it indicates the response for the associated request 
previously sent by the Host.

*/
typedef enum _EN_ACTIVITY_TYPE
{       
	PSC_DEFN=0,/**< To be used in case of sending Definition/Redefinition.
		     -While defining new PSC the host should mention 0xFF as
			 the value.
			 -While redefining
			 host should mention the exact PSCID of the PSC which it wants to redefine
			 -Rang of PSCID values: 
				- 0xFF in case host is redefining new PSC 
				- 0 to 0x7E in case host wants to re-define an existing PSC 
			 - SleepModeReq (Host to Target) Msg Types. */
	PSC_ACTIVATION,/**<For activating a PSC(power saving class). 
				 - The PSCID should be an already defined.
				 - SleepModeReq (Host to Target) Msg Types. */
	PSC_DEACTIVATION,/**< For deactivating a PSC using unique PSC ID 
				 - PSCID should already be activated
				 - SleepModeReq (Host to Target) Msg Types. */
	PSC_DEFNANDACTIVATION,/**<For defining and activating a PSC at one go itself
						  Rang of PSCIDs:
						  - 0xFF if host wants to define a new PSC and activate it
						  - 0 to 0x7E if host wants to redefine an already defined PSC
						  and activate it.
						  - SleepModeReq (Host to Target) Msg Types. */
	PSC_ACK,/**< Ack for the SleepModeReq. 
		  - SleepModeIndication (Target to Host) Msg Types */
	PSC_NACK,/**< Nack can be accompanied by a valid PSCID in case it’s a NACK to a
			Redefinition request, In such a case PSCID field carries a legitimate
			(non 0xff) value.
			- SleepModeIndication (Target to Host) Msg Types */
	PSC_DELETE,/**< In cases where BS and MAC decide to delete a particular PSCID 
	        in SUBTYPE_SLEEP_MODE_RESP we will get the enActivity field set to Delete type
			- SleepModeIndication (Target to Host) Msg Types */
	PSC_NACK_POSTHODEFNPENDING,/**< Block all Host Requests till HO+SleepMode Re-negotiation is completed.
						  - SleepModeIndication (Target to Host) Msg Types */	
} EN_ACTIVITY_TYPE;	

/**
The PSC structure that is used in the sleepmode messages for exchanging 
PSC information between Host and Target(MAC).
*/
typedef  struct _ST_PSC_DEFN{
    UINT8   u8PSCID;/**< Power Saving Class ID. Range of PSCID values are:
				    - 0xFF(decimal 255) - When message contains a new definition
				    - 0 to 0xFE - when message is for activation or contains 
						re-definition cum activation request*/
    UINT8   u8PSCType;/**< Power Saving Class Type*/
    UINT8   u8direction;  /**< Direction 
                         - 0 Not specified- For both the directions
                         - 1 is for DL
                         - 2 is for UL*/
    UINT8   u8InitialSleepWindow; /**<Initial sleep window in number of frames*/
    
    UINT8   u8FinalSleepWindowExp;/**<Final Sleep window exponent*/
    UINT8   u8ListenWindow;/**<Listen window in number of frames */
    UINT16  u16FinalSleepWindowBase;/**<Final sleep window base*/
                
    UINT8   bTTWF; /**<Traffic Triggered Wakening Flag associated with the PSC. 
				   Valid only when PSC Type is 1. Valid values are:
						0 - To not use TTWF
						1 - To use TTWF*/
    UINT8   bTIR;  /**<Traffic Indication Required flag associated with the PSC. 
				   Valid only when PSC Type is 1. Valid values are:
						0 - To not use TIR
						1 - To use TIR*/
    UINT8   padding[2]; /**< Reserved*/
    // To keep the struct 4 byte aligned
    
    UINT16  aCIDs[MAX_NUM_CIDS]; /**<List of CIDs for which the PSC is applicable*/  
    EN_ACTIVITY_TYPE enActivity ; /**< Activity type enum*/
    UINT32  u32NoOfCids;  /**<Number of CIDs included in the aCID list*/
        	
} ST_PSC_DEFN;

/**
Sleep mode Message that is used to set/get sleepmode related PSC information by 
the Host Application. The message consists of the count of the PSC information 
in the message followed by the PSC information.
*/
typedef struct _ST_SLEEPMODE_MSG
{
	UINT32 numOfDefinitions; /**< Number of valid PSC definitions in the message out of the possible MAX_NUM_OF_SLEEP_MODE_DEFNS*/
    ST_PSC_DEFN PSC_Defn [MAX_NUM_OF_SLEEP_MODE_DEFNS]; /**<PSC definition structure array*/
}ST_SLEEPMODE_MSG;
/**
PHY State ENUM structure
*/
typedef enum _EN_PHY_SYNC_TYPE
{
	SYNC_ACHIEVED=1, /**< Basebad is in Phy Sync*/
	SYNC_TRYING_TO_SYNC, /**< Basebad is Trying to Sync*/
	SYNC_INVALID /**< Basebad is not Synced*/
} EN_PHY_SYNC_TYPE;
/**
RDM message structure
*/
typedef struct _ST_RDM_MSG
{
	UINT32 u32Addr; /**< RDM Address Information*/
    UINT32 u32Data; /**< Data at u32Addr*/
}ST_RDM_MSG, *PST_RDM_MSG;
/**
BS Tracking Info
*/
typedef struct _ST_BSTRACKING_INFO
{
	
	UINT32 u32NbrAdvIdx;  /**< Neighbour Index*/
	UINT32 u32PreambleId;  /**< Preamble Id*/
	UINT32 u32Frequency;  /**< Frequency of BS in 125kHz steps. */
	UINT32 u32Bandwidth;  /**< Bandwidth in Hz */
	UINT32 u32BSIdHigh; /**< Higher bytes of BSID. */
	UINT32 u32BSIdLow; /**< Lower bytes of BSID. */
	SINT32 s32R1CINR; /**< R1CINR value in 1.15.16 format. This field is valid only when the R1CINRValid bit in FlagBits shown below is set */
	SINT32 s32R3CINR; /**< R3CINR value in 1.15.16 format. This field is valid only when the R3CINRValid bit in FlagBits shown below is set */
	SINT32 s32RSSI;  /**< RSSI Value in 1.15.16 format.*/
	SINT32 s32ToF; /**< Relative Delay w.r.t Serving BS in 0.0625 OFDM sample steps*/
	SINT32 s32RTD; /**< RTD in OFDM sample steps. This field is valid only when the Serving BS indicator below is set*/
	UINT32 u32MeasDur;  /**< Measurement duration in frames*/
	UINT32 u32FlagBits; /**< Flag Bits representing validity and status of various fields. It represents a bitmap as shown below: 
           					 - Bit0:  Currently Tracking going on
           					 - Bit1:  Serving BS Id indicator
           					 - Bit2:  Previous Serving BS Id indicator
           					 - Bit3:  Reuse3 CINR valid
           					 - Bit4:  Reuse1 CINR valid
           					 - Bit5:  BSId valid
           					 - Bit6:  Reuse factor of the neighbor BS
           					 - Bit7:  HO REQ Trigger status
           					 - Bit8:  SCN REQ Trigger status
           					 - Bit9:  SCN REP Trigger status 
           					 - Bits 10 to 31 are reserved bits */
}ST_BSTRACKING_INFO, *PST_BSTRACKING_INFO;
/**
BS Tracker Info
*/
typedef struct _ST_BSTRACKER_INFO
{
	UINT32 u32BSCount; /**< Number of valid BSes found*/
	ST_BSTRACKING_INFO aBSLoggerInfo[MAX_BS_TRACKING_RECORDS]; /**< Array of #_ST_BSTRACKING_INFO of size #MAX_BS_TRACKING_RECORDS representing BSes found*/
}ST_BSTRACKER_INFO, *PST_BSTRACKER_INFO;
/**
Antenna Switching and Selection Message
*/

#define MAX_NUM_RX_GPIO_REG_USED	4 /**< Maximum number of GPIOs for Rx to be used */
#define MAX_NUM_TX_GPIO_REG_USED	2 /**< Maximum number of GPIOs for Tx to be used */
#define MAX_DURATION				32/**< Maximum number of Frames */

/**
Antenna Configuration Request/Response
*/
typedef struct _ST_GPIO_SETTING_INFO
{
	UINT32	u32NumOfRxGpios; /**< Total Number of Rx GPIOs*/
	UINT32	u32NumOfTxGpios; /**< Total Number of Tx GPIOs*/
	SINT32	s32RxGpios[MAX_NUM_RX_GPIO_REG_USED]; /**< This array contains the GPIO 
												  numbers used. Valid numbers 0 - 15
												  */
	SINT32	s32TxGpios[MAX_NUM_TX_GPIO_REG_USED]; /**< This array contains the GPIO 
												  numbers used. Valid numbers 0 - 15
												  */
	UINT32	u32ConfigSettingsFlag; /**< Valid values
								   - 0 - Get the GPIO configuration from Firmware
								   - 1 - Set the GPIO configuration from Firmware
								   */
	UINT32	u32ErrorStatus;	/**< 0 - Success, Error is returned in case of failures*/
} ST_GPIO_SETTING_INFO,*PST_GPIO_SETTING_INFO;
/**
Antenna Scanning Request
*/
typedef struct _ST_RSSI_INFO_REQ
{
	UINT32	u32RequestBSScan; /**< 0 - use ongoing scan, 1 - send scan request,
							  80000001 - Send Scan Request Repetitively (Infinit Scanning)
							  */
	UINT32	u32Iterations; /**< Number of scan iterations to be requested*/
	UINT32	u32NumPattern; /**< Number of Patterns to be scanned*/
	UINT32	u32GpioPatternInfo[MAX_NUM_RX_GPIO_REG_USED]; /**< Contains the bit pattern information 
														  based on the frames to be scanned
														  */
	UINT32	u32ScanDurInterleavingInterval; /**< This contains the scan duration and the interleaving 
											interval to be sent in the MOB_SCN-REQ request. 
											Bit[7:0] – Scan Duration in 5msec frames (Optional)
											Bit[15:8] – Interleaving Interval in 5msec frames (Optional)
											Bit[31:!5] – Reserved  
											*/
	UINT32	u32Reserved1; /**< Reserved fields for future use*/
	UINT32	u32Reserved2; /**< Reserved fields for future use*/
	UINT32	u32Reserved3; /**< Reserved fields for future use*/
}ST_RSSI_INFO_REQ,*PST_RSSI_INFO_REQ;
/**
Antenna Scanning Response
*/
typedef struct _ST_RSSI_INFO_IND
{
	UINT32	u32ActiveStatus; /**< Possible values:
							 - 0 - Scan Abort
							 - 1 - Active (Scan Completed)
							 - 2 - scan Ongoing
							 */
	UINT32	u32NumPattern; /**< Number of Patterns that were scanned */
	UINT32	u32GpioPatternInfo[MAX_NUM_RX_GPIO_REG_USED]; /**< Contains the bit pattern information 
														  for the frames that were scanned*/
	SINT32	siRssi_Rx1[MAX_DURATION]; /**< RSSI measured on Antenna 1 reported in dBm. 
									  The information that is filled into this array is 
									  for the actual frames that is scanned, and the 
									  maximum frames is limited to MAX_DURATION
									  */
	SINT32	siCinr_Rx1[MAX_DURATION]; /**< CINR measured on Antenna 1 reported in dB. 
									  The information that is filled into this array is 
									  for the actual frames that is scanned, and the maximum 
									  frames is limited to MAX_DURATION
									  */
	SINT32	siRssi_Rx2[MAX_DURATION]; /**< RSSI measured on Antenna 2 reported in dBm. 
									  The information that is filled into this array is 
									  for the actual frames that is scanned, and the 
									  maximum frames is limited to MAX_DURATION
									  */
	SINT32	siCinr_Rx2[MAX_DURATION]; /**< CINR measured on Antenna 2 reported in dB. 
									  The information that is filled into this array is 
									  for the actual frames that is scanned, and the maximum 
									  frames is limited to MAX_DURATION
									  */
	UINT32	u32FrameNumber[MAX_DURATION]; /**< Frame Numbers corresponding to the measurements 
										  reported for this Antenna Scanning Interval
										  */
	UINT32	u32CurrGpioPattern[MAX_NUM_RX_GPIO_REG_USED]; /** GPIO setting of the current antenna 
														  selection stored in the bit0 of the elements of the array
														  */
	SINT32	siCurrAntRssi_Rx1; /** RSSI on antenna1 with the currently chosen 
							   GPIO setting reported in dBm
							   */
	SINT32	siCurrAntCinr_Rx1; /**< CINR on antenna1 with the currently chosen 
								GPIO setting reported in dB
								*/
	SINT32	siCurrAntRssi_Rx2; /** RSSI on antenna2 with the currently chosen 
							   GPIO setting reported in dBm
							   */
	SINT32	siCurrAntCinr_Rx2; /**< CINR on antenna2 with the currently chosen 
								GPIO setting reported in dB
								*/
	UINT32	u32ErrorStatus; /**< If Error status is set for other than ANTENNA_SWITCHING_SUCCESS, 
							values in the fields should not be considered
							*/
}ST_RSSI_INFO_IND,*PST_RSSI_INFO_IND;
/**
Antenna Selection Request
*/
typedef struct _ST_ANTENNA_INFO_REQ
{
	UINT32	u32RxAntSettingValid; /**< Provides whether Rx Antenna selection (u32RxAntennaSetting Field) 
								  is valid or not.
								  - 0 - Invalid
								  - 1 - Valid
								  */
	UINT32	u32TxAntSettingValid; /**< Provides whether Tx Antenna selection (u32TxAntennaSetting Field) 
								  is valid or not.
								  - 0 - Invalid
								  - 1 - Valid
								  */
	UINT32	u32RxAntennaSetting[MAX_NUM_RX_GPIO_REG_USED]; /**< provides the Rx Antenna Setting Information, 
														  Only Bit [0] is valid
														  */
	UINT32	u32TxAntennaAutoSelect; /**< To enable/disable the auto Tx selection: 
									- 1 – Enable auto Tx selection, 
									- 0 - Disable auto Tx selection
									*/
	UINT32	u32TxAntennaSetting; /**< Antenna to be selected for Tx: 
								 - 0 – Initial selection is based on the better of the two antennas 
								 based on RSSI metric,
								 - 1 – select antenna returning RX1,
								 - 2 – select antenna returning RX2
								 */
	UINT32	u32AlphaAnt1; /**< Used to set IIR Averaging parameter on Antenna 1 in Q16 format*/
	UINT32	u32AlphaAnt2; /**< Used to set IIR Averaging parameter on Antenna 2 in Q16 format*/
	UINT32	s32HysThreshAnt1; /**< Used to set Switching Threshold on Antenna 1 in dB*/
	UINT32	s32HysThreshAnt2; /**< Used to set Switching Threshold on Antenna 2 in dB*/
}ST_ANTENNA_INFO_REQ,*PST_ANTENNA_INFO_REQ;
/**
Antenna Switching Status Codes
*/
typedef enum _ANTENNA_SWITCHING_STATUS_CODES
{
	ANTENNA_SWITCHING_SUCCESS = 0, 
	ANTENNA_SWITCHING_FEATURE_NOT_SUPPORTED,
	ANTENNA_SWITCHING_GPIO_NOT_CONFIGURED,
	ERROR_SCAN_RSP_TIMEOUT,
	ERROR_SCAN_RSP_DENIED,
	ERROR_ONGOING_SCANNING_DOES_NOT_EXISTS,
	ERROR_MODEM_SHUTDOWN,
	ERROR_IN_GPIO_INDEX,
	TX_ANTENNA_SETTING_INVALID,
	ANTENNA_SCANNING_REQ_INVALID,
	NET_ENTRY_IN_PROGRESS,
	ERROR_NO_PHY_SYNC,
	ERROR_RX_GPIO_DO_NOT_EXIST,
	ERROR_TX_GPIO_DO_NOT_EXIST
} ANTENNA_SWITCHING_STATUS_CODES;
/**
PLL Status ENUM structure
*/
typedef enum _EN_PLL_STATUS_TYPE
{
	PLL_VAL_LOCKED=1, /**< PLL State is LOCKED*/
	PLL_VAL_NOT_LOCKED, /**< PLL State is NOT LOCKED*/
	PLL_VAL_INVALID /**< PLL State is NOT KNOWN*/
} EN_PLL_STATUS_TYPE;

typedef enum _EN_CHIP_VER_TYPE
{
	MS120=0xbece0120,
	MS121=0xbece0121,
	MS130=0xbece0130,
	MS200=0xbece0200,
	MS300=0xbece0300
}EN_CHIP_VER_TYPE;

/****************************************************************************************/
/*									AUTH PARAMETER INDICATION							*/
/****************************************************************************************/
/**
Authentication parameter indication structure
*/
typedef struct _AUTH_PARAMETER_INDICATION
{
	UCHAR ucEAPMethodType;			/**< 0=EAP AKA, 1= EAP SIM*/
	UCHAR ucRequestType;			/**< 0 = Read Authentication Parameters Request, */
									/**< 1 = Write Authentication Parameters Request*/

	UCHAR ucAckNack;				/**< ACK = 0, NACK = 1*/
									/**< For Request Type = 0 and ACK: Payload = Authentication Parameters from nonvolatile memory,*/
									/**< For Request Type = 0 and NACK: Payload = currently none (later, one could include a cause)*/
									/**< For Request Type = 1 and ACK: Payload = currently none*/
									/**< For Request Type = 1 and NACK: Payload = currently none (later, one could include a cause)*/
	
	USHORT usAuthPayloadLength;	/**< Length of the AuthParamsPayload to be read*/
	UCHAR aucAuthPayload[MAX_VARIABLE_LENGTH-16]; /**<Authentication payload*/
}AUTH_PARAMETER_INDICATION, *PAUTH_PARAMETER_INDICATION;

/****************************************************************************************/
/*									WRITE AUTH PARAMETERS								*/
/****************************************************************************************/

/**
Write authentication parameter structure
*/
typedef struct _WRITE_AUTH_PARAMS
{
	UCHAR ucEAPMethodType;		/**< EAP Method Type*/
								/**< 0 = EAP AKA*/
								/**< 1 = EAP SIM*/
	USHORT usAuthParamsPayloadLength;	/**<  length of AuthParamsPayload to be written (below)*/
	UCHAR aucAuthParamsPayload[MAX_VARIABLE_LENGTH-16];	
}WRITE_AUTH_PARAMS, *PWRITE_AUTH_PARAMS;

/****************************************************************************************/
/*									AUTH READ VECTOR									*/
/****************************************************************************************/

typedef struct _AUTH_READ_VECTOR
{
	UCHAR ucWNID[16];
	UCHAR ucK[16];
	UCHAR ucOPc[16];
	UCHAR ucNonK[16];
	UCHAR ucNonOPc[16];

	UCHAR ucC1_5_R1_5[100];
	UCHAR ucPseudonymID[23];
	UCHAR ucSQNMS[192];

}AUTH_READ_VECTOR, *PAUTH_READ_VECTOR;

/****************************************************************************************/
/*									AUTH WRITE VECTOR									*/
/****************************************************************************************/

typedef struct _AUTH_WRITE_VECTOR
{
	UCHAR	aucSEC_SNYMID_ID[23];
	UCHAR	aucSEC_SQNMS_ID[6*32];
	UCHAR	aucATRLock[2];
}AUTH_WRITE_VECTOR, *PAUTH_WRITE_VECTOR;

#define AUTH_METHOD_EAP_AKA				0x0000
#define AUTH_METHOD_EAP_SIM 			0x0001

/**
ertPS grant change interval structure
*/
typedef struct _ERTPS_GRANT_SIZE_SET_REQ
{
	UINT8 u8MsgType; /**< 8 bit Message Type*/
	UINT8 u8Padding; /**< added for word alignment*/
	UINT16	u16VCID;/**< 16bit VCID*/
	UINT32	u32BytesPerUGI; /**< Number of payload bytes per unsolicited grant interval*/
	UINT32 u32SDUsPerUGI; /**< Number of SDU’s (complete plus fragmented) per unsolicited 
						      grant interval*/
}ERTPS_GRANT_SIZE_SET_REQ, *PERTPS_GRANT_SIZE_SET_REQ;

/**
Handover information structure
*/
typedef struct _HANDOVER_INFORMATION
{
   UINT8  u8CurrentBSID[6]; /**<Base Station ID of the Current Serving BS */
    UINT8  u8LastBSID[6]; /**< Base Station ID of the Last serving BS */
    UINT32 u32CurrentPreambleID;/**<Preamble ID of the Current Serving BS*/
    UINT32 u32LastPreambleID;/**<Preamble ID of the Last Serving BS  */
    UINT32 u32TotalHOSuccessful;/**< Total number of succesful handovers*/
	
    UINT32 u32TotalUnpredtiveHO;/**< Total number of unpredective Handovers performed */
    UINT32 u32HOCancels;/**< Total number of Handovers cancels*/
    UINT32 u32HORejects;/**< Total number of Handovers rejects*/
    UINT32 u32LastBSFrameNumberDuringHO;/**<Last Frame number reccevied on the Last serving BS */
    UINT32 u32CurrentBSFrameNumberDuringHO;/**< Current Frame number at the Serving BS */
    UINT32 u32HandOverLatency;/**< Last Handover Latency in ms*/
    UINT32 u32HOOptimizationTlv;/**< HO-Optimization TLV recevied during the last Handover in RNG_RSP */
    UINT32 u32Reserved;/**< Reserved*/
	
}ST_HANDOVER_INFORMATION, *PST_HANDOVER_INFORMATION;
/**
* Idle MODE Statistics Information Structure
*/
typedef struct _IDLEMODE_INFORMATION
{
    UINT8  u8CurrentBSID[6];/**<Base Station ID of the Current Serving BS */
	UINT8  u8Padding[2];/**<Padding bytes*/
    UINT32 u32ReEntryQCSLoc;/**<Location in code from where Reentry was triggered */
    UINT32 u32HOOptimizationTlv;/**<HO-Optimization TLV recevied during the last Idle Handover in RNG_RSP */
    UINT32 u32TotalIdlemodeShutdowns;/**<Total number of times the chip was shutdown  */
    UINT32 u32TotalIdleLocationUpdates;/**<Total number of location updates performed */
    UINT32 u32TotalReEntries;/**< Total number of Idlemode re-entries performed*/
    UINT32 u32PagingCycle;/**<Paging cycle of the BS*/
    UINT32 u32PagingOffset;	/**<Paging offset of the BS */
    UINT32 u32PagingGroupID; /**<Paging group ID of the current BS */
}ST_IDLEMODE_INFORMATION, *PST_IDLEMODE_INFORMATION;
typedef struct _ST_DUMP_DEBUG
{
	UINT	uiFunctionCode;
	UINT	uiFileCode;
	UINT	uiSetReset;
}ST_DUMP_DEBUG,*PST_DUMP_DEBUG;

/**
OMA DM tree level structure
*/
typedef struct _OMA_DM_TREE_LEVEL
{
	UINT32 u32FieldId; /**< Field ID as is specifed in OMADMUserAPI.h for each node/leaf of the structure specified in OMADMTreeStruct.h*/
	UINT32 u32FieldInstance; /**< 0 by default. If the Field ID is an array, this value is the array instance for the node*/
}OMA_DM_TREE_LEVEL,*POMA_DM_TREE_LEVEL;

/**
OMADM object structure
*/
typedef struct _OMADM_OBJECT_INFO
{
	UINT32	u32OMADMObjSize;	/**< Size of the Object which is element/node size. Refer to #SUBTYPE_OMADM_GET_PARAM_SIZE_REQ 
									API to get the OMA DM object size.
									*/
	SINT8   s8ErrorCode;	/**< Set by the library to indicate if the API call is successfull or not. Any non zero value should be considered as failure*/
	OMA_DM_TREE_LEVEL stTreeLevel[MAX_OMA_DM_TREE_LEVEL_DEPTH]; /**< Please refer to _OMA_DM_TREE_LEVEL */
	UINT32 u32NumOfLevel; /**< Number of valid depth levels specified in the tree strcture above.*/
	UINT8   au8OMADMObjVal[1];	/**< The value of the OMADM Parameter Object. Data size is w.r.t node/leaf size as is in OMADMTreeStruct.h*/
}OMADM_OBJECT_INFO, *POMADM_OBJECT_INFO;

/**
OMADM Param Size Fetch structure
*/
typedef struct _OMADM_PARAMETER_INFO
{
	OMA_DM_TREE_LEVEL stTreeLevel[MAX_OMA_DM_TREE_LEVEL_DEPTH];/**< Please refer to _OMA_DM_TREE_LEVEL */
	UINT32 u32NumOfLevel; /**< Number of valid depth levels specified in the tree strcture above.*/
	SINT8   s8ErrorCode;	/**< Set by the library to indicate if the API call is successfull or not. Any non zero value should be considered as failure*/
	UINT32	u32OMADMObjSize;	/**< Size of the Object which is element/node size. Refer to #SUBTYPE_OMADM_GET_PARAM_SIZE_REQ 
									API to get the OMA DM object size.
									*/
}OMADM_PARAMETER_INFO, *POMADM_PARAMETER_INFO;
/**
OMADM version info structure
*/
typedef struct _OMADM_PARAMETER_VERSION_CHECK
{
	UINT32 u32MajorVersion; /**< Major version of the OMADM API used. */
	UINT32 u32MinorVersion; /**<Minor  version of the OMADM API used. */
	
}OMADM_PARAMETER_VERSION_CHECK,*POMADM_PARAMETER_VERSION_CHECK;
/**
Security object IDs ENUM
*/
typedef enum _SEC_PARAM_ID_LIST
{
ENCRYPTION_STATUS_FLAG = 0,/**<Certificates Encryption Status Flag*/
BECEEM_ENCRYPTION_FLAG,/**<Beceem Encryption Flag*/
FIRMWARE_DECRYPTION_KEY, /**<Firmware Decryption Key */
CERTIFICATE1_DATA_LEN, /**<Certificate-1 Data Length*/
CERTIFICATE1_DATA, /**<Certificate-1 Data*/
PRIVATE_KEY_DATA1_LEN, /**<Private Key Data-1 Length*/
PRIVATE_KEY_DATA1, /**<Private Key Data-1 */
CERTIFICATE2_DATA_LEN, /**<Certificate-2 Data Length*/
CERTIFICATE2_DATA, /**<Certificate-2 Data*/
PRIVATE_KEY_DATA2_LEN, /**<Private Key Data-2 Length*/
PRIVATE_KEY_DATA2, /**<Private Key Data-2 */
}SEC_PARAM_ID_LIST,*PSEC_PARAM_ID_LIST;

/**
Security Object structure
*/
typedef struct _SEC_OBJECT_INFO
{
	UINT32	u32SecObjId;	/**< These are constant values. Please refer to #_SEC_PARAM_ID_LIST */
	UINT32	u32SecObjSize;	/**< Size of the Object ID #_SEC_PARAM_ID_LIST.*/
	SINT8   s8ErrorCode;	/**< The Error values (less than zero is Failed(<0)/Success(0)); Please set this to zero for Set requests.*/
	UINT8   au8SecObjVal[1];	/**< The value of the OMADM Parameter Object. The data format should follow OAM DM standard based on the Object Id.*/
}SEC_OBJECT_INFO, *PSEC_OBJECT_INFO;
/**
OMADM Param Size Fetch structure
*/
typedef struct _SEC_PARAMETER_INFO
{
	UINT32	u32SecObjId;	/**< These are constant values. Please refer to #_SEC_PARAM_ID_LIST */
	SINT32	s32SecObjSize;	/**< Output - Size of the Object ID as mentioned in the OMADM Specs.*/
}SEC_PARAMETER_INFO, *PSEC_PARAMETER_INFO;

/**
Security certificate and key info structure
*/
typedef struct _SEC_CERTIFICATE_AND_KEY_INFO
{
	UINT8 au8Key[MAX_KEY_SIZE_PER_CERTIFICATE]; /**< Security Key that can be used by application for encrypt/decrypt certificate data*/
	UINT32 u32KeyLength; /**<Length of the key stored in the au8Key Array*/
	UINT32 u32ID; /**< Key IDs possible is 1 or 2 only*/
	UINT32 u32CertifcateSize; /**< Certificate size */
	UINT8 au8Certificate[1]; /**<Certificate data */
}SEC_CERTIFICATE_AND_KEY_INFO,*PSEC_CERTIFICATE_AND_KEY_INFO;

/**
IMEI Info structure
*/
typedef struct _IMEI_INFO
{
	INT32 s32ErrorCode; /**<Error code. IMEI data is valid if this field is SUCCESS(0) else it is considered failure*/
	UINT8 au8IMEI[MAX_IMEI_SIZE]; /**<IMEI data array*/
}IMEI_INFO,*PIMEI_INFO;
/**
 Burst Decoded per MCS Structure
*/
typedef struct stPerMCSBurstsDecoded
{
	UINT32 BcmBasebandBurstRcvCCQpsk12Rate1; /**< Bursts received with QPSK 1/2 with repetition 1 */
	UINT32 BcmBasebandBurstRcvCCQpsk12Rate2; /**< Bursts received with QPSK 1/2 with repetition 2 */
	UINT32 BcmBasebandBurstRcvCCQpsk12Rate4; /**< Bursts received with QPSK 1/2 with repetition 4 */
	UINT32 BcmBasebandBurstRcvCCQpsk12Rate6; /**< Bursts received with QPSK 1/2 with repetition 6 */
	UINT32 BcmBasebandBurstRcvCCQpsk34; /**< Bursts received with CC QPSK 3/4 */
	UINT32 BcmBasebandBurstRcvCC16Qam12; /**< Bursts received with CC 16 QAM 1/2 */
	UINT32 BcmBasebandBurstRcvCC16Qam34; /**< Bursts received with CC 16 QAM 3/4 */
	UINT32 BcmBasebandBurstRcvCC64Qam12; /**< Bursts received with CC 64 QAM 1/2 */
	UINT32 BcmBasebandBurstRcvCC64Qam23; /**< Bursts received with CC 64 QAM 2/3 */
	UINT32 BcmBasebandBurstRcvCC64Qam34; /**< Bursts received with CC 64 QAM 3/4 */

	UINT32 BcmBasebandBurstRcvCTCQpsk12Rate1; /**< Bursts received with CTC QPSK 1/2 with repetition 1 */
	UINT32 BcmBasebandBurstRcvCTCQpsk12Rate2; /**< Bursts received with CTC QPSK 1/2 with repetition 2 */
	UINT32 BcmBasebandBurstRcvCTCQpsk12Rate4; /**< Bursts received with CTC QPSK 1/2 with repetition 4 */
	UINT32 BcmBasebandBurstRcvCTCQpsk12Rate6; /**< Bursts received with CTC QPSK 1/2 with repetition 6 */
	UINT32 BcmBasebandBurstRcvCTCQpsk34; /**< Bursts received with CTC QPSK 3/4 */
	UINT32 BcmBasebandBurstRcvCTC16Qam12; /**< Bursts received with CTC 16 QAM 1/2 */
	UINT32 BcmBasebandBurstRcvCTC16Qam34; /**< Bursts received with CTC 16 QAM 3/4 */
	UINT32 BcmBasebandBurstRcvCTC64Qam12;/**< Bursts received with CTC 64 QAM 1/2 */
	UINT32 BcmBasebandBurstRcvCTC64Qam23; /**< Bursts received with CTC 64 QAM 2/3 */
	UINT32 BcmBasebandBurstRcvCTC64Qam34; /**< Bursts received with CTC 64 QAM 3/4 */
	UINT32 BcmBasebandBurstRcvCTC64Qam56; /**< Bursts received with CTC 64 QAM 5/6 */
}stPerMCSBurstsDecoded;

typedef stPerMCSBurstsDecoded stMimoMatAPerMCSBurstsDecoded;
typedef stPerMCSBurstsDecoded stMimoMatBPerMCSBurstsDecoded;
typedef stPerMCSBurstsDecoded stNonMimoPerMCSBurstsDecoded;

/**
PER_DLZONE_PER_MCS Structure
*/
typedef struct _ST_PERDLZONE_PER_MCS_BURSTS_DECODED
{
	stMimoMatAPerMCSBurstsDecoded	tMimoMatA;
	stMimoMatBPerMCSBurstsDecoded	tMimoMatB;
	stNonMimoPerMCSBurstsDecoded	tNonMimo;	
}ST_PERDLZONE_PER_MCS_BURSTS_DECODED;
/**
MRU Information Structure
*/
typedef struct _ST_MRU_INFORMATION
{
	UINT32	mruindex;/**< MRU index */
	UINT32	freqList[3];/**<  Frequency list Primary[0],Secondary1[1] and Secondary2[2]*/
	UINT32	bwInfo;/**< Bandwidth*/
}ST_MRU_INFORMATION, *PST_MRU_INFORMATION;

/**
NSP search status
*/
typedef enum _NSP_SEARCH_STATUS
{
	NSP_DISCOVERY_SUCCESS = 0,/**<Discovery is successful*/
	NSP_DISCOVERY_NOT_NEEDED = 1,/**<Discovery is not required*/
	NSP_16G_NOT_SUPPORTED = 2,/**<802.16g is not supported*/
	NSP_SEARCH_FAILURE = 3 ,/**<Search Failed*/
	NSP_DISCOVERY_ABORT = 4/**<Discovery aborted*/,
    NSP_DISCOVERY_SCAN_DURATION_TIMEOUT = 5,/**<Discovery scan duration timed out*/
}NSP_SEARCH_STATUS,*PNSP_SEARCH_STATUS;
/**
NSP search discovery phy parameters info
*/
typedef struct _ST_NW_DISCOVERY_PHY_INFO
{
	UINT32 u32CF;/**<Center frequency in KHz*/
	UINT32 u32BW;/**<Bandwidth in KHz*/
	UINT32 u32Pid;/**<Preamble index*/
	UINT32 u32DCDChangeCount;/**<DCD change count*/
}ST_NW_DISCOVERY_PHY_INFO,*PST_NW_DISCOVERY_PHY_INFO;
/**
Config parameter structure for NSP search
*/
typedef struct _ST_CONFIG_INFO_PER_NAPID
{
	UINT32 u32Duration;/**<Duration is seconds for NSP search . 
						The user can specify an infinite timeout by setting this to 0. */
	UINT32 u32NAPID; /**<NAPID for search */
    UINT32 u32CountNumBS; /**<Number of BS count*/
    UINT32 u32NSPChangeCount;/**<NSP change count*/
    UINT32 Reserved;/**<Reserved field*/
    ST_NW_DISCOVERY_PHY_INFO  stPhyInfo[NUM_BS_PER_NAPID];/**<Phy info struct array of size #NUM_BS_PER_NAPID.  
														Please refer to #_ST_NW_DISCOVERY_PHY_INFO */

}ST_CONFIG_INFO_PER_NAPID,*PST_CONFIG_INFO_PER_NAPID;
/**
NSP list structure
*/
typedef struct _NSPLIST
{
	UINT32 u32NSPID;/**<NSP ID*/
	UINT32 u32NspNameLength;/**<NSP name length*/
	UINT8 au8NsmNameVerbose[128];/**<NSP name*/
	
}NSPLIST,*PNSPLIST;
/**
Phy parameter information for NSP search response
*/
typedef struct _PHY_INFO_NSP_RESP
{
	UINT32 u32PID;/**<Preamble index*/
	UINT32 u32CF;/**<Center frequency KHz*/
	UINT32 u32Bandwidth;/**<Bandwidth KHz*/
	SINT32 u32RSSI;/**<Received signal strength indication*/
	UINT32 u32CINR;/**<Carrier to interference noise ratio*/

}PHY_INFO_NSP_RESP,*PPHY_INFO_NSP_RESP;
/**
NSP info structure for NSP search response
*/
typedef struct _BSNSPINFO
{
	UINT8 BSID[8]; /**<Base station identifier*/
	UINT32 u32DCDChangeCount;/**<DCD change count*/
	UINT32 u32NSPChangeCount;/**<NSP change count*/
	UINT32 u32NumNSPEntries;/**<Number of NSP entries*/
	NSPLIST astNSPList[MAX_NSP_LIST_LENGTH]; /**<Array of NSPs found, Please refer to #_NSPLIST*/
	PHY_INFO_NSP_RESP stPHYINFO;/**<phy info for NSP search, refer to #_PHY_INFO_NSP_RESP*/
	
}BSNSPINFO,*PBSNSPINFO;
/**
NSP list structure for NSP search response
*/
typedef struct _BSNSPLIST
{
NSP_SEARCH_STATUS u32Status; /**< To indicate success or failure. Please refer to #_NSP_SEARCH_STATUS 	*/
UINT32 u32EndOfDiscoveryRsp; /**< Search discovery is complete. If this field is not set Application can expect more NSP list after discovery response is received.*/   
BSNSPINFO stBSInformation;	  /**< Information about NSP search done, Please refer to #_BSNSPINFO . Contents of this structure is valid only if u32Status is NSP_DISCOVERY_SUCCESS. Please refer to #_NSP_SEARCH_STATUS*/
}BSNSPLIST,*PBSNSPLIST;

/**
NSP based Net entry structure request
*/
typedef struct _NSP_BASED_NETENTRY_ST
{
	UINT32 u32NspIdValid; /**< Flag to indicate if NSP ID Is valid (TRUE(1)) or not (FALSE(0)).  */
	UINT32 u32NspId;/**< network service provider identifier (NSP ID)*/
	VENDOR_SPECIFIC_LINKPARAMS stVendorSpecificParam;/**< Vendor specific parameter. Refer to #_VENDOR_SPECIFIC_LINKPARAMS*/

}NSP_BASED_NETENTRY_ST,*PNSP_BASED_NETENTRY_ST;
/**
Modem status enum
*/
typedef enum _MODEM_STATUS_
{
	DEVICE_STATUS_IND, /**<Indicates the device status.  payload would be of #_DEV_STATUS*/
	CONNECTION_STATUS_IND,/**<Indicates the connection status.  payload would be of #_DEV_CONNECTION_PROGRESS_INFO*/
	LINK_STATUS_IND  /**<Indicates the link status.  payload would be of #_LINK_STATUS_REASON*/
}MODEM_STATUS,*PMODEM_STATUS;

/**
Modem status information structure
*/
typedef struct _ST_MODEM_STATE_INFO
{
	MODEM_STATUS eModemStatus;/**<Modem status #_MODEM_STATUS*/
	UINT32 u32Status;/**< Status payload corresponding to Modem status */
}ST_MODEM_STATE_INFO,*PST_MODEM_STATE_INFO;

/**
Network entry timing statistics structure
*/
typedef struct _ST_BCMBASEBAND_NwENTRY_TIMING_STATS
{
	UINT32	u32NwSearchTime;/**< Time taken for network search*/
	UINT32	u32PhySyncTime;/**< Time taken for Phy sync*/
	UINT32	u32MacSyncTime;/**< Time taken for MAC sync*/
	UINT32	u32RangingTime;/**< Time taken for Ranging*/
	UINT32	u32SbcTime;/**< Time taken for SBC */
	UINT32	u32AuthPhaseTime;/**< Time taken for Authentication */
	UINT32	u32RegNegTime;/**< Time taken for registration*/
	UINT32	u32ProvSfTime;/**< Time taken for provisional service flow addition*/
	UINT32	u32NwEntryTime;/**< Time taken for network entry*/
}ST_BCMBASEBAND_NwENTRY_TIMING_STATS, *PST_BCMBASEBAND_NwENTRY_TIMING_STATS;

/**
Pdus Decoded Per MCS for DL Zone
*/
typedef struct _ST_PER_MCS_PDUS_DECODED
{
	UINT32 BcmBasebandPduRcvCCQpsk12Rate1;
	UINT32 BcmBasebandPduRcvCCQpsk12Rate2;
	UINT32 BcmBasebandPduRcvCCQpsk12Rate4;
	UINT32 BcmBasebandPduRcvCCQpsk12Rate6;
	UINT32 BcmBasebandPduRcvCCQpsk34;
	UINT32 BcmBasebandPduRcvCC16Qam12;
	UINT32 BcmBasebandPduRcvCC16Qam34;
	UINT32 BcmBasebandPduRcvCC64Qam12;
	UINT32 BcmBasebandPduRcvCC64Qam23;
	UINT32 BcmBasebandPduRcvCC64Qam34;

	UINT32 BcmBasebandPduRcvCTCQpsk12Rate1;
	UINT32 BcmBasebandPduRcvCTCQpsk12Rate2;
	UINT32 BcmBasebandPduRcvCTCQpsk12Rate4;
	UINT32 BcmBasebandPduRcvCTCQpsk12Rate6;
	UINT32 BcmBasebandPduRcvCTCQpsk34;
	UINT32 BcmBasebandPduRcvCTC16Qam12;
	UINT32 BcmBasebandPduRcvCTC16Qam34;
	UINT32 BcmBasebandPduRcvCTC64Qam12;
	UINT32 BcmBasebandPduRcvCTC64Qam23;
	UINT32 BcmBasebandPduRcvCTC64Qam34;
	UINT32 BcmBasebandPduRcvCTC64Qam56;
}ST_PER_MCS_PDUS_DECODED;

typedef ST_PER_MCS_PDUS_DECODED ST_MIMO_MATA_PER_MCS_PDUS_DECODED;
typedef ST_PER_MCS_PDUS_DECODED ST_MIMO_MATB_PER_MCS_PDUS_DECODED;
typedef ST_PER_MCS_PDUS_DECODED ST_NON_MIMO_PER_MCS_PDUS_DECODED;



/**
Burst Transmitted Per MCS for UL Zone
*/
typedef struct _ST_PER_MCS_BURSTS_TRANSMITTED
{
	UINT32 BcmBasebandBurstsTxCCQpsk12Rate1;
	UINT32 BcmBasebandBurstsTxCCQpsk12Rate2;
	UINT32 BcmBasebandBurstsTxCCQpsk12Rate4;
	UINT32 BcmBasebandBurstsTxCCQpsk12Rate6;
	UINT32 BcmBasebandBurstsTxCCQpsk34;
	UINT32 BcmBasebandBurstsTxCC16Qam12;
	UINT32 BcmBasebandBurstsTxCC16Qam34;
	UINT32 BcmBasebandBurstsTxCC64Qam12;
	UINT32 BcmBasebandBurstsTxCC64Qam23;
	UINT32 BcmBasebandBurstsTxCC64Qam34;

	UINT32 BcmBasebandBurstsTxCTCQpsk12Rate1;
	UINT32 BcmBasebandBurstsTxCTCQpsk12Rate2;
	UINT32 BcmBasebandBurstsTxCTCQpsk12Rate4;
	UINT32 BcmBasebandBurstsTxCTCQpsk12Rate6;
	UINT32 BcmBasebandBurstsTxCTCQpsk34;
	UINT32 BcmBasebandBurstsTxCTC16Qam12;
	UINT32 BcmBasebandBurstsTxCTC16Qam34;
	UINT32 BcmBasebandBurstsTxCTC64Qam12;
	UINT32 BcmBasebandBurstsTxCTC64Qam23;
	UINT32 BcmBasebandBurstsTxCTC64Qam34;
	UINT32 BcmBasebandBurstsTxCTC64Qam56;
}ST_PER_MCS_BURSTS_TRANSMITTED;

typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_CSM_PILOTA_PER_MCS_BURSTS_TRANSMITTED;
typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_CSM_PILOTB_PER_MCS_BURSTS_TRANSMITTED;
typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_NON_CSM_PER_MCS_BURSTS_TRANSMITTED;

typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_CSM_PILOTA_PER_MCS_BURSTS_RCVD; /*This is for backward compatibility purpose only*/
typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_CSM_PILOTB_PER_MCS_BURSTS_RCVD; /*This is for backward compatibility purpose only*/
typedef ST_PER_MCS_BURSTS_TRANSMITTED ST_NON_CSM_PER_MCS_BURSTS_RCVD;/*This is for backward compatibility purpose only*/

typedef struct _ST_PER_MCS_PDUS_TRANSMITTED
{
    UINT32 BcmBasebandPduTxCCQpsk12Rate1;
    UINT32 BcmBasebandPduTxCCQpsk12Rate2;
    UINT32 BcmBasebandPduTxCCQpsk12Rate4;
    UINT32 BcmBasebandPduTxCCQpsk12Rate6;
    UINT32 BcmBasebandPduTxCCQpsk34;
    UINT32 BcmBasebandPduTxCC16Qam12;
    UINT32 BcmBasebandPduTxCC16Qam34;
    UINT32 BcmBasebandPduTxCC64Qam12;
    UINT32 BcmBasebandPduTxCC64Qam23;
    UINT32 BcmBasebandPduTxCC64Qam34;

    UINT32 BcmBasebandPduTxCTCQpsk12Rate1;
    UINT32 BcmBasebandPduTxCTCQpsk12Rate2;
    UINT32 BcmBasebandPduTxCTCQpsk12Rate4;
    UINT32 BcmBasebandPduTxCTCQpsk12Rate6;
    UINT32 BcmBasebandPduTxCTCQpsk34;
    UINT32 BcmBasebandPduTxCTC16Qam12;
    UINT32 BcmBasebandPduTxCTC16Qam34;
    UINT32 BcmBasebandPduTxCTC64Qam12;
    UINT32 BcmBasebandPduTxCTC64Qam23;
    UINT32 BcmBasebandPduTxCTC64Qam34;
    UINT32 BcmBasebandPduTxCTC64Qam56;

}ST_PER_MCS_PDUS_TRANSMITTED;


typedef ST_PER_MCS_PDUS_TRANSMITTED ST_CSM_PILOTA_PER_MCS_PDUS_TRANSMITTED;
typedef ST_PER_MCS_PDUS_TRANSMITTED ST_CSM_PILOTB_PER_MCS_PDUS_TRANSMITTED;
typedef ST_PER_MCS_PDUS_TRANSMITTED ST_NON_CSM_PER_MCS_PDUS_TRANSMITTED;

/**
Per UL Zone Per MCS PDUs Info
*/
typedef struct _ST_PER_UL_ZONE_PER_MCS_PDU_INFO
{
    ST_CSM_PILOTA_PER_MCS_PDUS_TRANSMITTED tCSMPilotA;
    ST_CSM_PILOTB_PER_MCS_PDUS_TRANSMITTED tCSMPilotB;
    ST_NON_CSM_PER_MCS_PDUS_TRANSMITTED    tNonCSM;
}ST_PER_UL_ZONE_PER_MCS_PDU_INFO;


/**
Per UL Zone Per MCS Burst Info
*/
typedef struct _ST_PER_UL_ZONE_PER_MCS_BURST_INFO
{	
    ST_CSM_PILOTA_PER_MCS_BURSTS_TRANSMITTED	tCSMPilotA;
	ST_CSM_PILOTB_PER_MCS_BURSTS_TRANSMITTED	tCSMPilotB;
	ST_NON_CSM_PER_MCS_BURSTS_TRANSMITTED		tNonCSM;

}ST_PER_UL_ZONE_PER_MCS_BURST_INFO;

/**
Per DL Zone Per MCS Pdus Info
*/
typedef struct _ST_PERDLZONE_PER_MCS_PDUS_DECODED
{
	ST_MIMO_MATA_PER_MCS_PDUS_DECODED	tMimoMatA;
	ST_MIMO_MATB_PER_MCS_PDUS_DECODED	tMimoMatB;
	ST_NON_MIMO_PER_MCS_PDUS_DECODED	tNonMimo;
}ST_PERDLZONE_PER_MCS_PDUS_DECODED;

/**
DL Per Zone Structure
*/
typedef struct _ST_PERZONE_DL_INFO
{
    UINT32 	u32LastFrameNumber; /**<last frame in which DL allocation for the MS is found*/
    SINT8  	s8PCINRRate1; /**<MAT-A Physical CINR measurement if this is an STC zone or 
								Physical CINR measurement if this is non-STC zones*/
    SINT8 	s8PCINRRate2; /**<MAT-B Physical CINR measurement if this is an STC zone */
    SINT8   s8ECINRRate; /**<Effective CINR. The units of ECINR will be in terms of MCS (not dB)*/
    SINT8	Reserved0; // s8ECINRRate2 has been made reserved
    UINT32 	u32LastFrameNumberPCINR; /**<last frame in which PCINR is reported by the MS*/
    UINT32 	u32LastFrameNumberECINR; /**<last frame in which ECINR is reported by the MS*/
    UINT32 	Reserved1; 
    UINT32 	Reserved2; 
    ST_PERDLZONE_PER_MCS_BURSTS_DECODED	stPerDLZonePerMCSBursts; /**< cumulative number of bursts received by the MS per MCS in this DL zone  */
    ST_PERDLZONE_PER_MCS_PDUS_DECODED	stPerDLZonePerMCSPdus; /**< cumulative number of PDUs received by the MS per MCS in this DL zone  */
}ST_PERZONE_DL_INFO, *PST_PERZONE_DL_INFO;

/**
UL Per Zone Structure
*/
typedef struct _ST_PERZONE_UL_INFO
{
    UINT32 u32LastFrameNumber;
    UINT32 Reserved1;
    UINT32 Reserved2;
    UINT32 Reserved3;
    UINT32 Reserved4;
    ST_PER_UL_ZONE_PER_MCS_BURST_INFO	stPerULZonePerMCSBursts;
	ST_PER_UL_ZONE_PER_MCS_PDU_INFO stPerULZonePerMCSPdus;
}ST_PERZONE_UL_INFO, *PST_PERZONE_UL_INFO;

/**
DL/UL Per Zone Structure
*/
typedef struct _ST_PERZONE_INFO
{
	ST_PERZONE_DL_INFO	stPerDLZoneInfo[NUMBER_OF_DL_ZONE_TYPES]; /**< Array representing DL information of size #NUMBER_OF_DL_ZONE_TYPES.
																	The array contains DL MCS information for #NUMBER_OF_DL_ZONE_TYPES zones with array index
																	representing DL zones as follows:
																	 - 0 : Broadcast PUSC R1
																	 - 1 : MIMO PUSC R1
																	 - 2 : MIMO PUSC BF
																	 - 3 : PUSC BF
																	 - 4 : BAND AMC
																	 - 5 : BAND AMC BF
																	 - 6 : FUSC
																	 - 7 : Broadcast PUSC R3
																	 - 8 : MIMO PUSC R3
																	 - 9 : MBS
																	 */
	ST_PERZONE_UL_INFO	stPerULZoneInfo[NUMBER_OF_UL_ZONE_TYPES]; /**< Array representing UL information of size #NUMBER_OF_UL_ZONE_TYPES */
}ST_PERZONE_INFO;

/**
DL zone types
*/
typedef enum _DL_ZTYPES_
{
	DL_ZONETYPE_BC_PUSC_R1 =0,/**<Broadcast PUSC R1*/
	DL_ZONETYPE_MIMO_PUSC_R1=1,/**<MIMO PUSC R1*/
	DL_ZONETYPE_MIMO_PUSC_BF=2,/**< MIMO PUSC BF*/
	DL_ZONETYPE_PUSC_BF =3,/**<PUSC BF*/
	DL_ZONETYPE_BAMC=4,/**<BAND AMC*/
	DL_ZONETYPE_BAMC_BF=5,/**<BAND AMC BF*/
	DL_ZONETYPE_FUSC=6,/**<FUSC*/
	DL_ZONETYPE_BC_PUSC_R3=7,/**<Broadcast PUSC R3*/
	DL_ZONETYPE_MIMO_PUSC_R3=8,/**<MIMO PUSC R3*/
	DL_ZONETYPE_MBS = 9,/**<MBS*/
}DL_ZTYPES;

/**
UL zone types
*/
typedef enum  _UL_ZONE_TYPES
{
	SCH_ZONETYPE_UL_PUSC_NONSEGMENTED =0,/**<PUSC Non Segmented*/
	SCH_ZONETYPE_UL_PUSC_SEGMENTED =1,/**< PUSC Segmented*/
	SCH_ZONETYPE_UL_BAND_AMC_2BY3 =2,/**< Band AMC 2 BY3*/
}UL_ZONE_TYPES;

/**
Per Zone Stats Information
*/
typedef struct _ST_PER_ZONE_MCS_STATS
{
	UINT32	BcmBasebandMajorGroupIndicatorFCH; /**<MGI : only valid for system re-use=3. 0xFFxxxxxx indicates invalid */
    UINT8	BcmBasebandULAllocatedSubChannelsBitMap[9];
	UINT8	u8CINRMesurementIndic; /**<indicates if the measurement is active on CQICH and/or REP-REQ. 
										- 0 - No measurement
										- 1 - CQICH
										- 2 - REP_REQ */

    UINT8	u8Reserved[2];
    ST_PERZONE_INFO	stPerZoneInfo;
}ST_PER_ZONE_MCS_STATS;

/**
Set WiMAX Options structure
*/
typedef struct _ST_WIMAX_OPTIONS
{
	UINT8	u8WimaxOptionSpecifier; /**<WiMAX options specificier. 
										- BIT 0 : Network connect options
										- BIT 1 : MIB EEPROM writes control 
										- BIT 3 : MCS MIBS logging control
										- BIT 4 : EAP security level
										- BIT 5 : Idle mode paging cycle value
										- BIT 6 : BS out of coverage counter value in ms
										- BIT 7 : Reserved*/
	UINT32	u32WimaxOptionsPayload; /**<WiMAX options payload */
}ST_WIMAX_OPTIONS, *PST_WIMAX_OPTIONS;

/**
Set WiMAX Options All structure
*/
typedef struct _ST_WIMAX_OPTIONS_ALL
{
	INT32   i32Status; /**<Status field valid only for response APIs. 0 if successful else error code*/
	UINT8	u8WimaxOptionSpecifier;/**<WiMAX options specificier */
    UINT8	u8Reserved[3];/**<Reserved */
	UINT32	au32WimaxOptionsPayloadPerOpt[MAX_WIMAX_OPTIONS_POSSIBLE];/**<Array of #MAX_WIMAX_OPTIONS_POSSIBLE payloads, 
																		each payload representing each bit of options specifier */
	
	}ST_WIMAX_OPTIONS_ALL, *PST_WIMAX_OPTIONS_ALL;

typedef struct _ST_MCS_ZONE_INDEX_VALUE
{
	UINT32	u32McsChangeFlag;
	UINT32	u32McsBurstsVal;
	UINT32	u32McsPdusVal;
	UINT32	u32LastFrameNumber;
	SINT8	s8PCINRRate1;
	SINT8	s8PCINRRate2;
	SINT8	s8ECINRRate;
	UINT8	u8CINRMesurementIndic;
}ST_MCS_ZONE_INDEX_VALUE, *PST_MCS_ZONE_INDEX_VALUE;

/**
Scan Parameter Information.
*/
typedef struct _ST_BCMBASEBAND_ScanParams
{
	UINT32 u32ScanDuration; /**< Scan duration in TBD */
    UINT32 u32InterleavingInterval; /**< Interleaving interval */
}ST_BCMBASEBAND_ScanParams;

/**
CbyN Information.
*/
typedef struct _ST_BCMBASEBAND_CbyNTable
{
	//CbyN for regions
	SINT32 s32CbyNAckNackRegion;
	SINT32 s32CbyNFastFeedbackRegion;
	SINT32 s32CbyNRanging;
	SINT32 s32CbyNSounding;
	
	// CbyN for QPSK 
	SINT32 s32CbyNQpsk1by3;
	SINT32 s32CbyNQpsk1by2;
	SINT32 s32CbyNQpsk2by3;
	SINT32 s32CbyNQpsk3by4;
	
	// CbyN for 16QAM      
	SINT32 s32CbyN16QAM1by2;
	SINT32 s32CbyN16QAM2by3;
	SINT32 s32CbyN16QAM3by4;
	SINT32 s32CbyN16QAM5by6;
	
	// CbyN for 64QAM      
	SINT32 s32CbyN64QAM1by2;
	SINT32 s32CbyN64QAM2by3;
	SINT32 s32CbyN64QAM3by4;
	SINT32 s32CbyN64QAM5by6;
}ST_BCMBASEBAND_CbyNTable;

/**
NI Information.
*/
typedef struct _ST_BCMBASEBAND_NI
{
	SINT32 s32AckCQIPRRegionNI;
	SINT32 s32PUSCRegionNI;
	SINT32 s32OptionalPUSCRegionNI;
	SINT32 s32AMCRegionNI;
	SINT32 s32AASRegionNI;    
	SINT32 s32PeriodicRangingRegionNI;    
	SINT32 s32SoundingRegionNI;
	SINT32 s32MIMORegionNI;
}ST_BCMBASEBAND_NI;
/**
OMA-DM tree information
*/
typedef struct _ST_OMA_DM_TREE_INFO
{
	CHAR szOMADMTreeFile[128]; /**<Binary file path that contains OMA-DM tree to be downloaded to EEPROM*/
	
}ST_OMA_DM_TREE_INFO,*PST_OMA_DM_TREE_INFO;

/*
* MIB Stat Info Struct
*/
typedef struct _MIB_STATS_INFO
{
	UINT32	u32MibObjId;
    UINT8	u8MibType;
    UINT32	u32MibSize;
    UINT8	u8MibIndex;
    UINT32	u32MibKey[2];
    SINT8	s8ErrorCode;
    UINT8	au8MibObjVal[2048];
}MIB_STATS_INFO, *PMIB_STATS_INFO;

/**
enum RSA Padding Info
*/
typedef enum _PADDING_INFO
{
	RSA_NO_PADDING, /**<Indicates the No Padding is done.*/
	RSA_PKCS1_PADDING /**<Indicates the Padding based on PKCS1.*/
}PADDING_INFO; 

/**
* RSA Encrypt Private Structure
*/
typedef struct _ST_EAP_RSA_PRIVATE
{
	UINT32	u32DataLen; /**< Length of the data to be encrypted (App -> Library), 
						or the encrypted data (Library -> App).  
						Note that the encrypted data may be larger due to padding.*/
	UINT32	u32Padding; /**< The padding algorithm to be used 
						Padding values #_PADDING_INFO */
	UINT8	u8AESkey[32];/**< The AES symmetric key used to decrypt the device 
							certificate. Provided by the Host App in the 
							(App -> Library), all zeros in the (Library -> App).*/
	UINT8	u8Data[1]; /**< The data. 
							In the (App -> Library), data to be encrypted. 
							In the (Library -> App), it is the encrypted data.*/
}ST_EAP_RSA_PRIVATE, *PST_EAP_RSA_PRIVATE;

/**
* ENUM EAP Type
*/
typedef enum _EAP_TYPE 
{
	EAP_NONE =0,/**< EAP Type is None.*/
	EAP_TLS, /**< EAP Type is TLS.*/
	EAP_TTLS, /**< EAP Type is TTLS.*/
	EAP_AKA, /**< EAP Type is AKA.*/
	EAP_PSK, /**< EAP Type is PSK.*/
	EAP_SIM /**< EAP Type is SIM.*/
}EAP_TYPE_T;

/**
* EAP ENCRYPT PRIVATE Payload
*/
typedef struct _ST_EAP_ENCRYPT_PRIVATE_PAYLOAD
{
	EAP_TYPE_T	eapType; /**< Indicates which EAP Types #_EAP_TYPE */
	USHORT 		usEAPPayloadLength; /**< Indicates the EAP Payload Length. */
	UCHAR 	    aucEAPPayload[MAX_EAP_ENCRYPT_PAYLOAD_LENGTH]; /**<EAP Payload Data, Payload type: #_ST_EAP_RSA_PRIVATE
																	Valid for EAP_TLS, EAP_TTLS only. */
}ST_EAP_ENCRYPT_PRIVATE_PAYLOAD;

/**
* TX ANTENNA SWITCH INFORMATION
*/

typedef struct _ST_TXANTSWITCH_INFORMATION
{

     UINT32 u32Reserved[6];				/**< Reserved */

     UINT32 u32EnableAutoTxSwitching;  /**< Antenna Selection mode in unsigned Q32.0 format. 
									   - 0 – Autonomous Switching Disabled,
									   - 1 – Autonomous switching Enabled */

     UINT32 u32TxAntennaState;   /**< Current TX Antenna in unsigned Q32.0 format.
								 - 0 – Antenna1,
								 - 1 – Antenna2 */


}ST_TXANTSWITCH_INFORMATION, *PST_TXANTSWITCH_INFORMATION;

/**
* enum Scan Power mode
*/

typedef enum _SCAN_POWER_MODE
{
    LOW_POW_NON_MIMO_SCAN_ANT1 = 100, /**< WiMAX network scanning will be done with low power mode using only Rx1 path.
    									   BSID detection will be done using single antenna.*/
	LOW_POW_NON_MIMO_SCAN_ANT2,	 	  /**< WiMAX network scanning will be done with low power mode using only Rx2 path.
    									   BSID detection will be done using single antenna.*/
    LOW_POW_MIMO_SCAN_ANT1, 		  /**< WiMAX network scanning will be done with low power mode using Rx1 path only but BSID detection
    									   (few frames in the normal mode)phase will be configured to double antenna*/
    LOW_POW_MIMO_SCAN_ANT2, 		  /**< WiMAX network scanning will be done with low power mode using Rx2 path only but BSID detection
    									   (few frames in the normal mode)phase will be configured to double antenna*/
    DEFAULT_SCAN_MODE				  /**< Default double antenna mode */
    
}SCAN_POWER_MODE;
/**
*  Scan Config param structure
*/
typedef struct _ST_SCAN_CONFIG_PARAM 
{
	SCAN_POWER_MODE eScanPowMode;  /**< Mode for scan request #_SCAN_POWER_MODE */
    UINT32       u32Rsvd1;		   /**< Reserved field*/
    UINT32       u32Rsvd2;		   /**< Reserved field*/
    UINT32       u32Rsvd3;		   /**< Reserved field*/
    
}ST_SCAN_CONFIG_PARAM, *PST_SCAN_CONFIG_PARAM;

/**
	Scan Config param status structure
*/
typedef struct _ST_SCAN_CONFIG_PARAM_STATUS
{
	UINT32	bStatus; /**<Status of config param request. 
						- 1 : successful
						- 0 : failed
						*/
	ST_SCAN_CONFIG_PARAM stScanConfigParam; /**<Config param settings. Valid only if status field is successful.
											Refer to #_ST_SCAN_CONFIG_PARAM for details.*/
}ST_SCAN_CONFIG_PARAM_STATUS,*PST_SCAN_CONFIG_PARAM_STATUS;

/**
	MRU add info structure
*/
typedef struct _ST_MRU_ADD_INFO
{
    UINT32  u32CenterFreq; /**< The center frequency to add to the MRU table*/
	UINT32	bwInfo;/**< Bandwidth*/
    UINT32  reserved1; /**< reserved for future use*/
    UINT32  reserved2; /**< reserved for future use*/
}ST_MRU_ADD_INFO, *PST_MRU_ADD_INFO;

/**
enum Trigger type
*/

typedef enum _TRIGGER_TYPE
{
	TRIGGER_TYPE_NOT_DEFINED = 0,	/**< Trigger type is not defined */
	TRIGGER_TYPE_RSSI, 				/**< Type of trigger gives higher weight to RSSI */
	TRIGGER_TYPE_CINR	   			/**< Type of trigger gives higher weight to CINR */
	
}TRIGGER_TYPE;

/**
Trigger information structure
*/

typedef struct _ST_TRIGGER_INFO
{
	TRIGGER_TYPE eTriggerType; /**< Trigger type used #_TRIGGER_TYPE */
	SINT32 s32TriggerThreshold; /**< Threshold to be used for the #_TRIGGER_TYPE, in dB */

}ST_TRIGGER_INFO, *PST_TRIGGER_INFO;

/**
enum Trigger status	
*/

typedef enum _TRIGGER_STATUS
{
	TRIGGER_STATUS_FAILED = 0,  /**< Status is returned as failed in following scenarios:
									 - For ServingBS : If trigger of serving BS is greater than or equal to the  threshold set
									   by #TYPE_TRIGGER_INFO.
									 - For IntraFA group : If trigger set for any one of Base stations in the group
									   is greater than or equal to the threshold set by #TYPE_TRIGGER_INFO.
									 - For InterFA group : If trigger set for any one of Base stations in the group
									   is greater than  or equal to the threshold set by #TYPE_TRIGGER_INFO.  
								*/
								
	TRIGGER_STATUS_PASSED, 	   /**< Status is returned as passed in following scenarios:
									 - For ServingBS : If trigger set for serving BS is less than threshold set
									   by #TYPE_TRIGGER_INFO. 
									 - For IntraFA group : If trigger set for all the Base stations in the group 
									   is less than the threshold set by #TYPE_TRIGGER_INFO.
									 - For InterFA group : If trigger set for all the Base stations in the group 
									   is less than the threshold set by #TYPE_TRIGGER_INFO and all the base stations
									   in the group are being tracked. */

	
	TRIGGER_STATUS_NO_MEASUREMENTS,	/**< 
										 - Status for servingBS, IntraFA group and InterFA group
										is returned as No Measurements Found if no measurements are found for BSs
										 - InterFA group status is considered as No Basestations found if any of BS
										in the group is not being tracked*/   
								
	TRIGGER_STATUS_NOT_DEFINED		/**< Status is returned as Not Defined if trigger type set is #_TRIGGER_TYPE::TRIGGER_TYPE_NOT_DEFINED.
									*/

}TRIGGER_STATUS;

/**
Trigger Status Information structure
*/

typedef struct _ST_TRIGGER_STATUS_INFO
{
	TRIGGER_STATUS eServingBSStatus; /**< Trigger status of serving BS*/
	TRIGGER_STATUS eIntraFANeighboursStatus; /**< Trigger status of IntraFA neighbours.
											IntraFA neighbours comprises BSs having same frequency as serving BS*/
	TRIGGER_STATUS eInterFANeighboursStatus; /**< Trigger status of InterFA neighbours.
											InterFA neighbours comprises BSs having different frequency than that of serving BS Frequency*/
	ST_TRIGGER_INFO stTriggerInfo; /**< Trigger information set by MIB requests #BCM_CFG_WIMAX_TRIGGER_TYPE and #BCM_CFG_WIMAX_TRIGGER_THRESHOLD.*/

}ST_TRIGGER_STATUS_INFO, *PST_TRIGGER_STATUS_INFO;

/**
enum UART IP Filter status
*/

typedef enum _IP_FILTER_STATUS
{
	IP_FILTER_STATUS_INVALID = 0, /**< UART IP Filtering status is invalid*/
	IP_FILTER_STATUS_SUCCESS = 1, /**< UART IP Filtering status is success*/	
	IP_FILTER_STATUS_NOT_SUPPORTED = 4 /**< UART IP Filtering status not supported*/

}IP_FILTER_STATUS;

/**
Packets Dropped By The Application
*/
typedef struct _ST_CONTROLMESSAGES_DROPPED
{
	UINT32 cm_responses; 
	UINT32 cm_control_newdsx_multiclassifier_resp;
	UINT32 link_control_resp;
	UINT32 status_rsp;
	UINT32 stats_pointer_resp;
	UINT32 idle_mode_status;
	UINT32 auth_ss_host_msg;	
	UINT32 low_priority_message;
}ST_CONTROLMESSAGES_DROPPED, *PST_CONTROLMESSAGES_DROPPED;

/**
Modem status enum
*/
typedef enum _E_BASEBAND_MODEM_STATUS_
{
 MODE_ACTIVE = 0,/**< Modem is active and running*/
 MODEM_CRASHED_HANG =1,/**< Modem is crashed or hung*/
 MODEM_IN_SHUTDOWN_MODE=2,/**< Modem is in shutdown state*/
 MODEM_IN_IDLE_MODE=3,/**< Modem is in idle mode state*/
 MODEM_FIRMWARE_DOWNLOAD_IN_PROGRESS=4,/**< Modem firmware download is in progress. */
}E_BASEBAND_MODEM_STATUS,*PE_BASEBAND_MODEM_STATUS;

/**
PHY info structure
*/
typedef struct _ST_PHY_INFO
{
   /** Center Frequency (in kHz)*/
   UINT32 u32CenterFreq;
   /** Bandwidth (in kHz)*/
   UINT32 u32Bandwidth;
   /** Bit Map of Preamble Id */
   UINT32 au32Preamble[PREAMBLE_ARRAY_SIZE];
}ST_PHY_INFO, *PST_PHY_INFO;
/**
Channel specification struct
*/
typedef struct _ST_CHANNEL_SPECIFICATION
{
	/** Number of Channels to scan */
	UINT32 u32NumChannels;
	/** Array of Channels to scan */
	ST_PHY_INFO stChannelList[MAX_NUM_CHANNELS];
}ST_CHANNEL_SPECIFICATION, *PST_CHANNEL_SPECIFICATION;

 
/**
Payload Parameter for CAPL based search request
*/
typedef struct _ST_CAPL_BASED_SEARCH_PARAMS
{
	/**Duration in seconds for the entire search. 
	The Firmware should abort the scan once this timer expires. 
	The user can specify an infinite timeout by setting this to 0. 
	*/
	UINT32 u32Duration; 
	/**Specifies whether the BS wants to report BS-by-BS as and when each BS is detected or a consolidated 
	list of BSes at the end of the search.
	- u32ReportType = 1 to provide BS by BS list followed by a consolidated list at
						the end of the search (to indicate the end of search). 
	- u32ReportType = 2 to provide consolidated list
	- u32ReportType = 3 to provide the details of first BS found*/
	UINT32 u32ReportType;
    /**	Number of NAP IDs included in the request */
	UINT32 u32NumNapID;
	/** Specifies the NAP ID or Operator ID filter for filtering the results.
	If u32NumNapID is set to 0, then all the detected BSs which satisfies CINR and 
	RSSI thresholds	given in this request will be reported. 
	*/
	UINT32 au32NapIdList[MAX_NUM_NAP_ID];
	/**Specifies the flags for customizing the network search
	If bit 1 is set, device reports CINR and RSSI on both antennae
	*/
	UINT32 u32Flags;
    /** CINR Threshold */
    INT32  iCinrThreshold;
	/* RSSI Threshold */
	INT32  iRssiThreshold;
	/**Specifies the details of the channels included in the request */ 
    ST_CHANNEL_SPECIFICATION stChannelSpec;
}ST_CAPL_BASED_SEARCH_PARAMS, *PST_CAPL_BASED_SEARCH_PARAMS;

/**
detected base stations struct
*/
typedef struct _ST_DETECTED_BS_INFO
{  
	BYTE   bCompressedMap; /** Indicates whether the BS has compressed maps or normal maps  */
	BYTE   bFullBsid; /** Indicates whether the BSID in the response is complete BSID or
					  partial BSID (in the case of compressed maps).For normal maps this will always be TRUE*/
	UINT8  au8Bsid[6]; /** BS ID */
	UINT32 u32PreambleIndex; /** Preamble ID */
	UINT32 u32CenterFreq; /** Center Frequency */
    UINT32 u32Bandwidth; /** Bandwidth */
	SINT32 s32CINR[2];  /** CINR measurement values */
	SINT32 s32RSSI[2];  /** RSSI measurement values */	
	UINT32 u32Index; /** Index within the search request that the BS has been detected at*/

}ST_DETECTED_BS_INFO, *PST_DETECTED_BS_INFO;

/**
CAPL based search response
*/
typedef struct _ST_CAPL_SEARCH_RSP
{
	/** Indicates whether the search is complete or not */
	UINT32 u32SearchCompleteFlag;
	/** Indicates the Error Status  0 - Success */
	UINT32 u32ErrorStatus;
	/** Number of detected BSs */
	UINT32 u32NumBS;
	/** Details of detected BSs */
	ST_DETECTED_BS_INFO bsInfo[MAX_NUM_BS]; 

}ST_CAPL_SEARCH_RSP, *PST_CAPL_SEARCH_RSP;

/**
Log Notify Buffer
*/
typedef struct _ST_LOG_NOTIFY_BUFFER
{
	UINT32 u32TotalSize; /**< TotalSize in Bytes*/
	UCHAR u8Logbuffer[1]; /**<Log buffer*/

}ST_LOG_NOTIFY_BUFFER, *PST_LOG_NOTIFY_BUFFER;


/**
Proximity Sensor control message enum
*/
typedef enum _PROXIMITY_SENSOR
{	
E_PS_INIT=1,	/**< Initialize Proximity Sensor. Input Payload Structure #_ST_PS_INIT */
E_PS_ENBLE,		/**< Enable Proximity Sensor */
E_PS_DISABLE,	/**< Disable Proximity Sensor */
E_PS_STATUS,	/**< Proximity Sensor Status. Output Payload structure #_ST_PS_OBJECT_STATUS_RESP */
E_PS_WRITE,		/**< Proximity Sensor Write. Input Payload Structure #_ST_PS_WRITE*/
E_PS_READ,		/**< Proximity Sensor Read. Input Payload Structure #_ST_PS_READ_REQ Output Payload Structure #_ST_PS_READ_RESP */
E_PS_SHUT_DOWN,	/**< Proximity Sensor shutdown */
E_PS_WAKE_UP,	/**< Proximity Sensor Wakeup*/
E_PS_CALL_BACK,	/**< Proximity Sensor callback. Input Payload Structure #_ST_PS_CALL_BACK_REQ Output Payload Structure #_ST_PS_OBJECT_STATUS_RESP */
E_PS_STATUS_POLL,/**< Proximity Sensor Status poll. Output Payload Structure #_ST_PS_OBJECT_STATUS_RESP*/
E_PS_POLL,		/**< Proximity Sensor Poll. Input Payload Structure #_ST_PS_OBJECT_STATUS_RESP */
E_IS_PS_INSTALL,/**< Proximity Sensor Install. Output Payload Structure #_ST_IS_PS_INSTALL_RESP*/
E_PS_INTR_CLEAR,/**< Proximity Sensor  Clear*/
E_PS_ALGO_ENABLE,/**< Input Payload Enable*/
E_PS_ALGO_DISABLE,/**< Input Payload Disable*/
E_PS_ALGO_CONFIG  /**< Input Payload Configuration. Input Payload Structure #_ST_PS_ALGO_CONFIG */
}E_PROXIMITY_SENSOR,*P_E_PROXIMITY_SENSOR;





/**
Payload structure for Proximity Sensor 
*/
typedef struct _ST_PROXIMITY_CONTROL
{
    E_PROXIMITY_SENSOR eProximityControl; /**< refer to #_PROXIMITY_SENSOR */
    UINT32 uilength;                     /**< Length of the payload data*/
    UCHAR auchPayload[1];                /**< Payload for specific #_PROXIMITY_SENSOR message*/
 }ST_PROXIMITY_CONTROL,*PST_PROXIMITY_CONTROL;



/**
Payload for PS_INIT Request

*/
typedef struct _ST_PS_INIT
{
    UINT32 u32SensorId;
    UINT32 u32AddrFormat;
    UINT32 u32DataFromat;
    UINT32 u32InterfaceType;
    UINT32 u32I2CSlave;
    UINT32 u32GpioOrSpio;
    UINT32 u32NumOfAddrDataPair;
    UCHAR  ucAddrDataPair[1];
}ST_PS_INIT,*PST_PS_INIT;



/**
Payload for PS_STATUS Response

*/
typedef struct _ST_PS_OBJECT_STATUS_RESP
{
    UINT32 u32ObjectDetectStatus;  /**< 0-Not detected, 1- detected*/
}ST_PS_OBJECT_STATUS_RESP,*PST_PS_OBJECT_STATUS_RESP;




/**
Payload for PS_WRITE Request
*/

typedef struct _ST_PS_WRITE
{

    UINT32 u32WriteRegAddr; /**< Address of Register to be written */
    UINT32 u32WriteRegVal;	/**< Value to be written */
}ST_PS_WRITE,*PST_PS_WRITE;

  

/**
Payload for PS_READ Request
*/
typedef struct _ST_PS_READ_REQ
{
    UINT32 u32ReadRegAddr; /**< Address of Register to be read*/
}ST_PS_READ_REQ,*PST_PS_READ_REQ;

/**
Payload for PS_READ Response
*/
typedef struct _ST_PS_READ_RESP
{
    UINT32 u32ReadRegVal;    /**< Value at u32ReadRegAddr*/
}ST_PS_READ_RESP,*PST_PS_READ_RESP;

/**
Payload for PS_CALL_BACK Request
*/
typedef struct _ST_PS_CALL_BACK_REQ
{
    UINT32 u32ControlFwCallBack;  /**< 0 – Disable, 1-Enable*/
    UINT32 u32ControlHostCallBack;  /**< 0 – Disable, 1-Enable*/
    
}ST_PS_CALL_BACK_REQ,*PST_PS_CALL_BACK_REQ;


/**
Payload for IS_PS_INSTALL Resp
*/
typedef struct _ST_IS_PS_INSTALL_RESP
{
    UINT32 u32IsPSDevConnected;/**< 0-Not connected, 1- Connected Return if beceem HW with I2C has PS device connected */
}ST_IS_PS_INSTALL_POLL_RESP,*PST_IS_PS_INSTALL_POLL_RESP;

  


/**
Payload for PS_ALGO_CONFIG
*/
typedef struct _ST_PS_ALGO_CONFIG
{

    UINT32   u32SarMode;    /**< Proximity Sensor Available or Not Available */

    UINT32   u32SarBackoff;   /**< Enter this value in Q.2 format.*/

    UINT32   u32SarNoOfFdbkSamples;   /**< Enter the samples in Power of 2*/

}ST_PS_ALGO_CONFIG,*PST_PS_ALGO_CONFIG;




/*
Control Message Logging Request type
*/
typedef enum _CONTROLMSG_LOGGING
{
	CONTROLMSG_LOGGING_ENABLE,
	CONTROLMSG_LOGGING_DISABLE,
	CONTROLMSG_LOGGING_STATUS
}CONTROLMSG_LOGGING;

/*
Control Message Structure
*/
typedef struct _ST_CONTROL_MSG
{
	USHORT usReserved;
	USHORT usMsgLength;
	UCHAR ucControlMsg[MAX_MGMT_MESSAGE_LENGTH];

}ST_CONTROL_MSG,*PST_CONTROL_MSG;
#define DYN_LOGGING_MAX_PROCESSES	25
#define ONE				1

//--------------Changes here must be updated in BeceemWiMAXTestApp 's menu (#26)
// Debug Flags
#define DEBUG_OFF			0x0
#define DEBUG_COARSE		ONE << 0
#define DEBUG_FINE			ONE << 1

// Debug Paths
#define DEBUG_PATH_RX		ONE << 0
#define DEBUG_PATH_TX		ONE << 1
#define DEBUG_PATH_NVM		ONE << 2
#define DEBUG_PATH_CAPI		ONE << 3
#define DEBUG_PATH_RDM		ONE << 4
#define DEBUG_PATH_WRM		ONE << 5
#define DEBUG_PATH_OTHER	ONE << 6
#define DEBUG_PATH_ENDIAN_HANDLER	ONE << 7
#define DEBUG_PATH_ALL		0xffffffff

// Debug Where
#define LOG_STD_SCR			ONE << 0
#define LOG_DBG_VIEW		ONE << 1
#define LOG_FILE			ONE << 2
#define BECEEM_LOG_SYSLOG			ONE << 3	// currently logs to /var/log/messages in linux and /var/log/system.log in MACOS.
#define LOG_ALL				0xff
//---------------------------------------------------------------------

/* Enum mentioning process(es) to log */
typedef enum _E_DYN_LOGGING_FOR {
	PROCESS_SELF, /* Log current process */
	PROCESS_BY_PID, /* Log process mentioned by #_ST_DYNAMIC_LOGGING::uiPid */
	PROCESS_ALL /* Log all the processes using library */
} E_DYN_LOGGING_FOR;



typedef struct _ST_SHM_LOG_REC {
	PID 		nPid;			/* Process ID, 0 => record entry is free */
	UINT 		uDebugFlag;		/* DEBUG_OFF/DEBUG_COARSE/DEBUG_FINE(with HexDump) */
	UINT 		uDebugPath;		/* Bit mask to select debug paths */
	UINT 		uLogWhere;		/* Bit mask to select logging to bcm_sdk.log/console/syslog */
} ST_SHM_LOG_REC, *PST_SHM_LOG_REC;


/*
Dynamic Logging Structure
*/
typedef struct _ST_DYNAMIC_LOGGING {
	/* Enum to select process(es) for logging */
	E_DYN_LOGGING_FOR nLogFor;
	/* Process identifier to be logged */
	UINT uiPid;
	/* Number of valid records in #_ST_SHM_LOG_REC */
	UINT uiNumRecs;
	/* #_ST_SHM_LOG_REC contains dynamic logging attributes */
	ST_SHM_LOG_REC arrShmLogRecs[DYN_LOGGING_MAX_PROCESSES];
} ST_DYNAMIC_LOGGING, *PST_DYNAMIC_LOGGING;


/**
Data struct info per scheduling type
*/
typedef struct _ST_PER_SF_INFO{  
	UINT32	u32BcmBasebandSchType;/**<Scheduling type of the SF */
	UINT32  u32BcmBasebandNumOfCidsThisSF;/**<Number of CIDs belonging to the scheduling type*/
	UINT32	u32BcmBasebandCid[MAX_DL_OR_UL_SFS];/**<Maximum #MAX_DL_OR_UL_SFS list of CIDs belonging to the scheduling type*/
}ST_PER_SF_INFO ,*PST_PER_SF_INFO;

#pragma pack(pop)


#endif /* BCM_TYPEDEFS_H */
