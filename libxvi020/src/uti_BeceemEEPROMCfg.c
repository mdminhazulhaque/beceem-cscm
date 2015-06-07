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


#include "uti_BeceemEEPROMCfg.h"
#include "BeceemWiMAX.h"
#include "WiMAXCommonAPI.h"
#ifdef WIN32
	#pragma warning(disable : 4311 4047 4024)
#endif
#include "globalobjects.h"
/* Compile options */
//#define CFG_V4HACK_ENABLE

/* #defines */
#define	CFG_COMP_VERSION_OFFSET						14
#define	CFG_BLOCK_STARTPTR_OFFSET					16
#define	CFG_BLOCK_VERSION_OFFSET					2
#define	CFG_BLOCK_SIZE_OFFSET						0

#define	CFG_DEFAULT_EEPROM_SIZE						0x1000
#define CFG_MIBS_ADDRESS							0xBF60AC80

#define	CFG_TARGET_CONTEXT_ADDRESS					0xBF60EF00
#define	CFG_WELLKNOW_PATTERN						0x0ADD0ADD

#define	CFG_UPLOADTORAM								0
#define	CFG_SKIP_UPLOADTORAM						1

#define CFG_RELOCATION_LIST_ADDRESS					(0xBF60EF00 - 4*(CFG_MAX_BLOCKS + 1))


/* MACROS */
#define	CFG_COMP_MAJOR_VERSION(ver)					(ver >> 8)
#define CFG_COMP_MINOR_VERSION(ver)					(ver & 0xFF)

#define	CFG_GET_BLOCKID(paramid)					(paramid >> 24)
#define	CFG_GET_PARAMID(paramid)					(paramid & 0x00FFFFFF)
#define CFG_IS_BLOCKID(paramid)						((paramid&0x00FFFFFF) == 0x00FFFFFF)
#define CFG_USER_EXTRACT_OFFSET(size)				(size >> 16)
#define CFG_USER_EXTRACT_SIZE(size)					(size & 0xFFFF)

#define	CFG_IS_LEGACY_VERSION(obj)					(1 == obj->u32OverallMajorVersion)

#define	CFG_BLOCK_ACTIVEPTR(obj, paramid)			(obj->stBlockTbl[CFG_GET_BLOCKID(paramid)].pstActiveOffsetTables)
#define	CFG_BLOCK_OFFSETPTR(obj, paramid)			(CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr)
#define	CFG_BLOCK_MAX_PARAMS(obj, paramid)			(CFG_BLOCK_ACTIVEPTR(obj, paramid)->u32MaxParams)

#define	CFG_PARAM_EXTRACT_DEFVAL(obj, paramid)		(CFG_UINT32)((CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].pvDefaultVal))
#define	CFG_PARAM_EXTRACT_OFFSET(obj, paramid)		((CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].u32Offset) & 0x003FFFFF)
#define	CFG_PARAM_EXTRACT_SIZE(obj, paramid)		((CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].u32Size) & 0xFFFFFF)
#define	CFG_PARAM_EXTRACT_TYPE(obj, paramid)		((CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].u32Size) >> 24)

#define	CFG_PARAM_SET_OFFSET(obj, paramid, offset)		(CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].u32Offset = offset & 0x003FFFFF)
#define	CFG_PARAM_SIZE(obj, paramid)				(CFG_BLOCK_ACTIVEPTR(obj, paramid)->pstOffsetTblPtr[CFG_GET_PARAMID(paramid)].u32Size)


/* Accessibility Matrix */							/* *Comp*                                  *RF*                          *PHY*                            *MAC*                    *VSRO/TARA*                       *HW*                               *MIB*                                  *VSA*       */
#ifndef  CFG_OVERRIDE_DEFAULT_ACCESSMATRIX
#define	CFG_ACCESSIBLE_PARAM_CHKSUM_LIST			{CFG_COMP_CHECKSUM_ID,          CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_HW_CHECKSUM_ID,                CFG_MIB_CHECKSUM_ID,                      CFG_VSA_CHECKSUM_ID}
#define	CFG_ACCESSIBLE_PARAM_BLKVER_LIST			{0,                             CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_HW_VERSION_ID,                 CFG_MIB_VERSION_ID,                       CFG_VSA_VERSION_ID}
#define	CFG_ACCESSIBLE_PARAM_BLKST_LIST				{0,                             CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_COMP_HW_PARAMETERS_POINTER_ID, CFG_COMP_MIBCONFIG_PARAMETERS_POINTER_ID, CFG_COMP_VSA_PARAMETERS_POINTER_ID}
#define	CFG_ACCESSIBLE_PARAM_BLKSZ_LIST				{CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_ERR_PARAMID_NOT_SUPPORTED, CFG_HW_LENGTH_ID,                  CFG_MIB_LENGTH_ID,                        CFG_VSA_LENGTH_ID}
#define	CFG_ACCESSIBLE_READ_MASK					(CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_CALIB)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_RF)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_PHY)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_MAC)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_HW)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_MIB)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_OMA)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_SEC)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_MRU))
#define	CFG_ACCESSIBLE_WRITE_MASK					(CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_MIB)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_VSA)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_OMA)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_SEC)|CFG_BITFIELD_BLOCKMASK(CFG_BLOCK_MRU))
#define	CFG_ACCESSIBLE_BLOCK_MASK					(CFG_ACCESSIBLE_READ_MASK|CFG_ACCESSIBLE_WRITE_MASK)
#endif

/* External References */
extern CFG_POFFTBL_LIST		CFG_BlockVersionList[];
extern CFG_UINT32			CFG_MaxVersionList[];

/* Global definitions */
/* g_u32MIBWriteFlag = 0 means write into EEPROM 
   g_u32MIBWriteFlag = 1 means write into Shadow */
static CFG_UINT32	g_u32MIBWriteFlag = 0;

static CFG_UINT32	g_u32CallFromCFGInitFlag = 0;
/* Function Pointers Definitions */
typedef CFG_UINT32			(*CFG_DEVREAD)(CFG_CONTEXTTABLE *, CFG_UINT32, CFG_UINT32 *);
typedef CFG_UINT32			(*CFG_DEVWRITE)(CFG_CONTEXTTABLE *, CFG_UINT32, CFG_UINT32);

/* Forward Declarations */
static CFG_VOID				CFG_Swap(CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT8 u8DataType);
static CFG_UINT32			CFG_RAMWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 u32Data);
static CFG_UINT32			CFG_EEPROMWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 u32Data);
static CFG_UINT32			CFG_EEPROMBulkWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT8 * pu8Buffer, CFG_UINT32 u32Size);
static CFG_UINT32			CFG_RAMRead(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32SrcOffset, CFG_UINT32 *pu32Data);
static CFG_UINT32			CFG_EEPROMRead(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 *pu32Data);
static CFG_UINT32 			CFG_Write(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVWRITE pfnDevWrite, CFG_DEVREAD pfnDevRead);
static CFG_UINT32 			CFG_OptimizeWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVWRITE pfnDevWrite, CFG_DEVREAD pfnDevRead);
static CFG_UINT8			CFG_Read(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVREAD pfnDevRead);
static CFG_UINT32			CFG_WriteParam(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId, CFG_UINT8 *pu8ShadowAddress);
#if 0
static CFG_UINT32			CFG_CheckAndAutoUpdateMIB(CFG_CONTEXTTABLE *pstCntxTbl);
#endif
static CFG_UINT16			CFG_CalculateChecksum(CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Size, CFG_UINT16 u16CheckSum);
static CFG_UINT16			CFG_ComputeDiffChecksum(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size,CFG_UINT32 *pbCheckSumValid);
static CFG_INT32			CFG_LegacyInit(CFG_LEGACYCONTEXTTABLE *pstCntxTbl);
static CFG_UINT32			CFG_LegacyReadParam(void *pstCntxTbl, CFG_UINT8 *pu8Data, CFG_UINT32 u32DataSize, CFG_UINT32 u32ParamId);
static CFG_UINT32			CFG_LegacyWriteMIBOnShadow(void *pstCtxTbl, CFG_UINT8 *pu8Data, CFG_UINT32 u32DataSize, CFG_UINT32 u32ParamId);

CFG_UINT32	gu32ChipId = 0;
CFG_UINT32 gu32AccEEPROM = 1;
UINT32 g_bNVMTypeIsFLASH = FALSE;


void CFG_UpdateSecuSecTable(CFG_CONTEXTTABLE *pstCntxTbl)
{
	CFG_UINT16 u16Offset = 0;
	CFG_UINT16 u16Length = 0;
	g_u32CallFromCFGInitFlag = 1;
	// Get the length
	CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&u16Length, sizeof(u16Length), CFG_SECU_CERTIFICATE1_DATA_LENGTH_ID);
	// No need to update the offset of CFG_SECU_CERTIFICATE1_DATA_ID
	// need to update the size of CFG_SECU_CERTIFICATE1_DATA_ID
	CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_CERTIFICATE1_DATA_ID) = 
			(CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_CERTIFICATE1_DATA_ID) & 0xFF000000)  |
										(u16Length & 0xFFFFFF);
	

	// Compute new offset for PRIVATE_KEY_DATA1_LENGTH_ID
	// update the offset of CFG_SECU_PRIVATE_KEY_DATA1_LENGTH_ID
	u16Offset = (CFG_UINT16)CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl,CFG_SECU_CERTIFICATE1_DATA_ID) + u16Length;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA1_LENGTH_ID,  u16Offset);
	// Get the length of privare key data 1
	u16Length = 0;
	CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&u16Length, sizeof(u16Length), CFG_SECU_PRIVATE_KEY_DATA1_LENGTH_ID);

	
	// need to update the size and offset of CFG_SECU_PRIVATE_KEY_DATA1_ID
	CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA1_ID) = 
			(CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA1_ID) & 0xFF000000)  |
										(u16Length & 0xFFFFFF);
	u16Offset += 2;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA1_ID, u16Offset);


	// Compute new offset for CFG_SECU_CERTIFICATE2_DATA_LENGTH_ID
	u16Offset = u16Offset + u16Length;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_CERTIFICATE2_DATA_LENGTH_ID,  u16Offset);
	// Get the length of cert 2 data
	u16Length = 0;
	CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&u16Length, sizeof(u16Length), CFG_SECU_CERTIFICATE2_DATA_LENGTH_ID);

	// need to update the size and offset of CFG_SECU_CERTIFICATE2_DATA_ID
	CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_CERTIFICATE2_DATA_ID) = 
			(CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_CERTIFICATE2_DATA_ID) & 0xFF000000)  |
										(u16Length & 0xFFFFFF);
	u16Offset += 2;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_CERTIFICATE2_DATA_ID,  u16Offset);

	// Compute new offset for CFG_SECU_PRIVATE_KEY_DATA2_LENGTH_ID
	u16Offset = u16Offset + u16Length;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA2_LENGTH_ID,  u16Offset);
	// Get the length of private key data 2
	u16Length = 0;
	CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&u16Length, sizeof(u16Length), CFG_SECU_PRIVATE_KEY_DATA2_LENGTH_ID);

	// need to update the size and offset of CFG_SECU_PRIVATE_KEY_DATA2_ID
	CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA2_ID) = 
			(CFG_PARAM_SIZE(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA2_ID) & 0xFF000000)  |
										(u16Length & 0xFFFFFF);
	u16Offset += 2;
	CFG_PARAM_SET_OFFSET(pstCntxTbl, CFG_SECU_PRIVATE_KEY_DATA2_ID, u16Offset);
	g_u32CallFromCFGInitFlag = 0;
}




