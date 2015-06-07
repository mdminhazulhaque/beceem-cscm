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
 * Description	:	OMA-DM section get / set routines.
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h"
#include "uti_BeceemEEPROMCfg.h"
#include "globalobjects.h"
/* this is taken from uti_BeceemEEPROMCfg.h, any changes in uti_*.h should be 
** update in this too 
*/
UINT uiSecurityBase = CFG_SECU_CERTIFICATES_ENCRYPTION_STATUS_FLAG_ID;

int BeceemWiMAX_OMADMReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	UINT uiEEPROMChkSumInfo = 0,u32EEPROMVersion[2] = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	/* Checking EEPROM version and OMADM checksum validity*/
	BeceemWiMAX_rdmBulk(pWiMAX, EERPOM_VER_INT_ADDR, (UCHAR *)u32EEPROMVersion, 8);
	BeceemWiMAX_rdm(pWiMAX, EEPROM_CHECKSUM_INT_ADDR, &uiEEPROMChkSumInfo, 4);
	if(u32EEPROMVersion[0] < 4 || EEPROM_CHK_VAILIDITY(uiEEPROMChkSumInfo,9)==0)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		   "EEPROM Version %d not compatible with OMADM request\n", u32EEPROMVersion[0]);
		return OMA_DM_ERROR_INVALID_VERSION;
	}

	switch(pMsg->usSubType)
    {
		case SUBTYPE_OMADM_SET_REQ:
			return BeceemWiMAX_ProcessOMADMSetReq(pWiMAX, pMsg, uiLength);

		case SUBTYPE_OMADM_GET_REQ:
			return BeceemWiMAX_ProcessOMADMGetReq(pWiMAX, (POMADM_OBJECT_INFO)pMsg->szData);

		case SUBTYPE_OMADM_GET_PARAM_SIZE_REQ:
			return BeceemWiMAX_ProcessOMADMGetParamSize(pWiMAX, (POMADM_PARAMETER_INFO)pMsg->szData);
		
		case SUBTYPE_OMADM_VERSION_CHECK:
			return BeceemWiMAX_ProcessOMADMVersionCheck(pWiMAX, (POMADM_PARAMETER_VERSION_CHECK)pMsg->szData);
		case SUBTYPE_OMADM_DWNLD_TREE:
			return BeceemWiMAX_ProcessOMADMTreeUpdate(pWiMAX,(PST_OMA_DM_TREE_INFO)pMsg->szData);
	}
	return 0;
}

int BeceemWiMAX_OMADMCalibReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	int iRet = 0;
	UINT32 u32EEPROMVersion[2] = {0};	
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	/* Checking EEPROM version */
	BeceemWiMAX_rdmBulk(pWiMAX, EERPOM_VER_INT_ADDR, (UCHAR *)u32EEPROMVersion, 8);
	if(u32EEPROMVersion[0] < 4)
	{
	    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX,
		   "EEPROM Version %d not compatible with OMADM request\n", u32EEPROMVersion[0]);
		return OMA_DM_ERROR_INVALID_VERSION;
	}

		
	switch(pMsg->usSubType)
    {
		case SUBTYPE_SET_OMA_DM_INIT_DATA:
			iRet = CFG_UpdateOMAChecksum(pWiMAX->pEEPROMContextTable,CFG_OMA_OMA_DATA_ID);
			if (iRet <= 0)
				return ERROR_OMA_DM_INIT_FALIED;

		default:
	      DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "SubType is Invalid: [%d]\n", pMsg->usSubType);
			break;
	}
	return 0;
}

