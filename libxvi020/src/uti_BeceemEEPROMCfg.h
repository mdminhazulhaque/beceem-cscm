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


#ifndef	_UTI_BECEEMEEPROM_H__
#define	_UTI_BECEEMEEPROM_H__

#include "PrivateTypedefs.h"
/*  Compile time Macros */
#define CFG_SWAPENDIAN_ENABLE

#ifdef WIN32
	typedef unsigned __int8							CFG_UINT8;
	typedef unsigned __int16						CFG_UINT16;
	typedef unsigned __int32						CFG_UINT32;
	typedef unsigned __int64						CFG_UINT64;
	typedef void									CFG_VOID;
	typedef	void*									CFG_HANDLE;
	typedef __int32									CFG_INT32;
#elif (defined(LINUX) || defined(MACOS))
	typedef signed int								CFG_INT32;
	typedef unsigned char							CFG_UINT8;
	typedef unsigned short							CFG_UINT16;
	typedef unsigned int							CFG_UINT32;
	typedef unsigned long long     		    		CFG_UINT64;
	typedef void									CFG_VOID;
	typedef	void*									CFG_HANDLE;
#endif
/*  Other Macros */
#define	CFG_MAX_BLOCKS								12

#define	CFG_BLOCK_COMP								0
#define CFG_BLOCK_RF								1
#define CFG_BLOCK_PHY								2
#define CFG_BLOCK_MAC								3
#define	CFG_BLOCK_CALIB								4
#define	CFG_BLOCK_HW								5
#define CFG_BLOCK_MIB								6
#define CFG_BLOCK_SEC								7
#define CFG_BLOCK_OMA								8
#define CFG_BLOCK_MRU								9
#define CFG_BLOCK_BCM								10
#define CFG_BLOCK_VSA								11
#define	CFG_BLOCK_NOACCESS							0xFF

#define	CFG_BITFIELD_BLOCKMASK(block)				(1 << (block + 1))

/*  Errors */
#define CFG_NOT_SUPPORTED							(0)
#define CFG_ERR_UNTOUCHED							(0)		/* this block was not loaded/validated */
#define CFG_ERR_NONE								(-1)	/* No Error */
#define CFG_ERR_CRITICAL							(-2)	/* Internal Error, some write went beyond EEPROM boundary */
#define CFG_ERR_VERSION								(-3)	/* Invalid Version */
#define CFG_ERR_CHECKSUM							(-4)	/* Invalid Checksum	*/
#define CFG_ERR_VERSION_NOT_SUPPORTED				(-5)	/* Version not supported */
#define CFG_ERR_PARAMID_NOT_SUPPORTED				(-6)	/* ParamId provided is wrong/not supported */
#define CFG_ERR_INVALID_SIZE						(-7)	/* Size provided (for VSA only), is beyond VSA boundary */
#define CFG_ERR_BLOCK_START							(-8)	/* block start offset is wrong */
#define CFG_ERR_BLOCK_SIZE							(-9)	/* block size is invalid */
#define CFG_ERR_INVALID_ACCESS						(-10)	/* access on an inaccessible block */
#define CFG_ERR_MUTEX								(-11)	/* error on mutex */
#define CFG_ERR_EEPROM_SIZE							(-12)	/* shared memory size < size on eeprom */
#define CFG_ERR_SKIPUPLOAD							(-13)	/* upload to shadow skipped */

/*  Macro to create the param command */
#define CFG_MAKE_USERPARAM_SIZE(offset, size)		((offset << 16)|(size&0xFFFF))

