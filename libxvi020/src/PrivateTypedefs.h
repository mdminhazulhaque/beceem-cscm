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
 * Description	:	Internal type definitions.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef PRIVATE_TYPEDEFS_H
#define PRIVATE_TYPEDEFS_H

#ifdef WIN32
	#pragma warning(disable : 4214) /*To avoid nonstandard extension used : bit field types other than int*/
	#pragma warning(disable : 4055 4054)/* To avoid functionpointer conversion waring*/
#endif

#include "Typedefs.h"
#ifndef WIN32
typedef UCHAR BOOLEAN,BOOL;
#endif
#include "PrivateStatisticsStruct.h"
#include "cntrl_SignalingInterface.h"


/********************************************************************/
/*				Macro To swap Buytes to avoid endian problem		*/
/********************************************************************/
#define SWAP_ENDIAN(A)((USHORT)((A>>8)|(A<<8)))
//#define SWAP_ENDIAN32(u32Value) ((B_UINT32)(((u32Value&0x000000FF)<<24)+ ((u32Value&0x0000FF00)<<8) + ((u32Value&0x00FF0000)>>8)+            ((u32Value&0xFF000000)>>24)))
#define CFG_MAKE_USERPARAM_SIZE(offset, size)		((offset << 16)|(size&0xFFFF))
#define MAX_FLASH_WRITE_MIB_BUFF_SIZE 200

/****************************************************************************************/
/*										LEADER											*/
/****************************************************************************************/


#pragma pack(push,1)
typedef struct _LEADER
{
	USHORT	usVcid;
	USHORT	usPayloadLength;
	UCHAR	ucStatus;
	UCHAR	ucUnused[3];

}LEADER,*PLEADER;

/****************************************************************************************/
/*									PACKET TO SEND TO DRIVER							*/
/****************************************************************************************/

typedef struct _PACKETTOSEND
{
	LEADER	Leader;
	DWORD	dwPayload;

}PACKETTOSEND,*PPACKETTOSEND;

/****************************************************************************************/
/*									PACKET TO SEND EXT		  							*/
/****************************************************************************************/
typedef struct _PACKETTOSENDEXT
{
	LEADER	Leader;
	UCHAR	dwPayload;
	UCHAR	szData[MAX_BUFFER_LENGTH];

}PACKETTOSENDEXT,*PPACKETTOSENDEXT;

/****************************************************************************************/
/*									MAC MANAGEMENT MESSAGE  							*/
/****************************************************************************************/




/****************************************************************************************/
/*									FIRMWARE INFO	  									*/
/****************************************************************************************/
typedef struct _FIRMWARE_INFO
{
	PVOID		pvMappedAddress;	
	UINT		u32Length;			
	UINT		u32StartAddress;		

}FIRMWARE_INFO, *PFIRMWARE_INFO;


/****************************************************************************************/
/*									CONFIG FILE INFO	  									*/
/****************************************************************************************/
typedef struct _CONFIGFILE_INFO
{
	PUCHAR		puCofigBuffer;	
	UINT32		u32Length;				

}CONFIGFILE_INFO, *PCONFIGFILE_INFO;



typedef struct _MAC_MGMT_MESSAGE
{
    UINT	uiIfaceMsgType;

	UINT	uiMgmtMsgDirection;
	UINT	uiFrameNumber;
	UINT	uiMsgRxTxed;

	UINT	uiPayloadLength;
	UINT	uiCID ;
	UINT	uiCmacOk;

	UCHAR	aucPayload[1];

}MAC_MGMT_MESSAGE, *PMAC_MGMT_MESSAGE;

/****************************************************************************************/
/*										RDM & WRM										*/
/****************************************************************************************/

#ifndef WIN32
#pragma pack(push,1)
typedef struct _RDM_BUFFER
{
	UINT	ulRegister;
	UINT	ulLength;
} RDM_BUFFER, *PRDM_BUFFER;

typedef struct _WRM_BUFFER
{
	UINT	ulRegister;
	UINT	ulLength;
	UCHAR	aucData[4];
} WRM_BUFFER, *PWRM_BUFFER;
#pragma pack(pop)
#endif