CFG_INT32 CFG_InitializeMRU_initStruct(CFG_CONTEXTTABLE *pstCntxTbl)
{
	int iRetValue= 0,iFunctionReturnedValue = 0;
	MRU_INIT_STRUCT *pDeviceData = NULL;
	g_u32CallFromCFGInitFlag = 1;
	
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	pDeviceData = (MRU_INIT_STRUCT *)GetDeviceDataMRUPointer();
	if (pDeviceData == NULL)
		{
			iFunctionReturnedValue = 2;
			goto end_return_case;
		}

	// Either no error or checksum is wrong (that can be corrected)
	if((pstCntxTbl->stBlockTbl[CFG_BLOCK_MRU].u8BlockError == (B_UINT8) CFG_ERR_NONE) ||
		(pstCntxTbl->stBlockTbl[CFG_BLOCK_MRU].u8BlockError == (B_UINT8)CFG_ERR_CHECKSUM))
	{	
		if(pstCntxTbl->stBlockTbl[CFG_BLOCK_MRU].u8BlockError == (B_UINT8)CFG_ERR_CHECKSUM)
		{
			// Reset will correct the checksum and reset the MRU table in EEPROM
			iRetValue = CFG_ResetMRU((CFG_UINT8 *)pstCntxTbl);
			if(iRetValue < 0)
			{
				// Unable to correct checksum
				// MRU_INIT_STRUCT contains all 0's
				iFunctionReturnedValue = -1;
				goto end_return_case;

			}
			pDeviceData->U8WasChecksumCorrected = 1;
		}
		pDeviceData->u8Valid = 1;

		// Fill the Length
		iRetValue = CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&pDeviceData->u16Length,
						sizeof(pDeviceData->u16Length), CFG_MRU_LENGTH_ID);

		if(iRetValue < 0)
			{
				iFunctionReturnedValue = -1;
				goto end_return_case;
			}
		// Fill the version
		iRetValue = CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&pDeviceData->u8Version,
						sizeof(pDeviceData->u8Version), CFG_MRU_VERSION_ID);
		if(iRetValue < 0)
			{
				iFunctionReturnedValue = -1;
				goto end_return_case;
			}
		// Fill the head Index
		iRetValue = CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&pDeviceData->u8HeadIndex,
						sizeof(pDeviceData->u8HeadIndex), CFG_MRU_HEAD_INDEX_ID);
		if(iRetValue < 0)
			{
				iFunctionReturnedValue = -1;
				goto end_return_case;
			}
		// Fill the tail index
		iRetValue = CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&pDeviceData->u8TailIndex,
			sizeof(pDeviceData->u8TailIndex), CFG_MRU_TAIL_INDEX_ID);

		if(iRetValue < 0)
			{
				iFunctionReturnedValue = -1;
				goto end_return_case;
			}
		// Fill the MRU data
		iRetValue = CFG_ReadParam((CFG_UINT8 *)pstCntxTbl, (B_UINT8 *)&pDeviceData->mruRows,
			sizeof(MRU_ROW_FORMAT) * MAX_CHANNEL_PLAN_REF_ID_PER_CAPL, CFG_MRU_MRUDATA_ID);

		if(iRetValue < 0)
			{
				iFunctionReturnedValue = -1;
				goto end_return_case;
			}
		iFunctionReturnedValue = 0;
		goto end_return_case;
	}
	
	// MRU block is invalid for this EEPROM (in case of old EEPROM)
	// MRU_INIT_STRUCT contains all 0's
	iFunctionReturnedValue = 3;
	
end_return_case:
	g_u32CallFromCFGInitFlag = 0;
	return iFunctionReturnedValue;
}

/*  ========================================================================== */
/*  ----------------------------------- INIT  -------------------------------- */
/*  ========================================================================== */
CFG_UINT32	CFG_Init(CFG_HANDLE hDriver, CFG_HANDLE hMutex, CFG_UINT8 *pu8SharedMem, CFG_UINT32 u32SharedMemSize)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT32			u32Size = 0;
	CFG_INT32			iRet = 0, i = 0;
	CFG_UINT32			u32Address = CFG_TARGET_CONTEXT_ADDRESS;
	CFG_UINT8			u8BlockId = CFG_BLOCK_COMP;
	CFG_UINT8*			pContextTableOnDDR = NULL;
	CFG_UINT8*			pShadowAccess = NULL;
	
	UNUSED_ARGUMENT(u32SharedMemSize);
	/* This is the max memory used by this code */

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	memset(pu8SharedMem, 0, sizeof(CFG_LEGACYCONTEXTTABLE));
	BECEEMCALLOC_RET(pContextTableOnDDR, CFG_UINT8*,CFG_UINT8,(sizeof(CFG_CONTEXTTABLE)-2*sizeof(CFG_HANDLE)), CFG_UINT32)

	pstCntxTbl->hDriver = hDriver;
	pstCntxTbl->hMutex  = hMutex;

	/* Target would have evaluated the eeprom */
	/* So just read the Context Table from Target */
	iRet = BeceemWiMAX_rdmBulk(pstCntxTbl->hDriver, u32Address, (UCHAR *)pContextTableOnDDR, (sizeof(CFG_CONTEXTTABLE) - 2*sizeof(CFG_HANDLE)));
	if (iRet < 0)
		goto returnCode;

	/* Populate library context table from shadow */
	pShadowAccess = pContextTableOnDDR;
	memcpy(pstCntxTbl,pShadowAccess,sizeof(CFG_UINT32)*2);
	pShadowAccess+=sizeof(CFG_UINT32)*2;

	if(*g_pBigEndianMachine)
	{
		*(CFG_UINT32 *)pShadowAccess=(((*(CFG_UINT32 *)pShadowAccess>>16)&0xffff)+((*(CFG_UINT32 *)pShadowAccess << 16)&0xffff0000)); //Swap TotalSize & StartOffset
	}

	memcpy(&pstCntxTbl->u16StartOffset,pShadowAccess,sizeof(CFG_UINT32));
	pShadowAccess+=sizeof(CFG_UINT32);

	
		
	pstCntxTbl->pu8EEPROMShadow = (CFG_UINT8*)(UADDTYPE)(*(CFG_UINT32 *)pShadowAccess);
	pShadowAccess+=sizeof(CFG_UINT32);
	
	pstCntxTbl->u32OverallMajorVersion = *(CFG_UINT32 *)pShadowAccess;
	pShadowAccess+=sizeof(CFG_UINT32);

	

	for(i=0;i<CFG_MAX_BLOCKS;i++)
	{
		*(CFG_UINT32 *)&pstCntxTbl->stBlockTbl[i].u32BlockStartOffsets = *(CFG_UINT32 *)pShadowAccess;
		pShadowAccess+=sizeof(CFG_UINT32);

		if(*g_pBigEndianMachine)
			*(CFG_UINT32 *)&pstCntxTbl->stBlockTbl[i].u16Padding = Swap_Int(*(CFG_UINT32 *)pShadowAccess);
		else
			*(CFG_UINT32 *)&pstCntxTbl->stBlockTbl[i].u16Padding = *(CFG_UINT32 *)pShadowAccess;
		pShadowAccess+=sizeof(CFG_UINT32);
		
		pstCntxTbl->stBlockTbl[i].pstActiveOffsetTables = (CFG_POFFTBL_LIST)(UADDTYPE)(*(CFG_UINT32 *)pShadowAccess);
		pShadowAccess+=sizeof(CFG_UINT32);
	
	}

	/* Check if the Shadow has the pattern */
	if(CFG_WELLKNOW_PATTERN != pstCntxTbl->u32WellKnowPattern) {
		memset(pstCntxTbl, 0, sizeof(CFG_CONTEXTTABLE));
		iRet = CFG_ERR_CRITICAL;
		goto returnCode;
	}
	
	/* Load Relocation Table, as default values are populated by the firmware */
	iRet = BeceemWiMAX_rdm(pstCntxTbl->hDriver, CFG_TARGET_CONTEXT_ADDRESS - 4, (UINT *)&u32Address, sizeof(UINT32));
	if(iRet)
		goto returnCode;
		
	if(u32Address == ~CFG_WELLKNOW_PATTERN) {
		u32Size = 0;
		u32Address = CFG_RELOCATION_LIST_ADDRESS;
		while(u32Size < CFG_MAX_BLOCKS) {	
			iRet = BeceemWiMAX_rdm(pstCntxTbl->hDriver, u32Address, (UINT *)&pstCntxTbl->pu8ReLocAddress[u32Size], sizeof(UINT32));
			if(iRet)
				goto returnCode;	
			
			if(pstCntxTbl->pu8ReLocAddress[u32Size] > 0) {
				pstCntxTbl->pu8ReLocAddress[u32Size] = (CFG_UINT8 *)((UADDTYPE)pstCntxTbl->pu8ReLocAddress[u32Size]|0xB0000000);
				pstCntxTbl->pu8ReLocAddress[u32Size] -= pstCntxTbl->stBlockTbl[u32Size].u32BlockStartOffsets;
			}
			u32Address += 4;
			u32Size++;
		}
	}

	iRet = BeceemWiMAX_rdm(pstCntxTbl->hDriver, CFG_MIBS_ADDRESS, (UINT *)&pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB], sizeof(UINT32));
	if(iRet)
		goto returnCode;
	
	pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB] = (CFG_UINT8 *)((UADDTYPE)pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB]|0xB0000000);
	pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB] -= pstCntxTbl->stBlockTbl[CFG_BLOCK_MIB].u32BlockStartOffsets;

	/* Discard blocks not accessible by DLL */
	pstCntxTbl->u32InitBitField &= CFG_ACCESSIBLE_BLOCK_MASK;

	/* Get Chip Id */
	iRet = BeceemWiMAX_rdm(pstCntxTbl->hDriver, 0xAF000000, &gu32ChipId, sizeof(UINT32));
	if (iRet < 0)
		goto returnCode;

	if(gu32ChipId <= 0xBECE0121) {
		/* T2 Chip, so use Legacy code */
		iRet = CFG_LegacyInit((CFG_LEGACYCONTEXTTABLE *)pu8SharedMem);
		goto returnCode;
	} else if(CFG_IS_LEGACY_VERSION(pstCntxTbl)) {
		/* Legacy format on a "Not-T2" chip!!! */
		iRet = CFG_ERR_VERSION_NOT_SUPPORTED;
		goto returnCode;
	}

	/* Read the VSA version as the Target(Firmware) would not have touched this block */
	if(CFG_Read(pstCntxTbl, &pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u8BlockVersion,
		pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u32BlockStartOffsets + CFG_BLOCK_VERSION_OFFSET,
			sizeof(CFG_UINT8), sizeof(CFG_UINT8), CFG_EEPROMRead) != 1)
		{
			iRet = CFG_ERR_CRITICAL;
			goto returnCode;
		}

	/* If the version is invalid return invalid version */
	if(pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u8BlockVersion >= CFG_MaxVersionList[CFG_BLOCK_VSA]) {
		pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u8BlockError = (CFG_UINT8)CFG_ERR_VERSION;
	} else {
		pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u8BlockError = (CFG_UINT8)CFG_ERR_NONE;
	}

	/* Load the version tables of every block */
	while(u8BlockId < CFG_MAX_BLOCKS) {
		if((CFG_ACCESSIBLE_BLOCK_MASK & CFG_BITFIELD_BLOCKMASK(u8BlockId)) &&
                        ((CFG_UINT8)CFG_ERR_NONE == pstCntxTbl->stBlockTbl[u8BlockId].u8BlockError ||
						(CFG_UINT8)CFG_ERR_CHECKSUM == pstCntxTbl->stBlockTbl[u8BlockId].u8BlockError))  {
				pstCntxTbl->stBlockTbl[u8BlockId].pstActiveOffsetTables =
					&CFG_BlockVersionList[u8BlockId][pstCntxTbl->stBlockTbl[u8BlockId].u8BlockVersion];

				// Security section dynamic updation
				// This will dynamically update the offsets in offset table stored in struct.cpp file
				if((u8BlockId == CFG_BLOCK_SEC) && (pstCntxTbl->stBlockTbl[u8BlockId].u8BlockVersion >= 0x12))
				{
					CFG_UpdateSecuSecTable(pstCntxTbl);
				}
		} 
		else 
		{
			pstCntxTbl->stBlockTbl[u8BlockId].pstActiveOffsetTables = CFG_NOT_SUPPORTED;
		}
		if(!(CFG_ACCESSIBLE_BLOCK_MASK & CFG_BITFIELD_BLOCKMASK(u8BlockId))) {
			pstCntxTbl->stBlockTbl[u8BlockId].u8BlockError =(CFG_UINT8) CFG_ERR_NONE;
		}
		u8BlockId++;
	}


	if(CFG_InitializeMRU_initStruct(pstCntxTbl) < 0)
		{
			DPRINT(DEBUG_MESSAGE,DEBUG_PATH_RX,"CFG_InitializeMRU_initStruct failed");
			iRet = CFG_ERR_CRITICAL;
			goto returnCode;
			
		}
	

	

	// Reallocated the relocation address of MIB section to the EEPROM shadow address.
	// This is required to read the MIB parameters from the EEPROM shadow.
	pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB] = pstCntxTbl->pu8EEPROMShadow;


	/* return the bit field, see the eeprom doc for details */
	iRet = pstCntxTbl->u32InitBitField;


