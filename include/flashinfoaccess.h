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
 * Description	:	Flash Access Definitions and Structures
 * Author		:
 * Reviewer		:
 *
 */



/**
@file flashinfoaccess.h
*/
#ifndef BCM_FLASH_INFO_ST_H
#define BCM_FLASH_INFO_ST_H


#pragma pack (push,1)


/**
Flash control section version info struct
*/
typedef struct _DYNAMIC_FLASH_CS_INFO
{
	
	// magic number as 0xBECE-F1A5 - F1A5 for "flas-h"
	B_UINT32 MagicNumber;/**< Magic number */					

	// Flash Layout versioning
	B_UINT16 FlashLayoutMajorVersion;/**< Flash layout Major number*/		
	B_UINT16 FlashLayoutMinorVersion; /**<Flash layout Minor number */

	// ISO Image/Format/BuildTool versioning
	B_UINT16 ISOImageMajorVersion;/**< ISO Image Major number*/
	B_UINT16 ISOImageMinorVersion; /**<ISO Image Minor number */

	// SCSI/Flash BootLoader versioning
	B_UINT16 SCSIFirmwareMajorVersion;	/**<SCSI firmware major version */	
	B_UINT16 SCSIFirmwareMinorVersion; /**<SCSI firmware minor version */


}DYNAMIC_FLASH_CS_INFO,*PDYNAMIC_FLASH_CS_INFO;

/**
flash sections enum
*/
typedef enum _E_DYNAMIC_SECTION_VAL
{
	NO_SECTION_VAL = 0,/**< no section *///for reading value from absolute offset. 
	ISO_IMAGE1,/**< ISO Image1 */
	ISO_IMAGE2,/**< ISO Image2*/
	DSD0,/**< DSD0*/
	DSD1,/**< DSD1*/
	DSD2,/**< DSD2*/
	VSA0,/**< Vendor specific area 0*/
	VSA1,/**< Vendor specific area 1*/
	VSA2,/**< Vendor specific area 2*/
	SCSI,/**< SCSI section*/
	CONTROL_SECTION/**<Control section */
} E_DYNAMIC_SECTION_VAL,*PE_DYNAMIC_SECTION_VAL;

/**
Flash read/write struct
*/
typedef struct _DYNAMIC_FLASH_READWRITE_ST
{
	E_DYNAMIC_SECTION_VAL Section;/**<Section type. Refer to #_E_DYNAMIC_SECTION_VAL */	
	B_UINT32 offset;/**< Offset within section*/	    //Offset within Section.
	B_UINT32 numOfBytes;  /**< Number of bytes*/  //NOB from the offset
	B_UINT32 bWriteVerify;/**< Write verify. Valid only for Writes*/
	B_UINT8 *pDataBuff;  /**< Pointer to buffer used for read/write*///Buffer for reading/writing
} DYNAMIC_FLASH_READWRITE_ST,*PDYNAMIC_FLASH_READWRITE_ST;

/**
Bitmap struct for different sections on flash
*/
typedef struct _ST_FLASH2X_BITMAP
{
	UCHAR ISO_IMAGE1;/**<bitmap info of ISO_IMAGE1*/
	UCHAR ISO_IMAGE2;/**<bitmap info of ISO_IMAGE2*/
	UCHAR DSD0;/**<bitmap info of DSD0 */
	UCHAR DSD1;/**<bitmap info of DSD1*/
	UCHAR DSD2;/**<bitmap info of DSD2*/
	UCHAR VSA0;/**<bitmap info of VSA0*/
	UCHAR VSA1;/**<bitmap info of VSA1*/
	UCHAR VSA2;/**<bitmap info of VSA2*/
	UCHAR SCSI;/**<bitmap info of SCSI*/
	UCHAR CONTROL_SECTION;/**< bitmap info of Control section*/
	//Reserved for future use
	UCHAR Reserved0;
	UCHAR Reserved1;
	UCHAR Reserved2;
} ST_FLASH2X_BITMAP, *PST_PFLASH2X_BITMAP;

/**
Flash copy section struct
*/
typedef struct _ST_FLASH2X_COPY_SECTION
{
	E_DYNAMIC_SECTION_VAL eSrcSection;	/**<Source section. Refer to #_E_DYNAMIC_SECTION_VAL  */
	E_DYNAMIC_SECTION_VAL eDstSection;	/**<Destination section. Refer to #_E_DYNAMIC_SECTION_VAL  */
	B_UINT32 offset;/**<Offset with in section */	    //Offset within Section.
	B_UINT32 numOfBytes; /**<Number of bytes to copy*/   //NOB from the offset
} ST_FLASH2X_COPY_SECTION,*PST_FLASH2X_COPY_SECTION;

