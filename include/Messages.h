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
 * Description	:	This includes declaration of all the messages flowing
 *					between the library and the application.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BECEEM_MESSAGES_H
#define BECEEM_MESSAGES_H

/**
@ingroup network_search
Network search command
@image html Network_Search.jpg
*/
#define TYPE_NETWORK_SEARCH							1

/**
@ingroup network_search

This API triggers a search of the network environment and 
enumerates all base stations in the vicinity. The request to trigger this search is sent as follows.
Payload: #_ST_NETWORK_SEARCH_PARAMS

No concurrent search requests are allowed if the application wishes to issue another 
network search request, it should first abort the current network search before issuing 
a fresh request. 

The API supports only one continuous frequency range at a time. 
If multiple frequency ranges are to be specified the application must issue multiple 
network search requests. And the results for each network search have to be maintained by 
the application. The baseband will only maintain the results of the latest (last) 
network search request.

The search request takes in a list of bandwidths to search over as an input parameter. 
If this parameter is not specified then the default bandwidth will be chosen for the 
search.

The network search request cannot be issued once the sync-up request has been issued 
and the SS has successfully synced. In this case, a sync-down request has to be sent 
explicitly to the baseband before issuing a network search request. 

Returns: 
	- ERROR_SS_IS_INSYNC : If baseband has achieved sync.
	- ERROR_NET_SEARCH_INPROGRESS : If another network search request is running.
	- ERROR_THREAD_NOT_STARTED : If library failed to start network search thread.

@image html NS_Sync_up_Req.jpg

@image html NS_Use_Case_Scenarios1.jpg

@image html NS_Use_Case_Scenarios2.jpg

@image html NS_BSID_Decoding.jpg

@image html NS_BSID_Complete_Decoding.jpg

*/

#define SUBTYPE_NETWORK_SEARCH_REQ					1 /* App -> DLL */

/**
@ingroup network_search

Once the baseband has enumerated the network environment, the application callback 
function is called. This notification is issued to all the applications that 
have registered a callback with the library. 

The payload is a BS list structure in the form #_BSLIST

If Per Antenna Reporting Mode is enabled, The payload is a BS list structure per
antenna in the form #_ST_BSLIST_PER_ANTENNA. For More Information on selecting 
Per Antenna Reporting, Please refer SUBTYPE_SET_WIMAX_OPTIONS_REQ of 
TYPE_SET_WIMAX_OPTIONS.

The network search response can be BS by BS or a list (depending on the report 
type specified in the request). In a BS by BS response, the u32NumBaseStationsFound will 
always be one. Only one BS is reported at a time. In a list type response, it will be an array of base stations with u32NumBaseStationsFound specifying the total number of base 
stations actually detected.

The u32SearchCompleteFlag indicates that the search has been completed. 

In case of a network search failure, the network search response will be sent 
with u32NumBaseStationsFound set to 0, and u32SearchCompleteFlag set to 1, 
indicating that the search has been completed and no base stations have been found.
Invoking the network search request gets the baseband out of Idle mode if it had entered into it.

*/

#define SUBTYPE_NETWORK_SEARCH_RESP					1 /* DLL -> App */
/**
@ingroup network_search
The network search can be aborted using the Network Search Abort message. 
The target returns all the base stations detected till that point (if any) 
in case the reporting type requested was to provide consolidated list in #SUBTYPE_NETWORK_SEARCH_REQ.
Refer to #_ST_NETWORK_SEARCH_PARAMS::u32ReportType.
*/
#define SUBTYPE_NETWORK_SEARCH_ABORT_REQ			2 /* APP -> DLL */

/**
@ingroup network_search
This API is used to query on the network search results. 
It can be used any time by the application to retrieve the results of the last 
network search performed. 
*/
#define SUBTYPE_NETWORK_SEARCH_RESULTS_QUERY_REQ	3 /* APP -> DLL */
/**
@ingroup network_search
This API is used to do NSP(Network service provider) search after 
base station search is done and NAP (Network access provider) ID is available. Once the search is complete
modem responds back with #_BSNSPLIST.

Payload : #_ST_CONFIG_INFO_PER_NAPID.

Returns: 
	- ERROR_SS_IS_INSYNC : If baseband has achieved sync.
	- ERROR_NET_SEARCH_INPROGRESS : If another network search request is running.

@image html NSPDiscovery.JPG
*/
#define SUBTYPE_NETWORK_DISCOVERY_REQ 4
/**
@ingroup network_search
This API callback is in response to the network #SUBTYPE_NETWORK_DISCOVERY_REQ.

Payload: #_BSNSPLIST
*/
#define SUBTYPE_NETWORK_DISCOVERY_RESP 4

/**
@ingroup network_search
This API is used to do search based on parameters specification relevant to CAPL.
Refer to the payload specification for details on search parameters and criterion.

Payload: #_ST_CAPL_BASED_SEARCH_PARAMS
*/
#define SUBTYPE_CAPL_BASED_SEARCH_REQ	5            

/**
@ingroup network_search
This API callback is in response to the network search request of #SUBTYPE_CAPL_BASED_SEARCH_REQ.

Payload: #_ST_CAPL_SEARCH_RSP
*/
#define SUBTYPE_CAPL_BASED_SEARCH_RSP	5  

/**
@ingroup network_search
This API is used to set modem to low power scan mode. This is intended to be used for dual mode
devices which have low power scanning requirements. This setting should be applied before
issuing the search command.


Payload: #_ST_SCAN_CONFIG_PARAM
*/
#define SUBTYPE_SET_LOWPOWER_SCANMODE_REQ		    6
/**
@ingroup network_search
This API is callback for #SUBTYPE_SET_LOWPOWER_SCANMODE_REQ . 

SUCCESS/FAILURE is indicated by #_ST_SCAN_CONFIG_PARAM_STATUS::bStatus. 

Payload: #_ST_SCAN_CONFIG_PARAM_STATUS
*/
#define SUBTYPE_SET_LOWPOWER_SCANMODE_RESP		 	6

/**
@ingroup network_search

Primary purpose of this API is to download the search parameters to baseband modem so that 
it can use these parameters for autonomous search.
When this API is called with valid parameters:
-  Baseband modem initiates a network search for the frequencies mentioned once and search results would be made available to application via #SUBTYPE_CAPL_BASED_SEARCH_RSP indication. 
-  Search parameter payload would be used for future search done by baseband modem initiated by #SUBTYPE_SYNC_DOWN_AUTONOMOUS_SCAN_REQ

Refer to the payload for details on search parameters.

Payload: #_ST_CAPL_BASED_SEARCH_PARAMS
*/

#define SUBTYPE_AUTONOMOUS_SEARCH_REQ				7


/**
@ingroup network_entry_exit
The APIs in this section handle management of WiMAX link between the baseband 
SS (subscriber station) modem and a BS (base station).
@image html Network_Entry.jpg
*/
#define TYPE_NETWORK_ENTRY							2
/*********************************************************************/

/**
@ingroup network_entry_exit
To trigger a DL (down link) and UL (up link) sync of the baseband with the base station, 
this command has to be sent to the WiMAX library.

Payload is #_ST_SYNCUP_REQ_PARAMS for sync up request. It has centre frequency, bandwidth, preamble ID,
and BSID of the BS found during network search. 

    - Specifying ‘0’ for the bandwidth parameter will result in the target selecting the default bandwidth. 
        Application should specify the valid bandwidth value.
    - If Preamble ID and BSID or Preamble ID alone is specified, PHY tries to sync to the same. 
    - Not specifying Preamble ID (specifying 0xFF for the same) results in the target picking up the best BS. 
    - The library handles a timeout for this and sends a Sync-Status notification with link status value of PHY_SYNC_ERROR 
        if the modem does not respond.
    -  Invoking the Sync up request causes the baseband to be woken up if it was in low power shutdown mode.

Returns: 
	- ERROR_SYNC_UP_INPROGRESS : If baseband sync-up is in progress
	- ERROR_THREAD_NOT_STARTED : If library fails to start sync-up thread
	- ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS : If MAC address write is initiated and not complete

*/
#define SUBTYPE_SYNC_UP_REQ							1 /* App -> DLL */

/**
@ingroup network_entry_exit
Once the baseband modem achieves DL and UL sync with the base station, 
the library calls the application’s callback function with #_ST_SYNC_STATUS

In case of a successful sync-up, this notification is issued to all the applications 
that have registered a callback with the library. If sync-up fails, the above 
sync-up response is sent with PHY_SYNC_ERROR status. Other fields are not valid 
in this case.
*/
#define SUBTYPE_SYNC_STATUS_RESP					1 /* DLL -> App */

/**
@ingroup network_entry_exit
After sync-up, the baseband has to go through the network entry process 
(also referred to as the link-up process). When the application wants to initiate
network entry, this command should be sent. 
The Vendor-Specific Parameter Structure #_VENDOR_SPECIFIC_LINKPARAMS

Returns : 
	- ERROR_NET_ENTRY_INPROGRESS : If baseband Network entry is in progress
	- ERROR_THREAD_NOT_STARTED : If library fails to start Network entry thread
	- ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS : If MAC address write is initiated and not complete

*/
#define SUBTYPE_NETWORK_ENTRY_REQ					2 /* App -> DLL */

/**
@ingroup network_entry_exit
At any point of time, if an application wishes to know the current sync/link 
status, it sends this message.

As a response to this request, the library calls back the application with a 
Link Status Notification. The link status is as indicated by the field eStatusCode 
in #_LINK_STATUS structure explained subsequently.

Invoking the Link Status request gets the baseband out of Idle mode 
if it has entered into it.

Returns : 
	- ERROR_NET_ENTRY_INPROGRESS : If baseband is running network entry request.
	- ERROR_THREAD_NOT_STARTED : If library fails to start network entry thread.
*/
#define SUBTYPE_NETWORK_ENTRY_STATUS_REQ			3 /* App -> DLL */
/**
@ingroup network_entry_exit
When the baseband modem achieves a MAC level link-up with base station 
(entire network entry is successfully completed), the library calls the 
callback function with the this message. This notification is issued to all 
the applications that have registered a callback with the library.

The Link Status notification API gets called asynchronously when there is 
a state change in the baseband with the payload #_LINK_STATUS.
*/
#define SUBTYPE_NETWORK_ENTRY_STATUS_RESP			2 /* DLL -> App */

/**
@ingroup network_entry_exit
After PHY-level sync-up, the baseband has to go through the network entry process 
(also referred to as the link-up process). When the application wants to initiate
network entry based on NSPID (network service provider identifier), this command should be sent. 
The NSP based network entry request structure #_NSP_BASED_NETENTRY_ST

Returns : 
	- ERROR_NET_ENTRY_INPROGRESS : If baseband Network entry is in progress
	- ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS : If MAC address write is initiated and not complete

*/
#define SUBTYPE_NSP_NETWORK_ENTRY_REQ    4
/**
@ingroup network_entry_exit
Device status is indicated to the application whenever there is a status change in the baseband modem.
Payload load for this notification is #_ST_MODEM_STATE_INFO. 


*/
#define SUBTYPE_DEVICE_STATUS_INDICATION  5

/**
@ingroup network_entry_exit
Provides time duration elapsed since network entry in seconds. API would fail if baseband modem 
is not in network entry stage. This is blocking call and no callback is associated with this.
Payload: double
Returns: 
 - ERROR_DEVICE_NOT_IN_LINKUP: If device is not in linkup state.
 
 
*/


#define SUBTYPE_TIME_ELAPSED_SINCE_NETWORK_ENTRY     6


/**
@ingroup network_entry_exit
If an application wants to tear down connection between the Beceem baseband 
subscriber station modem and base station, the following APIs have to be used.
Invoking the Link Status request gets the baseband out of Idle mode 
if it has entered into it.
*/


#define TYPE_LINK_DOWN								3
/*********************************************************************/
/**
@ingroup network_entry_exit
To achieve a PHY level sync-down, an application will send this message to library.
When the MAC level network deregistration is desired – the application sends this message
when the baseband completes a network deregistration, the library calls the callback 
When the baseband achieves a sync-down, the library/driver calls back the application’s 
callback function with a #_LINK_STATUS structure.
*/
#define SUBTYPE_SYNC_DOWN_REQ						1 /* App -> DLL */
/**
@ingroup network_entry_exit
When the MAC level network deregistration is desired – the application sends this message
When the baseband completes a network deregistration, the library calls the callback 
function with a link status structure  #_LINK_STATUS with appropriate values. Baseband waits for a new sync up request after this.  

Invoking the Link Status request gets the baseband out of Idle mode 
if it has entered into it.
*/
#define SUBTYPE_SYNC_DEREG_REQ						2 /* App -> DLL */

