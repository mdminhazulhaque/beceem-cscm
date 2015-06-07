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
 * Description	:	MIB initializations.
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h"
#include "HostMIBSInterface.h"
#include "config_paramsSS.h"


void AddMIBEntry(
				PBECEEMWIMAX pWiMAX,
				UINT32 uiIndex,
				UINT16 u16Attribute,
				UINT32 u32Offset,			/* offset of table or the entry */
				UINT32 u32FieldSize,			/* size of field */
				UINT8 u8FieldType
				)
{
	pWiMAX->m_sMIBTable[uiIndex].u16MIBAttribute = u16Attribute;
	pWiMAX->m_sMIBTable[uiIndex].u32Offset = u32Offset;
	pWiMAX->m_sMIBTable[uiIndex].u32FieldSize = u32FieldSize;
	pWiMAX->m_sMIBTable[uiIndex].u8FieldType = u8FieldType;
}

void AddMIBTableEntry(
				PBECEEMWIMAX pWiMAX,
				UINT32 uiIndex,
				UINT32 u16Attribute,
				UINT32 u32TableOffset,		/* offset of table */
				UINT16 u16TableRowSize,		/* size of single row */
				UINT8  u8NumRows,			/* num of rows */
				UINT32 u32TableEntryOffset,	/* offset of table entry */
				UINT32 u32TableEntrySize,		/* size of this field */
				UINT8 u8FieldType
				)
{
	pWiMAX->m_sMIBTable[uiIndex].u16MIBAttribute =(UINT16) u16Attribute;

	pWiMAX->m_sMIBTable[uiIndex].u32Offset = u32TableEntryOffset-u32TableOffset;
	pWiMAX->m_sMIBTable[uiIndex].u32FieldSize = u32TableEntrySize;
	pWiMAX->m_sMIBTable[uiIndex].u8FieldType = u8FieldType;

	pWiMAX->m_sMIBTable[uiIndex].u32TableOffset = u32TableOffset;
	pWiMAX->m_sMIBTable[uiIndex].u16TableRowSize = u16TableRowSize;
	pWiMAX->m_sMIBTable[uiIndex].u8NumRows = u8NumRows;
}

void AddPropreiteryStatsMIB(PBECEEMWIMAX pWiMAX, UINT32 uiMember, UINT uiSize)
{
	if(!pWiMAX)
		return;

	AddMIBEntry(
			pWiMAX,
			pWiMAX->m_iPropStatsIndex++,
			BIT_TGT_PROP_STATS|BIT_NON_INDEX,
			uiMember,
			uiSize,
			(UINT8)-1
			);
}

#define ADD_PROP_CFG_MIB_MEMBER(MEMBER) AddMIBEntry(												\
												pWiMAX,												\
												pWiMAX->m_iPropCfgIndex++,							\
												BIT_TGT_PROP_CONFIG|BIT_NON_INDEX,					\
												(UINT32)(UADDTYPE)&((SYSTEM_CONFIGURATION *)NULL)->MEMBER,	\
												sizeof(((SYSTEM_CONFIGURATION *)NULL)->MEMBER),		\
												(UINT8)-1													\
												)
#define ADD_STD_CFG_MIB_MEMBER(MEMBER, TYPE) AddMIBEntry(											\
												pWiMAX,												\
												pWiMAX->m_iStdCfgIndex++,							\
												BIT_TGT_STD_CONFIG|BIT_NON_INDEX,					\
												(UADDTYPE)(&((stConfigParams *)NULL)->MEMBER),			\
												sizeof(((stConfigParams *)NULL)->MEMBER),			\
												TYPE												\
												)

#define ADD_STD_CONFIG_MIB_TABLE_1IND_MEMBER(MEMBER, TYPE) AddMIBTableEntry(						\
												pWiMAX,												\
											pWiMAX->m_iStdCfgIndex++,								\
											BIT_TGT_STD_CONFIG|BIT_ONE_INDEX,						\
											(UINT32)(UADDTYPE)&MIB_STD_CONFIG_TABLE_BASE,			\
											sizeof(MIB_STD_CONFIG_TABLE_BASE),						\
											CONFIG_MIB_TABLE_NUMROWS,								\
											(UINT32)(UADDTYPE)&MIB_STD_CONFIG_TABLE_BASE.MEMBER,		\
											sizeof(MIB_STD_CONFIG_TABLE_BASE.MEMBER),				\
											TYPE													\
											)