// MAC Address : DataType = B_UINT8
#define CFG_COMP_MAC_ADDRESS_ID												0
// UICC ATR Lock Flag : DataType = B_UINT8
#define CFG_COMP_UICC_ATR_LOCK_FLAG_ID										1
// Checksum : DataType = B_UINT16
#define CFG_COMP_CHECKSUM_ID													2
// Reserved : DataType = B_UINT8
#define CFG_COMP_RESERVED_ID													3
// EEPROM Size : DataType = B_UINT16
#define CFG_COMP_EEPROM_SIZE_ID												4
// EEPROM Version : DataType = B_UINT8
#define CFG_COMP_EEPROM_VERSION_ID											5
// RF Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_RF_PARAMETERS_POINTER_ID									6
// PHY Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_PHY_PARAMETERS_POINTER_ID									7
// MAC Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_MAC_PARAMETERS_POINTER_ID									8
// CAL Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_CAL_PARAMETERS_POINTER_ID									9
// HW Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_HW_PARAMETERS_POINTER_ID									10
// MIB-Config Parameters Pointer : DataType = B_UINT32
#define CFG_COMP_MIBCONFIG_PARAMETERS_POINTER_ID							11
// Security Pointer : DataType = B_UINT32
#define CFG_COMP_SECURITY_POINTER_ID											12
// OMA-DM Pointer : DataType = B_UINT32
#define CFG_COMP_OMADM_POINTER_ID											13
// MRU Pointer : DataType = B_UINT32 
#define CFG_COMP_MRU_POINTER_ID												14
// Beceem Parameters Pointer : DataType = B_UINT32 
#define CFG_COMP_BECEEM_PARAMETERS_POINTER_ID								15
// VSA Parameters Pointer : DataType = B_UINT32 
#define CFG_COMP_VSA_PARAMETERS_POINTER_ID									16
// Reserved : DataType = B_UINT32  
#define CFG_COMP_RESERVED1_ID													17

/* Length : DataType = CFG_UINT16 */

#define CFG_HW_LENGTH_ID                                                    83886080
/* Version : DataType = CFG_UINT8  */
#define CFG_HW_VERSION_ID                                                   83886081
/* Checksum : DataType = CFG_UINT16 */
#define CFG_HW_CHECKSUM_ID                                                  83886082
/* GPIO-0 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO0_FUNCTIONALITY_SETTING_ID                               83886083
/* GPIO-1 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO1_FUNCTIONALITY_SETTING_ID                               83886084
/* GPIO-2 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO2_FUNCTIONALITY_SETTING_ID                               83886085
/* GPIO-3 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO3_FUNCTIONALITY_SETTING_ID                               83886086
/* GPIO-4 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO4_FUNCTIONALITY_SETTING_ID                               83886087
/* GPIO-5 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO5_FUNCTIONALITY_SETTING_ID                               83886088
/* GPIO-6 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO6_FUNCTIONALITY_SETTING_ID                               83886089
/* GPIO-7 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO7_FUNCTIONALITY_SETTING_ID                               83886090
/* GPIO-8 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO8_FUNCTIONALITY_SETTING_ID                               83886091
/* GPIO-9 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO9_FUNCTIONALITY_SETTING_ID                               83886092
/* GPIO-10 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO10_FUNCTIONALITY_SETTING_ID                              83886093
/* GPIO-11 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO11_FUNCTIONALITY_SETTING_ID                              83886094
/* GPIO-12 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO12_FUNCTIONALITY_SETTING_ID                              83886095
/* GPIO-13 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO13_FUNCTIONALITY_SETTING_ID                              83886096
/* GPIO-14 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO14_FUNCTIONALITY_SETTING_ID                              83886097
/* GPIO-15 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO15_FUNCTIONALITY_SETTING_ID                              83886098
/* GPIO-16 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO16_FUNCTIONALITY_SETTING_ID                              83886099
/* GPIO-17 Functionality Setting : DataType = CFG_UINT8  */
#define CFG_HW_GPIO17_FUNCTIONALITY_SETTING_ID                              83886100
/* GPIO-18 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO18_FUNCTIONALITY_SETTING_ID								83886101
/* GPIO-19 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO19_FUNCTIONALITY_SETTING_ID								83886102
/* GPIO-20 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO20_FUNCTIONALITY_SETTING_ID								83886103
/* GPIO-21 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO21_FUNCTIONALITY_SETTING_ID								83886104
/* GPIO-22 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO22_FUNCTIONALITY_SETTING_ID								83886105
/* GPIO-23 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO23_FUNCTIONALITY_SETTING_ID								83886106
/* GPIO-24 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO24_FUNCTIONALITY_SETTING_ID								83886107
/* GPIO-25 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO25_FUNCTIONALITY_SETTING_ID								83886108
/* GPIO-26 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO26_FUNCTIONALITY_SETTING_ID								83886109
/* GPIO-27 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO27_FUNCTIONALITY_SETTING_ID								83886110
/* GPIO-28 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO28_FUNCTIONALITY_SETTING_ID								83886111
/* GPIO-29 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO29_FUNCTIONALITY_SETTING_ID								83886112
/* Reserved : DataType = CFG_UINT8  */
/* GPIO-30 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO30_FUNCTIONALITY_SETTING_ID								83886113
/* GPIO-31 Functionality Setting : DataType = CFG_UINT8   */
#define CFG_HW_GPIO31_FUNCTIONALITY_SETTING_ID								83886114
/* IMEI : DataType = CFG_UINT8   */
#define CFG_HW_IMEI_ID														83886172
/* Reserved : DataType = CFG_UINT8   */
#define CFG_HW_RESERVED_ID													83886173
/* Content Version : DataType = CFG_UINT8   */
#define CFG_HW_CONTENT_VERSION_ID											83886174
/* Padding : DataType = CFG_UINT8   */
#define CFG_HW_PADDING_ID													83886175