/****************************************************************************************/
/*									LINK STATUS	RESPONSE								*/
/****************************************************************************************/
typedef struct _LINK_STATE
{
      UCHAR		ucLinkStatus;
      BYTE		bIdleMode;
	  BYTE		bShutdownMode;
}LINK_STATE, *PLINK_STATE;

/****************************************************************************************/
/*									SYSTEM CONFIGURATION 								*/
/****************************************************************************************/
typedef struct _SYSTEM_CONFIGURATION
{
      B_UINT32 u32CfgVersion;

      /* Scanning Related Params */
      B_UINT32 u32CenterFrequency;
      B_UINT32 u32BandAScan;
      B_UINT32 u32BandBScan;
      B_UINT32 u32BandCScan;
      B_UINT32 u32minGrantsize;   // size of minimum grant is 0 or 6
      B_UINT32 u32PHSEnable;


      /* HO Params */
      B_UINT32 u32HoEnable;
      B_UINT32 u32HoReserved1;
      B_UINT32 u32HoReserved2;
      
      B_UINT32 u32MimoEnable;
      B_UINT32 u32SecurityEnable;
      
 
      B_UINT32 u32PowerSavingModesEnable; 
      B_UINT32 u32PowerSavingModeOptions; 
      B_UINT32 u32ArqEnable;
 

      B_UINT32 u32HarqEnable;
      B_UINT32 u32EEPROMLoc;
      B_UINT32 u32Customize;
//      B_UINT32 u32FirmwareOptions;       
      B_UINT32 u32ConfigBW;  /* In Hz */
      B_UINT32 u32ShutDownTimer;
//      B_UINT32 u32EncryptSupport;
      B_UINT32 u32RadioParameter;
      B_UINT32 u32PhyParameter1;
      B_UINT32 u32PhyParameter2;
      B_UINT32 u32PhyParameter3;
      B_UINT32 u32TestOptions; 
      B_UINT32 u32MaxMACDataperDLFrame;
      B_UINT32 u32MaxMACDataperULFrame; 
      B_UINT32 u32Corr2MacFlags;
      B_UINT32 HostDrvrConfig1;
      B_UINT32 HostDrvrConfig2;
      B_UINT32 HostDrvrConfig3;
      B_UINT32 HostDrvrConfig4;
      B_UINT32 HostDrvrConfig5;
      B_UINT32 HostDrvrConfig6;
	  B_UINT32 Segmented_PUSC_Enable;

} SYSTEM_CONFIGURATION,*PSYSTEM_CONFIGURATION;

typedef struct _ST_FLASH_PARAM_ID_WRITES_
{
	UINT32  u32ParamID;
	INT32    i32ReturnStatus;
	UINT32  u32Size;
	UCHAR ucData[MAX_FLASH_WRITE_MIB_BUFF_SIZE];
}ST_FLASH_PARAM_ID_WRITES,*PST_FLASH_PARAM_ID_WRITES;


#pragma pack(pop)

#define SYNC_UP			1
#define SYNC_DOWN		0

#define LINK_UP_TIMEOUT	0
#define LINK_UP			2
#define LINK_DOWN		3


#define AUTH_REQ_PAYLOAD	220
#define AUTH_RSP_PAYLOAD	221
#define AUTH_RSP_PAYLOAD_NAI_SPOOFING	208
#define SUBTYPE_TIME_ELAPSED_SINCE_NETWORK_ENTRY     6
#define AUTH_COMP_PAYLOAD	222
#define AUTH_ENCRYPT_PVT_REQ	181
#define AUTH_ENCRYPT_PVT_RESP	182



/*      swStatus for Requests control Message					*/
/****************************************************************/
#define CM_CONTROL_REQ                  0x80
#define STATUS_REQ						0x81
#define WTM_CONTROL_REQUEST				0x84
#define LINK_CONTROL_REQ				0x83
#define IDLE_MODE_CONTROL_REQ			0x8E
#define INIT_PARAM_CONTROL_REQ			0x8F
#define GET_SS_BS_INFO_REQ				0x85
#define GET_STATS_POINTER_REQ			0x86
#define AUTH_HOST_SS_MSG				0x88
#define SERVICE_FLOW_MSG				0x89
#define SLEEP_MODE_REQ  			    0x8A  // Sleep Mode Req Message
#define SERVICE_FLOW_ERTPS_GRANT_CHANGE_REQ	0x8D

