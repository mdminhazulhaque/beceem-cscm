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
 * Description	:	Internal statistics definitions.	
 * Author		:
 * Reviewer		:
 *
 */


#ifndef PRIVATE_STATS_STRUCT_H
#define PRIVATE_STATS_STRUCT_H

#include "Typedefs.h"
#include "StatisticsStruct.h"
#include "flashinfoaccess.h"
typedef  unsigned char WmanIfChannelNumber;
typedef  unsigned char wmanIfSsPkmTekDataEncryptAlg;
typedef  unsigned char wmanIfSsPkmTekDataAuthentAlg;
typedef  unsigned char wmanIfSsPkmTekEncryptAlg;
typedef unsigned char WmanIfBsIdType[6] ;
typedef unsigned int DateAndTime;
typedef unsigned int TruthValue;
typedef unsigned int Counter32 ;
typedef unsigned char SnmpAdminString[MAX_SNMP_ADMIN_STRING] ;
typedef unsigned char WmanIfMacVersion;

#define MAX_DFLASH_SIZE (64*1024*1024)
#define DEFAULT_SECTOR_SIZE                     (64*1024)



typedef struct wmanIfCmnCpsServiceFlowEntry
{
	B_UINT32 wmanIfCmnCpsSfId ; /* Index */
	B_UINT32 wmanIfCmnCpsSfCid;
	B_UINT32 wmanIfCmnCpsSfDirection;
	B_UINT32 wmanIfCmnCpsSfState;
	B_UINT32 wmanIfCmnCpsTrafficPriority;
	B_UINT32 wmanIfCmnCpsMaxSustainedRate ;
	B_UINT32 wmanIfCmnCpsMaxTrafficBurst ;
	B_UINT32 wmanIfCmnCpsMinReservedRate ;
	B_UINT32 wmanIfCmnCpsToleratedJitter; 
	B_UINT32 wmanIfCmnCpsMaxLatency; 
	B_UINT32 wmanIfCmnCpsFixedVsVariableSduInd;
	B_UINT32 wmanIfCmnCpsSduSize ;
	B_UINT32 wmanIfCmnCpsSfSchedulingType;
	B_UINT32 wmanIfCmnCpsArqEnable;
	B_UINT32 wmanIfCmnCpsArqWindowSize;
	B_UINT32 wmanIfCmnCpsArqBlockLifetime;
	B_UINT32 wmanIfCmnCpsArqSyncLossTimeout;
	B_UINT32 wmanIfCmnCpsArqDeliverInOrder;
	B_UINT32 wmanIfCmnCpsArqRxPurgeTimeout;
	B_UINT32 wmanIfCmnCpsArqBlockSize; 
	B_UINT32 wmanIfCmnCpsMinRsvdTolerableRate ;
	B_UINT32 wmanIfCmnCpsReqTxPolicy;

	B_UINT32 wmanIfCmnSfCsSpecification;
	B_UINT32 wmanIfCmnCpsTargetSaid ;

	B_UINT8 wmanIfCmnCpsSfMacAddress[8];
}wmanIfCmnCpsServiceFlowEntry;

typedef  struct wmanIfSsChannelMeasurementEntry
{
	B_UINT32 wmanIfSsHistogramIndex;
	B_UINT32 wmanIfSsChannelNumber;
	B_UINT32 wmanIfSsStartFrame;
	B_UINT32 wmanIfSsDuration;
	B_UINT32 wmanIfSsBasicReport ;
	B_UINT32 wmanIfSsMeanCinrReport;
	B_UINT32 wmanIfSsStdDeviationCinrReport;
	B_UINT32 wmanIfSsMeanRssiReport;
	B_UINT32 wmanIfSsStdDeviationRssiReport;	
	

}wmanIfSsChannelMeasurementEntry;