/**
@ingroup network_entry_exit
This API is called to achieve PHY level sync-down with the base station.
When this API is called baseband modem will not initiate complete network de-registration.
When the baseband achieves a sync-down, the library/driver calls back the application’s 
callback function with a #_LINK_STATUS structure.
*/
#define SUBTYPE_SYNC_DOWN_NO_DREG_REQ						3 /* App -> DLL */

/**
@ingroup network_entry_exit
This API is called to put baseband modem in low power scan mode.
Baseband modem would go to low power scan mode and starts periodic search for parameters downloaded using
#SUBTYPE_AUTONOMOUS_SEARCH_REQ
*/
#define SUBTYPE_SYNC_DOWN_AUTONOMOUS_SCAN_REQ               4

/**
@ingroup authentication_status 
Authentication related APIs
*/
#define TYPE_AUTHENTICATION							4
/*********************************************************************/
/*
- This API is no longer supported. It will be removed from subsequent releases.
@ingroup authentication_status 
When an application intends to know the baseband modem's current authentication status, 
it sends this message.

As a response or as an unsolicited indication when the authentication status changes, 
the library calls the application's callback function with an authentication status 
indication Authentication Status Indication. 
*/
#define SUBTYPE_AUTH_STATUS_REQ						1 /* App -> DLL */

/*
- This API is no longer supported. It will be removed from subsequent releases.
@ingroup authentication_status 
Either as a response to an authentication status request or when the baseband 
authentication status changes, the application's callback function will be 
called with this message.

	Payload : UINT8 (Authenticated : 0 and Not Authenticated : 1)
*/
#define SUBTYPE_AUTH_STATUS_RESP					2 /* DLL -> App */

/*
- This API is no longer supported. It will be removed from subsequent releases.
@ingroup authentication_status 
When an application wants to read authentication parameters from the baseband, it 
sends this message: 

EAP Method Type:
0 = EAP AKA
1 = EAP SIM 
Right now only EAP AKA is supported.
As a response to this request, the application’s callback function is called with 
an authentication parameters indication (Authentication Parameters Indication). 
*/
#define SUBTYPE_AUTH_READ_PARAM_REQ					3 /* App -> DLL */

/*
- This API is no longer supported. It will be removed from subsequent releases.
@ingroup authentication_status 
As a response to read authentication parameters request or as a confirmation of a 
write authentication parameters request, the application’s callback function is called 
with this message.

Payload: #_AUTH_PARAMETER_INDICATION

*/
#define SUBTYPE_AUTH_PARAM_INDICATION				5 /* DLL -> App */

/*
- This API is no longer supported. It will be removed from subsequent releases.
@ingroup authentication_status 
If an application wants to write authentication parameters to the baseband, 
it sends this message with #_WRITE_AUTH_PARAMS structure

As an indication of success, the application is called with an authentication parameters 
indication (Authentication Parameters Indication). 
If a new set of authentication parameters were successfully written, it reflects in 
the indication, else it has the old set of parameters.
*/
#define SUBTYPE_AUTH_WRITE_PARAM_REQ				4 /* App -> DLL */

/**
@ingroup authentication_status 
When EAP packet is received by MS from BS ( AAA server sends this packet to BS), the application’s 
callback function is called with this message.

Payload: #_EAP_PAYLOAD

@note EAP Method Type is not required here as the EAP packet has that information.
*/
#define SUBTYPE_EAP_TRANSFER_INDICATION				6 /* DLL -> App */

/**
@ingroup authentication_status 
When an application/EAP supplicant wishes to send EAP packet to the AAA server, it 
is called with this message

Payload: #_EAP_PAYLOAD

@note EAP Method Type is not included as the EAP packet has that information.
*/
#define SUBTYPE_EAP_TRANSFER_REQ					7 /* App -> DLL */
/**
@ingroup authentication_status 
When a supplicant completes EAP authentication successfully, it sends an EAP 
complete request to the baseband so that it can use the session key. If it fails 
authentication and wishes to retry, it sends an EAP complete request to the 
baseband with a failure status (non-zero value) and the baseband automatically 
re-starts the security process. 

Payload: #_EAP_COMPLETE_INFO

@image html NetworkEntrySequencDiagram.jpg

*/
#define SUBTYPE_EAP_COMPLETE_REQ					8 /* App -> DLL */
/**
@ingroup authentication_status 
This API is used to encrypt a message digest using the private key stored on 
the device using the RSA algorithm. The private key is first decrypted using 
the supplied AES key, then the data is encrypted using the private key.  

Payload: #_ST_EAP_RSA_PRIVATE

Returns:
	- SUCCESS, If Payload is valid.

*/

/** 
This is Vendor specfic API, details are out of scope of this document. 
*/

#define 	SUBTYPE_EAP_ENCRYPT_PRIVATE_REQUEST		9    /* App -> DLL */

/**
@ingroup authentication_status 
The baseband responds back to the EAP Encrypt Private Request, the DLL calls 
the application’s registered callback function. 

Payload: #_ST_EAP_RSA_PRIVATE

*/

/** 
This is Vendor specfic API , details are out of scope of this document. 
*/

#define 	SUBTYPE_EAP_ENCRYPT_PRIVATE_RESPONSE    10   /* DLL -> App */


/**
@ingroup authentication_status 
When an application/EAP supplicant wishes to send EAP packet to the AAA server and baseband modem 
needs to spoof for NAI in the payload this API is called. 

Payload: #_EAP_PAYLOAD

*/
/** 
This is Vendor specfic API , details are out of scope of this document. 
*/

#define 	 SUBTYPE_EAP_SPOOF_INFO_REQ 		11
	
/**
@ingroup service_flow
Service Flow Message type
*/
#define TYPE_SERVICE_FLOW							6
/*********************************************************************/

/**
@ingroup service_flow
Irrespective of whether the base station allows only provisioned service 
flows or supports dynamic service flows, every time a new service flow is 
created, an SF Add Indication is sent to the application’s callback function.
This notification is issued to all the applications that have registered a 
callback with the library. Service flow addition SUCCESS/FAILURE should be 
interpreted based on #_stLocalSFAddIndication::u8CC as follows:
 - Zero indicates SUCCESS
 - Non-Zero indicates FAILURE


Payload: #_stLocalSFAddIndication

@image html BSInitiateDSA.jpg

*/
#define SUBTYPE_SF_ADD_INDICATION					1 /* DLL -> App */

/**
@ingroup service_flow
For both base station initiated and subscriber station initiated service flows, 
every time a service flow is deleted, an SF Delete Indication is sent to the 
application’s callback function. This notification is issued to all the applications 
that have registered a callback with the library. 

Payload: #stLocalSFDeleteIndication

@image html BSInitiatedDSD.jpg

*/
#define SUBTYPE_SF_DELETE_INDICATION				2 /* DLL -> App */

/**
@ingroup service_flow
Every time a service flow is modified, an SF Change Indication is sent to the 
application’s callback function. This notification is issued to all the applications that have registered a 
callback with the library. Service flow change SUCCESS/FAILURE should
be interpreted based on #_stLocalSFAddIndication::u8CC as follows:
 - Zero indicates SUCCESS
 - Non-Zero indicates FAILURE.
  

Payload:#_stLocalSFAddIndication 

@image html BSInitiatedDSC.jpg


*/
#define SUBTYPE_SF_CHANGE_INDICATION				3 /* DLL -> App */

/**
@ingroup service_flow
If a base station supports subscriber station initiated service flows, every time 
an application wishes to create a service flow, an SF Add Request is sent.


Application should fill  valid values with in the range (i.e. application should fill default 
values mentioned in the document as well).

Payload: #_stLocalSFAddRequest 

On success, the application’s callback is called with SF add indication 

@image html SSInitiatedDSA.jpg


*/
#define SUBTYPE_SF_ADD_REQ							1 /* App -> DLL */

/**
@ingroup service_flow
If a base station supports subscriber station initiated service flows, every time 
an application wishes to delete a service flow, an SF Delete Request is sent. 

Application should fill the value of SFID corresponding to the connection to be deleted.

Payload: #_stLocalSFDeleteRequest.

On success, the application’s callback is called with SF delete indication 
(SF Delete Indication). 

@image html SSInitiatedDSD.jpg
*/
#define SUBTYPE_SF_DELETE_REQ						2 /* App -> DLL */
/**
@ingroup service_flow
Every time an application wishes to change/activate a service flow, 
an SF Change Request is sent , having payload #_stLocalSFAddIndication.
As a response to this call, the application’s callback function is called with the SF 
change indication.


Application should fill relavant values of fields that needs to be modified. For other 
fields retain the values originally sent in last add or change indication.


@Note:
In case of provisioned service flows, when the base station creates these 
service flows, some of them may not be activated. So, the sfActiveSet member of 
this structure has its bValid field set to Zero. It only has valid sfAdmittedSet. 
In such a case, when the service flow needs to be used, the application 
copies the sfAdmittedSet to sfActiveSet – sets the bValid field to 1, updates the 
Transaction ID element in the sfActiveSet and sends an SF Change Request 
with this structure. If the base station successfully activates the connection, 
the library calls (with a value of 1 in the bValid field of sfActiveSet member) the 
application with SF change indication (refer to 5.1.4.1.3, SF Change Indication). 


@image html SSInitiatedDSC.jpg

*/
#define SUBTYPE_SF_CHANGE_REQ						3 /* App -> DLL */

/**
@ingroup service_flow
If a base station supports subscriber station initiated service flows, any time 
an application wishes to change the eRTPS grant size on a particular service flow, 
an SF eRTPS Change Request is sent. The FW shall issue this change request towards
base station and shall apply the eRTPS change based on the acceptance from the base
station.

Application should fill valid values with in the range (i.e. application should fill default 
values mentioned in the document as well).

Payload: #_ERTPS_GRANT_SIZE_SET_REQ.

*/
#define SUBTYPE_SF_ERTPS_CHANGE_REQ					4 /* App -> DLL */
/**
@ingroup baseband_info
WiMAX networks can use an IP CS (convergence sublayer) or an Ethernet CS. 
When an IP CS is used, a MAC address has to be exchanged between the base station 
and the subscriber station. This MAC address (among other things like RF 
calibration tables, PHY/MAC parameters, authentication information, and so on) 
is usually saved into an external non-volatile storage (typically EEPROM) and read 
every time the system is powered up. The following APIs read among other things, 
MAC address from a non-volatile storage and various version numbers.
*/
#define TYPE_SS_INFO								7
/*********************************************************************/
/**
@ingroup baseband_info
When an application wishes to know the currently active MAC address, baseband version 
number, driver version number, the chipset version, etc., it sends a subscriber station 
information request.
The application is then called back with a subscriber station information 
indication (Subscriber Station Information Indication).
*/
#define SUBTYPE_SS_INFO_REQ							1 /* App -> DLL */

/**
@ingroup baseband_info
Either as a response to a MAC address read request or as a response to MAC address 
set request, the application is called back with the MAC address indication.
The notification message has payload of the format #_SUBSCRIBER_STATION_INFO
*/

#define SUBTYPE_SS_INFO_INDICATION					1 /* DLL -> App */

/**
@ingroup baseband_info
When an application wishes to know the EEPROM Diagnostic Information, EEPROM Size
Compatibility, RF, HW, and MAC Sections Valid Status, it sends a subscriber station 
information request.
The application is then called back with a subscriber station information 
Response (Subscriber Station Information Response).
*/
#define SUBTYPE_SS_EEPROM_INFO_REQ					2 /* App -> DLL */

/**
@ingroup baseband_info
Either as a response SS EEPROM info request, the application is called back with 
the SS EEPROM response. The response message has payload of the 
format #UINT32.

Each bit represents the validity of the sections.
Bit is set to 1, if CheckSum is Passed.

	- 0 - Always set to 1 Unused
	- 1 - Compatibility
	- 2 - RF
	- 3 - PHY
	- 4 - MAC
	- 5 - CALIBRATION
	- 6 - Hardware
	- 7 - MIB
	- 8 - Security
	- 9 - OMA-DM
	- 10 - MRU
	- 11 - 23 is unused
	- 24 - 31 - EEPROM Size
*/