#define ADD_STD_STATS_MIB_MEMBER(MEMBER) AddMIBEntry(												\
												pWiMAX,												\
												pWiMAX->m_iStdStatsIndex++,							\
												MIB_ATTRIBUTE,					\
												(UINT32)(UADDTYPE)&MIB_STD_STATS_BASE.MEMBER,				\
												sizeof(MIB_STD_STATS_BASE.MEMBER),					\
												(UINT8)(-1)													\
												)

#define ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(MEMBER,TYPE) AddMIBTableEntry(								\
												pWiMAX,												\
											pWiMAX->m_iStdStatsIndex++,								\
											MIB_ATTRIBUTE,						\
											(UINT32)(UADDTYPE)&MIB_STD_STATS_TABLE_BASE,				\
											sizeof(MIB_STD_STATS_TABLE_BASE),						\
											MIB_TABLE_NUMROWS,										\
											(UINT32)(UADDTYPE)&MIB_STD_STATS_TABLE_BASE.MEMBER,		\
											sizeof(MIB_STD_STATS_TABLE_BASE.MEMBER),				\
											(UINT8)(TYPE)														\
											)

#define ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(MEMBER, TYPE) AddMIBTableEntry(								\
												pWiMAX,												\
											pWiMAX->m_iStdStatsIndex++,								\
											MIB_ATTRIBUTE,						\
											(UINT32)(UADDTYPE)&MIB_STD_STATS_TABLE_BASE,				\
											sizeof(MIB_STD_STATS_TABLE_BASE),						\
											MIB_TABLE_NUMROWS,										\
											(UINT32)(UADDTYPE)&MIB_STD_STATS_TABLE_BASE.MEMBER,		\
											sizeof(MIB_STD_STATS_TABLE_BASE.MEMBER),				\
											(UINT8)(TYPE)														\
											)

VOID BeceemWiMAX_InitializeStdConfigMIBs(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

	/********************************************************************************/
	/*							STD CONFIG MIBS										*/
	/********************************************************************************/

	pWiMAX->m_iStdCfgIndex = BCM_STANDARD_CFG_MIB_BASE_START;

	/* 0 */
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsLostDLMapInterval, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsLostULMapInterval, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsContentionRangRetries, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsRequestRetries, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsRegRequestRetries, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT1Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT2Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT3Timeout, TYPE_UINT8);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT4Timeout, TYPE_UINT8);
    ADD_STD_CFG_MIB_MEMBER(wmanIfSsT6Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT12Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT14Timeout, TYPE_UINT8);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT18Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT20Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsT21Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfSsSBCRequestRetries, TYPE_UINT8);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnInvitedRangRetries, TYPE_UINT16);

	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnDSxReqRetries, TYPE_UINT32);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnDSxRespRetries, TYPE_UINT32);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnT7Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnT8Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnT10Timeout, TYPE_UINT16);
	ADD_STD_CFG_MIB_MEMBER(wmanIfCmnT22Timeout, TYPE_UINT16);

#define MIB_STD_CONFIG_TABLE_BASE ((stConfigParams * )NULL)->wmanIfCmnCryptoSuiteTable[0]
#define CONFIG_MIB_TABLE_NUMROWS MAX_CRYPTOSUITE_INDEX

	ADD_STD_CONFIG_MIB_TABLE_1IND_MEMBER(wmanIfCmnCryptoSuiteDataEncryptAlg, TYPE_UINT8);
	ADD_STD_CONFIG_MIB_TABLE_1IND_MEMBER(wmanIfCmnCryptoSuiteDataAuthentAlg, TYPE_UINT8);
	ADD_STD_CONFIG_MIB_TABLE_1IND_MEMBER(wmanIfCmnCryptoSuiteTekEncryptAlg, TYPE_UINT8);
    ADD_STD_CFG_MIB_MEMBER(BcmConfigNetSearchPeriodicWakeup, TYPE_UINT32);
	ADD_STD_CFG_MIB_MEMBER(wmanIfM2SsT44Timer, TYPE_UINT32);

}

