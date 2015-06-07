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
 * Description	:
 * Author		:
 * Reviewer		:
 *
 */


#include "BeceemWiMAX.h" 
#include "VendorSpecificTypeDefs.h"


int BeceemWiMAX_AuthReadParam(PVOID pArg, PWIMAX_MESSAGE pMsg);
int BeceemWiMAX_AuthWriteParam(PVOID pArg, PWIMAX_MESSAGE pMsg);
int BeceemWiMAX_SendAuthIndication(PVOID pArg, UCHAR ucMethodType, UCHAR ucEAPMethodType);
int BeceemWiMAX_Read_EAP_AKA(PVOID pArg, USHORT *puiOutDataLen, UCHAR *pucOutData);
int BeceemWiMAX_Read_IMEI(PVOID pArg, USHORT *puiOutDataLen, UCHAR *pucOutData);
int BeceemWiMAX_Write_EAP_AKA(PVOID pArg, UCHAR *pInData);

int BeceemWiMAX_VendorSpecificMessage(PVOID pArg, PVOID pvMsg, UINT uiLength)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	PWIMAX_MESSAGE pMsg = (PWIMAX_MESSAGE)pvMsg;
	UNUSED_ARGUMENT(uiLength);

	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	switch(pMsg->usSubType)
	{
		case SUBTYPE_AUTH_STATUS_REQ:
			break;

		case SUBTYPE_AUTH_READ_PARAM_REQ:
			if(!pWiMAX->m_bInitEEPROM)
				return ERROR_EEPROM_NOTINITIALIZED;
			return BeceemWiMAX_AuthReadParam(pWiMAX, pMsg);
		
		case SUBTYPE_AUTH_WRITE_PARAM_REQ:
			if(!pWiMAX->m_bInitEEPROM)
				return ERROR_EEPROM_NOTINITIALIZED;
			return BeceemWiMAX_AuthWriteParam(pWiMAX, pMsg);

	}
	return SUCCESS;
}


int BeceemWiMAX_AuthReadParam(PVOID pArg, PWIMAX_MESSAGE pMsg)
{
	UCHAR ucReadInfoType = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	ucReadInfoType = *(UCHAR *)(pMsg->szData);
	return BeceemWiMAX_SendAuthIndication(pWiMAX, READ_AUTH_PARAM_REQUEST, ucReadInfoType);
}

int BeceemWiMAX_SendAuthIndication(PVOID pArg, UCHAR ucMethodType, UCHAR ucEAPMethodType)
{
	PWIMAXMESSAGE pWMMsg = NULL;
	AUTH_PARAMETER_INDICATION stAuthParamIndication;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;
	BECEEMCALLOC(pWMMsg,PWIMAXMESSAGE,WIMAXMESSAGE,1)
	WiMAXMessage_Init(pWMMsg,TYPE_AUTHENTICATION, SUBTYPE_AUTH_PARAM_INDICATION,0,0); 

	stAuthParamIndication.ucEAPMethodType = ucEAPMethodType;
	stAuthParamIndication.ucRequestType = ucMethodType;
	stAuthParamIndication.usAuthPayloadLength = 0;

	switch(ucEAPMethodType)
	{
	case AUTH_METHOD_EAP_AKA:
			if(BeceemWiMAX_Read_EAP_AKA(
								pArg,
								&stAuthParamIndication.usAuthPayloadLength,
								stAuthParamIndication.aucAuthPayload
									   ))
				stAuthParamIndication.ucAckNack = 1;						/* 0 - ACK, 1 - NACK */
			else
				stAuthParamIndication.ucAckNack = 0;						/* 0 - ACK, 1 - NACK */
			break;	
	case AUTH_METHOD_EAP_SIM:
			if(BeceemWiMAX_Read_IMEI(
							pArg,
							&stAuthParamIndication.usAuthPayloadLength,
							stAuthParamIndication.aucAuthPayload
						))
				stAuthParamIndication.ucAckNack = 1;						/* 0 - ACK, 1 - NACK */
			else
				stAuthParamIndication.ucAckNack = 0;						/* 0 - ACK, 1 - NACK */
			break;	
	}

	WiMAXMessage_AppendData(
						pWMMsg,
						(UCHAR *) &stAuthParamIndication, 
						sizeof(AUTH_PARAMETER_INDICATION) 
						+ stAuthParamIndication.usAuthPayloadLength 
						- sizeof(stAuthParamIndication.aucAuthPayload) 
					);

	BeceemWiMAX_CallAppCallback(pWiMAX, pWMMsg);
	BECEEM_MEMFREE(pWMMsg)
	return SUCCESS;
}