/*    wmanIfSsPkmTekEntry wmanIfSsPkmTekTable*/
typedef struct wmanIfSsPkmTekEntry
{
 	B_UINT32 wmanIfSsPkmTekSAId;
	B_UINT32 wmanIfSsPkmTekSAType;
	B_UINT32 WmanIfDataEncryptAlgId;
	B_UINT32 WmanIfDataAuthAlgId;
	B_UINT32 WmanIfTekEncryptAlgId;
	B_UINT32 wmanIfSsPkmTekState ;
	B_UINT32 wmanIfSsPkmTekKeySequenceNumber;
	DateAndTime wmanIfSsPkmTekExpiresOld;
	DateAndTime wmanIfSsPkmTekExpiresNew;
	Counter32 wmanIfSsPkmTekKeyRequests;
	Counter32 wmanIfSsPkmTekKeyReplies ;
	Counter32 wmanIfSsPkmTekKeyRejects ;
	Counter32 wmanIfSsPkmTekInvalids; 
	Counter32 wmanIfSsPkmTekAuthPends ;
	B_UINT32 wmanIfSsPkmTekKeyRejectErrorCode;
	SnmpAdminString wmanIfSsPkmTekKeyRejectErrorString ;
	B_UINT32 wmanIfSsPkmTekInvalidErrorCode; 
	SnmpAdminString wmanIfSsPkmTekInvalidErrorString ;
	
 }wmanIfSsPkmTekEntry;
typedef struct WmanIfSsOfdmaDcdBurstProfileEntry
{
	B_UINT32 wmanIfSsOfdmaDiucIndex;
	B_UINT32 wmanIfSsOfdmaDownlinkFrequency;
	B_UINT32 wmanIfSsOfdmaDcdFecCodeType;
	B_UINT32 wmanIfSsOfdmaDiucMandatoryExitThresh;
	B_UINT32 wmanIfSsOfdmaDiucMinEntryThresh;
	
}wmanIfSsOfdmaDcdBurstProfileEntry;

typedef struct WmanIfSsOfdmDcdBurstProfileEntry
{
	B_UINT32 wmanIfSsOfdmDiucIndex;
	B_UINT32 wmanIfSsOfdmDownlinkFrequency;
	B_UINT32 wmanIfSsOfdmDcdFecCodeType;
	B_UINT32 wmanIfSsOfdmDiucMandatoryExitThresh;
	B_UINT32 wmanIfSsOfdmDiucMinEntryThresh;
	B_UINT32 wmanIfSsOfdmTcsEnable;

}wmanIfSsOfdmDcdBurstProfileEntry;

typedef struct WmanIfSsOfdmaUcdBurstProfileEntry
{
	B_UINT32 wmanIfSsOfdmaUiucIndex;
	B_UINT32 wmanIfSsOfdmaUcdFecCodeType;
	B_UINT32 wmanIfSsOfdmaRangingDataRatio;
	B_UINT8 wmanIfSsOfdmaNorCOverNOverride[5+3];
}wmanIfSsOfdmaUcdBurstProfileEntry;


typedef struct WmanIfSsOfdmUcdBurstProfileEntry
{
	B_UINT32 wmanIfSsOfdmUiucIndex;

	B_UINT32 wmanIfSsOfdmUcdFecCodeType;
	B_UINT32 wmanIfSsOfdmFocusCtPowerBoost;
	B_UINT32 wmanIfSsOfdmUcdTcsEnable;
}wmanIfSsOfdmUcdBurstProfileEntry;