int BeceemWiMAX_SecReqMessage(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
    {
		case SUBTYPE_SECURITY_SET_REQ:
			return BeceemWiMAX_ProcessSecSetReq(pWiMAX, pMsg, uiLength);

		case SUBTYPE_SECURITY_GET_REQ:
			return BeceemWiMAX_ProcessSecGetReq(pWiMAX, (PSEC_OBJECT_INFO)pMsg->szData);

		case SUBTYPE_SECURITY_GET_PARAM_SIZE_REQ:
			{
				PWIMAXMESSAGE pWMMsg=NULL;
				UINT uiSecId = 0;
				int iSize = 0;
				PSEC_PARAMETER_INFO pSecParam = 0;
	
				pSecParam = (PSEC_PARAMETER_INFO)pMsg->szData;

				VALIDATE_SEC_PARAM_ID(pSecParam->u32SecObjId)

				uiSecId = uiSecurityBase + pSecParam->u32SecObjId;
				
				iSize = CFG_ParamSize(pWiMAX->pEEPROMContextTable,uiSecId);
				if (iSize < 0)
					return ERROR_SEC_OBJECT_NOTFOUND;
				
				pSecParam->s32SecObjSize = iSize;
				BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)

				WiMAXMessage_Init(
					pWMMsg,
					TYPE_SECURITY, 
					SUBTYPE_SECURITY_GET_PARAM_SIZE_RESP, 
					(UCHAR*)pSecParam,
					sizeof(SEC_PARAMETER_INFO)
					);

				BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
				BECEEM_MEMFREE(pWMMsg)
			}
			break;
		
		case SUBTYPE_GET_IMEI_DATA:
			return BeceemWiMAX_GetIMEIInfo(pWiMAX,(PUCHAR)pMsg->szData);			
	}
	return 0;
}

INT BeceemWiMAX_GetIMEIInfo(PBECEEMWIMAX pWiMAX, PUCHAR pData)
{
	PIMEI_INFO pIMEIInfo = (PIMEI_INFO)pData;
	INT32 iRet = 0;
	INT32 iParaSize = 0;

	iParaSize = CFG_ParamSize(pWiMAX->pEEPROMContextTable, CFG_HW_IMEI_ID);
	
	if(iParaSize <= 0)
	{
		pIMEIInfo->s32ErrorCode = ERROR_EEPROM_NOT_ACCESSIBLE;
		return ERROR_EEPROM_NOT_ACCESSIBLE;
	}

	iRet = CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8*)pIMEIInfo->au8IMEI, 
					iParaSize, CFG_HW_IMEI_ID);
	if (iRet < 0)
	{
		pIMEIInfo->s32ErrorCode = ERROR_EEPROM_NOT_ACCESSIBLE;
		return ERROR_EEPROM_NOT_ACCESSIBLE;
	}
	
	pIMEIInfo->s32ErrorCode = SUCCESS;
	return SUCCESS;
}

int BeceemWiMAX_ProcessOMADMGetReq(PBECEEMWIMAX pWiMAX, POMADM_OBJECT_INFO pOMADMObj)
{
	PUCHAR pucOutBuffer = NULL;
	int iRet = 0,iRetval=0;
	int iSize =0;
	OMA_DM_NODE_OFFSETINFO stOMADMOffsetInfo = {0};
	if (pWiMAX->m_bDeviceInitInProgress)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

	GetOMADM_Offset(pOMADMObj->stTreeLevel,pOMADMObj->u32NumOfLevel,
		&stOMADMOffsetInfo,pOMADMObj->u32OMADMObjSize,TRUE);

	if(stOMADMOffsetInfo.s32ErrorCode != SUCCESS)
	{
		pOMADMObj->s8ErrorCode = (SINT8)stOMADMOffsetInfo.s32ErrorCode;
		return stOMADMOffsetInfo.s32ErrorCode;
	}
	BECEEMCALLOC(pucOutBuffer,PUCHAR,CHAR,MAX_BUFFER_LENGTH)
	
	iRet = GetOMADMData(pWiMAX, &stOMADMOffsetInfo, pucOutBuffer, (UINT *)&iSize);
		
	if((UINT32)iSize != stOMADMOffsetInfo.u32NumBytes)
	{
		pOMADMObj->s8ErrorCode = OMA_DM_ERROR_READ_BUFFER_TOO_SMALL;
		iRetval=OMA_DM_ERROR_READ_BUFFER_TOO_SMALL;
		goto returnCode;
	}
	
	if(iRet < 0)
	{
		pOMADMObj->s8ErrorCode = ERROR_OMADM_OBJECT_NOTFOUND;
		iRetval=ERROR_OMADM_OBJECT_NOTFOUND;
		goto returnCode;
	}
	else
		memcpy(pOMADMObj->au8OMADMObjVal, pucOutBuffer, stOMADMOffsetInfo.u32NumBytes);
	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pucOutBuffer)
	return iRetval;
	
}