returnCode:
	BECEEM_MEMFREE(pContextTableOnDDR)
	return iRet;
	
}




/*  ========================================================================== */
/*  --------------------------------- READ PARAM ----------------------------- */
/*  ========================================================================== */

CFG_INT32	CFG_ReadParam(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufferSize, CFG_UINT32 u32ParamId)
{
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset = 0;
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT8*			pu8ShadowBkup = pstCntxTbl->pu8EEPROMShadow;

	if(!(g_u32CallFromCFGInitFlag || BeceemWiMAX_IsSafeToAccessContextTabel()))
		{
			return CFG_ERR_INVALID_ACCESS;
		}
	/* Handle Legacy version */
	if(gu32ChipId <= 0xBECE0121) {
		/* T2 Chip, so use Legacy code */
		return CFG_LegacyReadParam((void*)pstCntxTbl, pu8Buffer, u32BufferSize, u32ParamId);
	} else if(CFG_IS_LEGACY_VERSION(pstCntxTbl)) {
		/* Legacy format on a "Not-T2" chip!!! */
		return CFG_ERR_VERSION_NOT_SUPPORTED;
	}

	/* Check if the Block is accessable */
	if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_INVALID_ACCESS;
	/* Check if the paramid is valid */
	} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	/* Check if buffer is long enough */
	} else if(CFG_USER_EXTRACT_OFFSET(u32BufferSize) > CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_INVALID_SIZE;
	}

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	/*Get the relocation address to eeprom shadow pointer*/
	if(pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)] > 0) {
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)];
	}

	/* Obtain the absolute location on RAM */
	u32Datatype = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	u32Offset   = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl,u32ParamId)
				+ CFG_USER_EXTRACT_OFFSET(u32BufferSize)
				+ pstCntxTbl->stBlockTbl[CFG_GET_BLOCKID(u32ParamId)].u32BlockStartOffsets;

	/* Check for embedded size/offset */
	if(CFG_USER_EXTRACT_SIZE(u32BufferSize) > (CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId) - CFG_USER_EXTRACT_OFFSET(u32BufferSize))) {
		u32BufferSize = CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId) - CFG_USER_EXTRACT_OFFSET(u32BufferSize);
	} else {
		u32BufferSize = CFG_USER_EXTRACT_SIZE(u32BufferSize);
	}

	if (CFG_GET_BLOCKID(u32ParamId) == CFG_BLOCK_VSA)
	{
		/* Need to restore the EEPROM shadow address before accessing EEPROM */
		pstCntxTbl->pu8EEPROMShadow = pu8ShadowBkup;
		/* read the data from eeprom for VSA*/
		if(CFG_Read(pstCntxTbl, pu8Buffer, u32Offset, u32BufferSize,(CFG_UINT8)u32Datatype, CFG_EEPROMRead) != 1)
			{
				goto error_return;
			}
		
	}
	else
	{
		/* for other blocks read the data from RAM */
		if(CFG_Read(pstCntxTbl, pu8Buffer, u32Offset, 
					u32BufferSize,(CFG_UINT8)u32Datatype, CFG_RAMRead) != 1)
			{
				goto error_return;
			}
	}
	pstCntxTbl->pu8EEPROMShadow = pu8ShadowBkup;

	return u32BufferSize;
	
error_return:
	pstCntxTbl->pu8EEPROMShadow = pu8ShadowBkup;
	return CFG_ERR_CRITICAL;
}


/*  ========================================================================== */
/*  --------------------------------- WRITE MIBS ----------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_WriteMIB(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	if(CFG_BLOCK_MIB != CFG_GET_BLOCKID(u32ParamId)) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	}

	/* Handle Legacy version */
	if(gu32ChipId <= 0xBECE0121) {
		/* T2 Chip, so use Legacy code */
		return CFG_LegacyWriteMIBOnShadow((void*)pstCntxTbl, pu8Buffer, u32BufSize, u32ParamId);
	} else if(CFG_IS_LEGACY_VERSION(pstCntxTbl)) {
		/* Legacy format on a "Not-T2" chip!!! */
		return CFG_ERR_VERSION_NOT_SUPPORTED;
	}

	return CFG_WriteParam(pstCntxTbl, pu8Buffer, u32BufSize, u32ParamId, pstCntxTbl->pu8EEPROMShadow);
}

CFG_INT32 CFG_CorrectChecksum(CFG_UINT8 *pu8SharedMem, CFG_UINT8 u8BlockId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT8			u8RAMData[1000] = {0};

	CFG_UINT32			u32Size = 0;
	CFG_UINT32			u32Offset = 0;
	CFG_UINT32			u32DeltaSize = 0;
	CFG_UINT16			u16Chksum = 0;


	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
	u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;

	if(pstCntxTbl->pu8ReLocAddress[u8BlockId] > 0) {
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[u8BlockId];
	}
	else
	{
		return CFG_ERR_CRITICAL;
	}

	u32Size = u32BlockSize;
	u32Offset = u32BlockStart;

	// calculate the checksum for MRU
	while(u32DeltaSize < u32Size) {
		u32DeltaSize = (u32Size > 1000)?1000:u32Size;
		if(CFG_Read(pstCntxTbl, u8RAMData, u32Offset, u32DeltaSize, 1, CFG_RAMRead) != 1)
			{
				return CFG_ERR_CRITICAL;
			}
		u16Chksum =u16Chksum + CFG_CalculateChecksum(u8RAMData, u32DeltaSize,(CFG_UINT16)0);
		u32Size -= u32DeltaSize;
		u32Offset += u32DeltaSize;
		u32DeltaSize = 0;
	}

	// Writting the checksum into RAM
	if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize, 
		sizeof(u16Chksum), sizeof(u16Chksum), CFG_RAMWrite,CFG_RAMRead)) 
    {
        pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
		return CFG_ERR_CRITICAL;
	}

	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;

	/* Write new checksum onto EEPROM */
	if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize,
				sizeof(u16Chksum), sizeof(u16Chksum), CFG_EEPROMWrite, CFG_EEPROMRead)) {
		return CFG_ERR_CRITICAL;
	}

	pstCntxTbl->stBlockTbl[u8BlockId].u8BlockError =(CFG_UINT8)CFG_ERR_NONE;
	return sizeof(u16Chksum);
	
}


/*  ==================== Called only by OMA-DM init application ============== */
/*  --------------- WRITE Correct checksum for OMADM ------------------------- */
/*  ========================================================================== */
CFG_INT32 CFG_UpdateOMAChecksum(CFG_UINT8 *pu8SharedMem, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT8			u8BlockId = 0;

	CFG_UINT32			u32Size = 0;
	CFG_UINT32			u32Offset = 0;
	CFG_UINT32			u32DeltaSize = 0;
	CFG_UINT16			u16Chksum = 0;
	CFG_UINT8			u8RAMData[1000] = {0};

	/* Obtain the absolute location on RAM */
	u8BlockId =(CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;

	if(CFG_BLOCK_OMA != u8BlockId) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	}

	if(pstCntxTbl->pu8ReLocAddress[u8BlockId] > 0) {
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)];
	}
	else
	{
		return CFG_ERR_CRITICAL;
	}

	u32Size = u32BlockSize;
	u32Offset = u32BlockStart;

	// read the entire OMA-DM section from RAM and calculate the checksum
	while(u32DeltaSize < u32Size) {
		u32DeltaSize = (u32Size > 1000)?1000:u32Size;
		if(CFG_Read(pstCntxTbl, u8RAMData, u32Offset, u32DeltaSize, 1, CFG_RAMRead) != 1)
			{
				return CFG_ERR_CRITICAL;
			}
			
		u16Chksum= u16Chksum + CFG_CalculateChecksum(u8RAMData, u32DeltaSize, 0);
		u32Size -= u32DeltaSize;
		u32Offset += u32DeltaSize;
		u32DeltaSize = 0;
	}

	// Writting the checksum into RAM
	if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize, 
		sizeof(u16Chksum), sizeof(u16Chksum), CFG_RAMWrite,CFG_RAMRead)) 
    {
        /* restore shadow? */
        pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
		return CFG_ERR_CRITICAL;
	}

	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;

	/* Write new checksum onto EEPROM */
	if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize,
				sizeof(u16Chksum), sizeof(u16Chksum), CFG_EEPROMWrite, CFG_EEPROMRead)) {
		return CFG_ERR_CRITICAL;
	}
	
	return sizeof(u16Chksum);
}


CFG_INT32	CFG_ResetMRUFlash(CFG_UINT8 *pu8SharedMem)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT8*			pu8ShadowAddress = pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU];
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset  = 0;
	CFG_UINT32			u32BufSize = 0;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT16			u16Chksum = 0;
	CFG_UINT8			u8BlockId = 0;
	CFG_UINT8			*pBlockContents = NULL;
	CFG_UINT32			bCheckSumValid = FALSE;

	CFG_UINT32 u32ParamId = CFG_MRU_MRUDATA_ID;
	MRU_ROW_FORMAT mruRows[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];
	CFG_UINT8 headIndex = 0x3F;

	memset(mruRows,0,sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL);

	/* Check if the Block has been loaded */
	if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_INVALID_ACCESS;
	/* Check if the paramid is valid */
	} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	} 

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	if(pu8ShadowAddress)
		pstCntxTbl->pu8EEPROMShadow = pu8ShadowAddress;
	
	u8BlockId = (CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
	u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;
	u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
				  + u32BlockStart;
	
	
	BECEEMCALLOC_RET(pBlockContents,CFG_UINT8 *,CFG_UINT8,u32BlockSize+sizeof(u16Chksum),CFG_INT32)
	
	/* Before Writing, Read the data section contents and compute differenatial checksum */
	if(BeceemWiMAX_NVMrdm(u32BlockStart,pBlockContents,sizeof(u16Chksum)+u32BlockSize) != 4)
	{
			u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
			goto error_Case;
	}

	
	if(pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)] > 0) 
	{
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)];
	}

	memcpy(&u16Chksum,&pBlockContents[u32BlockSize],sizeof(u16Chksum));

	CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));
	
	u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl,(CFG_UINT8 *) &mruRows, u32Offset, sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL,&bCheckSumValid);
		if(!bCheckSumValid)
		{
			u32BufSize =(CFG_UINT32) CFG_ERR_CRITICAL;
			goto error_Case;
			
		}
	memcpy(&pBlockContents[u32Offset-u32BlockStart],&mruRows,sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL);

	CFG_Swap(&pBlockContents[u32Offset-u32BlockStart], sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL,(CFG_UINT8)u32Datatype);

	u32BufSize = sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL;
	if( NULL != pu8ShadowAddress) 
	{
		if(u32BufSize != CFG_Write(pstCntxTbl, &pBlockContents[u32Offset-u32BlockStart], u32Offset, u32BufSize,(CFG_UINT8)u32Datatype, CFG_RAMWrite, CFG_RAMRead))
			{
				u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
				goto error_Case;
			}
	}

	
	u32ParamId = CFG_MRU_HEAD_INDEX_ID;
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
				  + u32BlockStart;
	
	u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl, &headIndex, u32Offset, 1,&bCheckSumValid);
	if(!bCheckSumValid)
		{
			u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
			goto error_Case;
			
		}
	memcpy(&pBlockContents[u32Offset-u32BlockStart],&headIndex,1);

	u32BufSize = 1;
	if( NULL != pu8ShadowAddress) 
	{
		if(u32BufSize !=  CFG_Write(pstCntxTbl, &pBlockContents[u32Offset-u32BlockStart], 
								u32Offset, u32BufSize, 1, CFG_RAMWrite, CFG_RAMRead))
			{
				u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
				goto error_Case;
			}
			
	}
		
	u32ParamId = CFG_MRU_TAIL_INDEX_ID;
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
				  + u32BlockStart;
	u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl, &headIndex, u32Offset, 1,&bCheckSumValid);
	if(!bCheckSumValid)
		{
			u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
			goto error_Case;
			
		}
	memcpy(&pBlockContents[u32Offset-u32BlockStart],&headIndex,1);

	u32BufSize = 1;
	if( NULL != pu8ShadowAddress) 
	{
		if(u32BufSize != CFG_Write(pstCntxTbl, &pBlockContents[u32Offset-u32BlockStart], u32Offset, u32BufSize, 1, CFG_RAMWrite, CFG_RAMRead))
			{
				u32BufSize =(CFG_UINT32)CFG_ERR_CRITICAL;
				goto error_Case;
			}
	}
	
	
	CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));
	memcpy(&pBlockContents[u32BlockSize],&u16Chksum,sizeof(u16Chksum));
	
	
	if(CFG_EEPROMBulkWrite(pstCntxTbl,u32BlockStart,pBlockContents,u32BlockSize+sizeof(u16Chksum)) != 4)
		{
			u32BufSize = (CFG_UINT32)CFG_ERR_CRITICAL;
			goto error_Case;
		}
		

		
	/* Update the  shadow also */
	

	if( NULL != pu8ShadowAddress) {
		pstCntxTbl->pu8EEPROMShadow = pu8ShadowAddress;
		if(sizeof(u16Chksum)!= CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize, sizeof(u16Chksum), sizeof(u16Chksum), CFG_RAMWrite,CFG_RAMRead))
			{
				u32BufSize =(CFG_UINT32)CFG_ERR_CRITICAL;
				goto error_Case;
			}
		pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	}
	u32BufSize = 2+ sizeof(mruRows);
		