/**
Flash raw access struture
*/
typedef struct _ST_FLASH_RAW_ACCESS_
{
UINT32 uiOffset;/**<Offset to read*/
UINT32 uiSize;/**<Total data to read*/
PUCHAR pBuffer;/**<Output buffer to be allocated by application*/ 
}ST_FLASH_RAW_ACCESS,*PST_FLASH_RAW_ACCESS;

//Enums and structure required for decoding 'BcmBasebandFlashMapInfo' MIB
//Bit mapped data in BcmBasebandFlashMapInfo needs to be decoded based on the below structure and enums

/**
Flash map details structure
*/
 typedef struct _ST_FLASHMAPDETAILS_
 {
	 B_UINT32  u32Reserved:8;
	 B_UINT32  u32AutoInitSelectedNvm:8;/**<refer to #_E_AUTOINITSELECTEDNVM*/
	 B_UINT32  u32DSD2SectionInfo:8;/**<refer to #_E_DSDSECTIONINFO_*/
	 B_UINT32  u32DSD1SectionInfo:8;/**<refer to #_E_DSDSECTIONINFO_*/
	 B_UINT32  u32DSD0SectionInfo:8;/**<refer to #_E_DSDSECTIONINFO_*/	
	 B_UINT32  u32ISO2SectionInfo:8;/**<refer to #_E_ISOSECTIONINFO_*/
	 B_UINT32  u32ISO1SectionInfo:8;/**<refer to #_E_ISOSECTIONINFO_*/
	 B_UINT32  u32MscToNwSwitchReason:8;/*<refer to #_E_SWITCHREASON_*/
 }ST_FLASHMAPDETAILS,*PST_FLASHMAPDETAILS;

/**
enum ISO section info
*/
 typedef enum _E_ISOSECTIONINFO_
 {
	 eISOSectionValid=1,/**<Section valid*/
	 eISOSectionCorrupted=2,/**<Section corrupted*/
	 eISOSectionNotUsed=3,/**<Section not used*/
	 eISOSectionNotDefined=4,/**<Section not defined*/
	 eISOSectionValidAndSelected=5/**<Section valid and selected*/
 }E_ISOSECTIONINFO; 
/**
enum DSD section info
*/
 typedef enum _E_DSDSECTIONINFO_
 {
	 eDSDSectionValid=1,/**<Section valid*/
	 eDSDSectionCorrupted=2,/**<Section corrupted*/
	 eDSDSectionNotUsed=3,/**<Section not used*/
	 eDSDSectionNotDefined=4,/**<Section not defined*/
	 eAutoInitExecuted=5/**<Auto init executed*/
 }E_DSDSECTIONINFO;

/**
enum network mode switch reason
*/
 typedef enum _E_SWITCHREASON_
 {
	 eSwitchReasonNotDefined = 0,/**<eSwitchReasonNotDefined*/
	 eNwModeSwitchReason_Sw_Cmd_Host = 1,/**<eNwModeSwitchReason_Sw_Cmd_Host*/
	 eNwModeSwitchReason_InactiveTimeout = 2,/**<eNwModeSwitchReason_InactiveTimeout*/
	 eNwModeSwitchReason_MscTimeout = 3,/**<eNwModeSwitchReason_MscTimeout*/
	 eNwModeSwitchReason_Bad_Cs = 4,/**<eNwModeSwitchReason_Bad_Cs*/
	 eNwModeSwitchReason_Bad_Iso = 5,/**<eNwModeSwitchReason_Bad_Iso*/
	 eNwModeSwitchReason_Flash_Boot =6/**<eNwModeSwitchReason_Flash_Boot*/ 
}E_SWITCHREASON;

/**
enum auto init selected
*/
typedef enum _E_AUTOINITSELECTEDNVM
{
        eAutoInitFromEEPROM = 1,/**<eAutoInitFromEEPROM*/
    	 eAutoInitFromFlash = 2,/**<eAutoInitFromFlash*/
	 eDefaultAutoInit = 3   /**<eDefaultAutoInit*/
}E_AUTOINITSELECTEDNVM,*PAUTOINITSELECTEDNVM;


#pragma pack(pop)


#endif /* BCM_FLASH_INFO_ST_H */
