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
 * Description	:	MIB set / get handlers.
 * Author		:
 * Reviewer		:
 *
 */



#include "BeceemWiMAX.h"
#include "HostMIBSInterface.h"
#include "config_paramsSS.h"
#include "uti_BeceemEEPROMCfg.h"
#include "mruEEPROM.h"
#include "WiMAXCommonAPI.h"
#include "ProxyEndianHandler.h"
#include "globalobjects.h"
extern UINT8 gCheckSumErrorFlag;
int BeceemWiMAX_MIBReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
    {
		case SUBTYPE_MIB_SET_REQ:
			return BeceemWiMAX_ProcessMIBSetReq(pWiMAX, pMsg, uiLength);

		case SUBTYPE_MIB_GET_REQ:
			return BeceemWiMAX_ProcessMIBGetReq(pWiMAX, (PMIB_OBJECT_INFO)pMsg->szData);

		case SUBTYPE_MULTIPLE_MIB_SET_REQ:
			return BeceemWiMAX_ProcessMultipleMIBSetReq(pWiMAX, pMsg, uiLength);

		case SUBTYPE_MULTIPLE_MIB_GET_REQ:
			return BeceemWiMAX_ProcessMultipleMIBGetReq(pWiMAX, (PMIB_OBJECTS)pMsg->szData,FALSE);
		case SUBTYPE_MULTIPLE_MIB_PERIODIC_INDICATION_REQ:
			return BeceemWiMAX_ProcessMultiplePeriodicIndicationReq(pWiMAX,
							    (PMIB_OBJECTS_TIMER) pMsg->
							    szData);
	}
	return 0;
}

int GetDlUlZoneInformation(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0, iPerZoneInfoOffset = 0;

	INT32 uiBaseAddress = 0;
	INT32 uiStatsPtrBaseAddress = 0;
	
	INT32 OID=0;
	INT32 FieldSize=0;

	OID=pMIBObj->u32MibObjId;

	
	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS,(UINT*) &uiStatsPtrBaseAddress, sizeof(INT32));
		if(iRet)
			return iRet;
	
		if(!uiStatsPtrBaseAddress)
		{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
			return ERROR_NO_STATISTIC_RESPONSE;
		}


	switch(OID)
		{

		case BCM_STATS_DL_ZONE_INFO:
			if(!(pMIBObj->au32Key[0]>=0 && pMIBObj->au32Key[0]<=NUMBER_OF_DL_ZONE_TYPES))
			{

		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Key Value!!!");
				return ERROR_INVALID_MIB_ATTRIBUTE;
			}
			break;

		case BCM_STATS_UL_ZONE_INFO:
			if(!(pMIBObj->au32Key[0]>=0 && pMIBObj->au32Key[0]<=NUMBER_OF_UL_ZONE_TYPES))
			{
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Key Value!!!");
				return ERROR_INVALID_MIB_ATTRIBUTE;
			}
			break;
				
		}
	
	
	FieldSize=pWiMAX->m_sMIBTable[OID].u32FieldSize ;
	uiBaseAddress=uiStatsPtrBaseAddress+pWiMAX->m_sMIBTable[OID].u32Offset;		 
	iPerZoneInfoOffset=FieldSize*pMIBObj->au32Key[0];

	
	

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiBaseAddress+iPerZoneInfoOffset, (PUCHAR)pucOutData, FieldSize);
	if(iRet)
		return iRet;


	*puiOutDataLen = FieldSize;

    return SUCCESS;
	
	
}

int ValidateOID(UINT uiOID)
{
	if(uiOID >= BCM_STANDARD_CFG_MIB_BASE_START && uiOID <= BCM_STANDARD_CFG_MIB_BASE_END)
	{
		/* STANDARD CONFIG MIBS */
		return SUCCESS;
	}
	else if(uiOID >= BCM_PROPREITARY_CFG_MIB_BASE_START && uiOID <= BCM_PROPREITARY_CFG_MIB_BASE_END)
	{
		/* BCM PROPREITARY CONFIG MIBS */
		return SUCCESS;
	}
	else if(uiOID >= BCM_STANDARD_STATS_MIB_BASE_START && uiOID <= BCM_STANDARD_STATS_MIB_BASE_START+299)
	{
		/* STANDARD STATS MIBS */
		return SUCCESS;
	}
	else if(uiOID >= BCM_STANDARD_STATS_MIB_BASE_START+300 && uiOID <= BCM_STANDARD_STATS_MIB_BASE_END)
	{
		/* STANDARD HOST STATS MIBS */
		return SUCCESS;
	}
	else if(uiOID >= BCM_PROPREITARY_STATS_MIB_BASE_START && uiOID <= BCM_PROPREITARY_STATS_MIB_BASE_END)
	{
		/* BCM PROPREITARY STATS MIBS */
		return SUCCESS;
	}

	return ERROR_MIB_OBJECT_NOTFOUND;
}

int BeceemWiMAX_ProcessMIBGetReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj)
{
	PUCHAR pucOutBuffer = NULL;
	PWIMAXMESSAGE pWMMsg = NULL;
	UINT uiAttr = 0;
	int iSize = MAX_BUFFER_LENGTH, iRet = 0;	
	pucOutBuffer = (PUCHAR)calloc(1,MAX_BUFFER_LENGTH);
	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);

	if(!pucOutBuffer || !pWMMsg)
		{
			BECEEM_MEMFREE(pucOutBuffer)
			BECEEM_MEMFREE(pWMMsg)
			return ERROR_MALLOC_FAILED;
		}
	iRet = GetMIBData(pWiMAX, pMIBObj, pucOutBuffer, (UINT *)&iSize);

	if(iRet < 0)
	{
		pMIBObj->u32MibObjSize=0;
		BECEEM_MEMFREE(pucOutBuffer)
		BECEEM_MEMFREE(pWMMsg)
		return iRet;
	}
		
	memcpy(pMIBObj->au8MibObjVal, pucOutBuffer, iSize);

	pMIBObj->u32MibObjSize = iSize;

	uiAttr = pWiMAX->m_sMIBTable[pMIBObj->u32MibObjId].u16MIBAttribute;

	if(BIT_TGT_STD_CONFIG == (uiAttr & 0xFF00))
	{
		switch(pMIBObj->u32MibObjId)
		{
		case WMAN_IF_SS_LOSTDLMAPINTERVAL:
		case WMAN_IF_SS_LOSTULMAPINTERVAL:
		case WMAN_IF_SS_T1TIMEOUT:
		case WMAN_IF_SS_T2TIMEOUT:
		case WMAN_IF_SS_T3TIMEOUT:
		case WMAN_IF_SS_T6TIMEOUT:
		case WMAN_IF_SS_T12TIMEOUT:
		case WMAN_IF_SS_T14TIMEOUT:
		case WMAN_IF_SS_T18TIMEOUT:
		case WMAN_IF_SS_T20TIMEOUT:
		case WMAN_IF_SS_T21TIMEOUT:
		case WMAN_IF_CMN_T7TIMEOUT:
		case WMAN_IF_CMN_T8TIMEOUT:
		case WMAN_IF_CMN_T10TIMEOUT:
		case WMAN_IF_CMN_T22TIMEOUT:
		case WMAN_IF_2M_SS_T44TIMER:	
			if(pMIBObj->u32MibObjSize == sizeof(USHORT))
			{
				*(USHORT *)pMIBObj->au8MibObjVal= (*(USHORT *)pMIBObj->au8MibObjVal)*5;
			}
			else if(pMIBObj->u32MibObjSize == sizeof(UCHAR))
			{
				*(UCHAR  *)pMIBObj->au8MibObjVal = (*(UCHAR *)pMIBObj->au8MibObjVal)*5;
			}
			else
			{
				*(UINT  *)pMIBObj->au8MibObjVal = *(UINT *)pMIBObj->au8MibObjVal * 5;
			}


			break;

		case WMAN_IF_SS_T4TIMEOUT:
		case WMAN_IF_SS_CONTENTIONRANGRETRIES:
		case WMAN_IF_SS_REQUESTRETRIES:
		case WMAN_IF_SS_REGREQUESTRETRIES:
		case WMAN_IF_SS_SBCREQUESTRETRIES:
		case WMAN_IF_CMN_DSXREQRETRIES:
		case WMAN_IF_CMN_DSXRESPRETRIES:
		case WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG:
		case WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG:
		case WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG:
		case WMAN_IF_SS_INVITEDRANGRETRIES:
			memcpy(pMIBObj->au8MibObjVal,pMIBObj->au8MibObjVal,pMIBObj->u32MibObjSize);
			break;
		default:
			break;
		}
	}

		WiMAXMessage_Init(
						pWMMsg,
						TYPE_MIB, 
						SUBTYPE_MIB_GET_RESP, 
						(UCHAR*)pMIBObj,
					sizeof(MIB_OBJECT_INFO)+iSize-1
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		
	BECEEM_MEMFREE(pucOutBuffer)
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int WriteStandardConfigMIB(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,UINT32 bMultipleMIBSetCaller)
{
	UINT uiAttr = 0;
	UINT uiSize = 0;
	UINT uiOffset = 0;
	UINT uiTableOffset = 0;
	UINT uiNumRows = 0;
	UINT uiRowSize = 0;
	UINT usNumIndex = 0;
	UINT uiType = 0;
	PUCHAR pucData = NULL;
	UINT szData=1024;
	UINT uiSizeToWrite = 0;
	int iRetval=0;
	int iSubOffset = 0;
	UINT uiValue = 0;
	UINT uiMIBValue = 0;
	int iOID = pMIBObj->u32MibObjId;
	int iRet = ValidateOID(iOID);
	if(iRet)
	return iRet;

	BECEEMCALLOC(pucData,PUCHAR,char,szData)
	
	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
	uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
	uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
	uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;
	uiType = pWiMAX->m_sMIBTable[iOID].u8FieldType;


	if  (pMIBObj->u32MibObjSize != uiSize)
    {   
		iRetval=ERROR_INVALID_MIB_OBJ_SIZE;
        goto returnCode;

    }
	
	usNumIndex = uiAttr & 0x00FF;
	//Convertion to timer value/5
	switch(pMIBObj->u32MibObjId)
	{
	case WMAN_IF_SS_LOSTDLMAPINTERVAL:
		uiMIBValue = CFG_MIB_WMANIFSSLOSTDLMAPINTERVAL_ID;
		uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_LOSTULMAPINTERVAL:
		uiMIBValue = CFG_MIB_WMANIFSSLOSTULMAPINTERVAL_ID;
        uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
        break;
	case WMAN_IF_SS_T1TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST1TIMEOUT_ID;
        uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T2TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST2TIMEOUT_ID;
        uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T3TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST3TIMEOUT_ID;
		uiValue = *((UINT8*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T6TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST6TIMEOUT_ID;
		uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T12TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST12TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T14TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST14TIMEOUT_ID;
		uiValue = *((UINT8*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T18TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST18TIMEOUT_ID;
		uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T20TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST20TIMEOUT_ID;
		uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_SS_T21TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST21TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_CMN_T7TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFCMNT7TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_CMN_T8TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFCMNT8TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_CMN_T10TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFCMNT10TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_CMN_T22TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFCMNT22TIMEOUT_ID;
	    uiValue = *((USHORT*)pMIBObj->au8MibObjVal)/5;
		break;
	case WMAN_IF_2M_SS_T44TIMER:
		uiMIBValue = CFG_MIB_WMANIF2MSST44TIMER_ID;
		memcpy(&uiValue,pMIBObj->au8MibObjVal,sizeof(UINT32));
		uiValue = uiValue/5;
		break;
	case WMAN_IF_SS_T4TIMEOUT:
		uiMIBValue = CFG_MIB_WMANIFSST4TIMEOUT_ID;
		break;
	case WMAN_IF_SS_CONTENTIONRANGRETRIES:
		uiMIBValue = CFG_MIB_WMANIFSSCONTENTIONRANGRETRIES_ID;
		break;
	case WMAN_IF_SS_REQUESTRETRIES:
		uiMIBValue = CFG_MIB_WMANIFSSREQUESTRETRIES_ID;
		break;
	case WMAN_IF_SS_REGREQUESTRETRIES:
		uiMIBValue = CFG_MIB_WMANIFSSREGREQUESTRETRIES_ID;
		break;
	case WMAN_IF_SS_SBCREQUESTRETRIES:
		uiMIBValue = CFG_MIB_WMANIFSSSBCREQUESTRETRIES_ID;
		break;
	case WMAN_IF_CMN_DSXREQRETRIES:
		uiMIBValue = CFG_MIB_WMANIFCMNDSXREQRETRIES_ID;
		break;
	case WMAN_IF_CMN_DSXRESPRETRIES:
		uiMIBValue = CFG_MIB_WMANIFCMNDSXRESPRETRIES_ID;
		break;
	case WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG:
		uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAENCRYPTALG1_ID;
		break;
	case WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG:
		uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAAUTHENTALG1_ID;
		break;
	case WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG:
		uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITETEKENCRYPTALG1_ID;
		break;
	case BCM_CFG_NETSEARCH_PERIODIC_WAKEUP_TIMER:
		uiMIBValue = CFG_MIB_BCMCONFIGNETSEARCHPERIODICWAKEUP_ID;
		break;
	case WMAN_IF_SS_INVITEDRANGRETRIES:
		uiMIBValue = CFG_MIB_WMANIFCMNINVITEDRANGRETRIES_ID;
		break;
	default:
		break;
	}
		
	//
	if(uiType == TYPE_MACADDREESS)
	{
		iRetval=BeceemWiMAX_WriteMACAddress((PVOID)pWiMAX, pMIBObj->au8MibObjVal);
		goto returnCode;
		
	}

	if (pWiMAX->m_bDeviceInitInProgress)
	{
	
		iRetval=ERROR_SHARED_MEM_NOT_INITIALIZED;
		goto returnCode;
	}
	
	Mutex_Lock(&(pWiMAX->m_ShMemMutex));

	if(usNumIndex == BIT_NON_INDEX)
	{
		/*Write Shared memory to EEPROM*/
				iSubOffset = uiOffset%4;			/*   iSubOffset is distance from int boundry */
		uiOffset = uiOffset-iSubOffset;		/*  make sure iOffset is Integer boundry start */

		/*Write Shared memory to CellRAM*/
		uiSizeToWrite = ((uiSize+3)/4)*4;
		
		
		switch(pMIBObj->u32MibObjId)
		{
			case WMAN_IF_SS_LOSTDLMAPINTERVAL:
			case WMAN_IF_SS_LOSTULMAPINTERVAL:
			case WMAN_IF_SS_T1TIMEOUT:
			case WMAN_IF_SS_T2TIMEOUT:
			case WMAN_IF_SS_T3TIMEOUT:
			case WMAN_IF_SS_T6TIMEOUT:
			case WMAN_IF_SS_T12TIMEOUT:
			case WMAN_IF_SS_T14TIMEOUT:
			case WMAN_IF_SS_T18TIMEOUT:
			case WMAN_IF_SS_T20TIMEOUT:
			case WMAN_IF_SS_T21TIMEOUT:
			case WMAN_IF_CMN_T7TIMEOUT:
			case WMAN_IF_CMN_T8TIMEOUT:
			case WMAN_IF_CMN_T10TIMEOUT:
			case WMAN_IF_CMN_T22TIMEOUT:
			case WMAN_IF_2M_SS_T44TIMER:
				if(uiSize == sizeof(USHORT))
					*(USHORT *)pucData = (USHORT)(uiValue);
				else if(uiSize == sizeof(UCHAR))
					*(UCHAR *)pucData = (UCHAR)(uiValue);
				else
					*(UINT *)pucData = uiValue;	
				break;
			default:
				memcpy(pucData, (UCHAR *)pMIBObj->au8MibObjVal, uiSizeToWrite);
				break;
		}		
		
		if(!bMultipleMIBSetCaller ||(bMultipleMIBSetCaller && !pWiMAX->u32NVMIsFlashFlag))
		{
			iRet = CFG_WriteMIB(pWiMAX->pEEPROMContextTable, pucData, uiSize, uiMIBValue);		
		}
		else if(bMultipleMIBSetCaller && pWiMAX->u32NVMIsFlashFlag)
		{
			iRet = CacheMulitpleMIBData(pWiMAX, pucData, uiSize, uiMIBValue);		
		}
		
		Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		if (iRet < 0)
		{
			if (iRet == CFG_ERR_VERSION_NOT_SUPPORTED)
			{
				iRetval=ERROR_NO_MIB_SUPPORT;
				goto returnCode;
			}
			else
			{
				iRetval= ERROR_EEPROM_NOT_ACCESSIBLE;
				goto returnCode;
			}
		}
		else
		{
			iRetval=SUCCESS;
			goto returnCode;
		}
	}

	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
	iRetval=ERROR_MIB_OBJECT_NOTFOUND;
	
returnCode:
	 BECEEM_MEMFREE(pucData)
	 return iRetval;

}
int CacheMulitpleMIBData(PBECEEMWIMAX pWiMAX,UINT8 *pBuffer,UINT32 u32Size,UINT32 u32MIBValue)
{
	if(!pBuffer)
		return ERROR_INVALID_BUFFER;
	
	if(MAX_FLASH_MIB_UPDATES_DONE_AT_A_TIME < pWiMAX->u32FlashMulitpleMIBWriteCount ||
		MAX_FLASH_WRITE_MIB_BUFF_SIZE < u32Size)
		{
			return ERROR_BUFFER_INSUFFICIENT;
		}
	
	pWiMAX->astFlashMIBWrites[pWiMAX->u32FlashMulitpleMIBWriteCount].u32ParamID = u32MIBValue;
	pWiMAX->astFlashMIBWrites[pWiMAX->u32FlashMulitpleMIBWriteCount].u32Size = u32Size;
	memcpy(pWiMAX->astFlashMIBWrites[pWiMAX->u32FlashMulitpleMIBWriteCount].ucData,
												pBuffer,u32Size);
	pWiMAX->u32FlashMulitpleMIBWriteCount++;
	return u32Size;

}
int WriteCachedMIBstoFlash(PBECEEMWIMAX pWiMAX)
{
	int iRet = 0;
	iRet =  CFG_WriteMultipleParamsToFlashSection(pWiMAX->pEEPROMContextTable,pWiMAX->astFlashMIBWrites,
							pWiMAX->u32FlashMulitpleMIBWriteCount, ((CFG_CONTEXTTABLE *)(pWiMAX->pEEPROMContextTable))->pu8ReLocAddress[CFG_BLOCK_MIB]);

	

	return iRet;
	
}
int ReadBCMPropreitaryConfig(int iOffset,  UCHAR *pucOutData, UINT *puiOutDataLen)
{
	/* Read from SYS_CONFIG */
	int iRet=0;
	FILE *fp = NULL;

	iRet = OpenSysConfigFile(&fp, "r");
	if(iRet)
		return iRet;

	if(iOffset <4 )
		return ERROR_INVALID_MIB_OBJ_ID;
	iRet = fseek(fp, iOffset, SEEK_SET);
	if(iRet)
		return ERROR_INVALID_MIB_OBJ_ID;

	fread(pucOutData,1,sizeof(UINT),fp);
	
	*(UINT *)pucOutData = htonl(*(UINT *)pucOutData);
	
	CloseSysConfigFile(&fp);

	*puiOutDataLen = sizeof(UINT);

	return SUCCESS;
}

int GetTypeSpecificData(UCHAR *pucOutData, const UCHAR *pucData, UINT uiSize, UINT uiType)
{
	int i = 0;
	switch(uiType)
	{
	case TYPE_UINT8 :
		memcpy(pucOutData, pucData, uiSize);
		break;
	case TYPE_UINT16 :
		for(i=0; i<(int)uiSize; i+=sizeof(UINT16))
		{
			*(UINT16 *)(pucOutData+i) = ntohs(*(UINT16 *)(pucData+i));
		}
		break;
	case TYPE_UINT32:
		for(i=0; i<(int)uiSize; i+=sizeof(UINT32))
		{
			*(UINT32 *)(pucOutData+i) = htonl(*(UINT32 *)(pucData+i));
		}
		break;
	case TYPE_MACADDREESS :
		memcpy(pucOutData, pucData, 6);
		break;
	default:
		memcpy(pucOutData, pucData, uiSize);
		break;
	}

	return SUCCESS;
}

int SearchData(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,const UCHAR *pucData, UCHAR *pucOutData,UINT *puiOutDataLen, UINT *puiOffset)
{
	UINT uiAttr = 0;
	UINT uiSize = 0;
	UINT uiOffset = 0;
	UINT uiTableOffset = 0;
	UINT uiNumRows = 0;
	UINT uiRowSize = 0;
	UINT usNumIndex = 0;
	UINT uiIndex = 0;
	int iRow = 0;

	int iOID = pMIBObj->u32MibObjId;
	int iRet = ValidateOID(iOID);
	if(iRet)
		return iRet;

	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
	uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
	uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
	uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;

	usNumIndex = uiAttr & 0x00FF;

	if(usNumIndex == BIT_NON_INDEX)
	{
		memcpy(pucOutData, pucData, uiSize);
		*puiOutDataLen = uiSize;
		return SUCCESS;
	}

	for(iRow=0; iRow<(int)uiNumRows; iRow++)
	{
		int i = 0;
		UCHAR *pRow = 0;
		/* check row by row */
		pRow = 	(UCHAR *)pucData + iRow*uiRowSize;

		for(i=0; i< (int)usNumIndex; i++)
		{
/*			// Do not swap if it is Host MIBS
			if(BIT_HOST_STD_STATS != uiMIBType)
				uiIndex = htonl(*((UINT *)pRow+i));
			else
				uiIndex = *((UINT *)pRow+i);
*/
			if ( usNumIndex > 1)
				{
					//solve it for PHS	
					switch(iOID)
						{
						case WMAN_IF_CMN_PHSRULEVERIFY:
						case WMAN_IF_CMN_PHSRULEFIELD:
						case WMAN_IF_CMN_PHSRULESIZE:
						case WMAN_IF_CMN_PHSRULEMASK:
						case WMAN_IF_CMN_PHSMODIFIEDBYTES:	
						case WMAN_IF_CMN_PHSMODIFIEDNUMPACKETS:
						case WMAN_IF_CMN_PHSERRORNUMPACKETS:
							if(i==0)
							{
								uiIndex = (*((UINT *)pRow));
							}
							else
							{
								uiIndex = pRow[4];
							}
							break;
						case WMAN_IF_CMN_CLASSIFIERRULEPRIORITY:
						case WMAN_IF_CMN_CLASSIFIERRULEIPTOSLOW:
						case WMAN_IF_CMN_CLASSIFIERRULEIPTOSHIGH:
						case WMAN_IF_CMN_CLASSIFIERRULEIPTOSMASK:
						case WMAN_IF_CMN_CLASSIFIERRULEIPPROTOCOL:
						case WMAN_IF_CMN_CLASSIFIERRULEIPSRCADDR:
						case WMAN_IF_CMN_CLASSIFIERRULEIPSRCMASK:
						case WMAN_IF_CMN_CLASSIFIERRULEIPDESTADDR:
						case WMAN_IF_CMN_CLASSIFIERRULEIPDESTMASK:	
						case WMAN_IF_CMN_CLASSIFIERRULESRCPORTSTART:
						case WMAN_IF_CMN_CLASSIFIERRULESRCPORTEND:
						case WMAN_IF_CMN_CLASSIFIERRULEDSTPORTSTART:	
						case WMAN_IF_CMN_CLASSIFIERRULEDSTPORTEND:
						 	
							if(i==0)
							{
								uiIndex = (*((UINT *)pRow));
							}
							else
							{
								uiIndex = (*((USHORT*)(pRow + 6)));
							}
							break;
						}
					 
					
					
				}
				else
					{
						uiIndex = (*((UINT *)pRow+i));
					}
				if(uiIndex != pMIBObj->au32Key[i])
					break;
			
		}
		if(i>= (int)usNumIndex)
		{
			/* Success */
                         S_MIBS_PHS_RULE *pPHSRuleTable = (S_MIBS_PHS_RULE *)pRow;

                         UINT uiType = pWiMAX->m_sMIBTable[iOID].u8FieldType;
                         
                        switch(iOID)
                            {
                            case WMAN_IF_CMN_PHSRULEMASK:
			      				DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"modifying length of WMAN_IF_CMN_PHSRULEMASK to %d",pPHSRuleTable->u8PHSMLength);
                                uiSize = pPHSRuleTable->u8PHSMLength;
                            break;    
                            case WMAN_IF_CMN_PHSRULEFIELD:
								DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,"modifying length of WMAN_IF_CMN_PHSRULEFIELD to %d", pPHSRuleTable->u8PHSFLength);
                                uiSize = pPHSRuleTable->u8PHSFLength;
                             break;

                            }
			GetTypeSpecificData(pucOutData, pRow+uiOffset, uiSize, uiType);

			*puiOutDataLen = uiSize;
			*puiOffset = uiOffset;
			return SUCCESS;
		}
	}
	return ERROR_MIB_OBJECT_NOTFOUND;
}

int ReadStdConfigFromSharedMem(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR *pucOutData, UINT *puiOutDataLen)
{
	UINT uiAttr = 0;
	UINT uiSize = 0;
	UINT uiOffset = 0;
	UINT uiTableOffset = 0;
	UINT uiNumRows = 0;
	UINT uiRowSize = 0;
	UINT usNumIndex = 0;
	
	int iOID = pMIBObj->u32MibObjId;
	int iRet = ValidateOID(iOID);
	if(iRet)
		return iRet;

	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
	uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
	uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
	uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;

	usNumIndex = uiAttr & 0x00FF;

	Mutex_Lock(&(pWiMAX->m_ShMemMutex));

	if (pWiMAX->m_bDeviceInitInProgress)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		return ERROR_SHARED_MEM_NOT_INITIALIZED;
    }

	if(usNumIndex == BIT_NON_INDEX)
	{
		UINT uiType = pWiMAX->m_sMIBTable[iOID].u8FieldType;

		*puiOutDataLen = uiSize;
		
		/* Only MAC Address has to be handled seperatly */
		if(uiType == TYPE_MACADDREESS)
		{
			GetMACAddress(pWiMAX, pucOutData);
		}
		else
		{	
			GetTypeSpecificData(pucOutData, (const UCHAR *)pWiMAX->pEEPROMContextTable+uiOffset, uiSize, uiType);
		}
		
		Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		return SUCCESS;
	}
	
	memcpy(pucOutData,  (UCHAR *)pWiMAX->pEEPROMContextTable+uiTableOffset, uiNumRows*uiRowSize);
	*puiOutDataLen = uiNumRows*uiRowSize;

	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));

	return iRet;
}

