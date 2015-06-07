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
 * Description	:	This file defines the NVM Interface API and
 *					related Macros
 * Author		:	Viswanath Dibbur
 * Reviewer		:
 *
 */


#define _UTI_BECEEMEEPROMCFGSTRUCT_CPP


#include "uti_BeceemEEPROMCfg.h"


/* **************************************** HW ************************************ */

static CFG_OFFSETTABLE CFG_HW_18[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{144,33554434},
		// GPIO-0 Functionality Setting
		{3,16777217},
		// GPIO-1 Functionality Setting
		{4,16777217},
		// GPIO-2 Functionality Setting
		{5,16777217},
		// GPIO-3 Functionality Setting
		{6,16777217},
		// GPIO-4 Functionality Setting
		{7,16777217},
		// GPIO-5 Functionality Setting
		{8,16777217},
		// GPIO-6 Functionality Setting
		{9,16777217},
		// GPIO-7 Functionality Setting
		{10,16777217},
		// GPIO-8 Functionality Setting
		{11,16777217},
		// GPIO-9 Functionality Setting
		{12,16777217},
		// GPIO-10 Functionality Setting
		{13,16777217},
		// GPIO-11 Functionality Setting
		{14,16777217},
		// GPIO-12 Functionality Setting
		{15,16777217},
		// GPIO-13 Functionality Setting
		{16,16777217},
		// GPIO-14 Functionality Setting
		{17,16777217},
		// GPIO-15 Functionality Setting
		{18,16777217},
		// GPIO-16 Functionality Setting
		{19,16777217},
		// GPIO-17 Functionality Setting
		{20,16777217},
		// GPIO-18 Functionality Setting
		{21,16777217},
		// GPIO-19 Functionality Setting
		{22,16777217},
		// GPIO-20 Functionality Setting
		{23,16777217},
		// GPIO-21 Functionality Setting
		{24,16777217},
		// GPIO-22 Functionality Setting
		{25,16777217},
		// GPIO-23 Functionality Setting
		{26,16777217},
		// GPIO-24 Functionality Setting
		{27,16777217},
		// GPIO-25 Functionality Setting
		{28,16777217},
		// GPIO-26 Functionality Setting
		{29,16777217},
		// GPIO-27 Functionality Setting
		{30,16777217},
		// GPIO-28 Functionality Setting
		{31,16777217},
		// GPIO-29 Functionality Setting
		{32,16777217},
		// GPIO-30 Functionality Setting
		{33,16777217},
		// GPIO-31 Functionality Setting
		{34,16777217},
		// GPIO-32 Functionality Setting
		{35,16777217},
		// GPIO-33 Functionality Setting
		{36,16777217},
		// GPIO-34 Functionality Setting
		{37,16777217},
		// GPIO-35 Functionality Setting
		{38,16777217},
		// GPIO-36 Functionality Setting
		{39,16777217},
		// GPIO-37 Functionality Setting
		{40,16777217},
		// GPIO-38 Functionality Setting
		{41,16777217},
		// GPIO-39 Functionality Setting
		{42,16777217},
		// GPIO-40 Functionality Setting
		{43,16777217},
		// GPIO-41 Functionality Setting
		{44,16777217},
		// GPIO-42 Functionality Setting
		{45,16777217},
		// GPIO-43 Functionality Setting
		{46,16777217},
		// GPIO-44 Functionality Setting
		{47,16777217},
		// GPIO-45 Functionality Setting
		{48,16777217},
		// GPIO-46 Functionality Setting
		{49,16777217},
		// GPIO-47 Functionality Setting
		{50,16777217},
		// GPIO-48 Functionality Setting
		{51,16777217},
		// GPIO-49 Functionality Setting
		{52,16777217},
		// GPIO-50 Functionality Setting
		{53,16777217},
		// GPIO-51 Functionality Setting
		{54,16777217},
		// GPIO-52 Functionality Setting
		{55,16777217},
		// GPIO-53 Functionality Setting
		{56,16777217},
		// GPIO-55 Functionality Setting
		{57,16777217},
		// GPIO-56 Functionality Setting
		{58,16777217},
		// GPIO-57 Functionality Setting
		{59,16777217},
		// GPIO-58 Functionality Setting
		{60,16777217},
		// GPIO-59 Functionality Setting
		{61,16777217},
		// GPIO-60 Functionality Setting
		{62,16777217},
		// GPIO-61 Functionality Setting
		{63,16777217},
		// GPIO-62 Functionality Setting
		{64,16777217},
		// GPIO-63 Functionality Setting
		{65,16777217},
		// SPIO-0 Functionality Setting
		{66,16777217},
		// SPIO-1 Functionality Setting
		{67,16777217},
		// SPIO-2 Functionality Setting
		{68,16777217},
		// SPIO-3 Functionality Setting
		{69,16777217},
		// SPIO-4 Functionality Setting
		{70,16777217},
		// SPIO-5 Functionality Setting
		{71,16777217},
		// SPIO-6 Functionality Setting
		{72,16777217},
		// SPIO-7 Functionality Setting
		{73,16777217},
		// SPIO-8 Functionality Setting
		{74,16777217},
		// SPIO-9 Functionality Setting
		{75,16777217},
		// SPIO-10 Functionality Setting
		{76,16777217},
		// SPIO-11 Functionality Setting
		{77,16777217},
		// SPIO-12 Functionality Setting
		{78,16777217},
		// SPIO-13 Functionality Setting
		{79,16777217},
		// SPIO-14 Functionality Setting
		{80,16777217},
		// SPIO-15 Functionality Setting
		{81,16777217},
		// PMU Field 1
		{82,16777217},
		// PMU Field 2
		{83,16777217},
		// PMU Field 3
		{84,16777217},
		// PMU Field 3A
		{85,16777217},
		// PMU Field 4
		{86,16777217},
		// PMU Field 5
		{87,16777217},
		// PMU Field 6
		{88,16777217},
		// PMU Field 7
		{89,16777217},
		// PMU Field 8
		{90,16777217},
		// PMU Field 9
		{91,16777217},
		// IMEI
		{92,16777240},
		// Reserved
		{116,16777242},
		// Content Version
		{142,16777217},
		// Padding
		{143,16777217},
};


static CFG_OFFSETTABLE CFG_HW_16[] = {
            /* Length */
            {0,33554434},
            /* Version */
            {2,16777217},
            /* Checksum */
            {88,33554434},
            /* GPIO-0 Functionality Setting */
            {3,16777217},
            /* GPIO-1 Functionality Setting */
            {4,16777217},
            /* GPIO-2 Functionality Setting */
            {5,16777217},
            /* GPIO-3 Functionality Setting */
            {6,16777217},
            /* GPIO-4 Functionality Setting */
            {7,16777217},
            /* GPIO-5 Functionality Setting */
            {8,16777217},
            /* GPIO-6 Functionality Setting */
            {9,16777217},
            /* GPIO-7 Functionality Setting */
            {10,16777217},
            /* GPIO-8 Functionality Setting */
            {11,16777217},
            /* GPIO-9 Functionality Setting */
            {12,16777217},
            /* GPIO-10 Functionality Setting */
            {13,16777217},
            /* GPIO-11 Functionality Setting */
            {14,16777217},
            /* GPIO-12 Functionality Setting */
            {15,16777217},
            /* GPIO-13 Functionality Setting */
            {16,16777217},
            /* GPIO-14 Functionality Setting */
            {17,16777217},
            /* GPIO-15 Functionality Setting */
            {18,16777217},
            /* GPIO-16 Functionality Setting */
            {19,16777217},
            /* GPIO-17 Functionality Setting */
            {20,16777217},
            /* GPIO-18 Functionality Setting */
            {21,16777217},
            /* GPIO-19 Functionality Setting */
            {22,16777217},
            /* GPIO-20 Functionality Setting */
            {23,16777217},
            /* GPIO-21 Functionality Setting */
            {24,16777217},
            /* GPIO-22 Functionality Setting */
            {25,16777217},
            /* GPIO-23 Functionality Setting */
            {26,16777217},
            /* GPIO-24 Functionality Setting */
            {27,16777217},
            /* GPIO-25 Functionality Setting */
            {28,16777217},
            /* GPIO-26 Functionality Setting */
            {29,16777217},
            /* GPIO-27 Functionality Setting */
            {30,16777217},
            /* GPIO-28 Functionality Setting */
            {31,16777217},
            /* GPIO-29 Functionality Setting */
            {32,16777217},
            /* GPIO-30 Functionality Setting */
            {33,16777217},
            /* GPIO-31 Functionality Setting */
            {34,16777217},
            /* GPIO-32 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-33 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-34 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-35 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-36 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-37 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-38 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-39 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-40 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-41 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-42 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-43 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-44 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-45 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-46 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-47 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-48 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-49 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-50 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-51 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-52 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-53 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-55 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-56 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-57 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-58 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-59 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-60 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-61 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-62 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* GPIO-63 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-0 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-1 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-2 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-3 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-4 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-5 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-6 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-7 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-8 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-9 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-10 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-11 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-12 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-13 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-14 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* SPIO-15 Functionality Setting */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 1 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 2 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 3 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 3A */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 4 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 5 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 6 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 7 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 8 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
            /* PMU Field 9 */
            {CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},            
            /* IMEI */
            {35,16777240},
            /* Reserved */
            {59,16777242},
            /* Padding */
            {85,16777217},
};



