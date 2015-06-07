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
 * Description	:	This file contains MRU format used in EEPROM
 * Author		:
 * Reviewer		:
 *
 */


#ifndef	_MRU_EEPROM_H__
#define _MRU_EEPROM_H__

#include "OMADMTreeStruct.h"

typedef struct _MRU_ROW_FORMAT
{
	UINT16 NextIndex:6;
	UINT16 WhichIsPrimary:2;
	UINT16 WhichIsSecondary1:2;
	UINT16 ChannelPlanIndex:6;
}MRU_ROW_FORMAT, *PMRU_ROW_FORMAT;

typedef struct _ROW_DESCRIPTOR
{
  MRU_ROW_FORMAT stMruContents;   
  UINT16 u16RowIndex;
  UINT16 u16Padding;
}ROW_DESCRIPTOR;

typedef struct _MRU_INIT_STRUCT
{
	UINT16 u16Length;
	UINT8  u8Version;
	UINT8  u8Valid;
	UINT8  u8HeadIndex;
	UINT8  u8TailIndex;
	UINT8  U8WasChecksumCorrected;
	UINT8  Padding;
    MRU_ROW_FORMAT mruRows[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];

}MRU_INIT_STRUCT;

typedef struct _ST_MRU_CHANGELIST
{
  UINT8  u8HeadIndex;
  UINT8  u8TailIndex;
  UINT16 u16NumRows;
  ROW_DESCRIPTOR stRowDesciptor[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];

}ST_MRU_CHANGELIST, *PST_MRU_CHANGELIST;


#endif /* _MRU_EEPROM_H__ */