int BeceemWiMAX_Read_EAP_AKA(PVOID pArg, USHORT *puiOutDataLen, UCHAR *pucOutData)
{
	//AUTH_WRITE_VECTOR AuthWriteParams;
	PAUTH_READ_VECTOR pAuthVector = (PAUTH_READ_VECTOR) pucOutData;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	/* Copy WNID Length is 16 bytes. Reference HIM document. */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucWNID, 16, CFG_SEC_SECURITY_EAP_NETWORK_ID_ID); dialgoues 
	CFG_ReadParam((unsigned char*)pAuthVector->ucWNID, VSRO_EAP_NETWORK_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);*/
	/* DebugDump("SECURITY_EAP_NETWORK_ID", (UCHAR *)pAuthVector->ucWNID, 16); */
	*puiOutDataLen += 16;

	/* Copy K Length is 16 Bytes. Reference HIM document. */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucK, 16, CFG_SEC_SECURITY_K_ID);//CFG_SEC_K_ID */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucK, VSRO_SECURITY_K_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SECURITY_K_ID", (UCHAR *)pAuthVector->ucK, 16); */
	*puiOutDataLen += 16;

	/* Copy OPc Length is 16 Bytes. Reference HIM document */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucOPc, 16, CFG_SEC_SECURITY_OPC_ID);//CFG_SEC_OPC_ID */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucOPc, VSRO_SECURITY_OPC_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SECURITY_OPC_ID", (UCHAR *)pAuthVector->ucOPc, 16); */
	*puiOutDataLen += 16;

	/* Copy NonK Length is 16 Bytes. Reference HIM document */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucNonK, 16, CFG_SEC_SECURITY_NON_K_ID); */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucNonK, VSRO_SECURITY_NONK_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SECURITY_NON_K_ID", (UCHAR *)pAuthVector->ucNonK, 16); */
	*puiOutDataLen += 16;

	/* Copy NonOPc */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucNonOPc, 16, CFG_SEC_SECURITY_NON_OPC_ID); */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucNonOPc, VSRO_SECURITY_NON_OPC_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
	*puiOutDataLen += 16;

	/* Copy constants. C[1 -5] -80 Bytes. r [1-5] - 20 Bytes. */

	/*  80 Bytes. */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucC1_5_R1_5, 80, CFG_SEC_SECURITY_MILENGE_CCONSTNTS_ID); */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucC1_5_R1_5, VSRO_SECURITY_MILENGE_CONSTNTS_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SECURITY_MILENGE_CCONSTNTS_ID", (UCHAR *)pAuthVector->ucC1_5_R1_5, 80); */
	*puiOutDataLen += 80;

	/* 20 Bytes. */
	/* CFG_ReadParam((unsigned char*)pAuthVector->ucC1_5_R1_5+80, 20, CFG_SEC_SECURITY_MILENGE_RCONSTNTS_ID); */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucC1_5_R1_5+80, VSRO_SECURITY_MILENGE_RCONSTNTS_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SECURITY_MILENGE_RCONSTNTS_ID", (UCHAR *)pAuthVector->ucC1_5_R1_5+80, 20); */
	*puiOutDataLen += 20;


	/* Copy SQN. Length is 192 Bytes Reference HIM documet. */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucSQNMS, 192, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("SQNMS", (UCHAR *)pAuthVector->ucSQNMS, 192); */
	*puiOutDataLen += 192;

	/* Copy Pseudonym ID. Length is 23 Bytes Reference HIM document. */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char*)pAuthVector->ucPseudonymID, 23, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("Pseudonym ID", (UCHAR *)pAuthVector->ucPseudonymID, 23); */
	*puiOutDataLen += 23;

#if 0
	/*  Read ATR to calc CRC */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, AuthWriteParams.aucATRLock, sizeof(AuthWriteParams.aucATRLock), CFG_VSA_VENDORSPECIFICDATA_ID);
	memcpy(AuthWriteParams.aucSEC_SNYMID_ID, pAuthVector->ucPseudonymID, sizeof(AuthWriteParams.aucSEC_SNYMID_ID));
	memcpy(AuthWriteParams.aucSEC_SQNMS_ID, pAuthVector->ucSQNMS, sizeof(AuthWriteParams.aucSEC_SQNMS_ID));
	ulCRC = CFG_CalculateChecksum((UCHAR *)&AuthWriteParams, sizeof(AUTH_WRITE_VECTOR), 0xFFFF);
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, (unsigned char *)&read_crc, 4, CFG_VSA_VENDORSPECIFICDATA_ID);

	if(read_crc == ulCRC)
	{
		DPRINT(DEBUG_MESSAGE, "Dynamic CRC matched");
		return 0;
	}

	DPRINT(DEBUG_ERROR, "DYNAMIC CRC DID NOT MATCH !!!");
	return ERROR_AUTH_CRC_MISMATCH;