/* ************************************** MIBS *********************************** */
static CFG_OFFSETTABLE CFG_MIB_20[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{592,33554434},
		// MIBFlag
		{3,16777217},
		// BcmConfigNetSearchPeriodicWakeup
		{4,67108868},
		// BcmConfigUartFunctionality
		{8,67108868},
		// BPFSReserved
		{12,67109364},
		// wmanif2MSST44Timer
		{512,67108868},
		// wmanIfSsLostDLMapInterval
		{516,33554434},
		// wmanIfSsLostULMapInterval 
		{518,33554434},
		// wmanIfSsContentionRangRetries
		{520,33554434},
		// wmanIfSsRequestRetries 
		{522,33554434},
		// wmanIfSsRegRequestRetries
		{524,33554434},
		// wmanIfSsT1Timeout
		{526,33554434},
		// wmanIfSsT2Timeout
		{528,33554434},
		// wmanIfSsT6Timeout
		{530,33554434},
		// wmanIfSsT12Timeout
		{532,33554434},
		// wmanIfSsT18Timeout
		{534,33554434},
		// wmanIfSsT20Timeout
		{536,33554434},
		// wmanIfSsT21Timeout
		{538,33554434},
		// wmanIfSsT14Timeout
		{540,16777217},
		// wmanIfSsT3Timeout
		{541,16777217},
		// wmanIfSsT4Timeout
		{542,16777217},
		// wmanIfSsSBCRequestRetries
		{543,16777217},
		// wmanIfCmnInvitedRangRetries
		{544,33554434},
		// wmanIfCmnT7Timeout
		{546,33554434},
		// wmanIfCmnT8Timeout
		{548,33554434},
		// wmanIfCmnT10Timeout
		{550,33554434},
		// wmanIfCmnT22Timeout
		{552,33554434},
		// u16Reserved1
		{554,33554434},
		// wmanIfCmnDSxReqRetries
		{556,67108868},
		// wmanIfCmnDSxRespRetries
		{560,67108868},
		// wmanMssSpFrequency
		{564,67108868},
		// wmanMssNap
		{568,67108868},
		// wmanMssChanBandwidth
		{572,67108868},
		// wmanIfCmnCryptoSuiteIndex-1
		{576,67108868},
		// wmanIfCmnCryptoSuiteDataEncryptAlg-1
		{580,16777217},
		// wmanIfCmnCryptoSuiteDataAuthentAlg-1
		{581,16777217},
		// wmanIfCmnCryptoSuiteTekEncryptAlg-1
		{582,16777217},
		// u8Reserved-1
		{583,16777217},
		// wmanIfCmnCryptoSuiteIndex-2
		{584,67108868},
		// wmanIfCmnCryptoSuiteDataEncryptAlg-2
		{588,16777217},
		// wmanIfCmnCryptoSuiteDataAuthentAlg-2
		{589,16777217},
		// wmanIfCmnCryptoSuiteTekEncryptAlg-2
		{590,16777217},
		// Content Version
		{591,16777217},
};



static CFG_OFFSETTABLE CFG_MIB_19[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{592,33554434},
		// MIBFlag
		{3,16777217},
		// BcmConfigNetSearchPeriodicWakeup
		{4,67108868},
		// BcmConfigUartFunctionality
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// BPFSReserved
		{8,67109372},
		// wmanif2MSST44Timer
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// wmanIfSsLostDLMapInterval
		{516,33554434},
		// wmanIfSsLostULMapInterval
		{518,33554434},
		// wmanIfSsContentionRangRetries
		{520,33554434},
		// wmanIfSsRequestRetries
		{522,33554434},
		// wmanIfSsRegRequestRetries
		{524,33554434},
		// wmanIfSsT1Timeout
		{526,33554434},
		// wmanIfSsT2Timeout
		{528,33554434},
		// wmanIfSsT6Timeout
		{530,33554434},
		// wmanIfSsT12Timeout
		{532,33554434},
		// wmanIfSsT18Timeout
		{534,33554434},
		// wmanIfSsT20Timeout
		{536,33554434},
		// wmanIfSsT21Timeout
		{538,33554434},
		// wmanIfSsT14Timeout
		{540,16777217},
		// wmanIfSsT3Timeout
		{541,16777217},
		// wmanIfSsT4Timeout
		{542,16777217},
		// wmanIfSsSBCRequestRetries
		{543,16777217},
		// wmanIfCmnInvitedRangRetries
		{544,33554434},
		// wmanIfCmnT7Timeout
		{546,33554434},
		// wmanIfCmnT8Timeout
		{548,33554434},
		// wmanIfCmnT10Timeout
		{550,33554434},
		// wmanIfCmnT22Timeout
		{552,33554434},
		// u16Reserved1
		{554,33554434},
		// wmanIfCmnDSxReqRetries
		{556,67108868},
		// wmanIfCmnDSxRespRetries
		{560,67108868},
		// wmanMssSpFrequency
		{564,67108868},
		// wmanMssNap
		{568,67108868},
		// wmanMssChanBandwidth
		{572,67108868},
		// wmanIfCmnCryptoSuiteIndex-1
		{576,67108868},
		// wmanIfCmnCryptoSuiteDataEncryptAlg-1
		{580,16777217},
		// wmanIfCmnCryptoSuiteDataAuthentAlg-1
		{581,16777217},
		// wmanIfCmnCryptoSuiteTekEncryptAlg-1
		{582,16777217},
		// u8Reserved-1
		{583,16777217},
		// wmanIfCmnCryptoSuiteIndex-2
		{584,67108868},
		// wmanIfCmnCryptoSuiteDataEncryptAlg-2
		{588,16777217},
		// wmanIfCmnCryptoSuiteDataAuthentAlg-2
		{589,16777217},
		// wmanIfCmnCryptoSuiteTekEncryptAlg-2
		{590,16777217},
		// Content Version
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
};

static CFG_OFFSETTABLE CFG_MIB_16[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{592,33554434},
		/* MIBFlag */
		{3,16777217},
		// BcmConfigNetSearchPeriodicWakeup
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// BcmConfigUartFunctionality
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		/* BPFSReserved */
		{4,67109376},
		// wmanif2MSST44Timer
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		/* wmanIfSsLostDLMapInterval */
		{516,33554434},
		/* wmanIfSsLostULMapInterval */
		{518,33554434},
		/* wmanIfSsContentionRangRetries */
		{520,33554434},
		/* wmanIfSsRequestRetries */
		{522,33554434},
		/* wmanIfSsRegRequestRetries */
		{524,33554434},
		/* wmanIfSsT1Timeout */
		{526,33554434},
		/* wmanIfSsT2Timeout */
		{528,33554434},
		/* wmanIfSsT6Timeout */
		{530,33554434},
		/* wmanIfSsT12Timeout */
		{532,33554434},
		/* wmanIfSsT18Timeout */
		{534,33554434},
		/* wmanIfSsT20Timeout */
		{536,33554434},
		/* wmanIfSsT21Timeout */
		{538,33554434},
		/* wmanIfSsT14Timeout */
		{540,16777217},
		/* wmanIfSsT3Timeout */
		{541,16777217},
		/* wmanIfSsT4Timeout */
		{542,16777217},
		/* wmanIfSsSBCRequestRetries */
		{543,16777217},
		/* wmanIfCmnInvitedRangRetries */
		{544,33554434},
		/* wmanIfCmnT7Timeout */
		{546,33554434},
		/* wmanIfCmnT8Timeout */
		{548,33554434},
		/* wmanIfCmnT10Timeout */
		{550,33554434},
		/* wmanIfCmnT22Timeout */
		{552,33554434},
		/* u16Reserved1 */
		{554,33554434},
		/* wmanIfCmnDSxReqRetries */
		{556,67108868},
		/* wmanIfCmnDSxRespRetries */
		{560,67108868},
		/* wmanMssSpFrequency */
		{564,67108868},
		/* wmanMssNap */
		{568,67108868},
		/* wmanMssChanBandwidth */
		{572,67108868},
		/* wmanIfCmnCryptoSuiteIndex-1 */
		{576,67108868},
		/* wmanIfCmnCryptoSuiteDataEncryptAlg-1 */
		{580,16777217},
		/* wmanIfCmnCryptoSuiteDataAuthentAlg-1 */
		{581,16777217},
		/* wmanIfCmnCryptoSuiteTekEncryptAlg-1 */
		{582,16777217},
		/* u8Reserved-1 */
		{583,16777217},
		/* wmanIfCmnCryptoSuiteIndex-2 */
		{584,67108868},
		/* wmanIfCmnCryptoSuiteDataEncryptAlg-2 */
		{588,16777217},
		/* wmanIfCmnCryptoSuiteDataAuthentAlg-2 */
		{589,16777217},
		/* wmanIfCmnCryptoSuiteTekEncryptAlg-2 */
		{590,16777217},
		// Content Version
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
};

static CFG_OFFSETTABLE CFG_SECU_17[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{7376,33554434},
		// Certificates Encryption Status Flag
		{3,16777217},
		// Beceem Encryption Flag
		{4,16777217},
		// Firmware Decryption Key
		{5,16777416},
		// Certificate-1 Data Length
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// Certificate-1 Data
		{205,16779776},
		// Private Key Data-1 Length
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// Private Key Data-1
		{2765,16778240},
		// Certificate-2 Data Length
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// Certificate-2 Data
		{3789,16779776},
		// Private Key Data-2 Length
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// Private Key Data-2
		{6349,16778240},
		// Content Version
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Padding
		{7373,16777219},
};

static CFG_OFFSETTABLE CFG_SECU_18[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{7376,33554434},
		// Certificates Encryption Status Flag
		{3,16777217},
		// Beceem Encryption Flag
		{4,16777217},
		// Firmware Decryption Key
		{5,16777416},
		// Certificate-1 Data Length
		{205,33554434},
		// Certificate-1 Data
		{207,16779264},
		// Private Key Data-1 Length
		{2255,33554434},
		// Private Key Data-1
		{2257,16778748},
		// Certificate-2 Data Length
		{3789,33554434},
		// Certificate-2 Data
		{3791,16779264},
		// Private Key Data-2 Length
		{5839,33554434},
		// Private Key Data-2
		{5841,16778748},
		/* Content Version */
		{7373,16777217},
		/* Padding */
		{7374,16777218},
};

static CFG_OFFSETTABLE CFG_OMA_17[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{16006,33554434},
		/* Device Disable Flag */
		{3,16777217},
		/* OMA Data */
		{4,16793218},
};

static CFG_OFFSETTABLE CFG_MRU_1[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{198,33554434},
		// Reserved
		{3,16777217},
		// Head Index
		{4,16777217},
		// Tail Index
		{5,16777217},
		// Reserved1
		{6,16777218},
		// MRUData
		{8,16777406},
};

static CFG_OFFSETTABLE CFG_BCM_1[] = {
		// Length
		{0,33554434},
		// Layout Version
		{2,16777217},
		// Checksum
		{2052,33554434},
		// Content Version
		{3,16777217},
		// Reserved
		{4,16779264},
};

static CFG_OFFSETTABLE CFG_VSA_16[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{5878,33554434},
		/* VendorSpecificData */
		{3,16783091},
};





/* **************************************** RF ************************************ */