int ReadStdHostStatsFromIOCTL(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR *pucOutData, UINT *puiOutDataLen)
{
	UINT uiAttr = 0;
	UINT uiSize = 0;
	UINT uiOffset = 0;
	UINT uiTableOffset = 0;
	UINT uiNumRows = 0;
	UINT uiRowSize = 0;
	UINT usNumIndex = 0;
	DWORD dwNumBytesReturned = 0;
	S_MIBS_HOST_STATS_MIBS *pStHostStats = NULL;
	/*S_MIBS_HOST_STATS_MIBS stHostStats;*/
	
	int iOID = pMIBObj->u32MibObjId;
	int iRet = ValidateOID(iOID);
	if(iRet)
		return iRet;

	BECEEMCALLOC(pStHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)
	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
	uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
	uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
	uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;

	usNumIndex = uiAttr & 0x00FF;

	iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_HOST_MIBS,
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							&dwNumBytesReturned
						);

    if(iRet)
	{
		BECEEM_MEMFREE(pStHostStats);	
		return iRet;
	}


	
	if(usNumIndex == BIT_NON_INDEX)
	{
		memcpy(pucOutData, ((UCHAR *)pStHostStats)+uiOffset, uiSize);
		*puiOutDataLen = uiSize;
		BECEEM_MEMFREE(pStHostStats);	
		return SUCCESS;
	}
	memcpy(pucOutData, ((UCHAR *)pStHostStats)+uiTableOffset, uiNumRows*uiRowSize);
	*puiOutDataLen = uiNumRows*uiRowSize;
	BECEEM_MEMFREE(pStHostStats);
	return iRet;
}
int ReadHostStatsFromIOCTL(PBECEEMWIMAX pWiMAX, S_MIBS_HOST_STATS_MIBS *pstHostStats)
{
	DWORD dwNumBytesReturned = 0;
	int iRet = 0;

	if(!pWiMAX || !pstHostStats)
		return ERROR_INVALID_ARGUMENT;

    if(!pWiMAX->m_bMultiMIBCall)
        {
    	iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_HOST_MIBS,
							(PVOID)pstHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							(PVOID)pstHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							&dwNumBytesReturned
						);
        }
    else
        {
            memcpy(pstHostStats,pWiMAX->pstHostStatsCache,sizeof(S_MIBS_HOST_STATS_MIBS));
        }

	return iRet;
}
int GetSFIDList(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0, i, j, k; 
	int iRetval=0;
	S_MIBS_HOST_STATS_MIBS *pstHostStats = NULL;
	SFID_LIST stSFIDList;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pstHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)

	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHostStats);

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}

	memset(&stSFIDList, 0, sizeof(SFID_LIST));

	for(i=0, j=0; i<MIBS_MAX_SERVICEFLOWS-1; i++)
	{
		if(pstHostStats->astSFtable[i].bValid)
		{
			stSFIDList.sfidList[j++].u32SFID = pstHostStats->astSFtable[i].ulSFID;
		}
	}	

	for(i = 0; i < j; i++)
	{
		int l = 0;
		for(k = 0; k < MIBS_MAX_CLASSIFIERS ; k++)
		{

			if(pstHostStats->astClassifierTable[k].ulSFID == stSFIDList.sfidList[i].u32SFID
				&& pstHostStats->astClassifierTable[k].bUsed)
			{
				stSFIDList.sfidList[i].au32ClassifierIdList[l].u32ClasifierID= pstHostStats->astClassifierTable[k].usClassifierRuleIndex;
				stSFIDList.sfidList[i].au32ClassifierIdList[l].u32PHSRuleID= pstHostStats->astClassifierTable[k].u32PHSRuleID;
				stSFIDList.sfidList[i].u32NumClassifierPHS++;
				l++;
			}
		}
	}

  	for(i = 0; i < j; i++)
	{
		int l = 0;
		for(k = 0; k < MIBS_MAX_PHSRULES ; k++)
		{

			if(pstHostStats->astPhsRulesTable[k].ulSFID == stSFIDList.sfidList[i].u32SFID)
			{

				stSFIDList.sfidList[i].au32ClassifierIdList[l].u32PHSRuleID= pstHostStats->astPhsRulesTable[k].u8PHSI;
				
				l++;
			}
		}
	}
    
	stSFIDList.u32NumActiveSFIDs = j;
	*puiOutDataLen = sizeof(SFID_LIST);
	memcpy(pucOutData, &stSFIDList, sizeof(SFID_LIST));
	iRetval=iRet;
returnCode:
	BECEEM_MEMFREE(pstHostStats)
	return iRetval;
}
int GetNumTransmittedAndReceivedPkts(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj, 
							UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0,iRetval=0;
	UINT32 id = pMIBObj->u32MibObjId;
	UINT uiSize = 0, uiOffset = 0;
	S_MIBS_HOST_STATS_MIBS *pstHOSTStats = NULL;
	
	BECEEMCALLOC(pstHOSTStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)
	
	uiSize = pWiMAX->m_sMIBTable[id].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[id].u32Offset;

	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHOSTStats);

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}

	*puiOutDataLen = sizeof(ULONG64);
	
	memcpy(pucOutData, (PUCHAR )pstHOSTStats + uiOffset, sizeof(ULONG64));


returnCode:
	BECEEM_MEMFREE(pstHOSTStats)
	return iRetval;

}

int GetNumRxBytesPerSF(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData, 
					   UINT *puiOutDataLen, int iOID)
{
	eSchedType_t eQueueType = 0;
	int iRet = 0, i = 0;
	S_MIBS_HOST_STATS_MIBS *pstHostStats = NULL;
	UINT	uiFound = 0;
	int iRetval=0;

	memset(pucOutData, 0, 4);
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	BECEEMCALLOC(pstHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)

	switch(iOID)
	{
		case BCM_BASEBAND_NUMOF_RX_BE_BYTES:
			eQueueType = BE;
			break;
		case BCM_BASEBAND_NUMOF_RX_UGS_BYTES:
			eQueueType = UGS;
			break;
		case BCM_BASEBAND_NUMOF_RX_RTPS_BYTES:
			eQueueType = RTPS;
			break;
		case BCM_BASEBAND_NUMOF_RX_ERTPS_BYTES:
			eQueueType = ERTPS;
			break;
		case BCM_BASEBAND_NUMOF_RX_NRTPS_BYTES:
			eQueueType = NRTPS;
			break;
		default:				
			iRetval=ERROR_INVALID_ARGUMENT;
			goto returnCode;
	}
	
	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHostStats);
	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}	

	for(i=0; i<MIBS_MAX_SERVICEFLOWS-1; i++)
	{
		if(pstHostStats->astSFtable[i].bActive && 
			pstHostStats->astSFtable[i].u8QueueType == eQueueType && 
			pstHostStats->astSFtable[i].ucDirection == 0)
		{
			uiFound = 1;
			
			*((UINT *)pucOutData) += (UINT)pstHostStats->astSFtable[i].uiTotalRxBytes;
			//memcpy(pucOutData, (PUCHAR)&stHostStats.astSFtable[i].uiTotalRxBytes, 4);
		}
	}
	if(!uiFound)
	{
		//printf("No DATA on the SF %x\n", eQueueType);
		*(UINT *) pucOutData = 0x0;
	}
	*puiOutDataLen = sizeof(pstHostStats->astSFtable[i].uiTotalRxBytes);
	iRetval=iRet;
returnCode:
	BECEEM_MEMFREE(pstHostStats)
	return iRetval;
		
}


int GetNumTxBytesPerSF(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData, 
					   UINT *puiOutDataLen, int iOID)
{
	eSchedType_t eQueueType = 0;
	int iRet = 0,iRetval=0, i = 0;
	S_MIBS_HOST_STATS_MIBS *pstHostStats = NULL;
	UINT	uiFound = 0;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	
	BECEEMCALLOC(pstHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)

	memset(pucOutData, 0, 4);

	switch(iOID)
	{
		case BCM_BASEBAND_NUMOF_TX_BE_BYTES:
			eQueueType = BE;
			break;
		case BCM_BASEBAND_NUMOF_TX_UGS_BYTES:
			eQueueType = UGS;
			break;
		case BCM_BASEBAND_NUMOF_TX_RTPS_BYTES:
			eQueueType = RTPS;
			break;
		case BCM_BASEBAND_NUMOF_TX_ERTPS_BYTES:
			eQueueType = ERTPS;
			break;
		case BCM_BASEBAND_NUMOF_TX_NRTPS_BYTES:
			eQueueType = NRTPS;
			break;
		default:
			iRetval=ERROR_INVALID_ARGUMENT;
			goto returnCode;
		}
	
	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHostStats);
	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}	

	for(i=0; i<MIBS_MAX_SERVICEFLOWS-1; i++)
	{
		if(pstHostStats->astSFtable[i].bActive && 
			pstHostStats->astSFtable[i].u8QueueType == eQueueType && 
			pstHostStats->astSFtable[i].ucDirection == 1)
		{
			uiFound = 1;
			
			*((UINT *)pucOutData) += (UINT)pstHostStats->astSFtable[i].uiTotalTxBytes;
		}
	}
	if(!uiFound)
	{
		*(UINT *) pucOutData = 0x0;
	}
	*puiOutDataLen = sizeof(pstHostStats->astSFtable[i].uiTotalTxBytes);
	iRetval=iRet;
returnCode:
	BECEEM_MEMFREE(pstHostStats)
	return iRetval;		
}

int GetNumOfDSDRecvd(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0,iRetval=0;
	S_MIBS_HOST_STATS_MIBS *pstHostStats;
	
	BECEEMCALLOC(pstHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)
	memset(pucOutData, 0, 4);

	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHostStats);
	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	} 
	*(UINT *) pucOutData = pstHostStats->stHostInfo.u32TotalDSD;
	*puiOutDataLen = sizeof(pstHostStats->stHostInfo.u32TotalDSD);
iRetval=iRet;
returnCode:
	BECEEM_MEMFREE(pstHostStats)
	return iRetval;
}

int GetNumOfSuccessfulSFCreation(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{

	int iRet = 0;
	SFID_LIST *pstSFIDList = NULL;

	iRet = GetSFIDList(pWiMAX, pucOutData,puiOutDataLen);
	if(iRet)
		return iRet;
	pstSFIDList = (SFID_LIST *)pucOutData;
	*(UINT *)pucOutData = pstSFIDList->u32NumActiveSFIDs;
	*puiOutDataLen = sizeof(pstSFIDList->u32NumActiveSFIDs);
	return iRet;
}

int GetBWInfo(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0;
	UINT uiRetVal = 0;

	iRet = BeceemWiMAX_rdm(pWiMAX, BASEBAND_BW_INFO_ADDR, &uiRetVal, sizeof(UINT32));
	if (iRet)
		return iRet;

	*puiOutDataLen = sizeof(UINT32);
	memcpy(pucOutData, &uiRetVal, *puiOutDataLen);

	return iRet;
}

int ExecuteRdmReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMibObjInfo, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0;
	PST_RDM_MSG pstRdmMsg = 0;
	
	pstRdmMsg = (PST_RDM_MSG)pMibObjInfo->au8MibObjVal;
	if (pMibObjInfo->u32MibObjId == BCM_CFG_RDM_REQ)
	{
		iRet = BeceemWiMAX_rdm(pWiMAX, pstRdmMsg->u32Addr, &pstRdmMsg->u32Data, sizeof(UINT32));
		if (iRet)
			return iRet;

		*puiOutDataLen = sizeof(ST_RDM_MSG);
		memcpy(pucOutData, pstRdmMsg, *puiOutDataLen);
	}
	return SUCCESS;
}

int GetPhyStateInfo (PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0;
	UINT uiPhyStateVal = 0;

	iRet = BeceemWiMAX_rdm(pWiMAX, PHY_STATE_ADDRS, &uiPhyStateVal, sizeof(UINT32));
	if (iRet)
		return iRet;

	*puiOutDataLen = sizeof(UINT32);
	if (uiPhyStateVal == PHY_SYNC_ACHIEVED)
		uiPhyStateVal = SYNC_ACHIEVED;
	else if (uiPhyStateVal == PHY_TRYING_TO_SYNC)
		uiPhyStateVal = SYNC_TRYING_TO_SYNC;
	else
		uiPhyStateVal = SYNC_INVALID;

	memcpy(pucOutData, &uiPhyStateVal, *puiOutDataLen);

	return iRet;
}

int GetPLLStatusInfo(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = 0;
	UINT uiPLLVal = 0;

	iRet = BeceemWiMAX_rdm(pWiMAX, PLL_STATUS_ADDRS, &uiPLLVal, sizeof(UINT32));
	if (iRet)
		return iRet;

	*puiOutDataLen = sizeof(UINT32);
	if (uiPLLVal == PLL_LOCKED)
		uiPLLVal = PLL_VAL_LOCKED;
	else if (uiPLLVal == PLL_NOT_LOCKED)
		uiPLLVal = PLL_VAL_NOT_LOCKED;
	else
		uiPLLVal = PLL_VAL_INVALID;

	memcpy(pucOutData, &uiPLLVal, *puiOutDataLen);

	return iRet;
}

int GetBaseBandTxPwrInfo(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen, UINT uiMibType)
{
	int iRet = 0;
	UINT uiTempVal = 0,uiMibAddr = 0;

	if (uiMibType == BCM_STATS_TX_PWR_INFO)
		uiMibAddr = BASEBAND_TX_PWR_INFO_ADDR;
	else if (uiMibType == BCM_STATS_ANTENNA_VAL_INFO)
		uiMibAddr = BASEBAND_ANTENNA_VAL_ADDR;
	
	iRet = BeceemWiMAX_rdm(pWiMAX, uiMibAddr, &uiTempVal, sizeof(UINT32));
	if (iRet)
		return iRet;

	*puiOutDataLen = sizeof(UINT32);
	memcpy(pucOutData, &uiTempVal, *puiOutDataLen);

	return iRet;
}