/*		swStatus for Response control Message					*/
/****************************************************************/
#define CM_CONTROL_RESP					0xA0
#define STATUS_RSP						0xA1
#define WTM_CONTROL_RESPONSE			0xA2
#define LINK_CONTROL_RESP				0xA3
#define GET_SS_BS_CONTROL_RESP			0xA5
#define GET_STATS_POINTER_CONTROL_RESP  0xA6
#define MGMT_MSG_INFO_SW_STATUS			0xA7
#define IDLE_MODE_CONTROL_RESP			0xAE
#define INIT_PARAM_CONTROL_RESP		    0xAF
#define AUTH_SS_HOST_MSG				0xA8
#define SERVICE_FLOW_MSG_RESP			0xA9
#define SLEEP_MODE_IND 				    0xAA //Sleep Mode Response Message
#define MAC_LOGGING_NOTIFICATION		0xAB
#define CID_LOGGING_NOTIFICATION		0xAC


#define MGMT_MSG_INFO_DM				223


/*		Message Type to Reset Device Notification			*/
/************************************************************/
#define RESET_DEVICE				0xFE
#define UNLOAD_DEVICE				0xFF
#define APP_CLOSE_NOTIFICATION		0x00

#define LINK_MESSAGE_TYPE			0x0001

#define LINK_UP_REQ					0x0001
#define LINK_UP_ACK					0x0002
#define SYNC_UP_ACK					0x0002
#define LINK_UP_NAK					0x0003
#define LINK_DOWN_REQ				0x0004
#define LINK_DOWN_ACK				0x0005
#define LINK_DOWN_NAK				0x0006
#define LINK_UP_NOTIFICATION		0x0007
#define LINK_DOWN_NOTIFICATION		0x0008
#define LINK_UP_REQ_SYNC_UP			0x0001
#define LINK_UP_REQ_NET_ENTRY		0x0002

/*Payload to be send to device as a first byte	*/
#define LINK_UP_REQ_PAYLOAD				245
#define LINK_UP_REQ_SYNC_UP_PAYLOAD		1
#define LINK_UP_REQ_NET_ENTRY_PAYLOAD	2
#define LINK_UP_REQ_NSP_NET_ENTRY_PAYLOAD 3
#define LINK_UP_ACK_PAYLOAD				246
#define HANDOVER_MESSAGE				232
#define LINK_UP_NAK_PAYLOAD				0x00
#define DEVICE_STATUS_PAYLOAD			202
#define LINK_STATUS_IND_PAYLOAD         228
#define LINK_DOWN_REQ_PAYLOAD			226
#define SYNC_DOWN_REQ					1
#define SYNC_DOWN_NO_DREG_REQ           3
#define LINK_DOWN_DEREG_PAYLOAD			226
#define DEREG_REQ						2
#define SYNC_DOWN_AUTONOMOUS_SCAN       4

#define LINK_DOWN_ACK_PAYLOAD			226
#define LINK_DOWN_NAK_PAYLOAD			0x03
#define LINK_UP_NOTIFICATION_PAYLOAD	0x04
#define LINK_DOWN_NOTIFICATION_PAYLOAD	0x05
#define LINK_STATUS_REQ_PAYLOAD			228
#define INITATE_SHUTDOWN_PAYLOAD		204

#define MAC_ADDR_RESP_PAYLOAD			236
#define SS_INFO_RSP_PAYLOAD				235
#define SS_INFO_REQ_PAYLOAD    			235
#define HANDOVER_NOTIFICAITON_PAYLOAD	232

#define GET_STATISTICS_REQ_PAYLOAD		237
#define GET_STATISTICS_RESP_PAYLOAD		237

#define GO_TO_IDLE_MODE_PAYLOAD			200
#define GO_TO_IDLE_MODE_RSP_PAYLOAD		210
#define HOST_PSC_REQ                    188 
/* Network search request */
#define NW_SEARCH_QUERY					197
#define NW_SEARCH_REQ					198
#define NW_SEARCH_ABORT					199

#define SUBTYPE_AUTONOMOUS_BASED_NW_SEARCH_REQ 7

#define NW_SEARCH_TYPE_BCM_WIMAX        1
#define NW_SEARCH_TYPE_CAPL             2