/* statistics structure */
typedef struct stMIBStatistics
{
	B_UINT32  wmanIfSsPkmAuthState;
	B_UINT32  wmanIfSsPkmAuthKeySequenceNumber;
	DateAndTime  wmanIfSsPkmAuthExpiresOld; 	
	DateAndTime  wmanIfSsPkmAuthExpiresNew;
	TruthValue wmanIfSsPkmAuthReset;

	B_UINT32  wmanIfSsPkmAuthentInfos;
	B_UINT32  wmanIfSsPkmAuthRequests;
	B_UINT32  wmanIfSsPkmAuthReplies;
	B_UINT32  wmanIfSsPkmAuthRejects;
	B_UINT32  wmanIfSsPkmAuthInvalids;
	B_UINT32	wmanIfSsPkmAuthRejectErrorCode;
	B_UINT8		wmanIfSsPkmAuthRejectErrorString[128];

	B_UINT32	wmanIfSsPkmAuthInvalidErrorCode;
	B_UINT8 	  wmanIfSsPkmAuthInvalidErrorString[128];

	DateAndTime  wmanIfSsPkmAuthGraceTime;
	DateAndTime  wmanIfSsPkmTekGraceTime;
	B_UINT32  wmanIfSsPkmAuthWaitTimeout;
	B_UINT32  wmanIfSsPkmReauthWaitTimeout;
	B_UINT32  wmanIfSsPkmOpWaitTimeout;
	B_UINT32  wmanIfSsPkmRekeyWaitTimeout;
	B_UINT32  wmanIfSsPkmAuthRejectWaitTimeout;

	B_UINT32  wmanIfSsOfdmCtBasedResvTimeout;
	B_UINT32  wmanIfSsOfdmBwReqOppSize;
	B_UINT32  wmanIfSsOfdmRangReqOppSize;
	B_UINT32  wmanIfSsOfdmUplinkCenterFreq;
	B_UINT32  wmanIfSsOfdmNumSubChReqRegionFull;
	B_UINT32  wmanIfSsOfdmNumSymbolsReqRegionFull;
	B_UINT32  wmanIfSsOfdmSubChFocusCtCode;
	B_UINT32  wmanIfSsOfdmUpLinkChannelId;
	B_UINT32  wmanIfSsOfdmBsEIRP;
	B_UINT32  wmanIfSsOfdmChannelNumber;
	B_UINT32  wmanIfSsOfdmTTG;
	B_UINT32  wmanIfSsOfdmRTG;
	B_UINT32  wmanIfSsOfdmInitRngMaxRSS;
	B_UINT32  wmanIfSsOfdmDownlinkCenterFreq;
	B_UINT8 wmanIfSsOfdmBsId[6+2];

	B_UINT32  wmanIfSsOfdmMacVersion;
	B_UINT32  wmanIfSsOfdmFrameDurationCode;
	B_UINT32  wmanIfSsOfdmDownLinkChannelId;
	B_UINT32  wmanIfSsOfdmaCtBasedResvTimeout;
	B_UINT32  wmanIfSsOfdmaBwReqOppSize;
	B_UINT32  wmanIfSsOfdmaRangReqOppSize;
	B_UINT32  wmanIfSsOfdmaUplinkCenterFreq;
	B_UINT32  wmanIfSsOfdmaInitRngCodes;
	B_UINT32  wmanIfSsOfdmaPeriodicRngCodes;
	B_UINT32  wmanIfSsOfdmaBWReqCodes;
	B_UINT32  wmanIfSsOfdmaPerRngBackoffStart;
	B_UINT32  wmanIfSsOfdmaPerRngBackoffEnd;
	B_UINT32  wmanIfSsOfdmaStartOfRngCodes;
	B_UINT32  wmanIfSsOfdmaPermutationBase;
	B_UINT8 wmanIfSsOfdmaULAllocSubchBitmap[9+3];
	B_UINT8 wmanIfSsOfdmaOptPermULAllocSubchBitmap[13+3];

	B_UINT32  wmanIfSsOfdmaBandAMCAllocThreshold;
	B_UINT32  wmanIfSsOfdmaBandAMCReleaseThreshold;
	B_UINT32  wmanIfSsOfdmaBandAMCAllocTimer;
	B_UINT32  wmanIfSsOfdmaBandAMCReleaseTimer;
	B_UINT32  wmanIfSsOfdmaBandStatRepMAXPeriod;
	B_UINT32  wmanIfSsOfdmaBandAMCRetryTimer;
	B_UINT32  wmanIfSsOfdmaSafetyChAllocThreshold;
	B_UINT32  wmanIfSsOfdmaSafetyChReleaseThreshold;
	B_UINT32  wmanIfSsOfdmaSafetyChAllocTimer;
	B_UINT32  wmanIfSsOfdmaSafetyChReleaseTimer;
	B_UINT32  wmanIfSsOfdmaBinStatRepMAXPeriod;
	B_UINT32  wmanIfSsOfdmaSafetyChaRetryTimer;
	B_UINT32  wmanIfSsOfdmaHARQAackDelayULBurst;
	B_UINT32  wmanIfSsOfdmaCQICHBandAMCTranaDelay;
	B_UINT32  wmanIfSsOfdmaBsEIRP;
	B_UINT32  wmanIfSsOfdmaChannelNumber;
	B_UINT32  wmanIfSsOfdmaTTG;
	B_UINT32  wmanIfSsOfdmaRTG;
	B_UINT32  wmanIfSsOfdmaInitRngMaxRSS;
	B_UINT32  wmanIfSsOfdmaDownlinkCenterFreq;
	B_UINT8 wmanIfSsOfdmaBsId[6+2];

	B_UINT32  wmanIfSsOfdmaMacVersion;
	B_UINT32  wmanIfSsOfdmaFrameDurationCode;
	B_UINT32  wmanIfSsOfdmaSizeCqichIdField;
	B_UINT32  wmanIfSsOfdmaHARQAackDelayBurst;
	B_UINT32  wmanIfBsSsOfdmReqCapFftSizes;
	B_UINT32  wmanIfBsSsOfdmReqCapSsDemodulator;
	B_UINT32  wmanIfBsSsOfdmReqCapSsModulator;
	B_UINT32  wmanIfBsSsOfdmReqCapFocusedCtSupport;
	B_UINT32  wmanIfBsSsOfdmReqCapTcSublayerSupport;
	B_UINT32  wmanIfBsSsOfdmRspCapFftSizes;
	B_UINT32  wmanIfBsSsOfdmRspCapSsDemodulator;
	B_UINT32  wmanIfBsSsOfdmRspCapSsModulator;
	B_UINT32  wmanIfBsSsOfdmRspCapFocusedCtSupport;
	B_UINT32  wmanIfBsSsOfdmRspCapTcSublayerSupport;
	B_UINT8 reserverdField[6+2];
	B_UINT8 wmanIfSsUnknownTlv[MAX_STRING_LEN];

	B_UINT32  wmanIfSsDynamicServiceType;
	B_UINT8 wmanIfSsDynamicServiceFailReason[256];

	B_UINT32  wmanIfSsRssiStatus;
	B_UINT8 wmanIfSsRssiStatusInfo[256];

	wmanIfSsChannelMeasurementEntry wmanIfSsChannelMeasurementTable [MAX_HISTOGRAM_INDEX];	

	wmanIfSsPkmTekEntry wmanIfSsPkmTekTable[MAX_SAID_ENTRIES];

	wmanIfSsOfdmUcdBurstProfileEntry wmanIfSsOfdmUcdBurstProfileTable[MAX_OFDMUIUCINDEX];

	wmanIfSsOfdmDcdBurstProfileEntry wmanIfSsOfdmDcdBurstProfileTable[MAX_OFDMDIUCINDEX];

	wmanIfSsOfdmaUcdBurstProfileEntry wmanIfSsOfdmaUcdBurstProfileTable[MAX_OFDMAUIUCINDEX];

	wmanIfSsOfdmaDcdBurstProfileEntry wmanIfSsOfdmaDcdBurstProfileTable[MAX_OFDMADIUCINDEX];

	wmanIfCmnCpsServiceFlowEntry 	wmanIfCmnCpsServiceFlowTable[MAX_CPS_SERVICE_FLOWENTRIES];
    UINT32 BcmBasebandAMuxnVAIN1Q7;

    SINT32   s32wmanIfTxPowerInPt5dB;	
	UINT32   u32wmanIfTxPowHeadroomInPt5dB;
	UINT32   u32wmanIfRngRspForIrCode;		
	UINT32   u32wmanIfNoRngRspForIrCode;
	UINT32   u32wmanIfRngRspForPrCode;	
	UINT32   u32wmanIfNoRngRspForPrCode;
	UINT32   u32wmanIfHoIndSuccess;		
	UINT32   u32wmanIfIRReqtoRegRspTime;	
	UINT32   u32wmanIfNumOfRegRspRcvd;		
	UINT32   u32wmanIfNumIRReqWithNoRegRsp;
	UINT32   u32wmanIfTotNumIRReqSinceReset;
	UINT32   u32wmanIfTemperature;			
	UINT32   u32wmanIfSecsSincePowerCycle;	
	UINT32   u32wmanIfArqNumReTxBlocks;		
	ST_PER_SF_INFO		stwmanIfDLSFInfo[NUMBER_OF_DL_UL_SF_INFO];
	ST_PER_SF_INFO		stwmanIfULSFInfo[NUMBER_OF_DL_UL_SF_INFO];
    
}stMIBStatistics;