int GetMIBData(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	UINT uiAttr = 0;
	UINT  uiMIBValue = 0;
	/*UCHAR aucBuffer[40000] = {0};*/
	UCHAR *pBufferData = NULL;
	int iRetval=0;

	int iOID = pMIBObj->u32MibObjId;

	int iRet = ValidateOID(iOID);
	if(iRet)
		return iRet;
	BECEEMCALLOC(pBufferData,UCHAR * ,S_MIBS_HOST_STATS_MIBS,1)
	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	switch(uiAttr & 0xFF00)
	{
	case BIT_TGT_STD_CONFIG:
		/*
			Read from shared mem/cellram
			non tabled and tabled
		*/

		if (pMIBObj->u32MibObjId == BCM_CFG_RDM_REQ)
		{
			iRet = ExecuteRdmReq(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
			iRetval=iRet;
			goto returnCode;
		}

		if (pWiMAX->m_bDeviceInitInProgress)
			{
				iRetval=ERROR_SHARED_MEM_NOT_INITIALIZED;
				goto returnCode;
			}

		{
			switch(pMIBObj->u32MibObjId)
			{
				case WMAN_IF_SS_LOSTDLMAPINTERVAL:
				uiMIBValue = CFG_MIB_WMANIFSSLOSTDLMAPINTERVAL_ID;
				break;

				case WMAN_IF_SS_LOSTULMAPINTERVAL:
				uiMIBValue = CFG_MIB_WMANIFSSLOSTULMAPINTERVAL_ID;
				break;
			
				case WMAN_IF_SS_CONTENTIONRANGRETRIES:
				uiMIBValue = CFG_MIB_WMANIFSSCONTENTIONRANGRETRIES_ID;
				break;

				case WMAN_IF_SS_REQUESTRETRIES:
				uiMIBValue = CFG_MIB_WMANIFSSREQUESTRETRIES_ID;
				break;

				case WMAN_IF_SS_REGREQUESTRETRIES:
				uiMIBValue = CFG_MIB_WMANIFSSREGREQUESTRETRIES_ID;
				break;

				case WMAN_IF_SS_T1TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST1TIMEOUT_ID;
				break;

				case WMAN_IF_SS_T2TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST2TIMEOUT_ID;
				break;

				case WMAN_IF_SS_T3TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST3TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_T6TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST6TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_T12TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST12TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_T14TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST14TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_T18TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST18TIMEOUT_ID;
				break;

				case WMAN_IF_SS_T20TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST20TIMEOUT_ID;
				break;

				case WMAN_IF_SS_SBCREQUESTRETRIES:
				uiMIBValue = CFG_MIB_WMANIFSSSBCREQUESTRETRIES_ID;
				break;
				
				case WMAN_IF_SS_T21TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST21TIMEOUT_ID;
				break;
				
				case WMAN_IF_CMN_T7TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFCMNT7TIMEOUT_ID;
				break;
				
				case WMAN_IF_CMN_T8TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFCMNT8TIMEOUT_ID;
				break;

				case WMAN_IF_CMN_T10TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFCMNT10TIMEOUT_ID;
				break;
				
				case WMAN_IF_CMN_T22TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFCMNT22TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_T4TIMEOUT:
				uiMIBValue = CFG_MIB_WMANIFSST4TIMEOUT_ID;
				break;
				
				case WMAN_IF_SS_INVITEDRANGRETRIES:
				uiMIBValue = CFG_MIB_WMANIFCMNINVITEDRANGRETRIES_ID;
				break;

				case WMAN_IF_CMN_DSXREQRETRIES:
				uiMIBValue = CFG_MIB_WMANIFCMNDSXREQRETRIES_ID;
				break;

				case WMAN_IF_CMN_DSXRESPRETRIES:
				uiMIBValue = CFG_MIB_WMANIFCMNDSXRESPRETRIES_ID;
				break;

				case WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG:
				{
					if(pMIBObj->au32Key[0] == 0)	
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAENCRYPTALG1_ID;
					else if(pMIBObj->au32Key[0] == 1)
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAENCRYPTALG2_ID;
					else
					{
						free(pBufferData);
						return ERROR_MIB_OBJECT_NOTFOUND;
					}	
				}						
				break;

				case WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG:
				{
					if(pMIBObj->au32Key[0] == 0)	
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAAUTHENTALG1_ID;
					else if(pMIBObj->au32Key[0] == 1)
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITEDATAAUTHENTALG2_ID;
					else
					{
						free(pBufferData);
						return ERROR_MIB_OBJECT_NOTFOUND;
					}
				}		
				break;

				case WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG:
				{
					if(pMIBObj->au32Key[0] == 0)	
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITETEKENCRYPTALG1_ID;
					else if(pMIBObj->au32Key[0] == 1)
						uiMIBValue = CFG_MIB_WMANIFCMNCRYPTOSUITETEKENCRYPTALG2_ID;
					else
					{
						free(pBufferData);
						return ERROR_MIB_OBJECT_NOTFOUND;
					}
				}
				break;			

				case BCM_CFG_NETSEARCH_PERIODIC_WAKEUP_TIMER:
				uiMIBValue = CFG_MIB_BCMCONFIGNETSEARCHPERIODICWAKEUP_ID;
				break;	

				case WMAN_IF_2M_SS_T44TIMER:
				uiMIBValue = CFG_MIB_WMANIF2MSST44TIMER_ID;
				break;

				default:
					iRetval=ERROR_MIB_OBJECT_NOTFOUND;
					goto returnCode;
				
			}

			iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (CFG_UINT8*)pucOutData, pWiMAX->m_sMIBTable[iOID].u32FieldSize, uiMIBValue);
			if (iRet <= 0)
			{
				if (iRet == CFG_ERR_VERSION_NOT_SUPPORTED)
					{
						iRetval=ERROR_NO_MIB_SUPPORT;
						goto returnCode;
					}
				else
					{
						iRetval=ERROR_MIB_OBJECT_NOTFOUND;
						goto returnCode;
					}
			}
			*puiOutDataLen = iRet;
		}
		iRet = SUCCESS;
		break;

	case BIT_TGT_PROP_CONFIG:
		{
			/*
				Read from file
				non tabled
			*/
			int iOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
			iRet = ReadBCMPropreitaryConfig(iOffset, pucOutData, puiOutDataLen);
			if (iOID == BCM_CFG_AUTO_SCAN_ENABLE)
			{
				UINT *puiTempVal = 0;

				puiTempVal = (UINT*)pucOutData;
				*puiTempVal &= BIT_MASK_AUTO_SCAN_ENABLE;
			}
			else if(iOID == BCM_CFG_WIMAX_TRIGGER_TYPE)
			{
				UINT *puiTempVal = 0;

				puiTempVal = (UINT*)pucOutData;

				*puiTempVal = *puiTempVal >> BIT_POSITION_WIMAX_TRIGGER_TYPE;
				
			}
			
		}
		break;

	case BIT_TGT_STD_STATS:
	case BIT_TGT_PROP_STATS:
	/*
			Read from reg - stats read
			non tabled and tabled
	*/
		switch(pMIBObj->u32MibObjId)
		{
			case BCM_BASEBAND_SF_ID_LIST:
				iRet = GetSFIDList(pWiMAX, pucOutData, puiOutDataLen);
				break;
			case BCM_STATS_NUMPACKETS_TRANSMITTED_FROM_HOST:
			case BCM_STATS_NUMPACKETS_RECEIVED_BY_HOST:
				iRet = GetNumTransmittedAndReceivedPkts(pWiMAX,pMIBObj,pucOutData,puiOutDataLen);
				break;
				
			
			case BCM_STATS_PHY_STATE_INFO:
				iRet = GetPhyStateInfo(pWiMAX, pucOutData, puiOutDataLen);
				break;

			case BCM_STATS_BW_INFO:
				iRet = GetBWInfo(pWiMAX, pucOutData, puiOutDataLen);
				break;

			case BCM_STATS_PLL_STATUS_INFO:
				iRet = GetPLLStatusInfo(pWiMAX, pucOutData, puiOutDataLen);
			break;
            
			case WMAN_IF_SS_MACADDRESS:
				*puiOutDataLen = 6;
                 iRet = GetMACAddress(pWiMAX, pucOutData);
            break;

			case BCM_STATS_TX_PWR_INFO:
			case BCM_STATS_ANTENNA_VAL_INFO:
				iRet = GetBaseBandTxPwrInfo(pWiMAX, pucOutData, puiOutDataLen, 
					pMIBObj->u32MibObjId);
				break;
			
			case BCM_BASEBAND_NUMOF_RX_BE_BYTES:
			case BCM_BASEBAND_NUMOF_RX_UGS_BYTES:
			case BCM_BASEBAND_NUMOF_RX_RTPS_BYTES:
			case BCM_BASEBAND_NUMOF_RX_ERTPS_BYTES:
			case BCM_BASEBAND_NUMOF_RX_NRTPS_BYTES:
				iRet = GetNumRxBytesPerSF(pWiMAX, pucOutData, puiOutDataLen, iOID);
				break;
			case BCM_BASEBAND_NUMOF_TX_ERTPS_BYTES:
			case BCM_BASEBAND_NUMOF_TX_NRTPS_BYTES:
			case BCM_BASEBAND_NUMOF_TX_RTPS_BYTES:
			case BCM_BASEBAND_NUMOF_TX_UGS_BYTES:
			case BCM_BASEBAND_NUMOF_TX_BE_BYTES:
				iRet = GetNumTxBytesPerSF(pWiMAX, pucOutData, puiOutDataLen, iOID);
				break;
			
			case BCM_BASEBAND_NUMOF_SF_DELETIONS:
				iRet = GetNumOfDSDRecvd(pWiMAX, pucOutData, puiOutDataLen);
				break;

			case BCM_BASEBAND_NUMOF_SF_CREATION_SUCCESS:
				iRet = GetNumOfSuccessfulSFCreation(pWiMAX, pucOutData, puiOutDataLen);
				break;
			
			case BCM_BASEBAND_MRU_INFO:
				iRet = GetMRUData(pWiMAX, pucOutData, puiOutDataLen);
				break;

			case BCM_BASEBAND_RX_PACKETS_HISTOGRAM:
			case BCM_BASEBAND_TX_PACKETS_HISTOGRAM:
				iRet = GetRxTxPacketHistorgram(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_BASEBAND_NUM_OF_PACKETS_DROPPED_PER_SF:
			case BCM_BASEBAND_NUM_OF_BYTES_DROPPED_PER_SF:
				iRet = GetPacketsOrBytesDroppedPerSF(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_BASEBAND_NEIGBOUR_BSINFO:
				iRet = GetNeighbourInfoFromBSLogger(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;
			case BCM_BASEBAND_TRIGGER_STATUS_INFO:
				iRet = GetTriggerStatusInfo(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_STATS_PDURCVQPSK12:
			case BCM_STATS_PDURCVQPSK34:
			case BCM_STATS_PDURCV16QAM12:
			case BCM_STATS_PDURCV16QAM34:
			case BCM_STATS_PDURCV64QAM12:
			case BCM_STATS_PDURCV64QAM23:
			case BCM_STATS_PDURCV64QAM34:
			case BCM_STATS_PDURCV64QAM56:
			case BCM_STATS_PDURCVQPSK12CTCREP6:
			case BCM_STATS_PDURCVQPSK12CTCREP4:
			case BCM_STATS_PDURCVQPSK12CTCREP2:
			case BCM_STATS_PDURCVQPSK12CTCREP1:
			case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP6:
			case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP4:
			case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP2:
			case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP1: 
				iRet = GetPduRcvStatInformation(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_STATS_BURSTRCVQPSK12:
			case BCM_STATS_BURSTRCVQPSK34:
			case BCM_STATS_BURSTRCV16QAM12:
			case BCM_STATS_BURSTRCV16QAM34: 
			case BCM_STATS_BURSTRCV64QAM12:
			case BCM_STATS_BURSTRCV64QAM23:
			case BCM_STATS_BURSTRCV64QAM34:
			case BCM_STATS_BURSTRCV64QAM56: 
			case BCM_STATS_BURSTSNDQPSK12:
			case BCM_STATS_BURSTSNDQPSK34:
			case BCM_STATS_BURSTSND16QAM12:
			case BCM_STATS_BURSTSND16QAM34:
				iRet = GetMcsStatInformation(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_BASEBAND_PER_DLZONE_PER_MCS_BURSTS_DECODED:
				iRet = GetDlZoneStatInformation(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);				
				break;

			case BCM_BASEBAND_NUM_OF_CONTROLMESSAGES_DROPPED:
				iRet = GetNumberOfControlMessagesDropped(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;

			case BCM_STATS_PDUSENDQPSK12REP1:
			case BCM_STATS_PDUSENDQPSK12REP2:
			case BCM_STATS_PDUSENDQPSK12REP4:
			case BCM_STATS_PDUSENDQPSK12REP6:
			case BCM_STATS_PDUSENDQPSK34:
			case BCM_STATS_PDUSEND16QAM12:
			case BCM_STATS_PDUSEND16QAM34:
			case BCM_STATS_PDUSEND64QAM12:
			case BCM_STATS_PDUSEND64QAM23:
			case BCM_STATS_PDUSEND64QAM34:
			case BCM_STATS_PDUSEND64QAM56:	
				iRet = GetPduSndStatInformation(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break; 
	
			case BCM_STATS_HARQRETRYEXCEEDED:
				iRet = GetHARQRetriesExceeded(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;
			case BCM_BASEBAND_NUM_OF_DLHARQ_BURSTS_IN_ERRORS:
				iRet = GetDLHARQsInError(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;
			case BCM_BASEBAND_NUM_OF_HARQ_RETRY_IN_ERRORS:
				iRet = GetHARQRetryInError(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;
			case BCM_STATS_DL_ZONE_INFO:				
			case BCM_STATS_UL_ZONE_INFO:
				iRet = GetDlUlZoneInformation(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
			
				break;	
			case BCM_STATS_INTRA_FA_PREAMBLE_INDEX:
            case BCM_STATS_BCMBSINTRAFAMEANCINR:
            case BCM_STATS_INTER_FA_PREAMBLE_INDEX:
            case BCM_STATS_BCMBSINTERFAMEANCINR:
                
                iRet = BeceemWiMAX_neighbour_stats_wrapper(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				break;
			default:
				iRet = SearchStatsData(pWiMAX, pMIBObj, pucOutData, puiOutDataLen);
				
				
				if((pMIBObj->u32MibObjId == BCM_BASEBAND_REALM_INFO) && (SUCCESS == iRet))
				{
					iRet = GetRealmInfo(pWiMAX,pucOutData,puiOutDataLen);
				}
				else if ((pMIBObj->u32MibObjId == BCM_BASEBAND_PER_ZONE_PER_MCS_BURSTS_DECODED) && (SUCCESS == iRet))
				{
					ST_PER_ZONE_MCS_STATS* pstPerZoneMcsStats = NULL;
					int i = 0;
					UCHAR *pucTemp = NULL;
					PST_PERZONE_DL_INFO pstPerDLZone = NULL;

					pstPerZoneMcsStats = (ST_PER_ZONE_MCS_STATS*)pucOutData;
					for (i = 0; i<NUMBER_OF_DL_ZONE_TYPES; i++)
					{
						pstPerDLZone = &pstPerZoneMcsStats->stPerZoneInfo.stPerDLZoneInfo[i];

						pucTemp = (UCHAR*)&pstPerDLZone->s8PCINRRate1;
						*(UINT *)pucTemp = Swap_Int(*(UINT *)pucTemp);
					}

					pucTemp = (UCHAR*)&pstPerZoneMcsStats->BcmBasebandULAllocatedSubChannelsBitMap;
					for (i=0; i<3; i++)
					{
						*(UINT *)pucTemp = Swap_Int(*(UINT *)pucTemp);
						pucTemp += sizeof(UINT);
					}
				}
				break;
		}
		break;

	case BIT_HOST_STD_STATS:
		{
			UINT uiOffset;
			UINT uiOldSize = 0;
			UINT uiSizeOfData = /*sizeof(aucBuffer)*/sizeof(S_MIBS_HOST_STATS_MIBS);
			iRet = ReadStdHostStatsFromIOCTL(pWiMAX, pMIBObj, pBufferData, &uiSizeOfData);
			uiOldSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;

			if(
				iOID == WMAN_IF_CMN_CLASSIFIERRULEIPSRCADDR ||
				iOID == WMAN_IF_CMN_CLASSIFIERRULEIPSRCMASK ||
				iOID == WMAN_IF_CMN_CLASSIFIERRULEIPDESTADDR ||
				iOID == WMAN_IF_CMN_CLASSIFIERRULEIPDESTMASK 
			 )
			{
				S_MIBS_CLASSIFIER_RULE *pClassifierEntry = (S_MIBS_CLASSIFIER_RULE *)pBufferData;

				/* Change the size depending on the IPV4 or IPV6 protocol flag */
				if(!pClassifierEntry->bIpv6Protocol)
					pWiMAX->m_sMIBTable[iOID].u32FieldSize = MIBS_MAX_IP_RANGE_LENGTH * MIBS_IP_LENGTH_OF_ADDRESS;
			}
			if(!iRet)
				iRet = SearchData(pWiMAX, pMIBObj, pBufferData, pucOutData, puiOutDataLen, &uiOffset);

			/* Revert back to old size */
			pWiMAX->m_sMIBTable[iOID].u32FieldSize =(UINT32)uiOldSize;
		}
		break;
	
	default:
		iRetval=ERROR_INVALID_MIB_ATTRIBUTE;
		goto returnCode;	
}

	iRetval=iRet;
returnCode:
	BECEEM_MEMFREE(pBufferData)
	return iRetval;
}
/**
@ingroup Utility_Functions
*	Function		:	This will read the statistics doing series of RDMs.
*	@param	PVOID pArg - is void pointer, pointing to a valid BECEEMWIMAX instance.
*	@param	pMIBObj pointer to MIB_OBJECT_INFO structure
*	@retval		SUCCESS(0) on success, error code on failure.
*/

int SearchStatsData(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,
		    UCHAR * pucOutData, UINT * puiOutDataLen)
{
    UINT uiAttr = 0;
    UINT uiSize = 0;
    UINT uiOffset = 0;
    UINT uiTableOffset = 0;
    UINT uiNumRows = 0;
    UINT uiRowSize = 0;
    UINT usNumIndex = 0;
    UINT uiIndex = 0;
    int iRow = 0;
    int iRetval = 0;
    PUCHAR pucData = NULL;

    int iOID = pMIBObj->u32MibObjId;
    int iRet = ValidateOID(iOID);

    if (iRet)
		return iRet;

    uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
    uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
    uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
    uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
    uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
    uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;

    usNumIndex = uiAttr & 0x00FF;

    if (pWiMAX->m_bStatsPointerFlag == FALSE && !pWiMAX->m_bMultiMIBCall)
	{
	    iRet =
		BeceemWiMAX_GetStatisticsAddress(pWiMAX,
						 &(pWiMAX->m_uiStatAddress));
	    if (iRet)
		return iRet;

	    iRet =
		BeceemWiMAX_ReadStatisticsLength(pWiMAX, pWiMAX->m_uiStatAddress,
						 &(pWiMAX->m_uiStructLength));
	    if (iRet)
		return iRet;

	    pWiMAX->m_bStatsPointerFlag = TRUE;
	}

    if (usNumIndex == BIT_NON_INDEX)
	{
	    /*  Skip offset num of bytes from stats address */
	    int iSubOffset = uiOffset % 4;	/*   iSubOffset is distance from int boundry */
	    int iSizeToRead = 0;

	    uiOffset = uiOffset - iSubOffset;	/*  make sure iOffset is Integer boundry start */

	    iSizeToRead = ((uiSize + 3) / 4) * 4;
	    if (!iSizeToRead)
		return ERROR_INVALID_MIB_OBJ_ID;

	    BECEEMCALLOC(pucData, UCHAR *, UCHAR, iSizeToRead)
        if(!pWiMAX->m_bMultiMIBCall)
            {
        		iRet =
	            	BeceemWiMAX_rdmBulk(pWiMAX, pWiMAX->m_uiStatAddress + uiOffset,
				        pucData, iSizeToRead);
            }
        else
            {
                memcpy(pucData,&pWiMAX->pStatsCacheData[uiOffset],iSizeToRead);
            }
	    if (iRet)
		{
		    iRetval = iRet;
		    goto returnCode;
		}
	    /* *puiOutDataLen = iSizeToRead - iSubOffset; */
	    *puiOutDataLen = uiSize;

	    if (iOID == BCM_STATS_BSID || iOID == WMAN_IF_SS_OFDMABSID
		|| iOID == BCM_STATS_FLASH_MAP_INFO)
		{
		    *(UINT *) pucOutData = htonl(*(UINT *) pucData);
		    *((UINT *) pucOutData + 1) = htonl(*((UINT *) pucData + 1));
		}
	    else
		{
		    BeceemWiMAX_CheckAndSwapMIBData(pWiMAX, pucData, iOID);
		    memcpy(pucOutData, pucData + iSubOffset, *puiOutDataLen);
		}
	    iRetval = SUCCESS;
	    goto returnCode;
	}
    BECEEM_MEMFREE(pucData)
	BECEEMCALLOC(pucData, UCHAR *, UCHAR, uiRowSize)
	for (iRow = 0; iRow < (int) uiNumRows; iRow++)
	{
	    int i = 0;
	    UCHAR *pRow = 0;

	    /* check row by row */

	    memset(pucData, 0, sizeof(uiRowSize));
         if(!pWiMAX->m_bMultiMIBCall)
            {
        	    iRet =
        		BeceemWiMAX_rdmBulk(pWiMAX,
		    		    pWiMAX->m_uiStatAddress + uiTableOffset +
			    	    (iRow * uiRowSize), pucData, uiRowSize);
            }
            else
                {
                    iRet =0;
                    memcpy(pucData,&pWiMAX->pStatsCacheData[uiTableOffset + (iRow * uiRowSize)],uiRowSize);
                }
	    if (iRet)
		{
		    iRetval = iRet;
		    goto returnCode;
		}
	    pRow = pucData;

	    for (i = 0; i < (int) usNumIndex; i++)
		{
		    uiIndex = *((UINT *) pRow + i);
		    if (uiIndex != pMIBObj->au32Key[i])
			break;
		}
	    if (i >= (int) usNumIndex)
		{
		    /* Success */
		    memcpy(pucOutData, pRow + uiOffset, uiSize);
		    *puiOutDataLen = uiSize;
		    iRetval = SUCCESS;
		    goto returnCode;

		}
	}

    iRetval = ERROR_MIB_OBJECT_NOTFOUND;
  returnCode:
    BECEEM_MEMFREE(pucData) return iRetval;
}

void BeceemWiMAX_CheckAndSwapMIBData(PBECEEMWIMAX pWiMAX,PUCHAR pucData,INT32 iOID)
{

	if(!pWiMAX->m_bBigEndianMachine)
		return;

	
	switch(iOID)
	{
		case BCM_STATS_PHY_UL_BURST_DATA_ZONE:
		case BCM_STATS_PHY_DL_BURST_DATA_ZONE:
		case BCM_STATS_PHY_UL_BURST_DATA_FEC_SCHEME:
		case BCM_STATS_PHY_DL_MAP_FEC_SCHEME:
		case BCM_STATS_PHY_UL_BURST_DATA_UIUC:
		case BCM_STATS_PHY_DL_BURST_DATA_DIUC:
		case BCM_STATS_PHY_UL_BURST_DATA_FEC_REPETITION:
		case BCM_STATS_PHY_DL_BURST_DATA_FEC_REPETITION:
		case BCM_STATS_NUMDLCHSMARKEDUNUSABLE:	
		case BCM_STATS_NUMULCHSMARKEDUNUSABLE:
		case BCM_STATS_NUMSYNCFAILURES:
		case BCM_STATS_NUMBWREQUESTSSENT:
		case BCM_STATS_CURDCDCHANGECNT:
		case BCM_STATS_CURUCDCHANGECNT:
		case BCM_STATS_NUMUCDSRECEIVED:
		case BCM_STATS_NUMDCDSRECEIVED:
		case BCM_STATS_NUMSBCFAILURES:
		case BCM_STATS_NUMAUTHENTICATIONFAILURES:
		case BCM_STATS_PREAMBLE_INDEX:
		case BCM_STATS_PRCODE_END:
		case BCM_STATS_PRCODE_START:
		case BCM_STATS_HRCODEEND:
		case BCM_STATS_HRCODESTART:
		case BCM_STATS_PHY_DL_BURST_DATA_FEC_SCHEME:
		case BCM_STATS_PHY_DL_FRAME_RATIO:
		case BCM_STATS_PHY_UL_FRAME_RATIO:
		case BCM_STATS_DLIDCELL:
		case BCM_STATS_BCMBASEBANDULPERMUTATION:
		case BCM_STATS_IRCODESTART:
		case BCM_STATS_IRCODEEND:
		case BCM_STATS_TXPOWERHEADROOM:
		case BCM_STATS_TXPOWER:
		case BCM_STATS_BRCODESTART:
		case BCM_STATS_BRCODEEND:		
			
			*(INT *)pucData = Swap_Int(*(INT *)pucData);
			break;
	}
	
}
int BeceemWiMAX_ProcessMIBSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = -1,iRetval=0;
	PMIB_OBJECT_INFO pMIBObj = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	PWIMAXMESSAGE pWMMsg = NULL;
	UNUSED_ARGUMENT(uiLength);
	
	if(!pWiMAX | !pMsg)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	pMIBObj = (PMIB_OBJECT_INFO)pMsg->szData;
	
	/*Special READ-ONLY config MIBs.*/
	if(pMIBObj->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG
			|| pMIBObj->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG
			|| pMIBObj->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG)
	{
		
		iRetval=ERROR_MIB_OBJ_READONLY;	
		goto returnCode;
	}
	/*Other Config MIBs*/
	else if(pMIBObj->u32MibObjId >= BCM_STANDARD_CFG_MIB_BASE_START && pMIBObj->u32MibObjId <= BCM_STANDARD_CFG_MIB_BASE_END)
	{
		/* STANDARD CONFIG MIBS */
		iRet = WriteStandardConfigMIB(pWiMAX, pMIBObj,FALSE);
		if(iRet)
		{
			iRetval=iRet;
			goto returnCode;
		}	
	}
	else if(pMIBObj->u32MibObjId >= BCM_PROPREITARY_CFG_MIB_BASE_START && pMIBObj->u32MibObjId <= BCM_PROPREITARY_CFG_MIB_BASE_END)
	{

		if (pMIBObj->u32MibObjId == BCM_BASEBAND_RESET_MRU_TABLE)
			{
					iRet = CFG_ResetMRU((CFG_UINT8 *)pWiMAX->pEEPROMContextTable);
					if(iRet < 0)
						{
							iRetval=ERROR_RESET_FAILED;
							goto returnCode;
						}
					else
						{
							iRetval=SUCCESS;
							goto returnCode;
						}
			}
		else if(pMIBObj->u32MibObjId == BCM_BASEBAND_ADD_MRU)
	        {
	         
	            //maybe add multiples in the future?
	            if(pMIBObj->u32MibObjSize != sizeof(ST_MRU_ADD_INFO))
				{	
	               	iRetval=ERROR_INVALID_MIB_OBJ_SIZE;
					goto returnCode;
				}
	            iRet = SetMRUData(pWiMAX,(ST_MRU_ADD_INFO*)pMIBObj->au8MibObjVal);
	            pMIBObj->s8ErrorCode =(SINT8)iRet;
	            WiMAXMessage_Init(
	                pWMMsg,
	                TYPE_MIB, 
	                SUBTYPE_MIB_SET_RESP, 
	                (UCHAR*)pMIBObj,
	                (sizeof(MIB_OBJECT_INFO) + pMIBObj->u32MibObjSize) - 1
	                );
	            BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

	          	iRetval=iRet;
				goto returnCode;
	        }

				
		if(pMIBObj->u32MibObjSize != sizeof(UINT32))
			{
				iRetval=ERROR_INVALID_MIB_OBJ_SIZE;
				goto returnCode;
			}
	

		if (pMIBObj->u32MibObjId == BCM_CFG_RESET_BER_COUNTER)
		{
			UINT uiValue = 0;

			memcpy(&uiValue,pMIBObj->au8MibObjVal,pMIBObj->u32MibObjSize);
					
			iRet = BeceemWiMAX_wrm(pWiMAX, BASEBAND_REG_RESET_BER_ADDR, uiValue);
			if(iRet)
			{
			
				pMIBObj->s8ErrorCode =(SINT8)iRet;

				WiMAXMessage_Init(
								pWMMsg,
								TYPE_MIB, 
								SUBTYPE_MIB_SET_RESP, 
								(UCHAR*)pMIBObj,
								sizeof(MIB_OBJECT_INFO) + sizeof(UINT32)
								);

				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

				iRetval=iRet;
				goto returnCode;
			}
			else
			{
				pMIBObj->s8ErrorCode = 0;

				WiMAXMessage_Init(
								pWMMsg,
								TYPE_MIB, 
								SUBTYPE_MIB_SET_RESP, 
								(UCHAR*)pMIBObj,
								sizeof(MIB_OBJECT_INFO) + sizeof(UINT32)
								);

				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

				iRetval=SUCCESS;
				goto returnCode;
			}
		}

		iRet = BeceemWiMAX_MIBWriteConfig(pWiMAX, pMIBObj);
		if(iRet)
		{
			iRetval=iRet;
			goto returnCode;
		}
	}
	else if(pMIBObj->u32MibObjId == WMAN_IF_SS_PKMAUTHRESET)
	{
		iRetval=WriteStandardStatsMIB(pWiMAX, pMIBObj);
		goto returnCode;
	}
	else if(pMIBObj->u32MibObjId >= BCM_PROPREITARY_STATS_MIB_BASE_START && pMIBObj->u32MibObjId <= BCM_PROPREITARY_STATS_MIB_BASE_END)
	{
		if ((pMIBObj->u32MibObjId == BCM_BASEBAND_MRU_INFO) || 
			(pMIBObj->u32MibObjId == BCM_BASEBAND_RESET_MRU_TABLE))
			{
			iRetval=ERROR_MIB_FOR_DIAGNOSTIC_ONLY;
			goto returnCode;
			}
		else
			{
			iRetval=ERROR_MIB_OBJ_READONLY;
			goto returnCode;	
		}
	}
	else
		{
			iRetval=ERROR_INVALID_MIB_OBJ_ID;
			goto returnCode;
		}
	if(iRet == SUCCESS)
		{
		/*  Send pMIBObject to application callback */
	
		WiMAXMessage_Init(
						pWMMsg,
						TYPE_MIB, 
						SUBTYPE_MIB_SET_RESP, 
						(UCHAR*)pMIBObj,
						sizeof(MIB_OBJECT_INFO)
						);

		BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
		}
	iRetval=iRet;

returnCode:
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
}

int WriteStandardStatsMIB(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj)
{
	UINT uiAttr = 0;
	UINT uiSize = 0;
	UINT uiOffset = 0;
	UINT uiTableOffset = 0;
	UINT uiNumRows = 0;
	int iRetval=0;
	UINT uiRowSize = 0, usNumIndex = 0;
	PUCHAR pucData =NULL;
	UINT szData=1024;

	int iOID = pMIBObj->u32MibObjId;
	int iRet = ValidateOID(iOID);
	if(iRet)
		return iRet;

	uiAttr = pWiMAX->m_sMIBTable[iOID].u16MIBAttribute;
	uiSize = pWiMAX->m_sMIBTable[iOID].u32FieldSize;
	uiOffset = pWiMAX->m_sMIBTable[iOID].u32Offset;
	uiTableOffset = pWiMAX->m_sMIBTable[iOID].u32TableOffset;
	uiNumRows = pWiMAX->m_sMIBTable[iOID].u8NumRows;
	uiRowSize = pWiMAX->m_sMIBTable[iOID].u16TableRowSize;

	usNumIndex = uiAttr & 0x00FF;
	
	if (pWiMAX->m_bStatsPointerFlag == FALSE)
	{
		iRet = BeceemWiMAX_GetStatisticsAddress(pWiMAX,  &(pWiMAX->m_uiStatAddress));
		if(iRet)
			return iRet;
		
		iRet = BeceemWiMAX_ReadStatisticsLength(pWiMAX, pWiMAX->m_uiStatAddress, &(pWiMAX->m_uiStructLength));
		if(iRet)
			return iRet;
		
		pWiMAX->m_bStatsPointerFlag = TRUE;
	}
	
	if(usNumIndex == BIT_NON_INDEX)
	{
		/*  Skip offset num of bytes from stats address */
		int iSubOffset = uiOffset%4;			/*   iSubOffset is distance from int boundry */
		int iSizeToRead = 0;
		uiOffset = uiOffset-iSubOffset;		/*  make sure iOffset is Integer boundry start */

		iSizeToRead = ((uiSize+3)/4)*4;

		BECEEMCALLOC(pucData,PUCHAR,CHAR,szData)
		iRet = BeceemWiMAX_rdmBulk(pWiMAX, pWiMAX->m_uiStatAddress+uiOffset, pucData, iSizeToRead);
		if(iRet)
		{
			iRetval=iRet;
			goto returnCode;

		}
		
		memcpy(pucData + iSubOffset, pMIBObj->au8MibObjVal, uiSize);
		BeceemWiMAX_wrmBulk(pWiMAX, pWiMAX->m_uiStatAddress+uiOffset, pucData, iSizeToRead);

		iRetval=SUCCESS;
		goto returnCode;

	}
iRetval=ERROR_MIB_OBJECT_NOTFOUND;

returnCode:
	BECEEM_MEMFREE(pucData)
	return iRetval;
}

int BeceemWiMAX_MIBWriteConfig(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj)
{
	int iOffset, iSize, index, iOpenSysFile;
	FILE *fp = NULL;

	if(!pWiMAX || !pMIBObj)
		return ERROR_INVALID_ARGUMENT;
	
	index = pMIBObj->u32MibObjId; 
	iOffset = pWiMAX->m_sMIBTable[index].u32Offset;
	iSize = pWiMAX->m_sMIBTable[index].u32FieldSize;

	iOffset = iOffset-(iOffset%4); /*  make sure iOffset is Integer boundry start */

	iOpenSysFile = OpenSysConfigFile(&fp, "rb+");
	if(iOpenSysFile)
		return iOpenSysFile;

	if(iOffset <4 )
		return ERROR_INVALID_MIB_OBJ_ID;

	if(fseek(fp, iOffset, SEEK_SET))
		return ERROR_INVALID_MIB_OBJ_ID;

	*(UINT *)pMIBObj->au8MibObjVal = htonl(*(UINT *)pMIBObj->au8MibObjVal);
	if (pMIBObj->u32MibObjId == BCM_CFG_AUTO_SCAN_ENABLE)
	{
		UINT uiReadVal = 0;
		UINT* puiTempVal = 0;

		fread(&uiReadVal,1,sizeof(UINT),fp);
		uiReadVal = ntohl(uiReadVal);
		puiTempVal = (UINT *)&pMIBObj->au8MibObjVal;
		if (*puiTempVal)
			uiReadVal |= ntohl(*puiTempVal);
		else
			uiReadVal ^= 1;

		*puiTempVal = htonl(uiReadVal); 

		CloseSysConfigFile(&fp);
		iOpenSysFile = OpenSysConfigFile(&fp, "rb+");
		if(iOpenSysFile)
			return iOpenSysFile;

		if(fseek(fp, iOffset, SEEK_SET))
			return ERROR_INVALID_MIB_OBJ_ID;

	}
	if (pMIBObj->u32MibObjId == BCM_CFG_WIMAX_TRIGGER_TYPE)
	{
		UINT uiReadVal = 0;
		UINT* puiTempVal = 0;

		fread(&uiReadVal,1,sizeof(UINT),fp);
		uiReadVal = ntohl(uiReadVal);
		puiTempVal = (UINT *)&pMIBObj->au8MibObjVal;
		
		if(ntohl(*puiTempVal)<TRIGGER_TYPE_NOT_DEFINED || ntohl(*puiTempVal)>TRIGGER_TYPE_CINR)
			return ERROR_INVALID_VALUE;

		/* Compare Bit 30 of config file param with Bit 0 of value given by user and make it equal */
		if(((uiReadVal>>BIT_WIMAX_TRIGGER_TYPE_LOWER) & 1) != ((ntohl(*puiTempVal)>>0) & 1))
		{
			uiReadVal ^= (1<<BIT_WIMAX_TRIGGER_TYPE_LOWER);	
		}

		/* Compare Bit 31 of config file param with Bit 1 of value given by user and make it equal */
		if(((uiReadVal>>BIT_WIMAX_TRIGGER_TYPE_UPPER) & 1) != ((ntohl(*puiTempVal)>>1) & 1))
		{
			uiReadVal ^= (1<<BIT_WIMAX_TRIGGER_TYPE_UPPER);	
		}
		*puiTempVal = htonl(uiReadVal);

		pWiMAX->stTriggerInformation.eTriggerType = uiReadVal>>BIT_POSITION_WIMAX_TRIGGER_TYPE;

		if(fseek(fp, iOffset, SEEK_SET))
			return ERROR_INVALID_MIB_OBJ_ID;

	}
	if(pMIBObj->u32MibObjId == BCM_CFG_WIMAX_TRIGGER_THRESHOLD)
	{
		pWiMAX->stTriggerInformation.s32TriggerThreshold = ntohl(*(UINT *)&pMIBObj->au8MibObjVal);
	}
	
	
	fwrite((const void*)pMIBObj->au8MibObjVal,1,4,fp);
	
	CloseSysConfigFile(&fp);
	return SUCCESS;
}

void BeceemWiMAX_KillMultipleMibThread(void *pParam)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
	if(!pWiMAX)
		return;

    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "--------Start KillMultipleMibThread--------");
	pWiMAX->m_bMultipleMibThreadRunning = FALSE;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "---------End KillMultipleMibThread---------");
}




void *BeceemWiMAX_MultipleMibThreadFunc(void *pParam)
{
		
	PWIMAX_MESSAGE pRequestMessage=NULL;
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pParam;
	UINT32 u32MIBsFragment=0;
	UINT32 u32totalChunks=0;
	PUCHAR puchBaseAddr = 0;
	UINT32 u32Count=0;
    INT iRet=0;
	SET_THREAD_NAME();

	if(!pWiMAX)
		return (PVOID)ERROR_INVALID_ARGUMENT;

	pWiMAX->m_bMultipleMibThreadRunning= TRUE;
	BECEEMCALLOC_RET(pRequestMessage,PWIMAX_MESSAGE,WIMAX_MESSAGE,1,PVOID)
	while(pWiMAX->m_bMultipleMibThreadRunning)
	{
		/* Check to kill this thread */
		
		if ((pWiMAX->m_bIdleMode == TRUE) || (pWiMAX->m_bModemInShutdown == TRUE))
		{
#ifdef WIN32
			Sleep(500*4);
#else
			usleep(500000*4);
#endif
			continue;
		}
		if(pWiMAX->stMultiMIBNotification.u32MIBUpdateFlag)
		{
			
			PMIB_OBJECTS pMIBObj =NULL;
			
			puchBaseAddr = pRequestMessage->szData;

			u32MIBsFragment = pWiMAX->stMultiMIBNotification.u32MIBsFragmentSize;
            if(!u32MIBsFragment)
                {
                    u32MIBsFragment  = pWiMAX->stMultiMIBNotification.u32MIBsFragmentSize = 
                        pWiMAX->stMultiMIBNotification.u32NumMibObjs;
                }
			u32totalChunks=pWiMAX->stMultiMIBNotification.u32NumMibObjs/u32MIBsFragment;

			if(pWiMAX->stMultiMIBNotification.u32NumMibObjs > (u32totalChunks*u32MIBsFragment))
				u32totalChunks++;

			for(u32Count=0; u32Count < u32totalChunks ; u32Count++)
			{
				pMIBObj = (PMIB_OBJECTS)(puchBaseAddr);
				if((u32Count+1)*u32MIBsFragment <= pWiMAX->stMultiMIBNotification.u32NumMibObjs)
				{	
					pMIBObj->u16NumMibObjs=(UINT16)u32MIBsFragment;
      				puchBaseAddr+= sizeof(UINT16);
					memcpy(puchBaseAddr,&pWiMAX->stMultiMIBNotification.MibObjs[u32Count*u32MIBsFragment],sizeof(MIB_OBJECT_INFO)*pMIBObj->u16NumMibObjs);
					puchBaseAddr+=sizeof(MIB_OBJECT_INFO)*pMIBObj->u16NumMibObjs;
				}
                else
				{
					pMIBObj->u16NumMibObjs=(UINT16)(pWiMAX->stMultiMIBNotification.u32NumMibObjs-(u32Count*u32MIBsFragment));
      				puchBaseAddr+= sizeof(UINT16)	;
					memcpy(puchBaseAddr,&pWiMAX->stMultiMIBNotification.MibObjs[u32Count*u32MIBsFragment],sizeof(MIB_OBJECT_INFO)*pMIBObj->u16NumMibObjs);
				
				}

			}

			pWiMAX->stMultiMIBNotification.u32MIBUpdateFlag=0;

		}
		
	
		puchBaseAddr = pRequestMessage->szData;
		for(u32Count=0;u32Count < u32totalChunks; u32Count++)
		{
			
			if (!pWiMAX->m_bMultipleMibThreadRunning)
			{
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
				   "Breaking out of loop, for Multiple Mib Thread.");
				goto returnCase;
			}
			
			iRet=BeceemWiMAX_ProcessMultipleMIBGetReq(pWiMAX, (PMIB_OBJECTS)puchBaseAddr,TRUE);

			
					
#ifdef WIN32
			Sleep(pWiMAX->stMultiMIBNotification.u32TimePeriod*1000);
#else
		    usleep(pWiMAX->stMultiMIBNotification.u32TimePeriod*1000*1000); /*convert millisecs to microsecs */
#endif
            if(iRet!=SUCCESS)
            {
				continue;
             }
            
            puchBaseAddr+=sizeof(UINT16)+sizeof(MIB_OBJECT_INFO)*u32MIBsFragment;

			if (!pWiMAX->m_bMultipleMibThreadRunning)
			{
			    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
				   "Breaking out of loop, for Multiple Mib Thread.");
				goto returnCase;
			}
		}

       
	}

returnCase:
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Exiting out of Multiple Mib thread.");
	BECEEM_MEMFREE(pRequestMessage)
	return SUCCESS;
}

int BeceemWiMAX_ProcessMultiplePeriodicIndicationReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECTS_TIMER pMIBObjs)
{
	INT iRet=0;
	
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	
	if(pMIBObjs->u16TimePeriod == 0)
	{
		if(pWiMAX->m_bMultipleMibThreadRunning)
		{
			/* Stop Periodic Multple Mib thread thread */
			if(Thread_Stop(&(pWiMAX->m_MultipleMibThread), (PTHREADSTOPFUNC)BeceemWiMAX_KillMultipleMibThread, pWiMAX) != SUCCESS)
			{
			    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Failed to stop Multple Mib thread!!!");
				Thread_Terminate(&(pWiMAX->m_MultipleMibThread)); 
			}
		    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Successfully stopped Multple Mib thread!!");
			
		}
        else
            {
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX,
			   "%s u16TimePeriod is zero and thread not running?!!", __FUNCTION__);
                return ERROR_INVALID_ARGUMENT;
            }
		
	}
	else if(pMIBObjs->u16TimePeriod > 0)
	{
		pWiMAX->stMultiMIBNotification.u32TimePeriod = pMIBObjs->u16TimePeriod;
		pWiMAX->stMultiMIBNotification.u32MIBsFragmentSize =pMIBObjs->u16MIBFragmentSize;
		pWiMAX->stMultiMIBNotification.u32MIBUpdateFlag=1;
		pWiMAX->stMultiMIBNotification.u32NumMibObjs=(UINT32)pMIBObjs->u16NumMibObjs;
		memcpy(&pWiMAX->stMultiMIBNotification.MibObjs[0],pMIBObjs->MibObjs,sizeof(MIB_OBJECT_INFO)*pWiMAX->stMultiMIBNotification.u32NumMibObjs);
		if(pWiMAX->m_bMultipleMibThreadRunning)
		{
			/* Return since thread is running */
			iRet=SUCCESS;
			goto returnCode;	
		}
		/* Start periodic Multiple Get thread */
		if(Thread_Run(&(pWiMAX->m_MultipleMibThread),(PTHREADFUNC)BeceemWiMAX_MultipleMibThreadFunc,pWiMAX))
		{
		    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Launching Multiple Mib Get thread failed!");
			iRet=ERROR_THREAD_NOT_STARTED;
			goto returnCode;
		}
	}
		
		
   	iRet=SUCCESS;
returnCode:
	return iRet;

}

int BeceemWiMAX_ProcessMultipleMIBGetReq(PBECEEMWIMAX pWiMAX, PMIB_OBJECTS pMIBObjs,UINT32 bFrmThread)
{
    PWIMAXMESSAGE pucOutBuffer = NULL;
    int iRet = 0, iRetval = 0, iSize = MAX_BUFFER_LENGTH;
    UINT uiAttr = 0;

    /* outgoing PWIMAXMESSAGE can handle max of 5 * 10K message size */
    UINT16 u16Count = 0;
    UINT uiTotalMibs = 0, uiOutMsgLen =0 ;
    PUCHAR pIBaseAddr = 0;
    MIB_OBJECT_INFO *pMIBObjInfo = NULL;
	PUCHAR pucOutDataBuffer=0;


	if(!pWiMAX->pStatsCacheData)
	{
		BECEEMCALLOC(pWiMAX->pStatsCacheData, PUCHAR, CHAR, sizeof(stInternalStatsStruct))
	}
    if(!pWiMAX->pstHostStatsCache)
        {
            BECEEMCALLOC(pWiMAX->pstHostStatsCache, S_MIBS_HOST_STATS_MIBS *, S_MIBS_HOST_STATS_MIBS,1)
        }
    if(!pWiMAX->pucMibOutBuffer && bFrmThread)
	{
		BECEEMCALLOC(pWiMAX->pucMibOutBuffer, PWIMAXMESSAGE, WIMAXMESSAGE, 1)
            pucOutBuffer=pWiMAX->pucMibOutBuffer; 
	}
    else if(pWiMAX->pucMibOutBuffer && bFrmThread)
    {
            pucOutBuffer=pWiMAX->pucMibOutBuffer;    
    }
    else if(!bFrmThread)
	{
		BECEEMCALLOC(pucOutBuffer, PWIMAXMESSAGE, WIMAXMESSAGE, 1)    
	}

	
	

	if(!pucOutBuffer)
	{

		return -1;
	}

   	

    memset(pWiMAX->pStatsCacheData,0,sizeof(stInternalStatsStruct));
    memset(pWiMAX->pstHostStatsCache,0,sizeof(S_MIBS_HOST_STATS_MIBS));
    
    iRet=BeceemWiMAX_GetStatsBufferChunkFrmDDR(pWiMAX);
    if(iRet!=SUCCESS)  
	{
		DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "BeceemWiMAX_GetStatsBufferChunkFrmDDR Fails: Error :%d \n",iRet);
		goto ret_path;
	}

    iRet=ReadHostStatsFromIOCTL(pWiMAX,pWiMAX->pstHostStatsCache);
    if(iRet!=SUCCESS)
	{	
		DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "ReadHostStatsFromIOCTL Fails. Error :%d\n",iRet);
		goto ret_path;
	}


    pWiMAX->m_bMultiMIBCall = 1;
	
	pucOutBuffer->stMessage.usType = TYPE_MIB;
	pucOutBuffer->stMessage.usSubType =SUBTYPE_MULTIPLE_MIB_GET_RESP;
	pIBaseAddr = pucOutBuffer->stMessage.szData;
	uiOutMsgLen= sizeof(UINT16);
    for (u16Count = 0; u16Count < pMIBObjs->u16NumMibObjs; u16Count++)
	{	

	    memcpy((pIBaseAddr + uiOutMsgLen),&pMIBObjs->MibObjs[u16Count], sizeof(MIB_OBJECT_INFO));
	 	pMIBObjInfo = (PMIB_OBJECT_INFO) (pIBaseAddr + uiOutMsgLen);

		pucOutDataBuffer=pIBaseAddr + uiOutMsgLen+sizeof(MIB_OBJECT_INFO)-1;

	    iRet = GetMIBData(pWiMAX, pMIBObjInfo,pucOutDataBuffer , (UINT *) & iSize);
	   

		if (iRet != 0)
		{
		    iSize = 0;
		    pMIBObjInfo->s8ErrorCode = (SINT8) iRet;
			pMIBObjInfo->u32MibObjSize=0;
		}
	    else
		{
			pMIBObjInfo->u32MibObjSize = iSize;
			if (iSize != 0)
			iSize -= 1;

		    uiAttr =pWiMAX->m_sMIBTable[pMIBObjInfo->u32MibObjId].u16MIBAttribute;

		    if (BIT_TGT_STD_CONFIG == (uiAttr & 0xFF00))
			{
			    switch (pMIBObjInfo->u32MibObjId)
				{
				    case WMAN_IF_SS_LOSTDLMAPINTERVAL:
				    case WMAN_IF_SS_LOSTULMAPINTERVAL:
				    case WMAN_IF_SS_T1TIMEOUT:
				    case WMAN_IF_SS_T2TIMEOUT:
				    case WMAN_IF_SS_T3TIMEOUT:
				    case WMAN_IF_SS_T6TIMEOUT:
				    case WMAN_IF_SS_T12TIMEOUT:
				    case WMAN_IF_SS_T14TIMEOUT:
				    case WMAN_IF_SS_T18TIMEOUT:
				    case WMAN_IF_SS_T20TIMEOUT:
				    case WMAN_IF_SS_T21TIMEOUT:
				    case WMAN_IF_CMN_T7TIMEOUT:
				    case WMAN_IF_CMN_T8TIMEOUT:
				    case WMAN_IF_CMN_T10TIMEOUT:
				    case WMAN_IF_CMN_T22TIMEOUT:
				    case WMAN_IF_2M_SS_T44TIMER:

						if(pMIBObjInfo->u32MibObjSize == sizeof(USHORT))
						{
							*(USHORT *)pMIBObjInfo->au8MibObjVal= (*(USHORT *)pMIBObjInfo->au8MibObjVal)*5;
						}
						else if(pMIBObjInfo->u32MibObjSize == sizeof(UCHAR))
						{
							*(UCHAR  *)pMIBObjInfo->au8MibObjVal = (*(UCHAR *)pMIBObjInfo->au8MibObjVal)*5;
						}
						else
						{
							*(UINT  *)pMIBObjInfo->au8MibObjVal = *(UINT *)pMIBObjInfo->au8MibObjVal * 5;
						}
						break;

				    case WMAN_IF_SS_T4TIMEOUT:
						memcpy(pMIBObjInfo->au8MibObjVal,
							   pMIBObjInfo->au8MibObjVal,
								pMIBObjInfo->u32MibObjSize);
					break;

				    default:
					break;
				}
			}
		   
		}

	    uiOutMsgLen+= (sizeof(MIB_OBJECT_INFO) + iSize);
	    uiTotalMibs += 1;

	    /* Get the iSize for the next MIB object before calculating for the next buffer packet */
	    {
		if ((u16Count + 1) < pMIBObjs->u16NumMibObjs)
		    iSize =
			pWiMAX->m_sMIBTable[pMIBObjs->MibObjs[(u16Count + 1)].
					    u32MibObjId].u32FieldSize;
		else
		    iSize = 0;

	    }

	    if (((uiOutMsgLen + sizeof(MIB_OBJECT_INFO) + iSize) >
		 MAX_BUFFER_LENGTH) || ((u16Count + 1) == pMIBObjs->u16NumMibObjs))
		{

		    UINT16 ui16TotalMibs = (UINT16) uiTotalMibs;

		    memcpy(pIBaseAddr, &ui16TotalMibs, sizeof(UINT16));

			pucOutBuffer->uiHeaderLen=SIZE_WIMAX_MESSAGE - MAX_VARIABLE_LENGTH;

			pucOutBuffer->uiMessageLen =(uiOutMsgLen + pucOutBuffer->uiHeaderLen);

			CorrectEndiannessForProxy(&pucOutBuffer->stMessage,uiOutMsgLen,1);
	
			BeceemWiMAX_CallAppCallback(pWiMAX, pucOutBuffer);
			uiOutMsgLen= sizeof(UINT16);
		    uiTotalMibs = 0;
		}
		
	}

    iRetval = SUCCESS;
ret_path:
    pWiMAX->m_bMultiMIBCall = 0;
   	if(!bFrmThread)
	{
		BECEEM_MEMFREE(pucOutBuffer)
	}
    
  
    return iRetval;

}

int BeceemWiMAX_ProcessMultipleMIBSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = SUCCESS, iLength = 0;
	UINT16 u16Count = 0, uiSize = 0,uiIndex = 0;
	PWIMAXMESSAGE pOWMMsg[5] = {0};
	PMIB_OBJECTS pMIBObj = 0;
	PMIB_OBJECT_INFO pIMIBObjInfo = 0, pOMIBObjInfo = 0;
	PUCHAR pIBaseAddr = 0, pOWMsgBaseAddr = 0;
	UINT uiTotalMibs = 0, uiOutMsgIdx = 0, uiOutMsgLen[5] = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UNUSED_ARGUMENT(uiLength);	
	/*  Send pMIBObject to application callback */
	if(!pWiMAX | !pMsg)
		return ERROR_INVALID_ARGUMENT;

	pMIBObj = (PMIB_OBJECTS)pMsg->szData;
	pIBaseAddr = (pMsg->szData + sizeof(UINT16));
	
	for (u16Count = 0; u16Count < pMIBObj->u16NumMibObjs; u16Count++)
	{
		if (!pOWMMsg[uiOutMsgIdx])
		{
			uiOutMsgLen[uiOutMsgIdx] = sizeof(UINT16);
				
			pOWMMsg[uiOutMsgIdx] = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);

			if(!pOWMMsg[uiOutMsgIdx])
				continue;
			
			memset (pOWMMsg[uiOutMsgIdx], 0, sizeof(WIMAXMESSAGE));
			pOWMMsg[uiOutMsgIdx]->stMessage.usType = TYPE_MIB;
			pOWMMsg[uiOutMsgIdx]->stMessage.usSubType = SUBTYPE_MULTIPLE_MIB_SET_RESP;
			
			pOWMsgBaseAddr = pOWMMsg[uiOutMsgIdx]->stMessage.szData;
		}

		// pointing to the MIB Object
		pIMIBObjInfo = (PMIB_OBJECT_INFO)(pIBaseAddr + (uiOutMsgLen[uiOutMsgIdx] - sizeof(UINT16)));
        
		pOMIBObjInfo = (PMIB_OBJECT_INFO)(pOWMsgBaseAddr + uiOutMsgLen[uiOutMsgIdx]);
		if(pIMIBObjInfo->u32MibObjSize==0)
            {
                UINT16 ui16TotalMibs=(UINT16)(uiTotalMibs+1);
			    memcpy(pOWMsgBaseAddr, &ui16TotalMibs, sizeof(UINT16));                
				uiOutMsgLen[uiOutMsgIdx] += sizeof(MIB_OBJECT_INFO);
				pOMIBObjInfo->u32MibObjId=pIMIBObjInfo->u32MibObjId;
				pOMIBObjInfo->s8ErrorCode = ERROR_INVALID_MIB_OBJ_SIZE;
				uiOutMsgIdx++;
				iRet=ERROR_INVALID_MIB_OBJ_SIZE;
                break;
                

            }

        uiSize = (UINT16)(pIMIBObjInfo->u32MibObjSize + sizeof(MIB_OBJECT_INFO) - 1);
		memcpy((pOWMsgBaseAddr  + uiOutMsgLen[uiOutMsgIdx]), pIMIBObjInfo, uiSize);

		
	
		/*Special READ-ONLY config MIBs.*/
		if(pOMIBObjInfo->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG
				|| pOMIBObjInfo ->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG
				|| pOMIBObjInfo ->u32MibObjId == WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG)
		{
			pOMIBObjInfo->s8ErrorCode = ERROR_MIB_OBJ_READONLY;	
		}
		/*Other Config MIBs*/
		else if(pOMIBObjInfo->u32MibObjId >= BCM_STANDARD_CFG_MIB_BASE_START 
				&& pOMIBObjInfo->u32MibObjId <= BCM_STANDARD_CFG_MIB_BASE_END)
		{
			/* STANDARD CONFIG MIBS */
			iRet = WriteStandardConfigMIB(pWiMAX, pOMIBObjInfo,TRUE);
			if(iRet != 0)
				// fill error code
			    pOMIBObjInfo->s8ErrorCode = (SINT8)iRet;
		}
		else if(pOMIBObjInfo->u32MibObjId >= BCM_PROPREITARY_CFG_MIB_BASE_START 
				&& pOMIBObjInfo->u32MibObjId <= BCM_PROPREITARY_CFG_MIB_BASE_END)
		{
			if(pOMIBObjInfo->u32MibObjSize != sizeof(UINT32))
				pOMIBObjInfo->s8ErrorCode = ERROR_INVALID_MIB_OBJ_SIZE;

			iRet = BeceemWiMAX_MIBWriteConfig(pWiMAX, pOMIBObjInfo);
			if(iRet != 0)
				// fill error code
			    pOMIBObjInfo->s8ErrorCode =(SINT8) iRet;
		}
		else if(pOMIBObjInfo->u32MibObjId == WMAN_IF_SS_PKMAUTHRESET)
		{
			iRet = WriteStandardStatsMIB(pWiMAX, pOMIBObjInfo);
			if(iRet != 0)
				// fill error code
			    pOMIBObjInfo->s8ErrorCode = (SINT8)iRet;
		}
		else if(pOMIBObjInfo->u32MibObjId >= BCM_PROPREITARY_STATS_MIB_BASE_START 
				&& pOMIBObjInfo->u32MibObjId <= BCM_PROPREITARY_STATS_MIB_BASE_END)
		{
			pOMIBObjInfo->s8ErrorCode = ERROR_MIB_OBJ_READONLY;
		}
		else
			pOMIBObjInfo->s8ErrorCode = ERROR_INVALID_MIB_OBJ_ID;

		uiOutMsgLen[uiOutMsgIdx] += uiSize;
		uiTotalMibs += 1;
		
		if (((uiOutMsgLen[uiOutMsgIdx] + sizeof(MIB_OBJECT_INFO)) > MAX_BUFFER_LENGTH) ||
			((u16Count + 1) == pMIBObj->u16NumMibObjs))
		{
		
			UINT16 ui16TotalMibs=(UINT16)uiTotalMibs;
			memcpy(pOWMsgBaseAddr, &ui16TotalMibs, sizeof(UINT16));

			uiOutMsgIdx++;
			uiTotalMibs = 0;
		}
	}
	if(pWiMAX->u32NVMIsFlashFlag)
		{
			Mutex_Lock(&(pWiMAX->m_ShMemMutex));
			iRet = WriteCachedMIBstoFlash(pWiMAX);
			Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		}
	

	for (u16Count = 0; u16Count < uiOutMsgIdx; u16Count++)
	{
	

		PWIMAXMESSAGE pWMMsg = NULL;
		
		if ((u16Count != 0) && (u16Count == uiOutMsgIdx))
			break;
		if(pWiMAX->u32NVMIsFlashFlag)
		{
			pMIBObj = (PMIB_OBJECTS) pOWMMsg[u16Count]->stMessage.szData;
			pIMIBObjInfo =  (PMIB_OBJECT_INFO)pMIBObj->MibObjs;
			iLength  = sizeof(UINT16);

			for(uiIndex = 0; uiIndex < pMIBObj->u16NumMibObjs ; uiIndex++)
			{
				pIMIBObjInfo = (PMIB_OBJECT_INFO)((UCHAR *)pMIBObj + iLength);
				pIMIBObjInfo->s8ErrorCode = (SINT8)pWiMAX->astFlashMIBWrites[uiIndex].i32ReturnStatus;
				iLength += sizeof(MIB_OBJECT_INFO) + pIMIBObjInfo->u32MibObjSize - 1;

			}
		}
		pOWMMsg[u16Count]->uiHeaderLen = SIZE_WIMAX_MESSAGE - MAX_VARIABLE_LENGTH;
		pOWMMsg[u16Count]->uiMessageLen = uiOutMsgLen[u16Count] + pOWMMsg[u16Count]->uiHeaderLen;		
	


		pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
		if(!pWMMsg)
		{
			
				
			BECEEM_MEMFREE (pOWMMsg[u16Count]);
			
			return ERROR_MALLOC_FAILED;
		}



			WiMAXMessage_Init(
						pWMMsg,
						TYPE_MIB, 
						SUBTYPE_MULTIPLE_MIB_SET_RESP, 
						(UCHAR*)pOWMMsg[u16Count]->stMessage.szData,
						uiOutMsgLen[u16Count]
						);



		BeceemWiMAX_CallAppCallback(pWiMAX,pWMMsg);

		BECEEM_MEMFREE (pOWMMsg[u16Count]);
		BECEEM_MEMFREE(pWMMsg)
	}
	if(pWiMAX->u32NVMIsFlashFlag)
	{
		memset(pWiMAX->astFlashMIBWrites,0,sizeof(ST_FLASH_PARAM_ID_WRITES)*MAX_FLASH_MIB_UPDATES_DONE_AT_A_TIME);
		pWiMAX->u32FlashMulitpleMIBWriteCount =0;
	}
	return iRet;
}

INT GetRealmInfo(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT	   iRetval=0,iIndex = 0;
	UINT32 u32SwappedValue = 0;
	PUCHAR pucData = NULL;
	UINT32 *pRealmAddress =(UINT32*) pucOutData;
	UINT32 *pNAIRealmData =  (UINT32*)pucData;
	MIB_OBJECT_INFO stMIBObj = {0};
	PUCHAR pucOutTempData = NULL;	
	INT iSize = MAX_BUFFER_LENGTH;	
	UINT32 RealInfoFlag=0;

	stMIBObj.u32MibObjId=BCM_BASEBAND_NAI_INFO_VALID;
	BECEEMCALLOC(pucOutTempData,PUCHAR,CHAR,MAX_BUFFER_LENGTH)	
	iRetval = SearchStatsData(pWiMAX, &stMIBObj, pucOutTempData, (UINT *)&iSize);	
	RealInfoFlag=*(UINT32*)pucOutTempData;

	if(RealInfoFlag == 0)
	{

#ifdef WIN32
		DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Realm info available is not vaild\n");
#else
		DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Realm info available is not vaild\n");
#endif
			iRetval=ERROR_MIB_OBJECT_NOTFOUND;
			goto returnCode;	
	}
	iRetval = BeceemWiMAX_ValidateRdmAddress((UINT32)*pRealmAddress);
	if (iRetval < 0)
	{
#ifdef WIN32
		DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Realm address is not Valid. Realm address : 0X%08X\n",(UINT32)*pRealmAddress);
#else
		DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "Realm address is not Valid. Realm address : 0X%08X\n",(UINT32)*pRealmAddress);
#endif
		goto returnCode;
	}

	
	 BECEEMCALLOC(pucData,PUCHAR,CHAR,BASEBAND_NAI_FIELD_SIZE)	
	
	if(BeceemWiMAX_rdmBulk(pWiMAX, *pRealmAddress, pucData, BASEBAND_NAI_FIELD_SIZE))
		{			
			iRetval=ERROR_INVALID_ADDRS_INPUT;
			goto returnCode;	
	}


	pNAIRealmData =  (UINT32*)pucData;


	for(iIndex = 0; iIndex < (BASEBAND_NAI_FIELD_SIZE/4);iIndex++)
	{
		u32SwappedValue = htonl(pNAIRealmData[iIndex]);
		
		memcpy(&pNAIRealmData[iIndex],&u32SwappedValue,sizeof(UINT32));
		
	}

	*puiOutDataLen = BASEBAND_NAI_FIELD_SIZE;
	
	memset(pucOutData,0,BASEBAND_NAI_FIELD_SIZE);
	memcpy(pucOutData,pucData,BASEBAND_NAI_FIELD_SIZE);

iRetval=SUCCESS;

returnCode:
	BECEEM_MEMFREE(pucData);
	BECEEM_MEMFREE(pucOutTempData);
	return iRetval;
}


