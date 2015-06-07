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
 * Description	:	Beceem API Exported Functions.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_WIMAX_EXPORTS
#define BECEEM_WIMAX_EXPORTS

/**
@defgroup API_export_functions Exported API functions
*/
/**


All Beceem APIs are command-response based type of interactions. 
The APIs are implemented in a loadable library. Applications that need to control 
the device or get information from the device send a message to the library and are 
asynchronously called back with a response. Beceem provides a simple command line
based shell application as a sample demonstrating the usage of these APIs. 
Library implementation should ensure that all operating system-specific parts 
are de-coupled from API handlers so that porting is made easy. 
Further, these APIs are also agnostic of the underlying hardware interface used. 

@image html ApiSequenceDiagram.JPG



Only four functions are exposed by the library. A function to open the device and 
allocate/initialize library resources and one corresponding close function to 
free up allocated resources. A callback function has to be registered and 
this callback function is called by the library when previously pended requests 
complete or when the baseband modem notifies the host of events. Therefore, one 
function is exposed by the library to register a callback function. Last but not 
the least; one function is exposed that sends messages/requests to the baseband 
modem or to the library/driver.

*/
/**
@defgroup network_search Network Search APIs
*/

/**
@defgroup network_entry_exit Network Entry/Exit APIs
*/
/**
 @defgroup Link_Options Link Control Options
*/
/**
 @defgroup Shutdown_Options Shutdown APIs
 Application can initiate the base band to go to shutdown mode. 
*/
/**
 @defgroup Hibernate_Options Hibernate APIs
 Application can initiate the system level hibernation on few of the embedded CPE offerings from Beceem. 
 This feature is currently supported only on 352 line of chipsets.
*/
/**
@defgroup ho Handover APIs
*/

/**
@defgroup idle_mode Idle Mode APIs
*/

/**
@defgroup authentication_status Authentication APIs
*/

/**
@defgroup service_flow Service Flow APIs

WiMAX is a connection oriented protocol. All outgoing packets have to be 
classified according to rules specified by the the entity initiating the connection. 
Further a QoS (quality of service) level has to be maintained for each of these 
connections and some of these connections require that their protocol headers be 
compressed. A reference to SF (service flow) in this document refers to these 
WiMAX connections. 

In certain implementations, these service flows are created or deleted only by the 
base station (provisioned service flows). The base station can also decide to create 
these connections in an activated state or in a de-activated state. Hence, the 
subscriber station modem might have to modify/activate these service flows before 
using them and de-activate them when they are not of immediate use. 

In other implementations a service flow can be dynamically created, deleted, or modified 
by the subscriber station. 

*/
/**
@defgroup baseband_info  Baseband Information API

*/

/**
@defgroup mac_mgmt  Mac Management Message API

*/


/**
@defgroup mac_add_set  Mac Address Programming API

*/
/**
@defgroup device_removal  Device Removal Notification

*/
/**
@defgroup fw_download  Firmware Download API

*/
/**
@defgroup gpio_api  GPIO API

*/

/**
@defgroup vendor_eeprom Vendor EEPROM API
*/

/*
@defgroup baseband_logging  Logging APIs

*/


/**
@defgroup MIB_Api  MIB API

*/

/**
@defgroup mib_standard_config Standard Configuration MIBs
These are standard configuration MIBs which are readable and writable from the host.
The APIs defined below provide interface to the 802.16 MIB.  
The current implementation of MIB interface follows the 802.16f standard. Refer to 802.16f  standard to see the 
definition of Object and to determine the corresponding IDs. Both the APIs provided work asynchronously and the 
response comes back through the general callback. 
*/

/**
@defgroup mib_proprietary_config Beceem Proprietary Configuration MIBs
These are proprietary configuration MIBs which are readable and writable from the host.
Both the APIs provided work asynchronously and the response comes back through the general callback. 
*/

/**
@defgroup mib_statistics_standard Standard Statistics MIBs
The APIs defined below provide interface to the 802.16f statistics MIBs. Refer to this standard to see the 
definition of Object and to determine the corresponding IDs. These are standard statistics MIBs which are readable from the host.
Both the APIs provided work asynchronously and the response comes back through the general callback. 
*/

/**
@defgroup mib_statistics_proprietary Beceem Proprietary Statistics MIBs
The APIs defined below provide interface to the Beceem proprietary statistics MIBs. These are proprietary statistics MIBs which are readable from the host.
Both the APIs provided work asynchronously and the response comes back through the general callback. 
*/
/**
@defgroup stats  Statistics API

*/
/*
@defgroup MAC_Logging MAC Logging APIs
MAC logging APIs are provided to log the MAC debug informations. Utility diagnostics application running on the
top of BeceemWiMax Library can parse this and log appropriate debug messages. 
*/
/*
@defgroup PHY_Logging PHY Logging APIs
PHY logging APIs are provided to log the PHY debug informations. Utility diagnostics application running on the
top of BeceemWiMax Library can parse this and log appropriate debug messages. 
*/

