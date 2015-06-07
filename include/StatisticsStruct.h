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
 * Description	:	Statistics structures and defines.
 * Author		:
 * Reviewer		:
 *
 */

/**
@file StatisticsStruct.h
*/

#ifndef STATS_STRUCT_H
#define STATS_STRUCT_H

#include "Typedefs.h"
/*
Number of forware header correction types
*/
#define NUMBER_OF_FEC_TYPES 29

#define MAX_SNMP_ADMIN_STRING		128
#define MAX_HISTOGRAM_INDEX			4
#define MAX_SAID_ENTRIES			12
#define MAX_OFDMAUIUCINDEX			10
#define MAX_OFDMADIUCINDEX			12

#define MAX_OFDMUIUCINDEX			12
#define MAX_OFDMDIUCINDEX			11
#define MAX_CPS_SERVICE_FLOWENTRIES	4	

#define NUM_OF_SUBMAPS 				3
/**
Maximum number of connections
*/
#define MAX_NUM_CONNECTIONS 16
/**
MAC state enum
*/
typedef enum MAC_STATES
{
    MAC_INIT_STATE=0,
    MAC_SYNC_STATE=1,
    MAC_NEGO_STATE=2,
    MAC_NORMAL_STATE=3,
    MAC_SLEEP_STATE=4,
    MAC_IDLE_STATE=5,
    MAC_HHO_STATE=6,
    MAC_FBSS_STATE=7
}MAC_STATES;

typedef enum  DL_ZONE_TYPES
{
DL_ZT_BC_PUSC=0,
DL_ZT_MIMO_PUSC=1,
DL_ZT_MIMO_PUSC_BF=2,
DL_ZT_PUSC_BF =3,
DL_ZT_BAMC=4,
DL_ZT_BAMC_BF=5,
DL_ZT_FUSC=6,
}DL_ZONE_TYPES;

#pragma pack (push,1)
/**
Control Plane statistics
*/
typedef struct  stCPStatistics {
/*  Ranging Statistics */
	/**  
	Description : 
		Number of uplink channels marked unusable.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberofULChannelsMarkedUnusable;
	/**  
	Description : 
		Number of downlink channels marked unusable.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberofDLChannelsMarkedUnusable;
	/**  
	Description : 
		Number of synchronization failures.
	
	Length: UINT8
	**/
	B_UINT8                  u8NumberOfSyncFailures;
	/**  
	Description : 
		Number of bandwidth requests sent to basestation.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberOfBandwidthRequestsSent; 

	/**  
	Description : 
		Current downlink channel descriptor count.
	
	Length: UINT8
	*/
	B_UINT8                  u8CurrentDCDChangeCount;
	
	/**  
	Description : 
		Current downlink channel descriptor count.
	
	Length: UINT8
	*/
	B_UINT8                  u8CurrentUCDChangeCount;
	/**  
	Description : 
		Number of uplink channel descriptors received.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberOfUCDsReceived;

	/**  
	Description : 
		Number of downlink channel descriptors received.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberOfDCDsReceived;
	/**  
	Description : 
		This field is not used. 
	
	Length: UINT8*2
	*/
	B_UINT8                  u8Unused[2];
	/**  
	Description : 
		Number of  Subscriber station basic capability request failures.
	
	Length: UINT8
	*/
	B_UINT8                  u8NumberofSBCFailures;
	/**  
	Description : 
		Number of authentication failures.
	
	Length: UINT8
	*/
	/**  Authentication failures */
	B_UINT8                  u8NumberofAuthenticationFailures;

}stCPStatistics;