static CFG_OFFSETTABLE CFG_RF_18[] = {
		// Length
		{0,33554434},
		// Layout Version
		{2,16777217},
		// Checksum
		{2164,33554434},
		// Content Version
		{3,16777217},
		// RFIC ID
		{4,33554434},
		// PA ID
		{6,33554434},
		// FEM ID
		{8,33554434},
		// LNA ID
		{10,33554434},
		// RFIP Interface Mux
		{12,16777217},
		// Reference Design Assembly
		{13,16777232},
		// Voltage/Temp Curve
		{29,33554464},
		// BBIC Backoff at calibration
		{61,33554434},
		// DAC current setting at calibration
		{63,16777217},
		// Crystal PPM Temp Limits
		{64,16777223},
		// ADC Calibration Voltage
		{71,16777217},
		// Crystal PPM Offset
		{72,33554434},
		// XO Clock Frequency
		{74,67108868},
		// ANT1AB Config
		{78,33554434},
		// PA1_PA2_SW Config
		{80,33554434},
		// Antenna Port Config
		{82,16777217},
		// Temp Table Specifications-7
		{83,33554434},
		// Temp Table Specifications-16
		{85,33554434},
		// RF register config overwrite
		{87,16777276},
		// RF General Reserved Field-1
		{147,16777289},
		// Band 0 Polarity specifications
		{220,33554434},
		// Band 0 Center Frequencies
		{222,67108944},
		// Band 0 Calibration Frequencies Enabled
		{302,33554434},
		// Band 0 VCO_SEL Frequency 
		{304,67108868},
		// Band 0 RX LC trim
		{308,16777226},
		// Band 0 RX Gain Region Count
		{318,16777217},
		// Band 0 RX Gain Region State Table
		{319,33554444},
		// Band 0 RX PGA Gain Index 0
		{331,33554434},
		// Band 0 RX PGA Gain Steps
		{333,16777244},
		// Band 0 RX1 Front End Mid Step over Freq
		{361,16777236},
		// Band 0 RX2 Front End Mid Step over Freq
		{381,16777236},
		// Band 0 RX Front End Mid Step Temp Corr
		{401,16777223},
		// Band 0 RX1 Front End High Step over Freq
		{408,16777236},
		// Band 0 RX2 Front End High Step over Freq
		{428,16777236},
		// Band 0 RX Front End High Step Temp Corr
		{448,16777223},
		// Band 0 RX1 Gain Corr State
		{455,33554434},
		// Band 0 RX2 Gain Corr State
		{457,33554434},
		// Band 0 RX1 Gain Corr over Freq
		{459,33554472},
		// Band 0 RX2 Gain Corr over Freq
		{499,33554472},
		// Band 0 RX Gain Temp Corr
		{539,16777232},
		// Band 0 RX Gain Temp Corr Freq Scaling Factor
		{555,33554434},
		// Band 0 RX1 Ext LNA Gain over Freq
		{557,16777236},
		// Band 0 RX2 Ext LNA Gain over Freq
		{577,16777236},
		// Band 0 RX Ext LNA Temp Corr 
		{597,16777232},
		// Band 0 Spur Amplitudes
		{613,16777224},
		// Band 0 Rx2 Spur Amplitudes
		{621,16777224},
		// Band 0 DDR Spur Amplitudes
		{629,16777218},
		// Band 0 Rx2 DDR Spur Amplitudes
		{631,16777218},
		// Band 0 IQ Phase Temperature Slope
		{633,33554436},
		// Band 0 Thermal Noise Amplitude
		{637,16777218},
		// Band 0 Phase Noise Amplitude
		{639,16777218},
		// Band 0 Tx Maximum Allowed Power
		{641,16777218},
		// Band 0 Tx 64 QAM additional backoff
		{643,16777217},
		// Band 0 Max TX Power Lower Freq Threshold
		{644,33554434},
		// Band 0 Lower Freq Delta Max Tx Pwr 
		{646,16777217},
		// Band 0 TX Operational Mode
		{647,16777217},
		// Band 0 TX FB Select
		{648,16777217},
		// Band 0 Ext RF Filter Delay Curve
		{649,16777231},
		// Band 0 TX PA Bias Ref Enable
		{664,16777217},
		// Band 0 TX PA Bias Table
		{665,16777232},
		// Band 0 TX PA Drv Bias
		{681,16777224},
		// Band 0 TX1 PA Drv Amps
		{689,33554476},
		// Band 0 TX2 PA Drv Amps
		{733,33554476},
		// Band 0 TX1 Mixer Amps
		{777,33554462},
		// Band 0 TX2 Mixer Amps
		{807,33554462},
		// Band 0 TX Backoff Pouts
		{837,16777228},
		// Band 0 Optimal Feedback Cal Attn State over Freq
		{849,16777226},
		// Band 0 TX Nominal ExtAttn Step
		{859,16777217},
		// Band 0 TX Backoff for ExtAttn
		{860,16777217},
		// Band 0 TX1 ExtAttn Step Error
		{861,16777236},
		// Band 0 TX2 ExtAttn Step Error
		{881,16777236},
		// Band 0 TX Ext Attn Temp Corr
		{901,16777223},
		// Band 0 TX1 Pout Corr over Freq
		{908,16777236},
		// Band 0 TX2 Pout Corr over Freq
		{928,16777236},
		// Band 0 Nominal Loop Filter Parameters
		{948,33554442},
		// Band 0 TX1 Open Close Backoff Value
		{958,16777217},
		// Band 0 TX1 PA Gain Error Table
		{959,16777248},
		// Band 0 TX1 RF Asymmetry
		{991,16777236},
		// Band 0 TX Phase vs Pout
		{1011,16777232},
		// Band 0 TX Phase vs Freq
		{1027,16777232},
		// Band 0 TX Temp Corr
		{1043,16777232},
		// Band 0 TX Temp Corr Freq Scaling Factor
		{1059,33554434},
		// RF General Reserved Field-2
		{1061,16777346},
		// Band 1 Polarity specifications
		{1191,33554434},
		// Band 1 Center Frequencies
		{1193,67108944},
		// Band 1 Calibration Frequencies Enabled
		{1273,33554434},
		// Band 1 VCO_SEL Frequency 
		{1275,67108868},
		// Band 1 RX LC trim
		{1279,16777226},
		// Band 1 RX Gain Region Count
		{1289,16777217},
		// Band 1 RX Gain Region State Table
		{1290,33554444},
		// Band 1 RX PGA Gain Index 0
		{1302,33554434},
		// Band 1 RX PGA Gain Steps
		{1304,16777244},
		// Band 1 RX1 Front End Mid Step over Freq
		{1332,16777236},
		// Band 1 RX2 Front End Mid Step over Freq
		{1352,16777236},
		// Band 1 RX Front End Mid Step Temp Corr
		{1372,16777223},
		// Band 1 RX1 Front End High Step over Freq
		{1379,16777236},
		// Band 1 RX2 Front End High Step over Freq
		{1399,16777236},
		// Band 1 RX Front End High Step Temp Corr
		{1419,16777223},
		// Band 1 RX1 Gain Corr State
		{1426,33554434},
		// Band 1 RX2 Gain Corr State
		{1428,33554434},
		// Band 1 RX1 Gain Corr over Freq
		{1430,33554472},
		// Band 1 RX2 Gain Corr over Freq
		{1470,33554472},
		// Band 1 RX Gain Temp Corr
		{1510,16777232},
		// Band 1 RX Gain Temp Corr Freq Scaling Factor
		{1526,33554434},
		// Band 1 RX1 Ext LNA Gain over Freq
		{1528,16777236},
		// Band 1 RX2 Ext LNA Gain over Freq
		{1548,16777236},
		// Band 1 RX Ext LNA Temp Corr 
		{1568,16777232},
		// Band 1 Spur Amplitudes
		{1584,16777224},
		// Band 1 Rx2 Spur Amplitudes
		{1592,16777224},
		// Band 1 DDR Spur Amplitudes
		{1600,16777218},
		// Band 1 Rx2 DDR Spur Amplitudes
		{1602,16777218},
		// Band 1 IQ Phase Temperature Slope
		{1604,33554436},
		// Band 1 Thermal Noise Amplitude
		{1608,16777218},
		// Band 1 Phase Noise Amplitude
		{1610,16777218},
		// Band 1 Tx Maximum Allowed Power
		{1612,16777218},
		// Band 1 Tx 64 QAM additional backoff
		{1614,16777217},
		// Band 1 Max TX Power Lower Freq Threshold
		{1615,33554434},
		// Band 1 Lower Freq Delta Max Tx Pwr 
		{1617,16777217},
		// Band 1 TX Operational Mode
		{1618,16777217},
		// Band 1 TX FB Select
		{1619,16777217},
		// Band 1 Ext RF Filter Delay Curve
		{1620,16777231},
		// Band 1 TX PA Bias Ref Enable
		{1635,16777217},
		// Band 1 TX PA Bias Table
		{1636,16777232},
		// Band 1 TX PA Drv Bias
		{1652,16777224},
		// Band 1 TX1 PA Drv Amps
		{1660,33554476},
		// Band 1 TX2 PA Drv Amps
		{1704,33554476},
		// Band 1 TX1 Mixer Amps
		{1748,33554462},
		// Band 1 TX2 Mixer Amps
		{1778,33554462},
		// Band 1 TX Backoff Pouts
		{1808,16777228},
		// Band 1 Optimal Feedback Cal Attn State over Freq
		{1820,16777226},
		// Band 1 TX Nominal ExtAttn Step
		{1830,16777217},
		// Band 1 TX Backoff for ExtAttn
		{1831,16777217},
		// Band 1 TX1 ExtAttn Step Error
		{1832,16777236},
		// Band 1 TX2 ExtAttn Step Error
		{1852,16777236},
		// Band 1 TX Ext Attn Temp Corr
		{1872,16777223},
		// Band 1 TX1 Pout Corr over Freq
		{1879,16777236},
		// Band 1 TX2 Pout Corr over Freq
		{1899,16777236},
		// Band 1 Nominal Loop Filter Parameters
		{1919,33554442},
		// Band 1 TX1 Open Close Backoff Value
		{1929,16777217},
		// Band 1 TX1 PA Gain Error Table
		{1930,16777248},
		// Band 1 TX1 RF Asymmetry
		{1962,16777236},
		// Band 1 TX Phase vs Pout
		{1982,16777232},
		// Band 1 TX Phase vs Freq
		{1998,16777232},
		// Band 1 TX Temp Corr
		{2014,16777232},
		// Band 1 TX Temp Corr Freq Scaling Factor
		{2030,33554434},
		// RF General Reserved Field-3
		{2032,16777346},
		// Padding
		{2162,16777218},
};