VOID BeceemWiMAX_InitializePropCFGMIBs(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

	/********************************************************************************/
	/*							PROPRIETARY CONFIG MIBS								*/
	/********************************************************************************/
	pWiMAX->m_iPropCfgIndex = BCM_PROPREITARY_CFG_MIB_BASE_START;

	/*500*/
	ADD_PROP_CFG_MIB_MEMBER(u32CenterFrequency);
	/*501*/
	ADD_PROP_CFG_MIB_MEMBER(u32ArqEnable);
	/*502*/
	ADD_PROP_CFG_MIB_MEMBER(u32HoEnable);
	/*504 skipping 503*/
	pWiMAX->m_iPropCfgIndex = 504;
	ADD_PROP_CFG_MIB_MEMBER(u32PhyParameter2);
	/*PKMv2 505*/
	ADD_PROP_CFG_MIB_MEMBER(u32SecurityEnable);
	pWiMAX->m_iPropCfgIndex = 508;
	ADD_PROP_CFG_MIB_MEMBER(u32ConfigBW);
	pWiMAX->m_iPropCfgIndex = 510;
	ADD_PROP_CFG_MIB_MEMBER(HostDrvrConfig4);
	pWiMAX->m_iPropCfgIndex = 511;
	ADD_PROP_CFG_MIB_MEMBER(HostDrvrConfig5);
	
}

VOID BeceemWiMAX_InitializeStdStatsMIBs(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

	/********************************************************************************/
	/*							STANDARD STATS MIBS									*/
	/********************************************************************************/
#undef MIB_ATTRIBUTE
#define MIB_ATTRIBUTE BIT_TGT_STD_STATS|BIT_NON_INDEX
#define MIB_STD_STATS_BASE ((stInternalStatsStruct * )NULL)->tMibstats

	pWiMAX->m_iStdStatsIndex = BCM_STANDARD_STATS_MIB_BASE_START;

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthState);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthKeySequenceNumber);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthExpiresOld); 	
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthExpiresNew);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthReset);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthentInfos);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthRequests);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthReplies);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthRejects);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthInvalids);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthRejectErrorCode);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthRejectErrorString);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthInvalidErrorCode);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthInvalidErrorString);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthGraceTime);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmTekGraceTime);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthWaitTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmReauthWaitTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmOpWaitTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmRekeyWaitTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsPkmAuthRejectWaitTimeout);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmCtBasedResvTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmBwReqOppSize);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmRangReqOppSize);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmUplinkCenterFreq);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmNumSubChReqRegionFull);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmNumSymbolsReqRegionFull);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmSubChFocusCtCode);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmUpLinkChannelId);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmBsEIRP);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmChannelNumber);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmTTG);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmRTG);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmInitRngMaxRSS);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmDownlinkCenterFreq);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmBsId);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmMacVersion);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmFrameDurationCode);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmDownLinkChannelId);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaCtBasedResvTimeout);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBwReqOppSize);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaRangReqOppSize);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaUplinkCenterFreq);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaInitRngCodes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaPeriodicRngCodes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBWReqCodes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaPerRngBackoffStart);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaPerRngBackoffEnd);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaStartOfRngCodes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaPermutationBase);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaULAllocSubchBitmap);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaOptPermULAllocSubchBitmap);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandAMCAllocThreshold);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandAMCReleaseThreshold);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandAMCAllocTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandAMCReleaseTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandStatRepMAXPeriod);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBandAMCRetryTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSafetyChAllocThreshold);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSafetyChReleaseThreshold);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSafetyChAllocTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSafetyChReleaseTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBinStatRepMAXPeriod);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSafetyChaRetryTimer);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaHARQAackDelayULBurst);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaCQICHBandAMCTranaDelay);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBsEIRP);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaChannelNumber);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaTTG);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaRTG);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaInitRngMaxRSS);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaDownlinkCenterFreq);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaBsId);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaMacVersion);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaFrameDurationCode);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaSizeCqichIdField);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsOfdmaHARQAackDelayBurst);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmReqCapFftSizes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmReqCapSsDemodulator);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmReqCapSsModulator);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmReqCapFocusedCtSupport);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmReqCapTcSublayerSupport);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmRspCapFftSizes);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmRspCapSsDemodulator);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmRspCapSsModulator);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmRspCapFocusedCtSupport);
	ADD_STD_STATS_MIB_MEMBER(wmanIfBsSsOfdmRspCapTcSublayerSupport);
	ADD_STD_STATS_MIB_MEMBER(reserverdField);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsUnknownTlv);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsDynamicServiceType);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsDynamicServiceFailReason);

	ADD_STD_STATS_MIB_MEMBER(wmanIfSsRssiStatus);
	ADD_STD_STATS_MIB_MEMBER(wmanIfSsRssiStatusInfo);