#define SUBTYPE_BCM_WIMAX_NW_SEARCH_REQ		3
#define SUBTYPE_BCM_WIMAX_NW_SEARCH_RESP	3
#define SUBTYPE_BCM_WIMAX_NW_SEARCH_PER_ANTENNA	4
#define NW_SEARCH_SET_RSSI_CINR_REQ			5
#define NW_SEARCH_SET_RSSI_CINR_RESP		5
#define NW_SEARCH_LOW_POWER 				112
#define SUBTYPE_NW_SEARCH_LOW_POWER_REQ		1
#define SUBTYPE_NW_SEARCH_LOW_POWER_RESP	1
/* Network search response */
#define NW_SEARCH_RSP					198
#define NW_SEARCH_ACK					1
#define NW_SEARCH_NACK					0

/* Antenna request/responses */
#define ANTENNA_MSG						203
#define ANTENNA_CONFIG_REQ				3
#define ANTENNA_CONFIG_RSP				3
#define ANTENNA_SCANNING_REQ			4
#define ANTENNA_SCANNING_RSP			4
#define ANTENNA_SELECT_REQ				5
#define ANTENNA_SELECT_RSP				5

//MAC address write message 
#define SET_MAC_ADDR					193

//MAC address response 
#define		MAC_ADDR_RSP				236


/*Mac Address Payload		*/
#define MAC_ADDR_REQ_PAYLOAD  236
#define MAC_ADDR_RESP_PAYLOAD 236

#define SET_ERTPS_TX_MSG_CHANGE_REQ 189

#define SET_WIMAX_OPTIONS_REQ  190
#define	SET_WIMAX_OPTIONS_RSP  190

#define SET_WIMAX_OPTIONS_ALL_REQ  194
#define	SET_WIMAX_OPTIONS_ALL_RESP 194

#define SET_WIMAX_OPTIONS_ALL 1
#define GET_WIMAX_OPTIONS_ALL 2

#define INNER_NAI_MATCH_FAILURE 0x13 /*This code is received from firmware when Inner NAI parsing has failed*/

/*Connection Mangement Control (Service Flow) Message */

#define CM_MESSAGE_TYPE					0x0009	

/* Subtype	*/
#define CM_DSA_REQ						0x0001
#define CM_DSD_REQ						0x1001
#define CM_DSC_REQ						0x0101
#define CM_DEACTIVATE					0x0004

#define CM_DSA_RSP_PAYLOAD				0x0C
#define CM_DSA_ACK_PAYLOAD				0x0D
#define CM_DSC_RSP_PAYLOAD				0x0F
#define CM_DSC_ACK_PAYLOAD				0x10
#define CM_DSD_ACK_PAYLOAD				0x12

#define CM_DSA_REQ_PAYLOAD				0xB
#define CM_DSC_REQ_PAYLOAD				0xE
#define CM_DSD_REQ_PAYLOAD				0x11


/* AUTHENTICATION - Start	*/

#define AUTH_MESSAGE_TYPE				0x0020	

/*Subtype	*/
#define AUTH_REQ						0x0001
#define AUTH_RESP						0x0002
#define AUTH_COMPLETE					0x0003
#define AUTH_KEY_REQ					0x0005
#define AUTH_KEY_RESP					0x0006

#define OPERATION_TYPE_READ  			0x0001
#define OPERATION_TYPE_WRITE  			0x0002



#define READ_AUTH_PARAM_REQUEST			0x0000
#define WRITE_AUTH_PARAM_REQUEST		0x0001

/* AUTHENTICATION - END */


          

/* HALF MINI CARD  - Start */

#define HMC_REQUEST							245
#define SUBTYPE_HMC_STATUS_REQUEST			4
		
#define HMC_SHUTDOWN_REQ_FROM_FIRMWARE		16
#define HMC_WAKE_UP_NOTIFICATION_FRM_FW		17

/* HALF MINI CARD  - End   */

/* KDDI Host Implementation: IP Filtering Status*/
#define UART_IP_FILTER_QUERY_STATUS  		187


/* RESYNC LOG REQ*/

#define DISCONNECT_LOGGING_REQUEST     178
#define SUBTYPE_DISCONNECT_LOGGING_ENABLE_REQUEST 1
#define SUBTYPE_DISCONNECT_LOGGING_DISABLE_REQUEST 2


