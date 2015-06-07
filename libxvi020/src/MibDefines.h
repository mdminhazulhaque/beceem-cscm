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
 * Description	:	This header file contains the defines for all MIBS.
 * 					The grouping of MIBS is based on attributes and
 *					detailed below.
 * LEVEL    ATTRIBUTE      MEANING
 * L1       BIT_HO_IMPL    Indicates this MIB is implemented by host
 * L2       BIT_TGT_IMPL   Indicates this MIB is implemented by the target.
 * L2.1     BIT_TGT_CONFIG Indicates this is a Config MIB
 * L2.2     BIT_TGT_STATS  Indicates this is a Stats MIB
 *
 * Author		:
 * Reviewer		:
 *
 */


#ifndef _BCM_MIB_DEFINES_H
#define _BCM_MIB_DEFINES_H

#define BCM_STANDARD_CFG_MIB_BASE_START     0
#define BCM_STANDARD_CFG_MIB_BASE_END     499    

#define BCM_PROPREITARY_CFG_MIB_BASE_START       500
#define BCM_PROPREITARY_CFG_MIB_BASE_END       799

#define BCM_STANDARD_STATS_MIB_BASE_START        800
#define BCM_STANDARD_STATS_MIB_BASE_END          1399 

#define BCM_PROPREITARY_STATS_MIB_BASE_START     1400
#define BCM_PROPREITARY_STATS_MIB_BASE_END       1699   



/*Types*/
#define TYPE_UINT8				0x01
#define TYPE_UINT16				0x02
#define TYPE_UINT32				0x03
#define TYPE_MACADDREESS		0x04

/* Attribute Definitions */
/*
                 					B15 B14 B13 B12 B11 B10 B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
BIT_HO_IMPL      					 1   0   0   0   
BIT_HO_IMPL_ONE_INDEX				 1   0   0   1   
BIT_HO_IMPL_TWO_INDEX				 1   0   1   0   
BIT_TGT_IMPL     					                 1   0  0  0  
BIT_TGT_CONFIG   					                 1   1  0  0  
BIT_TGT_CONFIG_ONE_INDEX                             1   1  0  1  
BIT_TGT_CONFIG_TWO_INDEX                             1   1  1  0  
BIT_TGT_STATS    					                 1   0  0  0  1  0  0  0  
BIT_TGT_STATS_ONE_INDEX                              1   0  0  0  1  0  0  1  
BIT_TGT_STATS_TWO_INDEX                              1   0  0  0  1  0  1  0  
*/

/*
#define BIT_HO_IMPL                0x8000
#define BIT_HO_IMPL_ONE_INDEX      0x9000
#define BIT_HO_IMPL_TWO_INDEX      0xA000
#define BIT_TGT_IMPL               0x0800
#define BIT_TGT_CONFIG             0x0C00
#define BIT_TGT_CONFIG_ONE_INDEX   0x0D00
#define BIT_TGT_CONFIG_TWO_INDEX   0x0E00
#define BIT_TGT_STATS              0x0880
#define BIT_TGT_STATS_ONE_INDEX    0x0890
#define BIT_TGT_STATS_TWO_INDEX    0x08A0
*/

#define BIT_TGT_STD_CONFIG			0x0000		/* -> read from sharedmem/cellram */
#define BIT_TGT_PROP_CONFIG			0x0100		/* -> read from config file */
#define BIT_TGT_STD_STATS			0x0200		/* -> read from registers */
#define BIT_HOST_STD_STATS			0x0300		/* -> read from IOCTL */
#define BIT_TGT_PROP_STATS			0x0400		/* -> read from registers */

#define BIT_NON_INDEX				0x0000
#define BIT_ONE_INDEX				0x0001
#define BIT_TWO_INDEX				0x0002

typedef struct _S_MIB_INFO
{
	UINT16	u16MIBAttribute;

	UINT32	u32Offset;			/* Distance from base address of stats struct or starting table address */
	UINT32	u32FieldSize;		/* Field size in bytes */
	UINT8	u8FieldType;		/* Field type */

	/* below are the table specific data */

	UINT32	u32TableOffset;		/* offset of table */
	UINT16	u16TableRowSize;	/* size of single row */
	UINT8	u8NumRows;			/* number of rows */

}S_MIB_INFO, *PS_MIB_INFO;


#endif /* _BCM_MIB_DEFINES_H */