/* Length : DataType = CFG_UINT16 */
#define CFG_MIB_LENGTH_ID                                                   100663296
/* Version : DataType = CFG_UINT8  */
#define CFG_MIB_VERSION_ID                                                  100663297
/* Checksum : DataType = CFG_UINT16 */
#define CFG_MIB_CHECKSUM_ID                                                 100663298
/* MIBFlag : DataType = CFG_UINT8  */
#define CFG_MIB_MIBFLAG_ID                                                  100663299
/* BcmConfigNetSearchPeriodicWakeup : DataType = CFG_UINT32  */
#define CFG_MIB_BCMCONFIGNETSEARCHPERIODICWAKEUP_ID							100663300
/* BcmConfigUartFunctionality : DataType = CFG_UINT32  */
#define CFG_MIB_BCMCONFIGUARTFUNCTIONALITY_ID								100663301
/* BPFSReserved : DataType = CFG_UINT32  */
#define CFG_MIB_BPFSRESERVED_ID											100663302
/* wmanif2MSST44Timer : DataType = CFG_UINT32  */
#define CFG_MIB_WMANIF2MSST44TIMER_ID										100663303
/* wmanIfSsLostDLMapInterval : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSSLOSTDLMAPINTERVAL_ID								100663304
/* wmanIfSsLostULMapInterval  : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSSLOSTULMAPINTERVAL_ID								100663305
/* wmanIfSsContentionRangRetries : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSSCONTENTIONRANGRETRIES_ID							100663306
/* wmanIfSsRequestRetries  : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSSREQUESTRETRIES_ID								100663307
/* wmanIfSsRegRequestRetries : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSSREGREQUESTRETRIES_ID								100663308
/* wmanIfSsT1Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST1TIMEOUT_ID										100663309
/* wmanIfSsT2Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST2TIMEOUT_ID										100663310
/* wmanIfSsT6Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST6TIMEOUT_ID										100663311
/* wmanIfSsT12Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST12TIMEOUT_ID										100663312
/* wmanIfSsT18Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST18TIMEOUT_ID										100663313
/* wmanIfSsT20Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST20TIMEOUT_ID										100663314
/* wmanIfSsT21Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFSST21TIMEOUT_ID										100663315
/* wmanIfSsT14Timeout : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFSST14TIMEOUT_ID										100663316
/* wmanIfSsT3Timeout : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFSST3TIMEOUT_ID										100663317
/* wmanIfSsT4Timeout : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFSST4TIMEOUT_ID										100663318
/* wmanIfSsSBCRequestRetries : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFSSSBCREQUESTRETRIES_ID								100663319
/* wmanIfCmnInvitedRangRetries : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFCMNINVITEDRANGRETRIES_ID								100663320
/* wmanIfCmnT7Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFCMNT7TIMEOUT_ID										100663321
/* wmanIfCmnT8Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFCMNT8TIMEOUT_ID										100663322
/* wmanIfCmnT10Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFCMNT10TIMEOUT_ID										100663323
/* wmanIfCmnT22Timeout : DataType = CFG_UINT16  */
#define CFG_MIB_WMANIFCMNT22TIMEOUT_ID										100663324
/* u16Reserved1 : DataType = CFG_UINT16  */
#define CFG_MIB_U16RESERVED1_ID											100663325
/* wmanIfCmnDSxReqRetries : DataType = CFG_UINT32  */
#define CFG_MIB_WMANIFCMNDSXREQRETRIES_ID									100663326
/* wmanIfCmnDSxRespRetries : DataType = CFG_UINT32  */
#define CFG_MIB_WMANIFCMNDSXRESPRETRIES_ID								100663327
/* wmanMssSpFrequency : DataType = CFG_UINT32  */
#define CFG_MIB_WMANMSSSPFREQUENCY_ID										100663328
/* wmanMssNap : DataType = CFG_UINT32  */
#define CFG_MIB_WMANMSSNAP_ID												100663329
/* wmanMssChanBandwidth : DataType = CFG_UINT32  */
#define CFG_MIB_WMANMSSCHANBANDWIDTH_ID									100663330
/* wmanIfCmnCryptoSuiteIndex-1 : DataType = CFG_UINT32  */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEINDEX1_ID								100663331
/* wmanIfCmnCryptoSuiteDataEncryptAlg-1 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEDATAENCRYPTALG1_ID						100663332
/* wmanIfCmnCryptoSuiteDataAuthentAlg-1 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEDATAAUTHENTALG1_ID						100663333
/* wmanIfCmnCryptoSuiteTekEncryptAlg-1 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITETEKENCRYPTALG1_ID						100663334
/* u8Reserved-1 : DataType = CFG_UINT8   */
#define CFG_MIB_U8RESERVED1_ID												100663335
/* wmanIfCmnCryptoSuiteIndex-2 : DataType = CFG_UINT32  */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEINDEX2_ID								100663336
/* wmanIfCmnCryptoSuiteDataEncryptAlg-2 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEDATAENCRYPTALG2_ID						100663337
/* wmanIfCmnCryptoSuiteDataAuthentAlg-2 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITEDATAAUTHENTALG2_ID						100663338
/* wmanIfCmnCryptoSuiteTekEncryptAlg-2 : DataType = CFG_UINT8   */
#define CFG_MIB_WMANIFCMNCRYPTOSUITETEKENCRYPTALG2_ID						100663339
/* Content Version : DataType = CFG_UINT8   */
#define CFG_MIB_CONTENT_VERSION_ID										100663340

