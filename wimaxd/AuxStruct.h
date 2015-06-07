/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef AUX_STRUCT_H
#define AUX_STRUCT_H

// ---------------------------------------
// Network search parameters
// ---------------------------------------

typedef struct {

	UINT32 u32NumberOfBandwidths;
	UINT32 u32BandwidthKHz[MAX_NUM_BANDWIDTHS_SUPPORTED];

	UINT32 u32ScanMinFrequencyKHz;
	UINT32 u32ScanMaxFrequencyKHz;
	UINT32 u32ScanStepFrequencyKHz;

	UINT32 u32NumberOfFrequencies;
	UINT32 u32CenterFrequencyKHz[MAX_NUM_DISCRETE_CF_SUPPORTED];

	BOOL   bSearchMoreCF;
	UINT32 u32FirstFreqIndex;
	UINT32 u32LastFreqIndex;

	} stNetSearchParameters_t;

typedef stNetSearchParameters_t *hNetSearchParameters_t;

hNetSearchParameters_t NetSearchParameters_Create(void);
void NetSearchParameters_Destroy(hNetSearchParameters_t hParams);

// ---------------------------------------
// Base station information
// ---------------------------------------

typedef struct {

	/**
		BSID. 
		The first byte will indicate whether it is compressed map.
		The next 7 bytes of the array will contain the valid BSID value.
	*/
	UINT8	au8BSID[8];	
	/** Preamble index */
	UINT32	u32PreambleIndex; 
	/** Centre frequency (in kHz) */
	UINT32	u32CenterFrequency;
	/** Bandwidth (in kHz) */
	UINT32	u32Bandwidth;
	INT32	s32RelativeSignalStrength; 
	INT32	s32CINR;

	} stBSInformation_t;

typedef stBSInformation_t *hBSInformation_t;

hBSInformation_t BSInformation_Create(void);
hBSInformation_t BSInformation_CreateEx(	
		UINT8  au8BSID_val[],
		UINT32 u32PreambleIndex_val,
		UINT32 u32CenterFrequency_val,
		UINT32 u32Bandwidth_val,
		UINT32 u32RelativeSignalStrength_val,
		INT32  s32CINR_val);
void BSInformation_SetDefault (hBSInformation_t hBS);
void BSInformation_Destroy (hBSInformation_t hBSInfo);
void BSInformation_Print (hBSInformation_t ahBSInfo, PrintContext_t hPrint, int n);

// -------------------------------------------
// Various counters used in the Statistics tab
// -------------------------------------------

typedef struct BBStatisticsCounters {

	UINT32	  u32NumOfFramesRecvd;
	UINT32    u32NumOfFramesInError;
	UINT32    u32NumOfCrcErrors;
	UINT32    u32NumOfHcsErrors;
	UINT32    u32NumOfBytesFromHost;
	UINT32    u32NumOfBytesToHost;
	UINT32    u32NumOfPacketsFromHost;
	UINT32    u32NumOfPacketsToHost;

	UINT32    u32NumOfRcvQPSK12CTCBurstsRep6;
	UINT32    u32NumOfRcvQPSK12CTCBurstsRep4;
	UINT32    u32NumOfRcvQPSK12CTCBurstsRep2;
	UINT32    u32NumOfRcvQPSK12CTCBurstsRep1;

	UINT32    u32NumOfBurstRcvQPSK12;
	UINT32    u32NumOfBurstRcvQPSK34;
	UINT32    u32NumOfBurstRcv16QAM12;
	UINT32    u32NumOfBurstRcv16QAM34;
	UINT32    u32NumOfBurstRcv64QAM12;
	UINT32    u32NumOfBurstRcv64QAM23;
	UINT32    u32NumOfBurstRcv64QAM34;
	UINT32    u32NumOfBurstRcv64QAM56;

	UINT32    u32NumOfBurstSndQPSK12;
	UINT32    u32NumOfBurstSndQPSK34;
	UINT32    u32NumOfBurstSnd16QAM12;
	UINT32    u32NumOfBurstSnd16QAM34;

	UINT32    u32NumOfTxBEBytes;
	UINT32    u32NumOfRxBEBytes;
	UINT32    u32NumOfTxUGSBytes;
	UINT32    u32NumOfRxUGSBytes;
	UINT32    u32NumOfTxRTPSBytes;
	UINT32    u32NumOfRxRTPSBytes;
	UINT32    u32NumOfTxERTPSBytes;
	UINT32    u32NumOfRxERTPSBytes;
	UINT32    u32NumOfTxNRTPSBytes;
	UINT32    u32NumOfRxNRTPSBytes;

	} stBBStatisticsCounters_t;

typedef stBBStatisticsCounters_t *hBBStatisticsCounters_t;

hBBStatisticsCounters_t BBStatisticsCounters_Create (void);
void BBStatisticsCounters_Destroy (hBBStatisticsCounters_t hCounters);

// -------------------------------------------
// MIBs for statistics report
// -------------------------------------------

#define MIB_DATA_ID_HEADING   -1

#define MIB_DATA_TYPE_HEADING 0
#define MIB_DATA_TYPE_UINT8   1
#define MIB_DATA_TYPE_SINT8   2
#define MIB_DATA_TYPE_UINT32  3
#define MIB_DATA_TYPE_INT32   4

#define DEFAULT_STATS_REPORT_INDEX 0
#define MAX_STATS_REPORT   3

typedef struct {
	const INT32   i32MibID;
	const UINT32  u32MibType;
	const STRING  bszMIBDescription;
	} stMibData_t;

typedef const stMibData_t *hMibData_t;

typedef struct {
	STRING		bszStatsRptName;
	hMibData_t	hStatisticsReport;
   	BOOL		bBBStatsOffset;
	} stLinkStatsData_t;
typedef stLinkStatsData_t *hLinkStatsData_t;

hLinkStatsData_t MibData_Create (void);
void MibData_Destroy (hLinkStatsData_t hLinkStatsData);

#endif