#undef MIB_ATTRIBUTE

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsChannelMeasurementTable[0]
#define MIB_TABLE_NUMROWS MAX_HISTOGRAM_INDEX
#define MIB_ATTRIBUTE BIT_TGT_STD_STATS|BIT_ONE_INDEX

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsChannelNumber,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsStartFrame,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsDuration,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsBasicReport,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsMeanCinrReport,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsStdDeviationCinrReport,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsMeanRssiReport,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsStdDeviationRssiReport,TYPE_UINT32);

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsPkmTekTable[0]
#define MIB_TABLE_NUMROWS MAX_SAID_ENTRIES

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekSAType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(WmanIfDataEncryptAlgId,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(WmanIfDataAuthAlgId,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(WmanIfTekEncryptAlgId,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekState,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeySequenceNumber,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekExpiresOld,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekExpiresNew,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeyRequests,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeyReplies,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeyRejects,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekInvalids,TYPE_UINT32); 
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekAuthPends,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeyRejectErrorCode,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekKeyRejectErrorString,TYPE_UINT8);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekInvalidErrorCode,TYPE_UINT32); 
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsPkmTekInvalidErrorString,TYPE_UINT8);

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsOfdmUcdBurstProfileTable[0]
#define MIB_TABLE_NUMROWS MAX_OFDMUIUCINDEX

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmUcdFecCodeType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmFocusCtPowerBoost,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmUcdTcsEnable,TYPE_UINT32);

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsOfdmDcdBurstProfileTable[0]
#define MIB_TABLE_NUMROWS MAX_OFDMDIUCINDEX

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmDownlinkFrequency,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmDcdFecCodeType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmDiucMandatoryExitThresh,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmDiucMinEntryThresh,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmTcsEnable,TYPE_UINT32);

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsOfdmaUcdBurstProfileTable[0]
#define MIB_TABLE_NUMROWS MAX_OFDMAUIUCINDEX

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaUcdFecCodeType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaRangingDataRatio,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaNorCOverNOverride,TYPE_UINT8);


#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS

#define MIB_STD_STATS_TABLE_BASE ((stInternalStatsStruct * )NULL)->tMibstats.wmanIfSsOfdmaDcdBurstProfileTable[0]
#define MIB_TABLE_NUMROWS MAX_OFDMADIUCINDEX

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaDownlinkFrequency,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaDcdFecCodeType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaDiucMandatoryExitThresh,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(wmanIfSsOfdmaDiucMinEntryThresh,TYPE_UINT32);

	/********************************************************************************/
	/*							STANDARD STATS MIBS	- HOST STATS					*/
	/********************************************************************************/

	pWiMAX->m_iStdStatsIndex = BCM_STANDARD_STATS_MIB_BASE_START + 300;

	/* wmanIfCmnCpsServiceFlowTable  */
