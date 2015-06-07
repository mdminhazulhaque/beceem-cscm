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
 * Description	:	Flash 2.x interface implementation.
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h"
extern DEBUGPRINT *GTraceDebug;
INT BeceemWiMAX_FlashAPIHandler(PBECEEMWIMAX pWiMAX,PVOID pPayloadData,UINT uiLength)
{
	PWIMAX_MESSAGE pWiMAXMessage = (PWIMAX_MESSAGE)pPayloadData;
	switch(pWiMAXMessage->usSubType)
	{
	case SUBTYPE_GET_FLASH_VERSION_INFO_REQ:
		return BeceemWiMAX_GetFlashVersionInfo(pWiMAX,(PDYNAMIC_FLASH_CS_INFO)pWiMAXMessage->szData,uiLength);
	case SUBTYPE_READ_FLASH_SECTION_REQ:
		return BeceemWiMAX_ReadFlashSectionReq(pWiMAX,(PDYNAMIC_FLASH_READWRITE_ST)pWiMAXMessage->szData,uiLength);
	case SUBTYPE_WRITE_FLASH_SECTION_REQ:
		return BeceemWiMAX_WriteFlashSectionReq(pWiMAX,(PDYNAMIC_FLASH_READWRITE_ST)pWiMAXMessage->szData,uiLength);
	case SUBTYPE_COPY_FLASH_SECTION_REQ:
		return BeceemWiMAX_CopySection(pWiMAX, (PST_FLASH2X_COPY_SECTION)pWiMAXMessage->szData ,uiLength);
	case SUBTYPE_GET_FLASH_SECTION_BITMAP_REQ: 
		return BeceemWiMAX_GetflashSectionBitMap(pWiMAX,(PST_PFLASH2X_BITMAP)pWiMAXMessage->szData ,uiLength);
	case SUBTYPE_SET_ACTIVE_SECTION_REQ:
		return BeceemWiMAX_SetActiveSection(pWiMAX,(PE_DYNAMIC_SECTION_VAL)pWiMAXMessage->szData,uiLength);
	
	case SUBTYPE_READ_FLASH_REQ:
		return BeceemWiMAX_FlashRawRead(pWiMAX,(PST_FLASH_RAW_ACCESS)pWiMAXMessage->szData);
	default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "BeceemWiMAX_FlashAPIHandler invalid subtype : %s",
		     __FILE__);
	}
	return SUCCESS;
}