error_Case:
	BECEEM_MEMFREE(pBlockContents)
	
	/* No error */
	if(pu8BackupPtr)
		pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;

	return u32BufSize;
}

CFG_INT32	CFG_ResetMRU(CFG_UINT8 *pu8SharedMem)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	MRU_ROW_FORMAT mruRows[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];
	CFG_UINT8 headIndex = 0x3F;
	CFG_UINT32 dataWritten = 0;
	int iRetValue = 0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	memset(mruRows,0,sizeof(MRU_ROW_FORMAT)*MAX_CHANNEL_PLAN_REF_ID_PER_CAPL);
	if(pstCntxTbl->stBlockTbl[CFG_BLOCK_MRU].u8BlockError == (B_UINT8)CFG_ERR_CHECKSUM)
	{
		iRetValue = CFG_CorrectChecksum(pu8SharedMem, CFG_BLOCK_MRU);
		if(iRetValue < 0)
		{
			return iRetValue;
		}
	}
	if(g_bNVMTypeIsFLASH)
		return CFG_ResetMRUFlash(pu8SharedMem);

	// Reset the MRU data to Zero
	iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)mruRows, sizeof(MRU_ROW_FORMAT) * MAX_CHANNEL_PLAN_REF_ID_PER_CAPL,
					CFG_MRU_MRUDATA_ID, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
	
	// In case block is not supported e.g. for old EEPROM "iRetValue will be less than zero"
	if(iRetValue < 0)
	{
		return iRetValue;
	}
	dataWritten += iRetValue;


	iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)&headIndex, 1, CFG_MRU_HEAD_INDEX_ID,pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
	if(iRetValue < 0)
	{
		return iRetValue;
	}
	dataWritten += iRetValue;

	
	iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)&headIndex, 1, CFG_MRU_TAIL_INDEX_ID,pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
	if(iRetValue < 0)
	{
		return iRetValue;
	}
	dataWritten += iRetValue;

	return dataWritten;	
}

CFG_INT32	CFG_WriteParamMRU(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT32 dataWritten = 0;
	int iRetValue = 0;
	
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	if(CFG_BLOCK_MRU == CFG_GET_BLOCKID(u32ParamId))
	{
		// Write head and tail index
		iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)pu8Buffer,
						u32BufSize, u32ParamId, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
		if(iRetValue < 0)
		{
			return iRetValue;
		}
		dataWritten += iRetValue;
		return dataWritten;
	}

	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}

CFG_INT32	CFG_WriteMRUFlash(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT32 numofRows = 0;
	CFG_UINT32 rowOffset = 0;
	int iRetValue = 0;
	
	PST_FLASH_PARAM_ID_WRITES pstFlashParamWrite = NULL;
	ST_MRU_CHANGELIST *pstMRUList =  (ST_MRU_CHANGELIST *)pu8Buffer;
	UNUSED_ARGUMENT(u32BufSize);

	BECEEM_VALIDATEPTR(pstMRUList,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	BECEEMCALLOC_RET(pstFlashParamWrite,PST_FLASH_PARAM_ID_WRITES,ST_FLASH_PARAM_ID_WRITES,(pstMRUList->u16NumRows + 2),CFG_INT32)
	
	for(numofRows = 0; numofRows < pstMRUList->u16NumRows; numofRows++)
	{
		
		rowOffset = pstMRUList->stRowDesciptor[numofRows].u16RowIndex * sizeof(pstMRUList->stRowDesciptor[numofRows].stMruContents);

		memcpy(pstFlashParamWrite[numofRows].ucData,&pstMRUList->stRowDesciptor[numofRows].stMruContents,
			sizeof(pstMRUList->stRowDesciptor[numofRows].stMruContents));
		pstFlashParamWrite[numofRows].u32ParamID = u32ParamId ;
		pstFlashParamWrite[numofRows].u32Size = (rowOffset << 16)| sizeof(pstMRUList->stRowDesciptor[numofRows].stMruContents);
	}

	memcpy(pstFlashParamWrite[pstMRUList->u16NumRows].ucData,&pstMRUList->u8HeadIndex,
		sizeof(CFG_UINT8));
	pstFlashParamWrite[pstMRUList->u16NumRows].u32ParamID = CFG_MRU_HEAD_INDEX_ID ;
		pstFlashParamWrite[pstMRUList->u16NumRows].u32Size = sizeof(CFG_UINT8);

	memcpy(pstFlashParamWrite[pstMRUList->u16NumRows+1].ucData,&pstMRUList->u8TailIndex,
		sizeof(CFG_UINT8));
	pstFlashParamWrite[pstMRUList->u16NumRows+1].u32ParamID = CFG_MRU_TAIL_INDEX_ID ;
		pstFlashParamWrite[pstMRUList->u16NumRows+1].u32Size = sizeof(CFG_UINT8);

	iRetValue = CFG_WriteMultipleParamsToFlashSection(pu8SharedMem,pstFlashParamWrite,
				pstMRUList->u16NumRows + 2,pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
	if(iRetValue == SUCCESS)
	{
		iRetValue = 4;
	}
	else
	{
		iRetValue = -1;
	}

	BECEEM_MEMFREE(pstFlashParamWrite)
	
	return iRetValue;
}
CFG_INT32	CFG_WriteMRU(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{

	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT32 numofRows = 0;
	CFG_UINT32 rowOffset = 0;
	CFG_UINT32 dataWritten = 0;
	int iRetValue = 0;

	ST_MRU_CHANGELIST *stMRUList =  (ST_MRU_CHANGELIST *)pu8Buffer;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	if(CFG_BLOCK_MRU == CFG_GET_BLOCKID(u32ParamId))
	{
	if(g_bNVMTypeIsFLASH)
		{
			return CFG_WriteMRUFlash(pu8SharedMem,pu8Buffer,u32BufSize,u32ParamId);
		}
		
		

		// Write the modified MRU entries into the EEPROM
		for(numofRows = 0; numofRows < stMRUList->u16NumRows; numofRows++)
		{
			
			rowOffset = stMRUList->stRowDesciptor[numofRows].u16RowIndex * sizeof(stMRUList->stRowDesciptor[numofRows].stMruContents);
			
			iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)&stMRUList->stRowDesciptor[numofRows].stMruContents,
							(rowOffset << 16)| sizeof(stMRUList->stRowDesciptor[numofRows].stMruContents),
							u32ParamId ,
							pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
			// in case block is not supported for old EEPROM 
			if(iRetValue < 0)
			{
				return iRetValue;
			}
			dataWritten += iRetValue;
		}

		// Write head and tail index
		iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)&stMRUList->u8HeadIndex,
						1, CFG_MRU_HEAD_INDEX_ID, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
		if(iRetValue < 0)
		{
			return iRetValue;
		}
		dataWritten += iRetValue;

		 iRetValue = CFG_WriteParam(pstCntxTbl, (CFG_UINT8 *)&stMRUList->u8TailIndex,
						1, CFG_MRU_TAIL_INDEX_ID, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MRU]);
		if(iRetValue < 0)
		{
			return iRetValue;
		}
		dataWritten += iRetValue;


		return dataWritten;
		
	}

	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}



/*  ========================================================================== */
/*  --------------------------------- WRITE OMADM ---------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_WriteOMADM(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	if(CFG_BLOCK_OMA == CFG_GET_BLOCKID(u32ParamId)) {
		return CFG_WriteParam(pstCntxTbl, pu8Buffer, u32BufSize, u32ParamId, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_OMA]);
	}

	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}


/*  ========================================================================== */
/*  --------------------------------- WRITE Security ------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_WriteSecurity(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	if(CFG_BLOCK_SEC == CFG_GET_BLOCKID(u32ParamId)) 
	{

		return CFG_WriteParam(pstCntxTbl, pu8Buffer, u32BufSize, u32ParamId, pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_SEC]);

	}

	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}
/*  ========================================================================== */
/*  --------------------------------- WRITE VSA ------------------------------ */
/*  ========================================================================== */
CFG_INT32	CFG_WriteVSA(CFG_UINT8 *pu8SharedMem, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32VSAOffset, CFG_UINT32 u32BufSize)
{
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset  = 0;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_INT32			iRet = 0;



	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	/* Handle Legacy version */
	if(CFG_IS_LEGACY_VERSION(pstCntxTbl)) {
		return CFG_ERR_INVALID_ACCESS;
	}

	/* Check if the Block has been loaded */
	if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, CFG_VSA_VENDORSPECIFICDATA_ID)) {
		return CFG_ERR_INVALID_ACCESS;
	/* Check if buffer is long enough */
	} else if(u32VSAOffset + u32BufSize > CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, CFG_VSA_VENDORSPECIFICDATA_ID)) {
		return CFG_ERR_INVALID_SIZE;
	}

	/* Obtain the absolute location on RAM */
	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, CFG_VSA_CHECKSUM_ID);
	u32BlockStart = pstCntxTbl->stBlockTbl[CFG_BLOCK_VSA].u32BlockStartOffsets;
	u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, CFG_VSA_VENDORSPECIFICDATA_ID);
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, CFG_VSA_VENDORSPECIFICDATA_ID)
				  + CFG_USER_EXTRACT_OFFSET(u32BufSize)
				  + u32BlockStart;

#ifndef NVM_NOT_SUPPORTED
	/* Perform Bulk Writes to EEPROM*/
	if(u32BufSize == 4)
		{

			*((CFG_UINT32 *)pu8Buffer) = ntohl(*((UINT32*)pu8Buffer));
	    }
	iRet = CFG_EEPROMBulkWrite(pstCntxTbl, u32Offset+u32VSAOffset, pu8Buffer, u32BufSize);
	if(iRet != 4)
		return CFG_ERR_CRITICAL;
#else
	/* Write the data onto EEPROM */
	if(u32BufSize != CFG_Write(pstCntxTbl, pu8Buffer, u32Offset+u32VSAOffset, u32BufSize, u32Datatype, CFG_EEPROMWrite, CFG_EEPROMRead)) {
		return CFG_ERR_CRITICAL;
	}
#endif 

	return u32BufSize;
}

