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
 *					APIs - API declarations.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_WIMAX_VENDOR_SPECIFIC_EXTN
#define BECEEM_WIMAX_VENDOR_SPECIFIC_EXTN


#include "PrivateTypedefs.h"
#include "ErrorDefs.h"

/* This file is for vendor specific extensions */

#define MAX_GPIO_NUMBER_SUPPORTED 16

/* Vendor specific operation after opening device */
int BeceemWiMAX_VendorSpecificInitFunc(PVOID pArg);

/* Vendor specific operation before closing device*/
int BeceemWiMAX_VendorSpecificExitFunc(PVOID pArg);


/*
  Vendor specific extensions for specific API(TYPE/SUBTYPE combinations) should be handled through this function.
*/
ST_VENDOR_SPECIFIC_EXTN_RET VendorSpecificExtensionDispatch(PVOID pArg, 
															PWIMAX_MESSAGE pucSendBuffer, UINT uiLength);


#endif /* BECEEM_WIMAX_VENDOR_SPECIFIC_EXTN */