#define DISCONNECT_LOGGING_RESP         178
#define SUBTYPE_DISCONNECT_LOGGING_DISABLE_RESPONSE 0
#define SUBTYPE_DISCONNECT_LOGGING_ENABLE_RESPONSE 1
#define SUBTYPE_DISCONNECT_LOGGING_LOG_INDICATION_RESPONSE 2


#define START_CID_SNOOPING_REQ     179
#define SUBTYPE_CID_LOGGING_ENABLE_REQUEST 1
#define SUBTYPE_CID_LOGGING_DISABLE_REQUEST 2


#define START_CID_SNOOPING_RSP         179
#define SUBTYPE_CID_LOGGING_DISABLE_RESPONSE 0
#define SUBTYPE_CID_LOGGING_ENABLE_RESPONSE 1
#define SUBTYPE_CID_LOG_INDICATION_RESPONSE 2



typedef struct _ST_RESYNC_LOG_ADDR_SIZE
{
	UINT32 u32ADDR; /**< Address*/
	UINT32 u32TotalSize; /**< TotalSize in Bytes*/

}ST_RESYNC_LOG_ADDR_SIZE, *PST_RESYNC_LOG_ADDR_SIZE;



#define SUBTYPE_HIBERNATION_ENTRY_REQUEST 2


#define TYPE_PS_INFO           177


#define TYPE_PS_INFO_RESP      177


/*        SIZE of various structures */

extern const int SIZE_WIMAX_MESSAGE;
extern const int SIZE_stCPStatistics;
extern const int SIZE_stMPStatistics;
extern const int SIZE_stDPStatistics;
extern const int SIZE_stLinkStats;
extern const int SIZE_stLinkStatEx;
extern const int SIZE_stPhyStateInfo;
extern const int SIZE_stHoEventInfo;
extern const int SIZE_stStatistics_SS;
extern const int SIZE_PacketInfo;
extern const int SIZE_stMACMgmtMessage;

extern const int SIZE_WIMAX_MESSAGE;
extern const int SIZE_BSINFO;
extern const int SIZE_BSLIST;
extern const int SIZE_BSINFOEX;
extern const int SIZE_LEADER;
extern const int SIZE_PACKETTOSEND;
extern const int SIZE_PACKETTOSENDEXT;
extern const int SIZE_MAC_MGMT_MESSAGE;

extern const int SIZE_FIRMWARE_STATUS_INDICATION;

extern const int SIZE_CCPacketClassificationRuleSI;
extern const int SIZE_CPhsRuleSI;

extern const int SIZE_CConvergenceSLTypes;
extern const int SIZE_CServiceFlowParamSI;
extern const int SIZE_LocalSFAddRequest;
extern const int SIZE_SYSTEM_CONFIGURATION;
extern const int SIZE_SUBSCRIBER_STATION_INFO;
extern const int SIZE_LINK_STATUS;
extern const int SIZE_GPIO_INFO;
extern const int SIZE_GPIO_MULTI_INFO;
extern const int SIZE_GPIO_MULTI_MODE;

typedef struct _OMA_DM_NODE_INFO
{
	UINT32 u32Id;
	UINT32 u32ByteOffset;
	UCHAR  u8IsArray;
	UINT32 u32ElementSize;
	UINT32 u32numElements;
}OMA_DM_NODE_INFO;
typedef struct _OMA_DM_NODE_OFFSETINFO
{
	UINT32 u32RootOffset;
	UINT32 u32NumBytes;
	SINT32 s32ErrorCode;
}OMA_DM_NODE_OFFSETINFO,*POMA_DM_NODE_OFFSETINFO;
extern OMA_DM_NODE_INFO g_OmaDmNodeInfo[52][13];


typedef struct _ST_VENDOR_SPECIFIC_EXTN_RET
{
	INT32 iReturnValue; /*Vendor specific code return value. If this is not SUCCESS(0)
						Call returns to the applicaiton without continuing to execute the BeceemAPI for specific type/subtype*/
	UINT32 bContinueWithBCMPath;/* Should TRUE(1) by default. If the application don't want beceem API to be exectued 
								Then should this field to FALSE(0).
								If this field is FALSE SendWiMAXMessage function returns to the caller immediately without 
								executing BeceemAPI for specific TYPE/SUBTYPE
								*/
}ST_VENDOR_SPECIFIC_EXTN_RET,*PST_VENDOR_SPECIFIC_EXTN_RET;