/*  ========================================================================== */
/*  --------------------------------- PARAM SIZE ----------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_ParamSize(CFG_UINT8 *pu8SharedMem, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	

	//Sometimes the eeprom can be screwed up so this is neccessary for safety
    if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, u32ParamId))
    {
        return CFG_ERR_PARAMID_NOT_SUPPORTED;
    }

	if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	} else if(CFG_NOT_SUPPORTED != CFG_BLOCK_OFFSETPTR(pstCntxTbl, u32ParamId)) {
		return CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId);
	}
	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}

/*  ========================================================================== */
/*  --------------------------------- PARAM TYPE ----------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_ParamType(CFG_UINT8 *pu8SharedMem, CFG_UINT32 u32ParamId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)



	if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return CFG_ERR_PARAMID_NOT_SUPPORTED;
	} else if(CFG_NOT_SUPPORTED != CFG_BLOCK_OFFSETPTR(pstCntxTbl, u32ParamId)) {
		return CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	}
	return CFG_ERR_PARAMID_NOT_SUPPORTED;
}

/*  ========================================================================== */
/*  --------------------------------- ERROR CODE ----------------------------- */
/*  ========================================================================== */
CFG_INT32	CFG_GetErrorCode(CFG_UINT8 *pu8SharedMem, CFG_UINT8 u8BlockId)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	if(u8BlockId < CFG_MAX_BLOCKS) {
		return pstCntxTbl->stBlockTbl[u8BlockId].u8BlockError;
	}

	return CFG_ERR_INVALID_ACCESS;
}


CFG_UINT32 CFG_WriteMultipleParamsToFlashSection(CFG_UINT8 *pu8SharedMem,PST_FLASH_PARAM_ID_WRITES pstFlashParamWrite,
					UINT32 u32FlashMulitpleMIBWriteCount,CFG_UINT8 *pu8ShadowAddress)
{
	CFG_CONTEXTTABLE *pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset  = 0;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT32			u32BufSize = 0;
	CFG_UINT16			u16Chksum = 0;
	CFG_UINT8			u8BlockId = 0;
	CFG_UINT8			*pBlockContents = NULL;
	CFG_UINT32 u32ParamId = 0;
	CFG_UINT32 u32Index = 0;
	INT32 iRet = SUCCESS;
	CFG_UINT32  bCheckSumValid = FALSE;

	BECEEM_VALIDATEPTR(pu8SharedMem,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)



	while(u32Index < u32FlashMulitpleMIBWriteCount)
		{

				u32ParamId = pstFlashParamWrite[u32Index].u32ParamID;
				u8BlockId = (CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
				
				/* Check if the Block has been loaded */
				if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl,u32ParamId )) {
					goto continue_till_valid_ID_is_found;
				/* Check if the paramid is valid */
				} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl,u32ParamId)) {
					goto continue_till_valid_ID_is_found;
				/* Check if buffer is long enough */
				} 
				break;
continue_till_valid_ID_is_found:
				u32Index++;
				u32ParamId = 0;
	
		}

	if(!u32ParamId)
		return (CFG_UINT32)ERROR_NO_MIB_SUPPORT;

		/* Obtain the absolute location on RAM */
		u8BlockId = (CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
		u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
		u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;
		u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	
		
			
	pstCntxTbl->pu8EEPROMShadow = pu8ShadowAddress;

	
	
	BECEEMCALLOC_RET(pBlockContents,CFG_UINT8 *,CFG_UINT8,u32BlockSize+sizeof(USHORT),CFG_UINT32)

	iRet = BeceemWiMAX_NVMrdm(u32BlockStart,pBlockContents,sizeof(u16Chksum)+u32BlockSize);

	if(iRet <= 0)
		{
			iRet = ERROR_READ_FAILED;
			goto return_status_;
		}
	memcpy(&u16Chksum,&pBlockContents[u32BlockSize],sizeof(u16Chksum));
	CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));

	for(u32Index = 0; u32Index < u32FlashMulitpleMIBWriteCount; u32Index++)
		{
				u32ParamId = pstFlashParamWrite[u32Index].u32ParamID;
				u32BufSize = pstFlashParamWrite[u32Index].u32Size;
				
				u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
				u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
								+ CFG_USER_EXTRACT_OFFSET(u32BufSize )
							  	+ u32BlockStart;
				u32BufSize	  = CFG_USER_EXTRACT_SIZE(u32BufSize);
				/* Check if the Block has been loaded */
				if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl,u32ParamId )) {
					continue;
				/* Check if the paramid is valid */
				} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl,u32ParamId)) {
					pstFlashParamWrite[u32Index].i32ReturnStatus = CFG_ERR_PARAMID_NOT_SUPPORTED;
					continue;
				/* Check if buffer is long enough */
				} 	

				memcpy(&pBlockContents[u32Offset-u32BlockStart],pstFlashParamWrite[u32Index].ucData,u32BufSize);
				CFG_Swap(&pBlockContents[u32Offset-u32BlockStart], u32BufSize,(CFG_UINT8)u32Datatype);
				u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl, pstFlashParamWrite[u32Index].ucData, u32Offset, u32BufSize,&bCheckSumValid);
				if(!bCheckSumValid)
					{
						iRet = ERROR_READ_FAILED;
						goto return_status_;
					}

					
				
		}

		CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));
		memcpy(&pBlockContents[u32BlockSize],&u16Chksum,sizeof(u16Chksum));
			
		//write contents to FLASH.
		if(CFG_EEPROMBulkWrite(pstCntxTbl,u32BlockStart,pBlockContents,(u32BlockSize + sizeof(u16Chksum))) != 4)
			{
				iRet = ERROR_EEPROM_WRITE_FAILED;
			}
			else 
			{
				iRet = SUCCESS;
			}

		

				
			
return_status_:

	for(u32Index = 0; u32Index < u32FlashMulitpleMIBWriteCount;u32Index++)
		{
			if(iRet == SUCCESS)
				{
					break;
				}
				else 
					{
						if(!pstFlashParamWrite[u32Index].i32ReturnStatus)
							pstFlashParamWrite[u32Index].i32ReturnStatus = iRet;
					}
					
		}
	
	BECEEM_MEMFREE(pBlockContents)
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	return iRet;
}

 CFG_UINT32	CFG_WriteParamForFlash(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId, CFG_UINT8 *pu8ShadowAddress)
{
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset  = 0;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT16			u16Chksum = 0;
	CFG_UINT8			u8BlockId = 0;
	CFG_UINT8			*pBlockContents = NULL;
	CFG_UINT32			u32TotalChunkToWrite = 0;
	CFG_UINT32			bCheckSumValid = FALSE;	

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	/* Check if the Block has been loaded */
	if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_INVALID_ACCESS;
	/* Check if the paramid is valid */
	} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_PARAMID_NOT_SUPPORTED;
	/* Check if buffer is long enough */
	} else if(CFG_USER_EXTRACT_SIZE(u32BufSize) > CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_INVALID_SIZE;
	}

	/* Obtain the absolute location on RAM */
	u8BlockId = (CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
	u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;
	u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
				  + CFG_USER_EXTRACT_OFFSET(u32BufSize)
				  + u32BlockStart;
	u32BufSize	  = CFG_USER_EXTRACT_SIZE(u32BufSize);


	
	BECEEMCALLOC_RET(pBlockContents,CFG_UINT8 *,CFG_UINT8,u32BlockSize+sizeof(u16Chksum),CFG_UINT32)
	
	/* Before Writing, Read the data section contents and compute differenatial checksum */
	if(BeceemWiMAX_NVMrdm(u32BlockStart,pBlockContents,sizeof(u16Chksum)+u32BlockSize) != 4)
		{
			goto error_Case;
		}

	
	if(pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)] > 0) {
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)];
	}

	memcpy(&u16Chksum,&pBlockContents[u32BlockSize],sizeof(u16Chksum));

	CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));
	
	u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl, pu8Buffer, u32Offset, u32BufSize,&bCheckSumValid);
		if(!bCheckSumValid)
		{
			goto error_Case;
		}
	
	memcpy(&pBlockContents[u32Offset-u32BlockStart],pu8Buffer,u32BufSize);

	CFG_Swap(&pBlockContents[u32Offset-u32BlockStart], u32BufSize,(CFG_UINT8) u32Datatype);
	
	CFG_Swap((CFG_UINT8 *)&u16Chksum,sizeof(u16Chksum),sizeof(u16Chksum));
	
	memcpy(&pBlockContents[u32BlockSize],&u16Chksum,sizeof(u16Chksum));
	
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	u32TotalChunkToWrite = u32BlockSize + sizeof(u16Chksum);

	if((u8BlockId!=CFG_BLOCK_MIB) || !g_u32MIBWriteFlag)
	{
		if(CFG_EEPROMBulkWrite(pstCntxTbl,u32BlockStart,pBlockContents,u32TotalChunkToWrite) != 4)
			goto error_Case;
	}	
		
		
	/* Update the  shadow also */

	if(0 != pu8ShadowAddress) {
		pstCntxTbl->pu8EEPROMShadow = pu8ShadowAddress;
		if(u32BufSize != CFG_Write(pstCntxTbl, pu8Buffer, u32Offset, 
							u32BufSize,(CFG_UINT8) u32Datatype, CFG_RAMWrite, CFG_RAMRead))
			{
				pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
				goto error_Case;
			}
		
		if(sizeof(u16Chksum)!= CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, 
				u32BlockStart + u32BlockSize, sizeof(u16Chksum), sizeof(u16Chksum), CFG_RAMWrite,CFG_RAMRead))
			{
				pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
				goto error_Case;
			}
		
	}
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;

	BECEEM_MEMFREE(pBlockContents)
	/* No error */
	return u32BufSize;

error_Case:
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	BECEEM_MEMFREE(pBlockContents)
	return (CFG_UINT32)CFG_ERR_CRITICAL;
	

}