/**
@defgroup OMADM_Api OMA DM APIs
The APIs defined below provide interface to the Beceem OMA DM Parameters.
*/
/**
@defgroup OMADM_Calib_Api OMA DM Calibration APIs
The APIs defined below provide access to Calibrate Beceem OMA DM Parameters.
*/
/**
@defgroup SEC_Api Security APIs
The APIs defined below provide interface to the Security Parameters.
*/

/*
@defgroup DRV_Logging DRV Logging APIs
*/

/**
@defgroup Sleep_mode Sleep Mode APIs
Application can initiate the base band to go to sleep mode. 
*/
/**
@defgroup nw_layer_ctrl Network Layer Control APIs
Network layer control APIs
*/
/**
@defgroup Smart_Antenna Antenna Switching and Selection APIs
Application can initiate the slow scanning and fast switching of antennas. 
*/
/**
@defgroup Half_Mini_Card Half Mini Card APIs
Half Mini Card provides capability to switch off the WiMAX device using button, which will be
internally tied up to a GPIO. The application has to wakeup the device in each interval to check
the status of GPIO.
*/
/**
@defgroup FLASH_ACCESS_APIs  Flash Access APIs
These APIs are meant for accessing flash contents for flash map 2.x. Application should be aware of 
Beceem flash map 2.x for effectively using these APIs.
*/

/**
@defgroup RESYNC_LOG_APIs  Resync Log APIs
*/
/**
@defgroup CID_SNOOPING_LOG_APIs CID Tracking Log APIs
*/

/**
@defgroup PROXIMITY_SENSOR_APIs Proximity Sensor APIs
This API group is for controlling behavior of proximity sensor connected to baseband chip.
Presently this feature is supported on 352 chipsets only. 
*/

/**
@defgroup ex_state_machine_diag Example Connection Manager State Diagram
Diagram shown below depicts the typical state machine diagram for any connection manager using Beceem WiMAX baseband.
@image html CM_StateMachine.jpg
*/
/**
@defgroup CAPI_Support_info WiMAX CAPI specific info
Beceem provides support for WiMAX Common API 1.2 (CAPI).  This layer is built on Beceem WiMAX API layer.
Pre-requiste for using CAPI for network discovery and selections are:
- OMA-DM section should be configured with WiMAXsupp and WiMAX nodes as is mentioned in #TYPE_OMADM.
- Alternative to populating OMA-DM section is to set channel plan parameters for CAPI by calling  #SetChannelPlanParamsForNDnS
- These CAPL entries are used for network discovery which is internally carried out by CAPI layer.
- Channel plan supports tri-sectored deployments. i.e. channel plan contents are expected to get resolved as 
    Primary frequency, secondary frequency 1 and secondary frequency 2.
    Channel plan contents would have:
    - Start freqency: Start frequency channel
    - Band width: Bandwidth of the channel
    - End freqency = Start frequency + 2*BW
    
    In cases where channel plan needs to have a single frequency specify Start frequency to be same as that of end frequency.
        
-  Applications loading Beceem WiMAX library will have either Beceem WiMAX API interface  active or WiMAX CAPI interface active. Both of them
    will not be active for an application at the same time.

        Please refer to below APIs for using Beceem API when Common API interface is active: 
        - #RefreshOMAChannelPlan
        - #SetChannelPlanParamsForNDnS
        - #SendBeceemProprietaryMessage
        - #RegisterBeceemProprietaryCallback
        
*/

#include "Typedefs.h"
//#include "MACMgmtNames.h"
/*
 Beceem Symbolic name of device
*/

#ifdef WIN32
#define BCM_SYMBOLIC_NAME     "\\\\.\\BcmPcmciaCard"
#define HOST_PROXY_DLL_NAME    "hstxvi010.dll"
#else
#ifdef LINUX
#define BCM_SYMBOLIC_NAME     "/dev/tarang"
#elif MACOS
#define BCM_SYMBOLIC_NAME     "/dev/tarang0"
#endif
#endif
#ifdef WIN32
#ifdef WIMAX_COMMON_API_EXPORT
	#include "WiMaxAPI.h"
#endif
#endif
/*
 Exported function declaration
*/ 