int GetOMADMData(PBECEEMWIMAX pWiMAX, POMA_DM_NODE_OFFSETINFO pOMADMObj, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	INT32 iRet = 0;

	iRet = CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8*)pucOutData, 
					(pOMADMObj->u32RootOffset << 16)|pOMADMObj->u32NumBytes, CFG_OMA_OMA_DATA_ID);
	if (iRet < 0)
		return ERROR_OMADM_OBJECT_NOTFOUND;

	*puiOutDataLen = iRet;

	iRet = SUCCESS;

	return iRet;
}

int BeceemWiMAX_ProcessOMADMSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = -1;
	POMADM_OBJECT_INFO pOMADMObj = 0;
	OMA_DM_NODE_OFFSETINFO stOMADMOffsetInfo = {0};
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX | !pMsg)
		return ERROR_INVALID_ARGUMENT;

	pOMADMObj = (POMADM_OBJECT_INFO)pMsg->szData;

	if (pWiMAX->m_bDeviceInitInProgress)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "BeceemWiMAX_ProcessOMADMSetReq ENTER");
	Mutex_Lock(&(pWiMAX->m_ShMemMutex));

	GetOMADM_Offset(pOMADMObj->stTreeLevel,pOMADMObj->u32NumOfLevel,
		&stOMADMOffsetInfo,pOMADMObj->u32OMADMObjSize,TRUE);

	if(stOMADMOffsetInfo.s32ErrorCode != SUCCESS)
	{
		pOMADMObj->s8ErrorCode =(SINT8)stOMADMOffsetInfo.s32ErrorCode;
        Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
		return stOMADMOffsetInfo.s32ErrorCode;
	}

	iRet = CFG_WriteOMADM(pWiMAX->pEEPROMContextTable, pOMADMObj->au8OMADMObjVal, 
					(stOMADMOffsetInfo.u32RootOffset << 16)|stOMADMOffsetInfo.u32NumBytes, CFG_OMA_OMA_DATA_ID);

	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "BeceemWiMAX_ProcessOMADMSetReq EXIT");
	if (iRet < 0)
	{
		pOMADMObj->s8ErrorCode = ERROR_OMADM_OBJECT_NOTFOUND;
		return ERROR_OMADM_OBJECT_NOTFOUND;
	}

	return SUCCESS;
}

int BeceemWiMAX_ProcessSecGetReq(PBECEEMWIMAX pWiMAX, PSEC_OBJECT_INFO pSecObjApp)
{
    PUCHAR pucOutBuffer = NULL;
    PWIMAXMESSAGE pWMMsg = NULL;
    int iRet = 0;
    INT32 i32Size = 0;
    UINT32 uiSecId=0;
    PSEC_OBJECT_INFO pSecObjInfo=NULL;


	VALIDATE_SEC_PARAM_ID(pSecObjApp->u32SecObjId)
	
	if (pWiMAX->m_bDeviceInitInProgress)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;


    uiSecId = uiSecurityBase + pSecObjApp->u32SecObjId;

    i32Size = CFG_ParamSize(pWiMAX->pEEPROMContextTable,uiSecId);

	DPRINT(DEBUG_MESSAGE, DEBUG_PATH_TX, "Size: %d for SecID: %d",i32Size,pSecObjApp->u32SecObjId);
    if(i32Size<=0 ||i32Size> MAX_CERTBUFF_LEN)
    {
        iRet=ERROR_SEC_OBJECT_LENGTH_INVALID;
        goto returnCode1;
    }

 
    ALLOCATE_MEMORY(pucOutBuffer,UCHAR,
    sizeof(WIMAXMESSAGE) - MAX_VARIABLE_LENGTH+ sizeof(SEC_OBJECT_INFO)+i32Size-1,0);

    if(!pucOutBuffer)
    {
        iRet=ERROR_MALLOC_FAILED;
        goto returnCode1;
    }

    pWMMsg = (PWIMAXMESSAGE)pucOutBuffer;

    pSecObjInfo=(PSEC_OBJECT_INFO)pWMMsg->stMessage.szData;
    pSecObjInfo->u32SecObjId=pSecObjApp->u32SecObjId;
    iRet = GetSecData(pWiMAX, pSecObjInfo, pSecObjInfo->au8SecObjVal,(UINT *) &i32Size);
    
    if(iRet < 0)
    {
        i32Size = 0;
        pSecObjInfo->s8ErrorCode =(SINT8) iRet;
    }
	else
		/* copy security data to app req buffer also */
		memcpy(pSecObjApp->au8SecObjVal,pSecObjInfo->au8SecObjVal,i32Size);

	pSecObjApp->u32SecObjSize=pSecObjInfo->u32SecObjSize=i32Size;	
    

    pWMMsg->stMessage.usType = TYPE_SECURITY;
    pWMMsg->stMessage.usSubType = SUBTYPE_SECURITY_GET_RESP;
	pWMMsg->uiHeaderLen = SIZE_WIMAX_MESSAGE - MAX_VARIABLE_LENGTH;
	WiMAXMessage_SetData(pWMMsg, (PUCHAR)pSecObjInfo, sizeof(SEC_OBJECT_INFO)+i32Size-1);

    BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

    iRet=SUCCESS;

    FREE_MEMORY(pucOutBuffer);
returnCode1:
    return iRet;
	
}

