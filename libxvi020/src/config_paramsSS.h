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
 * Description	:	Structure definition for the config parameters. This
 *					was previously defined in SubscriberStationSS.h
 * Author		:	Viswanath Dibbur, M.Nagarajan
 * Reviewer		:
 *
 */


#ifndef     CONFIG_PARAMS_H_
#define     CONFIG_PARAMS_H_


#define MAX_CRYPTOSUITE_INDEX 2

typedef unsigned char	WmanIfDataEncryptAlgId ;
typedef unsigned char	WmanIfDataAuthAlgId ;
typedef unsigned char	WmanIfTekEncryptAlgId ;


/*Each entry contains the cryptographic suite pair that SS or BS supports."*/
typedef struct WmanIfCmnCryptoSuiteEntry
{
	B_UINT32 	wmanIfCmnCryptoSuiteIndex;

	B_UINT8 wmanIfCmnCryptoSuiteDataEncryptAlg;
	B_UINT8 wmanIfCmnCryptoSuiteDataAuthentAlg;
	B_UINT8 wmanIfCmnCryptoSuiteTekEncryptAlg ;
	B_UINT8 u8Reserved;

}WmanIfCmnCryptoSuiteEntry;

#pragma pack(push,1)
typedef struct stConfigParams{

		B_UINT16	u16length;
		/*   Version, Operational Params Version Number */
		B_UINT8		u8Version;

		/*   MIB Flag, which currently tells CRC failure and Version Mismatch */
		B_UINT8		u8MIBFlag;

		/* Network Search Periodic wake up timer */
		B_UINT32	BcmConfigNetSearchPeriodicWakeup;

		/*   Beceem Factory Settings */
		B_UINT32			u32BPFSReserved[126];

		/* T44 for MOB_SCAN_REQ Time Out */
		B_UINT32 	wmanIfM2SsT44Timer;		

		/*  wmanIfSsConfigurationTable (Factory Config) */
		/*   Lost DLMAP timeout */
		B_UINT16 	wmanIfSsLostDLMapInterval;
		/*   Lost ULMAP timeout */
		B_UINT16 	wmanIfSsLostULMapInterval ;

		/*   Number of retries on contention Ranging Requests. */
		B_UINT16 	wmanIfSsContentionRangRetries;
		/*   Number of retries on bandwidth allocation requests.. */
		B_UINT16 	wmanIfSsRequestRetries ;

		/*   Number of retries on registration requests. */
		B_UINT16 	wmanIfSsRegRequestRetries;
		/*   T1 wait for the DCD message timeout */
		B_UINT16 	wmanIfSsT1Timeout;

		/*   T2 wait for broadcast ranging timeout */
		B_UINT16 	wmanIfSsT2Timeout;
		/*   T6 REG response reception timeout value in milliseconds */
		B_UINT16 	wmanIfSsT6Timeout;

		/*   T12 wait for the UCD message timeout */
		B_UINT16 	wmanIfSsT12Timeout;
		/*   T18 SBC response reception timeout value in milliseconds */
		B_UINT16 	wmanIfSsT18Timeout;

		/*   T20 Time the SS searches for a preamble on a given channel */
		B_UINT16 	wmanIfSsT20Timeout;
		/*   T21 Time the SS waits for the DLMAP on a given channel */
		B_UINT16 	wmanIfSsT21Timeout;

		/*   T14 wait for DSX-RVD Timeout */
		B_UINT8 		wmanIfSsT14Timeout;
		/*   T3 Ranging response reception timeout value in milliseconds */
		B_UINT8 		wmanIfSsT3Timeout;
		B_UINT8 		wmanIfSsT4Timeout;
		/*   Number of retries on SBC Requests. */
		B_UINT8 		wmanIfSsSBCRequestRetries;

		/* wmanIfCmnBsSsConfigurationTable (EMS config) */

		/*   Number of retries on inviting Ranging Requests. */
		B_UINT16 	wmanIfCmnInvitedRangRetries;
		/*   T7 wait for DSA/DSC/DSD Response timeout */
		B_UINT16 	wmanIfCmnT7Timeout;

		/*   T8 wait for DSA/DSC Acknowledge timeout */
		B_UINT16 	wmanIfCmnT8Timeout;
		/*   T10 wait for transaction end timeout */
		B_UINT16 	wmanIfCmnT10Timeout;

		/*   Wait for ARQ Reset in ms. */
		B_UINT16 	wmanIfCmnT22Timeout;

		/*   Included for 4 Byte alignment -can be modified/removed later  */
		B_UINT16 	u16Reserved1;
		
		/*   Number of Timeout Retries on DSA/DSC/DSD Requests. */
		B_UINT32 	wmanIfCmnDSxReqRetries;
		/*   Number of Timeout Retries on DSA/DSC/DSD Responses. */
		B_UINT32 	wmanIfCmnDSxRespRetries;

		/*wmanMssSpFreqTable (EMS config)*/
		B_UINT32 	wmanMssSpFrequency;

		/*wmanMssNapNspTable (EMS config)*/
		B_UINT32 	wmanMssNap;
		/*  wmanMssChanBandwidthTable (EMS config) */
		B_UINT32 	wmanMssChanBandwidth;
		
		WmanIfCmnCryptoSuiteEntry wmanIfCmnCryptoSuiteTable[MAX_CRYPTOSUITE_INDEX];

		
		/*   **** PLEASE ADD NEW MIBS BELOW THIS ONLY ******/
			

}stConfigParams;

#define	CFG_MIBS_VERSIZEDIFF_TABLE	{\
			0,							 \
			0							\
		}
#pragma pack(pop)


#endif /* CONFIG_PARAMS_H_ */