int GetMRUData(PBECEEMWIMAX pWiMAX, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	UINT uiEEPROMChkSumInfo = 0, uiMruInfoIndex = 0;
	UINT8  u8HeadIndex = TAIL_MRU_INDEX; //init 
	INT iSize = 0, iRet = 0,iRetval=0, iAllocSize = 0;
	UINT uiOperatorIndex = 0, uiTreeLevel = 0,x = 0;
	OMADM_PARAMETER_INFO stOMADMParamInfo;
	PUCHAR	pucData = NULL;
	POMADM_OBJECT_INFO pstOMADMObj = NULL;
	ST_ChannelPlan	stChannelPlan[WIMAXSUPP_OPERATOR_NUMBER_OF_NODE] ;
	PMRU_ROW_FORMAT pstMruRows = NULL;
	PST_MRU_INFORMATION pstMruInfo = NULL;
    UINT maxMruInformation = 0;
    UINT uiWaitForInitCount = 0;

	OMA_DM_TREE_LEVEL stTestLevel[]={{OMA_DM_ST_ROOT_WIMAXSUPP,0},
										{OMA_DM_ST_WIMAXSUPP_OPERATOR,0},
										{OMA_DM_ST_OPERATOR_NETWORK_PARAMETERS,0},
										{OMA_DM_ST_NETWORK_PARAMETERS_CHANNEL_PLAN, 0},
									};

	memset(&stOMADMParamInfo,0,sizeof(OMADM_PARAMETER_INFO ));
	memset(&stChannelPlan,0,sizeof(ST_ChannelPlan)*WIMAXSUPP_OPERATOR_NUMBER_OF_NODE);
	pstMruInfo = (PST_MRU_INFORMATION)pucOutData;

    
	while(pWiMAX->m_bDeviceInitInProgress)
    {
        //only wait a max of 300 ms before returning
        //we don't want to get stuck here in an endless loop if there is some sort of problem w/ the device
        if(uiWaitForInitCount >= 3)
        {
            return ERROR_SHARED_MEM_NOT_INITIALIZED;
        }

    	DPRINT_COMMONAPI(DEBUG_MESSAGE,"Device INIT is in progress ");
#ifdef WIN32
	    Sleep(100);
#else
	    usleep(100000);
#endif
        uiWaitForInitCount++;
    }

	Mutex_Lock(&(pWiMAX->m_ShMemMutex));
    
    BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_INT_ADDR, &uiEEPROMChkSumInfo, 4);
	if (!EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,10) && 
		!EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,9))
    {
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
			return ERROR_MRU_OBJECT_NOTFOUND;
    }

	/* Get the Head Index */
	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (B_UINT8 *)&u8HeadIndex,
					sizeof(u8HeadIndex), CFG_MRU_HEAD_INDEX_ID);

	if(iRet < 0)
		{
			return ERROR_NVM_READ_FAILED;
		}
	/* Get MRU Row size */
	iSize = CFG_ParamSize((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, CFG_MRU_MRUDATA_ID);
	if (iSize < 0)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		return ERROR_MRU_OBJECT_NOTFOUND;
    }

    //
    iAllocSize = max(iSize,TAIL_MRU_INDEX*sizeof(MRU_ROW_FORMAT));

	pstMruRows = (PMRU_ROW_FORMAT)calloc(iAllocSize,1);
    if(!pstMruRows)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
        return ERROR_MALLOC_FAILED;
    }
    //validate that size is a safe minimum size.
  
	pucData = (PUCHAR)calloc(1,MAX_VARIABLE_LENGTH);
    if(!pucData)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
        BECEEM_MEMFREE(pstMruRows)
		return ERROR_MALLOC_FAILED;
    }
    
    for(x = 0; x < iAllocSize/sizeof(MRU_ROW_FORMAT); x++)
    {
        //set the tail index to an invalid index for each one of these.
        //in case of a read failure we don't want to be stuck in a loop.
        pstMruRows[x].NextIndex = TAIL_MRU_INDEX;
    }


	/* Fill the MRU data */
	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (B_UINT8 *)pstMruRows,
					iSize, CFG_MRU_MRUDATA_ID);
	if(iRet < 0)
		{
			iRetval=ERROR_NVM_READ_FAILED;
			goto returnCode;
		}
	/* Fetch the Channel Plan for all the Operators */
	for(uiOperatorIndex = 0; uiOperatorIndex< WIMAXSUPP_OPERATOR_NUMBER_OF_NODE;uiOperatorIndex++)
	{
		stTestLevel[1].u32FieldInstance = uiOperatorIndex;
		
		uiTreeLevel = sizeof(stTestLevel)/sizeof(stTestLevel[0]);
		memcpy(&stOMADMParamInfo.stTreeLevel, &stTestLevel, uiTreeLevel*sizeof(OMA_DM_TREE_LEVEL)); 
		stOMADMParamInfo.u32NumOfLevel = uiTreeLevel;

		iRet = BeceemWiMAX_ProcessOMADMGetParamSize(pWiMAX, &stOMADMParamInfo);
		if (iRet < 0)
		{
			iRetval=ERROR_MRU_OBJECT_NOTFOUND;
			goto returnCode;
		}

		pstOMADMObj = (POMADM_OBJECT_INFO)pucData;
		pstOMADMObj->u32NumOfLevel = sizeof(stTestLevel)/sizeof(stTestLevel[0]);
		pstOMADMObj->u32OMADMObjSize = stOMADMParamInfo.u32OMADMObjSize;
		memcpy(pstOMADMObj->stTreeLevel, &stTestLevel, uiTreeLevel*sizeof(OMA_DM_TREE_LEVEL));
		iRet = BeceemWiMAX_ProcessOMADMGetReq(pWiMAX, pstOMADMObj);
		if (iRet < 0)
		{
         	iRetval=ERROR_MRU_OBJECT_NOTFOUND;
			goto returnCode;
		}

		if(sizeof(ST_ChannelPlan) == pstOMADMObj->u32OMADMObjSize)
			memcpy(&stChannelPlan[uiOperatorIndex],pstOMADMObj->au8OMADMObjVal,sizeof(ST_ChannelPlan));
		else
		{
         	iRetval=ERROR_MRU_OBJECT_NOTFOUND;
			goto returnCode;
		}
	}
	

    maxMruInformation = (*puiOutDataLen)/sizeof(ST_MRU_INFORMATION);

    //the min of our out data,  or the minimum of TAIL_MRU_INDEX entries
    //
    maxMruInformation = min(TAIL_MRU_INDEX,maxMruInformation);

    //.. now we make sure that iSize is safe to use.
    maxMruInformation = min((iSize/sizeof(MRU_ROW_FORMAT)),maxMruInformation);

	uiOperatorIndex = 0;
	uiMruInfoIndex = 0;
	
	while (u8HeadIndex != TAIL_MRU_INDEX && uiMruInfoIndex < maxMruInformation)
	{
        //Just simplified the code to make it easier to read.
        PST_MRU_INFORMATION pstMruInfoCur = 0;
        PMRU_ROW_FORMAT pstMruRowCur = 0;
        ST_ChannelPlan* pOpChannelPlan = 0;
        ST_ChannelPlanEntries* pChannelPlanCur = 0;

        //get these right away.
        pstMruInfoCur = pstMruInfo + uiMruInfoIndex;
        pstMruRowCur = &pstMruRows[u8HeadIndex];

        //validate range.
        pOpChannelPlan = &stChannelPlan[uiOperatorIndex];
        
        //wanring we need to validate the range on this.

        if(pstMruRowCur->ChannelPlanIndex >= MAX_CHANNEL_PLAN_ENTRIES_PER_NSP)
        {
            break;
            //SOME ERROR CONDITION do not continue;
        }
        pChannelPlanCur = &pOpChannelPlan->Entries[pstMruRowCur->ChannelPlanIndex];

        /* Fill the channel plan details based on the head index */
        pstMruInfoCur->mruindex =  uiMruInfoIndex;
        switch(pstMruRowCur->WhichIsPrimary)	
        {
        case 1:
            pstMruInfoCur->freqList[0]	= pChannelPlanCur->FirstFreq;
            if(pstMruRowCur->WhichIsSecondary1 == 2)
            {
                pstMruInfoCur->freqList[1]	 = pChannelPlanCur->FirstFreq + pChannelPlanCur->NextFreqStep;

                pstMruInfoCur->freqList[2] = 	 pChannelPlanCur->LastFreq;
            }
            else if(pstMruRowCur->WhichIsSecondary1 == 3)
            {
                pstMruInfoCur->freqList[2]	 = pChannelPlanCur->FirstFreq + pChannelPlanCur->NextFreqStep;

                pstMruInfoCur->freqList[1] = 	 pChannelPlanCur->LastFreq;	
            }
            else
            {
			      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "INVALID MRU ENTRY");
            }
            break;

        case 2:
            pstMruInfoCur->freqList[0]	= pChannelPlanCur->FirstFreq + pChannelPlanCur->NextFreqStep;

            if(pstMruRowCur->WhichIsSecondary1 == 1)
            {
                pstMruInfoCur->freqList[1]	 = pChannelPlanCur->FirstFreq;						
                pstMruInfoCur->freqList[2] = 	 pChannelPlanCur->LastFreq;
            }
            else if(pstMruRowCur->WhichIsSecondary1 == 3)
            {
                pstMruInfoCur->freqList[2]	 = pChannelPlanCur->FirstFreq;

                pstMruInfoCur->freqList[1] = 	 pChannelPlanCur->LastFreq;	
            }
            else
            {
			      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "INVALID MRU ENTRY");
            }
            break;

        case 3:
            pstMruInfoCur->freqList[0]	= pChannelPlanCur->LastFreq;				

            if(pstMruRowCur->WhichIsSecondary1 == 2)
            {
                pstMruInfoCur->freqList[1]	 = pChannelPlanCur->FirstFreq + pChannelPlanCur->NextFreqStep;

                pstMruInfoCur->freqList[2] = 	 pChannelPlanCur->FirstFreq;
            }
            else if(pstMruRowCur->WhichIsSecondary1 == 1)
            {
                pstMruInfoCur->freqList[2]	 = pChannelPlanCur->FirstFreq + pChannelPlanCur->NextFreqStep;

                pstMruInfoCur->freqList[1] = 	 pChannelPlanCur->FirstFreq;	
            }
            else
            {
			      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "INVALID MRU ENTRY");
            }

            break;

        default:
		      DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "PRIMARY IS INVALID");
            break;
        }

        pstMruInfoCur->bwInfo = (pChannelPlanCur->BW/1000);

        if (pstMruRowCur->NextIndex == TAIL_MRU_INDEX)
            break;

        //is this the last one we can handle?
        if(uiMruInfoIndex+1 >= maxMruInformation)
            break;

        u8HeadIndex = (UINT8)pstMruRowCur->NextIndex;
        uiMruInfoIndex++;
    }
	
	if ((uiMruInfoIndex + 1) && (u8HeadIndex != TAIL_MRU_INDEX))
		*puiOutDataLen = ((uiMruInfoIndex + 1)* sizeof(ST_MRU_INFORMATION));
	else
	{	
     	iRetval=ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
		goto returnCode;
	}
 
	iRetval=SUCCESS;