#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS
#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_ONE_INDEX
#define MIB_STD_STATS_TABLE_BASE ((S_MIBS_HOST_STATS_MIBS * )NULL)->astSFtable[0]
#define MIB_TABLE_NUMROWS MIBS_MAX_SERVICEFLOWS
	
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(usCID,-1);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(ucDirection,-1);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsSfState,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(u8TrafficPriority,-1);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsMaxSustainedRate,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsMaxTrafficBurst,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsMinReservedRate,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsToleratedJitter,TYPE_UINT32); 
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsMaxLatency,TYPE_UINT32); 
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsFixedVsVariableSduInd,TYPE_UINT32);

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsSduSize,TYPE_UINT32 );
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsSfSchedulingType,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqEnable,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqWindowSize,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqBlockLifetime,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqSyncLossTimeout,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqDeliverInOrder,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqRxPurgeTimeout,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsArqBlockSize,TYPE_UINT32); 
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsMinRsvdTolerableRate,TYPE_UINT32);

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsReqTxPolicy,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnSfCsSpecification,TYPE_UINT32);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(stMibsExtServiceFlowTable.wmanIfCmnCpsTargetSaid,TYPE_UINT32);

	/* wmanIfCmnClassifierRuleTable  */

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS
#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_TWO_INDEX
#define MIB_STD_STATS_TABLE_BASE ((S_MIBS_HOST_STATS_MIBS * )NULL)->astClassifierTable[0]
#define MIB_TABLE_NUMROWS MIBS_MAX_CLASSIFIERS

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(u8ClassifierRulePriority, -1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(ucTosLow, -1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(ucTosHigh, -1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(ucTosMask, -1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(ucProtocol, -1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(stSrcIpAddress.stIPV6AddrIPLength.ucIpv6Address,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(stSrcIpAddress.stIPV6AddrIPLength.ucIpv6Mask,-1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(stDestIpAddress.stIPV6AddrIPLength.ucIpv6Address,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(stDestIpAddress.stIPV6AddrIPLength.ucIpv6Mask,-1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(usSrcPortRangeLo,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(usSrcPortRangeHi,-1);

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(usDestPortRangeLo,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(usDestPortRangeHi,-1);

	/* PHS */
#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS
#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_TWO_INDEX
#define MIB_STD_STATS_TABLE_BASE ((S_MIBS_HOST_STATS_MIBS * )NULL)->astPhsRulesTable[0]
#define MIB_TABLE_NUMROWS MIBS_MAX_PHSRULES

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(u8PHSF,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(u8PHSM,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(u8PHSS,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(u8PHSV,-1);

#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS
#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_ONE_INDEX
#define MIB_STD_STATS_TABLE_BASE ((S_MIBS_HOST_STATS_MIBS * )NULL)->astSFtable[0]
#define MIB_TABLE_NUMROWS MIBS_MAX_SERVICEFLOWS

	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(uiCurrentRxRate,-1);
	ADD_STD_STATS_MIB_TABLE_1IND_MEMBER(uiCurrentDrainRate,-1);
	
	/* PHS ModifiedBytes */
#undef MIB_STD_STATS_TABLE_BASE
#undef MIB_TABLE_NUMROWS
#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_TWO_INDEX
#define MIB_STD_STATS_TABLE_BASE ((S_MIBS_HOST_STATS_MIBS * )NULL)->astPhsRulesTable[0]
#define MIB_TABLE_NUMROWS MIBS_MAX_PHSRULES

	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(PHSModifiedBytes,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(PHSModifiedNumPackets,-1);
	ADD_STD_STATS_MIB_TABLE_2IND_MEMBER(PHSErrorNumPackets,-1);

}

VOID BeceemWiMAX_InitializePropStatsMIBs(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

  	/********************************************************************************/
	/*							PROPRIETARY STATS MIBS								*/
	/********************************************************************************/

	pWiMAX->m_iPropStatsIndex = BCM_PROPREITARY_STATS_MIB_BASE_START;

	/* 1400 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.u32NumOfFramesRecvd,sizeof(UINT32));
	/* 1401 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32NumOfFramesInError,sizeof(UINT32));
	/* 1402 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32NumOfCrcErrors,sizeof(UINT32));
	/* 1403 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32NumOfHcsErrors,sizeof(UINT32));
	/* 1404 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32NumOfCqichTxmtd,sizeof(UINT32));
	/* 1405 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberOfBandwidthRequestsSent,sizeof(UINT8));
	/* 1406 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberOfSyncFailures,sizeof(UINT8));
	/* 1407 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberofDLChannelsMarkedUnusable,sizeof(UINT8));
	/* 1408 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberofULChannelsMarkedUnusable,sizeof(UINT8));
	/* 1409 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberOfDCDsReceived,sizeof(UINT8));
	/* 1410 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberOfUCDsReceived,sizeof(UINT8));
	/* 1411 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8CurrentUCDChangeCount,sizeof(UINT8));
	/* 1412 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8CurrentDCDChangeCount,sizeof(UINT8));
	/* 1413 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberofAuthenticationFailures,sizeof(UINT8));
	/* 1414 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tCPStats.u8NumberofSBCFailures,sizeof(UINT8));
	/* 1415 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfFchReceived ,sizeof(UINT32));
	/* 1416 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfDlMapsProcessed,sizeof(UINT32));
	/* 1417 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfUlMapReceived,sizeof(UINT32));
	/* 1418 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfBadMaps,sizeof(UINT32));
	/* 1419 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfBadFch,sizeof(UINT32));
	/* 1420 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tMPStats.u32NumOfNoFch,sizeof(UINT32));
	/* 1421 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfSdusFromHost,sizeof(UINT32));
	/* 1422 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfSdusToHost,sizeof(UINT32));
	/* 1423 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfControlPktsFromHost,sizeof(UINT32));
	/* 1424 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfControlPktsToHost,sizeof(UINT32));
	/* 1425 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfPdusTransmitted,sizeof(UINT32));
	/* 1426 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfPdusReceived,sizeof(UINT32));
	/* 1427 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfManagementPktsTransmitted,sizeof(UINT32));
	/* 1428 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfManagementPktsRecieved,sizeof(UINT32));
	/* 1429 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfTxPktsRejected,sizeof(UINT32));
	/* 1430 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumOfRxPktsRejected,sizeof(UINT32));
	/* 1431 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8DLIDCell,sizeof(UINT8));
	/* 1432 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.s8TxReferencePower,sizeof(SINT8));
	/* 1433 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.s8TxPowerHeadRoom,sizeof(SINT8));
	/* 1434 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.s8TxPower,sizeof(SINT8));
	/* 1435 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8IRCodeStart,sizeof(UINT8));
	/* 1436 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8IRCodeEnd,sizeof(UINT8));
	/* 1437 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8BRCodeStart,sizeof(UINT8));
	/* 1438 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8BRCodeEnd,sizeof(UINT8));
	/* 1439 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8HRCodeStart,sizeof(UINT8));
	/* 1440 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8HRCodeEnd,sizeof(UINT8));
	/* 1441 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.au8BSID,6);
	/* 1442 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32ModemState,sizeof(UINT32));
	/* 1443 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32CenterFrequency,sizeof(UINT32));
	/* 1444 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32DLRate,sizeof(UINT32));
	/* 1445 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32ULRate,sizeof(UINT32));

	/* 1446 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqRetrySnd,sizeof(UINT32)*4);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqRetryRcv,sizeof(UINT32)*4);

	/* 1448 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqPerAttemptRcvSuccess,sizeof(UINT32)*4);
	
	/* 1449 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandArqNumReTxPdus,sizeof(UINT32));
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,0); //Not supported any more.
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandArqTxBlockLifeTimeout,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandInitRangingFail,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPeriodicRangingFail,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandBandwidthRangingFail,sizeof(UINT32));
	
	/*1467*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandRssiRxPath1,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandRssiRxPath2,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHandoverRangingFail,sizeof(UINT32));
	
	/* added on 12-12-06 START */
	/* 1470 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8ULIDCell,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32MeanCINRIntraFANbrBS, sizeof(UINT32) * DM_MAX_NBRS);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32MeanCINRInterFANbrBS, sizeof(UINT32) * DM_MAX_NBRS);

	/* 1473 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqFinalRcvSuccess,sizeof(UINT32));
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandBadLengthInCompMap,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfAckDisabledDlHarqBursts,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfDlHarqAckEnabledBursts,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqPerAttemptRcvTransCompleted,sizeof(UINT32) * 4);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfAckDisabledCrc16FailedBursts,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqFinalRcvTransCompleted,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandSubMapErrors,sizeof(UINT32) * NUM_OF_SUBMAPS);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPreHarqBurstErrorRateLower,sizeof(UINT32));

	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHBurstNotRcvdB4AckSlot,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumHOCancel,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumHOReject,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandMeanTxPower,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandCumulativeDLMapLength,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandCumulativeULMapLength,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNetworkEntryDuration,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNetworkReEntryDuration,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfNacksForEachAttempts,sizeof(UINT32) * 4);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfAcksForEachAttempts,sizeof(UINT32) * 4);

	/* added on 12-12-06 END */
	/* MIB is handled by wrapping data from ST_BSTRACKER_INFO structure after rdm*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/* 1495 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandFrameRatio,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDLBurstProfile,sizeof(ST_BCMBASEBAND_DL_BURSTPROFILE));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandULBurstProfile,sizeof(ST_BCMBASEBAND_UL_BURSTPROFILE));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandSbcConnectionParams,sizeof(ST_BCM_SBC_CONNECTIONPARAMS));

	/* 1499 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPacketErrorRate,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandFrameErrorRate,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandUlBwReqInfo,sizeof(ST_UL_BW_REQ_INFO));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandBwAllocationInfo,sizeof(ST_UL_BW_ALLOC_INFO));

	/* 1503 */
	/* PS : This MIB has to be handled seperatly. So the uiMember field is set as zero*/
	AddPropreiteryStatsMIB(pWiMAX, 0 ,sizeof(SFID_LIST));

	/*  1504 - 1513 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.s32CurrentRssi,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.s32CurrentCinr,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32MacStateID,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32UplinkRangingCode,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32UplinkRangingseed,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32Powerworkmode,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32RSSIDeviationfield,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32NumBRTH,sizeof(UINT32));				
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32ConnCount,sizeof(UINT32));					
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32TransportCid,sizeof(UINT32)*MAX_NUM_CONNECTIONS);

	/* 1514 - 1517 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumofBytesFromHost,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumofBytesToHost,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumofULConnections,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tDPStats.u32NumofDLConnections,sizeof(UINT32));	

	/* 1518 - 1522 */	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.s32MeanCinr,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.s32DeviationCinr,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8PreambleIndex,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8PRCodeEnd,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tLinkStatsEx.u8PRCodeStart,sizeof(UINT8));

	/* 1523 - 1524 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.au32DLMCS,sizeof(UINT32)*NUMBER_OF_FEC_TYPES);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.au32ULMCS,sizeof(UINT32)*NUMBER_OF_FEC_TYPES);

	/* 1525 - 1544 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8UplinkBurstDataZone,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DownlinkBurstDataZone,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8UplinkBurstDataFecScheme,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DLMapFECScheme,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32FrameNumber,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32LocalFrameNumber,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32UplinkBurstDataDuration,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32DownlinkBurstDataDuration,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32UplinkBurstDataSize,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32DownlinkBurstDataSize,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32UplinkRangingSeed,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32UplinkBurstDataCid,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u32DownlinkBurstDataCid,sizeof(UINT32));	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8UplinkBurstDataUiuc,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DownlinkBurstDataDiuc,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8UplinkBurstDataFecRepetition,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DownlinkBurstDataFecRepetition,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8UlFrameRatio,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DlFrameRatio,sizeof(UINT8));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tPhyStateInfo.u8DownlinkBurstDataFecScheme,sizeof(UINT8));

	/* 1545 - 1552 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32NumOfHoDrops,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32NumOfHoTrials,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32PingPongCount,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32HoTypeId,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32NumOfIntraFaNbrs,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32IntraFAPreambleIndex,sizeof(SINT32)*DM_MAX_NBRS);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32InterFAPreambleIndex,sizeof(SINT32)*DM_MAX_NBRS);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHoEventInfo.s32HOLatency,sizeof(SINT32));

	/* 1553 - 1554 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32FrameNumber,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.s32CurrentOffsetBs,sizeof(SINT32));

	/* 1555 - 1561 */

	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32ServingBsPreambleIndex,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32ServingBsSectorId,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32ServingBsCellId,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32TargetBsPreambleIndex,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32TargetBsSectorId,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32TargetBsCellId,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tHOEventInfoEx.u32HOStatus,sizeof(UINT32));

	/* 1562 - 1563 */		
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32PhySyncGtbCount,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.u32NwEntryGtbCount,sizeof(UINT32));

	/* 1564 - 1567 */			
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tErrorCounter.u32DLMAPCrcErrors,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tErrorCounter.u32DLMAPHcsErrors,sizeof(UINT32));	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tErrorCounter.u32PduCrcErrors,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.tErrorCounter.u32PduHcsErrors,sizeof(UINT32));
	/*1568*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandCompressedMapCount,sizeof(UINT32));
	/*1569*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumofResync,sizeof(UINT32));
	/*1570 - 1575 MibInitialization is not required because we are performing RDMs */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	/*MIB 1575 Modified for common BS Logger*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBaseBandBSTrackingInfo,sizeof(ST_BSTRACKER_INFO));
	/* 1576 - 1579 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandSAIdTable,sizeof(UINT32) * MAX_SAID_ENTRIES);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPrimaryCinr,sizeof(SINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDiversityCinr,sizeof(SINT32));

	/* 1580-1597: FMIBS Additions */
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDLFrameNum,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfInitRangeSuccess,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfPeriodicRangingSuccess,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHOAttempts,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfHOSuccess,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandTimeInSleepMode,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandTimeInIdleMode,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandTimeInActiveMode,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//BE
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//UGS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//RTPS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//eRTPS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//nRTPS

	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDLTrafficRate,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandULTrafficRate,sizeof(UINT32));
	
	
	// Need to add the statistics for the queue types 
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//BE
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//UGS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//RTPS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//eRTPS
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//nRTPS
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfFailedSFCreation,sizeof(UINT32));

	// Need to add the stats of num of dsx pass and dsds
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//DSA ACKs
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));//DCDs

	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandCurrIdCellSeg,sizeof(UINT32));
	
	/*String for decorated NAI*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNAILocation,sizeof(UINT32));
	
	/* 1608 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/* 1609 - 1611 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDeviceStatus,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandConnectionStatus,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandStatusReason,sizeof(UINT32));

	/* 1612 - 1616 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(ST_MRU_INFORMATION)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32) * MIBS_MAX_HIST_ENTRIES);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32) * MIBS_MAX_HIST_ENTRIES);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/* 1617 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPreHarqBurstErrorRateUpper,sizeof(UINT32));

	/* 1618  when NAI validity MIB*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNAIValid, sizeof(UINT32));	

	/* 1619 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNwEntryBrkUp, sizeof(ST_BCMBASEBAND_NwENTRY_TIMING_STATS));	

	/* 1620 - 1624*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPostHarqBurstErrorRateLower,sizeof(UINT32));	
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfULHARQAckEnabledBurstAllocs, sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfULHARQAckEnabledReTx, sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPostHarqBurstErrorRateUpper, sizeof(UINT32));	
	
	/* 1625 - 1628 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPerZoneMCS, sizeof(ST_PER_ZONE_MCS_STATS));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandCbyNTable, sizeof(ST_BCMBASEBAND_CbyNTable));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNI, sizeof(ST_BCMBASEBAND_NI));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandScanParams, sizeof(ST_BCMBASEBAND_ScanParams));

	/* HO information*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHOInformation, sizeof(ST_HANDOVER_INFORMATION));
	/* Idle mode  information*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandIdleModeInformation, sizeof(ST_IDLEMODE_INFORMATION));

	/* 1631 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandTxAntSwitchInformation, sizeof(ST_TXANTSWITCH_INFORMATION));

	/* 1632 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct *)NULL)->Statistics.tLinkStats.u32MimoModeReport,sizeof(UINT32));

	/* 1633 for WiMAX trigger */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/* 1634 to 1636 For Harq*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPerRxBurstErrorRateUpper,sizeof(UINT32) * 4);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlPostHarqConfirmedFailures,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandDlHarqTransactionsInProgress,sizeof(UINT32));


	/* 1637 - 1648 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/* 1649 - 1653 */
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandTxAAsoundingType,sizeof(UINT32) * 4);
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumULSoundingZone,sizeof(UINT32));	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandIsCompressedMap,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNonHarqNumOfDlBurstsUnicastBurstErrorRate,sizeof(UINT32));	
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNonHarqNumOfDlBurstsBcstBurstErrorRate,sizeof(UINT32));

	/* 1654 - 1665*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));

	/*1666 -1668*/
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32));
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)NULL,sizeof(UINT32)*4);