static CFG_OFFSETTABLE CFG_RF_17[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{1728,33554434},
		// RFIC ID
		{3,33554434},
		// Reference Design Assembly
		{5,16777232},
		// Voltage/Temp Curve
		{21,33554464},
		// BBIC Backoff at calibration
		{53,33554434},
		// DAC current setting at calibration
		{55,16777217},
		// Crystal PPM Temp Limits
		{56,16777223},
		// ADC Calibration Voltage
		{63,16777217},
		// Crystal PPM Offset
		{64,33554434},
		// ANT1AB Config
		{66,33554434},
		// PA1_PA2_SW Config
		{68,33554434},
		// Antenna Port Config
		{70,16777217},
		// Temp Table Specifications-7
		{71,33554434},
		// Temp Table Specifications-16
		{73,33554434},
		// RF General Reserved Field-1
		{75,16777283},
		// Band 0 Center Frequencies
		{142,67108912},
		// Band 0 Calibration Frequencies Enabled
		{190,33554434},
		// Band 0 Tx LO FT Trim
		{192,16777222},
		// Band 0 LC trim
		{198,33554456},
		// Band 0 RX Gain Region Count
		{222,16777217},
		// Band 0 RX Gain Region State Table
		{223,33554444},
		// Band 0 RX PGA Gain Index 0
		{235,33554434},
		// Band 0 RX PGA Gain Steps
		{237,16777244},
		// Band 0 RX1 LNA Mid Gain over Freq
		{265,16777228},
		// Band 0 RX2 LNA Mid Gain over Freq
		{277,16777228},
		// Band 0 RX LNA Mid Gain Temp Corr
		{289,16777223},
		// Band 0 RX1 LNA High Gain over Freq
		{296,16777228},
		// Band 0 RX2 LNA High Gain over Freq
		{308,16777228},
		// Band 0 RX LNA High Gain Temp Corr
		{320,16777223},
		// Band 0 RX1 Gain Corr State
		{327,33554434},
		// Band 0 RX1 Gain Corr over Freq
		{329,33554456},
		// Band 0 RX1 Gain Temp Corr
		{353,16777232},
		// Band 0 RX1 Ext LNA Gain over Freq
		{369,16777228},
		// Band 0 RX1 Ext LNA Temp Corr 
		{381,16777232},
		// Band 0 RX2 Gain Corr State
		{397,33554434},
		// Band 0 RX2 Gain Corr over Freq
		{399,33554456},
		// Band 0 RX2 Gain Temp Corr
		{423,16777232},
		// Band 0 RX2 Ext LNA Gain over Freq
		{439,16777228},
		// Band 0 RX2 Ext LNA Temp Corr 
		{451,16777232},
		// Band 0 Tx Maximum Allowed Power
		{467,16777217},
		// Band 0 TX1 PGA Attn Index 0
		{468,33554434},
		// Band 0 TX1 PGA Attn Steps
		{470,16777227},
		// Band 0 TX1 PA Drv Index 0
		{481,33554434},
		// Band 0 TX1 PA Drv Attn Steps
		{483,16777236},
		// Band 0 TX PA Drv Index 21 Step
		{503,33554434},
		// Band 0 TX1 TPC Region Count
		{505,16777217},
		// Band 0 TX1 Region Control Variable
		{506,16777217},
		// Band 0 TX1 TPC Region State Table
		{507,33554444},
		// Band 0 TX1 Pout Corr over Freq
		{519,33554456},
		// Band 0 TX1 Temp Corr
		{543,16777232},
		// Band 0 TX1 Ext Attn Step over Freq
		{559,16777228},
		// Band 0 TX1 Ext Attn Temp Corr
		{571,16777223},
		// Band 0 TX power detect
		{578,16777228},
		// Band 0 Spur Amplitudes
		{590,16777224},
		// Band 0 IQ Phase Temperature Slope
		{598,33554436},
		// Band 0 Thermal Noise Amplitude
		{602,16777218},
		// Band 0 Phase Noise Amplitude
		{604,16777218},
		// Band 0 DDR Spur Amplitudes
		{606,16777218},
		// Band 0 TX2 Pout Corr over Freq
		{608,33554456},
		// Band 0 Tx 64 QAM additional backoff
		{632,16777217},
		// Band 0 RX Gain Temp Corr Freq Scaling Factor
		{633,33554434},
		// Band 0 TX Temp Corr Freq Scaling Factor
		{635,33554434},
		// RF General Reserved Field-2
		{637,16777249},
		// Band 1 Center Frequencies
		{670,67108912},
		// Band 1 Calibration Frequencies Enabled
		{718,33554434},
		// Band 1 Tx LO FT Trim
		{720,16777222},
		// Band 1 LC trim
		{726,33554456},
		// Band 1 RX Gain Region Count
		{750,16777217},
		// Band 1 RX Gain Region State Table
		{751,33554444},
		// Band 1 RX PGA Gain Index 0
		{763,33554434},
		// Band 1 RX PGA Gain Steps
		{765,16777244},
		// Band 1 RX1 LNA Mid Gain over Freq
		{793,16777228},
		// Band 1 RX2 LNA Mid Gain over Freq
		{805,16777228},
		// Band 1 RX LNA Mid Gain Temp Corr
		{817,16777223},
		// Band 1 RX1 LNA High Gain over Freq
		{824,16777228},
		// Band 1 RX2 LNA High Gain over Freq
		{836,16777228},
		// Band 1 RX LNA High Gain Temp Corr
		{848,16777223},
		// Band 1 RX1 Gain Corr State
		{855,33554434},
		// Band 1 RX1 Gain Corr over Freq
		{857,33554456},
		// Band 1 RX1 Gain Temp Corr
		{881,16777232},
		// Band 1 RX1 Ext LNA Gain over Freq
		{897,16777228},
		// Band 1 RX1 Ext LNA Temp Corr 
		{909,16777232},
		// Band 1 RX2 Gain Corr State
		{925,33554434},
		// Band 1 RX2 Gain Corr over Freq
		{927,33554456},
		// Band 1 RX2 Gain Temp Corr
		{951,16777232},
		// Band 1 RX2 Ext LNA Gain over Freq
		{967,16777228},
		// Band 1 RX2 Ext LNA Temp Corr 
		{979,16777232},
		// Band 1 Tx Maximum Allowed Power
		{995,16777217},
		// Band 1 TX1 PGA Attn Index 0
		{996,33554434},
		// Band 1 TX1 PGA Attn Steps
		{998,16777227},
		// Band 1 TX1 PA Drv Index 0
		{1009,33554434},
		// Band 1 TX1 PA Drv Attn Steps
		{1011,16777236},
		// Band 1 TX PA Drv Index 21 Step
		{1031,33554434},
		// Band 1 TX1 TPC Region Count
		{1033,16777217},
		// Band 1 TX1 Region Control Variable
		{1034,16777217},
		// Band 1 TX1 TPC Region State Table
		{1035,33554444},
		// Band 1 TX1 Pout Corr over Freq
		{1047,33554456},
		// Band 1 TX1 Temp Corr
		{1071,16777232},
		// Band 1 TX1 Ext Attn Step over Freq
		{1087,16777228},
		// Band 1 TX1 Ext Attn Temp Corr
		{1099,16777223},
		// Band 1 TX power detect
		{1106,16777228},
		// Band 1 Spur Amplitudes
		{1118,16777224},
		// Band 1 IQ Phase Temperature Slope
		{1126,33554436},
		// Band 1 Thermal Noise Amplitude
		{1130,16777218},
		// Band 1 Phase Noise Amplitude
		{1132,16777218},
		// Band 1 DDR Spur Amplitudes
		{1134,16777218},
		// Band 1 TX2 Pout Corr over Freq
		{1136,33554456},
		// Band 1 Tx 64 QAM additional backoff
		{1160,16777217},
		// Band 1 RX Gain Temp Corr Freq Scaling Factor
		{1161,33554434},
		// Band 1 TX Temp Corr Freq Scaling Factor
		{1163,33554434},
		// RF General Reserved Field-3
		{1165,16777249},
		// Band 2 Center Frequencies
		{1198,67108912},
		// Band 2 Calibration Frequencies Enabled
		{1246,33554434},
		// Band 2 Tx LO FT Trim
		{1248,16777222},
		// Band 2 LC trim
		{1254,33554456},
		// Band 2 RX Gain Region Count
		{1278,16777217},
		// Band 2 RX Gain Region State Table
		{1279,33554444},
		// Band 2 RX PGA Gain Index 0
		{1291,33554434},
		// Band 2 RX PGA Gain Steps
		{1293,16777244},
		// Band 2 RX1 LNA Mid Gain over Freq
		{1321,16777228},
		// Band 2 RX2 LNA Mid Gain over Freq
		{1333,16777228},
		// Band 2 RX LNA Mid Gain Temp Corr
		{1345,16777223},
		// Band 2 RX1 LNA High Gain over Freq
		{1352,16777228},
		// Band 2 RX2 LNA High Gain over Freq
		{1364,16777228},
		// Band 2 RX LNA High Gain Temp Corr
		{1376,16777223},
		// Band 2 RX1 Gain Corr State
		{1383,33554434},
		// Band 2 RX1 Gain Corr over Freq
		{1385,33554456},
		// Band 2 RX1 Gain Temp Corr
		{1409,16777232},
		// Band 2 RX1 Ext LNA Gain over Freq
		{1425,16777228},
		// Band 2 RX1 Ext LNA Temp Corr 
		{1437,16777232},
		// Band 2 RX2 Gain Corr State
		{1453,33554434},
		// Band 2 RX2 Gain Corr over Freq
		{1455,33554456},
		// Band 2 RX2 Gain Temp Corr
		{1479,16777232},
		// Band 2 RX2 Ext LNA Gain over Freq
		{1495,16777228},
		// Band 2 RX2 Ext LNA Temp Corr 
		{1507,16777232},
		// Band 2 Tx Maximum Allowed Power
		{1523,16777217},
		// Band 2 TX1 PGA Attn Index 0
		{1524,33554434},
		// Band 2 TX1 PGA Attn Steps
		{1526,16777227},
		// Band 2 TX1 PA Drv Index 0
		{1537,33554434},
		// Band 2 TX1 PA Drv Attn Steps
		{1539,16777236},
		// Band 2 TX PA Drv Index 21 Step
		{1559,33554434},
		// Band 2 TX1 TPC Region Count
		{1561,16777217},
		// Band 2 TX1 Region Control Variable
		{1562,16777217},
		// Band 2 TX1 TPC Region State Table
		{1563,33554444},
		// Band 2 TX1 Pout Corr over Freq
		{1575,33554456},
		// Band 2 TX1 Temp Corr
		{1599,16777232},
		// Band 2 TX1 Ext Attn Step over Freq
		{1615,16777228},
		// Band 2 TX1 Ext Attn Temp Corr
		{1627,16777223},
		// Band 2 TX power detect
		{1634,16777228},
		// Band 2 Spur Amplitudes
		{1646,16777224},
		// Band 2 IQ Phase Temperature Slope
		{1654,33554436},
		// Band 2 Thermal Noise Amplitude
		{1658,16777218},
		// Band 2 Phase Noise Amplitude
		{1660,16777218},
		// Band 2 DDR Spur Amplitudes
		{1662,16777218},
		// Band 2 TX2 Pout Corr over Freq
		{1664,33554456},
		// Band 2 Tx 64 QAM additional backoff
		{1688,16777217},
		// Band 2 RX Gain Temp Corr Freq Scaling Factor
		{1689,33554434},
		// Band 2 TX Temp Corr Freq Scaling Factor
		{1691,33554434},
		// RF General Reserved Field-4
		{1693,16777249},
		// Content Version
		{1726,16777217},		
		// Band 0 RX Mixer Step over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 RX Mixer Step over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 RX Mixer Step over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 RX Mixer Step Temp Corr
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 RX Mixer Step Temp Corr
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 RX Mixer Step Temp Corr
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},

		// Padding
		//{1726,16777218},
};

