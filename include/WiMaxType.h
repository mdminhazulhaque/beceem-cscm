/**
 * \file WiMaxType.h
 * WiMaxType.h
 * Defines the data types used throughout the WiMax Common API
 * Version 1.2 August 17 2007
 */

#ifndef BCM_WIMAXTYPE_H


#define BCM_WIMAXTYPE_H
/// maximum length of a NSP name
#define MAX_SIZE_OF_NSP_NAME       (128+1)

/// maximum length of a string buffer
#define MAX_SIZE_OF_STRING_BUFFER  256
//#pragma pack(push,4)
#ifdef WIN32 
/// unsigned 8-bit
typedef unsigned char  UINT8;

/// signed 16-bit
typedef short          INT16;

/// unsigned 16-bit
typedef unsigned short UINT16;  


/// signed 32-bit
typedef signed int     SINT32;


#endif

/// unsigned 16-bit char
typedef UINT16         UCHAR16;


/// signed 64-bit
typedef long long          INT64; 

/// unsigned 64-bit
typedef unsigned long long UINT64; 

/// 8-bit string null terminated
typedef UINT8*  WIMAX_API_STRING; 

typedef UINT8*  PUINT8; 

/// 16-bit string (unicode) UTF-16 null terminated
typedef UCHAR16* WIMAX_API_WSTRING;  

/// 32-bit profile id
/// Profile id 0 is reserve to connection without specifying a user account.
typedef UINT32  WIMAX_API_PROFILE_ID;     

typedef UINT32*  PUINT32;     
/// Enumeration of all the privilege level.
typedef enum _WIMAX_API_PRIVILEGE
{
	WIMAX_API_PRIVILEGE_READ_WRITE, /**< Read Write privilege access. */
	WIMAX_API_PRIVILEGE_READ_ONLY,  /**< Read Only privilege access. */
} WIMAX_API_PRIVILEGE;


/// Enumeration card presence state.
typedef enum _WIMAX_API_DEVICE_PRESENCE
{
	WIMAX_API_CARD_REMOVAL,         /**< card removal enum value 0 */
	WIMAX_API_CARD_INSERTION,       /**< card insertion enum value 1 */
} WIMAX_API_DEVICE_PRESENCE;

/// Enumeration device type.
typedef enum _WIMAX_API_DEVICE_TYPE
{
	WIMAX_API_DEV_TYPE_WIMAX,		/**< WiMax device type */
		SingleModeExpressPCCard,
		DualModeExpressPCCard,
		SingleModeUSBPCCard,
		DualModeUSBPCCar,
		BasicModem,
		SOHOModem,
		PMP,
		DualModePMP,
		UMPC,
		EmbeddedLaptop,
		InternetTablet,
		SingleModeHandset,
		DualModeHandset,
		PDA,
		GamingDev,
		VideoPhone,
		M2M,
		Camera,
		Camcorder,
		WearableDev,
		DualModeMsgDev,
		EBook,
		NavigationDev,
		InVehicleEntDev,
		HomeMediaGW,
			
} WIMAX_API_DEVICE_TYPE;

/// device identification structure
typedef struct _WIMAX_API_DEVICE_ID
{
	UINT32				  structureSize;        /**< size of this structure. */
	UINT32				  sdkHandle;            /**< Handle that is configured by the vendor specific SDK and usage is limited inside the SDK for vendor specific references. */
												/**< The SDK handle is set and used only by the SDK. This can be used to identify the calling application. */
												/**< This is an implementation specific for each vendor. */
	WIMAX_API_PRIVILEGE	  privilege;            /**< Read only or read write */
	UINT8				  deviceIndex;          /**< The index of the device. The device index 0 is reserved. 0 equal no device. */
	UINT32				  apiVersion;           /**< MSB 16bits are the major release number, LSB 16 bits are the minor release number */
    BOOL                  devicePresenceStatus; /**< The device presence status */
} WIMAX_API_DEVICE_ID, *WIMAX_API_DEVICE_ID_P;

