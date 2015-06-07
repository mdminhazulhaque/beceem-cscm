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
 * Description	:	Host MIB interface.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef _HOST_MIBSINTERFACE_H
#define _HOST_MIBSINTERFACE_H


//#pragma pack(push,4)
#define MIBS_MAX_CLASSIFIERS 100
#define MIBS_MAX_PHSRULES 100
#define MIBS_MAX_SERVICEFLOWS 17
#define MIBS_MAX_IP_RANGE_LENGTH 4
#define MIBS_MAX_PORT_RANGE 4
#define MIBS_MAX_PROTOCOL_LENGTH   32
#define MIBS_MAX_PHS_LENGTHS	 255
#define MIBS_IPV6_ADDRESS_SIZEINBYTES 0x10
#define MIBS_IP_LENGTH_OF_ADDRESS	4
#define MIBS_MAX_HIST_ENTRIES 12
#define MIBS_PKTSIZEHIST_RANGE 128

typedef enum _eSchedType_t 
{
    DEFAULT = 0,
    Undefined = 1,
    BE = 2,
    NRTPS =3,
    RTPS = 4,
    ERTPS = 5,
    UGS  = 6    
} eSchedType_t;

typedef struct _IPV4_IP_ADDR
	{
		UINT	           ulIpv4Addr[MIBS_MAX_IP_RANGE_LENGTH];/* Source Ip Address Range  */
		UINT               ulIpv4Mask[MIBS_MAX_IP_RANGE_LENGTH];/* Source Ip Mask Address Range */
}IPV4_IP_ADDR;
typedef struct _IPV6_IP_ADDR
	{
		UINT				ulIpv6Addr[MIBS_MAX_IP_RANGE_LENGTH * 4];/* Source Ip Address Range  */
		UINT               ulIpv6Mask[MIBS_MAX_IP_RANGE_LENGTH * 4];/* Source Ip Mask Address Range */

}IPV6_IP_ADDR;

typedef struct _IPV4_IP_ADDR_IP_LENGTH
	{
		UCHAR				ucIpv4Address[MIBS_MAX_IP_RANGE_LENGTH * MIBS_IP_LENGTH_OF_ADDRESS];
		UCHAR				ucIpv4Mask[MIBS_MAX_IP_RANGE_LENGTH * MIBS_IP_LENGTH_OF_ADDRESS];
}IPV4_IP_ADDR_IP_LENGTH;
typedef struct _IPV6_IP_ADDR_IP_LENGTH
	{
		UCHAR				ucIpv6Address[MIBS_MAX_IP_RANGE_LENGTH * MIBS_IPV6_ADDRESS_SIZEINBYTES];
		UCHAR				ucIpv6Mask[MIBS_MAX_IP_RANGE_LENGTH * MIBS_IPV6_ADDRESS_SIZEINBYTES];
}IPV6_IP_ADDR_IP_LENGTH;
typedef union _U_MIBS_IP_ADDRESS
{
	IPV4_IP_ADDR stIPV4Addr;
	IPV6_IP_ADDR stIPV6Addr;
	IPV4_IP_ADDR_IP_LENGTH stIPV4AddrIPLength;
	IPV6_IP_ADDR_IP_LENGTH stIPV6AddrIPLength;
}U_MIBS_IP_ADDRESS;


typedef struct _S_MIBS_HOST_INFO
{
	ULONG64						GoodTransmits;
	ULONG64						GoodReceives;
	/*  this to keep track of the Tx and Rx MailBox Registers. */
	UINT						NumDesUsed;
	UINT						CurrNumFreeDesc;
	UINT						PrevNumFreeDesc;

	/*  to keep track the no of byte recieved */
	UINT						PrevNumRcevBytes;
	UINT						CurrNumRcevBytes;

	/* qos */
	
	UINT						BEBucketSize;
	UINT						rtPSBucketSize;
	UINT						LastTxQueueIndex;
	BOOLEAN						TxOutofDescriptors;
	BOOLEAN						TimerActive;
	UINT32						u32TotalDSD;
	UINT32						aTxPktSizeHist[MIBS_MAX_HIST_ENTRIES];
	UINT32						aRxPktSizeHist[MIBS_MAX_HIST_ENTRIES];
}S_MIBS_HOST_INFO;

