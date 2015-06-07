/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "BeceemAPI.h"
#include "MibUserApi.h"
#include "Utility.h"

// ---------------------------------------
// Network search parameters
// ---------------------------------------

hNetSearchParameters_t NetSearchParameters_Create(void)
	{
	hNetSearchParameters_t hParams;

	if ((hParams = WcmMalloc(sizeof(stNetSearchParameters_t))) == NULL)
		return NULL;

	memset(hParams, 0, sizeof(stNetSearchParameters_t));

	return hParams;

	} /* hNetSearchParameters_t */

void NetSearchParameters_Destroy(hNetSearchParameters_t hParams)
	{
	WcmFree(hParams);
	} /* NetSearchParameters_Destroy */

// ---------------------------------------
// Base station information
// ---------------------------------------

hBSInformation_t BSInformation_Create(void)
	{

	hBSInformation_t hBSInfo;

	if ((hBSInfo = WcmMalloc(sizeof(stBSInformation_t))) == NULL)
		return NULL;

	BSInformation_SetDefault(hBSInfo);

	return hBSInfo;

	} /* BSInformation_Create */

hBSInformation_t BSInformation_CreateEx(
		UINT8  au8BSID_val[],
		UINT32 u32PreambleIndex_val,
		UINT32 u32CenterFrequency_val,
		UINT32 u32Bandwidth_val,
		UINT32 u32RelativeSignalStrength_val,
		INT32  s32CINR_val)
	{

	hBSInformation_t hBSInfo;

	if ((hBSInfo = BSInformation_Create()) == NULL)
		return NULL;
	
	if (au8BSID_val != NULL)
		memcpy(hBSInfo->au8BSID, au8BSID_val, sizeof(hBSInfo->au8BSID));
	hBSInfo->u32PreambleIndex		= u32PreambleIndex_val;
	hBSInfo->u32CenterFrequency		= u32CenterFrequency_val;
	hBSInfo->u32Bandwidth			= u32Bandwidth_val;
	hBSInfo->s32RelativeSignalStrength = (INT32) u32RelativeSignalStrength_val;
	hBSInfo->s32CINR				= s32CINR_val;

	return hBSInfo;

	} /* BSInformation_CreateEx */

void BSInformation_SetDefault (hBSInformation_t hBSInfo)
	{
	memset(hBSInfo, 0, sizeof(stBSInformation_t));
	hBSInfo ->u32PreambleIndex = 0xFF;
	}

void BSInformation_Destroy (hBSInformation_t hBSInfo)
	{
	WcmFree(hBSInfo);
	} /* BSInformation_Destroy */

void BSInformation_Print (hBSInformation_t hBSInfo, PrintContext_t hPrint, int n)
	{

	if (!hBSInfo || !hPrint)
		return;

	Print_Formatted(hPrint,S("\nBase Station %d\n"), n+1);
	Print_Formatted(hPrint,S("    Preamble Index ID         \t0x%04X\n"), hBSInfo->u32PreambleIndex);
	Print_Formatted(hPrint,S("    Center Frequency		  \t%4d\n"),    hBSInfo->u32CenterFrequency);
	Print_Formatted(hPrint,S("    Bandwidth				  \t%4d\n"),    hBSInfo->u32Bandwidth);
	Print_Formatted(hPrint,S("    Relative Signal Strength  \t0x%08X\n"), hBSInfo->s32RelativeSignalStrength);
	Print_Formatted(hPrint,S("    CINR                      \t%4d\n"),    hBSInfo->s32CINR);

	} /* BSInformation_Print */

// -------------------------------------------
// Various counters used in the Statistics tab
// -------------------------------------------

hBBStatisticsCounters_t BBStatisticsCounters_Create (void)
	{

	hBBStatisticsCounters_t hCounters;

	if ((hCounters = WcmMalloc(sizeof(stBBStatisticsCounters_t))) == NULL)
		return NULL;

	memset(hCounters, 0, sizeof(stBBStatisticsCounters_t));

	return hCounters;

	} /* BBStatisticsCounters_Create */

void BBStatisticsCounters_Destroy (hBBStatisticsCounters_t hCounters)
	{
	WcmFree(hCounters);
	} /* BBStatisticsCounters_Destroy */

// -------------------------------------------
// MIBs for statistics report
// -------------------------------------------