INT BeceemWiMAX_GetFlashVersionInfo(PBECEEMWIMAX pWiMAX,PDYNAMIC_FLASH_CS_INFO pFlashControlSection,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;
	PFLASH_2X_CS_INFO_PVT pFlash_2X_CS_Info = NULL;
	if( uiLength < (sizeof(USHORT)*2+sizeof(DYNAMIC_FLASH_CS_INFO)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	
	BECEEMCALLOC(pFlash_2X_CS_Info, PFLASH_2X_CS_INFO_PVT, FLASH_2X_CS_INFO_PVT,1)
	
	
//Read flash version info 
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_GET_FLASH_CS_INFO,
				NULL,
				0,
				(PVOID)pFlash_2X_CS_Info,
				sizeof(FLASH_2X_CS_INFO_PVT),
				(LPDWORD)&iBufferReturned
			);	

		if(iRet)
    		{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "IOCTL_BCM_GET_FLASH_CS_INFO failed with error : %d ",
		   iRet);
			}
	BeceemWiMAX_CheckAndSwapFlashInfo(pWiMAX, (PUCHAR)pFlashControlSection,(PUCHAR)pFlash_2X_CS_Info);
	
	BECEEM_MEMFREE(pFlash_2X_CS_Info)
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

VOID BeceemWiMAX_CheckAndSwapFlashInfo(PBECEEMWIMAX pWiMAX, PUCHAR pucFlashControlSection, PUCHAR pucFlash_2X_CS_Info)
{
    INT32 iIndex = 0;
      
    if(!pWiMAX->m_bBigEndianMachine)
    {
	  memcpy(pucFlashControlSection,pucFlash_2X_CS_Info,sizeof(DYNAMIC_FLASH_CS_INFO));
    }
	else
	{
		memcpy(pucFlashControlSection,pucFlash_2X_CS_Info,sizeof(UINT));
		pucFlashControlSection += sizeof(UINT);
		pucFlash_2X_CS_Info += sizeof(UINT);
	              
		while(iIndex++ < 3)
		{
		  memcpy(pucFlashControlSection, pucFlash_2X_CS_Info+2, sizeof(USHORT));
		  memcpy(pucFlashControlSection+2, pucFlash_2X_CS_Info, sizeof(USHORT));
		  pucFlashControlSection += sizeof(UINT);
		  pucFlash_2X_CS_Info += sizeof(UINT);
		}
	}
                  
}

INT BeceemWiMAX_ReadFlashSectionReq(PBECEEMWIMAX pWiMAX,PDYNAMIC_FLASH_READWRITE_ST pstFlashReadsect,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;
	
	if( uiLength < (sizeof(USHORT)*2+sizeof(DYNAMIC_FLASH_READWRITE_ST)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!pstFlashReadsect->pDataBuff)
		return ERROR_INVALID_ARGUMENT;
	
	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));

	//Read flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_FLASH2X_SECTION_READ,
				(PVOID)pstFlashReadsect,
				sizeof(DYNAMIC_FLASH_READWRITE_ST),
				(PVOID)pstFlashReadsect->pDataBuff,
				pstFlashReadsect->numOfBytes,
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_DFLASH_SECTION_READ failed with error : %d ", iRet);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

INT BeceemWiMAX_WriteFlashSectionReq(PBECEEMWIMAX pWiMAX,PDYNAMIC_FLASH_READWRITE_ST pstFlashReadsect,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;
	
	if( uiLength < (sizeof(USHORT)*2+sizeof(DYNAMIC_FLASH_READWRITE_ST)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!pstFlashReadsect->pDataBuff)
		return ERROR_INVALID_ARGUMENT;
	
	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	//write flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_FLASH2X_SECTION_WRITE,
				(PVOID)pstFlashReadsect,
				sizeof(DYNAMIC_FLASH_READWRITE_ST),
				NULL,
				0,
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_DFLASH_SECTIION_WRITE failed with error : %d and errorcode : %d", iRet,
		   iBufferReturned);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}
INT BeceemWiMAX_CopySection(PBECEEMWIMAX pWiMAX,PST_FLASH2X_COPY_SECTION pstCopySection,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;

	if( uiLength < (sizeof(USHORT)*2+sizeof(ST_FLASH2X_COPY_SECTION)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	//write flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_COPY_SECTION,
				(PVOID)pstCopySection,
				sizeof(ST_FLASH2X_COPY_SECTION),
				NULL,
				0,
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_COPY_SECTION failed with error : %d and errorcode : %d", iRet,
		   iBufferReturned);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

INT BeceemWiMAX_GetflashSectionBitMap(PBECEEMWIMAX pWiMAX,PST_PFLASH2X_BITMAP pstFlashSectionBitMap,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;

	if( uiLength < (sizeof(USHORT)*2+sizeof(ST_FLASH2X_BITMAP)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	//write flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_GET_FLASH2X_SECTION_BITMAP,
				(PVOID)pstFlashSectionBitMap,
				sizeof(ST_FLASH2X_BITMAP),
				(PVOID)pstFlashSectionBitMap,
				sizeof(ST_FLASH2X_BITMAP),
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_COPY_SECTION failed with error : %d and errorcode : %d", iRet,
		   iBufferReturned);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

INT BeceemWiMAX_SetActiveSection(PBECEEMWIMAX pWiMAX,PE_DYNAMIC_SECTION_VAL pDynamicSectionVal,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;

	if( uiLength < (sizeof(USHORT)*2+sizeof(E_DYNAMIC_SECTION_VAL)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;

	switch(*pDynamicSectionVal)
		{
		case DSD0:
		case DSD1:
		case DSD2:
			break;
		default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		     "BeceemWiMAX_SetActiveSection called with Section val #%d", *pDynamicSectionVal);
			return ERROR_INVALID_ARGUMENT;
		}
	
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	//write flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_SET_ACTIVE_SECTION,
				(PVOID)pDynamicSectionVal,
				sizeof(E_DYNAMIC_SECTION_VAL),
				NULL,
				0,
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_COPY_SECTION failed with error : %d and errorcode : %d", iRet,
		   iBufferReturned);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

INT BeceemWiMAX_SelectDSD(PBECEEMWIMAX pWiMAX,PE_DYNAMIC_SECTION_VAL pDynamicSectionVal,UINT uiLength)
{
	INT iRet = 0, iBufferReturned = 0;

	if( uiLength < (sizeof(USHORT)*2+sizeof(E_DYNAMIC_SECTION_VAL)))
		return ERROR_INVALID_BUFFER_LEN;

	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
	switch(*pDynamicSectionVal)
		{
		case DSD0:
		case DSD1:
		case DSD2:
			break;
		default:
	      DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		     "BeceemWiMAX_SetActiveSection called with Section val #%d", *pDynamicSectionVal);
			return ERROR_INVALID_ARGUMENT;
		}
	Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));
	//write flash section
	iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_SELECT_DSD,
				(PVOID)pDynamicSectionVal,
				sizeof(E_DYNAMIC_SECTION_VAL),
				NULL,
				0,
				(LPDWORD)&iBufferReturned
			);	

	if(iRet)
	{
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER,
		   "IOCTL_BCM_SELECT_DSD failed with error : %d and errorcode : %d", iRet,
		   iBufferReturned);
	}
	
	Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));
	return iRet;
}