typedef struct stBcmProprietaryStatsMIBs
{
	B_UINT32  BcmBasebandHarqRetrySnd[4];/* changed on 12-12-06 */
	B_UINT32  BcmBasebandHarqRetryRcv[4]; /* changed on 12-12-06 */
	B_UINT32  BcmBasebandHarqPerAttemptRcvSuccess[4];
	B_UINT32  BcmBasebandHarqFinalRcvSuccess;
	B_UINT32  BcmBasebandHarqPerAttemptRcvTransCompleted[4];
	B_UINT32  BcmBasebandHarqFinalRcvTransCompleted;
	
	B_UINT32  BcmBasebandDlPreHarqBurstErrorRateLower;
	B_UINT32  BcmBasebandDlPreHarqBurstErrorRateUpper;
	B_UINT32  BcmBasebandDlPostHarqBurstErrorRateLower;
	B_UINT32  BcmBasebandDlPostHarqBurstErrorRateUpper;
	B_UINT32  BcmBasebandDlPerRxBurstErrorRateUpper[4];
	B_UINT32  BcmBasebandDlPostHarqConfirmedFailures;
    B_UINT32  BcmBasebandDlHarqTransactionsInProgress;

	B_UINT32  BcmBasebandInitRangingFail;
	B_UINT32  BcmBasebandPeriodicRangingFail;
	B_UINT32  BcmBasebandBandwidthRangingFail;
	B_UINT32  BcmBasebandRssiRxPath1;
	B_UINT32  BcmBasebandRssiRxPath2;
	B_UINT32  BcmBasebandHandoverRangingFail;
	
	/* added on 12-12-06 start */
	B_UINT32  BcmBasebandBadLengthInCompMap; 
	B_UINT32  BcmBasebandNumOfAckDisabledDlHarqBursts;  	
	B_UINT32  BcmBasebandNumOfDlHarqAckEnabledBursts; 
	B_UINT32  BcmBasebandNumOfAckDisabledCrc16FailedBursts; 
	B_UINT32  BcmBasebandHBurstNotRcvdB4AckSlot; 
	B_UINT32  BcmBasebandNumHOCancel; 
	B_UINT32  BcmBasebandNumHOReject;
	B_SINT32  BcmBasebandMeanTxPower;
	B_UINT32  BcmBasebandCumulativeDLMapLength; 
	B_UINT32  BcmBasebandCumulativeULMapLength;
	B_UINT32  BcmBasebandNetworkEntryDuration;
	B_UINT32  BcmBasebandNetworkReEntryDuration;
	B_UINT32  BcmBasebandNumOfNacksForEachAttempts[4];
	B_UINT32  BcmBasebandNumOfAcksForEachAttempts[4];
	/* added on 12-12-06 End */

	B_UINT32  BcmBasebandFrameRatio;
	B_UINT32 BcmBasebandPacketErrorRate;
	B_UINT32 BcmBasebandFrameErrorRate;
	
	B_UINT32 BcmBasebandCompressedMapCount;
	B_UINT32 BcmBasebandNumofResync;

	ST_NEIGHBOUR_BWINFO BcmBasebandNeighbourBandwidthInfo;
	ST_BCMBASEBAND_DL_BURSTPROFILE BcmBasebandDLBurstProfile;
	ST_BCMBASEBAND_UL_BURSTPROFILE BcmBasebandULBurstProfile;
	ST_BCM_SBC_CONNECTIONPARAMS BcmBasebandSbcConnectionParams;
	ST_UL_BW_ALLOC_INFO BcmBasebandBwAllocationInfo;
	ST_UL_BW_REQ_INFO BcmBasebandUlBwReqInfo;
	
	B_UINT32 BcmBasebandSAIdTable[MAX_SAID_ENTRIES];
	B_SINT32  BcmBasebandPrimaryCinr;
	B_SINT32  BcmBasebandDiversityCinr;

	/*
	 * TJR 10Sept'07: New Statistics Added for FoxConn
	 */

	B_UINT32	BcmBasebandDLFrameNum;
	B_UINT32	BcmBasebandNumOfInitRangeSuccess;
	B_UINT32	BcmBasebandNumOfPeriodicRangingSuccess;
	B_UINT32	BcmBasebandHOAttempts;
	B_UINT32	BcmBasebandNumOfHOSuccess;
	DateAndTime BcmBasebandTimeInSleepMode;
	DateAndTime BcmBasebandTimeInIdleMode;
	DateAndTime BcmBasebandTimeInActiveMode;
	B_UINT32	BcmBasebandNumOfTxBEBytes;
	B_UINT32	BcmBasebandNumOfTxUGSBytes;
	B_UINT32	BcmBasebandNumOfTxrTPSBytes;
	B_UINT32	BcmBasebandNumOfTxerTPSBytes;
	B_UINT32	BcmBasebandNumOfTxnrTPSBytes;
	B_UINT32	BcmBasebandDLTrafficRate;
	B_UINT32	BcmBasebandULTrafficRate;
	B_UINT32	BcmBasebandNumOfFailedSFCreation;
	B_UINT32	BcmBasebandCurrIdCellSeg;
	B_UINT32	BcmBasebandNAILocation;
	B_UINT32    BcmBasebandDeviceStatus;
    B_UINT32    BcmBasebandConnectionStatus;
    B_UINT32    BcmBasebandStatusReason;
	ST_BCMBASEBAND_NwENTRY_TIMING_STATS BcmBasebandNwEntryBrkUp;
	B_UINT32	BcmBasebandNAIValid;
	B_UINT32	BcmBasebandNumOfULHARQAckEnabledBurstAllocs;
	B_UINT32	BcmBasebandNumOfULHARQAckEnabledReTx;
	ST_PER_ZONE_MCS_STATS	BcmBasebandPerZoneMCS;
	
	ST_BCMBASEBAND_CbyNTable	BcmBasebandCbyNTable;
	ST_BCMBASEBAND_NI			BcmBasebandNI;
	ST_BCMBASEBAND_ScanParams	BcmBasebandScanParams;
	
	/* The below two MIBs are not yet implemented by MAC */
	ST_HANDOVER_INFORMATION BcmBasebandHOInformation;
	ST_IDLEMODE_INFORMATION BcmBasebandIdleModeInformation;
	
	ST_TXANTSWITCH_INFORMATION BcmBasebandTxAntSwitchInformation;
	
	/* Common BSLogger */
	ST_BSTRACKER_INFO BcmBaseBandBSTrackingInfo;

	B_UINT32 BcmBasebandSubMapErrors[NUM_OF_SUBMAPS];
    B_UINT32 BcmBasebandTxAAsoundingType[4];
    B_UINT32 BcmBasebandNumULSoundingZone;
	B_UINT32 BcmBasebandIsCompressedMap;
    /* Non-HARQ stats */
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsUnicastAttempts;
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsUnicastSuccess;
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsUnicastBurstErrorRate;
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsBcstAttempts;
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsBcstSuccess;
    B_UINT32   BcmBasebandNonHarqNumOfDlBurstsBcstBurstErrorRate;  
    /* Non ARQ Stats */
    B_UINT32   BcmBasebandNonArqReassemblyFailure;
    B_UINT32   BcmBasebandNonArqTxAgedPdus;
    /* Arq Rx Stats */
    B_UINT32   BcmBasebandArqRxPurgeTimeout;
    B_UINT32   BcmBasebandArqRxSyncLoss;
    /* Arq Tx Stats */
    B_UINT32   BcmBasebandArqTxBlockLifeTimeout;
    B_UINT32   BcmBasebandArqTxRetryTimeout;
    B_UINT32   BcmBasebandArqTxSyncLoss;
    B_UINT32   BcmBasebandArqNumReTxPdus;
    
    B_UINT32   BcmBasebandHARQRecoveryTimeout;
    B_UINT32   BcmBasebandReorderInvalidSN;

    B_UINT32   BcmBasebandFlashMapInfo[2];    
    B_UINT32   BcmBasebandSCSIFwVersion; 
    B_UINT32   BcmBasebandEthCSSupport;
 
	
}stBcmProprietaryStatsMIBs;