static CFG_OFFSETTABLE CFG_RF_16[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{1392,33554434},
		// RFIC ID
		{3,33554434},
		// Reference Design Assembly
		{5,16777232},
		// Voltage/Temp Curve
		{21,33554464},
		// BBIC Backoff at calibration
		{53,33554434},
		// DAC current setting at calibration
		{55,16777217},
		// Crystal PPM Temp Limits
		{56,16777223},
		// Temperature Sensor Offset
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Crystal PPM Offset
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// ANT1AB Config
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// PA1_PA2_SW Config
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Antenna Port Config
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// 7-Temp Table Specifications
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// 16-Temp Table Specifications
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// RF General Reserved Field-1
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 Center Frequencies
		{63,67108912},
		// Band 0 Calibration Frequencies Enabled
		{111,33554434},
		// Band 0 Tx LO FT Trim
		{113,16777222},
		// Band 0 LC trim
		{119,33554456},
		// Band 0 RX Gain Region Count
		{143,16777217},
		// Band 0 RX Gain Region State Table
		{144,33554444},
		// Band 0 RX PGA Gain Index 0
		{156,33554434},
		// Band 0 RX PGA Gain Steps
		{158,16777244},
		// Band 0 RX LNA Mid Gain over Freq
		{186,16777228},
		// Band 0 RX2 LNA Mid Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 RX LNA Mid Gain Temp Corr
		{198,16777223},
		// Band 0 RX LNA High Gain over Freq
		{205,16777228},
		// Band 0 RX2 LNA High Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 RX LNA High Gain Temp Corr
		{217,16777223},
		// Band 0 RX1 Gain Corr State
		{243,33554434},
		// Band 0 RX1 Gain Corr over Freq
		{245,33554456},
		// Band 0 RX1 Gain Temp Corr
		{269,16777232},
		// Band 0 RX1 Ext LNA Gain over Freq
		{285,16777228},
		// Band 0 RX1 Ext LNA Temp Corr
		{297,16777232},
		// Band 0 RX2 Gain Corr State
		{313,33554434},
		// Band 0 RX2 Gain Corr over Freq
		{315,33554456},
		// Band 0 RX2 Gain Temp Corr
		{339,16777232},
		// Band 0 RX2 Ext LNA Gain over Freq
		{355,16777228},
		// Band 0 RX2 Ext LNA Temp Corr
		{367,16777232},
		// Band 0 Tx Maximum Allowed Power
		{383,16777217},
		// Band 0 TX1 PGA Attn Index 0
		{384,33554434},
		// Band 0 TX1 PGA Attn Steps
		{386,16777227},
		// Band 0 TX1 PA Drv Index 0
		{397,33554434},
		// Band 0 TX1 PA Drv Attn Steps
		{399,16777236},
		// Band 0 TX PA Drv Index 21 Step
		{419,33554434},
		// Band 0 TX1 TPC Region Count
		{421,16777217},
		// Band 0 TX1 Region Control Variable
		{422,16777217},
		// Band 0 TX1 TPC Region State Table
		{423,33554444},
		// Band 0 TX1 Pout Corr over Freq
		{435,33554456},
		// Band 0 TX1 Temp Corr
		{459,16777232},
		// Band 0 TX1 Ext Attn Step over Freq
		{475,16777228},
		// Band 0 TX1 Ext Attn Temp Corr
		{487,16777223},
		// Band 0 TX power detect
		{494,16777228},
		// Band 0 Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 IQ Phase Temperature Slope
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 Thermal Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 Phase Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 DDR Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 TX2 Pout Corr over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 Tx 64 QAM additional backoff
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 RX Gain Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 0 TX Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
        // RF General Reserved Field-2
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 Center Frequencies
		{506,67108912},
		// Band 1 Calibration Frequencies Enabled
		{554,33554434},
		// Band 1 Tx LO FT Trim
		{556,16777222},
		// Band 1 LC trim
		{562,33554456},
		// Band 1 RX Gain Region Count
		{586,16777217},
		// Band 1 RX Gain Region State Table
		{587,33554444},
		// Band 1 RX PGA Gain Index 0
		{599,33554434},
		// Band 1 RX PGA Gain Steps
		{601,16777244},
		// Band 1 RX LNA Mid Gain over Freq
		{629,16777228},
		// Band 1 RX2 LNA Mid Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 RX LNA Mid Gain Temp Corr
		{641,16777223},
		// Band 1 RX LNA High Gain over Freq
		{648,16777228},
		// Band 1 RX2 LNA High Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 RX LNA High Gain Temp Corr
		{660,16777223},
		// Band 1 RX1 Gain Corr State
		{686,33554434},
		// Band 1 RX1 Gain Corr over Freq
		{688,33554456},
		// Band 1 RX1 Gain Temp Corr
		{712,16777232},
		// Band 1 RX1 Ext LNA Gain over Freq
		{728,16777228},
		// Band 1 RX1 Ext LNA Temp Corr
		{740,16777232},
		// Band 1 RX2 Gain Corr State
		{756,33554434},
		// Band 1 RX2 Gain Corr over Freq
		{758,33554456},
		// Band 1 RX2 Gain Temp Corr
		{782,16777232},
		// Band 1 RX2 Ext LNA Gain over Freq
		{798,16777228},
		// Band 1 RX2 Ext LNA Temp Corr
		{810,16777232},
		// Band 1 Tx Maximum Allowed Power
		{826,16777217},
		// Band 1 TX1 PGA Attn Index 0
		{827,33554434},
		// Band 1 TX1 PGA Attn Steps
		{829,16777227},
		// Band 1 TX1 PA Drv Index 0
		{840,33554434},
		// Band 1 TX1 PA Drv Attn Steps
		{842,16777236},
		// Band 1 TX PA Drv Index 21 Step
		{862,33554434},
		// Band 1 TX1 TPC Region Count
		{864,16777217},
		// Band 1 TX1 Region Control Variable
		{865,16777217},
		// Band 1 TX1 TPC Region State Table
		{866,33554444},
		// Band 1 TX1 Pout Corr over Freq
		{878,33554456},
		// Band 1 TX1 Temp Corr
		{902,16777232},
		// Band 1 TX1 Ext Attn Step over Freq
		{918,16777228},
		// Band 1 TX1 Ext Attn Temp Corr
		{930,16777223},
		// Band 1 TX power detect
		{937,16777228},
		// Band 1 Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 IQ Phase Temperature Slope
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 Thermal Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 Phase Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 DDR Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 TX2 Pout Corr over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 Tx 64 QAM additional backoff
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 RX Gain Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 1 TX Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
        // RF General Reserved Field-3
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 Center Frequencies
		{949,67108912},
		// Band 2 Calibration Frequencies Enabled
		{997,33554434},
		// Band 2 Tx LO FT Trim
		{999,16777222},
		// Band 2 LC trim
		{1005,33554456},
		// Band 2 RX Gain Region Count
		{1029,16777217},
		// Band 2 RX Gain Region State Table
		{1030,33554444},
		// Band 2 RX PGA Gain Index 0
		{1042,33554434},
		// Band 2 RX PGA Gain Steps
		{1044,16777244},
		// Band 2 RX LNA Mid Gain over Freq
		{1072,16777228},
		// Band 2 RX2 LNA Mid Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 RX LNA Mid Gain Temp Corr
		{1084,16777223},
		// Band 2 RX LNA High Gain over Freq
		{1091,16777228},
		// Band 2 RX2 LNA High Gain over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 RX LNA High Gain Temp Corr
		{1103,16777223},
		// Band 2 RX1 Gain Corr State
		{1129,33554434},
		// Band 2 RX1 Gain Corr over Freq
		{1131,33554456},
		// Band 2 RX1 Gain Temp Corr
		{1155,16777232},
		// Band 2 RX1 Ext LNA Gain over Freq
		{1171,16777228},
		// Band 2 RX1 Ext LNA Temp Corr
		{1183,16777232},
		// Band 2 RX2 Gain Corr State
		{1199,33554434},
		// Band 2 RX2 Gain Corr over Freq
		{1201,33554456},
		// Band 2 RX2 Gain Temp Corr
		{1225,16777232},
		// Band 2 RX2 Ext LNA Gain over Freq
		{1241,16777228},
		// Band 2 RX2 Ext LNA Temp Corr
		{1253,16777232},
		// Band 2 Tx Maximum Allowed Power
		{1269,16777217},
		// Band 2 TX1 PGA Attn Index 0
		{1270,33554434},
		// Band 2 TX1 PGA Attn Steps
		{1272,16777227},
		// Band 2 TX1 PA Drv Index 0
		{1283,33554434},
		// Band 2 TX1 PA Drv Attn Steps
		{1285,16777236},
		// Band 2 TX PA Drv Index 21 Step
		{1305,33554434},
		// Band 2 TX1 TPC Region Count
		{1307,16777217},
		// Band 2 TX1 Region Control Variable
		{1308,16777217},
		// Band 2 TX1 TPC Region State Table
		{1309,33554444},
		// Band 2 TX1 Pout Corr over Freq
		{1321,33554456},
		// Band 2 TX1 Temp Corr
		{1345,16777232},
		// Band 2 TX1 Ext Attn Step over Freq
		{1361,16777228},
		// Band 2 TX1 Ext Attn Temp Corr
		{1373,16777223},
		// Band 2 TX power detect
		{1380,16777228},
		// Band 2 Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 IQ Phase Temperature Slope
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 Thermal Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 Phase Noise Amplitude
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 DDR Spur Amplitudes
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 TX2 Pout Corr over Freq
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 Tx 64 QAM additional backoff
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 RX Gain Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Band 2 TX Temp Corr Freq Scaling Factor
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},		
		// RF General Reserved Field-4
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
		// Content Version
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},		
		// Band 0 RX Mixer Step over Freq
		{224,16777228},
		// Band 1 RX Mixer Step over Freq
		{667,16777228},
		// Band 2 RX Mixer Step over Freq
		{1110,16777228},
		// Band 0 RX Mixer Step Temp Corr
		{236,16777223},
		// Band 1 RX Mixer Step Temp Corr
		{679,16777223},
		// Band 2 RX Mixer Step Temp Corr
		{1122,16777223},
};