/// Hardware ID
typedef struct _WIMAX_API_HW_DEVICE_ID
{
	UINT32					structureSize;     /**< size of this structure. */
	UINT8					deviceIndex;       /**< The index of the device. The index must stay the same for a device until the SDK is closed. */
	UCHAR16                 deviceName[MAX_SIZE_OF_STRING_BUFFER];  /**< Name of the device. Must be the same name then the OS device name.  */
	WIMAX_API_DEVICE_TYPE   deviceType;        /**< The type of the device */
} WIMAX_API_HW_DEVICE_ID, *WIMAX_API_HW_DEVICE_ID_P;

/// Device status enum
typedef enum _WIMAX_API_LOCK_STATUS
{
	WIMAX_API_DEVICE_LOCKED,    /**< Device is locked */
	WIMAX_API_DEVICE_UNLOCKED,  /**< Device is unlocked */
} WIMAX_API_LOCK_STATUS, *WIMAX_API_LOCK_STATUS_P;

/// Network type enum
typedef enum _WIMAX_API_NETWORK_TYPE
{
	WIMAX_API_HOME,              /**< Home network */
    WIMAX_API_PARTNER,           /**< Partner network */
	WIMAX_API_ROAMING_PARTNER,   /**< Roaming partner network */
	WIMAX_API_UNKNOWN            /**< Unknown network */
} WIMAX_API_NETWORK_TYPE;

/// NSP information structure
typedef struct _WIMAX_API_NSP_INFO
{
	UINT32                    structureSize;                 /**< size of this structure. */
	UCHAR16                   NSPName[MAX_SIZE_OF_NSP_NAME]; /**< the name of the NSP */
	UINT32                    NSPid;                         /**< Identifier */
	UINT8                     RSSI;                          /**< RSSI value */
	UINT8                     CINR;                          /**< CINR value */  
	WIMAX_API_NETWORK_TYPE    networkType;                   /**< Current network type */    
} WIMAX_API_NSP_INFO, *WIMAX_API_NSP_INFO_P;

/// Link status information structure
typedef struct _WIMAX_API_LINK_STATUS_INFO
{
	UINT32             structureSize;      /**< size of this structure. */
	UINT32             centerFrequency;    /**< Center frequncy of the current link from device to network, in unit of KHz */
   	UINT8              RSSI;               /**< Mean RSSI measured by device. As according to IEEE 802.16 standard, values are ranging from 0x00 to 0x53, where -123dBm is encoded as 0x00 and -40dBm encoded as 0x53 with 1dB increments. */ 
	UINT8              CINR;               /**< Mean RSSI measured by device. As according to IEEE 802.16 standard, values are ranging from 0x00 to 0x3F, where -10dB is encoded as 0x00 and 53dB encoded as 0x3F with 1dB increments. */
	UINT8              txPWR;              /**< Average transmit power for the last burst transmitted by the device. Based on IEEE802.16 standard, the values are ranging from 0xxx to 0xFF, where -84dBm is encoded as 0x00  and 43.5dBm is encoded as 0xFF with 0.5dB increment. */
	UINT8              bsId[6];            /**< BS ID of the current serving BS as received from the network */
} WIMAX_API_LINK_STATUS_INFO, *WIMAX_API_LINK_STATUS_INFO_P;

/// Profile information structure
typedef struct _WIMAX_API_PROFILE_INFO
{
	UINT32                structureSize;                          /**< size of this structure. */
	WIMAX_API_PROFILE_ID  profileID;                              /**< profile ID. Profile id 0 is reserve to connection without specifying a user account. */
	UCHAR16               profileName[MAX_SIZE_OF_STRING_BUFFER]; /**< profile name. */ 
} WIMAX_API_PROFILE_INFO, *WIMAX_API_PROFILE_INFO_P;