#define SUBTYPE_SS_EEPROM_INFO_RESP					2 /* DLL -> App */
/**
@ingroup baseband_info
The API defined can be used to get the versions of various EEPROM sections: RF, PHY, MAC, CAL, HW, 
MIB, SECURITY, OMADM, MRU, VSA sections.
*/


#define SUBTYPE_SS_EEPROM_DETAILED_INFO_REQ			3
/**
@ingroup baseband_info

The application is called back with subscriber station detailed EEPROM information as a
response for #SUBTYPE_SS_EEPROM_DETAILED_INFO_REQ if the EEPROM sections are valid.

Payload: #_ST_SS_EEPROM_DETAILED_INFO.
*/

#define SUBTYPE_SS_EEPROM_DETAILED_INFO_RESP		3

/*
@ingroup baseband_logging
This API is provided to gather logging information from the baseband for 
debugging in the field. For each type of log that the application wants to gather, 
a separate request has to be made with the required log type as the subtype parameter.

After every second the logging callback is called separately for each 
logging subtype the user has registered for, if that log has changed. 
Only the changed log entries since the last callback are reported to the user. 
Payload is an array of structures specified in the parameter structure table. 
This notification is issued only to the application that has issued a start logging 
request. 

Log structures can be one of the following:
- Initial Ranging requests #_RANGE_CH_TRACK
- BW Ranging requests	#_RANGE_CH_TRACK
- Periodic Ranging requests #_RANGE_CH_TRACK
- PDU or BW request #_BW_LOG 
- Closed Loop Power Control request #_POWER_ADJ_TRACK
- Open Loop Power Control request #_POWER_ADJ_TRACK
- CIDs Log requests #_CID_LOG

Returns: 
	- ERROR_THREAD_NOT_STARTED : If library could not start logger thread.
*/
#define TYPE_START_LOG								8

/*
@ingroup baseband_logging
This API is provided to stop logging information. For each type of log that 
the application wants to stop, a separate request has to be made with the 
required log type as the subtype parameter.
*/
#define TYPE_STOP_LOG								9
/*********************************************************************/

/*
@ingroup baseband_logging
	Request to enable the IR Logging
*/
#define SUBTYPE_IR_LOG_REQ							1 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_RANGE_CH_TRACK
*/
#define SUBTYPE_IR_LOG_RESP							1 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the BR Logging
*/
#define SUBTYPE_BR_LOG_REQ							2 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_RANGE_CH_TRACK
*/
#define SUBTYPE_BR_LOG_RESP							2 /* DLL -> App */
/*
@ingroup baseband_logging
		Request to enable the PR Logging
*/
#define SUBTYPE_PR_LOG_REQ							3 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_RANGE_CH_TRACK	
*/
#define SUBTYPE_PR_LOG_RESP							3 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the BWL Logging
*/
#define SUBTYPE_BWL_LOG_REQ							4 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_BW_LOG
*/
#define SUBTYPE_BWL_LOG_RESP						4 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the CL Logging
*/
#define SUBTYPE_CL_LOG_REQ							5 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_POWER_ADJ_TRACK
*/
#define SUBTYPE_CL_LOG_RESP							5 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the OL Logging
*/
#define SUBTYPE_OL_LOG_REQ							6 /* App -> DLL */
/**
@ingroup baseband_logging
	Payload : #_POWER_ADJ_TRACK
*/
#define SUBTYPE_OL_LOG_RESP							6 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the CID Logging
*/
#define SUBTYPE_CID_LOG_REQ							7 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : #_CID_LOG
*/
#define SUBTYPE_CID_LOG_RESP						7 /* DLL -> App */
/*
@ingroup baseband_logging
	Request to enable the CQICH Logging
*/
#define SUBTYPE_CQICH_LOG_REQ						8 /* App -> DLL */
/*
@ingroup baseband_logging
	Payload : UINT32 array

*/
#define SUBTYPE_CQICH_LOG_RESP						8 /* DLL -> App */



/**
@ingroup vendor_eeprom
Vendor-Specific Information  non-volatile storage
Baseband chip comes with a non-volatile storage device(Flash/EEPROM). 
The following APIs provide flexibility to store vendor-specific parameters on the non volatile storage. 

This VSA (Vendor specific Area) is pertinent to DSD (Device specific data) region of the device.

Vendor NVM(non volatile memory) space is divided into two Regions, Read-Only and Read-Write. 
Read-Only parameters could be like factory setting, which do not change over 
the entire life time of the baseband chip, while Read-Write can be updated any 
number of times the NVM hardware supports. Read-Only region is protected by a 
checksum and a CRC to handle NVM corruption, and incase of an NVM corruption 
default values will be made available (which is zeroes). Read-Write region is not 
protected by checksum or CRC, it is totally under the control of the vendor application. 
The Read-Only region is programmed using a ReaderWriterUtility only (which is 
executed during production time), while the Read-Write region can be written by the 
host at anytime.

Returns: 
	- ERROR_EEPROM_NOTINITIALIZED : If NVM is not initialized.
*/
#define TYPE_VENDOR_SPECIFIC_INFO					10
/*********************************************************************/

/**
@ingroup vendor_eeprom
Vendor-specific read-only information stored on the NVM can be read through this 
API request; the application is called back with the vendor-specific information. 
User should consider u8VSAData as unused for this API.

Payload: #_VENDOR_SPECIFIC_PARAM
*/
#define SUBTYPE_VSA_INFO_READ_REQUEST				1 /* App -> DLL */

/**
@ingroup vendor_eeprom
Vendor-Specific RO read information request’s response is given back through the 
register callback. The Information is available by reading the payload. The payload 
comes with the same offset, size sent through the request, but u8VSAData contains 
the parameter value. If the size is zero (0), it implies that the request failed. 

Payload: #_VENDOR_SPECIFIC_PARAM

This notification is issued only to the requesting application.
*/
#define SUBTYPE_VSA_INFO_READ_INDICATION			1 /* DLL -> App */
#if 0
/*
@ingroup vendor_eeprom
Vendor-Specific read-write information stored on the EEPROM can be read through this 
API request; the application is called back with the vendor-specific information. 
User should consider u8VSAData as unused for this API.

Payload: #_VENDOR_SPECIFIC_PARAM
*/
#define SUBTYPE_VSA_INFO_READ_REQUEST				2 /* App -> DLL */

/*
@ingroup vendor_eeprom
Vendor-specific RW read information request’s response is given back through the 
register callback. The information is available by reading the payload. The payload 
comes with the same offset, size sent through the request, but u8VSROData contains the 
parameter value. If the size is zero (0), it implies that the request failed. 

Payload: #_VENDOR_SPECIFIC_PARAM

This notification is issued only to the requesting application.
*/
#define SUBTYPE_VSA_INFO_READ_INDICATION			2 /* DLL -> App */

#endif

/**
@ingroup vendor_eeprom
Vendor-specific read-write information stored on the NVM can be written through this 
API request; the application is called back with the vendor-specific information. User 
should consider u8VSAData as unused for this API. 

Payload: #_VENDOR_SPECIFIC_PARAM
*/
#define SUBTYPE_VSA_INFO_WRITE_REQUEST				3 /* App -> DLL */
/**
@ingroup vendor_eeprom
Vendor-specific RW write information request’s response is given back through the 
register callback. The Information is available by reading the payload. The payload comes 
with the same offset, size send through the request, but u8VSAData contains the 
parameter value. If the size is zero (0), it implies that the request failed. 

Payload: #_VENDOR_SPECIFIC_PARAM

This notification is issued only to the requesting application.
*/
#define SUBTYPE_VSA_INFO_WRITE_INDICATION			3 /* DLL -> App */

/**
@ingroup vendor_eeprom
Vendor-specific read-size information of the NVM size can be read through this 
API request; the application is called back with the vendor-specific information.
No payload is required for this message. Only TYPE/SUBTYPE is sufficient.
*/
#define SUBTYPE_VSA_INFO_REGION_SIZE_REQUEST		4 /* App -> DLL */
/**
@ingroup vendor_eeprom
Vendor-specific read-size information request’s response is given back through the 
register callback. The information is available by reading the payload. The payload 
comes with the size, u32VSRegionSize contains the parameter value. 
If the size is zero (0), it implies that the request failed. 

Payload for this message is #UINT32 Parameter

This notification is issued only to the requesting application.
*/
#define SUBTYPE_VSA_INFO_REGION_SIZE_INDICATION	4 /* DLL -> App */


/**
@ingroup mac_add_set
This API allows the programming of the MAC Address of the baseband. The MAC address programmed via this API is session dependent and will not be retained between reboots or device removal and connecting again.

feature support info:
        - This feature is not supported any more.

*/
#define TYPE_MACADDRESS								11
/*********************************************************************/

/**
@ingroup mac_add_set
When an application wants to program the MAC Address for the baseband, it sends 
this message.

As a response to a Set MAC Address request, the calling application receives a Set MAC 
Address Response as explained below. Until a response arrives for the Set MAC Address 
request, the application must issue a warning to the user to not remove the card.
Payload is UCHAR[6].

Returns :
	- ERROR_AUTH_CRC_MISMATCH : If CRC check for EEPROM has failed
	- ERROR_WRITE_MAC_ADDRESS_FAILED : If writing MAC address has failed
	- ERROR_EEPROM_NOTINITIALIZED : If EEPROM is not initialized
	- ERROR_MAC_ADDRESS_WRITE_IN_PROGRESS : If MAC address write is initiated and not complete
	- ERROR_THREAD_NOT_STARTED : If library failed to start MAC address write thread
*/
#define SUBTYPE_MACADDRESS_SET_REQ					1 /* App -> DLL */

/**
@ingroup mac_add_set
As a response to the Set MAC Address, the application is called back with a response 
indicating the status of the operation. 

Payload : UINT32 Status.

Where Status :
	- 0 : Indicates Success
	- Non-zero : Indicates failure with error code

Error codes possible: 
	- ERROR_INVALID_ARGUMENT 
	- ERROR_EEPROM_NOTINITIALIZED

This notification is issued only to the application that has requested a Set MAC Address.
*/
#define SUBTYPE_MACADDRESS_SET_RESP					1 /* DLL -> App */

/**
@ingroup device_removal
When the device has been removed, this notification is sent to the 
application.

feature support info:
        -  This is supported for devices with USB host interface only.
*/
#define DEVICE_RESET_UNLOAD_TYPE					12
/*********************************************************************/
/**
@ingroup device_removal
This notification is sent to the application when the device has been unplugged from 
the host. 

feature support info:
        -  This is supported for devices with USB host interface only.

*/
#define DEVICE_UNLOAD								2 /* DLL -> App */
/**
@ingroup baseband_info	
Device configuration APIs deals with Get/Set transmit power, Get Temperature and UART 
IP Filtering status notification.
*/
#define TYPE_DEVICE_CONFIG							13
/*********************************************************************/

/**
@ingroup baseband_info
The application can set the baseband transmit power through this request. 
The TX PWR range can vary between -40 to +23 dBm. 
TX PWR value can be 
incremented in steps of 0.25 
Example usage:
	If TX POWER is to be set 2.5 dBm
	
	Payload  = 2.5 X 4

@Warning:
This is only for use under lab conditions. When this API is called, 
the normal transmit power control mechanism is disabled.

Payload: INT32
*/
#define SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_REQ		2 /* App -> DLL */

/**
@ingroup baseband_info
As a response to the Set Baseband Power request, the 
application is called back with the response #_S_SET_TX_POWER_RESPONSE

This notification is issued only to the requesting application.
*/
#define SUBTYPE_BASEBAND_TRANSMIT_POWER_SET_RESP	2 /* DLL -> App */

/**
@ingroup baseband_info
The application can request the baseband temperature through this request.
*/
#define SUBTYPE_GET_BASEBAND_TEMPERATURE_REQ		3 /* App -> DLL */

/**
@ingroup baseband_info
As a response to the set baseband power request, the application is called 
back with this response.
This notification is issued only to the requesting application.

Payload: degree Celsius (UINT32)
*/
#define SUBTYPE_GET_BASEBAND_TEMPERATURE_RESP		3 /* DLL -> App */

/**
@ingroup baseband_info
The application can reset power control mode of Baseband.
From manual power control mode to automatic power control mode.
*/
#define SUBTYPE_RESET_POWER_CONTROL_REQ				4 /* App -> DLL */