returnCode:	
	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
	BECEEM_MEMFREE(pstMruRows)
	BECEEM_MEMFREE(pucData)
	return iRetval;	
}


int SetMRUData(PBECEEMWIMAX pWiMAX, PST_MRU_ADD_INFO pMruAddInfo)
{
    //new center freq.
    UINT uiNewBandwidth = pMruAddInfo->bwInfo;
    UINT ui2NewCenterFreq = pMruAddInfo->u32CenterFreq;
    UINT iChannelIndex = 0;
    UINT iFound = 0;
	UINT uiEEPROMChkSumInfo = 0, uiMruInfoIndex = 0;
	UINT8  u8HeadIndex = TAIL_MRU_INDEX; //init 
    UINT8  u8TailIndex = TAIL_MRU_INDEX; //init 
	INT iSize = 0, iRet = 0, iAllocSize = 0;
	UINT uiOperatorIndex = 0, uiTreeLevel = 0,x = 0;
	OMADM_PARAMETER_INFO stOMADMParamInfo ;
	UCHAR	ucData[MAX_VARIABLE_LENGTH];
	POMADM_OBJECT_INFO pstOMADMObj = NULL;
	ST_ChannelPlan	stChannelPlan[WIMAXSUPP_OPERATOR_NUMBER_OF_NODE];
	PMRU_ROW_FORMAT pstMruRows = NULL;
   UINT maxMruInformation = 0;
    UINT uiWaitForInitCount = 0;



	OMA_DM_TREE_LEVEL stTestLevel[]={{OMA_DM_ST_ROOT_WIMAXSUPP,0},
										{OMA_DM_ST_WIMAXSUPP_OPERATOR,0},
										{OMA_DM_ST_OPERATOR_NETWORK_PARAMETERS,0},
										{OMA_DM_ST_NETWORK_PARAMETERS_CHANNEL_PLAN, 0},
									};
    
	//pstMruInfo = (PST_MRU_INFORMATION)pucOutData;
	memset(&stOMADMParamInfo,0,sizeof(OMADM_PARAMETER_INFO));
	memset(stChannelPlan,0,sizeof(ST_ChannelPlan)*WIMAXSUPP_OPERATOR_NUMBER_OF_NODE);

	while(pWiMAX->m_bDeviceInitInProgress)
    {
        //only wait a max of 300 ms before returning
        //we don't want to get stuck here in an endless loop if there is some sort of problem w/ the device
        if(uiWaitForInitCount >= 3)
        {
            return ERROR_SHARED_MEM_NOT_INITIALIZED;
        }

    	DPRINT_COMMONAPI(DEBUG_MESSAGE,"Device INIT is in progress ");
#ifdef WIN32
	    Sleep(100);
#else
	    usleep(100000);
#endif

        BeceemWiMAX_EnsureEEPROMInitialized(pWiMAX);
        uiWaitForInitCount++;
    }

    Mutex_Lock(&(pWiMAX->m_ShMemMutex));


	BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_INT_ADDR, &uiEEPROMChkSumInfo, 4);
	if (!EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,10) && 
		!EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,9))
    {
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
			return ERROR_MRU_OBJECT_NOTFOUND;
    }

	/* Get the Head Index */
	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (B_UINT8 *)&u8HeadIndex,
					sizeof(u8HeadIndex), CFG_MRU_HEAD_INDEX_ID);
	if(iRet < 0)
		{
			return ERROR_NVM_READ_FAILED;
		}


    /* Get the Head Index */
    iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (B_UINT8 *)&u8TailIndex,
        sizeof(u8TailIndex), CFG_MRU_TAIL_INDEX_ID);

	if(iRet < 0)
		{
			return ERROR_NVM_READ_FAILED;
		}
	
	/* Get MRU Row size */
	iSize = CFG_ParamSize((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, CFG_MRU_MRUDATA_ID);
	if (iSize < 0)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		return ERROR_MRU_OBJECT_NOTFOUND;
    }

    //
    iAllocSize = max(iSize,TAIL_MRU_INDEX*sizeof(MRU_ROW_FORMAT));

    //alloc one extra
	pstMruRows = (PMRU_ROW_FORMAT)calloc((iAllocSize + sizeof(MRU_ROW_FORMAT)),1);
    if(!pstMruRows)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
        return ERROR_MALLOC_FAILED;
    }
    //validate that size is a safe minimum size.
    
    
    for(x = 0; x < (iAllocSize+sizeof(MRU_ROW_FORMAT))/sizeof(MRU_ROW_FORMAT); x++)
    {
        //set the tail index to an invalid index for each one of these.
        //in case of a read failure we don't want to be stuck in a loop.
        pstMruRows[x].NextIndex = TAIL_MRU_INDEX;
    }


	/* Fill the MRU data */
	iRet = CFG_ReadParam((CFG_UINT8 *)pWiMAX->pEEPROMContextTable, (B_UINT8 *)pstMruRows,
					iSize, CFG_MRU_MRUDATA_ID);

	if(iRet < 0)
		{
			return ERROR_NVM_READ_FAILED;
		}

	/* Fetch the Channel Plan for all the Operators */
	for(uiOperatorIndex = 0; uiOperatorIndex< WIMAXSUPP_OPERATOR_NUMBER_OF_NODE;uiOperatorIndex++)
	{
		stTestLevel[1].u32FieldInstance = uiOperatorIndex;
		
		uiTreeLevel = sizeof(stTestLevel)/sizeof(stTestLevel[0]);
		memcpy(&stOMADMParamInfo.stTreeLevel, &stTestLevel, uiTreeLevel*sizeof(OMA_DM_TREE_LEVEL)); 
		stOMADMParamInfo.u32NumOfLevel = uiTreeLevel;

		iRet = BeceemWiMAX_ProcessOMADMGetParamSize(pWiMAX, &stOMADMParamInfo);
		if (iRet < 0)
		{
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
			BECEEM_MEMFREE(pstMruRows);
			return ERROR_MRU_OBJECT_NOTFOUND;
		}

		pstOMADMObj = (POMADM_OBJECT_INFO)ucData;
		pstOMADMObj->u32NumOfLevel = sizeof(stTestLevel)/sizeof(stTestLevel[0]);
		pstOMADMObj->u32OMADMObjSize = stOMADMParamInfo.u32OMADMObjSize;
		memcpy(pstOMADMObj->stTreeLevel, &stTestLevel, uiTreeLevel*sizeof(OMA_DM_TREE_LEVEL));
		iRet = BeceemWiMAX_ProcessOMADMGetReq(pWiMAX, pstOMADMObj);
		if (iRet < 0)
		{
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
			BECEEM_MEMFREE(pstMruRows);	
			return ERROR_MRU_OBJECT_NOTFOUND;
		}

		if(sizeof(ST_ChannelPlan) == pstOMADMObj->u32OMADMObjSize)
			memcpy(&stChannelPlan[uiOperatorIndex],pstOMADMObj->au8OMADMObjVal,sizeof(ST_ChannelPlan));
		else
		{
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
			BECEEM_MEMFREE(pstMruRows);		
			return ERROR_MRU_OBJECT_NOTFOUND;
		}
	}

    //the min of our out data,  or the minimum of TAIL_MRU_INDEX entries
    //
    maxMruInformation = TAIL_MRU_INDEX;

    //.. now we make sure that iSize is safe to use.
    maxMruInformation = min((iSize/sizeof(MRU_ROW_FORMAT)),maxMruInformation);



    

	uiOperatorIndex = 0;
	uiMruInfoIndex = 0;

    //JS: this is not good, since the channel plan 
    //warning, what do we do if a channel plan contains more than 3 entires in the future.
    for(x = 0; x < MAX_CHANNEL_PLAN_ENTRIES_PER_NSP; x++)
    {
        ST_ChannelPlanEntries* pChPlan = &stChannelPlan[uiOperatorIndex].Entries[x];
        if(pChPlan->BW == uiNewBandwidth)
        {
            
            //dwFreq;
            if(pChPlan->FirstFreq == ui2NewCenterFreq)
            {
                iChannelIndex = x;
                iFound = 1;
                break;
            }
            else if(pChPlan->FirstFreq + pChPlan->NextFreqStep == ui2NewCenterFreq)
            {
                iChannelIndex = x;
                iFound = 2;
                break;
            }
            else if(pChPlan->LastFreq == ui2NewCenterFreq)
            {
                iChannelIndex = x;
                iFound = 3;
                break;
            }
        }
    }

    if(iFound == 0)
    {
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		BECEEM_MEMFREE(pstMruRows);
		return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
    }

    {

    
        UINT8 uStartIndex = u8HeadIndex;
        UINT8 uCurIndex = u8HeadIndex;
        UINT8 uBeforeTailIndex = uCurIndex;
        UINT8 uTailIndex = uCurIndex;
		UINT8 uiMruInfoIndex = 0;
        UINT8 nMruCount = 0;
        UINT8 nMatchingAt= TAIL_MRU_INDEX;
		UINT8 nPrevMatch = TAIL_MRU_INDEX;
		UINT8 bChange = 0;

		UINT32 nPossibleValues = 3;
        UINT32 possibleValues[3] = { 1, 2, 3 };
        UINT32 newOrder[3] = { 0x0, 0x0, 0x0};
        UINT32 origOrder[3] = { 0x0, 0x0, 0x0};
      //  maxMruInformation = 3;
        while (uCurIndex != TAIL_MRU_INDEX && 
            uiMruInfoIndex < maxMruInformation && 
            uCurIndex < maxMruInformation ) //if our index goes too high.. then we have an issue .. a big issue.
        {
            //makes it easier to read.
            PMRU_ROW_FORMAT pstMruRowCur = 0;
            pstMruRowCur = &pstMruRows[uCurIndex];

            if(nMatchingAt  == TAIL_MRU_INDEX && pstMruRowCur->ChannelPlanIndex == iChannelIndex )
            {
                nPrevMatch = uTailIndex;
                nMatchingAt  = uCurIndex;
            }

            //order
            uBeforeTailIndex = uTailIndex;
            uTailIndex = uCurIndex;


            uCurIndex = (UINT8)pstMruRowCur->NextIndex;
            uiMruInfoIndex++;
            if(uiMruInfoIndex >= maxMruInformation)
                break;

            //do this last..
            nMruCount++;
            
        }

        //something is wrong.. the mru table is busted!, this table is too big, or possibly has 
        //an issue with it not being connected correctly
        if(uCurIndex != TAIL_MRU_INDEX)
        {
            //	return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
            //}
            //THIS IS A CRITICAL ERROR! in this instance we would want to ideally reset the MRU table to give it a fresh start.
            //because something is really really messed up.
            if(CFG_ResetMRU((CFG_UINT8 *)pWiMAX->pEEPROMContextTable) < 0)
            	{
			DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "CFG_ResetMRU @SetMRUData");
            	}
            Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
            BECEEM_MEMFREE(pstMruRows);
            return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
        }
        else
        {
            PMRU_ROW_FORMAT pstMruRowCur = 0;

            //this means we are on the last one, but everything else is already filled.
            //to we have to use the current tail and make it the new node.
            if(uiMruInfoIndex == maxMruInformation && nMatchingAt == TAIL_MRU_INDEX)
            {
                PMRU_ROW_FORMAT pstMruRowPrev = 0;
                //something is wrong if either of these are true
                if(uTailIndex == TAIL_MRU_INDEX || uBeforeTailIndex == TAIL_MRU_INDEX)
                {
                    if(CFG_ResetMRU((CFG_UINT8 *)pWiMAX->pEEPROMContextTable) < 0)
						{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "CFG_ResetMRU @SetMRUData");
            			}
                    Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
                    BECEEM_MEMFREE(pstMruRows);
                    return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
                }

                //simply add a new one.
                pstMruRowCur = &pstMruRows[uTailIndex];
                pstMruRowCur->WhichIsPrimary = 1;
                pstMruRowCur->WhichIsSecondary1 = 2;
                pstMruRowCur->NextIndex = uStartIndex;
                pstMruRowCur->ChannelPlanIndex =(UINT16) iChannelIndex;
                uStartIndex = uTailIndex;

                bChange = TRUE;

                //the node was currently pointing to the original tail node
                pstMruRowPrev = &pstMruRows[uBeforeTailIndex];
                pstMruRowPrev->NextIndex = TAIL_MRU_INDEX;
                uTailIndex = uBeforeTailIndex;

            }
            else if(nMatchingAt == TAIL_MRU_INDEX)            
            {
                //simply add a new one.
                pstMruRowCur = &pstMruRows[nMruCount];
                pstMruRowCur->WhichIsPrimary = 1;
                pstMruRowCur->WhichIsSecondary1 = 2;
                pstMruRowCur->NextIndex = uStartIndex;
                pstMruRowCur->ChannelPlanIndex = (UINT16)iChannelIndex;
                uStartIndex = nMruCount;

                bChange = TRUE;
                if(nMruCount == 0)
                {   
                    uTailIndex = nMruCount;
                }
            }
            else
            {
                //simply add a new one.
                pstMruRowCur = &pstMruRows[nMatchingAt];
                pstMruRowCur->ChannelPlanIndex = (UINT16)iChannelIndex;
                //need to fix the link up

                //this was alreayd the first one.
                if(uStartIndex == nMatchingAt)
                {
                    //don'tneed to change anything.
                    //pstMruRowCur->NextIndex = TAIL_MRU_INDEX;
                    uStartIndex = nMatchingAt;
                }//we found the item before this
                else if(nPrevMatch != TAIL_MRU_INDEX && nPrevMatch != nMatchingAt)
                {
                    PMRU_ROW_FORMAT pstMruRowPrev = &pstMruRows[nPrevMatch];

                    bChange = TRUE;
                    pstMruRowPrev->NextIndex = pstMruRowCur->NextIndex;
                    if(pstMruRowCur->NextIndex == TAIL_MRU_INDEX)
                        uTailIndex = nPrevMatch;
                    pstMruRowCur->NextIndex = uStartIndex;
                }
                else
                {
                    if(CFG_ResetMRU((CFG_UINT8 *)pWiMAX->pEEPROMContextTable) < 0)
						{
					DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "CFG_ResetMRU @SetMRUData");
            			}
					
                    Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
                    BECEEM_MEMFREE(pstMruRows);
                    return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
                }
                uStartIndex = nMatchingAt;
            }
            
            //pstMruRowCur->WhichIsPrimary = 1;
            //pstMruRowCur->WhichIsSecondary1 = 2;
            origOrder[0] = pstMruRowCur->WhichIsPrimary;
            origOrder[1] = pstMruRowCur->WhichIsSecondary1;

            if(origOrder[0] == origOrder[1])
            {
                //they are duplicates so we want to get rid of it.
                origOrder[1] = 0;
            }

            //fill in the original complete order
            for(x = 0; x < nPossibleValues; x++)
            {
                UINT v = 0;
                UINT32 freqIndex = possibleValues[x];
                for(v = 0; v < nPossibleValues; v++)
                {
                    if(origOrder[v] == freqIndex)
                    {
                        //already used skip it.
                        break;
                    }
                    //an open slot, write it.
                    if(origOrder[v] == 0)
                    {
                        origOrder[v] = freqIndex;
                        break;
                    }
                }
            }

            newOrder[0] = iFound;

            for(x = 0; x < nPossibleValues; x++)
            {
                UINT v = 0;
                UINT32 freqIndex = origOrder[x];
                for(v = 0; v < 3; v++)
                {
                    if(newOrder[v] == freqIndex)
                    {
                        //already used skip it.
                        break;
                    }
                    //an open slot, write it.
                    if(newOrder[v] == 0)
                    {
                        newOrder[v] = freqIndex;
                        break;
                    }
                }
            }
            pstMruRowCur->WhichIsPrimary = (UINT16)newOrder[0];
            pstMruRowCur->WhichIsSecondary1 = (UINT16)newOrder[1];


            if(memcmp(origOrder,newOrder,sizeof(newOrder)) != 0)
            {
                bChange = TRUE;
            }

            if(bChange)
            {

				if(pWiMAX->u32NVMIsFlashFlag)
					{
						pWiMAX->u32FlashMulitpleMIBWriteCount = 0;
						memset(&pWiMAX->astFlashMIBWrites,0,sizeof(pWiMAX->astFlashMIBWrites));
				
						CacheMulitpleMIBData(pWiMAX,&uStartIndex,1,CFG_MRU_HEAD_INDEX_ID);
						CacheMulitpleMIBData(pWiMAX,&uTailIndex,1,CFG_MRU_TAIL_INDEX_ID);
						CacheMulitpleMIBData(pWiMAX,(UINT8 *)pstMruRows,iSize,CFG_MRU_MRUDATA_ID);
						iRet = WriteCachedMIBstoFlash(pWiMAX);
						if(iRet < 0)
						{
							iRet = ERROR_EEPROM_WRITE_FAILED;
							goto error_condition;
						}
						
					}
				else
					{
                /* Get the Head Index */
	    	           iRet = CFG_WriteParamMRU(pWiMAX->pEEPROMContextTable, (B_UINT8 *)&uStartIndex,
                    1, CFG_MRU_HEAD_INDEX_ID);
						if(iRet <= 0)
							{
										iRet = ERROR_EEPROM_WRITE_FAILED;
										goto error_condition;
							}
                /* Get the Head Index */
        		       iRet =  CFG_WriteParamMRU(pWiMAX->pEEPROMContextTable,(B_UINT8 *)&uTailIndex,
                    1, CFG_MRU_TAIL_INDEX_ID);
						if(iRet <= 0)
							{
										iRet = ERROR_EEPROM_WRITE_FAILED;
										goto error_condition;
							}

                /* Get MRU Row size */
                /* Fill the MRU data */
		              iRet =   CFG_WriteParamMRU(pWiMAX->pEEPROMContextTable, (B_UINT8 *)pstMruRows,
                    iSize, CFG_MRU_MRUDATA_ID);
						if(iRet <= 0)
								{
										iRet = ERROR_EEPROM_WRITE_FAILED;
										goto error_condition;
								}
					}
            }
        }
    }
	
	//if ((uiMruInfoIndex + 1) && (u8HeadIndex != TAIL_MRU_INDEX))
	//	*puiOutDataLen = ((uiMruInfoIndex + 1)* sizeof(ST_MRU_INFORMATION));
	//else
	//{	
	//	free(pstMruRows);
	//	return ERROR_MRU_CHANNEL_TABLE_IS_EMPTY;
	//}
    Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
	BECEEM_MEMFREE(pstMruRows);
	return SUCCESS;
	