/// Device version
typedef struct _WIMAX_API_DEVICE_VERSION
{
	UINT32    structureSize;                      /**< size of this structure. */
	UCHAR16   name[MAX_SIZE_OF_STRING_BUFFER];    /**< name of the parts, such as ASIC, Board, RF, SW System. */
	UCHAR16   version[MAX_SIZE_OF_STRING_BUFFER]; /**< Version of the part */
} WIMAX_API_DEVICE_VERSION, *WIMAX_API_DEVICE_VERSION_P;

/// Device information structure
typedef struct _WIMAX_API_DEVICE_INFO
{
	UINT32                      structureSize;            /**< size of this structure. */
	WIMAX_API_DEVICE_VERSION    hwVersion;                /**< Hardware version */
	WIMAX_API_DEVICE_VERSION    swVersion;                /**< SW version */
	WIMAX_API_DEVICE_VERSION    rfVersion;                /**< RF version */
	WIMAX_API_DEVICE_VERSION    asicVersion;              /**< ASIC Version */
 	UINT8                       macAddress[6];            /**< AS defined by IEEE802.16 standard, 48bits device MAC address */
	UCHAR16                     vendorName[MAX_SIZE_OF_STRING_BUFFER]; /**< Name of the hardware vendor */
	BOOL                        vendorSpecificInfoIncl;   /**< Indicating the inclusion of vendor specific information */
    UCHAR16                     vendorSpecificInfo[MAX_SIZE_OF_STRING_BUFFER]; /** <Vendor specific information */
} WIMAX_API_DEVICE_INFO, *WIMAX_API_DEVICE_INFO_P;

/// Device status enum
typedef enum _WIMAX_API_DEVICE_STATUS
{
	WIMAX_API_DEVICE_STATUS_UnInitialized,  /**<  Device is uninitialized */
	WIMAX_API_DEVICE_STATUS_RF_OFF_HW_SW,   /**<  Device RF Off(both H/W and S/W) */
	WIMAX_API_DEVICE_STATUS_RF_OFF_HW,      /**<  Device RF Off(via H/W switch) */
	WIMAX_API_DEVICE_STATUS_RF_OFF_SW,      /**<  Device RF Off(via S/W switch) */
	WIMAX_API_DEVICE_STATUS_Ready,          /**<  Device is ready */
	WIMAX_API_DEVICE_STATUS_Scanning,       /**<  Device is scanning */
	WIMAX_API_DEVICE_STATUS_Connecting,     /**<  Connection in progress */
	WIMAX_API_DEVICE_STATUS_Data_Connected	/**<  Layer 2 connected */
} WIMAX_API_DEVICE_STATUS, *WIMAX_API_DEVICE_STATUS_P;

/// Connection progress information structure
typedef enum _WIMAX_API_CONNECTION_PROGRESS_INFO
{
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_Ranging,                   /**< Device is in Ranging */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_SBC,                       /**< Device is in SBC */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_EAP_authentication_Device, /**< Device is in EAP authentication Device */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_EAP_authentication_User,   /**< Device is in EAP authentication User */
 	WIMAX_API_DEVICE_CONNECTION_PROGRESS_3_way_handshake,           /**< Device is in 3-way-handshake */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_Registration,              /**< Device is in Registration */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_De_registration,           /**< Device is in De-registration */
	WIMAX_API_DEVICE_CONNECTION_PROGRESS_Registered,		        /**< Device is registered (operational) */
} WIMAX_API_CONNECTION_PROGRESS_INFO, *WIMAX_API_CONNECTION_PROGRESS_INFO_P;