#undef MIB_ATTRIBUTE

#define MIB_ATTRIBUTE BIT_HOST_STD_STATS|BIT_NON_INDEX
	//1669
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((S_MIBS_HOST_STATS_MIBS * )NULL)->stHostInfo.GoodReceives,sizeof(ULONG64));

	//1670
	AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((S_MIBS_HOST_STATS_MIBS * )NULL)->stHostInfo.GoodTransmits,sizeof(ULONG64));

#undef MIB_ATTRIBUTE
	
	
	
#define MIB_ATTRIBUTE BIT_TGT_STD_STATS|BIT_NON_INDEX
	//1671-1672
		AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPerZoneMCS.stPerZoneInfo.stPerDLZoneInfo,sizeof(ST_PERZONE_DL_INFO));
		AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandPerZoneMCS.stPerZoneInfo.stPerULZoneInfo,sizeof(ST_PERZONE_UL_INFO));
		
	//1673-1674	
		AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandFlashMapInfo,sizeof(ST_FLASHMAPDETAILS));
		AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandSCSIFwVersion,sizeof(UINT32));
	
	//1675
		AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandEthCSSupport,sizeof(B_UINT32));
    //1676
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.BcmBasebandAMuxnVAIN1Q7,sizeof(UINT32));
	
	//1677-1692
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.s32wmanIfTxPowerInPt5dB,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfTxPowHeadroomInPt5dB,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfRngRspForIrCode,sizeof(UINT32));	
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfNoRngRspForIrCode,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfRngRspForPrCode,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfNoRngRspForPrCode,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfHoIndSuccess,sizeof(UINT32));		
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfIRReqtoRegRspTime,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfNumOfRegRspRcvd,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfNumIRReqWithNoRegRsp,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfTotNumIRReqSinceReset,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfTemperature,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfSecsSincePowerCycle,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.u32wmanIfArqNumReTxBlocks,sizeof(UINT32));
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.stwmanIfDLSFInfo,sizeof(ST_PER_SF_INFO)*NUMBER_OF_DL_UL_SF_INFO);
        AddPropreiteryStatsMIB(pWiMAX, (UADDTYPE)&((stInternalStatsStruct * )NULL)->tMibstats.stwmanIfULSFInfo,sizeof(ST_PER_SF_INFO)*NUMBER_OF_DL_UL_SF_INFO);




}

VOID BeceemWiMAX_InitializeMIBTable(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX)
		return;

	/*Initialize Standard Config MIBS*/
	BeceemWiMAX_InitializeStdConfigMIBs(pWiMAX);

	/*Initialize Proprietary Config MIBS*/
	BeceemWiMAX_InitializePropCFGMIBs(pWiMAX);

	/*Initialize Standard Statistics MIBS*/
	BeceemWiMAX_InitializeStdStatsMIBs(pWiMAX);

	/*Initialize Proprietary Statistics MIBS*/
	BeceemWiMAX_InitializePropStatsMIBs(pWiMAX);
}