typedef struct _S_MIBS_CLASSIFIER_RULE
{
	UINT				ulSFID;
	UCHAR               ucReserved[2];
	B_UINT16            usClassifierRuleIndex;
	BOOLEAN				bUsed;
	USHORT				usVCID_Value;
	B_UINT8             u8ClassifierRulePriority; // This field detemines the Classifier Priority
	U_MIBS_IP_ADDRESS   stSrcIpAddress;
	UCHAR               ucIPSourceAddressLength;//Ip Source Address Length
    
	U_MIBS_IP_ADDRESS   stDestIpAddress;
    UCHAR               ucIPDestinationAddressLength;//Ip Destination Address Length
    
    UCHAR               ucIPTypeOfServiceLength;/* Type of service Length */
    UCHAR               ucTosLow;/* Tos Low */
    UCHAR               ucTosHigh;/* Tos High */
	UCHAR               ucTosMask;/* Tos Mask */
    
    UCHAR               ucProtocolLength;/* protocol Length */
    UCHAR               ucProtocol[MIBS_MAX_PROTOCOL_LENGTH];/* protocol Length */
    
    USHORT				usSrcPortRangeLo[MIBS_MAX_PORT_RANGE];
	USHORT				usSrcPortRangeHi[MIBS_MAX_PORT_RANGE];
    UCHAR               ucSrcPortRangeLength;
	
    USHORT				usDestPortRangeLo[MIBS_MAX_PORT_RANGE];
	USHORT				usDestPortRangeHi[MIBS_MAX_PORT_RANGE];
    UCHAR               ucDestPortRangeLength;

	BOOLEAN				bProtocolValid;
	BOOLEAN				bTOSValid;
	BOOLEAN				bDestIpValid;
	BOOLEAN				bSrcIpValid;

	/* For IPv6 Addressing */
	/* UCHAR				ucSourceIP6Address[16]; */
	/* UCHAR				ucDestinationAddress[16]; */
	UCHAR				ucDirection;
	BOOLEAN             bIpv6Protocol;
	UINT32				u32PHSRuleID;
}S_MIBS_CLASSIFIER_RULE;


typedef struct _S_MIBS_PHS_RULE
{
	UINT                           ulSFID;
	/*  brief 8bit PHSI Of The Service Flow */
    B_UINT8                         u8PHSI;	
    /*  brief PHSF Of The Service Flow */
    B_UINT8                         u8PHSFLength;
    B_UINT8                         u8PHSF[MIBS_MAX_PHS_LENGTHS];
    /*  brief PHSM Of The Service Flow */
    B_UINT8                         u8PHSMLength;
    B_UINT8                         u8PHSM[MIBS_MAX_PHS_LENGTHS];
    /*  brief 8bit PHSS Of The Service Flow */
    B_UINT8                         u8PHSS;
    /*  brief 8bit PHSV Of The Service Flow */
    B_UINT8                         u8PHSV;
    /* Reserved bytes are 5, so that it is similar to S_PHS_RULE structure */
	B_UINT8	    					reserved[5];
	
	INT	    					PHSModifiedBytes;
	 /*  brief  PHS Modified Num Packets*/
	UINT       PHSModifiedNumPackets;
 /*  brief PHS Error Num Packets*/
	UINT       PHSErrorNumPackets;
	
}S_MIBS_PHS_RULE;