#endif
	return 0;
}

int BeceemWiMAX_Read_IMEI(PVOID pArg, USHORT *puiOutDataLen, UCHAR *pucOutData)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
		
	/*  IMEI will be of 8 bytes */
	/* CFG_ReadParam(pucOutData, 8, CFG_SEC_IMEI_ID); */
	CFG_ReadParam((unsigned char*)pWiMAX->pEEPROMContextTable, pucOutData, VSRO_IMEI_SIZE, CFG_VSA_VENDORSPECIFICDATA_ID);
/* 	DebugDump("IMEI", (UCHAR *)pucOutData, 8); */
	*puiOutDataLen = 8;
	return 0;
}

int BeceemWiMAX_AuthWriteParam(PVOID pArg, PWIMAX_MESSAGE pMsg)
{
	PWRITE_AUTH_PARAMS pstWriteAuthParams = 0;
	USHORT usWriteInfoType = 0;
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX || !pMsg)
		return ERROR_INVALID_ARGUMENT;

	pstWriteAuthParams = (PWRITE_AUTH_PARAMS) pMsg->szData;
	usWriteInfoType = pstWriteAuthParams->ucEAPMethodType;

	switch(usWriteInfoType)
	{
	case AUTH_METHOD_EAP_AKA:
		return BeceemWiMAX_Write_EAP_AKA(pWiMAX, pstWriteAuthParams->aucAuthParamsPayload);

	/* Not Supported currently */
	/*case AUTH_METHOD_EAP_SIM:
		return Write_ATR_LOCK(pstWriteAuthParams->aucAuthParamsPayload);
	*/
	}
	
	return 0;
}

int BeceemWiMAX_Write_EAP_AKA(PVOID pArg, UCHAR *pInData)
{
	/*  Will write only aucSEC_SNYMID_ID[23], aucSEC_SQNMS_ID[6*32] and checksum */
	PAUTH_WRITE_VECTOR pAuthWriteParams = (PAUTH_WRITE_VECTOR)pInData;
	//AUTH_WRITE_VECTOR AuthWriteParams;

	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;
	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

#if 0
	memcpy(AuthWriteParams.aucSEC_SNYMID_ID,  pAuthWriteParams->aucSEC_SNYMID_ID, sizeof(pAuthWriteParams->aucSEC_SNYMID_ID)); 
	memcpy(AuthWriteParams.aucSEC_SQNMS_ID,  pAuthWriteParams->aucSEC_SQNMS_ID, sizeof(pAuthWriteParams->aucSEC_SQNMS_ID)); 
#endif

	CFG_WriteVSA(pWiMAX->pEEPROMContextTable, pAuthWriteParams->aucSEC_SQNMS_ID, AUTH_ID_OFFSET, sizeof(pAuthWriteParams->aucSEC_SQNMS_ID));
	CFG_WriteVSA(pWiMAX->pEEPROMContextTable, pAuthWriteParams->aucSEC_SNYMID_ID, AUTH_ID_OFFSET+sizeof(pAuthWriteParams->aucSEC_SQNMS_ID), sizeof(pAuthWriteParams->aucSEC_SNYMID_ID));

#if 0	
	/*  Read ATRLock value for calculating the CRC */
	CFG_ReadParam(pWiMAX->pEEPROMContextTable, AuthWriteParams.aucATRLock, sizeof(pAuthWriteParams->aucSEC_SQNMS_ID)+sizeof(pAuthWriteParams->aucSEC_SNYMID_ID), sizeof(pAuthWriteParams->aucATRLock), CFG_VSA_VENDORSPECIFICDATA_ID);
	ulCRC = CFG_CalculateChecksum((UCHAR *)&AuthWriteParams, sizeof(AUTH_WRITE_VECTOR), 0xFFFF);

	/*  Write CRC */
	CFG_WriteVSA((UCHAR *)&ulCRC, sizeof(AUTH_WRITE_VECTOR), 4, 1);
#endif

	BeceemWiMAX_SendAuthIndication(pWiMAX, WRITE_AUTH_PARAM_REQUEST, AUTH_METHOD_EAP_AKA);

	return 0;
}