/**
@ingroup baseband_info
As a response to the reset baseband power request, the application is called 
back with this response.
This notification is issued only to the requesting application.

Payload: UINT32

Where Status :
	- 0 : Indicates Success
	- Non-zero : Indicates failure with error code.
*/
#define SUBTYPE_RESET_POWER_CONTROL_RESP			4 /* App -> DLL */
/**
@ingroup baseband_info
This API is used to set the RSSI and CINR threshold for network search.
Payload contains the RSSI and CINR threshold to to be set.

Payload: #_ST_SET_RSSI_CINR

*/
#define SUBTYPE_SET_RSSI_CINR_THRESHOLD_REQ			5
/**
@ingroup baseband_info
As a response to the set baseband #SUBTYPE_SET_RSSI_CINR_THRESHOLD_REQ request, the application receives callback with this response.
Payload contains the actual threshold RSSI and threshold CINR values used for search filtering.


Payload: #_ST_SET_RSSI_CINR

*/
#define SUBTYPE_SET_RSSI_CINR_THRESHOLD_RESP			5
/**
@ingroup baseband_info
This notification gives the status of UART IP Filtering. The status of domain/URL blocking is to be interpreted by payload #_IP_FILTER_STATUS.

Payload: #_IP_FILTER_STATUS

*/
#define SUBTYPE_IP_FILTER_STATUS_NOTIFICATION			6



/**
@ingroup fw_download
The following APIs allow an application to control Beceem’s WiMAX chipset. 
This spans, reset API for the baseband modem, API for Firmware download and 
built-in processor control.

feature support info:
        -  This feature may not be supported on few platforms. Please check for support details.

*/
#define TYPE_CHIPSET_CONTROL						16
/*********************************************************************/
/**
@ingroup fw_download
When an application wants to reset the baseband modem runtime, it sends a message.

There are no other operations performed before the chip is reset. Application should 
remember that the base station may expect a deregistration from the subscriber station 
and may wait for a timeout before it allows this subscriber to connect again. 
Applications are expected to handle these scenarios. If the link is up, the application 
should send a link down (refer to 5.1.2.2, Link Down) request before sending this request.

feature support info:
        -  This feature is not supported on 5350 and 352 line of chipsets.

Returns:
	- ERROR_RESET_FAILED : If failed to reset
*/
#define SUBTYPE_RESET_REQ							1 /* App -> DLL */

/**
@ingroup fw_download
When an application initiated Baseband Chip Reset Request completes, the library 
calls back with this indication. This notification is issued only to 
the application that has issued a baseband chip reset request.

Once the reset is completed, the application is required to re-initialize the chipset 
in a similar manner as power-up initialization. The firmware has to be re-downloaded.

feature support info:
        -  This feature is not supported on 5350 and 352 line of chipsets.

*/
#define SUBTYPE_RESET_INDICATION					1 /* DLL -> App */

/**
@ingroup fw_download
An application that wants to download firmware afresh should send this message, 
where the Firmware Info Structure has the format #_ST_FIRMWARE_DNLD

feature support info:
        -  This feature is not supported on 5350 and 352 line of chipsets.


*/
#define SUBTYPE_FIRMWARE_DNLD_REQ					2 /* App -> DLL */

/**
@ingroup fw_download
As a success or failure indication of a firmware download request, the application 
is called back with this indication. This notification is issued only to 
the application that has issued a Firmware download status request. Where the 
Firmware Status Indication Structure has the format #_FIRMWARE_STATUS_INDICATION

feature support info:
        -  This feature is not supported on 5350 and 352 line of chipsets.


Returns: 
	- ERROR_THREAD_NOT_STARTED : If library failed to start Firmware download thread
*/
#define SUBTYPE_FIRMWARE_DNLD_STATUS_RESP			2 /* DLL -> App */

/**
@ingroup fw_download
Baseband modem status can be queried using this API.

Payload: #_E_BASEBAND_MODEM_STATUS_ (payload is meaningful if return value is SUCCESS.)

Returns:
	- SUCCESS : If operation is successful else appropriate error code
*/
#define SUBTYPE_BASEBAND_MODEM_STATUS_REQ     3


/**
@ingroup fw_download
When an application wants to get the chip reset status, it sends a message.
In response to this request it would receive #SUBTYPE_RESET_STATUS_RESP 

feature support info:
        -  This feature is not supported on 5350 and 352 line of chipsets.

*/
#define SUBTYPE_RESET_STATUS_REQ							4 /* App -> DLL */

/**
@ingroup fw_download
In respose to application request of #SUBTYPE_RESET_STATUS_REQ. Library would provide this callback.

Payload: 
	- TRUE(1) - If device is in chip reset.	
	- FALSE(0) - If device is not in reset mode.
*/
#define SUBTYPE_RESET_STATUS_RESP					4 /* DLL -> App */

/**
@ingroup fw_download
An application that wants to redownload config file afresh without doing power cycloe should call this API.
This redownload is effective after resync of baseband modem.
It is advised that user of this API refer to target params document for details of which fields can be effective after redownload.
Payload: 
    -   #_ST_CONFIGFILE_DNLD

Return value:
    - ERROR_SYSCONFIG_OPEN_FAILED
    - ERROR_INVALID_ARGUMENT
    - ERROR_DEVICEIOCONTROL_FAILED
    - SUCCESS: config file download is Successful.

*/
#define SUBTYPE_CONFIGFILE_RE_DNLD_REQ					5/* App -> DLL */




/**
@ingroup stats
API related to Baseband Statistics. 
*/
#define TYPE_STATISTICS								17
/*********************************************************************/

/**
@ingroup stats


Beceem statistics structure contains diagnostic information. A WiMAX connection management application can use 
these statistics elements to monitor various baseband statistics.
(For Example: packet error rates, counts of PDUs received/sent/dropped/corrupted.)

When an application wants to retrieve baseband statistics, it sends this API request.
The payload here is a time value in milliseconds, which is used as timer period. 
Every time this timer fires the application’s callback is called with Baseband 
Statistics Indication. The application can stop periodic statistics indications
 by sending statistics request with zero as the payload. 

The application gets baseband statistics information through a Baseband 
Statistics Indication.

Payload: UINT32

Returns :
	- ERROR_THREAD_NOT_STARTED : If library could not start statistics thread.
*/
#define SUBTYPE_BASEBAND_STATISTICS_REQ				1 /* App -> DLL */

/**
@ingroup stats 
This statistics indication is issued only to the application that has issued a statistics request.

When an application requests for baseband statistics information, the library will callback the application with
statistics payload information.

Payload : #stStatistics_SS
*/
#define SUBTYPE_BASEBAND_STATISTICS_INDICATION		1 /* DLL -> App */

/**
@ingroup stats
Application has the provision to disable/enable the resetting of statistics counters by baseband modem
on every re-sync by issuing this API request. 


Payload : UINT32

Where Payload can be set to:

	- 0 : To disable statistics reset on re-sync
	- 1 : To enable statistics reset on re-sync
	
*/
#define SUBTYPE_STATISTICS_RESET_CONTROL_REQ		2 /* App -> DLL */

/**
@ingroup stats

As a response to #SUBTYPE_STATISTICS_RESET_CONTROL_REQ, application receives this callback.
This notification is issued only to the application that has sent a reset control request.

Payload : UINT32

	- 0 : Disabled statistics reset on re-sync
	- 1 : Enabled statistics reset on re-sync
*/
#define SUBTYPE_STATISTICS_RESET_CONTROL_RESP		2 /* DLL -> App */

/**
@ingroup idle_mode
The WiMAX protocol has provision for idle mode support where the subscriber station can 
optimize on power consumption. The following APIs allow a subscriber station to initiate entry 
into idle mode in the baseband chipset and indicate the application of these statuses. 
*/
#define TYPE_IDLE_MODE								19
/*********************************************************************/

/**
@ingroup idle_mode
When an application wants to put the baseband modem in an idle mode, it sends this 
message
*/
#define SUBTYPE_IDLE_MODE_REQ						1 /* App -> DLL */

/**
@ingroup idle_mode
When the baseband goes into idle mode or comes out of idle mode, the application’s 
callback is called with this notification. This notification is issued to 
all the applications that have registered a callback with the library. The application 
can send data through the network stack to get the modem out of Idle mode.

Payload: #_ST_IDLEMODE_INDICATION
*/
#define SUBTYPE_IDLE_MODE_INDICATION				1 /* DLL -> App */

/**
@ingroup idle_mode
An application can request if the baseband modem has entered idle mode through this 
request message

As a response to an Idle Mode Status Request, the application is called back 
with an idle mode indication response. No payload is required.

*/
#define SUBTYPE_IDLE_MODE_STATUS_REQ				2 /* App -> DLL */

/**
@ingroup idle_mode
Library calls the application’s registered callback function, with Idle Mode 
Status Response. This notification is issued only to the application that has 
issued a management information base set request. 

TRUE is issued, When the baseband is in idle mode. 

Payload : UINT32 (TRUE/FALSE)
*/
#define SUBTYPE_IDLE_MODE_STATUS_RESP				2 /* App -> DLL */

/**
@ingroup idle_mode
An application can request to change the Idle Mode Inactivity Timer for the baseband 
modem through this request message

As a response to an Idle Mode Inactivity Timer Request, the application is called back 
with an idle mode inactivity timer response. 

Payload: time in seconds (UINT32)

*/
#define SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_REQ		3 /* App -> DLL */

/**
@ingroup idle_mode
The DLL calls the application’s registered callback function, with Idle Mode 
Timer Response. This notification is issued only to the application that has 
issued a idle mode inactivity timer set request. 

SUCCESS is returned on successful timer updation, Error Code is returned
to the application on failure.

Payload: SINT32

Returns: 
	- SUCCESS : If Idle Mode Inactivity Timer Updation is Success
	- ERROR_IDLE_MODE_INACTIVITY_TIMER_SET_FAILED : If Idle Mode Inactivity
													Timer Updation is Failed

*/
#define SUBTYPE_IDLE_MODE_INACTIVITY_TIMER_RESP		3 /* App -> DLL */
/**
@ingroup idle_mode
An application can request the baseband modem to wake up from idle mode 
through this request message

As a response to an Idle Mode wakeup request, the application is called back 
with an idle mode wakeup response. 

Payload: None

*/
#define SUBTYPE_IDLE_MODE_WAKEUP_REQ				4 /* App -> DLL */
/**
@ingroup idle_mode
The library calls the application’s registered callback function.
This notification is issued only to the application that has issued a 
idle mode wakeup request. 

Payload : UINT32
	- TRUE (1): If Idle Mode Wakeup is Success
	- FALSE (0): If Idle Mode Wakeup is Failed
*/
#define SUBTYPE_IDLE_MODE_WAKEUP_RESP				4 /* App -> DLL */


/**
@ingroup mac_mgmt
For network diagnostics tools, all MAC level signaling and control information exchanges 
between the subscriber station and the base station may have to be logged. This section 
describes APIs to extract all MAC level control exchanges from the baseband.
*/
#define TYPE_MACMGMT								21
/*********************************************************************/
/**
@ingroup mac_mgmt
When an application wants to extract all MAC level control exchanges from the baseband 
and enable a run-time callback for each of these exchanges, it calls a MAC Management 
Message Enable Request.

Once this request is made the application’s callback gets notifications on all MAC level 
control exchanges between the base station and the subscriber station. However, there 
is no provision to store past exchanges which could have happened before the application 
asked for these indications.
*/
#define SUBTYPE_MACMGMT_ENABLE_REQ					1 /* App -> DLL */

/**
@ingroup mac_mgmt
When an application wants to stop MAC level control exchanges being indicated, it calls 
a MAC Management Message Disable Request.
After this request, no MAC Level Control exchanges between the base station and the 
subscriber station are indicated to the application.
*/
#define SUBTYPE_MACMGMT_DISABLE_REQ					2 /* App -> DLL */

/**
@ingroup mac_mgmt
When an application wants to know the status of MAC management messages to HOST,
application can issue a MAC Management Message Get Status request.

Payload: NA

*/
#define SUBTYPE_MACMGMT_GET_STATUS_REQ				3 /* App -> DLL */

/**
@ingroup mac_mgmt
DLL responds to the received get status request by the application with the subtype
indicating the status of the MAC Management Message status to HOST.

Payload: UINT32 Status.

Where Status:
	- TRUE	(1): Indicates Success
	- FALSE	(0): Indicates failure

*/
#define SUBTYPE_MACMGMT_GET_STATUS_RESP				3 /* DLL -> App */

/**
@ingroup mac_mgmt
After an application calls MAC management message Enable request, the library 
extracts all further MAC level control exchanges between the base station and the 
subscriber station and calls the application registered callback function with the following. 
This notification is issued only to the application that has issued a MAC management 
request. The MAC management message mtructure is : #_S_MAC_MGMTMESSAGE

*/
#define SUBTYPE_MACMGMT_INDICATION					1 /* DLL -> App */