// Length : DataType = CFG_UINT16 
#define CFG_SECU_LENGTH_ID												117440512
// Version : DataType = CFG_UINT8  
#define CFG_SECU_VERSION_ID												117440513
// Checksum : DataType = CFG_UINT16 
#define CFG_SECU_CHECKSUM_ID												117440514
// Certificates Encryption Status Flag : DataType = CFG_UINT8  
#define CFG_SECU_CERTIFICATES_ENCRYPTION_STATUS_FLAG_ID					117440515
// Beceem Encryption Flag : DataType = CFG_UINT8  
#define CFG_SECU_BECEEM_ENCRYPTION_FLAG_ID								117440516
// Firmware Decryption Key : DataType = CFG_UINT8  
#define CFG_SECU_FIRMWARE_DECRYPTION_KEY_ID								117440517
// Certificate-1 Data Length : DataType = CFG_UINT16 
#define CFG_SECU_CERTIFICATE1_DATA_LENGTH_ID								117440518
// Certificate-1 Data : DataType = CFG_UINT8  
#define CFG_SECU_CERTIFICATE1_DATA_ID										117440519
// Private Key Data-1 Length : DataType = CFG_UINT16 
#define CFG_SECU_PRIVATE_KEY_DATA1_LENGTH_ID								117440520
// Private Key Data-1 : DataType = CFG_UINT8  
#define CFG_SECU_PRIVATE_KEY_DATA1_ID										117440521
// Certificate-2 Data Length : DataType = CFG_UINT16 
#define CFG_SECU_CERTIFICATE2_DATA_LENGTH_ID								117440522
// Certificate-2 Data : DataType = CFG_UINT8  
#define CFG_SECU_CERTIFICATE2_DATA_ID										117440523
// Private Key Data-2 Length : DataType = CFG_UINT16 
#define CFG_SECU_PRIVATE_KEY_DATA2_LENGTH_ID								117440524
// Private Key Data-2 : DataType = CFG_UINT8  
#define CFG_SECU_PRIVATE_KEY_DATA2_ID										117440525
// Content Version : DataType = CFG_UINT8   
#define CFG_SECU_CONTENT_VERSION_ID										117440526
// Padding : DataType = CFG_UINT8   
#define CFG_SECU_PADDING_ID												117440527

