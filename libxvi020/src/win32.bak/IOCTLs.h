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
 * Description	:	Beceem IOCTLs for Windows.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_IOCTLS
#define BECEEM_IOCTLS

#include <winioctl.h>

/* IOCTL's to send to driver for sending control requests to target code */

/* Currently using method buffered for all.Later could make some of them direct. */
/* 1.Control code for CONTROL MESSAGES */
#define IOCTL_SEND_CONTROL_MESSAGE CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x801,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)



/* 2.Control code to write a particular value to a particular register  */
#define IOCTL_BCM_REGISTER_WRITE CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x802,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* 3.control code to read a particular register */
#define IOCTL_BCM_REGISTER_READ CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x803,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* Control code to write a particular value to a particular register when device is in idle mode */
#define IOCTL_BCM_REGISTER_WRITE_PRIVATE CTL_CODE(FILE_DEVICE_UNKNOWN,\
                                                  0x826,				\
                                                  METHOD_BUFFERED, \
                                                  FILE_ANY_ACCESS)

/* control code to read a particular register when device in idle mode */
#define IOCTL_BCM_REGISTER_READ_PRIVATE CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x827,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)



/* 4.Control code to write x number of bytes to common memory  */
/* starting from address y */
#define IOCTL_BCM_COMMON_MEMORY_WRITE CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x804,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


#define IOCTL_BCM_GET_DRIVER_VERSION CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x829,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* 5.Control code to write x number of bytes to common memory  */
/* starting from address y */
#define IOCTL_BCM_COMMON_MEMORY_READ CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x805,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


/* 6.Control code for CONTROL MESSAGES */
#define IOCTL_GET_CONTROL_MESSAGE CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x806,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


/* 7.Control code for FIRMWARE DOWNLOAD */
#define IOCTL_BCM_FIRMWARE_DOWNLOAD CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x807,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* 8.Control code for FIRMWARE DOWNLOAD */
#define IOCTL_BCM_SET_SEND_VCID CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x808,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


/* IOCTL_WTM_CONTROL_REQ is same as IOCTL_CINR_LEVEL_REQ to avoid complexity in driver  */
#define IOCTL_WTM_CONTROL_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x817,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BE_BUCKET_SIZE CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x818,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_RTPS_BUCKET_SIZE CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x819,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_QOS_THRESHOLD	CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x820,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


#define IOCTL_DUMP_PACKET_INFO CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x821,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_MINIPORT_HALT CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x822,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_GET_PACK_INFO	CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x823,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_LINK_UP CTL_CODE(											\
											FILE_DEVICE_UNKNOWN,		\
											0x815,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_LINK_DOWN	CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x816,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_SWITCH_TRANSFER_MODE CTL_CODE(						\
											FILE_DEVICE_UNKNOWN,		\
											0x809,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_SWITCH_TRANSFER_MODE CTL_CODE(						\
											FILE_DEVICE_UNKNOWN,		\
											0x809,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_LINK_REQ	CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x80A,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_MAC_ADDR_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x811,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_MAC_ADDR_RESP	CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x812,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_RSSI_LEVEL_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x80B,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_IDLE_REQ CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x80C,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_SS_INFO_REQ CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x80D,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_GET_STATISTICS_POINTER CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x80E,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_CM_REQUEST CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x80F,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_INIT_PARAM_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x810,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_CLASSIFICATION_RULE CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x813,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)	

#define IOCTL_CINR_LEVEL_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x817,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* IOCTL_WTM_CONTROL_REQ is same as IOCTL_CINR_LEVEL_REQ to avoid complexity in driver  */
#define IOCTL_WTM_CONTROL_REQ CTL_CODE(									\
											FILE_DEVICE_UNKNOWN,		\
											0x817,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

/* IOCTL to make link up for device */
#define IOCTL_LINK_UP CTL_CODE(											\
											FILE_DEVICE_UNKNOWN,		\
											0x815,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define	IOCTL_CHIP_RESET CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x825,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_GET_CURRENT_STATUS CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x828,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_GPIO_SET_REQUEST CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x82A,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_GPIO_STATUS_REQUEST CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x82B,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_GET_HOST_MIBS CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x853,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

 

#define IOCTL_BCM_REGISTER_EVENT CTL_CODE(								\
											FILE_DEVICE_UNKNOWN,		\
											0x82F,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_BCM_AUTO_RESET_TOGGLE CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x830,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)
#define IOCTL_BCM_GET_DSX_INDICATION  CTL_CODE(							\
											FILE_DEVICE_UNKNOWN,		\
											0x854,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)


#define IOCTL_BCM_EEPROM_REGISTER_WRITE CTL_CODE(		\
					FILE_DEVICE_UNKNOWN,		\
					0x857,						\
					METHOD_BUFFERED,			\
					FILE_ANY_ACCESS)


#define IOCTL_BCM_EEPROM_REGISTER_READ CTL_CODE(		\
					FILE_DEVICE_UNKNOWN,		\
					0x858,						\
					METHOD_BUFFERED,			\
					FILE_ANY_ACCESS)