int GetSecData(PBECEEMWIMAX pWiMAX, PSEC_OBJECT_INFO pSecObj, UCHAR *pucOutData,UINT *puiOutDataLen)
{
	UINT uiSecId = 0;
	int iSize = 0,iRet = 0;
	
	uiSecId = uiSecurityBase + pSecObj->u32SecObjId;

	iSize = CFG_ParamSize(pWiMAX->pEEPROMContextTable,uiSecId);

	if (iSize < 0)
		{
			return ERROR_SEC_OBJECT_NOTFOUND;
		}
	else if (iSize == 0)
		{
		
			return ERROR_SEC_OBJECT_ZERO_LENGTH;
		}

	iRet = CFG_ReadParam(pWiMAX->pEEPROMContextTable, (CFG_UINT8*)pucOutData, iSize, uiSecId);
	if (iRet < 0)
		{
		
			return ERROR_SEC_OBJECT_NOTFOUND;
		}
	*puiOutDataLen = iRet;

	iRet = SUCCESS;

	
	return iRet;
}

int BeceemWiMAX_ProcessSecSetReq(PVOID pArg, PWIMAX_MESSAGE pMsg, UINT uiLength)
{
	int iRet = -1,iRetval=0, iSize = 0;
	PSEC_OBJECT_INFO pSecObj = 0;
	UINT uiSecId = 0;
	PUCHAR pucData = NULL;
	PWIMAXMESSAGE pWMMsg = NULL;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX) pArg;
	UNUSED_ARGUMENT(uiLength);
	if(!pWiMAX | !pMsg)
		return ERROR_INVALID_ARGUMENT;

	pSecObj = (PSEC_OBJECT_INFO)pMsg->szData;

	VALIDATE_SEC_PARAM_ID(pSecObj->u32SecObjId)

	if (pWiMAX->m_bDeviceInitInProgress)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;
	
	Mutex_Lock(&(pWiMAX->m_ShMemMutex));
	BECEEMCALLOC(pucData,PUCHAR,CHAR,MAX_BUFFER_SIZE)
	pWMMsg = (PWIMAXMESSAGE)calloc(sizeof(WIMAXMESSAGE),1);
	if(!pWMMsg)
	{
		iRetval=ERROR_MALLOC_FAILED;
		goto returnCode;
	}
	/*Write Shared memory to CellRAM*/
	memcpy(pucData, (UCHAR *)pSecObj->au8SecObjVal, pSecObj->u32SecObjSize);
	
	uiSecId = uiSecurityBase + pSecObj->u32SecObjId;
	iRet = CFG_WriteSecurity(pWiMAX->pEEPROMContextTable, pucData, pSecObj->u32SecObjSize, uiSecId);		
	
	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));
	if (iRet < 0)
		pSecObj->s8ErrorCode = ERROR_SEC_OBJECT_NOTFOUND;

	WiMAXMessage_Init(
			pWMMsg,
			TYPE_SECURITY, 
			SUBTYPE_SECURITY_SET_RESP, 
			(UCHAR*)pSecObj,
			sizeof(SEC_OBJECT_INFO)+iSize-1
			);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);

	iRetval=SUCCESS;
returnCode:
	BECEEM_MEMFREE(pucData)
	BECEEM_MEMFREE(pWMMsg)
	return iRetval;
	
}