/* Length : DataType = CFG_UINT16  */
#define CFG_OMA_LENGTH_ID													134217728
/* Version : DataType = CFG_UINT8   */
#define CFG_OMA_VERSION_ID													134217729
/* Checksum : DataType = CFG_UINT16  */
#define CFG_OMA_CHECKSUM_ID													134217730
/* Device Disable Flag : DataType = CFG_UINT8   */
#define CFG_OMA_DEVICE_DISABLE_FLAG_ID										134217731
/* OMA Data : DataType = CFG_UINT8   */
#define CFG_OMA_OMA_DATA_ID												134217732


// Length : DataType = CFG_UINT16 
#define CFG_MRU_LENGTH_ID													150994944
// Version : DataType = CFG_UINT8  
#define CFG_MRU_VERSION_ID												150994945
// Checksum : DataType = CFG_UINT16 
#define CFG_MRU_CHECKSUM_ID												150994946
// Content Version : DataType = CFG_UINT8   
#define CFG_MRU_CONTENT_VERSION_ID										150994947
// Head Index : DataType = CFG_UINT8  
#define CFG_MRU_HEAD_INDEX_ID												150994948
// Tail Index : DataType = CFG_UINT8  
#define CFG_MRU_TAIL_INDEX_ID												150994949
// Reserved1 : DataType = CFG_UINT8  
#define CFG_MRU_RESERVED1_ID												150994950
// MRUData : DataType = CFG_UINT8  
#define CFG_MRU_MRUDATA_ID												150994951



/* Length : DataType = CFG_UINT16  */
#define CFG_VSA_LENGTH_ID												184549376
/* Version : DataType = CFG_UINT8   */
#define CFG_VSA_VERSION_ID												184549377
/* Checksum : DataType = CFG_UINT16  */
#define CFG_VSA_CHECKSUM_ID												184549378
/* VendorSpecificData : DataType = CFG_UINT8   */
#define CFG_VSA_VENDORSPECIFICDATA_ID									184549379


#define CFG_CAL_CHECKSUM_ID             								67108866
// Calibration Flag : DataType = B_UINT8
#define CFG_CAL_CALIBRATION_FLAG_ID           							67108867

/* RF version */
#define CFG_RF_VERSION_ID												16777217

/* PHY version */
#define CFG_PHY_VERSION_ID												33554433

/* MAC version */
#define CFG_MAC_VERSION_ID												50331649

/* CAL version */
#define CFG_CAL_VERSION_ID												67108865

#define BECEEM_VALIDATEPTR(ptr,ret)\
			if(!ptr){DPRINT(DEBUG_ERROR, DEBUG_PATH_ALL,"Invalid Memory Access @ function %s and Line %d \n",__FUNCTION__,__LINE__);return ret;}
volatile BOOL *g_pBigEndianMachine;




typedef struct _CFG_OFFSETTABLE
{
	CFG_UINT32	 		u32Offset;
	CFG_UINT32			u32Size;
	CFG_VOID			*pvDefaultVal;

}	CFG_OFFSETTABLE;

typedef  struct _CFG_OFFTBL_LIST
{
	CFG_OFFSETTABLE*	pstOffsetTblPtr;
	CFG_UINT32			u32MaxParams;
}	CFG_OFFTBL_LIST,	*CFG_POFFTBL_LIST;



typedef struct _CFG_BLOCKTABLE
{

	CFG_UINT32             u32BlockStartOffsets;
	CFG_UINT16             u16Padding;
	CFG_UINT8              u8BlockError;
	CFG_UINT8              u8BlockVersion;
	CFG_POFFTBL_LIST		pstActiveOffsetTables;
	
}	CFG_BLOCKTABLE;
/* Context Table */
typedef struct _CFG_CONTEXTTABLE
{
	CFG_UINT32				u32WellKnowPattern;
	CFG_UINT32				u32InitBitField;
	CFG_UINT16				u16StartOffset;
	CFG_UINT16				u16TotalSize;
	CFG_UINT8*				pu8EEPROMShadow;
	CFG_UINT32				u32OverallMajorVersion;
	CFG_BLOCKTABLE			stBlockTbl[CFG_MAX_BLOCKS];

	CFG_UINT8*				pu8ReLocAddress[CFG_MAX_BLOCKS];
	CFG_HANDLE				hDriver;
	CFG_HANDLE				hMutex;
}	CFG_CONTEXTTABLE;