/*NVM read write structure*/

typedef  struct _NVM_READWRITE
{

	PVOID pBuffer;/*Data to be written from|read to. Memory should be allocated by the caller.*/
 	UINT32  uiOffset;/* offset at which data should be written to or read from.*/
	UINT32  uiNumBytes;/*No. of bytes to be written or read.*/
	BOOL  bVerify;/*Applicable only for write. If set verification of written data will be done.*/
} NVM_READWRITE,*PNVM_READWRITE;



/*SF add indication structure for CC reject scenarios */

typedef struct _ST_SF_INDICATION_CC{

  B_UINT8                       	u8Type;       /**<  Type*/
  B_UINT8      						eConnectionDir;  /**<  Connection Direction*/                  
  B_UINT16                          u16TID;     /**<  TID*/     
  B_UINT16                          u16CID;     /**<  16bitCID*/
  B_UINT16                          u16VCID;     /**<  16bitVCID*/
  CServiceFlowParamSI              *psfAuthorizedSet;
  CServiceFlowParamSI              *psfAdmittedSet;    
  CServiceFlowParamSI              *psfActiveSet;

  B_UINT8                           u8CC;   /**< Confirmation Code*/       
  B_UINT8                           u8Padd; /**<  8-bit Padding */
  B_UINT16                          u16Padd; /**< 16 bit Padding */  

}ST_SF_INDICATION_CC,*PST_SF_INDICATION_CC;


typedef struct _ST_TIME_ELAPSED_
{
	ULONG64	ul64TimeElapsedSinceNetEntry;
    UINT32   u32Reserved[4]; //By chance if required for future proofing
}ST_TIME_ELAPSED,*PST_TIME_ELAPSED;

//
typedef enum _BCM_INTERFACE_TYPE
{
        BCM_MII,
        BCM_CARDBUS,
        BCM_USB,
        BCM_SDIO,
        BCM_PCMCIA,
        BCM_SHM
} BCM_INTERFACE_TYPE;

typedef enum eNVM_TYPE {
        NVM_AUTODETECT = 0,
        NVM_EEPROM,
        NVM_FLASH,
        NVM_UNKNOWN
} NVM_TYPE, *PNVM_TYPE;

typedef enum DEV_DRIVER_STATUS {
		DEV_UNKNOWN_DRIVER_STATE=0,
		DEV_IN_RESET=1,
		DEV_NOT_IN_RESET=2
} E_DEV_DRIVER_STATUS;	

typedef struct _DEVICE_DRIVER_INFO
{
        NVM_TYPE                        u32NVMType;
        UINT                            MaxRDMBufferSize;
        BCM_INTERFACE_TYPE              u32InterfaceType;
        UINT                            u32DSDStartOffset;
        UINT                            u32RxAlignmentCorrection;
        E_DEV_DRIVER_STATUS             u32DeviceStatus;  
        UINT                            u32Reserverd[9];
} DEVICE_DRIVER_INFO;


typedef struct bulkwrmbuffer 
{
        UINT   Register;
        UINT   SwapEndian;
        UINT   Values[1];

}BULKWRM_BUFFER,*PBULKWRM_BUFFER;

#pragma pack(push,4)
typedef struct _WIMAX_API_CONNECTION_STAT_4
{
	UINT32   structureSize;    /**< size of this structure. */
	ULONG64   totalRxByte;      /**< Total transmitted bytes of payload traffic*/
	ULONG64   totalTxByte;      /**< Total received bytes of payload traffic*/
	ULONG64   totalRxPackets;   /**< Total number of received packet. Equivalent to IP packet. */ 
    ULONG64   totalTxPackets;   /**< Total number of transmitted packet. Equivalent to IP packet. */
} WIMAX_API_CONNECTION_STAT_4, *WIMAX_API_CONNECTION_STAT_P_4;
#pragma pack(pop)

typedef struct stUserThreadReq
{
	
	UINT ThreadState; /** 0->Inactivate LED thread.
						  1->Activate the LED thread */
						  	
}USER_THREAD_REQ,*PUSER_THREAD_REQ;

#endif /* PRIVATE_TYPEDEFS_H */