void  GetOMADM_Offset(OMA_DM_TREE_LEVEL* pTree,UINT32 u32Levels,
					POMA_DM_NODE_OFFSETINFO pOMADMOffsetInfo,UINT32 u32BufferSize,
					UCHAR bValidateBufferSize)
{
	UINT32 u32Index = 0;
	UINT32 u32Key=0;
	UCHAR u8StructIndex=0;
	UCHAR u8FieldIndex=0;
	OMA_DM_NODE_INFO* pNodeInfo=NULL;

	for(u32Index=0; u32Index < u32Levels; u32Index++)
	{
 		u32Key=pTree[u32Index].u32FieldId;
		u8StructIndex=(UCHAR)((u32Key>>8)-1);
		u8FieldIndex=((UCHAR)(u32Key&0xFF)-1);
		
		pNodeInfo=&(g_OmaDmNodeInfo[u8StructIndex][u8FieldIndex]);
		if ((pNodeInfo->u8IsArray==1) && (pTree[u32Index].u32FieldInstance >  pNodeInfo-> u32numElements))
		{
			/*out of bound array access*/
			pOMADMOffsetInfo->s32ErrorCode = OMA_DM_ERROR_OUT_OF_BOUND_ARRAY_ACCESS;
			return;
		}

		pOMADMOffsetInfo->u32RootOffset  = pOMADMOffsetInfo->u32RootOffset+ pNodeInfo->u32ByteOffset+pTree[u32Index].u32FieldInstance*pNodeInfo->u32ElementSize;
		
	}
	if (pNodeInfo->u8IsArray==1)
	{
		pOMADMOffsetInfo->u32NumBytes= pNodeInfo->u32ElementSize*pNodeInfo->u32numElements;
	}
	else
	{
		pOMADMOffsetInfo->u32NumBytes  = pNodeInfo->u32ElementSize;
	}
	if(!bValidateBufferSize)
		return;

	if (pOMADMOffsetInfo->u32NumBytes  > u32BufferSize)
	{
		/*not enough space to return data*/
		pOMADMOffsetInfo->u32NumBytes = pNodeInfo->u32ElementSize;/*this will indicate how much space is required*/
		pOMADMOffsetInfo->s32ErrorCode = OMA_DM_ERROR_READ_BUFFER_TOO_SMALL;
		return;	
	}
	else if(pOMADMOffsetInfo->u32NumBytes  < u32BufferSize )
	{
		/*input data buffer is too large*/
		pOMADMOffsetInfo->u32NumBytes = pNodeInfo->u32ElementSize;/*this will indicate max storage available for this field*/
		pOMADMOffsetInfo->s32ErrorCode = OMA_DM_ERROR_WRITE_BUFFER_TOO_LARGE;
	}	
}

int BeceemWiMAX_ProcessOMADMGetParamSize(PBECEEMWIMAX pWiMAX, POMADM_PARAMETER_INFO pOMADMParamInfo)
{
	int iRet = 0;
	OMA_DM_NODE_OFFSETINFO stOMADMOffsetInfo = {0};
	if (pWiMAX->m_bDeviceInitInProgress)
		return ERROR_SHARED_MEM_NOT_INITIALIZED;

	if (iRet)
		return iRet;

	GetOMADM_Offset(pOMADMParamInfo->stTreeLevel,pOMADMParamInfo->u32NumOfLevel,
		&stOMADMOffsetInfo,pOMADMParamInfo->u32OMADMObjSize,FALSE);
	

	if(stOMADMOffsetInfo.s32ErrorCode != SUCCESS)
	{
		pOMADMParamInfo->s8ErrorCode =(SINT8)stOMADMOffsetInfo.s32ErrorCode;
		return stOMADMOffsetInfo.s32ErrorCode;
	}
	pOMADMParamInfo->u32OMADMObjSize = stOMADMOffsetInfo.u32NumBytes;
	return SUCCESS;
}