static CFG_OFFSETTABLE CFG_RF_02[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{472,33554434},
		// RFIC ID
		{3,16777218},
		// Reserved-1
		{5,16777217},
		// Rx1 PGA2 Shift-up Threshold
		{6,16777219},
		// Rx1 PGA2 Shift-up Adjust
		{9,16777219},
		// Rx1 PGA2 Shift-down Threshold
		{12,16777219},
		// Rx1 PGA2 Shift-down Adjust
		{15,16777219},
		// Rx1 Mixer Shift-up Threshold
		{18,16777217},
		// Rx1 Mixer Shift-up Adjust
		{19,16777217},
		// Rx1 Mixer Shift-down Threshold
		{20,16777217},
		// Rx1 Mixer Shift-down Adjust
		{21,16777217},
		// Rx1 LNA Shift-up Threshold
		{22,16777217},
		// Rx1 LNA Shift-up Adjust
		{23,16777217},
		// Rx1 LNA Shift-down Threshold
		{24,16777217},
		// Rx1 LNA Shift-down Adjust
		{25,16777217},
		// Rx1 Channel Correction
		{26,16777232},
		// Rx1 Temperature Correction
		{42,16777232},
		// Rx1 PGA1 Gain
		{58,33554496},
		// Rx1 PGA2 Gain
		{122,33554440},
		// Rx1 Mixer Gain
		{130,33554436},
		// Rx1 LNA Gain
		{134,33554436},
		// Rx2 PGA2 Shift-up Threshold
		{138,16777219},
		// Rx2 PGA2 Shift-up Adjust
		{141,16777219},
		// Rx2 PGA2 Shift-down Threshold
		{144,16777219},
		// Rx2 PGA2 Shift-down Adjust
		{147,16777219},
		// Rx2 Mixer Shift-up Threshold
		{150,16777217},
		// Rx2 Mixer Shift-up Adjust
		{151,16777217},
		// Rx2 Mixer Shift-down Threshold
		{152,16777217},
		// Rx2 Mixer Shift-down Adjust
		{153,16777217},
		// Rx2 LNA Shift-up Threshold
		{154,16777217},
		// Rx2 LNA Shift-up Adjust
		{155,16777217},
		// Rx2 LNA Shift-down Threshold
		{156,16777217},
		// Rx2 LNA Shift-down Adjust
		{157,16777217},
		// Rx2 Channel Correction
		{158,16777232},
		// Rx2 Temperature Correction
		{174,16777232},
		// Rx2 PGA1 Gain
		{190,33554496},
		// Rx2 PGA2 Gain
		{254,33554440},
		// Rx2 Mixer Gain
		{262,33554436},
		// Rx2 LNA Gain
		{266,33554436},
		// Tx Maximum Allowed Power
		{270,16777217},
		// Power Detector ADC Voltage for Maximum Power
		{271,16777217},
		// Tx Channel Correction
		{272,16777232},
		// Tx Temperature Correction
		{288,16777232},
		// Tx Power Output
		{304,33554598},
		// Tx RF Mixer IQ Trim
		{470,16777217},
		// Tx IF Mixer IQ Trim
		{471,16777217},
};

/* **************************************** PHY *********************************** */

static CFG_OFFSETTABLE CFG_PHY_16[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{90,33554434},
		// Reserved
		{3,16777217},
		// Frame Length
		{4,33554434},
		// Channel Bandwidth
		{6,67108868},
		// Channel Frequency Step
		{10,67108868},
		// DL Subcarrier Allocations
		{14,67108868},
		// UL Subcarrier Allocations
		{18,67108868},
		// UL Sounding 1 Options
		{22,67108868},
		// FFT Size
		{26,33554434},
		// Duplexing
		{28,16777217},
		// Sampling Factor
		{29,16777217},
		// Cyclic Prefix
		{30,16777217},
		// TTG
		{31,67108868},
		// RTG
		{35,67108868},
		// No. DL symbols
		{39,16777217},
		// No. UL symbols
		{40,16777217},
		// UL Sounding Response Time
		{41,16777217},
		// UL Sounding Max No. Simultaneous Instructions
		{42,16777217},
		// Initial Ranging Method
		{43,16777217},
		// Handoff Ranging Method
		{44,16777217},
		// Periodic Ranging Method
		{45,16777217},
		// BW Request Method
		{46,16777217},
		// Fast FB/CQI Encoding
		{47,16777217},
		// Fast FB/CQI Allocation
		{48,16777217},
		// FEC Codes
		{49,16777217},
		// HARQ Support
		{50,16777217},
		// Incremental Redundancy
		{51,16777217},
		// DL Modulation Rate
		{52,16777217},
		// UL Modulation Rate
		{53,16777217},
		// Map types supported
		{54,16777217},
		// Reserved
		{55,16777217},
		// Power Control
		{56,33554434},
		// CINR Measurement
		{58,67108868},
		// STC/MIMO DL PUSC
		{62,33554434},
		// STC/MIMO DL FUSC
		{64,33554434},
		// STC/MIMO DL O-FUSC
		{66,33554434},
		// STC/MIMO DL O-AMC
		{68,33554434},
		// STC/MIMO DL PUSC-ASCA
		{70,16777217},
		// STC/MIMO UL PUSC
		{71,16777217},
		// STC/MIMO UL O-PUSC
		{72,16777217},
		// STC/MIMO UL O-AMC
		{73,16777217},
		// Closed Loop MIMO
		{74,33554434},
		// MIMO Feedback
		{76,33554434},
		// MIMO Midamble
		{78,16777217},
		// MIMO Soft HO/Macro Diversity
		{79,16777217},
		// HARQ DL MIMO
		{80,16777217},
		// HARQ UL MIMO
		{81,16777217},
		// FBSS
		{82,16777217},
		// Soft HO
		{83,16777217},
		// UL Macro diversity
		{84,16777217},
		// Max Concurrent bursts
		{85,16777217},
		// Max bursts in frame
		{86,16777217},
		// Max Concurrent  Burst in DL Subframe with H-ARQ
		{87,16777217},
		// Max Concurrent  Burst in UL Subframe with H-ARQ
		{88,16777217},
		// Content Version
		{89,16777217},
};



/* **************************************** MAC *********************************** */

static CFG_OFFSETTABLE CFG_MAC_16[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{148,33554434},
		// CS Options
		{3,33554434},
		// DHCP
		{5,16777217},
		// IP Address
		{6,134217744},
		// IP subnet mask
		{22,134217744},
		// IP default gateway
		{38,134217744},
		// IP DNS server #1
		{54,134217744},
		// IP DNS server #2
		{70,134217744},
		// PHS
		{86,16777217},
		// Compression Options
		{87,16777217},
		// PDU formats
		{88,33554434},
		// Feedback mechanisms
		{90,33554434},
		// Multicast connection
		{92,16777217},
		// Network Entry
		{93,16777217},
		// ARQ
		{94,16777217},
		// HARQ support
		{95,16777217},
		// QOS
		{96,16777217},
		// Data delivery service for mobile
		{97,16777217},
		// Request-grant mechanism
		{98,33554434},
		// Neighbor advertisement
		{100,16777217},
		// Scanning
		{101,16777217},
		// Scan reporting type support
		{102,16777217},
		// Association
		{103,16777217},
		// Association type support
		{104,16777217},
		// HO/Scan/Report Trigger Metrics
		{105,16777217},
		// HO Optimization
		{106,67108868},
		// MAC layer HO procedures
		{110,33554434},
		// CID and SAID Update
		{112,33554434},
		// Fast BS Switching
		{114,67108868},
		// Macro diversity handover
		{118,67108868},
		// Sleep mode
		{122,67108868},
		// Idle mode
		{126,67108868},
		// Expedited Network Re-entry from Idle Mode
		{130,67108868},
		// PKM version
		{134,16777217},
		// PKMv2 Authorization Policy - Initial network entry
		{135,16777217},
		// PKMv2 Authorization Policy - network re-entry
		{136,16777217},
		// SA Service Type
		{137,16777217},
		// Supported cryptographic suites
		{138,33554434},
		// MBS
		{140,33554434},
		// Multicast Broadcast Re-keying Algorithm (MBRA)
		{142,16777217},
		// Paging retry count
		{143,16777217},
		// DCD Interval
		{144,33554434},
		// UCD Interval
		{146,33554434},
};