/// The ststus reason enum
typedef enum _WIMAX_API_STATUS_REASON
{
	WIMAX_API_STATUS_REASON_Normal,
	WIMAX_API_STATUS_REASON_Fail_to_connect_to_NW,           /**< Failed to complete NW entry with the selected operator (unspecified reason).  */
	WIMAX_API_STATUS_REASON_Fail_to_connect_Ranging,         /**< Failed to complete ranging */
	WIMAX_API_STATUS_REASON_Fail_to_connect_SBC,             /**< SBC phase failed */
	WIMAX_API_STATUS_REASON_Fail_to_connect_EAP_AUTH_Device, /**< Security error. EAP authentication failed device level */
	WIMAX_API_STATUS_REASON_Fail_to_connect_EAP_AUTH_user,   /**< Security error. EAP authentication failed user level */
	WIMAX_API_STATUS_REASON_Fail_to_connect_3_Way_Handshake, /**< Security error. Handshake failed */
	WIMAX_API_STATUS_REASON_Fail_to_connect_REG,             /**< Registration failed */
	WIMAX_API_STATUS_REASON_Fail_to_connect_datapath	     /**< Failed to initialize the data path (failed to perform DSA to one UL and one DL SFs). */
} WIMAX_API_STATUS_REASON;

/// RF Status enum
typedef struct _WIMAX_API_RF_SWITCHES_STATUS
{
	UINT32   structureSize; /**< size of this structure. */
	BOOL     hw_RF_ON;      /**< Hardware RF is on */
	BOOL     sw_RF_ON;      /**< Software RF is on */ 
} WIMAX_API_RF_SWITCHES_STATUS, *WIMAX_API_RF_SWITCHES_STATUS_P;

/// Connected NSP information structure
typedef struct _WIMAX_API_CONNECTED_NSP_INFO
{
	UINT32                      structureSize;                 /**< size of this structure. */
	UCHAR16                     NSPName[MAX_SIZE_OF_NSP_NAME]; /**< Name of NSP */
	UCHAR16                     NSPRealm[MAX_SIZE_OF_NSP_NAME];/**< Name of NSP */
	UINT32                      NSPid;                         /**< Identifier */
	BOOL                        activated;                     /**< Is the device activated TRUE=Activated FALSE=NotActivated */
	UINT8                       RSSI;                          /**< Mean RSSI measured by device. As according to IEEE 802.16 standard, values are ranging from 0x00 to 0x53, where -123dBm is encoded as 0x00 and -40dBm encoded as 0x53 with 1dB increments. */
	UINT8                       CINR;                          /**< Mean RSSI measured by device. As according to IEEE 802.16 standard, values are ranging from 0x00 to 0x3F, where -10dB is encoded as 0x00 and 53dB encoded as 0x3F with 1dB increments. */
	WIMAX_API_NETWORK_TYPE      networkType;                   /**< Current network type */
} WIMAX_API_CONNECTED_NSP_INFO, *WIMAX_API_CONNECTED_NSP_INFO_P;

/// Roaming mode
typedef enum _WIMAX_API_ROAMING_MODE
{  
	WIMAX_API_ROAMING_ENABLED,  /**< Enable - Connection to roaming partner NSPs is allowed. */
	WIMAX_API_ROAMING_DISABLED  /**< Disable - Connection to roaming partner NSPs is NOT allowed. */ 
} WIMAX_API_ROAMING_MODE, *WIMAX_API_ROAMING_MODE_P;

/// Connection statistic structure
typedef struct _WIMAX_API_CONNECTION_STAT
{
	UINT32   structureSize;    /**< size of this structure. */
	UINT64   totalRxByte;      /**< Total transmitted bytes of payload traffic*/
	UINT64   totalTxByte;      /**< Total received bytes of payload traffic*/
	UINT64   totalRxPackets;   /**< Total number of received packet. Equivalent to IP packet. */ 
    UINT64   totalTxPackets;   /**< Total number of transmitted packet. Equivalent to IP packet. */
} WIMAX_API_CONNECTION_STAT, *WIMAX_API_CONNECTION_STAT_P;