#ifdef WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT 
	void __attribute__ ((constructor)) my_init(void);
	void __attribute__ ((destructor)) my_fini(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
@ingroup API_export_functions
The OpenWiMAXDevice function is used to open a Beceem WiMAX device and return a pointer (handle) 
to the device. This handle should be used for all subsequent interactions with the device. 

Returns: 
	- HANDLE - If the function succeeds, it returns a handle for the created interface
	- INVALID_HANDLE_VALUE - Failed to open the device handle
	- ERROR_INVALID_DEVICENAME - The WiMAX Device was not found
	- ERROR_LIB_INITIALIZATION - Library initialization failure
*/
PVOID DLL_EXPORT OpenWiMAXDevice();


/**
@ingroup API_export_functions
This is a variant of the OpenWiMAXDevice to be used if the application is to operate over a 'proxy' connection to an embedded 
implementation. In this scenario, the API library operates as a thin layer and communicates with the embedded library implementation 
over a serial or Ethernet connection. The application can communicate the remote connection details to the lib through this API. This API is used to
enable running diagnostic application on the library over serial/Ethernet connection. 

@return HANDLE - If the function succeeds, it returns a handle for the created interface

@return INVALID_HANDLE_VALUE - Failed to open the device handle 

@return ERROR_INVALID_DEVICENAME - The WiMAX device was not found

@return ERROR_LIB_INITIALIZATION - Library initialization failure
*/
#ifdef PROXY_ENABLED_BUILD
PVOID DLL_EXPORT OpenWiMAXDeviceRemote(

PSTPROXYPARAMETERS pstProxyParameters/**< Messsage buffer in the form of #STPROXYPARAMETERS*/

);
#endif 


/**
@ingroup API_export_functions
The CloseWiMAXDevice function is used to close the device and release all buffers, 
callback function references, and contexts associated with the device. 

Returns: 
	- VOID 

*/
VOID DLL_EXPORT CloseWiMAXDevice();

/**
@ingroup API_export_functions
A callback function has to be registered with the interface so that all asynchronous events and response to specific API requests can be handled.
The callback function has to be implemented by the application. It gets called by the WiMAX API library for the application that has registered callback function via RegisterCallback API call. Application can also pass a context (argument) if it wants to get any context information to be coming for every callback function call from the API layer. 
Following is the expected prototype of a callback function:

@code
    void CallbackFunction (void *pvContext, void *pvRecvBuffer, UINT32 u32Length);

@endcode

    - The pvContext parameter is whatever is passed as third parameter to the RegisterCallback routine.
    - The pvRecvBuffer parameter contains information about the event the callback addresses.
    - pvRecvBuffer is of type #PWIMAX_MESSAGE
    - Once the callback function execution is complete pvRecvBuffer pointer is not valid for access in the application any more.
    - The u32Length parameter is the number of bytes passed in the pvRecvBuffer. 
    - The buffer pointed to by pvRecvBuffer is owned by the library and must not be freed by the application.
  
@note

    - All information coming from the WiMAX modem to applications is through the registered callback function only. 
    - There can be multiple applications working with the library at the same time. 
    - All these applications get all control information via the callback function. 
    - The application should figure out if the callback is a response or unsolicited indication from baseband modem based on request placed by the application.

Returns: 

    - SUCCESS - returned if the operation succeeded 
    - ERROR_INVALID_DEVICE_HANDLE - if pvHandle is invalid
    - ERROR_LIB_INITIALIZATION - Library initialization error
    - ERROR_INVALID_NOTIFY_ROUTINE if pCallbackFnPtr is NULL


*/
UINT DLL_EXPORT RegisterCallback(PVOID pvHandle, 		/**<Handle received from  OpenWiMAXDevice*/
								PVOID pCallbackFnPtr, 	/**<Pointer to the function which will be called on asynchronous events*/
								PVOID pvCallbackContext	/**<Context passed to the callback function when called*/
								);
/**
@ingroup API_export_functions
The SendWiMAXMessage function is called by the application when it needs to request 
information from the baseband modem or when it needs to invoke a particular functionality in the modem.

Messages have types and subtypes. All messages corresponding to a particular category 
have the same type value and individual messages in this category are distinguished 
by their subtype values. All messages exchanged between applications and 
WiMAX library through the SendWiMAXMessage function and CallbackFunction have the following format:

@image html MessageFormat.JPG	 

Returns: 
	- SUCCESS - returned if the operation succeeds 
	- ERROR_LIB_INITIALIZATION - Library initialization failure
	- ERROR_INVALID_DEVICE_HANDLE - if pvHandle is invalid
	- ERROR_INVALID_BUFFER - if pu8SendBuffer is invalid
	- ERROR_INVALID_BUFFER_LEN - if u32Length is invalid
	- ERROR_FIRMWARE_DOWNLOAD_INPROGRESS - if Firmware is being downloaded currently 
	     					and no API can be serviced
    
@note All structures exchanged between the WiMAX APIs and applications are byte packed. 
No padding is allowed between fields unless explicitly added as elements in the structure itself. 
The syntax for packing is different for different compilers. 
Examples: For Microsoft compilers, byte packing is achieved by using a '\#pragma pack (1)' 
directive. For GNU compilers, packing is achieved by appending a '__attribute__ ((__packed__))' string 
to the end of the structure definition.

@note The baseband requires all messages exchanged between the driver/library 
and MAC Firmware to be in Big-endian format. However, the library ensures conversions 
from the platform-specific endian-ness to big-endian format for messages sent through it. 
The endian-ness of interaction between the driver/library APIs and applications remains 
the native platform endian-ness. For example, most Windows platforms run on x86 
compatible processors with Little-endian systems. The library on Windows interacts 
with application in little-endian format and takes care of converting all messages 
sent through it to the driver to the big-endian format. For certain APIs where the Beceem 
system is a blind conduit to the BS (base station), the application is responsible for 
maintaining endian-ness. Currently, all the API's except Security Related API handle 
endian-ness. For Security related API's the caller is responsible for handling endian 
conversion as the API acts as a pass through for the data contained in these messages. 
Unless specifically mentioned that endian conversion is not handled, all other API's that 
follow in this document can be considered to handle endian conversion correctly.

@note Every time the direction of communications is from baseband to applications, 
it is called response or indication.

Returns: 
	- SUCCESS - returned if the operation succeeds
	- ERROR_INVALID_ARGUMENT - Library initialization has failed or argument for a specific API is invalid.
	    

*/
INT DLL_EXPORT SendWiMAXMessage(PVOID pvHandle, 	/**< Handle received from  OpenWiMAXDevice*/
								PUCHAR pucSendBuffer,  	/**< Messsage buffer in the form of #WIMAX_MESSAGE*/
								UINT uiLength 			/**< Messsage length*/
									);
INT DLL_EXPORT rdm(UINT uiAddr, UCHAR *pucValue, INT uiSize);
INT DLL_EXPORT wrm(UINT uiAddr, UCHAR *pucValue, INT uiSize);

INT DLL_EXPORT eepromwrm(UINT uiAddr, UCHAR *pucValue, INT uiSize);
INT DLL_EXPORT eepromrdm(UINT uiAddr, UCHAR *pucValue, INT uiSize);



/**
@ingroup API_export_functions
	- For accessing Beceem API from the Application that uses Common API interface


	Using this API application can register for the response provided on 
	SendBeceemProprietaryMessage API call. pCallbackFnPtr is callback function pointer 
	which has to be registered with the library so that library provides an asynchronous 
	callback to the application. This should be called after WiMaxDeviceOpen 
	call is successful. This API is analogus to #RegisterCallback 
*/

UINT DLL_EXPORT RegisterBeceemProprietaryCallback( PVOID pCallbackFnPtr, PVOID pvCallbackContext);
/**
@ingroup API_export_functions
	- For accessing Beceem API from the Application that uses Common API interface

This API is analogous to #SendWiMAXMessage Beceem Proprietary API. Application that uses 
Common API interface can use these APIs to read/write MIBs or reading VSA or Reading EEPROM contents.
It is not recommended to use proprietary link related messages using these APIs which can alter the modem state.
Usage of the above two APIs is similar to that of the #SendWiMAXMessage and #RegisterCallback APIs explained in this document.
For working example on how to use these APIs it is available in the WiMAXCommonAPITestApp source file.
These APIs can be used to get request/response API messages and not for notifications.
Below are the sections of API that may be required to be used by system integrator while 
using the Common API interface for device management.
-	OMA DM APIs
-	Security APIs
-	MIB API
-	Idle Mode APIs
-	GPIO API
-	Vendor EEPROM API

	
*/
INT DLL_EXPORT SendBeceemProprietaryMessage(PUCHAR pucSendBuffer, UINT uiLength);

/**
@ingroup API_export_functions
	- For Application that uses Common API interface

This API is for applying the latest channel plan for the network search next cycle in the Common API interface.
This API is intended to be used by the agent that updates the channel plan on the device using OMA-DM APIs. This is a blocking call
and would read the channel plan information from the device and applies it for the next search.

Returns: 
	- SUCCESS - Channel plan is refreshed with latest values
	- ERROR_HANDLE_INVALID - Device handle is invalid
	- ERROR_API_NOT_SUPPORTED - Common API interface is not active
	- ERROR_MALLOC_FAILED - If memory allocation in the function fails
 
*/
INT  DLL_EXPORT RefreshOMAChannelPlan();

/**
@ingroup API_export_functions
Application using WiMAX CAPI 1.2 can use this API to set channel plan parameters for 
Network discovery.
This API contents override OMA-DM section contents.
This API should be called before device open to set Channel parameters for search.

*/
INT DLL_EXPORT SetChannelPlanParamsForNDnS(PVOID pST_OperatorChannelPlanSettings, UINT32 u32SizeOfChannelPlanSettings);
#ifdef __cplusplus
}
#endif


#endif /* BECEEM_WIMAX_EXPORTS */