/**
@ingroup gpio_api
The baseband chip has the capability to interface with other components on the board 
using GPIOs. The information about the GPIO connections and its usage is very specific 
to the board designed. 

@Warning:
The APIs provided here are very generic APIs to set and reset a 
particular GPIO.  These APIs should be used only as per the board design recommendation. 
These APIs can have undesirable effect on the chip if used otherwise.

*/
#define TYPE_GPIO									22
/*********************************************************************/
/**
@ingroup gpio_api
This API can be used to set a GPIO to 1 or 0. Only one GPIO pin can be set with one 
request. If multiple GPIOs have to be set – the application is expected to call multiple GPIO 
Set Requests.  

Payload: #_GPIO_INFO

Returns: 
	- ERROR_INVALID_GPIO_NUMBER : If GPIO ID is invalid.
*/
#define SUBTYPE_GPIO_SET_REQ						1 /* App -> DLL */
/**
@ingroup gpio_api
This API can be used to get the status of a particular GPIO pin. The status of only 
one GPIO pin can be retrieved with one API call. If values of multiple GPIOs have to 
be retrieved – the application is expected to call multiple GPIO Status requests. 

Payload: #_GPIO_INFO
*/
#define SUBTYPE_GPIO_STATUS_REQ						2 /* App -> DLL */

/**
@ingroup gpio_api
This is the callback function for GPIO status request sent by the application. 
The status of only one GPIO pin is updated with one indication. If multiple GPIO 
set/status requests have to been sent, the application should expect to be called 
with multiple GPIO Status Indication messages. This notification is issued only to 
the application that has issued a GPIO request.

Payload: #_GPIO_INFO
*/
#define SUBTYPE_GPIO_STATUS_INDICATION				2 /* DLL -> App */

/**
@ingroup gpio_api
This API can be used to set the LEDs to represent different states of the baseband.

Payload: #_LED_INFORMATION
Returns: 
	- SUCCESS : If  LED settings is set successfully.
	- ERROR_API_NOT_SUPPORTED : If API is not supported in the platform.
*/

#define SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_REQ		3 /*APP -> DLL*/

/**
@ingroup gpio_api
The library calls the application’s registered callback function if LED settings request is 
successful.

Payload: None
*/

#define SUBTYPE_GPIO_SET_LED_BEHAVIOR_SETTINGS_RESP		3 /*DLL -> APP*/

/**
@ingroup gpio_api
This API is used to get the LED settings presently in use by the baseband.

Payload: None
Returns: 
	- SUCCESS : If getting LED settings is Successful.
	- ERROR_API_NOT_SUPPORTED : If API is not supported in the platform.

*/

#define SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS_REQ		4 /*APP -> DLL*/

/**
@ingroup gpio_api
The library calls the application’s registered callback function in response to get LED settings request.
If the API is not supported in the platform it returns ERROR_API_NOT_SUPPORTED
Payload: _LED_INFORMATION x 4
Refer: #_LED_INFORMATION

*/

#define SUBTYPE_GPIO_GET_LED_BEHAVIOR_SETTINGS_RESP		4 /*DLL -> APP*/

/**
  @ingroup gpio_api

  Note: This API is available only for CPE platforms. 

  This API can be used to set multiple GPIOs output to 1 or 0.
 
  Payload: #_ST_ARRAY_GPIO_MULTI_INFO_

Returns:
  - ERROR_INVALID_GPIO_NUMBER : If GPIO ID is invalid.
  */
#define SUBTYPE_GPIO_MULTI_REQ            5 /* App -> DLL */
/**
  @ingroup gpio_api

  Note: This API is available only for CPE platforms.

  This API can be used to set multiple GPIO mode to IN or OUT.
 
  Payload: #_ST_ARRAY_GPIO_MULTI_MODE_
 
  Returns:
    - ERROR_INVALID_GPIO_NUMBER : If GPIO ID is invalid.
    */
#define SUBTYPE_GPIO_MODE_REQ           6 /* App -> DLL */
/**
@ingroup gpio_api
This API is used to enable/disable the driver's LED thread activity. By default, LED thread is in enabled state. 


Payload: UINT

Where payload value,
 - 0 : Disables the LED thread. There will be no LED activity till state is enabled. 
 - 1 : Enables the LED thread

*/
#define SUBTYPE_LED_THREAD_STATE_REQ	7 
/**
@ingroup ho
This section describes the HO (handoff) related APIs.
*/
#define TYPE_HANDOFF_MESSAGE						23
/*********************************************************************/
/**
@ingroup ho
This HO indication API is called back to the application when the baseband performs a handoff.

This notification is issued to all the applications that have registered a callback with the library.

Payload: #_S_HOINFO

*/
#define SUBTYPE_HANDOFF_INDICATION			1		/* DLL -> App */


/**
@ingroup MIB_Api

The APIs defined below provide interface to the 802.16 MIB. The current implementation 
of MIB interface follows the 802.16f standard. Refer to this standard to see 
the definition of Object and to determine the corresponding IDs. Both the APIs provided 
work asynchronously and the response comes back through the general callback. 

Returns: 
	- ERROR_AUTH_CRC_MISMATCH : If CRC check for EEPROM has failed.

*/
#define TYPE_MIB									24
/*********************************************************************/
/**
@ingroup MIB_Api

An application that wishes to modify any MIB parameter (with write access) should 
call the MIB Set Request. For any set request to take effect the user needs to do a DREG, and re-enter the network after performing a Set request. The set request is called with the following information:


For a set request, the value of u32MibObjSize must be set to the number of bytes to be 
written for the MIB object that is being set. Once the request is placed, 
the application is expected to receive the Set Response through the API Callback 
registered with the DLL. 

Any value set using MIBs stds/proprietary can change the system functioning
unpredictably. Consult Beceem before setting these values.

Payload: #_MIB_OBJECT_INFO 

Returns: 
	- ERROR_MIB_OBJ_READONLY : If application is trying to write a READ-ONLY MIB ID.
	- ERROR_INVALID_MIB_OBJ_SIZE : If size of MIB ID is invalid.
	- ERROR_INVALID_MIB_OBJ_ID : If MIB ID is invalid.
	- ERROR_MIB_OBJECT_NOTFOUND : If library could not find specified MIB object.
*/
#define SUBTYPE_MIB_SET_REQ							1 /* App -> DLL */

/**
@ingroup MIB_Api

The baseband chip responds back to the Set request, the DLL calls the application’s 
registered callback function. This notification is issued only to the application that has issued a management information base set request. 

The Payload is the same structure given during set request operation, but 
with the s8ErrorCode updated.  If the s8ErrorCode is zero (0) it implies the 
set succeeded, while a non-zero value implies a failure. The value of u32MibobjSize in 
MibObjInfo contained in payload indicates actual bytes written for the set request.

Payload: #_MIB_OBJECT_INFO 
*/
#define SUBTYPE_MIB_SET_RESP						1 /* DLL -> App */

/**
@ingroup MIB_Api

An application that wants to read any read-write/read-only MIB parameter should call 
the MIB Get Request with the following information, after the firmware is downloaded.

The user is expected to provide valid values for the u32MibObjId and u32MibObjSize 
in the payload. Once the Get Request is placed, the application is expected to 
receive the Get Response through the API Callback Registered with the DLL. 
The required MibObj value is taken from the Get Response Payload. The value of the 
u32MibobjSize field in MibObjInfo must be set to Zero for a get request.

Payload: #_MIB_OBJECT_INFO 

Returns: 
	- ERROR_INVALID_MIB_ATTRIBUTE : If application queries for invalid MIB object.
	- ERROR_MIB_OBJECT_NOTFOUND : If library could not find specified MIB object.
*/
#define SUBTYPE_MIB_GET_REQ							2 /* App -> DLL */

/**
@ingroup MIB_Api
The baseband responds back to the set request, the DLL calls the application’s 
registered callback function. This notification is issued only to the application 
that has issued a management information base get request. The response is in #_MIB_OBJECT_INFO 

Payload: #_MIB_OBJECT_INFO 
*/
#define SUBTYPE_MIB_GET_RESP						2 /* DLL -> App */

/**
@ingroup MIB_Api
An application that wishes to modify bulk MIB object parameters (with write access) 
should call the Multiple MIB Set Request with the following information.

Payload: #_MIB_OBJECTS 

For a Set Request the value of u16NumMibObjs must be set with the valid values for
each MIB object. Once the request is placed, the application is expected to receive the set response through the API Callback 
registered with the DLL. 

Any value set using MIBs stds/proprietary can change the system functioning
unpredictably. Consult Beceem before setting these values.

Returns: 
	- ERROR_MIB_OBJ_READONLY : If application is trying to write a READ-ONLY MIB ID
	- ERROR_INVALID_MIB_OBJ_SIZE : If size of MIB ID is invalid
	- ERROR_INVALID_MIB_OBJ_ID : If MIB ID is invalid
	- ERROR_MIB_OBJECT_NOTFOUND : If library could not find specified MIB object
*/
#define SUBTYPE_MULTIPLE_MIB_SET_REQ				3 /* App -> DLL */

/**
@ingroup MIB_Api

The baseband chip responds back to the Multiple MIB Set request, the DLL calls the 
application’s registered callback function. This notification is issued only to the 
application that has issued a management information base set request. 

Payload is the same structure given during Multiple MIB Set Request operation, but 
with the s8ErrorCode updated. If the s8ErrorCode is zero (0) it implies the 
set succeeded, while a non-zero value implies a failure. The value of u32MibobjSize in 
MibObjInfo contained in payload indicates actual bytes written for the set request.

Payload: #_MIB_OBJECTS */
#define SUBTYPE_MULTIPLE_MIB_SET_RESP				3 /* DLL -> App */

/**
@ingroup MIB_Api
An application that wants to read any read-write/read-only Multiple MIB parameter should
call the Multiple MIB Get Request with the following information, after the firmware is downloaded.

The user is expected to provide the total number of MIB objects and the valid values of each MIB 
object in the payload. Once the Multiple MIB Get Request is placed, the application is expected to 
receive the Get Response through the API Callback registered with the DLL. 
The required MibObj value is taken from the Get Response Payload.

Payload: #_MIB_OBJECTS 

Returns: 
	- ERROR_INVALID_MIB_ATTRIBUTE : If application queries for invalid MIB object
	- ERROR_MIB_OBJECT_NOTFOUND : If library could not find specified MIB object
*/
#define SUBTYPE_MULTIPLE_MIB_GET_REQ				4 /* App -> DLL */

/**
@ingroup MIB_Api
An application that wants to receive indication on set of MIBs on a periodic basis can subscribe to this call.

The user is expected to provide the total number of MIB objects and the valid MIB 
ID list in the payload. Once the multiple periodic MIB Get request is placed, the application is expected to 
receive the #SUBTYPE_MULTIPLE_MIB_GET_RESP notification on periodic basis.


Payload: #_MULTI_MIB_INDICATION_ST_

Returns: 
	- ERROR_THREAD_NOT_STARTED: If thread timer for indication could not be started
	- ERROR_INVALID_ARGUMENT: If parameter specified is invalid
*/
#define SUBTYPE_MULTIPLE_MIB_PERIODIC_INDICATION_REQ				5 /* App -> DLL */


/**
@ingroup MIB_Api
The baseband responds back to the Multiple Get request, the DLL calls the application’s 
Registered Callback function. This notification is issued only to the application 
that has issued a management information base get request. The response is in #_MIB_OBJECTS 

Payload: #_MIB_OBJECTS 
*/
#define SUBTYPE_MULTIPLE_MIB_GET_RESP				4 /* DLL -> App */

/**
@ingroup nw_layer_ctrl
Network Layer Control Message
*/
#define TYPE_NETWORK_LAYER_CONTROL					25 

/**
@ingroup nw_layer_ctrl
IP Refresh indication to the application. Device management/control application should 
refresh the IP when it receives this notification. 
*/
#define SUBTYPE_IP_REFRESH_INDICATION				1 /* DLL -> App */

/**
@ingroup MAC_Logging
MAC debug logging information.
*/
#define TYPE_MAC_LOGING_INFO						27
/**
@ingroup MAC_Logging
MAC debug logging information indication. This indication contains the debug logging payload which has to be parsed to decipher
the necessary information.
The payload for indication is a variable length UINT32 array.
*/
#define SUBTYPE_MAC_LOGING_INFO_INDICATION			1