typedef struct _S_MIBS_EXTSERVICEFLOW_PARAMETERS
{
	UINT32 wmanIfSfid;
	UINT32 wmanIfCmnCpsSfState;
	UINT32 wmanIfCmnCpsMaxSustainedRate;
	UINT32 wmanIfCmnCpsMaxTrafficBurst;
	UINT32 wmanIfCmnCpsMinReservedRate;
	UINT32 wmanIfCmnCpsToleratedJitter;
	UINT32 wmanIfCmnCpsMaxLatency;
	UINT32 wmanIfCmnCpsFixedVsVariableSduInd;
	UINT32 wmanIfCmnCpsSduSize;
	UINT32 wmanIfCmnCpsSfSchedulingType;
	UINT32 wmanIfCmnCpsArqEnable;
	UINT32 wmanIfCmnCpsArqWindowSize;
	UINT32 wmanIfCmnCpsArqBlockLifetime;
	UINT32 wmanIfCmnCpsArqSyncLossTimeout;
	UINT32 wmanIfCmnCpsArqDeliverInOrder;
	UINT32 wmanIfCmnCpsArqRxPurgeTimeout;
	UINT32 wmanIfCmnCpsArqBlockSize;
	UINT32 wmanIfCmnCpsMinRsvdTolerableRate;
	UINT32 wmanIfCmnCpsReqTxPolicy;
	UINT32 wmanIfCmnSfCsSpecification;
	UINT32 wmanIfCmnCpsTargetSaid;
}S_MIBS_EXTSERVICEFLOW_PARAMETERS;


typedef struct _S_MIBS_SERVICEFLOW_TABLE
{
	 /* classification extension Rule */
    UINT				ulSFID;
    USHORT				usVCID_Value;
	UINT				uiThreshold;
	B_UINT8             u8TrafficPriority; /*  This field determines the priority of the SF Queues */
    
	
	BOOLEAN				bValid;
    BOOLEAN             bActive;
	BOOLEAN				bActivateRequestSent;
	
	B_UINT8			    u8QueueType;//BE or rtPS

	UINT				uiMaxBucketSize;//maximum size of the bucket for the queue
	UINT				uiCurrentQueueDepthOnTarget;
	UINT				uiCurrentBytesOnHost;
	UINT				uiCurrentPacketsOnHost;
	UINT				uiDroppedCountBytes;
	UINT				uiDroppedCountPackets;
	UINT				uiSentBytes;
	UINT				uiSentPackets;
	UINT				uiCurrentDrainRate;
	UINT				uiThisPeriodSentBytes;
#ifdef WIN32
	LARGE_INTEGER		liDrainCalculated;
#else
	ULONG64 liDrainCalculated;
#endif

	UINT				uiCurrentTokenCount;

#ifdef WIN32
	LARGE_INTEGER		liLastUpdateTokenAt;
#else
	ULONG64 liLastUpdateTokenAt;
#endif

	UINT				uiMaxAllowedRate;
	UINT                NumOfPacketsSent;
	UCHAR				ucDirection;
	USHORT				usCID;
	S_MIBS_EXTSERVICEFLOW_PARAMETERS	stMibsExtServiceFlowTable;
	UINT				uiCurrentRxRate;
	UINT				uiThisPeriodRxBytes;
	UINT				uiTotalRxBytes;
	UINT				uiTotalTxBytes;
}S_MIBS_SERVICEFLOW_TABLE;

typedef struct _S_MIBS_DROPPED_APP_CNTRL_MESSAGES
{
	UINT cm_responses;
	UINT cm_control_newdsx_multiclassifier_resp;
	UINT link_control_resp;
	UINT status_rsp;
	UINT stats_pointer_resp;
	UINT idle_mode_status;
	UINT auth_ss_host_msg;
	UINT low_priority_message;

}S_MIBS_DROPPED_APP_CNTRL_MESSAGES;

typedef struct _S_MIBS_HOST_STATS_MIBS
{
	S_MIBS_HOST_INFO					stHostInfo;
	S_MIBS_CLASSIFIER_RULE			    astClassifierTable[MIBS_MAX_CLASSIFIERS];
	S_MIBS_SERVICEFLOW_TABLE			astSFtable[MIBS_MAX_SERVICEFLOWS];
	S_MIBS_PHS_RULE						astPhsRulesTable[MIBS_MAX_PHSRULES];
	S_MIBS_DROPPED_APP_CNTRL_MESSAGES	stDroppedAppCntrlMsgs;
}S_MIBS_HOST_STATS_MIBS;
//#pragma pack(pop)


#endif /* _HOST_MIBSINTERFACE_H */