/*  ========================================================================== */
/*  -------------------------------- Write Param ----------------------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_WriteParam(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT32 u32ParamId, CFG_UINT8 *pu8ShadowAddress)
{
	CFG_UINT8*			pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32			u32Datatype  = 0;
	CFG_UINT32			u32Offset  = 0;
	CFG_UINT32			u32BlockStart = 0;
	CFG_UINT32			u32BlockSize = 0;
	CFG_UINT16			u16Chksum = 0;
	CFG_UINT8			u8BlockId = 0;
	CFG_UINT32 			bCheckSumValid = FALSE;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	if(g_bNVMTypeIsFLASH)
		return CFG_WriteParamForFlash(pstCntxTbl,pu8Buffer,u32BufSize,u32ParamId,pu8ShadowAddress);
	
	/* Check if the Block has been loaded */
	if(CFG_NOT_SUPPORTED == CFG_BLOCK_ACTIVEPTR(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_INVALID_ACCESS;
	/* Check if the paramid is valid */
	} else if(CFG_GET_PARAMID(u32ParamId) >= CFG_BLOCK_MAX_PARAMS(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_PARAMID_NOT_SUPPORTED;
	/* Check if buffer is long enough */
	} else if(CFG_USER_EXTRACT_SIZE(u32BufSize) > CFG_PARAM_EXTRACT_SIZE(pstCntxTbl, u32ParamId)) {
		return (CFG_UINT32)CFG_ERR_INVALID_SIZE;
	}

	/* Obtain the absolute location on RAM */
	u8BlockId =(CFG_UINT8)CFG_GET_BLOCKID(u32ParamId);
	u32BlockSize  = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, ((u8BlockId << 24)|0x2));
	u32BlockStart = pstCntxTbl->stBlockTbl[u8BlockId].u32BlockStartOffsets;
	u32Datatype   = CFG_PARAM_EXTRACT_TYPE(pstCntxTbl, u32ParamId);
	u32Offset     = CFG_PARAM_EXTRACT_OFFSET(pstCntxTbl, u32ParamId)
				  + CFG_USER_EXTRACT_OFFSET(u32BufSize)
				  + u32BlockStart;
	u32BufSize	  = CFG_USER_EXTRACT_SIZE(u32BufSize);

#ifdef CFG_V4HACK_ENABLE
	if((u8BlockId == CFG_BLOCK_VSA) || (u8BlockId == CFG_BLOCK_MIB))
	{
#endif

	/* Before Writing, Read the data to compute differenatial checksum */
	if(CFG_Read(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize,
			sizeof(u16Chksum), sizeof(u16Chksum), CFG_EEPROMRead) != 1)
			return (CFG_UINT32)CFG_ERR_CRITICAL;

	if(pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)] > 0) {
		pstCntxTbl->pu8EEPROMShadow = pstCntxTbl->pu8ReLocAddress[CFG_GET_BLOCKID(u32ParamId)];
	}

	u16Chksum= u16Chksum + CFG_ComputeDiffChecksum(pstCntxTbl, pu8Buffer, u32Offset, u32BufSize,&bCheckSumValid);
	if(!bCheckSumValid)
		{
			pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;

	/* Write the data onto EEPROM */
	/* Only for Sections that are not reallocated, we are calling CFG_OptimizeWrite */
	if(u8BlockId == CFG_BLOCK_MIB)
	{
		if(!g_u32MIBWriteFlag)
		{
			if(u32BufSize != CFG_OptimizeWrite(pstCntxTbl, pu8Buffer, u32Offset, u32BufSize,(CFG_UINT8) u32Datatype, CFG_EEPROMWrite, CFG_EEPROMRead)) {
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			/* Write new checksum onto EEPROM */
			if(sizeof(u16Chksum) != CFG_OptimizeWrite(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize,
						sizeof(u16Chksum), sizeof(u16Chksum), CFG_EEPROMWrite, CFG_EEPROMRead)) {
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
		}
	}
	else
	{
		/* Write the data onto EEPROM */
		if(u32BufSize != CFG_Write(pstCntxTbl, pu8Buffer, u32Offset, u32BufSize,(CFG_UINT8) u32Datatype, CFG_EEPROMWrite, CFG_EEPROMRead)) {
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
		/* Write new checksum onto EEPROM */
		if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize,
					sizeof(u16Chksum), sizeof(u16Chksum), CFG_EEPROMWrite, CFG_EEPROMRead)) {
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
	}
#ifdef CFG_V4HACK_ENABLE
	}
#endif



	/* Update the EEPROM shadow also */

	if(0 != pu8ShadowAddress) {
		pstCntxTbl->pu8EEPROMShadow = pu8ShadowAddress;
		if(u32BufSize != CFG_Write(pstCntxTbl, pu8Buffer, u32Offset, u32BufSize, 
							(CFG_UINT8)u32Datatype, CFG_RAMWrite, CFG_RAMRead))
			{
					pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
					return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
		
		if(sizeof(u16Chksum) != CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16Chksum, u32BlockStart + u32BlockSize, 
								sizeof(u16Chksum), sizeof(u16Chksum), CFG_RAMWrite,CFG_RAMRead))
			{
				pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
								
		pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	}


	/* No error */
	return u32BufSize;
}



/*  ========================================================================== */
/*  ------------------------------ Compute Diff Checksum --------------------- */
/*  ========================================================================== */

static CFG_UINT16 CFG_ComputeDiffChecksum(CFG_CONTEXTTABLE	*pstCntxTbl, 
CFG_UINT8 *pu8Buffer,CFG_UINT32 u32Offset, CFG_UINT32 u32Size,CFG_UINT32 * pbCheckSumValid)
{
	CFG_UINT8	u8RAMData[1000];
	CFG_UINT32	u32DeltaSize = 0;
	CFG_UINT16	u16Checksum = 0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT16)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT16)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pbCheckSumValid,(CFG_UINT16)CFG_ERR_INVALID_ACCESS)

	u16Checksum = CFG_CalculateChecksum(pu8Buffer, u32Size, u16Checksum);

	while(u32DeltaSize < u32Size) {
		u32DeltaSize = (u32Size > 1000)?1000:u32Size;
		if(CFG_Read(pstCntxTbl, u8RAMData, u32Offset, 
				u32DeltaSize, 1, CFG_RAMRead) != 1)
			{
				*pbCheckSumValid = FALSE;
				return u16Checksum;
			}
		
		u16Checksum= u16Checksum - CFG_CalculateChecksum(u8RAMData, u32DeltaSize, 0);
		u32Size -= u32DeltaSize;
		u32Offset += u32DeltaSize;
		u32DeltaSize = 0;
	}
	*pbCheckSumValid = TRUE;
	return u16Checksum;
}

/*  ========================================================================== */
/*  ------------------------------ Auto Update MIB --------------------------- */
/*  ========================================================================== */
#if 0


static CFG_UINT32	CFG_CheckAndAutoUpdateMIB(CFG_CONTEXTTABLE *pstCntxTbl)
{
	CFG_UINT8			au8MIBData[1000] = {0};
	CFG_UINT32			u32MIBSize = 0;
	CFG_UINT32			iRet;
	CFG_UINT16			u16MIBChecksum = 0;
	CFG_UINT8*			pu8MIBReloc =NULL;
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

	pu8MIBReloc = pstCntxTbl->pu8ReLocAddress[CFG_BLOCK_MIB] + pstCntxTbl->stBlockTbl[CFG_BLOCK_MIB].u32BlockStartOffsets;

	/* Validate MIB Flag */
	iRet = BeceemWiMAX_rdm(pstCntxTbl->hDriver,(CFG_UINT32)pu8MIBReloc,(UINT *)au8MIBData,(INT) sizeof(UINT32));
 	if(iRet)
		return (CFG_UINT32)CFG_ERR_CRITICAL;
	if(au8MIBData[0] > 0) {
		if(au8MIBData[0] & 0x4) {
			/* Handle Difference in MIB version */
			return CFG_NOT_SUPPORTED;
		}
		/* Read default values from shadow and populate it into eeprom */
		u32MIBSize = (au8MIBData[3] << 8)|au8MIBData[2];
		au8MIBData[0] = 0x00;

        /* FIX we would overwrite the stack!!!!
           This was seen in rare instances and was happening 
		   when enabling / disabling the device
	    */
        if(u32MIBSize > sizeof(au8MIBData))
            return (CFG_UINT32)CFG_ERR_CRITICAL;

        /* Read default values from shadow */
		iRet = BeceemWiMAX_rdmBulk(pstCntxTbl->hDriver,(CFG_UINT32) pu8MIBReloc + 4, au8MIBData + 4, u32MIBSize - 4);
		/* Compute checksum */
		u16MIBChecksum = CFG_CalculateChecksum(au8MIBData, u32MIBSize, 0);
		/* Write data into EEPROM */
		if(u32MIBSize != CFG_Write(pstCntxTbl, au8MIBData, pstCntxTbl->stBlockTbl[CFG_BLOCK_MIB].u32BlockStartOffsets,
				u32MIBSize, sizeof(CFG_UINT32), CFG_EEPROMWrite, CFG_EEPROMRead))
				return (CFG_UINT32)CFG_ERR_CRITICAL;
				
		/* Write Checksum into EERPOM */
		if(sizeof(u16MIBChecksum)!= CFG_Write(pstCntxTbl, (CFG_UINT8 *)&u16MIBChecksum,
				pstCntxTbl->stBlockTbl[CFG_BLOCK_MIB].u32BlockStartOffsets + u32MIBSize,
				sizeof(u16MIBChecksum), sizeof(u16MIBChecksum), CFG_EEPROMWrite, CFG_EEPROMRead))
				return (CFG_UINT32)CFG_ERR_CRITICAL;
	}

	return (CFG_UINT32)CFG_ERR_NONE;
}
#endif

/*  ========================================================================== */
/*  ------------------------------ Calculate Checksum ------------------------ */
/*  ========================================================================== */

static CFG_UINT16	CFG_CalculateChecksum(CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Size, CFG_UINT16 u16CheckSum)
{
	CFG_UINT8	u8Temp;

	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT16)CFG_ERR_INVALID_ACCESS)


	while(u32Size--) {
		u8Temp = ~(*pu8Buffer);
		u16CheckSum= u16CheckSum + u8Temp;
		pu8Buffer++;
	}
	return u16CheckSum;
}



/*  ========================================================================== */
/*  ---------------------------------- Basic Read ---------------------------- */
/*  ========================================================================== */

static CFG_UINT8	CFG_Read(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVREAD pfnDevRead)
{
	CFG_INT32	s32Size = u32Size;
	CFG_UINT32	u32Data = 0;
	CFG_UINT8*	pu8Ptr = pu8Buffer;
	CFG_UINT32	u32TmpSize = u32Size;
	CFG_UINT8	u8AlignOff = 0;
	CFG_INT32	iRet = 0;

	//if we do not restore the shadow rom we saw problems.
    //Noticed that it caused the wrong checksum to be calculated and written when 
    //settings the certificate sec param to a certificate that was larger than 
    //1000 bytes.
    CFG_UINT8*  pu8BackupPtr = NULL;

	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT8)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT8)CFG_ERR_INVALID_ACCESS)
	pu8BackupPtr = pstCntxTbl->pu8EEPROMShadow;


    //JS.. Question, should we modify the shadow pointer here when we pfnDevRead == CFG_EEPROMRead.
    //here we are moving the alignment problem into the u8AlignOff member,  when using the CFG_RAMRead
    //this will work.  But when using CFG_EEPROMRead, we may end up w/ the incorrect offset.
    //However as long as all CFG_EEPROMRead's have pstCntxTbl->pu8EEPROMShadow 4 byte aligned then
    //it would not be an issue.
    //same question for the CFG_Write.

	u8AlignOff = (CFG_UINT8)(((UADDTYPE)pstCntxTbl->pu8EEPROMShadow)&0x3);
	if(u8AlignOff) {
		pstCntxTbl->pu8EEPROMShadow -= u8AlignOff;
		u32Offset += u8AlignOff;
	}
	u8AlignOff = (CFG_UINT8)(u32Offset&0x3);
	if(u8AlignOff)
	{
		iRet = (*pfnDevRead)(pstCntxTbl, u32Offset&~0x3, &u32Data);
		if (iRet <= 0)
		{
			pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
			return (CFG_UINT8)CFG_ERR_CRITICAL;
		}

		{
			int iTemp = 4-u8AlignOff;
			if(iTemp > s32Size) 
				iTemp = s32Size;
			u32Data = ntohl(u32Data);
			memcpy(pu8Buffer, ((unsigned char *)(&u32Data))+u8AlignOff, iTemp);
			pu8Buffer += iTemp;
			s32Size -= iTemp;
		}

		u32Offset = (u32Offset&~0x3) + 4;
	}

	
	while(s32Size > 3) {
		iRet = (*pfnDevRead)(pstCntxTbl, u32Offset, &u32Data);
		if(iRet <= 0)
		{
			pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
			return (CFG_UINT8)CFG_ERR_CRITICAL;
		}


		*(UINT *)pu8Buffer = ntohl(u32Data);



		s32Size -= 4;
		pu8Buffer += 4;
		u32Offset += 4;
	}


	if(s32Size > 0) {
		iRet = (*pfnDevRead)(pstCntxTbl, u32Offset, &u32Data);
		if (iRet <= 0)
			{
				pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
				return (CFG_UINT8)CFG_ERR_CRITICAL;
			}

		{
			PUCHAR pucTemp = (PUCHAR)(&u32Data);
			u32Data = ntohl(u32Data);
			memcpy(pu8Buffer, pucTemp, s32Size); 
			pu8Buffer += s32Size;
			s32Size = 0;
		}

	}


	
	CFG_Swap(pu8Ptr, u32TmpSize, u8DataType);
	pstCntxTbl->pu8EEPROMShadow = pu8BackupPtr;
	return 0x1;
}