/**
@ingroup MAC_Logging
Enable MAC debug logging messages
*/
#define SUBTYPE_ENABLE_MAC_LOGGING					2

/**
@ingroup MAC_Logging
Disable MAC debug logging messages
*/
#define SUBTYPE_DISABLE_MAC_LOGGING					3


/**
@ingroup Sleep_mode
This message should be used by the host application or target for exchanging Sleepmode related power save class information.
*/
#define TYPE_SLEEP_MODE                             26
/************************************************************/

/**
@ingroup Sleep_mode
Sleep mode request message that should be used for one or more of the following: 
 - PSC definition/redefinition/activation/deactivation
 The message is used for requests by the Host to Target(MAC). The host is expected 
 to input valid information.
 Payload: #ST_SLEEPMODE_MSG
*/
#define SUBTYPE_SLEEP_MODE_REQ				1 /* App -> Dll */

/**
@ingroup Sleep_mode
Sleep mode response message that contains one or more of the following: 
 - PSC definition/redefinition/activation/deactivation
 The message is used for sending the response by the Target(MAC) to Host.
Payload: #ST_SLEEPMODE_MSG
*/
#define SUBTYPE_SLEEP_MODE_RESP				2 /* Dll -> App  */
/**
@ingroup Smart_Antenna
An application that wants to perform slow scanning and fast switching of antennas for the MS.

feature support info:
        -  Only supported on Maya.

*/
#define TYPE_ANTENNA_SWITCHING_SELECTION			28
/************************************************************/

/**
@ingroup Smart_Antenna
Antenna Configuration request
- An application that wants to set/get the configuration to be used for Antenna Switching and 
Selection

- Set configuration from host:

The host is expected to provide the valid information when set configuration is selected.
Once the Configuration Set Request is placed, the application is expected to 
receive the Set Response through the API Callback Registered with the DLL. 

- Get configuration from baseband:
The host expects the information to be used for antenna switching when get configuration 
is selected. Once the configuration Get Request is placed, the application is expected to 
receive the Get Response through the API Callback Registered with the DLL.

Payload used for Antenna Configuration Request: #_ST_GPIO_SETTING_INFO
*/
#define SUBTYPE_ANTENNA_CONFIG_REQ			1 /* App -> Library -> Firmware */

/**
@ingroup Smart_Antenna
- Firmware responds to the configuration request with the configuration response based on the 
flag u32ConfigSettingsFlag setting of _ST_GPIO_SETTING_INFO.

- Set configuration from host:

If u32ConfigSettingsFlag is set to 1, the response is either success or failure. 
If the response is success, then u32ErrorStatus is set as SUCCESS else error code is filled in u32ErrorStatus. 

- Get configuration from host:

If u32ConfigSettingsFlag set to 0, the response contains the payload as 
ST_GPIO_SETTING_INFO and the information in the response is valid when u32ErrorStatis 
is set as SUCCESS. 

Payload used for Antenna Configuration Response: #_ST_GPIO_SETTING_INFO
*/
#define SUBTYPE_ANTENNA_CONFIG_RESP			1 /* Firmware -> Libary -> App  */
/**
@ingroup Smart_Antenna
Antenna Scanning Request
- The host can issue the request towards firmware to perform scanning operation 
for antenna switching.

Application issuing the Antenna Scanning Request should fill the proper valid 
information for firmware to perform the scanning. Once the Antenna Scanning Request 
is placed, the application is expected to receive the Antenna Scanning Response 
through the API Callback registered with the DLL. Payload is defined for response
as ST_RSSI_INFO_IND.

Payload used for Antenna Scanning Request: #_ST_RSSI_INFO_REQ
*/
#define SUBTYPE_ANTENNA_SCANNING_REQ		2 /* App -> Library -> Firmware */

/**
@ingroup Smart_Antenna
Antenna Scanning response
- Firmware on receiving the Antenna Scanning Request from Host application takes the 
appropriate action and on completion of the action, a response is sent back to the Host
with the u32ErrorStatus field set appropriately of ST_RSSI_INFO_IND.

Response is valid only when u32ErrorStatus of ST_RSSI_INFO_IND is set to SUCCESS.

Payload used for Antenna Scanning Response: #_ST_RSSI_INFO_IND
*/
#define SUBTYPE_ANTENNA_SCANNING_RESP		2 /* Firmware -> Libary -> App  */
/**
@ingroup Smart_Antenna
Antenna Selection Request
- An application requests firmware to select the Rx/Tx antennas as well as the 
Tx Antenna Auto Select mode. Once the Antenna Selection Request is placed, the 
application is expected to receive the Antenna Selection Response through the API 
Callback registered with the DLL. Payload is defined for response
as SINT32.

Payload used for Antenna Selection Request: #_ST_ANTENNA_INFO_REQ
*/
#define SUBTYPE_ANTENNA_SELECTION_SET_REQ	5 /* App -> Library -> Firmware */

/**
@ingroup Smart_Antenna
Antenna Selection Response
- Firmware responds to Antenna Selection Request by issuing a success or failure as 
payload.

Payload: SINT32

Returns: 
	- SUCCESS : If Antenna Selection request is Success
	- FAILURE : In case of Failure, Error Code is returned as payload
				#_ANTENNA_SWITCHING_STATUS_CODES
*/
#define SUBTYPE_ANTENNA_SELECTION_SET_RESP	5 /* Firmware -> Libary -> App  */
/**
@ingroup  Link_Options
This set of APIs is to enable/disable or to control few of the features supported by baseband modem. 
*/
#define TYPE_SET_WIMAX_OPTIONS					29
/**
@ingroup  Link_Options
Only one option should be used at one time. 
Payload is: #_ST_WIMAX_OPTIONS. 

Bit 0 to bit 7 in u8WimaxOptionSpecifier can be used to turn on/off to specify the link control options for the baseband modem. 
-	Bit 0 - when set to 1 baseband will receive a payload of UINT32 
	with the Network Connect options.
	-	The Network Connect options payload (u32Payload) is a bit field defined below:
		-	Bit 0 – Auto resync enable flag
			When set, auto resync is enabled and the modem tries to automatically 
			resync to a basestation based on its internal logic. 
			When reset auto resync is disabled, the modem will sync down and await 
			further commands from the host.		
		-	Bit 1 – Full BSID decoding enable flag for network search
			When set, the modem will always decode the full BSID during network search. 
			When reset the modem will not wait for full BSID. This option is only 
			applicable for base stations with compressed maps. It does not make 
			any difference while detecting a base station with normal maps.
		-	Bit 2 – Full BSID decoding enable flag for sync up. This flag controls 
			whether the full BSID is reported in sync up response. This is again 
			applicable for base stations with compressed maps and does not have any 
			affect on a sync up with a base station sending normal maps.
			When set, the modem will always decode the full BSID
			When reset, the modem will return the partial BSID in sync up response.
		-	Bit 3 – Per Antenna Reporting for network search
		-	Bit 4 –31 – Reserved for future use for Network 
			Connection related parameters (network search and sync up)

-	Bit 1 is used to turn off/on standard configuration MIB writes to non volatile memory(EEPROM or Flash).
	Payload is 32 bit.
		- If set to 1, MIB writes to NVM are disabled
		- If set to 0, MIB writes to NVM are enabled(Writes are enabled by default)
	This option is processed by library and no request is sent to baseband.

-	Bit 3 - is used to turn off/on, MCS MIBs logging and error rate MIBs logging on baseband modem.
		The payload is a bit field defined as below:
		-	Bit 0 – Enable/Disable MCS MIBs logging on baseband modem
			 - If bit is set MCS MIBs logging is enabled
			 - If bit is reset MCS MIB logging is disabled		
		-	Bit 1 – Enable/Disable  error rate stats MIBs logging on baseband modem
			 - If bit is set, error rate stats MIBs logging is enabled 
			 - If bit is reset error rate stats MIBs logging is disabled
		-	Bit 2 –31 – Reserved for future use
			


-	Bit 4 Indicate the EAP security Level and the values
	Payload is 32 bit
	-  (0)0 0 – Security level Low – FULL EAP Supplicant  in HOST-OS ( Default Option – Current option)
	-  (1)0 1 - Security level Moderate – Split EAP Supplicant: Part in HOST-OS & Private key operations in baseband
	-  (2)1 0 - Security level High – Full EAP Supplicant in Baseband firmware – TBD in Future
	-  (3)1 1 – Value reserved for Future use

-	Bit 5 – Idle mode paging cycle value. When this bit is set the 32 bit payload contains 
	the paging cycle value 
	
-	Bit 6 –BS out of coverage counter in ms.
	
-	Bit 7 is not used.
*/
#define SUBTYPE_SET_WIMAX_OPTIONS_REQ					1

/**
@ingroup  Link_Options
Indicates response for SUBTYPE_SET_WIMAX_OPTIONS_REQ. Payload for this should be same as the payload in SUBTYPE_SET_WIMAX_OPTIONS_REQ operation. 
Payload is same as : #_ST_WIMAX_OPTIONS

u32WimaxOptionsPayload indicates the SUCCESS/FAILURE.
0 Indicates SUCCESS, Non-Zero indicates failure.
*/
#define SUBTYPE_SET_WIMAX_OPTIONS_RESP					1

/**
@ingroup Link_Options
If an application wants to set all the WiMAX options at a time it can use this subtype. This API is used to enable/disable multiple features of baseband modem at a time, 
where as #SUBTYPE_SET_WIMAX_OPTIONS_REQ can be used to select single WiMAX option at a time. 

u8WimaxOptionSpecifier is used to select options to be set and is same as option specifier of #SUBTYPE_SET_WIMAX_OPTIONS_REQ, but supports multiple options to be set and 
au32WimaxOptionsPayloadPerOpt represents array of eight payloads with respect to eight bits of u8WimaxOptionSpecifier. 


Payload: #_ST_WIMAX_OPTIONS_ALL
*/
#define SUBTYPE_SET_ALL_WIMAX_OPTIONS_REQ          2

/**
@ingroup Link_Options
Indicates response for #SUBTYPE_SET_ALL_WIMAX_OPTIONS_REQ. Payload for this should be same as the payload in SUBTYPE_SET_ALL_WIMAX_OPTIONS_REQ operation. 

Payload: #_ST_WIMAX_OPTIONS_ALL
*/
#define SUBTYPE_SET_ALL_WIMAX_OPTIONS_RESP          2

/**
@ingroup Link_Options
This API subtype can be used to get all the WiMAX options that were set. Request for WiMAX options can be made with this subtype and payload filled with zeroes.

Payload: #_ST_WIMAX_OPTIONS_ALL
*/
#define SUBTYPE_GET_ALL_WIMAX_OPTIONS_REQ          3
/**
@ingroup Link_Options
This indicates response for #SUBTYPE_GET_ALL_WIMAX_OPTIONS_REQ. The payload received represents the present WiMAX link control options selected. 
*/
#define SUBTYPE_GET_ALL_WIMAX_OPTIONS_RESP         3
/*
@ingroup Link_Options
This API can be used to set mask for enabling/disabling particular control message coming from driver based on software status.
Payload is a UINT32 bitmap, where Bit0 to Bit15 bits correspond to 0xA0 to 0xAF and Bit16 to Bit31 correspond to 0xB0 to 0xBF software status messages.

Payload : UINT32
*/
#define SUBTYPE_CONTROLMSG_SUBSCRIPTION_REQ        4

/**
@ingroup Shutdown_Options
If an application wants to intiate shutdown to the Beceem baseband 
subscriber station modem, the following APIs have to be used.

feature support info:
        -  Not supported on CPE platforms (for ex: 5200,5350)
*/
#define TYPE_SHUTDOWN							30

/**
@ingroup Shutdown_Options
To achieve a MAC level shutdown, an application will send this message to DLL.
When the baseband achieves a shutdown, the library/driver calls back the application’s 
callback function with a payload as UINT.

Returns:
	- ERROR_THREAD_NOT_STARTED : If library fails to start shutdown thread
*/
#define SUBTYPE_SHUTDOWN_REQ						1 /* App -> DLL */