typedef struct _stInternalStatsStruct
{
	stStatistics_SS Statistics;

	/* followig are not exposed to the user */
	stBcmProprietaryStatsMIBs tBcmPropStats;
    stMIBStatistics  tMibstats;
}stInternalStatsStruct;

typedef struct _FLASH_2X_CS_INFO_PVT_

{

	

	// magic number as 0xBECE-F1A5 - F1A5 for "flas-h"

	B_UINT32 MagicNumber;					



	B_UINT32 FlashLayoutVersion ; 	



    // ISO Image/Format/BuildTool versioning

    B_UINT32 ISOImageVersion; 



    // SCSI/Flash BootLoader versioning

    B_UINT32 SCSIFirmwareVersion; 



	// ISO Image1 Part1/SCSI Firmware/Flash Bootloader Start offset, size

	B_UINT32 OffsetFromZeroForPart1ISOImage; 

	B_UINT32 OffsetFromZeroForScsiFirmware; 

	B_UINT32 SizeOfScsiFirmware ; 



	// ISO Image1 Part2 start offset

	B_UINT32 OffsetFromZeroForPart2ISOImage; 





	// DSD0 offset 

	B_UINT32 OffsetFromZeroForDSDStart; 	

	B_UINT32 OffsetFromZeroForDSDEnd;		



	// VSA0 offset

	B_UINT32 OffsetFromZeroForVSAStart; 	 

	B_UINT32 OffsetFromZeroForVSAEnd; 



	// Control Section offset

	B_UINT32 OffsetFromZeroForControlSectionStart;

	B_UINT32 OffsetFromZeroForControlSectionData;



	// NO Data Activity timeout to switch from MSC to NW Mode

	B_UINT32 CDLessInactivityTimeout;		



	// New ISO Image Signature

	B_UINT32 NewImageSignature; 			 

	

	B_UINT32 FlashSectorSizeSig;			// Sector Size Signature

	B_UINT32 FlashSectorSize;			// Sector Size 

	B_UINT32 FlashWriteSupportSize; 	// Write Size Support

	

	B_UINT32 TotalFlashSize;			// Total Flash Size



	// Flash Base Address for offset specified

	B_UINT32 FlashBaseAddr; 			

	B_UINT32 FlashPartMaxSize;			// Flash Part Max Size



	// Is CDLess or Flash Bootloader 

	B_UINT32 IsCDLessDeviceBootSig;



	// MSC Timeout after reset to switch from MSC to NW Mode

	B_UINT32 MassStorageTimeout;			



	/* Flash Map 2.0 Field */

	B_UINT32 OffsetISOImage1Part1Start; 	// ISO Image1 Part1 offset 

	B_UINT32 OffsetISOImage1Part1End; 

	B_UINT32 OffsetISOImage1Part2Start; 	// ISO Image1 Part2 offset

	B_UINT32 OffsetISOImage1Part2End; 

	B_UINT32 OffsetISOImage1Part3Start; 	// ISO Image1 Part3 offset

	B_UINT32 OffsetISOImage1Part3End; 



	B_UINT32 OffsetISOImage2Part1Start; 	// ISO Image2 Part1 offset

	B_UINT32 OffsetISOImage2Part1End; 

	B_UINT32 OffsetISOImage2Part2Start; 	// ISO Image2 Part2 offset

	B_UINT32 OffsetISOImage2Part2End; 

	B_UINT32 OffsetISOImage2Part3Start; 	// ISO Image2 Part3 offset

	B_UINT32 OffsetISOImage2Part3End; 





	// DSD Header offset from start of DSD

	B_UINT32 OffsetFromDSDStartForDSDHeader;

	B_UINT32 OffsetFromZeroForDSD1Start;	// DSD 1 offset

	B_UINT32 OffsetFromZeroForDSD1End;		

	B_UINT32 OffsetFromZeroForDSD2Start;	// DSD 2 offset

	B_UINT32 OffsetFromZeroForDSD2End;		



	B_UINT32 OffsetFromZeroForVSA1Start;	// VSA 1 offset

	B_UINT32 OffsetFromZeroForVSA1End; 

	B_UINT32 OffsetFromZeroForVSA2Start;	// VSA 2 offset

	B_UINT32 OffsetFromZeroForVSA2End; 



	/*

*	 ACCESS_BITS_PER_SECTOR	2

*	ACCESS_RW			0

*	ACCESS_RO				1

*	ACCESS_RESVD			2

*	ACCESS_RESVD			3

*	*/

	B_UINT32 SectorAccessBitMap[MAX_DFLASH_SIZE/(DEFAULT_SECTOR_SIZE *16)];



// All expansions to the control data structure should add here



}FLASH_2X_CS_INFO_PVT,*PFLASH_2X_CS_INFO_PVT;


#endif /* PRIVATE_STATS_STRUCT_H */