/// Contact information structure
typedef struct _WIMAX_API_CONTACT_INFO
{
	UINT32    structureSize;	                              /**< size of this structure. */
	UCHAR16   textForURI[MAX_SIZE_OF_STRING_BUFFER];		  /**< Text coming with URI. */
	UCHAR16   URI[MAX_SIZE_OF_STRING_BUFFER];	              /**< URI to be displayed. */
} WIMAX_API_CONTACT_INFO, *WIMAX_API_CONTACT_INFO_P;

/// Provision operation enum
/// When the device is not activated, receiving a update started notification can be interpreted as activating the device.
typedef enum _WIMAX_API_PROV_OPERATION
{
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_STARTED,                       /**< Update has started */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_COMPLETED,	                   /**< Update has completed */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_FAILED_NETWORK_DISCONNECT,     /**< Update failed because of network disconnect*/
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_FAILED,                        /**< Update failed */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_FAILED_INVALID_PROVISIONING,   /**< Update failed because of invalid provisioning */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_FAILED_BAD_AUTHENTICATION,     /**< Update failed because of authentication failure */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_REQUEST_INITIAL_PROVISIONING,  /**< Update requested initial provisioning */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_REQUEST_ONGOING_PROVISIONING,  /**< Update ongoing provisioning */
	WIMAX_API_PROV_OPERATION_CFG_UPDATE_REQUEST_RESET_PROVISIONING,    /**< Update requested reset provisioning */
} WIMAX_API_PROV_OPERATION;

/// Package update enum
typedef enum _WIMAX_API_PACK_UPDATE
{
	WIMAX_API_PACK_UPDATE_RECEIVED,                   /**< Update package received for a un-specified component*/
	WIMAX_API_PACK_UPDATE_RECEIVED_LOWER_STACK,       /**< Update package received for firmware update only*/
	WIMAX_API_PACK_UPDATE_RECEIVED_FULL_STACK,        /**< Update package received for firmware, SDK and driver*/
	WIMAX_API_PACK_UPDATE_RECEIVED_OMA_DM_CLIENT,     /**< Update package received for OMA-DM client*/
	WIMAX_API_PACK_UPDATE_STARTED,                    /**< Update package intall started */
	WIMAX_API_PACK_UPDATE_COMPLETED,                  /**< Update package successfully installed */
	WIMAX_API_PACK_UPDATE_FAILED_NETWORK_DISCONNECTED,/**< Update package failed to install because of network disconnection*/
	WIMAX_API_PACK_UPDATE_FAILED_INVALID_PACKAGE,     /**< Update package failed to install because of invalid package*/
	WIMAX_API_PACK_UPDATE_FAILED_BAD_AUTHENTICATION,  /**< Update package failed to install because of bas authentication */
	WIMAX_API_PACK_UPDATE_FAILED,                     /**< Update package failed to install */
} WIMAX_API_PACK_UPDATE;

/// Package information structure
typedef struct _WIMAX_API_PACKAGE_INFO
{
	UINT32   structureSize;                       /**< size of this structure. */
	UCHAR16  filePath[MAX_SIZE_OF_STRING_BUFFER]; /**< file path. */ 
	UCHAR16  fileName[MAX_SIZE_OF_STRING_BUFFER]; /**< file name. */ 
	BOOL     forceReboot;                         /**< (TRUE = Reboot required, FALSE No reboot required)*/
	BOOL     mandatoryUpdate;                     /**< (TRUE = mandatory FALSE, optional) */
	BOOL     warnUser;                            /**< (TRUE = warn user, FALSE = no user warning */
} WIMAX_API_PACKAGE_INFO, *WIMAX_API_PACKAGE_INFO_P;

/// Package update state
typedef enum _WIMAX_API_PACKAGE_UPDATE_STATE
{
	WIMAX_API_PACKAGE_UPDATE_DELAY,		/**< Package update was not accepted by the user. */
	WIMAX_API_PACKAGE_UPDATE_ACCEPTED,	/**< Package update was accepted by the user. */
	WIMAX_API_PACKAGE_UPDATE_DENIED,	/**< Package update was denied by the user. */
} WIMAX_API_PACKAGE_UPDATE_STATE;