typedef struct _CFG_LEGACYCONTEXTTABLE
{
	CFG_CONTEXTTABLE		stContextTbl;
	CFG_UINT32				u32VSROVersion;
	CFG_UINT32				u32ExVSROPresent;
	CFG_UINT8				au8VSROData[0xA00];

}	CFG_LEGACYCONTEXTTABLE;



CFG_UINT32	CFG_Init(CFG_HANDLE hDriver,
					 CFG_HANDLE hMutex,
					 CFG_UINT8 *pu8SharedMem,
					 CFG_UINT32 u32SharedMemSize);

CFG_INT32	CFG_ParamSize(CFG_UINT8 *pu8SharedMem,
						  CFG_UINT32 u32ParamId);

CFG_INT32	CFG_ParamType(CFG_UINT8 *pu8SharedMem,
						  CFG_UINT32 u32ParamId);

CFG_INT32	CFG_GetErrorCode(CFG_UINT8 *pu8SharedMem,
							 CFG_UINT8 u8BlockId);

CFG_INT32	CFG_ReadParam(CFG_UINT8 *pu8SharedMem,
						  CFG_UINT8 *pu8Buffer,
						  CFG_UINT32 u32BufferSize,
						  CFG_UINT32 u32ParamId);

CFG_INT32	CFG_WriteVSA(CFG_UINT8 *pu8SharedMem,
							CFG_UINT8 *pu8Buffer,
							CFG_UINT32 u32VSAOffset,
							CFG_UINT32 u32BufSize);

CFG_INT32	CFG_WriteMIB(CFG_UINT8 *pu8SharedMem,
							CFG_UINT8 *pu8Buffer,
							CFG_UINT32 u32BufSize,
							CFG_UINT32 u32ParamId);

CFG_INT32	CFG_WriteOMADM(CFG_UINT8 *pu8SharedMem, 
						   CFG_UINT8 *pu8Buffer, 
						   CFG_UINT32 u32BufSize, 
						   CFG_UINT32 u32ParamId);

CFG_INT32	CFG_WriteSecurity(CFG_UINT8 *pu8SharedMem, 
							  CFG_UINT8 *pu8Buffer, 
							  CFG_UINT32 u32BufSize, 
							  CFG_UINT32 u32ParamId);
CFG_INT32	CFG_UpdateOMAChecksum(CFG_UINT8 *pu8SharedMem, 
								  CFG_UINT32 u32ParamId);
CFG_INT32	CFG_WriteMRU(CFG_UINT8 *pu8SharedMem, 
						 CFG_UINT8 *pu8Buffer, 
						 CFG_UINT32 u32BufSize, 
						 CFG_UINT32 u32ParamId);
CFG_INT32	CFG_WriteParamMRU(CFG_UINT8 *pu8SharedMem, 
							  CFG_UINT8 *pu8Buffer, 
							  CFG_UINT32 u32BufSize, 
							  CFG_UINT32 u32ParamId);
CFG_INT32	CFG_ResetMRU(CFG_UINT8 *pu8SharedMem);
CFG_VOID	CFG_SetMIBWriteOption(CFG_UINT32 u32SetMIBOption);
CFG_UINT32 CFG_WriteMultipleParamsToFlashSection(CFG_UINT8 *pu8SharedMem,
										PST_FLASH_PARAM_ID_WRITES pstFlashParamWrite,
										UINT32 u32FlashMulitpleMIBWriteCount,
										CFG_UINT8 *pu8ShadowAddress);

CFG_UINT32 CFG_WriteBlockSection(CFG_CONTEXTTABLE *pstCntxTbl,CFG_UINT8 *pInBlockContents,
						CFG_UINT8 *pu8ShadowAddress,UINT8 u8BlockId);
CFG_UINT32 CFG_ReadBlockSection(CFG_CONTEXTTABLE *pstCntxTbl,CFG_UINT8 *pOutBlockContents,
							UINT8 u8BlockId);
CFG_UINT32	CFG_WriteParamForFlash(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, 
								   CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId, CFG_UINT8 *pu8ShadowAddress);
CFG_INT32	CFG_ResetMRUFlash(CFG_UINT8 *pu8SharedMem);

CFG_VOID CFG_ReadVersion(CFG_UINT8 * pu8SharedMem, CFG_UINT32 * pu32Buffer);


#endif /* _UTI_BECEEMEEPROM_H__ */