//
// BULK WRM
//
#define IOCTL_BCM_BULK_WRM CTL_CODE(		\
					FILE_DEVICE_UNKNOWN,		\
					0x90B,						\
					METHOD_BUFFERED,			\
					FILE_ANY_ACCESS)


/****************************************************************************************/
/*									LED STATE INFO											*/
/****************************************************************************************/
//IOCTLs for set/get the LED state information to/from the driver

#define IOCTL_SET_LED_BEHAVIOR_SETTING CTL_CODE(		\
					FILE_DEVICE_UNKNOWN,		\
					0x901,						\
					METHOD_BUFFERED,			\
					FILE_ANY_ACCESS)

#define IOCTL_GET_LED_BEHAVIOR_SETTING CTL_CODE(		\
					FILE_DEVICE_UNKNOWN,		\
					0x902,						\
					METHOD_BUFFERED,			\
					FILE_ANY_ACCESS)
/****************************************************************************************/
/*									LED STATE INFO											*/
/****************************************************************************************/

#define IOCTL_BCM_WAKE_UP_DEVICE_FROM_IDLE  CTL_CODE(										\
											FILE_DEVICE_UNKNOWN,		\
											0x859,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define IOCTL_DUMP_DEBUG			CTL_CODE(   \
											FILE_DEVICE_UNKNOWN,		\
											0x824,						\
											METHOD_BUFFERED,			\
											FILE_ANY_ACCESS)

#define	IOCTL_BCM_LIB_SHAREDEVENTS	CTL_CODE(  \
											FILE_DEVICE_UNKNOWN,  \
											0x905,      \
											METHOD_BUFFERED,   \
											FILE_ANY_ACCESS)
//

// NVM Read: Read from EEPROM or FLASH based on CFG option.

//

#define IOCTL_BCM_NVM_READ CTL_CODE(               \
                                            FILE_DEVICE_UNKNOWN, \
                                            0x907,\
                                            METHOD_BUFFERED,\
                                            FILE_ANY_ACCESS)
//

// NVM Write: Write to EEPROM or FLASH based on CFG option.

//

#define IOCTL_BCM_NVM_WRITE CTL_CODE(               \
                                            FILE_DEVICE_UNKNOWN, \
                                            0x908,\
                                            METHOD_BUFFERED,\
                                            FILE_ANY_ACCESS)
//

// Returns the EEPROM size.

//
#define IOCTL_BCM_GET_NVM_SIZE CTL_CODE(               \
                                            FILE_DEVICE_UNKNOWN, \
                                            0x909,\
                                            METHOD_BUFFERED,\
                                            FILE_ANY_ACCESS)
#define IOCTL_BCM_GPIO_MODE_REQUEST CTL_CODE(               \
                                            FILE_DEVICE_UNKNOWN, \
                                            0x82E,\
                                            METHOD_BUFFERED,\
                                            FILE_ANY_ACCESS)
#define IOCTL_BCM_GPIO_MULTI_REQUEST CTL_CODE(               \
                                            FILE_DEVICE_UNKNOWN, \
                                            0x82D,\
                                            METHOD_BUFFERED,\
                                            FILE_ANY_ACCESS)
#define IOCTL_BCM_FLASH2X_SECTION_READ   CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x865,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)

#define IOCTL_BCM_FLASH2X_SECTION_WRITE  CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x866,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)
     
#define IOCTL_BCM_GET_FLASH2X_SECTION_BITMAP CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x867,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)
     
#define IOCTL_BCM_SET_ACTIVE_SECTION CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x868,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)


#define IOCTL_BCM_COPY_SECTION CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x870,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)
     
#define IOCTL_BCM_GET_FLASH_CS_INFO CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x871,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)
     
#define IOCTL_BCM_SELECT_DSD CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x872,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)

#define IOCTL_BCM_NVM_RAW_READ CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x875,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)

#define IOCTL_BCM_TIME_SINCE_NET_ENTRY  CTL_CODE(  \
     FILE_DEVICE_UNKNOWN,  \
     0x876,      \
     METHOD_BUFFERED,   \
     FILE_ANY_ACCESS)

#define IOCTL_BCM_GET_DEVICE_DRIVER_INFO  CTL_CODE(  \
						FILE_DEVICE_UNKNOWN,  \
					     0x877,      \
					     METHOD_BUFFERED,   \
     					   FILE_ANY_ACCESS)

#define IOCTL_BCM_LED_THREAD_STATE_CHANGE_REQ  CTL_CODE( \
     				FILE_DEVICE_UNKNOWN,  			\
     				0x878,      					\
			        METHOD_BUFFERED,   				\
			        FILE_ANY_ACCESS)			        


#define IOCTL_BCM_CONFIG_FILE_DOWNLOAD   CTL_CODE( \
     				FILE_DEVICE_UNKNOWN,  			\
     				0x87B,      					\
			        METHOD_BUFFERED,   				\
			        FILE_ANY_ACCESS)			

#endif /* BECEEM_IOCTLS */