/**
Map parser statistics 
*/
typedef struct  stMPStatistics {
	/* B_UINT32    u32FramesReceived;    */
	/**  
	Description : 
		Number of frame control header received.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfFchReceived ; 
	/**  
	Description : 
		Number of downlink maps processed.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfDlMapsProcessed; 
	/**  
	Description : 
		Number of uplink maps received.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfUlMapReceived; 
	/**  
	Description : 
		Number of bad maps received.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfBadMaps; 
	/**  
	Description : 
		Number of bad frame control headers received.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfBadFch; 
	/**  
	Description : 
		Number of no frame control headers received.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfNoFch;

}stMPStatistics;
/**
Data plane statistics 
*/
typedef struct  stDPStatistics {
	/**  
	Description : 
		Number of service data units received from the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfSdusFromHost;
	/**  
	Description : 
		Number of service data units sent to the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfSdusToHost;
	/**  
	Description : 
		Number of control packets received from the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfControlPktsFromHost;
	/**  
	Description : 
		Number of control packets received from the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfControlPktsToHost;
	/**  
	Description : 
		Number of protocol data units transmitted.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfPdusTransmitted;
	/**  
	Description : 
		Number of protocol data units received. It has both broadcast and unicast PDUs for the MS.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfPdusReceived;
	/**  
	Description : 
		Number of management packets transmitted.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfManagementPktsTransmitted;
	/**  
	Description : 
		Number of control packets received from the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfManagementPktsRecieved;
	/**  
	Description : 
		Number of transmitted packets rejected.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfTxPktsRejected;
	/**  
	Description : 
		Number of received packets rejected.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfRxPktsRejected;
	/**  
	Description : 
		Number of bytes received from the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumofBytesFromHost;
	/**  
	Description : 
		Number of bytes sent to the host.
	
	Length: UINT32
	*/
	B_UINT32    u32NumofBytesToHost;
	/**  
	Description : 
		Number of uplink connections.
	
	Length: UINT32
	*/
	B_UINT32    u32NumofULConnections;
	/**  
	Description : 
		Number of downlink connections.
	
	Length: UINT32
	*/
	B_UINT32    u32NumofDLConnections;

}stDPStatistics;
/**
Link statistics
*/

typedef struct  stLinkStats{
	/**  
	Description : 
		Number of frames in error.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfFramesInError;
	/**  
	Description : 
		Number of CRC errors.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfCrcErrors;
	/**  
	Description : 
		Number of header checksum errors.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfHcsErrors;
	/**  
	Description : 
		Number of channel quality indicator channel.
	
	Length: UINT32
	*/
	B_UINT32    u32NumOfCqichTxmtd;
	/**  
	Description : 
		Current RSSI.
	
	Length: INT32
	*/
	B_SINT32    s32CurrentRssi;
	/**  
	Description : 
		Current CINR.
	
	Length: INT32
	*/
	B_SINT32    s32CurrentCinr;
	/**  
	Description : 
		Current MAC state identifier.
	
	Length: UINT32
	*/
	B_UINT32    u32MacStateID;
	/**  
	Description : 
		Uplink ranging code.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkRangingCode;
	/**  
	Description : 
		Uplink ranging seed.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkRangingseed;
	/**  
	Description : 
		Power work mode that specifies operational mode of power control module.
	
	Length: UINT32
	*/
	B_UINT32    u32Powerworkmode;
	/**
	Description :
		Mimo mode reported by the MS
		
	Length: UINT32
	*/	
	B_UINT32	u32MimoModeReport;
	/**  
	Description : 
		RSSI deviation field.
	
	Length: UINT32
	*/
	B_UINT32    u32RSSIDeviationfield;
	/**  
	Description : 
		Number of bandwidth request header.
	
	Length: UINT32
	*/
	B_UINT32    u32NumBRTH;
	/**  
	Description : 
		Number of connection count.
	
	Length: UINT32
	*/
	B_UINT32    u32ConnCount;
	/**  
	Description : 
		Array of transport connection identifiers. 
	
	Length: UINT32 * #MAX_NUM_CONNECTIONS
	*/
	B_UINT32    u32TransportCid[MAX_NUM_CONNECTIONS];

}stLinkStats;


/**
LinkstatsEx
*/
typedef struct stLinkStatEx{
	/**  
	Description : 
		CINR mean.
	
	Length: INT32
	*/
	B_SINT32	s32MeanCinr;		 
	/**  
	Description : 
		CINR deviation.
	
	Length: INT32
	*/
	B_SINT32	s32DeviationCinr;	
	/**  
	Description : 
		Transmit reference power.
	
	Length: INT8
	*/	
	B_SINT8		s8TxReferencePower;	
	/**  
	Description : 
		Preamble index.
	
	Length: UINT8
	*/
	B_UINT8		u8PreambleIndex;
	/**  
	Description : 
		Downlink cell ID.
	
	Length: UINT8
	*/
	B_UINT8		u8DLIDCell;
	/**  
	Description : 
		Uplink cell ID.
	
	Length: UINT8
	*/
	B_UINT8		u8ULIDCell;		 
	/**  
	Description : 
		Initial ranging code end.
	
	Length: UINT8
	*/
	B_UINT8		u8IRCodeEnd;	 
	/**  
	Description : 
		Initial ranging code start.
	
	Length: UINT8
	*/
	B_UINT8		u8IRCodeStart;
	/**  
	Description : 
		Transmit power.
	
	Length: INT8
	*/
	B_SINT8		s8TxPower;
	/**  
	Description : 
		Transmit head room.
	
	Length: SINT8
	*/
	B_SINT8		s8TxPowerHeadRoom;
	/**  
	Description : 
		Bandwidth request code end.
	
	Length: UINT8
	*/
	B_UINT8		u8BRCodeEnd;
	/**  
	Description : 
		Bandwidth request code start.
	
	Length: UINT8
	*/	
	B_UINT8		u8BRCodeStart;
	/**  
	Description : 
		Periodic ranging code end.
	
	Length: UINT32
	*/
	B_UINT8		u8PRCodeEnd;
	/**  
	Description : 
		Periodic ranging code start.
	
	Length: UINT32
	*/	
	B_UINT8		u8PRCodeStart;		 
	/**  
	Description : 
		Unused
	
	Length: UINT8*2
	*/
	B_UINT8		u8Unused_3[2];			 
	/**  
	Description : 
		Handover ranging code end.
	
	Length: UINT8
	*/
	B_UINT8		u8HRCodeEnd;	 
	
	/**  
	Description : 
		Handover ranging code start.
	
	Length: UINT8
	*/
	B_UINT8		u8HRCodeStart;
	/**  
	Description : 
		Base station identifier.	
	
	Length: UINT8*6
	*/
	B_UINT8		au8BSID[6];
	/**  
	Description : 
		Unused.
	
	Length: UINT8*2
	*/
    B_UINT8		u8Unused2[2];

}stLinkStatEx, *pstLinkStatEx;

/**
Phy statistics
*/
typedef struct stPhyStateInfo {
	/**  
	Description : 
		Downlink map forward error correction schemes.
	
	Length: UINT8
	*/
	B_UINT8		u8DLMapFECScheme;
	/**  
	Description : 
		Uplink burst data forward error correction schemes.
	
	Length: UINT8
	*/
	B_UINT8     u8UplinkBurstDataFecScheme;
	/**  
	Description : 
		Downlink burst data zone.
	
	Length: UINT8
	*/
	B_UINT8     u8DownlinkBurstDataZone;
	/**  
	Description : 
		Uplink burst data zone.
	
	Length: UINT8
	*/
	B_UINT8     u8UplinkBurstDataZone;
	/**  
	Description : 
		Frame number.
	
	Length: UINT32
	*/
	B_UINT32    u32FrameNumber;
	/**  
	Description : 
		Local frame number.
	
	Length: UINT32
	*/
	B_UINT32    u32LocalFrameNumber;
	/**  
	Description : 
		Uplink burst data duration.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkBurstDataDuration;
	/**  
	Description : 
		Downlink burst data duration.
	
	Length: UINT32
	*/
	B_UINT32    u32DownlinkBurstDataDuration;
	/**  
	Description : 
		Uplink burst data size.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkBurstDataSize;
	/**  
	Description : 
		Downlink burst data size.
	
	Length: UINT32
	*/
	B_UINT32    u32DownlinkBurstDataSize;
	/**  
	Description : 
		Uplink ranging seed.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkRangingSeed;
	/**  
	Description : 
		Uplink burst data connection identifier.
	
	Length: UINT32
	*/
	B_UINT32    u32UplinkBurstDataCid;
	/**  
	Description : 
		Downlink burst data connection identifier.
	
	Length: UINT32
	*/
	B_UINT32    u32DownlinkBurstDataCid;
	/**  
	Description : 
		Forward error correction repetition used for downlink burst data.
	
	Length: UINT8
	*/
	B_UINT8     u8DownlinkBurstDataFecRepetition;
	/**  
	Description : 
		Forward error correction repetition used for uplink burst data.
	
	Length: UINT8
	*/
	B_UINT8     u8UplinkBurstDataFecRepetition;
	/**  
	Description : 
		Downlink interval usage code used for downlink burst data.
	
	Length: UINT8
	*/
	B_UINT8     u8DownlinkBurstDataDiuc;
	/**  
	Description : 
		Uplink interval usage code used for uplink burst data.
	
	Length: UINT8
	*/
	B_UINT8     u8UplinkBurstDataUiuc;		
	/**  
	Description : 
		Unused.
	
	Length: UINT8
	*/
	B_UINT8     u8Unused_2;
	/**  
	Description : 
		Forward error correction scheme used for downlink burst data.
	
	Length: UINT8
	*/
	B_UINT8     u8DownlinkBurstDataFecScheme;	
	/**  
	Description : 
		Downlink frame ratio.
	
	Length: UINT8
	*/
	B_UINT8     u8DlFrameRatio;
	/**  
	Description : 
		Uplink frame ratio.
	
	Length: UINT8
	*/
	B_UINT8     u8UlFrameRatio;
	
	
}stPhyStateInfo,*pstPhyStateInfo;

/**
Handover statistics
*/
typedef struct  stHoEventInfo {

	/**  
	Description : 
		Number of handover drops.
	
	Length: SINT32
	*/
	B_SINT32    s32NumOfHoDrops;
	/**  
	Description : 
		Number of hand over trials.
	
	Length: SINT32
	*/
	B_SINT32    s32NumOfHoTrials;
	/**  
	Description : 
		Ping pong count.
	
	Length: SINT32
	*/
	B_SINT32    s32PingPongCount;
	/**  
	Description : 
		Hand over type identifier.
		- FBSS = 0
		- HHO_INTERFA = 1
		- HHO_INTRAFA = 2
	
	Length: SINT32
	*/
	B_SINT32    s32HoTypeId;
	

	/**  
	Description : 
		Number of intra frequency neighbours.
	
	Length: SINT32
	*/
	B_SINT32    s32NumOfIntraFaNbrs;
	/**  
	Description : 
		Array of intra frequnecy preamble index.
	
	Length: SINT32 * #DM_MAX_NBRS
	*/
	B_SINT32    s32IntraFAPreambleIndex[DM_MAX_NBRS /**< Maximum number of neighbours*/];
	/**  
	Description : 
		Array of mean CINR intra frequency neighbour base stations.
	
	Length: SINT32 * #DM_MAX_NBRS
	*/
	B_SINT32    s32MeanCINRIntraFANbrBS[DM_MAX_NBRS /**< Maximum number of neighbours*/];
	/**  
	Description : 
		Number of inter frequency neighbours.
	
	Length: SINT32
	*/
	B_SINT32    s32NumOfInterFaNbrs;
	/**  
	Description : 
		Array of inter frequency preamble index.
	
	Length: SINT32 * #DM_MAX_NBRS
	*/
	B_SINT32    s32InterFAPreambleIndex[DM_MAX_NBRS /**< Maximum number of neighbours*/];
	/**  
	Description : 
		Array of mean CINR inter frequency neighbour base stations.
	
	Length: SINT32 * #DM_MAX_NBRS
	*/
	B_SINT32    s32MeanCINRInterFANbrBS[DM_MAX_NBRS/**< Maximum number of neighbours*/];
	/**  
	Description : 
		Hand over latency
	
	Length: SINT32
	*/

	B_SINT32    s32HOLatency;
}stHoEventInfo,*pstHoEventInfo ;

/**
Hand over status
*/
typedef enum _HO_STATUS
{
    HO_STATUS_NORMAL = 0,
    HO_STATUS_SCANNING = 1,
    HO_STATUS_SUCCESS = 2,
    HO_STATUS_FAILURE = 3
}HO_STATUS;
/**
Extended hand over information structure
*/
typedef struct stHoEventInfoEx
{

	/**  
	Description : 
		Serving base station preamble index.
	
	Length: UINT32
	*/
    B_UINT32    u32ServingBsPreambleIndex;
	/**  
	Description : 
		Serving base station sector ID.
	
	Length: UINT32
	*/
    B_UINT32    u32ServingBsSectorId;
	/**  
	Description : 
		Serving base station cell ID.
	
	Length: UINT32
	*/
    B_UINT32    u32ServingBsCellId;
	/**  
	Description : 
		Target base station preamble index.
	
	Length: UINT32
	*/
    B_UINT32    u32TargetBsPreambleIndex;
	/**  
	Description : 
		Target base station sector ID.
	
	Length: UINT32
	*/
    B_UINT32    u32TargetBsSectorId;
	/**  
	Description : 
		Target base station cell ID.
	
	Length: UINT32
	*/
    B_UINT32    u32TargetBsCellId;
	/**  
	Description : 
		Hand over status 
		-  No handover yet (HO_STATUS_NORMAL = 0) 
		- Currently MS is Scanning (HO_STATUS_SCANNING = 1) 
		- Handover was successful(HO_STATUS_SUCCESS = 2)
		- Handover was a failure (HO_STATUS_FAILURE = 3) 

	Length: UINT32
	*/
    B_UINT32    u32HOStatus;
	
}stHoEventInfoEx; 

/**
Error statistics
*/
typedef struct stErrorCounter{
	
	/**  
	Description : 
		Downlink map CRC errors.
	
	Length: UINT32
	*/
	B_UINT32 u32DLMAPCrcErrors;
	/**  
	Description : 
		Downlink map header checksum errors.
	
	Length: UINT32
	*/
	B_UINT32 u32DLMAPHcsErrors;
	/**  
	Description : 
		Protocol data unit CRC errors.
	
	Length: UINT32
	*/
	B_UINT32 u32PduCrcErrors;
	/**  
	Description : 
		Protocol data unit header checksum errors.
	
	Length: UINT32
	*/
	B_UINT32 u32PduHcsErrors;
	
}stErrorCounter;

/**
	Statistics structure
*/
typedef struct stStatistics_SS{

	
	/**  
	Description : 
		Size of statistics structure.
	
	Length: UINT32
	*/
	B_UINT32	u32SizeOfStatsStruct;
	/**  
	Description : 
		Number of frames received.
	
	Length: UINT32
	*/
	B_UINT32	u32NumOfFramesRecvd;
	/**  
	Description : 
		Link statistics structure.
	
	Length: sizeof(stLinkStats)
	*/
	stLinkStats	tLinkStats;
	/**  
	Description : 
		Control plane statistics.
	
	Length: sizeof(stCPStatistics)
	*/
	stCPStatistics	tCPStats;
	/**  
	Description : 
		Map parser statistics.
	
	Length: sizeof(stMPStatistics)
	*/
	stMPStatistics	tMPStats;
	/**  
	Description : 
		Data plane statistics.
	
	Length: sizeof(stDPStatistics)
	*/
	stDPStatistics	tDPStats;
	
	/**  
	Description : 
		Extended link statistics structure.
	
	Length: sizeof(stLinkStatEx)
	*/
	stLinkStatEx	tLinkStatsEx;
	/**  
	Description : 
		Array of downlink modulation coding schemes. It contains total number of unicast packets for MS.
	
	Length: UINT32 * #NUMBER_OF_FEC_TYPES
	*/
	B_UINT32	au32DLMCS[NUMBER_OF_FEC_TYPES/**< Number of forware header correction types*/];

	/**  
	Description : 
		Array of uplink modulation coding schemes.
	
	Length: UINT32 * #NUMBER_OF_FEC_TYPES
	*/
	B_UINT32	au32ULMCS[NUMBER_OF_FEC_TYPES/**< Number of forware header correction types*/];
	/**  
	Description : 
		Phy statistics structure.
	
	Length: sizeof(stPhyStateInfo)
	*/
	stPhyStateInfo	tPhyStateInfo;
	/**  
	Description : 
		Hand over statistics structure
	
	Length: sizeof(stHoEventInfo)
	*/
	stHoEventInfo	tHoEventInfo;
	/**  
	Description : 
		Modem state
	
	Length: UINT32
	*/
	B_UINT32	u32ModemState;
	/**  
	Description : 
		Center frequency
	
	Length: UINT32
	*/
	B_UINT32	u32CenterFrequency;
	/**  
	Description : 
		Frame number 
	
	Length: UINT32
	*/
	B_UINT32	u32FrameNumber;
	/**  
	Description : 
		Downlink data rate.
	
	Length: UINT32
	*/

	B_UINT32	u32DLRate;
	/**  
	Description : 
		Uplink data rate.
	
	Length: UINT32
	*/
	B_UINT32	u32ULRate;
	/**  
	Description : 
		Current Base station offset.
	
	Length: UINT32
	*/

	B_SINT32	s32CurrentOffsetBs;
	/**  
	Description : 
		Hand over extended statistics structure.
	
	Length: sizeof(stHoEventInfoEx)
	*/

	stHoEventInfoEx tHOEventInfoEx;
	/**  
	Description : 
		Phy sync global timer block count.
	
	Length: UINT32
	*/

	B_UINT32	u32PhySyncGtbCount;
	/**  
	Description : 
		Network entry global timer block count.
	
	Length: UINT32
	*/
	B_UINT32	u32NwEntryGtbCount;
	/**  
	Description : 
		Error counter structure.
	
	Length: sizeof(stErrorCounter)
	*/

	stErrorCounter tErrorCounter;
	

}stStatistics_SS;

#pragma pack(pop)


#endif /* STATS_STRUCT_H */