static CFG_OFFSETTABLE CFG_PHY_00[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{220,33554434},
		// Reserved-1
		{3,16777217},
		// Center Frequencies
		{4,134217856},
		// Channels Scanned
		{132,33554434},
		// Frame Length
		{134,33554434},
		// Channel Bandwidth
		{136,67108868},
		// Channel Frequency Step
		{140,67108868},
		// DL Subcarrier Allocations
		{144,67108868},
		// UL Subcarrier Allocations
		{148,67108868},
		// UL Sounding 1 Options
		{152,67108868},
		// FFT Size
		{156,33554434},
		// Duplexing
		{158,16777217},
		// Sampling Factor
		{159,16777217},
		// Cyclic Prefix
		{160,16777217},
		// TTG
		{161,16777220},
		// RTG
		{165,16777220},
		// No. DL symbols
		{169,16777217},
		// No. UL symbols
		{170,16777217},
		// UL Sounding Response Time
		{171,16777217},
		// UL Sounding Max No. Simultaneous Instructions
		{172,16777217},
		// Initial Ranging Method
		{173,16777217},
		// Handoff Ranging Method
		{174,16777217},
		// Periodic Ranging Method
		{175,16777217},
		// BW Request Method
		{176,16777217},
		// Fast FB/CQI Encoding
		{177,16777217},
		// Fast FB/CQI Allocation
		{178,16777217},
		// FEC Codes
		{179,16777217},
		// HARQ Support
		{180,16777217},
		// Incremental Redundancy
		{181,16777217},
		// DL Modulation Rate
		{182,16777217},
		// UL Modulation Rate
		{183,16777217},
		// Map types supported
		{184,16777217},
		// Reserved
		{185,16777217},
		// Power Control
		{186,33554434},
		// CINR Measurement
		{188,33554436},
		// STC/MIMO DL PUSC
		{192,33554434},
		// STC/MIMO DL FUSC
		{194,33554434},
		// STC/MIMO DL O-FUSC
		{196,33554434},
		// STC/MIMO DL O-AMC
		{198,33554434},
		// STC/MIMO DL PUSC-ASCA
		{200,16777217},
		// STC/MIMO UL PUSC
		{201,16777217},
		// STC/MIMO UL O-PUSC
		{202,16777217},
		// STC/MIMO UL O-AMC
		{203,16777217},
		// Closed Loop MIMO
		{204,33554434},
		// MIMO Feedback
		{206,33554434},
		// MIMO Midamble
		{208,16777217},
		// MIMO Soft HO/Macro Diversity
		{209,16777217},
		// HARQ DL MIMO
		{210,16777217},
		// HARQ UL MIMO
		{211,16777217},
		// FBSS
		{212,16777217},
		// Soft HO
		{213,16777217},
		// UL Macro diversity
		{214,16777217},
		// Max Concurrent bursts
		{215,16777217},
		// Max bursts in frame
		{216,16777217},
		// Max Concurrent  Burst in DL Subframe with H-ARQ
		{217,16777217},
		// Max Concurrent  Burst in UL Subframe with H-ARQ
		{218,16777217},
		// Reserved-3
		{219,16777217},
};



static CFG_OFFSETTABLE CFG_MAC_01[] = {
		//  Length
		{0,33554434},
		//  Version
		{2,16777217},
		//  Checksum
		{156,33554434},
		//  CS Options
		{3,33554434},
		//  DHCP
		{5,16777217},
		//  MAC Address
		{6,134217736},
		//  IP Address
		{14,134217744},
		//  IP subnet mask
		{30,134217744},
		//  IP default gateway
		{46,134217744},
		//  IP DNS server #1
		{62,134217744},
		//  IP DNS server #2
		{78,134217744},
		//  PHS
		{94,16777217},
		//  CS Options
		{95,16777217},
		//  PDU formats
		{96,33554434},
		//  Feedback mechanisms
		{98,33554434},
		//  Multicast connection
		{100,16777217},
		//  Network Entry
		{101,16777217},
		//  ARQ
		{102,16777217},
		//  HARQ support
		{103,16777217},
		//  QOS
		{104,16777217},
		//  Data delivery service for mobile
		{105,16777217},
		//  Request-grant mechanism
		{106,33554434},
		//  Neighbor advertisement
		{108,16777217},
		//  Scanning
		{109,16777217},
		//  Scan reporting type support
		{110,16777217},
		//  Association
		{111,16777217},
		//  Association type support
		{112,16777217},
		//  HO/Scan/Report Trigger Metrics
		{113,16777217},
		//  HO Optimization
		{114,67108868},
		//  MAC layer HO procedures
		{118,33554434},
		//  CID and SAID Update
		{120,33554434},
		//  Fast BS Switching
		{122,67108868},
		//  Macro diversity handover
		{126,67108868},
		//  Sleep mode
		{130,67108868},
		//  Idle mode
		{134,67108868},
		//  Expedited Network Re-entry from Idle Mode
		{138,67108868},
		//  PKM version
		{142,16777217},
		//  PKMv2 Authorization Policy - Initial network entry
		{143,16777217},
		//  PKMv2 Authorization Policy - network re-entry
		{144,16777217},
		//  SA Service Type
		{145,16777217},
		//  Supported cryptographic suites
		{146,33554434},
		//  MBS
		{148,33554434},
		//  Multicast Broadcast Re-keying Algorithm (MBRA)
		{150,16777217},
		//  Paging retry count
		{151,16777217},
		//  DCD Interval
		{152,33554434},
		//  UCD Interval
		{154,33554434},
};



/* ******************************* LEGACY DEFINITIONS **************************** */

static CFG_OFFSETTABLE CFG_MIB_01[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{592,33554434},
		/* MIBFlag */
		{3,16777217},
		/* BPFSReserved */
		{4,67109376},
		/* wmanIfSsLostDLMapInterval */
		{516,33554434},
		/* wmanIfSsLostULMapInterval */
		{518,33554434},
		/* wmanIfSsContentionRangRetries */
		{520,33554434},
		/* wmanIfSsRequestRetries */
		{522,33554434},
		/* wmanIfSsRegRequestRetries */
		{524,33554434},
		/* wmanIfSsT1Timeout */
		{526,33554434},
		/* wmanIfSsT2Timeout */
		{528,33554434},
		/* wmanIfSsT6Timeout */
		{530,33554434},
		/* wmanIfSsT12Timeout */
		{532,33554434},
		/* wmanIfSsT18Timeout */
		{534,33554434},
		/* wmanIfSsT20Timeout */
		{536,33554434},
		/* wmanIfSsT21Timeout */
		{538,33554434},
		/* wmanIfSsT14Timeout */
		{540,16777217},
		/* wmanIfSsT3Timeout */
		{541,16777217},
		/* wmanIfSsT4Timeout */
		{542,16777217},
		/* wmanIfSsSBCRequestRetries */
		{543,16777217},
		/* wmanIfCmnInvitedRangRetries */
		{544,33554434},
		/* wmanIfCmnT7Timeout */
		{546,33554434},
		/* wmanIfCmnT8Timeout */
		{548,33554434},
		/* wmanIfCmnT10Timeout */
		{550,33554434},
		/* wmanIfCmnT22Timeout */
		{552,33554434},
		/* u16Reserved1 */
		{554,33554434},
		/* wmanIfCmnDSxReqRetries */
		{556,67108868},
		/* wmanIfCmnDSxRespRetries */
		{560,67108868},
		/* wmanMssSpFrequency */
		{564,67108868},
		/* wmanMssNap */
		{568,67108868},
		/* wmanMssChanBandwidth */
		{572,67108868},
		/* wmanIfCmnCryptoSuiteIndex-1 */
		{576,67108868},
		/* wmanIfCmnCryptoSuiteDataEncryptAlg-1 */
		{580,16777217},
		/* wmanIfCmnCryptoSuiteDataAuthentAlg-1 */
		{581,16777217},
		/* wmanIfCmnCryptoSuiteTekEncryptAlg-1 */
		{582,16777217},
		/* u8Reserved-1 */
		{583,16777217},
		/* wmanIfCmnCryptoSuiteIndex-2 */
		{584,67108868},
		/* wmanIfCmnCryptoSuiteDataEncryptAlg-2 */
		{588,16777217},
		/* wmanIfCmnCryptoSuiteDataAuthentAlg-2 */
		{589,16777217},
		/* wmanIfCmnCryptoSuiteTekEncryptAlg-2 */
		{590,16777217},
		/* u8Reserved-2 */
		{591,16777217},
};


static CFG_OFFSETTABLE CFG_VSRO_01[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{437,33554434},
		/* VendorSpecificData */
		{3,16777663},
};

static CFG_OFFSETTABLE CFG_VSRO_02[] = {
		/* Length */
		{0,33554434},
		/* Version */
		{2,16777217},
		/* Checksum */
		{447,33554434},
		/* VendorSpecificData */
		{3,16777663},
};
static CFG_OFFSETTABLE CFG_CAL_16[] = {
		// Length
		{0,33554434},
		// Version
		{2,16777217},
		// Checksum
		{88,33554434},
		// Calibration Flag
		{3,16777217},
		// Calibration Vendor ID
		{4,16777217},
		// Date
		{5,16777219},
		// Time
		{8,16777219},
		// PHY Binary Version
		{11,33554434},
		// MAC Binary Version
		{13,16777219},
		// Cal API Library Version
		{16,16777219},
		// CAL SW Version
		{19,16777219},
		// Calibration Temperature
		{22,16777217},
		// RX Calibration Amplitude
		{23,16777217},
		// Calibration Bandwidth
		{CFG_NOT_SUPPORTED, CFG_NOT_SUPPORTED},
		// TARA Custom Field 1
		{24,134217736},
		// TARA Custom Field 2
		{32,134217736},
		// TARA Custom Field 3
		{40,134217736},
		// TARA Custom Field 4
		{48,134217736},
		// TARA Custom Field 5
		{56,134217736},
		// TARA Custom Field 6
		{64,134217736},
		// TARA Custom Field 7
		{72,134217736},
		// TARA Custom Field 8
		{80,134217736},
		// Content Version
		{CFG_NOT_SUPPORTED,CFG_NOT_SUPPORTED},
};

static CFG_OFFSETTABLE CFG_CAL_17[] = {
  // Length
  {0,33554434},
  // Version
  {2,16777217},
  // Checksum
  {88,33554434},
  // Calibration Flag
  {3,16777217},
  // Calibration Vendor ID
  {4,16777217},
  // Date
  {5,16777219},
  // Time
  {8,16777219},
  // PHY Binary Version
  {11,33554434},
  // MAC Binary Version
  {13,16777219},
  // Cal API Library Version
  {16,16777219},
  // CAL SW Version
  {19,16777219},
  // Calibration Temperature
  {22,16777217},
  // RX Calibration Amplitude
  {23,16777217},
  // Calibration Bandwidth
  {24,33554434},
  // Calibration Custom Field 1
  {26,16777222},
  // Calibration Custom Field 2
  {32,134217736},
  // Calibration Custom Field 3
  {40,134217736},
  // Calibration Custom Field 4
  {48,134217736},
  // Calibration Custom Field 5
  {56,134217736},
  // Calibration Custom Field 6
  {64,134217736},
  // Calibration Custom Field 7
  {72,134217736},
  // Calibration Custom Field 8
  {80,134217735},
  // Content Version
  {87,16777217},
};



/* ********************************** VERSION LISTS ****************************** */