INT BeceemWiMAX_ProcessOMADMVersionCheck(PBECEEMWIMAX pWiMAX, 
							POMADM_PARAMETER_VERSION_CHECK pOMADMVersionCheck)
{
	UNUSED_ARGUMENT(pWiMAX);
	if((pOMADMVersionCheck->u32MajorVersion == OMA_DM_MAJOR_VERSION_INTERNAL) && 
		(pOMADMVersionCheck->u32MinorVersion == OMA_DM_MINOR_VERSION_INTERNAL))
		{
			return SUCCESS;
		}
	pOMADMVersionCheck->u32MajorVersion = OMA_DM_MAJOR_VERSION_INTERNAL;
	pOMADMVersionCheck->u32MinorVersion = OMA_DM_MINOR_VERSION_INTERNAL;
	
	return OMA_DM_ERROR_INVALID_VERSION;
}
INT BeceemWiMAX_ProcessOMADMTreeUpdate(PBECEEMWIMAX pWiMAX, 
							PST_OMA_DM_TREE_INFO pOMADMTreeInfo)
{
#ifdef WIN32
	HANDLE	hFileHandle = NULL;
	UINT32 u32BytesRead = 0;
	PUCHAR puchOMADMTree = NULL;
	BOOL bReadStatus = FALSE;
	hFileHandle = CreateFile(
						pOMADMTreeInfo->szOMADMTreeFile,
						FILE_READ_DATA,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN,
						NULL
					);
	
	if(!hFileHandle)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "Couldnot open file !!!");
		return ERROR_INVALID_ADDRS_INPUT;
	}
	
	BECEEMCALLOC(puchOMADMTree,PUCHAR,ST_ROOT,1)

	bReadStatus = ReadFile(hFileHandle,(LPVOID)puchOMADMTree,sizeof(ST_ROOT),(LPDWORD)&u32BytesRead,NULL);	

	if(!bReadStatus)
		{
			CloseHandle(hFileHandle );
			BECEEM_MEMFREE(puchOMADMTree);
			return ERROR_READ_FAILED;
		}

	
	if(u32BytesRead == sizeof(ST_ROOT))
		{
			/*Do OMADM Download here*/
			
			BeceemWiMAX_DownloadOMADMTree(pWiMAX,puchOMADMTree,u32BytesRead);
			CloseHandle(hFileHandle );
			BECEEM_MEMFREE(puchOMADMTree);
		}
	else
		{
			CloseHandle(hFileHandle );
			BECEEM_MEMFREE(puchOMADMTree);
			return ERROR_READ_FAILED;
		}
#else
	struct stat stFileStat={0};
	UINT32 u32FileDescriptor = 0,u32FileSize = 0;
	INT32 s32RetVal = 0;
	PUCHAR puchOMADMTree = NULL;
	 u32FileDescriptor  = open((char *)pOMADMTreeInfo->szOMADMTreeFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if(u32FileDescriptor == -1)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_TX, "File could not be opened, plz check the path or name");
		
		return ERROR_INVALID_ADDRS_INPUT;	
	}
	fstat(u32FileDescriptor, &stFileStat);
	u32FileSize = stFileStat.st_size;

	
	BECEEMCALLOC(puchOMADMTree,PUCHAR,UCHAR,u32FileSize)
		
	s32RetVal = read(u32FileDescriptor, 
		puchOMADMTree,u32FileSize);	
	if((s32RetVal == -1) || ((UINT32)s32RetVal != u32FileSize))
		{
			BECEEM_MEMFREE(puchOMADMTree);
			close(u32FileDescriptor);
			return ERROR_INVALID_ADDRS_INPUT;
		}
	else
		{
			BeceemWiMAX_DownloadOMADMTree(pWiMAX,puchOMADMTree,u32FileSize);
			BECEEM_MEMFREE(puchOMADMTree);
			close(u32FileDescriptor);
		}

	
#endif
	
	return SUCCESS;
}

INT BeceemWiMAX_DownloadOMADMTree(PBECEEMWIMAX pWiMAX,PUCHAR pOMADMTree,UINT32 u32Size)
{
	int iRet = 0;
	Mutex_Lock(&(pWiMAX->m_ShMemMutex));	
	
	iRet = CFG_WriteOMADM(pWiMAX->pEEPROMContextTable, pOMADMTree, 
					u32Size, CFG_OMA_OMA_DATA_ID);

	Mutex_Unlock(&(pWiMAX->m_ShMemMutex));

	if (iRet < 0)
	{
		return ERROR_EEPROM_WRITE_FAILED;
	}
	return SUCCESS;
}