/*  ========================================================================== */
/*  ---------------------------------- Basic optimized Write ----------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_OptimizeWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVWRITE pfnDevWrite, CFG_DEVREAD pfnDevRead)
{
	CFG_INT32	s32Size = u32Size;
	CFG_UINT32	u32Data = 0;
	CFG_UINT32  u32temp = 0;
	CFG_UINT8	u8AlignOff = 0;
	CFG_UINT8*	u8TmpPtr = pu8Buffer;
	CFG_UINT8*	u8ShadowTemp = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32	u32TmpSize = u32Size;
	CFG_INT32	iRet = 0;
	CFG_UINT8	u8DataMatched = 0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	CFG_Swap(pu8Buffer, u32Size, u8DataType);

	u8AlignOff = (CFG_UINT8)(((UADDTYPE)pstCntxTbl->pu8EEPROMShadow)&0x3);
	if(u8AlignOff) {
		pstCntxTbl->pu8EEPROMShadow -= u8AlignOff;
		u32Offset += u8AlignOff;
	}

	/* All operation below assumes data is in Little Endian */
	u8AlignOff = (CFG_UINT8)(u32Offset&0x3);
	if(u8AlignOff) {

		CFG_UINT8 *pu8DupBuffer = pu8Buffer;
		CFG_UINT8 u8DupAlignOff = u8AlignOff;
		CFG_INT32 s32DupSize = s32Size;
		u8DataMatched = 0;

		if(pfnDevWrite == &CFG_EEPROMWrite)
		{
			CFG_UINT32 u32ShadowData = 0;
			// Read the data from EEPROM
			iRet = CFG_RAMRead(pstCntxTbl, u32Offset&~0x3, &u32ShadowData);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			u32Data = u32ShadowData;


			{
				PUCHAR pucTemp = (unsigned char *)(&u32Data);
				CFG_INT32 u32TempField = 4-u8AlignOff;
				if(u32TempField > s32Size) {
					u32TempField = s32Size;
				}
				u32Data = ntohl(u32Data);
				memcpy(pucTemp+u8AlignOff, pu8Buffer, u32TempField);
				u32Data = ntohl(u32Data);
				pu8Buffer+=u32TempField;
				u8AlignOff=u8AlignOff+(CFG_UINT8)u32TempField;
				s32Size-=u32TempField;
			}
				


			// Compare the EEPROM shadow data with data we need to write
			if(u32ShadowData == u32Data)
			{
				// increment the offset as data is same 
				// No need to write into EEPROM
				u8DataMatched = 1;
				u32Offset = (u32Offset&~0x3) + 4;
			}
		}
		if(!u8DataMatched)
		{
			pu8Buffer = pu8DupBuffer;
			u8AlignOff = u8DupAlignOff;
			s32Size = s32DupSize ;

			iRet = (*pfnDevRead)(pstCntxTbl, u32Offset&~0x3, &u32Data);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}

			{
				PUCHAR pucTemp = (PUCHAR)(&u32Data);
				CFG_INT32  sz = 4-u8AlignOff;
				if(sz > s32Size) {
					sz = s32Size;
				}
				u32Data = ntohl(u32Data);
				memcpy(pucTemp+u8AlignOff, pu8Buffer, sz);
				u32Data = ntohl(u32Data);
				pu8Buffer+=sz;
				u8AlignOff=u8AlignOff+(CFG_UINT8)sz;
				s32Size-=sz;
			}
				

			iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset&~0x3, u32Data);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			u32Offset = (u32Offset&~0x3) + 4;
		}
	}

		

	if((pfnDevWrite == &CFG_EEPROMWrite) && (gu32AccEEPROM))
	{
		// Align it to 16 bytes
		if(s32Size > 15)
		{
			while(u32Offset % 16 != 0)
			{
	
				u32Data = *((PUINT)pu8Buffer);
				u32Data = ntohl(u32Data);

				iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
				if(iRet <= 0) {
					pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
					return (CFG_UINT32)CFG_ERR_CRITICAL;
				}
				s32Size -= 4;
				pu8Buffer += 4;
				u32Offset += 4;
			}
		}

		while(s32Size > 15) {
			iRet = CFG_EEPROMBulkWrite(pstCntxTbl, u32Offset, pu8Buffer, 16);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			s32Size -= 16;
			pu8Buffer += 16;
			u32Offset += 16;
		}
	}

	while(s32Size > 3) {
		u8DataMatched = 0;
	
		u32Data = *((PUINT)pu8Buffer);
		u32Data = ntohl(u32Data);


		if(pfnDevWrite == &CFG_EEPROMWrite)
		{
			CFG_UINT32 u32ShadowData = 0;
			// Read the data from EEPROM
			iRet = CFG_RAMRead(pstCntxTbl, u32Offset&~0x3, &u32ShadowData);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			if(u32ShadowData == u32Data)
			{
				// increment the offset as data is same 
				// No need to write into EEPROM
				u8DataMatched = 1;
				s32Size -= 4;
				pu8Buffer += 4;
				u32Offset += 4;
			}
		}
		if(!u8DataMatched)
		{
			iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			s32Size -= 4;
			pu8Buffer += 4;
			u32Offset += 4;
		}
	}

	if(s32Size > 0) {
		u8DataMatched = 0;
		if(pfnDevWrite == &CFG_EEPROMWrite)
		{
			CFG_UINT32 u32ShadowData = 0;
			// Read the data from EEPROM
			iRet = CFG_RAMRead(pstCntxTbl, u32Offset&~0x3, &u32ShadowData);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			u32Data = u32ShadowData;
	
			u32temp = *((PUINT)pu8Buffer);
			u32temp = ntohl(u32temp);

			u32Data = (u32temp & (0xFFFFFFFF << ((4 - s32Size)*8))) | (u32Data & (0xFFFFFFFF >> (s32Size*8)));
		
			if(u32Data == u32ShadowData)
			{
				u8DataMatched = 1;
			}
		}
		if(!u8DataMatched)
		{
			iRet = (*pfnDevRead)(pstCntxTbl, u32Offset, &u32Data);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}


			u32temp = *((PUINT)pu8Buffer);
			u32temp = ntohl(u32temp);

			u32Data = (u32temp & (0xFFFFFFFF << ((4 - s32Size)*8))) | (u32Data & (0xFFFFFFFF >> (s32Size*8)));
			iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
		}
	}


	/* Revert Data back to the Endian it was */
	CFG_Swap(u8TmpPtr, u32TmpSize, u8DataType);


	pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
	return u32TmpSize;
}




/*  ========================================================================== */
/*  ---------------------------------- Basic Write --------------------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_Write(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT8 *pu8Buffer, CFG_UINT32 u32Offset, CFG_UINT32 u32Size, CFG_UINT8 u8DataType, CFG_DEVWRITE pfnDevWrite, CFG_DEVREAD pfnDevRead)
{
	CFG_INT32	s32Size = u32Size;
	CFG_UINT32	u32Data = 0;
	CFG_UINT32  u32temp = 0;
	CFG_UINT8	u8AlignOff = 0;
	CFG_UINT8*	u8TmpPtr = pu8Buffer;
	CFG_UINT8*	u8ShadowTemp = pstCntxTbl->pu8EEPROMShadow;
	CFG_UINT32	u32TmpSize = u32Size;
	CFG_INT32	iRet = 0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Buffer,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)


	CFG_Swap(pu8Buffer, u32Size, u8DataType);

	u8AlignOff = (CFG_UINT8)(((UADDTYPE)pstCntxTbl->pu8EEPROMShadow)&0x3);
	if(u8AlignOff) {
		pstCntxTbl->pu8EEPROMShadow -= u8AlignOff;
		u32Offset += u8AlignOff;
	}

	/* All operation below assumes data is in Little Endian */
	u8AlignOff = (CFG_UINT8)(u32Offset&0x3);
	if(u8AlignOff) {
		iRet = (*pfnDevRead)(pstCntxTbl, u32Offset&~0x3, &u32Data);
		if(iRet <= 0) {
			pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}

		{
			char *ch = (char *)(&u32Data);
			CFG_INT32 sz = 4-u8AlignOff;
			if(sz > s32Size) {
				sz = s32Size;
			}
			u32Data = ntohl(u32Data);
			memcpy(ch+u8AlignOff, pu8Buffer, sz);
			u32Data = ntohl(u32Data);
			pu8Buffer+=sz;
			u8AlignOff=u8AlignOff+(CFG_UINT8)sz;
			s32Size-=sz;
		}


		iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset&~0x3, u32Data);
		if(iRet <= 0) {
			pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
		u32Offset = (u32Offset&~0x3) + 4;
	}

		

	if((pfnDevWrite == &CFG_EEPROMWrite) && (gu32AccEEPROM))
	{
#ifndef NVM_NOT_SUPPORTED
		if(s32Size > 15)
		{
			iRet = CFG_EEPROMBulkWrite(pstCntxTbl, u32Offset, pu8Buffer, s32Size);

			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			s32Size = 0;
			u32Offset= 0;
		}
#endif
		// Align it to 16 bytes
		if(s32Size > 15)
		{
			while(u32Offset % 16 != 0)
			{
	
				u32Data = *((PUINT)pu8Buffer);
				u32Data = ntohl(u32Data);

				iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
				if(iRet <= 0) {
					pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
					return (CFG_UINT32)CFG_ERR_CRITICAL;
				}
				s32Size -= 4;
				pu8Buffer += 4;
				u32Offset += 4;
			}
		}

		while(s32Size > 15) {
			iRet = CFG_EEPROMBulkWrite(pstCntxTbl, u32Offset, pu8Buffer, 16);
			if(iRet <= 0) {
				pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
			s32Size -= 16;
			pu8Buffer += 16;
			u32Offset += 16;
		}

	}

	while(s32Size > 3) {
		
		u32Data = *((PUINT)pu8Buffer);
		u32Data = ntohl(u32Data);
		iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
		if(iRet <= 0) {
			pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
		s32Size -= 4;
		pu8Buffer += 4;
		u32Offset += 4;
	}

	if(s32Size > 0) {
		iRet = (*pfnDevRead)(pstCntxTbl, u32Offset, &u32Data);
		if(iRet <= 0) {
			pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}	
		u32temp = *((PUINT)pu8Buffer);
		u32temp = ntohl(u32temp);

		u32Data = (u32temp & (0xFFFFFFFF << ((4 - s32Size)*8))) | (u32Data & (0xFFFFFFFF >> (s32Size*8)));
		iRet = (*pfnDevWrite)(pstCntxTbl, u32Offset, u32Data);
		if(iRet <= 0) {
			pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
			return (CFG_UINT32)CFG_ERR_CRITICAL;
		}
	}


	/* Revert Data back to the Endian it was */
	CFG_Swap(u8TmpPtr, u32TmpSize, u8DataType);


	pstCntxTbl->pu8EEPROMShadow = u8ShadowTemp;
	return u32TmpSize;
}


/*  ========================================================================== */
/*  -------------------------------- Device Read ----------------------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_EEPROMRead(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 *pu32Data)
{
	CFG_UINT32	u32Retry = 0x5;
	CFG_UINT32	iRet = 0;

#ifndef NVM_NOT_SUPPORTED
	UNUSED_ARGUMENT(pstCntxTbl);
#endif

	
#ifdef CFG_V4HACK_ENABLE
	return CFG_RAMRead(pstCntxTbl, u32Offset, pu32Data);
#endif

	do {
		#ifdef NVM_NOT_SUPPORTED
			iRet = BeceemWiMAX_HostEEPROMRead(pstCntxTbl->hDriver, u32Offset, pu32Data);
		#else
			iRet = BeceemWiMAX_NVMrdm(u32Offset,(PUCHAR)pu32Data,sizeof(CFG_UINT32));
		#endif
	} while(u32Retry-- && iRet != 4);
	
	
	return iRet;
}


static CFG_UINT32	CFG_RAMRead(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 *pu32Data)
{
	CFG_UINT32	u32Retry = 0x5;
	CFG_UINT32	iRet = 0;
    CFG_UINT32  u32Address=0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	u32Address = (0xFFFFFFFF & (UADDTYPE)pstCntxTbl->pu8EEPROMShadow);
	u32Address+=u32Offset;
	do {
		iRet =  BeceemWiMAX_rdm(pstCntxTbl->hDriver, u32Address, pu32Data, sizeof(UINT32));
	} while(u32Retry-- && iRet != 0);
	if (iRet == 0)
		return 0x4;
	
	return iRet;
}


/*  ========================================================================== */
/*  -------------------------------- Device Write ---------------------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_EEPROMWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 u32Data)
{
	CFG_UINT32	u32Retry = 0x5;
	CFG_UINT32	iRet = 0;

#ifndef NVM_NOT_SUPPORTED
	UNUSED_ARGUMENT(pstCntxTbl);
#endif
	/* This write will read back and verify the data */
	do {
		#ifdef NVM_NOT_SUPPORTED
			iRet = BeceemWiMAX_HostEEPROMWrite(pstCntxTbl->hDriver, u32Offset, u32Data);
		#else
			iRet = BeceemWiMAX_NVMwrm(u32Offset,(PUCHAR)&u32Data,sizeof(CFG_UINT32));
		#endif
	} while(u32Retry-- && iRet != 4);


	return iRet;
}