// MIBs for default Stats tab
static const stMibData_t aBasicStatisticsReportMibs[] = { 

	{BCM_STATS_MODEMSTATE,     MIB_DATA_TYPE_UINT32,       S("Modem state           ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("")},
	{BCM_STATS_BSID,           MIB_DATA_TYPE_UINT8,        S("Base ID               ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("")},
	{BCM_STATS_PHY_FRAME_NUM,  MIB_DATA_TYPE_UINT32,       S("Frame number          ")},                   
	{BCM_STATS_NUMFRAMESRECVD, MIB_DATA_TYPE_UINT32,       S("Num frames received   ")},
	{BCM_STATS_NUMFRAMESINERR, MIB_DATA_TYPE_UINT32,       S("Frames in error       ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("")},
	{BCM_STATS_NUMCRCERR,      MIB_DATA_TYPE_UINT32,       S("Num Of CRC errors     ")},
	{BCM_STATS_NUMHCSERR,      MIB_DATA_TYPE_UINT32,       S("Num Of HCS errors     ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("")},
	{BCM_BASEBAND_CENTERFREQ,  MIB_DATA_TYPE_UINT32,       S("CenterFrequency (kHz) ")},
	{BCM_STATS_CURRENT_CINR,   MIB_DATA_TYPE_INT32,        S("CINR (dB)             ")},
	{BCM_STATS_CURRENT_RSSI,   MIB_DATA_TYPE_INT32,        S("RSSI (dBm)            ")},
	{BCM_STATS_TXPOWER,        MIB_DATA_TYPE_SINT8,        S("Tx power (dBm)        ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("\nCurrent data rates:")},
	{BCM_BASEBAND_DLRATE,      MIB_DATA_TYPE_UINT32,       S("     DL rate (kbps)   ")},
	{BCM_BASEBAND_ULRATE,      MIB_DATA_TYPE_UINT32,       S("     UL rate (kbps)   ")},

//	{MIB_DATA_ID_HEADING,          MIB_DATA_TYPE_HEADING,  S("\nTraffic data rates:")},
//	{BCM_BASEBAND_DL_TRAFFIC_RATE, MIB_DATA_TYPE_UINT32,   S("     DL rate (kbps)   ")},
//	{BCM_BASEBAND_UL_TRAFFIC_RATE, MIB_DATA_TYPE_UINT32,   S("     UL rate (kbps)   ")},


	{MIB_DATA_ID_HEADING,	   MIB_DATA_TYPE_HEADING,      S("\nNumber of packets:")},
	{BCM_BASEBAND_TX_PACKETS_HISTOGRAM, MIB_DATA_TYPE_UINT32,  
														   S("     From Host        ")},
	{BCM_BASEBAND_RX_PACKETS_HISTOGRAM, MIB_DATA_TYPE_UINT32,  
														   S("     To Host          ")},
	{MIB_DATA_ID_HEADING,          MIB_DATA_TYPE_HEADING,  S("\nNumber of bytes:")},
	{BCM_STATS_NUMBYTES_FROM_HOST, MIB_DATA_TYPE_UINT32,   S("     From Host        ")},
	{BCM_STATS_NUMBYTES_TO_HOST,   MIB_DATA_TYPE_UINT32,   S("     To Host          ")},

	{MIB_DATA_ID_HEADING,      MIB_DATA_TYPE_HEADING,      S("\nNumber of connections:")},
	{BCM_STATS_NUM_UL_CONNS,   MIB_DATA_TYPE_UINT32,       S("     UL               ")},
	{BCM_STATS_NUM_DL_CONNS,   MIB_DATA_TYPE_UINT32,       S("     DL               ")},

	{0, 0, NULL}
	};

// MIBs for Phy Stats report
static const stMibData_t aPhyStatisticsReportMibs[] = {
	{MIB_DATA_ID_HEADING,                          MIB_DATA_TYPE_HEADING,      S("Number of DL bursts:")},
	{BCM_STATS_BURSTRCVQPSK12,                     MIB_DATA_TYPE_UINT32,       S("     QPSK 1/2         ")},
	{BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP1,     MIB_DATA_TYPE_UINT32,       S("        QPSK 1/2-1    ")},
	{BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP2,     MIB_DATA_TYPE_UINT32,       S("        QPSK 1/2-2    ")},
	{BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP4,     MIB_DATA_TYPE_UINT32,       S("        QPSK 1/2-4    ")},
	{BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP6,     MIB_DATA_TYPE_UINT32,       S("        QPSK 1/2-6    ")},
	{BCM_STATS_BURSTRCVQPSK34,                     MIB_DATA_TYPE_UINT32,       S("     QPSK 3/4         ")},
	{BCM_STATS_BURSTRCV16QAM12,                    MIB_DATA_TYPE_UINT32,       S("     16QAM 1/2        ")},
	{BCM_STATS_BURSTRCV16QAM34,                    MIB_DATA_TYPE_UINT32,       S("     16QAM 3/4        ")},
	{BCM_STATS_BURSTRCV64QAM12,                    MIB_DATA_TYPE_UINT32,       S("     64QAM 1/2        ")},
	{BCM_STATS_BURSTRCV64QAM23,                    MIB_DATA_TYPE_UINT32,       S("     64QAM 2/3        ")},
	{BCM_STATS_BURSTRCV64QAM34,                    MIB_DATA_TYPE_UINT32,       S("     64QAM 3/4        ")},
	{BCM_STATS_BURSTRCV64QAM56,                    MIB_DATA_TYPE_UINT32,       S("     64QAM 5/6        ")},

	{MIB_DATA_ID_HEADING,                          MIB_DATA_TYPE_HEADING,      S("\nNumber of UL bursts:")},
	{BCM_STATS_BURSTSNDQPSK12,                     MIB_DATA_TYPE_UINT32,       S("     QPSK 1/2         ")},
	{BCM_STATS_BURSTSNDQPSK34,                     MIB_DATA_TYPE_UINT32,       S("     QPSK 3/4         ")},
	{BCM_STATS_BURSTSND16QAM12,                    MIB_DATA_TYPE_UINT32,       S("     16QAM 1/2        ")},
	{BCM_STATS_BURSTSND16QAM34,                    MIB_DATA_TYPE_UINT32,       S("     16QAM 3/4        ")},
	{0, 0, NULL}
	};

#if 0
// MIBS for service flow report
static const stMibData_t aServiceFlowReportMibs[] = {
	{WMAN_IF_CMN_CPSSFDIRECTION,	  MIB_DATA_TYPE_UINT8, S("     Direction        ")},
	{WMAN_IF_CMN_CPSMAXSUSTAINEDRATE, MIB_DATA_TYPE_UINT32,S("     Max rate (kbps)  ")},

	{0, 0, NULL}

	};
#endif

// MIBS for service flow report
static const stMibData_t aServiceFlowTypeStatisticsReportMibs[] = {
	{MIB_DATA_ID_HEADING,               MIB_DATA_TYPE_HEADING,  S("Number of bytes:")},
	{BCM_BASEBAND_NUMOF_TX_BE_BYTES,    MIB_DATA_TYPE_UINT32,   S("     TX BE           ")},
	{BCM_BASEBAND_NUMOF_RX_BE_BYTES,    MIB_DATA_TYPE_UINT32,   S("     RX BE           ")},
	{BCM_BASEBAND_NUMOF_TX_UGS_BYTES,   MIB_DATA_TYPE_UINT32,   S("     TX UGS          ")},
	{BCM_BASEBAND_NUMOF_RX_UGS_BYTES,   MIB_DATA_TYPE_UINT32,   S("     RX UGS          ")},
	{BCM_BASEBAND_NUMOF_TX_RTPS_BYTES,  MIB_DATA_TYPE_UINT32,   S("     TX RTPS         ")},
	{BCM_BASEBAND_NUMOF_RX_RTPS_BYTES,  MIB_DATA_TYPE_UINT32,   S("     RX RTPS         ")},
	{BCM_BASEBAND_NUMOF_TX_ERTPS_BYTES, MIB_DATA_TYPE_UINT32,   S("     TX ERTPS        ")},
	{BCM_BASEBAND_NUMOF_RX_ERTPS_BYTES, MIB_DATA_TYPE_UINT32,   S("     RX ERTPS        ")},
	{BCM_BASEBAND_NUMOF_TX_NRTPS_BYTES, MIB_DATA_TYPE_UINT32,   S("     TX NRTPS        ")},
	{BCM_BASEBAND_NUMOF_RX_NRTPS_BYTES, MIB_DATA_TYPE_UINT32,   S("     RX NRTPS        ")},

	{0, 0, NULL}

	};

// Statistics report types
static stLinkStatsData_t stStatsTableData[]= {
				     {S("basic"),  aBasicStatisticsReportMibs,           FALSE}, 
				     {S("phy"),    aPhyStatisticsReportMibs,             FALSE},
				     {S("sftype"), aServiceFlowTypeStatisticsReportMibs, FALSE},
				     {NULL, NULL, FALSE}
				    };

hLinkStatsData_t MibData_Create (void)
	{
	return (hLinkStatsData_t) &stStatsTableData;
	}

#ifdef MS_VISUAL_CPP
#pragma warning (push)
#pragma warning (disable:4100)
#endif

void MibData_Destroy (hLinkStatsData_t hLinkStatsData)
	{
	}

#ifdef MS_VISUAL_CPP
#pragma warning (pop)
#endif


