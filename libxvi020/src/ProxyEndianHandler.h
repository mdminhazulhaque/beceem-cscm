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
 * Description	:	Endian-ness handler for proxy (applicable only for
 *					big-endian machines) - APIs.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef WIMAXMESSAGE_PROXY_ENDIAN_HANDLER_H
#define WIMAXMESSAGE_PROXY_ENDIAN_HANDLER_H
#include "BeceemWiMAX.h"
#include "PrivateTypedefs.h"
#include "Typedefs.h"
#include "ErrorDefs.h"
#include "Messages.h"

UINT CorrectEndiannessForProxy(PWIMAX_MESSAGE pWiMAXMessage,
	UINT uiDataLenth,const UINT uiDirectionOfDataFlow /*1 - going out of library
												0- Coming into  Library*/);

void CheckMachineEndianness();

#endif