/**
@ingroup Shutdown_Options
This response indicates the current state of the modem as shutdown.

This notification is issued only to the application which has requested shutdown 
As a response to the Shutdown Mode request the application is called back with a 
Shutdown Mode Response notification.

Payload is a UINT.

	- SUCCESS(0) if baseband goes to shutdown mode
 	- Non-zero implies that shutdown mode response timeout occurred	
*/
#define SUBTYPE_SHUTDOWN_RESP						1 /* App -> DLL */
/**
@ingroup Shutdown_Options
This notification is issued by the baseband to the DLL after entering into 
the shutdown state by baseband chip.

Payload is NULL.

*/
#define SUBTYPE_SHUTDOWN_REQ_FROM_FIRMWARE			2 /* DLL -> App */
/**
@ingroup Shutdown_Options
The baseband chip can come out from shutdown mode for a small duration by performing
periodic wakeup procedure. The baseband issues a notification towards DLL when wakeup procedures 
identifies any potential BS's. 

This notification is issued to all the applications that have 
registered a callback with the library. 

Payload: #_BSLIST

*/
#define SUBTYPE_SHUTDOWN_MODE_PERIODIC_WAKE_UP_IND			3 /* Firwmare -> DLL -> App */
/**
@ingroup Shutdown_Options
The baseband chip can come out from shutdown mode completely by performing 
complete wakeup procedure. The baseband issues a notification towards DLL when complete wakeup 
procedure ends.

This notification is issued to all the applications that have 
registered a callback with the library. 

Payload would be NULL.

*/
#define SUBTYPE_SHUTDOWN_MODE_COMPLETE_WAKE_UP_IND			4 /* Firwmare -> DLL -> App */
/**
@ingroup Shutdown_Options
The application can request if the baseband modem has entered shutdown mode by invoking 
this request. This request is the preferred way to get the shutdown status of the 
baseband modem since a Link Status request will cause the baseband to get out of idle mode.

As a response to the Shutdown Mode request the application is called back with a 
Shutdown Mode Status notification.
*/
#define SUBTYPE_SHUTDOWN_MODE_STATUS_REQ			5 /* App -> DLL */

/**
@ingroup Shutdown_Options
As a response to the Shutdown Mode Status Request the application is called 
back with a Shutdown Mode Status Indication. This response indicates the current 
state of the modem.

This notification is issued only to the application which has requested Shutdown 
Mode Status.
Payload is a UINT.
 - 0 Baseband is in shutdown mode
 - nonzero value implies that baseband is not in shutdown mode

*/
#define SUBTYPE_SHUTDOWN_MODE_STATUS_RESP			5 /* DLL -> App */
/*
@ingroup Shutdown_Options
An application can request the baseband modem to wake up from shutdown mode 
through this request message

Payload: None

*/
#define SUBTYPE_WAKE_FROM_SHUTDOWN			 6 /* DLL -> App */

/**
@ingroup PHY_Logging
PHY debug logging information.
- This feature is not supported anymore. 
*/
#define TYPE_PHY_LOGING_INFO						31

/**
@ingroup PHY_Logging
PHY debug logging information indication. This indication contains the debug logging payload 
which has to be parsed to decipher the necessary information.
The payload for indication is a variable length UINT32 array.
*/
#define SUBTYPE_PHY_LOGING_INFO_INDICATION			1 /* DLL -> App */
/**
@ingroup PHY_Logging
Enable PHY debug logging messages
*/
#define SUBTYPE_ENABLE_PHY_LOGGING					2 /* App -> DLL */

/**
@ingroup PHY_Logging
Disable PHY  debug logging messages
*/
#define SUBTYPE_DISABLE_PHY_LOGGING					3 /* App -> DLL */
/*
@ingroup DRV_Logging
DRV debug logging information.
*/
#define TYPE_DRV_LOGING_INFO						32

/*
@ingroup DRV_Logging
Set DRV debug logging messages
*/
#define SUBTYPE_SET_DRV_LOGGING					1 /* App -> DLL */
/**
@ingroup OMADM_Api

OMA-DM related APIs for accessing OMA-DM parameters. These APIs are blocking/synchronous. There will be no callback APIs associated with this
group of APIs. 
*/

#define TYPE_OMADM									33
/**
@ingroup OMADM_Api
Application that wants to read/write OMA-DM parameters to device should call this API to check for version compatibility.
If this matches then this version check is successful. It should go ahead and use the OMA-DM read/write and param size APIs.
If the version check is not successful then there is incompatibility with the version of OMADMUserApi.h that the driver is compiled with
and the one used by the application.

Application should pass major and minor version number to payload as is available in the OMADMUserApi.h.
This version is specific to Beceem and is not the version specified in the sprint OMA-DM specification.
Payload: #_OMADM_PARAMETER_VERSION_CHECK
Returns:
	- OMA_DM_ERROR_INVALID_VERSION: if version mismatch is found
		#_OMADM_PARAMETER_VERSION_CHECK is updated with the version that library is compatible with.
		
	- SUCCESS: If version check is successful
		

*/

#define SUBTYPE_OMADM_VERSION_CHECK				1	
/**
@ingroup OMADM_Api

An application that wishes to modify any OMA-DM parameter (with write access) should 
call the OMA DM Set Request. The set request is called with the following information:

For setting the OMA DM element/node value in the tree. 
- #_OMADM_OBJECT_INFO::u32OMADMObjSize:  must be set to the number of bytes to be 
written for the tree node or element that is being set for the particular OMA-DM Parameter 

 - #_OMADM_OBJECT_INFO::stTreeLevel Contains where the the OMA-DM object can be found in the OMA-DM tree structure 
 specified in OMADMTreeStruct.h
 
 - #_OMADM_OBJECT_INFO::u32NumOfLevel: tree depth set in the tree level structure
- #_OMADM_OBJECT_INFO::s8ErrorCode: is updated with the opeartion success (zero) or failed (non zero value)

- #_OMADM_OBJECT_INFO::au8OMADMObjVal : should contain the data to be set in the tree node/element
Before issuing this OMA-DM set request, application should query the size of the 
OMA-DM parameter. Refer #SUBTYPE_OMADM_GET_PARAM_SIZE_REQ for more details.

Payload: #_OMADM_OBJECT_INFO

Returns: 
	- SUCCESS: If parameter is written successfully
	- ERROR_OMADM_OBJECT_NOTFOUND: If library could not find specified OMA-DM object
	
*/
#define SUBTYPE_OMADM_SET_REQ						2 /* App -> Library */


/**
@ingroup OMADM_Api

An application that wishes to read any OMA DM parameter should 
call the OMA-DM read request. The get request is called with the following information:

For getting the OMA-DM element/node value in the tree. 
- #_OMADM_OBJECT_INFO::u32OMADMObjSize:  must be set to the number of bytes to be 
read from the tree node or element that is being set for the particular OMA-DM parameter. 

 - #_OMADM_OBJECT_INFO::stTreeLevel: Contains where the the OMA-DM object can be found in the OMA-DM tree structure 
 specified in OMADMTreeStruct.h
 
 - #_OMADM_OBJECT_INFO::u32NumOfLevel: tree depth set in the tree level structure. 
- #_OMADM_OBJECT_INFO::s8ErrorCode: is updated with the opeartion success (zero) or failed (non zero value).

- #_OMADM_OBJECT_INFO::au8OMADMObjVal : should contain the data read from the tree node/element. Data is valid only if the operation is successful.
Before issuing this OMA-DM set request, application should query the size of the 
OMA-DM parameter. Refer #SUBTYPE_OMADM_GET_PARAM_SIZE_REQ for more details.

Payload: #_OMADM_OBJECT_INFO

Returns: 
	- SUCCESS: If the parameter is read to application successfully
	- ERROR_OMADM_OBJECT_NOTFOUND : If library could not find specified OMA-DM object
	- OMA_DM_ERROR_READ_BUFFER_TOO_SMALL: If the buffer size passed to the API is too small
	
*/
#define SUBTYPE_OMADM_GET_REQ						3 /* App -> Library */

/**
@ingroup OMADM_Api

An application that wants to read OMA DM parameter size should call this API.

The user is expected to provide valid values for the #_OMADM_PARAMETER_INFO::stTreeLevel. It must also be set with the 
valid values for  #_OMADM_PARAMETER_INFO::u32NumOfLevel which indicates the depth of the tree values populated.

Once the Get Parameter Size Request is placed, the application is expected to receive the 
get Parameter Size in #_OMADM_PARAMETER_INFO::u32OMADMObjSize when the #_OMADM_PARAMETER_INFO::s8ErrorCode is 0.


Payload: #_OMADM_PARAMETER_INFO

Returns: 
	- SUCCESS: If the parameter size is found successfully
	- ERROR_OMADM_OBJECT_NOTFOUND : If library could not find specified OMA DM object
*/
#define SUBTYPE_OMADM_GET_PARAM_SIZE_REQ			4 /* App -> Library */
/**
@ingroup OMADM_Api

An application that wants to write the entire OMA DM tree should call this API.

The user is expected to provide proper path of the binary file which contains OMA-DM parameters into #_ST_OMA_DM_TREE_INFO::szOMADMTreeFile. 
It is the application's responsiblity to verify the proper version of the OMA-DM tree is downloaded on to the device to be in sync with the device's EEPROM 
layout. 

Payload: #_ST_OMA_DM_TREE_INFO

Returns: 
	- SUCCESS: If the OMA-DM tree is successfully downloaded on to the EEPROM

*/
#define SUBTYPE_OMADM_DWNLD_TREE				5

/**
@ingroup SEC_Api

The APIs defined below provide interface to the security APIs. Both the APIs provided 
work asynchronously and the response comes back through the general callback. 
Difference between Authentication APIs and security APIs: 
 - Authentication APIs are meant to be used during authentication stage of network entry procedure
 - Security APIs are used for storing/reading security certificates and private keys used during authentication phase
 
*/

#define TYPE_SECURITY									34

/**
@ingroup SEC_Api

An application that wants to modify any Security parameter (with write access) should 
call the Security Set request. The set request is called with the following information:

For a set request the value of u32SecObjSize must be set to the number of bytes to be 
written for the security object that is being set for the particular security parameter 
i.e. u32SecObjId. Once the request is placed, the application is expected to receive 
the Set response through the API Callback registered with the library. 

Before issuing this security set request, application should query to get the size of the 
Security parameter to be set by issuing an another API i.e. #SUBTYPE_SECURITY_GET_PARAM_SIZE_REQ.
Please refer #SUBTYPE_SECURITY_GET_PARAM_SIZE_REQ for more details.

Refer ENUM for Security Parameter IDs: #_SEC_PARAM_ID_LIST

Payload: #_SEC_OBJECT_INFO

Returns: 
	- ERROR_SEC_OBJECT_NOTFOUND: If library could not find specified security object.
*/
#define SUBTYPE_SECURITY_SET_REQ							1 /* App -> Library */
/**
@ingroup SEC_Api

As a response to the set request, the library calls the application’s registered callback function. 
This notification is issued only to the application that has issued a management information base set request. 
The payload is the same structure given during Set Request operation, but with the s8ErrorCode updated.  If the s8ErrorCode is zero (0) it implies the 
set succeeded, while a non-zero value implies a failure. 

Payload: #_SEC_OBJECT_INFO

Errors:
	- ERROR_SEC_OBJECT_NOTFOUND:  If library could not find specified security object
	
*/
#define SUBTYPE_SECURITY_SET_RESP						1 /* Library -> App */

/**
@ingroup SEC_Api

If an application wants to read any of the read-write/read-only security parameter it should call 
the security get request with the following information, after the firmware is downloaded.

The user is expected to provide valid values for the u32SecObjId in the payload. 
Once the get request is placed, the application is expected to receive the Get Response
through the API callback registered with the library. 

The required security object value is taken from the Get Response Payload. 
The value of the  u32SecObjSize field in #_SEC_OBJECT_INFO must be set to Zero for a get Request.

Refer ENUM for Security Parameter IDs: #_SEC_PARAM_ID_LIST

Payload: #_SEC_OBJECT_INFO

Returns: 
	- ERROR_SEC_OBJECT_NOTFOUND: If library could not find specified Security object.
*/
#define SUBTYPE_SECURITY_GET_REQ						2 /* App -> Library */
/**
@ingroup SEC_Api

The baseband responds back to the get request, the Library calls the application’s 
registered callback function. This notification is issued only to the application 
that has issued a get request.

Payload: #_SEC_OBJECT_INFO
*/
#define SUBTYPE_SECURITY_GET_RESP					2 /* Library -> App */
/**
@ingroup SEC_Api

An application that wants to read the size of any read-write/read-only Security parameter should call 
this API with the following information, after the firmware is downloaded.

The user is expected to provide valid values for the u32SecObjId in the payload. 
Once the Get parameter size request is placed, the application is expected to receive the 
get parameter size response through the API callback registered with the library. 

The required security object size is taken from the security get parameter size response payload, 
size should be greater than zero.

Refer ENUM for Security Parameter IDs: #_SEC_PARAM_ID_LIST

Payload: #_SEC_PARAMETER_INFO

Returns: 
	- On Sucess callback function is invoked with the above payload
	- ERROR_SEC_OBJECT_NOTFOUND : If library could not find specified Security object
*/
#define SUBTYPE_SECURITY_GET_PARAM_SIZE_REQ			3 /* App -> Library */

