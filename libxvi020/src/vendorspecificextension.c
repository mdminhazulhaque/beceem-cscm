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
 * Description	:	Place holder for vendor specific extention to Beceem
 *					APIs.
 * Author		:
 * Reviewer		:
 *
 */


#include "vendorspecificextension.h"
#include "BeceemWiMAX.h"

ST_VENDOR_SPECIFIC_EXTN_RET VendorSpecificExtensionDispatch(PVOID pArg, PWIMAX_MESSAGE pucSendBuffer, UINT uiLength)
{
	ST_VENDOR_SPECIFIC_EXTN_RET stVendorSpecificExtRet = {0};
	UNUSED_ARGUMENT(uiLength);
	UNUSED_ARGUMENT(pucSendBuffer);
	UNUSED_ARGUMENT(pArg);
	
	stVendorSpecificExtRet.bContinueWithBCMPath = TRUE;/*-Required for Regular Beceem API implementaiton execution*/
	stVendorSpecificExtRet.iReturnValue = SUCCESS; /*-This should be the return status of any API call prior to calling beceem API path
												     -If this field is not SUCCESS API call returns immediately without continuing with BeceemAPI execution path
												   */
	/*
	- Vendor specific extensions for specific APIs should be added here
	*/
	return stVendorSpecificExtRet;
}

int BeceemWiMAX_VendorSpecificInitFunc(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	
	return SUCCESS;
}

int BeceemWiMAX_VendorSpecificExitFunc(PVOID pArg)
{
	PBECEEMWIMAX pWiMAX = (PBECEEMWIMAX)pArg;

	if(!pWiMAX)
		return ERROR_INVALID_ARGUMENT;

	
	return SUCCESS;

}