error_condition:
	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
	BECEEM_MEMFREE(pstMruRows);
	return iRet;

}



int GetRxTxPacketHistorgram(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = -1;
	S_MIBS_HOST_STATS_MIBS *pStHostStats = NULL;
	DWORD dwNumBytesReturned = 0;

	
	BECEEMCALLOC(pStHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)
	iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_HOST_MIBS,
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							&dwNumBytesReturned
						);

    if(iRet)
	{
		BECEEM_MEMFREE(pStHostStats);	
		return iRet;
	}

	*puiOutDataLen = sizeof(UINT32) * MIBS_MAX_HIST_ENTRIES;
	if (pMIBObj->u32MibObjId == BCM_BASEBAND_RX_PACKETS_HISTOGRAM)
		memcpy(pucOutData, pStHostStats->stHostInfo.aRxPktSizeHist, *puiOutDataLen);
	else
		memcpy(pucOutData, pStHostStats->stHostInfo.aTxPktSizeHist, *puiOutDataLen);

	BECEEM_MEMFREE(pStHostStats);
	return SUCCESS;
}

int GetPacketsOrBytesDroppedPerSF(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	int iRet = -1;
	S_MIBS_HOST_STATS_MIBS *pStHostStats = NULL;
	DWORD dwNumBytesReturned = 0;

	BECEEMCALLOC(pStHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)

	
	iRet = DeviceIO_Send(
							&(pWiMAX->m_Device),
							IOCTL_BCM_GET_HOST_MIBS,
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							(PVOID)pStHostStats,
							sizeof(S_MIBS_HOST_STATS_MIBS),
							&dwNumBytesReturned
						);

    if(iRet)
	{
		BECEEM_MEMFREE(pStHostStats);	
		return iRet;
	}

	*puiOutDataLen = sizeof(UINT32);
	if (pMIBObj->u32MibObjId == BCM_BASEBAND_NUM_OF_BYTES_DROPPED_PER_SF)
		memcpy(pucOutData, &pStHostStats->astSFtable[pMIBObj->au32Key[0]].uiDroppedCountBytes, *puiOutDataLen);
	else
		memcpy(pucOutData, &pStHostStats->astSFtable[pMIBObj->au32Key[0]].uiDroppedCountPackets, *puiOutDataLen);

	BECEEM_MEMFREE(pStHostStats);
	return SUCCESS;
}