/**
@ingroup SEC_Api

The baseband responds back to the get parameter size request, the library calls the application’s 
registered callback function. This notification is issued only to the application 
that has issued a management information base get request. 

Payload: #_SEC_PARAMETER_INFO

*/
#define SUBTYPE_SECURITY_GET_PARAM_SIZE_RESP			3 /* Library -> App */

/**
@ingroup SEC_Api
Application can use this API to read the IMEI data. IMEI (International Mobile Equipment Identity) is unique number
which is factory programmed.
This is a synchronous API where Application need to pass the payload of #_IMEI_INFO

Payload: #_IMEI_INFO

Returns: 
	- SUCCESS : If IMEI read is successful.
	- ERROR_EEPROM_NOT_ACCESSIBLE: if Parameter is not available in the device.

*/
#define SUBTYPE_GET_IMEI_DATA							4
/**
@ingroup OMADM_Calib_Api

OMA-DM Calibration related APIs. These APIs are blocking/synchronous. There will be no callback APIs associated with this
group of APIs. 
*/

#define TYPE_OMADM_CALIBRATION								35
/**
@ingroup OMADM_Calib_Api
Calibration Application can use this API to correct the OMA-DM Checksum.

Payload: None

Returns: 
	- SUCCESS: If OMA DM Checksum update is successful
	- ERROR_EEPROM_NOT_ACCESSIBLE: If Parameter is not available in the device

*/
#define SUBTYPE_SET_OMA_DM_INIT_DATA								1

/**
@ingroup Half_Mini_Card
Half Mini Card related APIs defined below should be used with programmed EEPROM with proper GPIO mapped for HMC support.

For HMC support, the application:
 - Gets the HMC shutdown indication #SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION when GPIO is activated
 - Sends HMC Status request after definite time interval using #SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST API
 - Will receive wakeup notification or shutdown indication from firmware as a response to 
   #SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST.

feature support info:
        -  This is supported on custom platform only. Check with Beceem technical team before usage.
*/

#define TYPE_HALF_MINI_CARD				     					36
/**
@ingroup Half_Mini_Card
This API can be used by application to check status of half mini card. 
#SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION from baseband modem is the trigger for application to query for half mini card status. By requesting for the card status with this API,
application places request on baseband modem to check for half mini card status. Baseband modem responds back with appropriate indication(#SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION 
or #SUBTYPE_HALF_MINI_CARD_WAKEUP_RESPONSE)to provide status.

Payload: None
Returns: 
	- SUCCESS: If request is sent to baseband modem successfully
	- ERROR_DEVICE_NOT_IN_HMC_SHUTDOWN: Device is not in half mini card shutdown status
*/
#define SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST					1
/**
@ingroup Half_Mini_Card
The application is called back with this indication as a response to #SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST or 
when GPIO of HMC is activated.

Payload: None
*/
#define SUBTYPE_HALF_MINI_CARD_SHUTDOWN_INDICATION				2
/**
@ingroup Half_Mini_Card
As a response to #SUBTYPE_HALF_MINI_CARD_STATUS_REQUEST, application is called back with this indication 
when modem has woken up from HMC shutdown.

Payload: None
*/
#define SUBTYPE_HALF_MINI_CARD_WAKEUP_RESPONSE					3

/**
@ingroup FLASH_ACCESS_APIs
These APIs are used for accessing Flash map contents or updating the flash section.
These are synchronous APIs i.e. these APIs are of blocking type and there are no callbacks associated with
any of these flash APIs.
*/
#define TYPE_FLASH_API											37

/**
@ingroup FLASH_ACCESS_APIs
This API is called by an application interested in reading from a section #_E_DYNAMIC_SECTION_VAL from
a specified offset and number of bytes mentioned in the payload.

#_DYNAMIC_FLASH_READWRITE_ST::pDataBuff should be allocated by the application for number of bytes mentioned.

Payload: #_DYNAMIC_FLASH_READWRITE_ST

*/
#define SUBTYPE_READ_FLASH_SECTION_REQ							1

/**
@ingroup FLASH_ACCESS_APIs
This API is called by an application interested in writing to a section #_E_DYNAMIC_SECTION_VAL at 
a specific offset and number of bytes mentioned in the payload.

#_DYNAMIC_FLASH_READWRITE_ST::pDataBuff should be allocated by the application for number of bytes mentioned.

Payload: #_DYNAMIC_FLASH_READWRITE_ST

*/
#define SUBTYPE_WRITE_FLASH_SECTION_REQ							2

/**
@ingroup FLASH_ACCESS_APIs
This API provides version and other relavent information related to flash map.
Payload: #_DYNAMIC_FLASH_CS_INFO
*/
#define SUBTYPE_GET_FLASH_VERSION_INFO_REQ						3

/**
@ingroup FLASH_ACCESS_APIs
This API is used to copy from #_ST_FLASH2X_COPY_SECTION::eSrcSection to #_ST_FLASH2X_COPY_SECTION::eDstSection 
from specified offset and number of bytes.

Payload: #_ST_FLASH2X_COPY_SECTION
*/
#define SUBTYPE_COPY_FLASH_SECTION_REQ							4

/**
@ingroup FLASH_ACCESS_APIs
This API provides bit map information for each of the sections present on flash.

Bit field significance:
	- bit 0: section is present. 1 if section is present and 0 if not present
	- bit 1: section is valid. 1 if section is valid and 0 if invalid
	- bit 2: write disabled status. 1 if disable and 0 if enabled
	- bit 3: Section is active. 1 if active and 0 if inactive
	- bit 4 to bit 7: reserved


Payload: #_ST_FLASH2X_BITMAP
*/
#define SUBTYPE_GET_FLASH_SECTION_BITMAP_REQ					5

/**
@ingroup FLASH_ACCESS_APIs
This API is called to set any one of the DSDs as active DSD. Valid only for DSD0,DSD1 and DSD2. 

Payload: #_E_DYNAMIC_SECTION_VAL
*/
#define SUBTYPE_SET_ACTIVE_SECTION_REQ							6

/**
@ingroup FLASH_ACCESS_APIs
This API is called to read raw bytes on flash.

Payload: #_ST_FLASH_RAW_ACCESS_
*/
#define SUBTYPE_READ_FLASH_REQ							7

/*
Used for enabling logging of control messages.
*/
#define TYPE_LOGGING_INFO										38
/*
Request to enable/disable/get status of control messages.
*/
#define SUBTYPE_CONTROLMSGS_LOGGING_REQ							1
/*
Notification with control message information
*/
#define SUBTYPE_CONTROLMSGS_LOGGING_NOTIFICATION				2


/*
DYNAMIC_LOGGING_APIs

These APIs are used to enable dynamic logging for current process/process by PID/all the
processes using library by selecting attributes for logging. 
*/
#define TYPE_LOGGING_CONTROL									39

/*
An application using this API will be able to:

 - Select logging of particular process(by PID). 
 - Select paths of library where logging is to be enabled/disabled based on bit mask.
 - Select where to log, i.e., to console/log-file/syslog. 

Payload : #_ST_DYNAMIC_LOGGING 
*/
#define SUBTYPE_LOGGING_SET										 1

/*
Application can read the logging attributes of various processes with this API.

Payload : #_ST_DYNAMIC_LOGGING 
*/
#define SUBTYPE_LOGGING_GET										 2

/**
@ingroup RESYNC_LOG_APIs
These APIs are used to enable or disable resync logs captured by baseband. 
*/
#define TYPE_RESYNC_LOG_CONTROL									40
/**
@ingroup RESYNC_LOG_APIs

To enable resync logging.

Payload: None 
*/
#define SUBTYPE_RESYNC_ENABLE_REQ                               1

/**
@ingroup RESYNC_LOG_APIs

In response to #SUBTYPE_RESYNC_ENABLE_REQ, application receives this callback which 
indicates that resync logging is enabled.

Payload: None  
*/
#define SUBTYPE_RESYNC_ENABLE_RESP                              1
/**
@ingroup RESYNC_LOG_APIs

To disable resync logging.

Payload: None 
*/
#define SUBTYPE_RESYNC_DISABLE_REQ                               2
/**
@ingroup RESYNC_LOG_APIs

In response to #SUBTYPE_RESYNC_DISABLE_REQ, application receives this callback which 
indicates that resync logging is enabled.

Payload: None 
*/
#define SUBTYPE_RESYNC_DISABLE_RESP                              2

/**
@ingroup RESYNC_LOG_APIs
Notification to application when resync log is created. 

Payload: #_ST_LOG_NOTIFY_BUFFER
*/
#define SUBTYPE_RESYNC_DISCONNECT_LOG_NOTIFICATION                             3



/**
@ingroup CID_SNOOPING_LOG_APIs
These APIs are used to enable or disable CID logs captured by baseband. 
*/
#define TYPE_CID_TRACKING_LOG_CONTROL									41

/**
@ingroup CID_SNOOPING_LOG_APIs

To enable CID logging.

Payload: None 
*/
#define SUBTYPE_CID_ENABLE_REQ                               1

/**
@ingroup CID_SNOOPING_LOG_APIs

In response to #SUBTYPE_CID_ENABLE_REQ, application receives this callback which 
indicates that CID logging is enabled.

Payload: None  
*/
#define SUBTYPE_CID_ENABLE_RESP                              1
/**
@ingroup CID_SNOOPING_LOG_APIs

To disable CID logging.

Payload: None 
*/
#define SUBTYPE_CID_DISABLE_REQ                               2
/**
@ingroup CID_SNOOPING_LOG_APIs

In response to #SUBTYPE_CID_DISABLE_REQ, application receives this callback which 
indicates that CID logging is enabled.

Payload: None 
*/
#define SUBTYPE_CID_DISABLE_RESP                              2

/**
@ingroup CID_SNOOPING_LOG_APIs
Notification to application when CID log is created. 

Payload: #_ST_LOG_NOTIFY_BUFFER
*/
#define SUBTYPE_CID_LOG_NOTIFICATION                             3


/**
@ingroup Hibernate_Options
All the hibernation related requests and notifications are grouped under this API

Hibernation Sequence Diagram
@image html Hibernation_Sequence_Diagram.jpg

*/

#define TYPE_HIBERNATE							42

/**
@ingroup Hibernate_Options
Application can initiate the baseband to go to hibernate mode. 
To achieve a System level hibernate, an application will send this message to
API. This will initiate system level hibernation of the device.

*/
#define SUBTYPE_HIBERNATE_REQ						1 /* App -> DLL */

/**
@ingroup Hibernate_Options
This notification is issued by the baseband modem to all the applications just before entering
the hibernate state.

*/
#define SUBTYPE_HIBERNATE_RESP						1 /*  Firwmare -> DLL -> App */
/**
@ingroup Hibernate_Options
The baseband chip can come out from hibernation mode by performing
complete wakeup procedure. The baseband issues a notification to application to signal its previous status.
This notification is issued to all the applications that have registered a
callback with the library. 
Payload: NULL.

*/
#define SUBTYPE_HIBERNATION_WAKE_UP_IND			2 /* Firwmare -> DLL -> App */


/**
@ingroup PROXIMITY_SENSOR_APIs
API group for Proximity sensor related controls and notifications.

*/

#define TYPE_PROXIMITY_SENSOR							43

/**
@ingroup PROXIMITY_SENSOR_APIs
All host application requests to control or get status of the proximity sensor should use this API.
Please refer to payload for details.
Payload: #_ST_PROXIMITY_CONTROL.
*/
#define SUBTYPE_PROXIMITY_FUNCTIONS_REQ						1 /* App -> DLL */


/**
@ingroup PROXIMITY_SENSOR_APIs
All response and notification related to the proximity sensor would come on this API.
Please refer to payload for details.
Payload: #_ST_PROXIMITY_CONTROL.

*/
#define SUBTYPE_PROXIMITY_FUNCTIONS_RESP					1 /* DLL -> APP */




#endif /* #ifdef BECEEM_MESSAGES_H */