/*  ========================================================================== */
/*  -------------------------------- Device Write ---------------------------- */
/*  ========================================================================== */
static CFG_UINT32	CFG_EEPROMBulkWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT8 * pu8Buffer, CFG_UINT32 u32Size)
{
	CFG_UINT32	u32Retry = 0x5;
	CFG_UINT32	iRet = 0;
#ifndef NVM_NOT_SUPPORTED
	UNUSED_ARGUMENT(pstCntxTbl);
#endif
	/* This write will read back and verify the data */
	do {
		#ifdef NVM_NOT_SUPPORTED
		iRet = BeceemWiMAX_HostEEPROMBulkWrite(pstCntxTbl->hDriver, u32Offset, pu8Buffer, u32Size);
		#else
			iRet = BeceemWiMAX_NVMwrm(u32Offset,pu8Buffer,u32Size);
			if(iRet == 4)/*For backward compatibility purpose. Plz don;t get into the logic of why it is so :-)*/
				{
					iRet = 0;
				}
		#endif
	} while(u32Retry-- && iRet != 0);
	if (iRet == 0)
		return 0x4;
	
	return iRet;
}

static CFG_UINT32	CFG_RAMWrite(CFG_CONTEXTTABLE *pstCntxTbl, CFG_UINT32 u32Offset, CFG_UINT32 u32Data)
{
	CFG_UINT32	u32Retry = 0x5;
	CFG_UINT32	iRet = 0;
    CFG_UINT32 u32Address=0;
	
	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)

   	u32Address = (0xFFFFFFFF & (UADDTYPE)pstCntxTbl->pu8EEPROMShadow);
	u32Address+=u32Offset;

	do {
		iRet = BeceemWiMAX_wrm(pstCntxTbl->hDriver, u32Address, u32Data);
	} while(u32Retry-- && iRet != 0);
	if (iRet == 0)
		return 0x4;
	
	return iRet;
}


/*  ========================================================================== */
/*  -------------------------------- Swap Endian ----------------------------- */
/*  ========================================================================== */
static CFG_VOID		CFG_Swap(CFG_UINT8 *pu8Buffer, CFG_UINT32 u32BufSize, CFG_UINT8 u8DataType)
{


    if (!pu8Buffer)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_RX, "\n Invalid Memory Access @ function %s and Line %d",
		   __FUNCTION__, __LINE__);
	}

	
	if(0 == u8DataType || (u8DataType & 0x1)){
		return;
	}
		

	if( (u8DataType!=2) && (u8DataType!=4) ) {
		printf( "ERROR: Undefined data type: %d\n", u8DataType);
	}

	if(u32BufSize % u8DataType) {
		printf( "ERROR: Invalid Swap len: %d Data type: %d\n", u32BufSize, u8DataType);
	}

	while(u32BufSize){
		if(u8DataType==2) {	
			*(unsigned short *)pu8Buffer = ntohs((*(unsigned short *)pu8Buffer));
		} else {
			*(unsigned int *)pu8Buffer = ntohl((*(unsigned int *)pu8Buffer));
		}		
		pu8Buffer += u8DataType;
		u32BufSize -= u8DataType;
	}

}

/*  ========================================================================== */
/*  ------------------------------- Legacy Init  ----------------------------- */
/*  ========================================================================== */
#define CFG_LEGACY_MIBS_ADDRESS							0xBF60AC80
#define CFG_LEGACY_VSRO_SHADOW_LOC						0xBF60D37C
#define CFG_LEGACY_EXVSRO_SHADOW_LOC					0xBF60E000
#define CFG_LEGACY_EXVSRO_SIZE							0xA00
#define CFG_LEGACY_VSRO_DEFAULT_SIZE					0x1BF
#define CFG_LEGACY_VSRO_SIZE_LIST						{0x0, 0x1B5, 0x1BF, 0xA00}

static CFG_INT32	CFG_LegacyInit(CFG_LEGACYCONTEXTTABLE *pstCntxTbl)
{
	CFG_UINT32 u32VSROShadowLoc = CFG_LEGACY_VSRO_SHADOW_LOC;
	CFG_UINT32 u32VSROSize = CFG_LEGACY_VSRO_DEFAULT_SIZE;
	CFG_UINT32 u32Index = 0;
	CFG_UINT32 u32Temp = 0;
	int iRet = 0;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	/* Mark this as Legacy version */
	pstCntxTbl->stContextTbl.u32OverallMajorVersion = 1;

	iRet = BeceemWiMAX_rdm(pstCntxTbl->stContextTbl.hDriver, CFG_LEGACY_EXVSRO_SHADOW_LOC, &u32Temp, sizeof(UINT32));
	if (iRet < 0)
		return 0;

	if(u32Temp == 0xDFEA9CBA) {
		/* Update the VSROShadowLoc based on what needs to be read */
		u32VSROShadowLoc = CFG_LEGACY_EXVSRO_SHADOW_LOC + 4;
		u32VSROSize = CFG_LEGACY_EXVSRO_SIZE;
		pstCntxTbl->u32VSROVersion = 3;
	}

	while(u32Index < u32VSROSize) {
		iRet = BeceemWiMAX_rdm(pstCntxTbl->stContextTbl.hDriver, u32VSROShadowLoc, &u32Temp, sizeof(UINT32));
		if (iRet < 0)
			return 0;

		u32Temp = ntohl(u32Temp);
		*((CFG_UINT32 *)(pstCntxTbl->au8VSROData + u32Index)) = u32Temp;
		u32Index += 4;

		u32VSROShadowLoc += 4;
	}

	if(u32Temp != 0xDFEA9CBA) {
		/* Capture the version number of VSRO */
		pstCntxTbl->u32VSROVersion = pstCntxTbl->au8VSROData[2];
	}

	/* Unconditionally load MIB, as default values are populated by the firmware */
	pstCntxTbl->stContextTbl.stBlockTbl[CFG_BLOCK_MIB].pstActiveOffsetTables
				= &CFG_BlockVersionList[CFG_BLOCK_MIB][pstCntxTbl->stContextTbl.stBlockTbl[CFG_BLOCK_MIB].u8BlockVersion];
	return pstCntxTbl->stContextTbl.u32InitBitField;
}

static CFG_UINT32	CFG_LegacyReadParam(void *pstCtxTbl, CFG_UINT8 *pu8Data, CFG_UINT32 u32DataSize, CFG_UINT32 u32ParamId)
{
	CFG_LEGACYCONTEXTTABLE  *pstCntxTbl = (CFG_LEGACYCONTEXTTABLE  *)pstCtxTbl;
	CFG_UINT8* u8TempShadow = pstCntxTbl->stContextTbl.pu8EEPROMShadow;
	CFG_UINT32 u32Datatype;
	CFG_UINT32 u32Offset = CFG_USER_EXTRACT_OFFSET(u32DataSize);
	CFG_UINT32 u32Size = CFG_USER_EXTRACT_SIZE(u32DataSize);
	CFG_UINT32 u32VSROSizes[] = CFG_LEGACY_VSRO_SIZE_LIST;

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Data,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	


	if(CFG_VSA_VENDORSPECIFICDATA_ID == u32ParamId && u32Offset < u32VSROSizes[pstCntxTbl->u32VSROVersion]) {
		if(u32Offset + u32Size > u32VSROSizes[pstCntxTbl->u32VSROVersion]) {
			/* truncating to the maxsize */
			u32Size = u32VSROSizes[pstCntxTbl->u32VSROVersion] - u32Offset;
		}
		if(pstCntxTbl->u32VSROVersion < 3) {
			/* Skip 2 bytes Block Size, 1 byte Block Version */
			u32Offset += 3;
		}
		memcpy(pu8Data, pstCntxTbl->au8VSROData + u32Offset, u32Size);
		return u32Size;
	} else if(CFG_VSA_LENGTH_ID == u32ParamId) {
		pu8Data[1] = (CFG_UINT8)(u32VSROSizes[pstCntxTbl->u32VSROVersion] >> 8);
		pu8Data[0] = (CFG_UINT8)(u32VSROSizes[pstCntxTbl->u32VSROVersion]&0xFF);
		return 2;
	} else if(u32ParamId >= CFG_MIB_LENGTH_ID && u32ParamId <= CFG_MIB_CONTENT_VERSION_ID) {
		u32Datatype   = CFG_PARAM_EXTRACT_TYPE(((CFG_CONTEXTTABLE *)pstCntxTbl), u32ParamId);
		u32Offset     = CFG_PARAM_EXTRACT_OFFSET((&pstCntxTbl->stContextTbl), u32ParamId);
		if(u32DataSize > CFG_PARAM_EXTRACT_SIZE((&pstCntxTbl->stContextTbl), u32ParamId)) {
			u32DataSize = CFG_PARAM_EXTRACT_SIZE((&pstCntxTbl->stContextTbl), u32ParamId);
		}
		pstCntxTbl->stContextTbl.pu8EEPROMShadow = pstCntxTbl->stContextTbl.pu8ReLocAddress[CFG_BLOCK_MIB];
		if(CFG_Read((CFG_CONTEXTTABLE *)pstCtxTbl, pu8Data, u32Offset, u32DataSize, (CFG_UINT8)u32Datatype, CFG_RAMRead) != 1)
			{
				pstCntxTbl->stContextTbl.pu8EEPROMShadow = u8TempShadow;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
		
		pstCntxTbl->stContextTbl.pu8EEPROMShadow = u8TempShadow;
		return u32DataSize;
	}

	return (CFG_UINT32)CFG_ERR_INVALID_ACCESS;
}


static CFG_UINT32	CFG_LegacyWriteMIBOnShadow(void *pstCtxTbl, CFG_UINT8 *pu8Data, CFG_UINT32 u32DataSize, CFG_UINT32 u32ParamId)
{
	CFG_LEGACYCONTEXTTABLE  *pstCntxTbl = (CFG_LEGACYCONTEXTTABLE  *)pstCtxTbl;
	CFG_UINT8* u8TempShadow = pstCntxTbl->stContextTbl.pu8EEPROMShadow;
	CFG_UINT32 u32Datatype;
	CFG_UINT32 u32Offset = CFG_USER_EXTRACT_OFFSET(u32DataSize);
	CFG_UINT32 u32Size = CFG_USER_EXTRACT_SIZE(u32DataSize);

	BECEEM_VALIDATEPTR(pstCntxTbl,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	BECEEM_VALIDATEPTR(pu8Data,(CFG_UINT32)CFG_ERR_INVALID_ACCESS)
	

	if(u32ParamId >= CFG_MIB_LENGTH_ID && u32ParamId <= CFG_MIB_CONTENT_VERSION_ID) {
		u32Datatype   = CFG_PARAM_EXTRACT_TYPE(((CFG_CONTEXTTABLE *)pstCntxTbl), u32ParamId);
		u32Offset     = CFG_PARAM_EXTRACT_OFFSET((&pstCntxTbl->stContextTbl), u32ParamId);
		if(u32DataSize > CFG_PARAM_EXTRACT_SIZE((&pstCntxTbl->stContextTbl), u32ParamId)) {
			u32DataSize = CFG_PARAM_EXTRACT_SIZE((&pstCntxTbl->stContextTbl), u32ParamId);
		}
		pstCntxTbl->stContextTbl.pu8EEPROMShadow = pstCntxTbl->stContextTbl.pu8ReLocAddress[CFG_BLOCK_MIB];
		if(u32DataSize != CFG_Write((CFG_CONTEXTTABLE *)pstCtxTbl, pu8Data, 
						u32Offset, u32DataSize,(CFG_UINT8) u32Datatype, CFG_RAMWrite, CFG_RAMRead))
			{
				pstCntxTbl->stContextTbl.pu8EEPROMShadow = u8TempShadow;
				return (CFG_UINT32)CFG_ERR_CRITICAL;
			}
		pstCntxTbl->stContextTbl.pu8EEPROMShadow = u8TempShadow;

		return u32Size;
	}

	return (CFG_UINT32)CFG_ERR_INVALID_ACCESS;
}

CFG_VOID CFG_SetMIBWriteOption(CFG_UINT32 u32SetMIBOption)
{
	g_u32MIBWriteFlag = u32SetMIBOption;
}

CFG_VOID CFG_ReadVersion(CFG_UINT8 * pu8SharedMem, CFG_UINT32 * pu32Buffer)
{
	CFG_CONTEXTTABLE*	pstCntxTbl = (CFG_CONTEXTTABLE *)pu8SharedMem;
	*pu32Buffer=pstCntxTbl->u32OverallMajorVersion;
	pu32Buffer++;
	*pu32Buffer=pstCntxTbl->stBlockTbl[0].u8BlockVersion;
	return;
}