int GetNeighbourInfoFromBSLogger(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0,iRetval=0, iRecordsCount = 0, iNeighbourIndx, i;
	UINT32 uiStatsPtrBaseAddress = 0;
	INT32 iHOLoggerOffset = 0;
	PST_BSTRACKER_INFO pHOLogger = NULL;
	PST_BSTRACKING_INFO pBsLogger = NULL;
	PST_NEIGHBOUR_BWINFO pNeighbourInformation = NULL;
	UNUSED_ARGUMENT(pMIBObj);
	
	
	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}
	BECEEMCALLOC(pHOLogger,PST_BSTRACKER_INFO,ST_BSTRACKER_INFO,1)
	iHOLoggerOffset = (UADDTYPE)&((stInternalStatsStruct *)NULL)->tBcmPropStats.BcmBaseBandBSTrackingInfo;

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iHOLoggerOffset, (PUCHAR)pHOLogger, sizeof(ST_BSTRACKER_INFO));

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}	
	pNeighbourInformation = (PST_NEIGHBOUR_BWINFO)calloc(sizeof(ST_NEIGHBOUR_BWINFO),1);
	if(!pNeighbourInformation) {
		iRetval=ERROR_MALLOC_FAILED;
		goto returnCode;
	}
	iRecordsCount = pHOLogger->u32BSCount;
	pBsLogger = &pHOLogger->aBSLoggerInfo[0];

	
	for(i=0,iNeighbourIndx=0;i<iRecordsCount;i++)
	{
		if(!(pBsLogger->u32FlagBits & (1 << SERVING_BSID_BIT_POSITION)))
		{
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].u32PreambleIndex = pBsLogger->u32PreambleId;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].u32ChannelBandwidth = pBsLogger->u32Bandwidth;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].u32CentreFreq = pBsLogger->u32Frequency;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].u32BSIDMsb = pBsLogger->u32BSIdHigh;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].u32BSIDLsb = pBsLogger->u32BSIdLow;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].s32Rssi = (pBsLogger->s32RSSI/(1<<16))*4;
			pNeighbourInformation->stNbrInfo[iNeighbourIndx].s32Cinr = pBsLogger->s32R1CINR/(1<<16);
			iNeighbourIndx++;
		}	
		pBsLogger++;

	}
	pNeighbourInformation->u32BSCount = iNeighbourIndx;

	memcpy(pucOutData,pNeighbourInformation,sizeof(ST_NEIGHBOUR_BWINFO));

	*puiOutDataLen = sizeof(ST_NEIGHBOUR_BWINFO);
	iRetval=SUCCESS;


returnCode:
	BECEEM_MEMFREE(pHOLogger)
	BECEEM_MEMFREE(pNeighbourInformation)
	return iRetval;

}



int GetPduRcvStatInformation(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0,iRetval=0, iPerZoneInfoOffset = 0,i = 0;
	ST_PER_ZONE_MCS_STATS *pstMcsStats = NULL;
	UINT32 uiStatsPtrBaseAddress = 0;
	UINT32 Total_PDUs_QPSK12 = 0;
	UINT32 Total_PDUs_QPSK34 = 0;
	UINT32 Total_PDUs_16QAM12 = 0;
	UINT32 Total_PDUs_16QAM34 = 0;
	UINT32 Total_PDUs_64QAM12 = 0;
	UINT32 Total_PDUs_64QAM23 = 0;
	UINT32 Total_PDUs_64QAM34 = 0;
	UINT32 Total_PDUs_64QAM56 = 0;
	UINT32 Total_PDUs_QPSK12_CTCREP6 = 0;
	UINT32 Total_PDUs_QPSK12_CTCREP4 = 0;
	UINT32 Total_PDUs_QPSK12_CTCREP2 = 0;
	UINT32 Total_PDUs_QPSK12_CTCREP1 = 0;
	UINT32 Total_PDUs_QPSK12_CCREP6 = 0;
	UINT32 Total_PDUs_QPSK12_CCREP4 = 0;
	UINT32 Total_PDUs_QPSK12_CCREP2 = 0;
	UINT32 Total_PDUs_QPSK12_CCREP1 = 0;


	UINT32 Total_BURSTs_QPSK12_CTCREP6 = 0;
	UINT32 Total_BURSTs_QPSK12_CTCREP4 = 0;
	UINT32 Total_BURSTs_QPSK12_CTCREP2 = 0;
	UINT32 Total_BURSTs_QPSK12_CTCREP1 = 0;
	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}
	 BECEEMCALLOC(pstMcsStats,ST_PER_ZONE_MCS_STATS *,ST_PER_ZONE_MCS_STATS,1)

	iPerZoneInfoOffset = (UADDTYPE)&((stInternalStatsStruct *)NULL)->tBcmPropStats.BcmBasebandPerZoneMCS;

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iPerZoneInfoOffset, (PUCHAR)pstMcsStats, sizeof(ST_PER_ZONE_MCS_STATS));

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}


	for(i=0; i<NUMBER_OF_DL_ZONE_TYPES; i++)
	{
		Total_PDUs_QPSK12_CTCREP1 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTCQpsk12Rate1;

		Total_PDUs_QPSK12_CTCREP2 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTCQpsk12Rate2;

		Total_PDUs_QPSK12_CTCREP4 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTCQpsk12Rate4;

		Total_PDUs_QPSK12_CTCREP6 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTCQpsk12Rate6;
		
		Total_PDUs_QPSK12_CCREP1 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCCQpsk12Rate1;

		Total_PDUs_QPSK12_CCREP2 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCCQpsk12Rate2;

		Total_PDUs_QPSK12_CCREP4 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCCQpsk12Rate4;

		Total_PDUs_QPSK12_CCREP6 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCCQpsk12Rate6;		

		Total_PDUs_16QAM12 		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCC16Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCC16Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCC16Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC16Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC16Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC16Qam12;

		Total_PDUs_16QAM34 		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCC16Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCC16Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCC16Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC16Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC16Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC16Qam34;

		Total_PDUs_64QAM12		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCC64Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCC64Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCC64Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC64Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC64Qam12
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC64Qam12;

		Total_PDUs_64QAM23		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCC64Qam23
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCC64Qam23
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCC64Qam23
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC64Qam23
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC64Qam23
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC64Qam23;

		Total_PDUs_64QAM34		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCC64Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCC64Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCC64Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC64Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC64Qam34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC64Qam34;

		Total_PDUs_64QAM56		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTC64Qam56
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTC64Qam56
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTC64Qam56;		

		Total_PDUs_QPSK34		 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCCQpsk34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCCQpsk34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCCQpsk34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatA.BcmBasebandPduRcvCTCQpsk34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tMimoMatB.BcmBasebandPduRcvCTCQpsk34
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSPdus.tNonMimo.BcmBasebandPduRcvCTCQpsk34;
		Total_BURSTs_QPSK12_CTCREP1 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatA.BcmBasebandBurstRcvCTCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatB.BcmBasebandBurstRcvCTCQpsk12Rate1
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tNonMimo.BcmBasebandBurstRcvCTCQpsk12Rate1;

		Total_BURSTs_QPSK12_CTCREP2 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatA.BcmBasebandBurstRcvCTCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatB.BcmBasebandBurstRcvCTCQpsk12Rate2
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tNonMimo.BcmBasebandBurstRcvCTCQpsk12Rate2;

		Total_BURSTs_QPSK12_CTCREP4 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatA.BcmBasebandBurstRcvCTCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatB.BcmBasebandBurstRcvCTCQpsk12Rate4
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tNonMimo.BcmBasebandBurstRcvCTCQpsk12Rate4;

		Total_BURSTs_QPSK12_CTCREP6 += pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatA.BcmBasebandBurstRcvCTCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tMimoMatB.BcmBasebandBurstRcvCTCQpsk12Rate6
									+ pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts.tNonMimo.BcmBasebandBurstRcvCTCQpsk12Rate6;		

	}


	


	switch(pMIBObj->u32MibObjId)
	{
		case BCM_STATS_PDURCVQPSK12:
			{
				Total_PDUs_QPSK12 = Total_PDUs_QPSK12_CTCREP1 + Total_PDUs_QPSK12_CTCREP2 + Total_PDUs_QPSK12_CTCREP4 + Total_PDUs_QPSK12_CTCREP6
									+ Total_PDUs_QPSK12_CCREP1 + Total_PDUs_QPSK12_CCREP2 + Total_PDUs_QPSK12_CCREP4 + Total_PDUs_QPSK12_CCREP6;
				

				memcpy(pucOutData, &Total_PDUs_QPSK12, sizeof(UINT32));	
				*puiOutDataLen = sizeof(UINT32);
				
			}
			break;

		case BCM_STATS_PDURCVQPSK34:
			{
				memcpy(pucOutData, &Total_PDUs_QPSK34, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCV16QAM12:
			{
				memcpy(pucOutData, &Total_PDUs_16QAM12, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCV16QAM34:
			{
				memcpy(pucOutData, &Total_PDUs_16QAM34, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCV64QAM12:
			{
				memcpy(pucOutData, &Total_PDUs_64QAM12, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCV64QAM23:
			{
				memcpy(pucOutData, &Total_PDUs_64QAM23, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
				
			}
			break;

		case BCM_STATS_PDURCV64QAM34:
			{
				memcpy(pucOutData, &Total_PDUs_64QAM34, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
				
			}
			break;

		case BCM_STATS_PDURCV64QAM56:
			{
				memcpy(pucOutData, &Total_PDUs_64QAM56, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
				
			}
			break;

		case BCM_STATS_PDURCVQPSK12CTCREP6:
			{
				memcpy(pucOutData, &Total_PDUs_QPSK12_CTCREP6, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCVQPSK12CTCREP4:
			{
				memcpy(pucOutData, &Total_PDUs_QPSK12_CTCREP4, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCVQPSK12CTCREP2:
			{
				memcpy(pucOutData, &Total_PDUs_QPSK12_CTCREP2, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_STATS_PDURCVQPSK12CTCREP1:
			{
				memcpy(pucOutData, &Total_PDUs_QPSK12_CTCREP1, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;				
		case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP6:
			{
				memcpy(pucOutData, &Total_BURSTs_QPSK12_CTCREP6, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP4:
			{
				memcpy(pucOutData, &Total_BURSTs_QPSK12_CTCREP4, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP2:
			{
				memcpy(pucOutData, &Total_BURSTs_QPSK12_CTCREP2, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;

		case BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP1:
			{
				memcpy(pucOutData, &Total_BURSTs_QPSK12_CTCREP1, sizeof(UINT32));
				*puiOutDataLen = sizeof(UINT32);
			}
			break;				
			
	}

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pstMcsStats)
	return iRetval;
	
	
}

int GetTriggerStatusInfo(PBECEEMWIMAX pWiMAX, PMIB_OBJECT_INFO pMIBObj, UCHAR * pucOutData, UINT * puiOutDataLen)
{
	PST_BSTRACKER_INFO pHOLogger = NULL;
	PST_BSTRACKING_INFO pBsLogger = NULL;
	INT32 iRet = 0,iRetval=0, iRecordsCount = 0, i = 0, s32CINR = 0;
	INT32 iHOLoggerOffset = 0, iInterFaBSNotTracked = 0;
	UINT32 uiStatsPtrBaseAddress = 0, uiServingBSFreq = 0;
	ST_TRIGGER_STATUS_INFO stTriggerStatusInfo = {0};
	ST_TRIGGER_INFO stTriggerInfo = {0};

    DOUBLE dTriggerThreshold=0;
    DOUBLE dRSSI=0;
    DOUBLE dCINR=0;
				
    UNUSED_ARGUMENT(pMIBObj);
	
	memcpy(&stTriggerInfo, &pWiMAX->stTriggerInformation, sizeof(ST_TRIGGER_INFO));
	memcpy(&stTriggerStatusInfo.stTriggerInfo, &pWiMAX->stTriggerInformation,sizeof(ST_TRIGGER_INFO));
	
	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

	BECEEMCALLOC(pHOLogger,PST_BSTRACKER_INFO,ST_BSTRACKER_INFO,1)
	
	iHOLoggerOffset = (UADDTYPE)&((stInternalStatsStruct *)NULL)->tBcmPropStats.BcmBaseBandBSTrackingInfo;

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iHOLoggerOffset, (PUCHAR)pHOLogger, sizeof(ST_BSTRACKER_INFO));

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}	

	iRecordsCount = pHOLogger->u32BSCount;
	pBsLogger = pHOLogger->aBSLoggerInfo;
	

	if(iRecordsCount)
	{
		/* Initializing to TRIGGER_STATUS_PASSED */
		stTriggerStatusInfo.eServingBSStatus = TRIGGER_STATUS_PASSED;
		stTriggerStatusInfo.eIntraFANeighboursStatus = TRIGGER_STATUS_PASSED;
		stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_PASSED;
		
		/* Find serving BS ,serving BS Frequency and assign status */
		for(i=0; i<iRecordsCount; i++)
		{
			if(pBsLogger->u32FlagBits & BSLOGGER_SERVING_BS_INDICATOR)
			{
				uiServingBSFreq = pBsLogger->u32Frequency;
				if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_RSSI)
				{
				    
				    dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dRSSI=(DOUBLE)(pBsLogger->s32RSSI/(DOUBLE)(1<<16));
                    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "dRSSI: %f dTriggerThreshold: %f ",dRSSI,dTriggerThreshold);
    				if(dRSSI >= dTriggerThreshold)
						stTriggerStatusInfo.eServingBSStatus = TRIGGER_STATUS_FAILED;
					
				}
				else if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_CINR)
				{

    				s32CINR = (pBsLogger->u32FlagBits & BSLOGGER_R1_CINR_VALID_BIT) ? pBsLogger->s32R1CINR: pBsLogger->s32R3CINR;
                    dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dCINR=(DOUBLE)(s32CINR/(DOUBLE)(1<<16));
                    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "dCINR: %f dTriggerThreshold: %f ",dCINR,dTriggerThreshold);
    				if( dCINR>=dTriggerThreshold)
						stTriggerStatusInfo.eServingBSStatus = TRIGGER_STATUS_FAILED;
					
				}
				
			}

			pBsLogger++;
		}

		pBsLogger = pHOLogger->aBSLoggerInfo;


		/* Find status for IntraFA and InterFA neighbours */

		for(i=0; i<iRecordsCount; i++)
		{

			/* IntraFA neighbours */
			if(pBsLogger->u32Frequency == uiServingBSFreq && (pBsLogger->u32FlagBits & BSLOGGER_SERVING_BS_INDICATOR)==0)
			{
				if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_RSSI)
				{
				    dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dRSSI=(DOUBLE)(pBsLogger->s32RSSI/(DOUBLE)(1<<16));

					if(dRSSI >= dTriggerThreshold)
						stTriggerStatusInfo.eIntraFANeighboursStatus = TRIGGER_STATUS_FAILED;
				}
				else if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_CINR)
				{
					s32CINR = (pBsLogger->u32FlagBits & BSLOGGER_R1_CINR_VALID_BIT) ? pBsLogger->s32R1CINR: pBsLogger->s32R3CINR;

                    dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dCINR=(DOUBLE)(s32CINR/(DOUBLE)(1<<16));
					if( dCINR>=dTriggerThreshold)	
                        stTriggerStatusInfo.eIntraFANeighboursStatus = TRIGGER_STATUS_FAILED;
					
				}
				
			}

			/* InterFA Neighbours */
			else if(pBsLogger->u32Frequency != uiServingBSFreq)
			{
				if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_RSSI)
				{
				    dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dRSSI=(DOUBLE)(pBsLogger->s32RSSI/(DOUBLE)(1<<16));
					if(dRSSI >= dTriggerThreshold)
						stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_FAILED;
				}
				else if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_CINR)
				{
					s32CINR = (pBsLogger->u32FlagBits & BSLOGGER_R1_CINR_VALID_BIT) ? pBsLogger->s32R1CINR: pBsLogger->s32R3CINR;
					
					dTriggerThreshold=(DOUBLE)stTriggerInfo.s32TriggerThreshold;
                    dCINR=(DOUBLE)(s32CINR/(DOUBLE)(1<<16));
					if( dCINR>=dTriggerThreshold)
						stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_FAILED;
					
				}
				
				if((pBsLogger->u32FlagBits & BSLOGGER_CURRENTLY_TRACKING_BIT) == 0)
					iInterFaBSNotTracked = 1;

			}
			pBsLogger++;
				
		}
		
		if(iInterFaBSNotTracked) 
		{
			stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_NO_MEASUREMENTS;
		}

		if(stTriggerInfo.eTriggerType == TRIGGER_TYPE_NOT_DEFINED)
		{
			stTriggerStatusInfo.eServingBSStatus = TRIGGER_STATUS_NOT_DEFINED;
			stTriggerStatusInfo.eIntraFANeighboursStatus = TRIGGER_STATUS_NOT_DEFINED;
			stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_NOT_DEFINED;
		}
		
	
	}
	else
	{
		/* If no records are found, assigning status to "No measurements available" */
		stTriggerStatusInfo.eServingBSStatus = TRIGGER_STATUS_NO_MEASUREMENTS;
		stTriggerStatusInfo.eIntraFANeighboursStatus = TRIGGER_STATUS_NO_MEASUREMENTS;
		stTriggerStatusInfo.eInterFANeighboursStatus = TRIGGER_STATUS_NO_MEASUREMENTS;
		
	}

	memcpy(pucOutData,&stTriggerStatusInfo,sizeof(ST_TRIGGER_STATUS_INFO));

	*puiOutDataLen = sizeof(ST_TRIGGER_STATUS_INFO);
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pHOLogger)
	return iRetval;
	
}


int BeceemWiMAX_neighbour_stats_wrapper(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,
                                                UCHAR *pucOutData,UINT *puiOutDataLen)
{
    int iRet = 0;
    PST_BSTRACKER_INFO pBcmBaseBandBSTrackingInfo=NULL;
    UINT32 uiStatsPtrBaseAddress = 0;
	INT32 numBsTrackingRecords=0;
	ST_BSTRACKING_INFO stServingBsRecods;
	double dServingBSFreq;
	
     
	UINT32 i,index=0;
	UINT32 u32ServingBSPreambleId;
    memset(pucOutData,0,(sizeof(SINT32)*DM_MAX_NBRS));

	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

    BECEEMCALLOC_RET(pBcmBaseBandBSTrackingInfo,PST_BSTRACKER_INFO,ST_BSTRACKER_INFO,1,int)

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, (UINT)(uiStatsPtrBaseAddress+(UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBaseBandBSTrackingInfo),(UCHAR *)(pBcmBaseBandBSTrackingInfo), (sizeof(ST_BSTRACKER_INFO)));


	if(iRet)
		return iRet;

	for(numBsTrackingRecords=0;numBsTrackingRecords<MAX_BS_TRACKING_RECORDS;numBsTrackingRecords++)
	{
		
		
		if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[numBsTrackingRecords].u32FlagBits & (1 << SERVING_BSID_BIT_POSITION))
		{
			memcpy(&stServingBsRecods,&pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[numBsTrackingRecords],sizeof(ST_BSTRACKING_INFO));
			break;
			
		}
	}

	
	dServingBSFreq=pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[numBsTrackingRecords].u32Frequency;
	u32ServingBSPreambleId=pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[numBsTrackingRecords].u32PreambleId;
	
	
	switch(pMIBObj->u32MibObjId)
	{
		case BCM_STATS_INTRA_FA_PREAMBLE_INDEX:
		{

			for(i=0;i<pBcmBaseBandBSTrackingInfo->u32BSCount;i++)
			{
				if((double)pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32Frequency==dServingBSFreq)
					if (pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32PreambleId != u32ServingBSPreambleId)
				((UINT32*)pucOutData)[index++]=pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32PreambleId;	
			}
		}
		break;
	
		case BCM_STATS_BCMBSINTRAFAMEANCINR:
		{

			for(i=0;i<pBcmBaseBandBSTrackingInfo->u32BSCount;i++)
			{
				if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32Frequency==dServingBSFreq)
				{ 
					if (pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32PreambleId != u32ServingBSPreambleId)
					{
						if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32FlagBits & (1 << REUSE1_CINR_VALID_BIT_POSITION))          
					   	{
        					((SINT32*)pucOutData)[index++]=((pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].s32R1CINR)/(1<<16));
                        }
						else if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32FlagBits & (1 << REUSE3_CINR_VALID_BIT_POSITION ))          
                        {
        					((SINT32*)pucOutData)[index++]=((pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].s32R3CINR)/(1<<16));	
                        }
					}
                    
                }
			}
			
		}
		break;
			
		case BCM_STATS_INTER_FA_PREAMBLE_INDEX:
		{

			for(i=0;i<pBcmBaseBandBSTrackingInfo->u32BSCount;i++)
			{
				if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32Frequency!=dServingBSFreq)
					((UINT32*)pucOutData)[index++]=pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32PreambleId;
			}
		}
		break;
			
		case BCM_STATS_BCMBSINTERFAMEANCINR:
			{	

				for(i=0;i<pBcmBaseBandBSTrackingInfo->u32BSCount;i++)
				{
					if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32Frequency!=dServingBSFreq)
                        {               
                           if(pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32FlagBits & (1 << REUSE1_CINR_VALID_BIT_POSITION))
                            {
        	        		((SINT32*)pucOutData)[index++]=((pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].s32R1CINR)/(1<<16));	
                            }
                           else if (pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].u32FlagBits & (1 <<REUSE3_CINR_VALID_BIT_POSITION ))
                            {
        					((SINT32*)pucOutData)[index++]=((pBcmBaseBandBSTrackingInfo->aBSLoggerInfo[i].s32R3CINR)/(1<<16));
                            }
                       }
                            
				}
			}
			break;

		
	}

	BECEEM_MEMFREE(pBcmBaseBandBSTrackingInfo)
	*puiOutDataLen = (sizeof(UINT32)*(index));
	return SUCCESS;


}


int GetMcsStatInformation(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0, iDlMcsOffset = 0, iUlMcsOffset = 0;
    INT32 stDlMcsStats[NUMBER_OF_FEC_TYPES];
    INT32 stUlMcsStats[NUMBER_OF_FEC_TYPES];
	UINT32 uiStatsPtrBaseAddress = 0;
	int result = 0;

	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

	iDlMcsOffset = (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.au32DLMCS;
	iUlMcsOffset = (UADDTYPE)&((stInternalStatsStruct * )NULL)->Statistics.au32ULMCS;
		
	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iDlMcsOffset, (PUCHAR)&stDlMcsStats, (sizeof(INT32) * NUMBER_OF_FEC_TYPES));

	if(iRet)
		return iRet;

    iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iUlMcsOffset, (PUCHAR)&stUlMcsStats, (sizeof(INT32) * NUMBER_OF_FEC_TYPES));

	if(iRet)
		return iRet;
	

	switch(pMIBObj->u32MibObjId)
	{
		case BCM_STATS_BURSTRCVQPSK12:
			{
				result = stDlMcsStats[0]+ stDlMcsStats[13];					
			}
			break;

		case BCM_STATS_BURSTRCVQPSK34:
			{
				result = stDlMcsStats[1]+ stDlMcsStats[15];	
			}
			break;

		case BCM_STATS_BURSTRCV16QAM12:
			{
				result = stDlMcsStats[2]+ stDlMcsStats[16];
			}
			break;

		case BCM_STATS_BURSTRCV16QAM34:
			{
				result = stDlMcsStats[3]+ stDlMcsStats[17];
			}
			break;

		case BCM_STATS_BURSTRCV64QAM12:
			{
				result = stDlMcsStats[4]+ stDlMcsStats[18];
			}
			break;

		case BCM_STATS_BURSTRCV64QAM23:
			{
				result = stDlMcsStats[5]+ stDlMcsStats[19];
			}
			break;

		case BCM_STATS_BURSTRCV64QAM34:
			{
				result = stDlMcsStats[6]+ stDlMcsStats[20];
			}
			break;

		case BCM_STATS_BURSTRCV64QAM56:
			{
				result = stDlMcsStats[21];
			}
			break;

		case BCM_STATS_BURSTSNDQPSK12:
			{
				result = stUlMcsStats[0]+ stUlMcsStats[13];
			}
			break;

		case BCM_STATS_BURSTSNDQPSK34:
			{
				result = stUlMcsStats[1]+ stUlMcsStats[15];
			}
			break;

		case BCM_STATS_BURSTSND16QAM12:
			{
				result = stUlMcsStats[2]+ stUlMcsStats[16];
			}
			break;

		case BCM_STATS_BURSTSND16QAM34:
			{
				result = stUlMcsStats[3]+ stUlMcsStats[17];
			}
			break;


	}
	memcpy(pucOutData, &result, sizeof(UINT32));	
	*puiOutDataLen = sizeof(UINT32);

	return SUCCESS;
	
	
}

int GetDlZoneStatInformation(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0,iRetval=0, iPerZoneInfoOffset = 0,i = 0;
	ST_PER_ZONE_MCS_STATS *pstMcsStats = NULL;
	ST_PERDLZONE_PER_MCS_BURSTS_DECODED *pstPerDlZoneSts = NULL;
	INT32 uiStatsPtrBaseAddress = 0;
	INT32 * perMcsPtr1;
	INT32 * perMcsPtr2;
	INT32 * dlMcsPtr;
	UNUSED_ARGUMENT(pMIBObj);
	BECEEMCALLOC(pstPerDlZoneSts,ST_PERDLZONE_PER_MCS_BURSTS_DECODED *,ST_PERDLZONE_PER_MCS_BURSTS_DECODED,NUM_DL_ZONES)
	pstMcsStats = (ST_PER_ZONE_MCS_STATS *)calloc(sizeof(ST_PER_ZONE_MCS_STATS),1);
	if(!pstMcsStats)
	{
		iRetval=ERROR_MALLOC_FAILED;
		goto returnCode;
	}

	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS,(UINT32 *) &uiStatsPtrBaseAddress, sizeof(INT32));
	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		iRetval=ERROR_NO_STATISTIC_RESPONSE;
		goto returnCode;
	}
	iPerZoneInfoOffset = (UADDTYPE)&((stInternalStatsStruct *)NULL)->tBcmPropStats.BcmBasebandPerZoneMCS;

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iPerZoneInfoOffset, (PUCHAR)pstMcsStats, sizeof(ST_PER_ZONE_MCS_STATS));

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}

	
	
	perMcsPtr1 = (INT32*)&pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[0].stPerDLZonePerMCSBursts;
    perMcsPtr2 = (INT32*)&pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[7].stPerDLZonePerMCSBursts;
    dlMcsPtr  =  (INT32*)&pstPerDlZoneSts[0];

    for(i=0; i< sizeof(ST_PERDLZONE_PER_MCS_BURSTS_DECODED)/sizeof(UINT32);i++)
    {
       dlMcsPtr[i] = perMcsPtr1[i] + perMcsPtr2[i];    
    }

	perMcsPtr1 = (INT32*)&pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[1].stPerDLZonePerMCSBursts;
    perMcsPtr2 = (INT32*)&pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[8].stPerDLZonePerMCSBursts;
    dlMcsPtr  =  (INT32*)&pstPerDlZoneSts[1];

    for(i=0; i< sizeof(ST_PERDLZONE_PER_MCS_BURSTS_DECODED)/sizeof(UINT32);i++)
    {
       dlMcsPtr[i] = perMcsPtr1[i] + perMcsPtr2[i];  
    }

    for(i=2;i<=6;i++){    
     memcpy(&pstPerDlZoneSts[i],&pstMcsStats->stPerZoneInfo.stPerDLZoneInfo[i].stPerDLZonePerMCSBursts,sizeof(ST_PERDLZONE_PER_MCS_BURSTS_DECODED)); 
    }  

	memcpy(pucOutData, pstPerDlZoneSts, sizeof(ST_PERDLZONE_PER_MCS_BURSTS_DECODED)*NUM_DL_ZONES);	
	*puiOutDataLen = (sizeof(ST_PERDLZONE_PER_MCS_BURSTS_DECODED) *NUM_DL_ZONES);

   
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pstPerDlZoneSts)
	BECEEM_MEMFREE(pstMcsStats)
	return iRetval;
	
	
}