/// RF state enum
typedef enum _WIMAX_API_RF_STATE
{
	WIMAX_API_RF_ON,   /**< RF is on. */
    WIMAX_API_RF_OFF   /**< RF is off. */	
} WIMAX_API_RF_STATE;

/// Connection response enum
typedef enum _WIMAX_API_NETWORK_CONNECTION_RESP
{
	WIMAX_API_CONNECTION_SUCCESS,     /**< connection success */
	WIMAX_API_CONNECTION_FAILURE      /**< connection failure */ 
} WIMAX_API_NETWORK_CONNECTION_RESP;

/// IP Interface information structure
typedef struct _WIMAX_API_INTERFACE_INFO
{
	UINT32   structureSize;								/**< size of this structure. */
	UCHAR16  interfaceName[MAX_SIZE_OF_STRING_BUFFER];	/**< interface name. This is a string caption to be compatible for all OS. */ 
} WIMAX_API_INTERFACE_INFO, *WIMAX_API_INTERFACE_INFO_P;

/// The statuses provided by this API can generally be mapped to movements along the SDK common state machine. The indications may provide further detailed information using the API’s arguments when relevant.
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] deviceStatus - The device Status value
/// \param[in] statusReason - The device status reason value
/// \param[in] connectionProgressInfo - connection Progress value
typedef void (*IndDeviceStatusUpdate)(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_DEVICE_STATUS deviceStatus, WIMAX_API_STATUS_REASON statusReason, WIMAX_API_CONNECTION_PROGRESS_INFO connectionProgressInfo);


/// Indicator about card insertion and removal
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] cardPresence - card presence boolean value.
typedef void (*IndDeviceInsertRemove)(WIMAX_API_DEVICE_ID_P pDeviceId, BOOL cardPresence);


/// Indication of RF on/off changes.  RF power may be generated from external RF off event caused by hardware switch/control.
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] powerState - Indication that the device changed this power state.
typedef void (*IndControlPowerManagement)(WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_RF_STATE powerState);


/// Network connect indication.
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] networkConnectionResponse - Connection status value.
typedef void (*IndConnectToNetwork) (WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_NETWORK_CONNECTION_RESP networkConnectionResponse);


/// Network disconnect indication.
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] networkConnectionResponse - Connection status value.
typedef void (*IndDisconnectToNetwork) (WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_NETWORK_CONNECTION_RESP networkDisconnectResponse);


/// Provide results of Wide Scan search.
/// \param[in] pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[in] pNspList - Pointer to a structure with information on Networks found during search.
/// \param[in] listSize - Size of the Network list
typedef void (*IndNetworkSearchWideScan) (WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_NSP_INFO_P pNspList, UINT32 listSize);


/// Indicator for provisioning operation.
/// \param[in]  pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[out] pContactInfo - Provisioning information status.
typedef void (*IndProvisioningOperation) (WIMAX_API_DEVICE_ID_P pDeviceId,  WIMAX_API_PROV_OPERATION provisoningOperation);


/// Indicator for package update.
/// \param[in]  pDeviceId - Pointer to Device Identifier passed on open of device.
/// \param[out] packageUpdate - Package update status.
typedef void (*IndPackageUpdate) (WIMAX_API_DEVICE_ID_P pDeviceId, WIMAX_API_PACK_UPDATE packageUpdate);

//These APIs are added as part of Yota/Scartel CM requirement -START
//The callback is sent each time a scan cycle is completed. CmdNetworkSearch() involves this callback rising after 
//force scanning is completed.
typedef void (*IndNetworkSearch) (WIMAX_API_DEVICE_ID_P pDeviceId,WIMAX_API_NSP_INFO_P pNspList,
									UINT32	 listSize,UINT32 searchProgress);
//These APIs are added as part of Yota/Scartel CM requirement -END



#endif