static CFG_OFFTBL_LIST RFVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												// Version-0
		{CFG_NOT_SUPPORTED, 0},												// Version-1
		{CFG_RF_02, sizeof(CFG_RF_02)/sizeof(CFG_OFFSETTABLE)},				// Version-2
		{CFG_RF_02, sizeof(CFG_RF_02)/sizeof(CFG_OFFSETTABLE)},				// Version-3
		{CFG_RF_02, sizeof(CFG_RF_02)/sizeof(CFG_OFFSETTABLE)},				// Version-4
		{CFG_NOT_SUPPORTED, 0},												// Version-5
		{CFG_NOT_SUPPORTED, 0},												// Version-6
		{CFG_NOT_SUPPORTED, 0},												// Version-7
		{CFG_NOT_SUPPORTED, 0},												// Version-8
		{CFG_NOT_SUPPORTED, 0},												// Version-9
		{CFG_NOT_SUPPORTED, 0},												// Version-10
		{CFG_NOT_SUPPORTED, 0},												// Version-11
		{CFG_NOT_SUPPORTED, 0},												// Version-12
		{CFG_NOT_SUPPORTED, 0},												// Version-13
		{CFG_NOT_SUPPORTED, 0},												// Version-14
		{CFG_NOT_SUPPORTED, 0},												// Version-15
		{CFG_RF_16, sizeof(CFG_RF_16)/sizeof(CFG_OFFSETTABLE)},				// Version-16
		{CFG_RF_17, sizeof(CFG_RF_17)/sizeof(CFG_OFFSETTABLE)},				// Version-17
		{CFG_RF_18, sizeof(CFG_RF_18)/sizeof(CFG_OFFSETTABLE)}              // Version-18		
};

static CFG_OFFTBL_LIST PHYVerList[] = {
		{CFG_PHY_00, sizeof(CFG_PHY_00)/sizeof(CFG_OFFSETTABLE)},				// Version-0
		{CFG_PHY_00, sizeof(CFG_PHY_00)/sizeof(CFG_OFFSETTABLE)},				// Version-1
		{CFG_PHY_00, sizeof(CFG_PHY_00)/sizeof(CFG_OFFSETTABLE)},				// Version-2
		{CFG_PHY_00, sizeof(CFG_PHY_00)/sizeof(CFG_OFFSETTABLE)},				// Version-3
		{CFG_NOT_SUPPORTED, 0},												// Version-4
		{CFG_NOT_SUPPORTED, 0},												// Version-5
		{CFG_NOT_SUPPORTED, 0},												// Version-6
		{CFG_NOT_SUPPORTED, 0},												// Version-7
		{CFG_NOT_SUPPORTED, 0},												// Version-8
		{CFG_NOT_SUPPORTED, 0},												// Version-9
		{CFG_NOT_SUPPORTED, 0},												// Version-10
		{CFG_NOT_SUPPORTED, 0},												// Version-11
		{CFG_NOT_SUPPORTED, 0},												// Version-12
		{CFG_NOT_SUPPORTED, 0},												// Version-13
		{CFG_NOT_SUPPORTED, 0},												// Version-14
		{CFG_NOT_SUPPORTED, 0},												// Version-15
		{CFG_PHY_16, sizeof(CFG_PHY_16)/sizeof(CFG_OFFSETTABLE)},				// Version-16
		{CFG_PHY_16, sizeof(CFG_PHY_16)/sizeof(CFG_OFFSETTABLE)}				// Version-17
};

static CFG_OFFTBL_LIST MACVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												// Version-0
		{CFG_MAC_01, sizeof(CFG_MAC_01)/sizeof(CFG_OFFSETTABLE)},				// Version-1
		{CFG_NOT_SUPPORTED, 0},												// Version-2
		{CFG_NOT_SUPPORTED, 0},												// Version-3
		{CFG_NOT_SUPPORTED, 0},												// Version-4
		{CFG_NOT_SUPPORTED, 0},												// Version-5
		{CFG_NOT_SUPPORTED, 0},												// Version-6
		{CFG_NOT_SUPPORTED, 0},												// Version-7
		{CFG_NOT_SUPPORTED, 0},												// Version-8
		{CFG_NOT_SUPPORTED, 0},												// Version-9
		{CFG_NOT_SUPPORTED, 0},												// Version-10
		{CFG_NOT_SUPPORTED, 0},												// Version-11
		{CFG_NOT_SUPPORTED, 0},												// Version-12
		{CFG_NOT_SUPPORTED, 0},												// Version-13
		{CFG_NOT_SUPPORTED, 0},												// Version-14
		{CFG_NOT_SUPPORTED, 0},												// Version-15
		{CFG_MAC_16, sizeof(CFG_MAC_16)/sizeof(CFG_OFFSETTABLE)},				// Version-16
		{CFG_MAC_16, sizeof(CFG_MAC_16)/sizeof(CFG_OFFSETTABLE)}				// Version-17
};

static CFG_OFFTBL_LIST CALVerList[] = {
  {CFG_NOT_SUPPORTED, 0},            // Version-0
  {CFG_NOT_SUPPORTED, 0},            // Version-1
  {CFG_NOT_SUPPORTED, 0},            // Version-2
  {CFG_NOT_SUPPORTED, 0},            // Version-3
  {CFG_NOT_SUPPORTED, 0},            // Version-4
  {CFG_NOT_SUPPORTED, 0},            // Version-5
  {CFG_NOT_SUPPORTED, 0},            // Version-6
  {CFG_NOT_SUPPORTED, 0},            // Version-7
  {CFG_NOT_SUPPORTED, 0},            // Version-8
  {CFG_NOT_SUPPORTED, 0},            // Version-9
  {CFG_NOT_SUPPORTED, 0},            // Version-10
  {CFG_NOT_SUPPORTED, 0},            // Version-11
  {CFG_NOT_SUPPORTED, 0},            // Version-12
  {CFG_NOT_SUPPORTED, 0},            // Version-13
  {CFG_NOT_SUPPORTED, 0},            // Version-14
  {CFG_NOT_SUPPORTED, 0},            // Version-15
  {CFG_CAL_16, sizeof(CFG_CAL_16)/sizeof(CFG_OFFSETTABLE)},// Version-16
  {CFG_CAL_17, sizeof(CFG_CAL_17)/sizeof(CFG_OFFSETTABLE)}    // Version-17
};




static CFG_OFFTBL_LIST HWVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												/* Version-0 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-1 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-2 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-3 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-4 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-5 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-6 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-7 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-8 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-9 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-10 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-11 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-12 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-13 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-14 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-15 */
		{CFG_HW_16, sizeof(CFG_HW_16)/sizeof(CFG_OFFSETTABLE)},				/* Version-16 */
		{CFG_HW_16, sizeof(CFG_HW_16)/sizeof(CFG_OFFSETTABLE)},				/* Version-17 */
		{CFG_HW_18,	sizeof(CFG_HW_18)/sizeof(CFG_OFFSETTABLE)}				/* Version-18 */		
};

static CFG_OFFTBL_LIST MIBSVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												/* Version-0 */
		{CFG_MIB_01, sizeof(CFG_MIB_01)/sizeof(CFG_OFFSETTABLE)},			/* Version-1 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-2 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-3 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-4 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-5 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-6 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-7 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-8 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-9 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-10 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-11 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-12 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-13 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-14 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-15 */
		{CFG_MIB_16, sizeof(CFG_MIB_16)/sizeof(CFG_OFFSETTABLE)},			/* Version-16 */
		{CFG_MIB_16, sizeof(CFG_MIB_16)/sizeof(CFG_OFFSETTABLE)},			/* Version-17 */
		{CFG_MIB_16, sizeof(CFG_MIB_16)/sizeof(CFG_OFFSETTABLE)},			/* Version-18 */
		{CFG_MIB_19, sizeof(CFG_MIB_19)/sizeof(CFG_OFFSETTABLE)},			/* Version-19 */
		{CFG_MIB_20, sizeof(CFG_MIB_20)/sizeof(CFG_OFFSETTABLE)},			/* Version-20 */
};

static CFG_OFFTBL_LIST SECVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												/* Version-0 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-1 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-2 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-3 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-4 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-5 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-6 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-7 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-8 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-9 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-10 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-11 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-12 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-13 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-14 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-15 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-16 */
		{CFG_SECU_17, sizeof(CFG_SECU_17)/sizeof(CFG_OFFSETTABLE)},			/* Version-17 */
		{CFG_SECU_18, sizeof(CFG_SECU_18)/sizeof(CFG_OFFSETTABLE)}			/* Version-18 */
};

static CFG_OFFTBL_LIST OMAVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												/* Version-0 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-1 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-2 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-3 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-4 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-5 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-6 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-7 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-8 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-9 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-10 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-11 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-12 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-13 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-14 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-15 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-16 */
		{CFG_OMA_17, sizeof(CFG_OMA_17)/sizeof(CFG_OFFSETTABLE)}			/* Version-17 */
};


static CFG_OFFTBL_LIST MRUVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												// Version-0
		{CFG_MRU_1, sizeof(CFG_MRU_1)/sizeof(CFG_OFFSETTABLE)}				// Version-1
};

static CFG_OFFTBL_LIST BCMVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												// Version-0
		{CFG_BCM_1, sizeof(CFG_BCM_1)/sizeof(CFG_OFFSETTABLE)}				// Version-1
};

static CFG_OFFTBL_LIST VSAVerList[] = {
		{CFG_NOT_SUPPORTED, 0},												/* Version-0 */
		{CFG_VSRO_01, sizeof(CFG_VSRO_01)/sizeof(CFG_OFFSETTABLE)},			/* Version-1 */
		{CFG_VSRO_02, sizeof(CFG_VSRO_02)/sizeof(CFG_OFFSETTABLE)},			/* Version-2 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-3 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-4 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-5 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-6 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-7 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-8 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-9 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-10 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-11 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-12 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-13 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-14 */
		{CFG_NOT_SUPPORTED, 0},												/* Version-15 */
		{CFG_VSA_16, sizeof(CFG_VSA_16)/sizeof(CFG_OFFSETTABLE)},			/* Version-16 */
		{CFG_VSA_16, sizeof(CFG_VSA_16)/sizeof(CFG_OFFSETTABLE)}			/* Version-17 */
};



CFG_POFFTBL_LIST	CFG_BlockVersionList[]	= {CFG_NOT_SUPPORTED,
												RFVerList,
												PHYVerList,
												MACVerList,
												CALVerList,
												HWVerList,
												MIBSVerList,
												SECVerList,
												OMAVerList,
												MRUVerList,
												BCMVerList,
												VSAVerList};

CFG_UINT32			CFG_MaxVersionList[]	= {CFG_NOT_SUPPORTED,
												sizeof(RFVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(PHYVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(MACVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(CALVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(HWVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(MIBSVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(SECVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(OMAVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(MRUVerList)/sizeof(CFG_OFFTBL_LIST),
												sizeof(BCMVerList)/sizeof(CFG_OFFTBL_LIST),	
												sizeof(VSAVerList)/sizeof(CFG_OFFTBL_LIST)};