int GetNumberOfControlMessagesDropped(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0,iRetval;
	S_MIBS_HOST_STATS_MIBS *pstHostStats = NULL;
	UNUSED_ARGUMENT(pMIBObj);

	BECEEMCALLOC(pstHostStats,S_MIBS_HOST_STATS_MIBS *,S_MIBS_HOST_STATS_MIBS,1)

	iRet = ReadHostStatsFromIOCTL(pWiMAX, pstHostStats);

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}	

	memcpy(pucOutData,&pstHostStats->stDroppedAppCntrlMsgs,sizeof(pstHostStats->stDroppedAppCntrlMsgs));	
	*puiOutDataLen = sizeof(pstHostStats->stDroppedAppCntrlMsgs);



	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pstHostStats)
	return iRetval;
}

int GetPduSndStatInformation(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0,iRetval=0, iPerZoneInfoOffset = 0,i = 0;
	ST_PER_ZONE_MCS_STATS *pstMcsStats = NULL;
	UINT32 uiStatsPtrBaseAddress = 0;

	UINT32 Total_PDUs_QPSK12_REP1 = 0;
	UINT32 Total_PDUs_QPSK12_REP2 = 0;
	UINT32 Total_PDUs_QPSK12_REP4 = 0;
	UINT32 Total_PDUs_QPSK12_REP6 = 0;
	UINT32 Total_PDUs_16QAM12 = 0;
	UINT32 Total_PDUs_16QAM34 = 0;
	UINT32 Total_PDUs_64QAM12 = 0;
	UINT32 Total_PDUs_64QAM23 = 0;
	UINT32 Total_PDUs_64QAM34 = 0;
	UINT32 Total_PDUs_64QAM56 = 0;
	UINT32 Total_PDUs_QPSK34 = 0;
	

	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(INT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

	BECEEMCALLOC(pstMcsStats,ST_PER_ZONE_MCS_STATS *,ST_PER_ZONE_MCS_STATS,1)

	iPerZoneInfoOffset = (UADDTYPE)&((stInternalStatsStruct *)NULL)->tBcmPropStats.BcmBasebandPerZoneMCS;

	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+iPerZoneInfoOffset, (PUCHAR)pstMcsStats, sizeof(ST_PER_ZONE_MCS_STATS));

	if(iRet)
	{
		iRetval=iRet;
		goto returnCode;
	}


	for(i=0; i<NUMBER_OF_UL_ZONE_TYPES; i++)
	{
		Total_PDUs_QPSK12_REP1 += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCCQpsk12Rate1 
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTCQpsk12Rate1
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCCQpsk12Rate1
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTCQpsk12Rate1 
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCCQpsk12Rate1
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTCQpsk12Rate1;

		Total_PDUs_QPSK12_REP2 += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCCQpsk12Rate2
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTCQpsk12Rate2
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCCQpsk12Rate2
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTCQpsk12Rate2
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCCQpsk12Rate2
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTCQpsk12Rate2; 

		Total_PDUs_QPSK12_REP4 += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCCQpsk12Rate4
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTCQpsk12Rate4
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCCQpsk12Rate4
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTCQpsk12Rate4
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCCQpsk12Rate4
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTCQpsk12Rate4;

		Total_PDUs_QPSK12_REP6 += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCCQpsk12Rate6
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTCQpsk12Rate6
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCCQpsk12Rate6
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTCQpsk12Rate6
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCCQpsk12Rate6
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTCQpsk12Rate6;

		Total_PDUs_16QAM12	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCC16Qam12
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC16Qam12
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCC16Qam12
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC16Qam12
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCC16Qam12
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC16Qam12;

		Total_PDUs_16QAM34	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCC16Qam34
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC16Qam34
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCC16Qam34
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC16Qam34
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCC16Qam34
							   	  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC16Qam34;

		Total_PDUs_64QAM12	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCC64Qam12
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC64Qam12
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCC64Qam12
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC64Qam12
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCC64Qam12
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC64Qam12;

		Total_PDUs_64QAM23	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCC64Qam23
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC64Qam23
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCC64Qam23
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC64Qam23
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCC64Qam23
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC64Qam23;

		Total_PDUs_64QAM34	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCC64Qam34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC64Qam34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCC64Qam34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC64Qam34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCC64Qam34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC64Qam34;

		Total_PDUs_64QAM56	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTC64Qam56
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTC64Qam56
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTC64Qam56;


		Total_PDUs_QPSK34	   += pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCCQpsk34 
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotA.BcmBasebandPduTxCTCQpsk34 
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCCQpsk34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tCSMPilotB.BcmBasebandPduTxCTCQpsk34 
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCCQpsk34
								  +pstMcsStats->stPerZoneInfo.stPerULZoneInfo[i].stPerULZonePerMCSPdus.tNonCSM.BcmBasebandPduTxCTCQpsk34;

	}

	
		switch(pMIBObj->u32MibObjId)
		{
			case BCM_STATS_PDUSENDQPSK12REP1:
				{
					memcpy(pucOutData, &Total_PDUs_QPSK12_REP1, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;
				
			case BCM_STATS_PDUSENDQPSK12REP2:
				{
					memcpy(pucOutData, &Total_PDUs_QPSK12_REP2, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;
				
			case BCM_STATS_PDUSENDQPSK12REP4:
				{
					memcpy(pucOutData, &Total_PDUs_QPSK12_REP4, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSENDQPSK12REP6:
				{
					memcpy(pucOutData, &Total_PDUs_QPSK12_REP6, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSENDQPSK34:
				{
					memcpy(pucOutData, &Total_PDUs_QPSK34, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;
				
			case BCM_STATS_PDUSEND16QAM12:
				{
					memcpy(pucOutData, &Total_PDUs_16QAM12, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSEND16QAM34:
				{
					memcpy(pucOutData, &Total_PDUs_16QAM34, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSEND64QAM12:
				{
					memcpy(pucOutData, &Total_PDUs_64QAM12, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSEND64QAM23:
				{
					memcpy(pucOutData, &Total_PDUs_64QAM23, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSEND64QAM34:
				{
					memcpy(pucOutData, &Total_PDUs_64QAM34, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;

			case BCM_STATS_PDUSEND64QAM56:
				{
					memcpy(pucOutData, &Total_PDUs_64QAM56, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
				}
				break;		
		}
		

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pstMcsStats)
	return iRetval;
	
}
int GetHARQRetriesExceeded(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,
							UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0, iOffsetBcmBasebandHarqRetryRcv4 = 0, iOffsetBcmBasebandHarqPerAttemptRcvSuccess4 = 0;
	UINT32 u32HarqAttemptExceeded = 0;
	UADDTYPE u64TempVariable = 0;
	UNUSED_ARGUMENT(pMIBObj);
	u64TempVariable = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqRetryRcv[3];

	iOffsetBcmBasebandHarqRetryRcv4 = (INT32)u64TempVariable;
	
	u64TempVariable = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqPerAttemptRcvSuccess[3];
	
	iOffsetBcmBasebandHarqPerAttemptRcvSuccess4 = (INT32)u64TempVariable;

	iRet = ReadDataFromstatstoOffset(pWiMAX,(UINT *)&iOffsetBcmBasebandHarqRetryRcv4,(UINT *)&iOffsetBcmBasebandHarqPerAttemptRcvSuccess4);
	    
	if(iRet)
		return iRet;

	u32HarqAttemptExceeded = iOffsetBcmBasebandHarqRetryRcv4 - iOffsetBcmBasebandHarqPerAttemptRcvSuccess4;

	memcpy(pucOutData, &u32HarqAttemptExceeded, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
	return iRet;

}

int GetDLHARQsInError(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,
							UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0, iOffseBcmBasebandNumOfDlHarqAckEnabledBursts = 0, iOffsetBcmBasebandHarqFinalRcvSuccess= 0;
	UINT32 u32DLHarqInError= 0;
	UNUSED_ARGUMENT(pMIBObj);
	iOffseBcmBasebandNumOfDlHarqAckEnabledBursts = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandNumOfDlHarqAckEnabledBursts;
	
	iOffsetBcmBasebandHarqFinalRcvSuccess = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqFinalRcvSuccess;
	
	iRet = ReadDataFromstatstoOffset(pWiMAX,(UINT *)&iOffseBcmBasebandNumOfDlHarqAckEnabledBursts,(UINT *)&iOffsetBcmBasebandHarqFinalRcvSuccess);
	    
	if(iRet)
		return iRet;

	u32DLHarqInError = iOffseBcmBasebandNumOfDlHarqAckEnabledBursts - iOffsetBcmBasebandHarqFinalRcvSuccess;

	memcpy(pucOutData, &u32DLHarqInError, sizeof(UINT32));
					*puiOutDataLen = sizeof(UINT32);
	return iRet;

}

int ReadDataFromstatstoOffset(PBECEEMWIMAX pWiMAX,PUINT pu32Offset1,PUINT pu32Offset2)
{
	INT32 iRet = 0;
	UINT32 uiStatsPtrBaseAddress = 0,u32ContentsOffset1 = 0,u32ContentsOffset2 = 0 ;
	

	iRet = BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS, &uiStatsPtrBaseAddress, sizeof(UINT32));

	if(iRet)
		return iRet;

	if(!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
		return ERROR_NO_STATISTIC_RESPONSE;
	}

		
	iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+(*pu32Offset1), (PUCHAR)&u32ContentsOffset1, sizeof(UINT32) );

	if(iRet)
		return iRet;

    iRet = BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress+(*pu32Offset2), (PUCHAR)&u32ContentsOffset2, sizeof(UINT32) );

	if(iRet)
		return iRet;

	*pu32Offset1 = u32ContentsOffset1;
	*pu32Offset2 = u32ContentsOffset2;
	return iRet;
}

int GetHARQRetryInError(PBECEEMWIMAX pWiMAX,PMIB_OBJECT_INFO pMIBObj,
							UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0, iOffsetBcmBasebandHarqRetryRcv = 0, iOffsetBcmBasebandHarqPerAttemptRcvSuccess= 0,iIndex = 0;
	UINT32 au32HARQRetryInError[4]= {0};
	UADDTYPE u64TempVariable = 0;
	INT32 iOffsetBcmBasebandHarqRetryRcvBkp = 0, iOffsetBcmBasebandHarqPerAttemptRcvSuccessBkp = 0;
	UNUSED_ARGUMENT(pMIBObj);

	u64TempVariable = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqRetryRcv[0];
	iOffsetBcmBasebandHarqRetryRcv = (INT32)u64TempVariable;
	u64TempVariable = (UADDTYPE)&((stInternalStatsStruct * )NULL)->tBcmPropStats.BcmBasebandHarqPerAttemptRcvSuccess[0];
	iOffsetBcmBasebandHarqPerAttemptRcvSuccess = (INT32)u64TempVariable;

	iOffsetBcmBasebandHarqRetryRcvBkp = iOffsetBcmBasebandHarqRetryRcv;
	iOffsetBcmBasebandHarqPerAttemptRcvSuccessBkp = iOffsetBcmBasebandHarqPerAttemptRcvSuccess;
	for(iIndex = 0; iIndex < 4; iIndex++)
		{
			iOffsetBcmBasebandHarqRetryRcv += iIndex*sizeof(UINT32);
			iOffsetBcmBasebandHarqPerAttemptRcvSuccess += iIndex*sizeof(UINT32);
			iRet = ReadDataFromstatstoOffset(pWiMAX,(UINT *)&iOffsetBcmBasebandHarqRetryRcv,(UINT *)&iOffsetBcmBasebandHarqPerAttemptRcvSuccess);
	    
			if(iRet)
				return iRet;

			au32HARQRetryInError[iIndex] = iOffsetBcmBasebandHarqRetryRcv - iOffsetBcmBasebandHarqPerAttemptRcvSuccess;
			iOffsetBcmBasebandHarqPerAttemptRcvSuccess = iOffsetBcmBasebandHarqPerAttemptRcvSuccessBkp ;
			iOffsetBcmBasebandHarqRetryRcv = iOffsetBcmBasebandHarqRetryRcvBkp;
		}
		memcpy(pucOutData, au32HARQRetryInError, sizeof(UINT32)*4);
		*puiOutDataLen = sizeof(UINT32)*4;
	return iRet;
}



int BeceemWiMAX_GetStatsBufferChunkFrmDDR(PBECEEMWIMAX pWiMAX)
{
    INT32 iRet = 0;

    UINT32 uiStatsPtrBaseAddress = 0;

    iRet =
	BeceemWiMAX_rdm(pWiMAX, STATS_PTR_ADDRESS,
			(UINT32 *) &uiStatsPtrBaseAddress, sizeof(INT32));
    if (iRet)
	{
	    goto returnCode;
	}

    if (!uiStatsPtrBaseAddress)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Invalid Statistics address!!!");
	    iRet = ERROR_NO_STATISTIC_RESPONSE;
	    goto returnCode;
	}
    
    iRet =
	BeceemWiMAX_rdmBulk(pWiMAX, uiStatsPtrBaseAddress,
			     pWiMAX->pStatsCacheData, sizeof(stInternalStatsStruct));
    if (iRet)
	{
	    goto returnCode;
	}
  returnCode:
        return iRet;

}