INT BeceemWiMAX_FlashRawRead(PBECEEMWIMAX pWiMAX,PST_FLASH_RAW_ACCESS pstFlashRawAccess)
{
	
	NVM_READWRITE stNVMReadWrite = {0};
	INT iRet = 0,iBufferReturned;
	
	if(!pWiMAX)
		return -1;
	
	if(!pstFlashRawAccess->pBuffer || !pstFlashRawAccess->uiSize)
		return ERROR_INVALID_ARGUMENT;
	
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "NVM rdm with offset %d and size %d",
	   pstFlashRawAccess->uiOffset, pstFlashRawAccess->uiSize);
	
	if(!DeviceIO_IsValidHandle(&(pWiMAX->m_Device)))
		return ERROR_HANDLE_INVALID;
		Mutex_Lock(&(pWiMAX->m_EEPROM_AcessMutex));

		

		stNVMReadWrite.pBuffer = pstFlashRawAccess->pBuffer;
		stNVMReadWrite.uiNumBytes = pstFlashRawAccess->uiSize;
		stNVMReadWrite.uiOffset = pstFlashRawAccess->uiOffset;
	
		iRet = DeviceIO_Send(
				&(pWiMAX->m_Device),
				IOCTL_BCM_NVM_RAW_READ,
				(PVOID)&stNVMReadWrite,
				sizeof(NVM_READWRITE),
				#ifdef WIN32
				NULL
				#else
				(PVOID)&stNVMReadWrite
				#endif
				,
				#ifdef WIN32
				0
				#else
				sizeof(NVM_READWRITE)
				#endif
				,
				(LPDWORD)&iBufferReturned
			);
    DPRINT(DEBUG_MESSAGE, DEBUG_PATH_OTHER, "Bytes returned after NVM read %d", iBufferReturned);

	   	if(iRet)
    		{
			  
	    DPRINT(DEBUG_ERROR, DEBUG_PATH_OTHER, "NVM rdm failed with error : %d ", iRet);
    		}
		
		Mutex_Unlock(&(pWiMAX->m_EEPROM_AcessMutex));

    return iRet;

}
