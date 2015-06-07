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
 * Description	:	Beceem MIB Definitions
 * Author		:
 * Reviewer		:
 *
 */


#ifndef _BCM_MIB_USER_API_H_
#define _BCM_MIB_USER_API_H_

/*Standard 80216f Read/Write (Config)MIBS */
/* Range 0 - 499 */

/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Time since last received DL-MAP message before downlink synchronization is considered lost in ms

Length: UINT16

Default: 600 ms

Range: 0-600
*/
#define	WMAN_IF_SS_LOSTDLMAPINTERVAL			0
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Time since last received UL-MAP message before uplink synchronization is considered lost in ms

Length: UINT16

Default: 600 ms

Range: 0-600
*/
#define WMAN_IF_SS_LOSTULMAPINTERVAL			1
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host

Description: 
Number of retries on contention Ranging Requests

Length: UINT16

Default: 16

Range: 16-65535
*/
#define WMAN_IF_SS_CONTENTIONRANGRETRIES		2
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 


Description: 
Number of retries on bandwidth allocation requests

Length: UINT16

Default: 16

Range: 16-65535
*/
#define WMAN_IF_SS_REQUESTRETRIES				3
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Number of retries on registration requests

Length: UINT16

Default: 3

Range: 3-65535
*/
#define WMAN_IF_SS_REGREQUESTRETRIES			4
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for DCD timeout in ms

Length: UINT16

Default: 50,000

Range: NA
*/
#define WMAN_IF_SS_T1TIMEOUT					5
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for broadcast ranging timeout in ms

Length: UINT16

Default: 1000

Range: 0-10,000
*/
#define WMAN_IF_SS_T2TIMEOUT					6
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Ranging response reception timeout following the transmission of a Ranging Request in ms

Length: UINT8

Default: 60

Range: 0-200
*/
#define WMAN_IF_SS_T3TIMEOUT					7

/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Periodic ranging timer in seconds

Length: UINT8

Default: 5 

Range: 0-35
*/
#define WMAN_IF_SS_T4TIMEOUT					8
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for registration response in ms

Length: UINT16

Default: 1000

Range: 0-3000
*/
#define WMAN_IF_SS_T6TIMEOUT					9
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for UCD descriptor in ms
Length: UINT16

Default: 50,000

Range: 0-50,000
*/
#define WMAN_IF_SS_T12TIMEOUT					10
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for DSX-RVD Timeout in ms

Length: UINT8

Default: 200

Range: 0-200
*/
#define WMAN_IF_SS_T14TIMEOUT					11
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
wait for SBC-RSP timeout in ms

Length: UINT16

Default: 50

Range: 0-300
*/
#define WMAN_IF_SS_T18TIMEOUT					12
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Time SS searches for preambles on a given channel in ms

Length: UINT16

Default: NA

Range: NA
*/
#define WMAN_IF_SS_T20TIMEOUT					13
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Time SS searches for DL-MAP on a given channel in ms

Length: UINT16

Default: 1000

Range: 0-10000
*/
#define WMAN_IF_SS_T21TIMEOUT					14
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Number of retries on SBC request

Length: UINT8

Default: 16

Range: 3-16
*/
#define WMAN_IF_SS_SBCREQUESTRETRIES			15

/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Number of Invited Range Retries

Length: UINT16

Default: 0

Range: NA
*/
#define WMAN_IF_SS_INVITEDRANGRETRIES			16

/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Number of timeout retries on DSA/DSC/DSD requests

Length: UINT32

Default: 3

Range: NA
*/
#define	WMAN_IF_CMN_DSXREQRETRIES				17
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs


MIB Category:
	Readable and writable from the host 

Description: 
Number of timeout retries on DSA/DSC/DSD responses

Length: UINT32

Default: 3

Range: NA
*/
#define WMAN_IF_CMN_DSXRESPRETRIES				18
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for DSA/DSC/DSD response timeout in ms

Length: UINT16

Default: 1000

Range: 0-1000
*/
#define	WMAN_IF_CMN_T7TIMEOUT					19
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for DSA/DSC/DSD acknowledge timeout in ms

Length: UINT16

Default: 300

Range: 0-300
*/
#define WMAN_IF_CMN_T8TIMEOUT					20
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for Transaction End timeout in ms

Length: UINT16

Default: 500

Range: 0-3000
*/
#define WMAN_IF_CMN_T10TIMEOUT					21
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Wait for ARQ Reset in ms 

Length: UINT16

Default: 150

Range: 0-500
*/
#define WMAN_IF_CMN_T22TIMEOUT					22
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Read-only from the host 

Description: 
The value of this object is the data encryption algorithm for this cryptographic suite capability

Key Value:
The key is Crypto suite index in range 0-1

Length: UINT8

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_CRYPTOSUITEDATAENCRYPTALG	23
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Read-only from the host 

Description: 
The value of this object is the data authentication algorithm for this cryptographic suite capability

Key Value:
The key is Crypto suite index in range 0-1

Length: UINT8

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_CRYPTOSUITEDATAAUTHENTALG	24
/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Read-only from the host 

Description: 
The value of this object is the TEK key encryption algorithm for this cryptographic suite capability

Key Value:
The key is Crypto suite index in range 0-1

Length: UINT8

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_CRYPTOSUITETEKENCRYPTALG	25

/**
@ingroup mib_config_proprietary

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
    Network search periodic wakeup timer in milliseconds

Length: UINT32

Default: 60000(1min)

Range: >= 60000
*/
#define BCM_CFG_NETSEARCH_PERIODIC_WAKEUP_TIMER 26

/**
@ingroup mib_standard_config

Type: Standard Configuration MIBs

MIB Category:
	Readable and writable from the host 

Description: 
Mobility scan request retransmission timer.

Length: UINT32

Default: 160 ms

Range: NA
*/
#define WMAN_IF_2M_SS_T44TIMER 27

/* Beceem Propreitary Config MIBS Defines */
/*Range 500 - 699*/
/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs

MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS

Description: 
Setting this value to a certain frequency will force the baseband to scan only that frequency. Default value of 0 will cause baseband to scan all frequencies 

Length: UINT32

Default: 0

Range: frequency values (eg 2345 for 2345 GHz)
*/
#define BCM_CFG_FIXED_CENTER_FREQUENCY  500


/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs

MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS

Description: 
Enable/Disable ARQ support

Length: UINT32

Default: 1

Range: 0=Disable
	   1=Enable

*/
#define BCM_CFG_ARQ_ENABLE				501

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs

MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS

Description: 
To enable the hand off, set it to 1 and to disable hand off, set it to 0

Length: UINT32

Default: 0

Range: NA
1 - HO enabled
0 - HO disabled
*/
#define BCM_CFG_ENABLE_HO				502
/*
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	

MIB Category:
	This MIB can be used to read specific address location from the device.

Description: 
	Address location to read. Reads 4 bytes from the location specified
	IN : Valid Addresss location readable from host
	OUT: payload of length #_ST_RDM_MSG
Length: UINT32

Default: NA

Range: 

*/
#define BCM_CFG_RDM_REQ					503
/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS

Description: 
Enable/Disable Auto SCAN support

Length: UINT32

Default: 0

Range: 0=Disable, 
	   1=Enable

*/
#define BCM_CFG_AUTO_SCAN_ENABLE		504
/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS

Description: 
Enable/Disable PKM v2 security

Length: UINT32

Default: 0

Range: 0=Disable, 
	   1=Enable

*/
#define BCM_CFG_PKMv2_SECURITY_ENABLE		505

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	This is write only MIB and is used to reset the BER counter

Description: 
Resets the BER counters when value is set to 0

Length: UINT32

Default: 0x0BADFACE

Range: NA
*/
#define BCM_CFG_RESET_BER_COUNTER		506

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	This is write only MIB and is used to reset the MRU table
	
Description: 
	Resets the MRU entries in the EEPROM of the baseband
	No response is sent to the HOST application for this request

Length: NA

Default: NA

Range: NA
*/
#define BCM_BASEBAND_RESET_MRU_TABLE	507

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	These MIBs are readable and writable from the host and they contain load time configuration parameters for the SS
	
Description: 
	The bandwidth information of the baseband in configuration
	
Length: UINT32

Default: NA

Range: NA (e.g. 5 MHz, 7 MHz, 8.5 MHz, 10 MHz)
*/
#define BCM_CFG_FIXED_BANDWIDTH	508


/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	This is write only MIB and is used to add a frequency to the MRU table
	
Description: 
	Add this frequency to the MRU list. No response is sent to the HOST application for this request


Length: #_ST_MRU_ADD_INFO

Default: NA

Range: NA
*/
#define BCM_BASEBAND_ADD_MRU        				509

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	These MIBs are readable and writable from the host

Description: 
This MIB is used to set trigger type for WiMAX coverage. Trigger types available are:
 - 0 for TRIGGER_TYPE_NOT_DEFINED
 - 1 for Trigger which gives higher weight to RSSI
 - 2 for Trigger which gives higher weight to CINR 

The trigger threshold is set by MIB #BCM_CFG_WIMAX_TRIGGER_THRESHOLD and trigger status can be 
queried by #BCM_BASEBAND_TRIGGER_STATUS_INFO. 

Length: UINT32

Default: NA 

Range: 0-2
*/

#define BCM_CFG_WIMAX_TRIGGER_TYPE					510

/**
@ingroup mib_proprietary_config

Type: Beceem Proprietary Configuration MIBs
	
MIB Category:
	These MIBs are readable and writable from the host

Description: 
This MIB is used to set threshold for trigger type selected by #BCM_CFG_WIMAX_TRIGGER_TYPE. 
The trigger status can by queried by #BCM_BASEBAND_TRIGGER_STATUS_INFO.


Length: INT32

Default: NA

Range: 0-100
*/

#define BCM_CFG_WIMAX_TRIGGER_THRESHOLD				511



/* Standard 80216f Read (Stats)MIBS */
/* Range 800 - 1299 */
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the state of the MS
authorization FSM(finite state machine). The start state indicates that FSM is
in its initial state.

Length: UINT32

Default: NA

Range: start(1), authWait(2), authorized(3), reauthWait(4), authRejectWait(5), silent(6)

*/
#define WMAN_IF_SS_PKMAUTHSTATE						800
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the most recent authorization
key sequence number for this FSM(finite state machine)

Length: UINT32

Default: NA

Range: 0-15

*/
#define WMAN_IF_SS_PKMAUTHKEYSEQUENCENUM                        801

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the actual clock time for
expiration of the immediate predecessor of the most recent
authorization key for this FSM. If this FSM has only one
authorization key, then the value is the time of activation
of this FSM.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHEXPIRESOLD                            802

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this object is the actual clock time for
expiration of the most recent authorization key for this
FSM.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHEXPIRESNEW                            803


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read Write from the host?

Description: 
PKM authentication reset parameter

Length: UINT32

Default: NA

Range: NA
*/

#define WMAN_IF_SS_PKMAUTHRESET						804
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the count of times the MS has
transmitted an Authentication Information message

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHENTINFOS                            805
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the count of times the MS has
transmitted an Authentication Information message

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHREQUESTS                                      806
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the count of times the MS has
received an Authorization Reply message

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHREPLIES                                      807
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the count of times the MS has
received an Authorization Reject message

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHREJECTS                                       808

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the count of times the MS has
received an Authorization Invalid message

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHINVALIDS                                       809


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the enumerated description of
the Error-Code in most recent Authorization Reject message
received by the MS. This has value unknown(2) if the last
Error-Code value was 0, and none(1) if no Authorization
Reject message has been received since reboot.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHREJECTERRORCODE                                       810

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this object is the Display-String in most
recent Authorization Reject message received by the SS.
This is a zero length string if no Authorization Reject
message has been received since reboot.

Length: UINT8*128

Default: NA

Range: NA

*/
#define		WMAN_IF_SS_PKMAUTHREJECTERRORSTRING                     811
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this object is the enumerated description of
the Error-Code in most recent Authorization Reject message
received by the MS. This has value unknown(2)if the last
Error-Code value was 0, and none(1) if no Authorization
Reject message has been received since reboot.

Length: UINT32

Default: NA

Range: NA

*/
#define		WMAN_IF_SS_PKMAUTHINVALIDERRORCODE                     812
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this object is the Display-String in most
recent Authorization Invalid message received by the MS.
This is a zero length string if no Authorization Invalid
message has been received since reboot.

Length: UINT8*128

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHINVALIDERRORSTRING            813


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the grace time for an
authorization key. A SS is expected to start trying to get
a new authorization key beginning AuthGraceTime seconds
before the authorization key actually expires.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHGRACETIME                                     814
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the grace time for the TEK in
seconds. The SS is expected to start trying to acquire a
new TEK beginning TEK GraceTime seconds before the
expiration of the most recent TEK.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMTEKGRACETIME                                     815
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the Authorize Wait Timeout

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHWAITTIMEOUT                                      816
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the Reauthorize Wait Timeout
in seconds

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMREAUTHWAITTIMEOUT                                       817
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the Operational Wait Timeout
in seconds

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMOPWAITTIMEOUT                                       818
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the Rekey Wait Timeout in
seconds

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMREKEYWAITTIMEOUT                                       819
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The value of this MIB is the Authorization Reject Wait
timeout in seconds.
Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_PKMAUTHREJECTWAITTIMEOUT                                       820
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The number of UL-MAPs to receive before contention-based
reservation is attempted again for the same connection.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMCTBASEDRESVTIMEOUT                                       821
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Size (in units of PS) of PHY payload that MS may use to
format and transmit a bandwidth request message in a
contention request opportunity. The value includes all
PHY overhead as well as allowance for the MAC data the
message may hold.

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMBWREQOPPSIZE                                       822

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Size (in units of PS) of PHY payload that MS may use to
format and transmit a RNG-REQ message in a contention
request opportunity. The value includes all PHY overhead
as well as allowance for the MAC data the message may
hold and the maxiumum SS/BS roundtrip propagation delay.
Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMRANGREQOPPSIZE                           823
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Uplink center frequency (kHz)

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMUPLINKCENTERFREQ                         824
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Number of subchannels used by each transmit
opportunity when REQ Region-Full is allocated in
subchannelization region. Possible values are:
oneSubchannel(0),
twoSubchannels(1),
fourSubchannels(2),
eightSubchannels(3),
sixteenSubchannels(4).

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMNUMSUBCHREQREGIONFULL            825
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Number of OFDM symbols used by each transmit
opportunity when REQ Region-Full is allocated in
subchannelization region.

Length: UINT32

Default: NA

Range: 0-31

*/
#define WMAN_IF_SS_OFDMNUMSYMBOLSREQREGIONFULL            826

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Number of contention codes (CSE) that shall only be used to
request a subchannelized allocation. Default value 0.
Allowed values 0-8

Length: UINT32

Default: NA

Range: 0-8

*/
#define WMAN_IF_SS_OFDMSUBCHFOCUSCTCODE            827

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The identifier of the uplink channel to which this
message refers.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMUPLINKCHANNELID            828

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The EIRP is the equivalent isotropic radiated power of
the base station, which is computed for a simple
single-antenna transmitter.

Length: UINT32

Default: NA

Range: 0-65535

*/
#define WMAN_IF_SS_OFDMBSEIRP            829

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Downlink channel number as defined in 8.5.
Used for license-exempt operation only

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMCHANNELNUMBER            830
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Transmit / Receive transition gap

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMTTG             831
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Receive/Transmit transition gap

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMRTG            832
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Initial ranging maximum received signal strength at BS
Signed in units of 1 dBm

Length: UINT32

Default: NA

Range: 0-65535

*/
#define WMAN_IF_SS_OFDMINITRNGMAXRSS            833
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Downlink center frequency (kHz).

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMDOWNLINKCENTERFREQ            834
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Base station ID

Length: UINT8 * 8

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMBSID             835
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This parameter specifies the version of 802.16 to which
the message originator conforms

Length: UINT32

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMMACVERSION            836
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The duration of the frame. The frame duration code
values are specified in WiMAX standards.

Length: UINT32

Default: NA

Range: 0-6

*/
#define WMAN_IF_SS_OFDMFRAMEDURATIONCODE             837
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The identifier of the downlink channel to which this
message refers.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMDOWNLINKCHANNELID            838
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
The number of UL-MAPs to receive before contention-based
reservation is attempted again for the same connection.

Length: UINT32

Default: NA

Range: 1-255

*/
#define WMAN_IF_SS_OFDMACTBASEDRESVTIMEOUT            839
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Size (in units of PS) of PHY payload that SS may use to
format and transmit a bandwidth request message in a
contention request opportunity. The value includes all
PHY overhead as well as allowance for the MAC data the
message may hold.

Length: UINT32

Default: NA

Range: 1-65535

*/
#define WMAN_IF_SS_OFDMABWREQOPPSIZE             840
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Size (in units of PS) of PHY payload that SS may use to
format and transmit a RNG-REQ message in a contention
request opportunity. The value includes all PHY overhead
as well as allowance for the MAC data the message may
hold and the maxiumum SS/BS roundtrip propagation delay.

Length: UINT32

Default: NA

Range: 1-65535

*/
#define WMAN_IF_SS_OFDMARANGREQOPPSIZE              841

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Uplink center frequency (kHz)

Length: UINT32

Default: 0

Range: 0-4294967295
*/
#define WMAN_IF_SS_OFDMAUPLINKCENTERFREQ			842
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
Number of initial ranging CDMA codes. Possible values are 0 to 255. The total number of wmanIfSsOfdmaInitRngCodes, 
wmanIfSsOfdmaPeriodicRngCodes and wmanIfSsOfdmaBWReqCodes shall be equal or less than 256.

Length: UINT32

Default: 30

Range: 0-255
*/
#define WMAN_IF_SS_OFDMAINITRNGCODES				843
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
Number of periodic ranging CDMA codes. Possible values are 0 to 255. The total number of wmanIfSsOfdmaInitRngCodes, 
wmanIfSsOfdmaPeriodicRngCodes and wmanIfSsOfdmaBWReqCodes shall be equal or less than 256.

Length: UINT32

Default: 30

Range: 0-255
*/
#define WMAN_IF_SS_OFDMAPERIODICRNGCODES			844
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host
    
Description: 
Number of periodic ranging CDMA codes. Possible values are 0 to 255. The total number of wmanIfSsOfdmaInitRngCodes, 
wmanIfSsOfdmaPeriodicRngCodes and wmanIfSsOfdmaBWReqCodes shall be equal or less than 256.

Length: UINT32

Default: 30

Range: 0-255
*/
#define WMAN_IF_SS_OFDMABWREQCODES					845
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host
    
Description: 
Initial backoff window size for periodic ranging contention, expressed as a power of 2

Length: UINT32

Default: 0

Range: 0-15
*/
#define WMAN_IF_SS_OFDMAPERRNGBACKOFFSTART			846
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host


Description: 
Final backoff window size for periodic ranging contention, expressed as a power of 2

Length: UINT32

Default: 15

Range: 0-15
*/
#define WMAN_IF_SS_OFDMAPERRNGBACKOFFEND			847
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
Indicates the starting number, S, of the group of codes used for this uplink. All the ranging codes used on this uplink 
will be between S and ((S+N+M+L) mod 256) Where, N is the number of initial-ranging codes, M is the number of 
periodic-ranging codes, and L is the number of bandwidth-request codes. The range of values is 0 <= S <= 255.

Length: UINT32

Default: 0

Range: 0-255
*/
#define WMAN_IF_SS_OFDMASTARTOFRNGCODES				848
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Determines the UL_IDcell parameter for the subcarrier permutation to be used on this uplink channel

Length: UINT32

Default: 0

Range: 0-255
*/
#define WMAN_IF_SS_OFDMAPERMUTATIONBASE				849

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This is a bitmap describing the sub-channels allocated
to the segment in the UL, when using the uplink PUSC
permutation. The LSB of the first byte shall correspond to
subchannel 0. For any bit that is not set,
the corresponding subchannel shall not be used by the SS
on that segment.

Length: UINT8*12

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMAULALLOCSUBCHBITMAP            850
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This is a bitmap describing the sub-channels allocated to
the segment in the UL, when using the uplink optional PUSC
permutation (see 8.4.6.2.5 in IEEE 802.16-2004). The LSB
of the first byte shall correspond to subchannel 0. For any
bit that is not set, the corresponding subchannel shall not
be used by the SS on that segment.

Length: UINT8*16

Default: NA

Range: NA

*/
#define WMAN_IF_SS_OFDMAOPTPERMULALLOCSUBCHBITMAP             851
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band AMC allocation
threshold.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDAMCALLOCTHRESHOLD             852
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band AMC release
threshold.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDAMCRELEASETHRESHOLD             853
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band AMC allocation
timer.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDAMCALLOCTIMER            854
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band AMC release
timer.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDAMCRELEASETIMER                     855
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band status reporting
maximum period

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDSTATREPMAXPERIOD            856
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA band AMC retry
timer

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMABANDAMCRETRYTIMER                       857

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
Number of OFDM symbols used by each transmit
opportunity when REQ Region-Full is allocated in
subchannelization region.

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMASAFETYCHALLOCTHRESHOLD          858
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA safety channel release
threshold

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMASAFETYCHRELEASETHRESHOLD        859
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA safety channel allocation
timer

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMASAFETYCHALLOCTIMER                      860

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS

MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA safety channel release
timer

Length: UINT32

Default: NA

Range: 0-255

*/
#define WMAN_IF_SS_OFDMASAFETYCHRELEASETIMER            861
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA bin status reporting maximum period

Length: UINT32

Default: 0

Range: 0-255
*/

#define WMAN_IF_SS_OFDMABINSTATREPMAXPERIOD			862

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA safety channel retry
timer

Length: UINT32

Default: 0

Range: 0-255
*/
#define WMAN_IF_SS_OFDMASAFETYCHARETRYTIMER                     863


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA HARQ ACK delay for UL burst.
1 = one frame offset
2 = two frames offset
3 = three frames offset

Length: UINT32

Default: 0

Range: NA
*/
#define WMAN_IF_SS_OFDMAHARQAACKDELAYULBURST            864


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
This object defines the OFDMA CQICH band AMC transition
delay
Length: UINT32

Default: 0

Range: 0-255
*/
#define WMAN_IF_SS_OFDMACQICHBANDAMCTRANADELAY          865

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
The EIRP is the equivalent isotropic radiated power of the base station, which is computed for a simple 
single-antenna transmitter

Length: UINT32

Default: NA

Range: 0-65535
*/
#define WMAN_IF_SS_OFDMABSEIRP						866
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Read only from the host

Description: 
Downlink channel number as defined in 8.5. Used for
license-exempt operation only.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMACHANNELNUMBER                           867


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Transmit/Receive transition gap

Length: UINT32

Default: NA

Range: 0-65536
*/
#define WMAN_IF_SS_OFDMATTG							868
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Receive/Transmit transition gap

Length: UINT32

Default: NA

Range: 0-255
*/
#define WMAN_IF_SS_OFDMARTG							869
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Initial ranging maximum received signal strength at BS signed in units of 1 dBm

Length: UINT32

Default: NA

Range: 0-65535
*/
#define WMAN_IF_SS_OFDMAINITRNGMAXRSS				870
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Downlink center frequency (kHz)

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMADOWNLINKCENTERFREQ			871
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Base station ID, first 6 bytes contain base station identifier and the remaining bytes to be 
ignored.

Length: 8 bytes

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMABSID						872
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This parameter specifies the version of 802.16 to which the message originator conforms

Length: UINT32

Default: 
		 - ieee802Dot16Of2001  : 1,
		 - ieee802Dot16cOf2002 : 2,
		 - ieee802Dot16aOf2003 : 3,
		 - ieee802Dot16Of2004  : 4,
		 - ieee802Dot16Of2004  : 5,
		 - ieee802Dot16Of2004  : 6.

Range: NA
*/
#define WMAN_IF_SS_OFDMAMACVERSION					873
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
The duration of the frame 

Length: UINT32

Default: NA

Range: 0-8
*/
#define WMAN_IF_SS_OFDMAFRAMEDURATIONCODE			874
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This object defines the size of CQICH ID field 0 = Reserved 1 = 3 bits 2 = 4 bits 3 = 5 bits 4 = 6 bits 5 = 7 bits 
6 = 8 bits 7 = 9 bits 8255 = Reserved

Length: UINT32

Default: NA

Range: 0-7
*/
#define WMAN_IF_SS_OFDMASIZECQICHIDFIELD			875

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This object defines the OFDMA HARQ ACK delay for DL burst
1 = one frame offset
2 = two frames offset
3 = three frames offset

Length: UINT32

Default: NA

Range: 1-3
*/
#define WMAN_IF_SS_OFDMAHARQAACKDELAYBURST			876
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the FFT sizes supported by SS.
The usage is defined by WmanIfOfdmFftSizes.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMREQCAPFFTSIZES                        877

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the different demodulator options
supported by SS for downlink.
The usage is defined by WmanIfOfdmSsDeModType.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMREQCAPSSDEMODULATOR           878
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the different modulator options
supported by SS for uplink. The usage is defined by WmanIfOfdmSsModType.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMREQCAPSSMODULATOR                     879

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates whether the SS supports focused
contention. The usage is defined by WmanIfOfdmFocusedCt.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMREQCAPFOCUSEDCTSUPPORT			880
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates whether or not the SS supports
the TC sublayer. The usage is defined by
WmanIfOfdmTcSublayer.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMREQCAPTCSUBLAYERSUPPORT 			881
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the FFT sizes negotiated with the
SS. The usage is defined by WmanIfOfdmFftSizes.

Length: UINT32

Default: NA

Range: 0-7
*/
#define WMAN_IF_BS_SS_OFDMRSPCAPFFTSIZES			882
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the different demodulator options
negotiated for SS for downlink. The usage is defined by
WmanIfOfdmSsDeModType

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMRSPCAPSSDEMODULATOR   			883
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates the different modulator options
negotiated for SS for uplink. The usage is defined by
WmanIfOfdmSsModType.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMRSPCAPSSMODULATOR 			884
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates whether the SS has negotiated the
support for focused contention. The usage is defined by
WmanIfOfdmFocusedCt.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMRSPCAPFOCUSEDCTSUPPORT  			885
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This field indicates whether the SS has negotiated
support for the TC sublayer. The usage is defined by
WmanIfOfdmTcSublayer.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_BS_SS_OFDMRSPCAPTCSUBLAYERSUPPORT 			886
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
The MAC address of the SS generating the trap

Length: UINT8*6

Default: NA

Range: NA
*/
#define WMAN_IF_SS_MACADDRESS                                           887

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
Indicating the value of the unknown TLV

Length: UINT8*20

Default: NA

Range: NA
*/
#define WMAN_IF_SS_UNKNOWNTLV                                           888

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This object indicates the dynamic service flow
creation command type.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_DYNAMICSERVICETYPE                           889

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This object indicates the reason why the service flow
creation has failed.

Length: UINT8 * 256

Default: NA

Range: 0-255
*/
#define WMAN_IF_SS_DYNAMICSERVICEFAILREASON                     890

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
A RSSI alarm is generated if the RSSI is lower than
wmanIfSsRssiLowThreshold, or above
wmanIfSsRssiHighThreshold after alarm is restored.

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_RSSISTATUS                                           891

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	DCD channel attributes, defining the transmission characteristics of downlink channels

Description: 
This object provides additional information about RSSI
alarm. It is implementation specific.

Length: UINT8 * 256

Default: NA

Range: NA
*/
#define WMAN_IF_SS_RSSISTATUSINFO                                       892


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Represents the current OFDMA channel identity

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_CHANNELNUMBER					893
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Frame number in which measurement for this channel started

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: 0

Range: 0-65535
*/
#define WMAN_IF_SS_STARTFRAME						894


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Cumulative measurement duration on the channel in
multiples of Ts. For any value exceeding 0xFFFFFF,
report 0xFFFFFF.

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: NA

Range: 0-16777215
*/
#define WMAN_IF_SS_DURATION						895


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Bit #0: WirelessHUMAN detected on the channel
Bit #1: Unknown transmissions detected on the channel
Bit #2: Primary user detected on the channel
Bit #3: Unmeasured. Channel not measured

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_BASICREPORT                        896

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Mean CINR report

Key Value:
The key is histogram index ranging from 0-3

Length: UINT32

Default: 0

Range: 0-31
*/
#define WMAN_IF_SS_MEANCINRREPORT					897
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Standard deviation CINR report

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: 0

Range: 0-41
*/
#define WMAN_IF_SS_STDDEVIATIONCINRREPORT			898

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Mean RSSI report

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: 0

Range: 0-83
*/
#define WMAN_IF_SS_MEANRSSIREPORT			899
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Standard deviation RSSI report

Key Value:
The key is Histogram Index ranging from 0-3

Length: UINT32

Default: 0

Range: 0-83
*/
#define WMAN_IF_SS_STDDEVIATIONRSSIREPORT                       900
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the type of security
association.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: 0

Range: 0-2
*/
#define WMAN_IF_SS_PKMTEKSATYPE                                         901
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the data encryption algorithm
being utilized.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKDATAENCRYPTALG                         902
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the data authentication
algorithm being utilized.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKDATAAUTHENTALG                         903
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the TEK key encryption
algorithm for this cryptographic suite capability.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKENCRYPTALG                                     904
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the state of the indicated TEK
FSM. The start(1) state indicates that FSM is in its
initial state.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKSTATE                                          905
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the most recent TEK key
sequence number for this TEK FSM.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYSEQUENCENUMBER                      906
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the actual clock time for expiration of the immediate predecessor of the most recent
TEK for this FSM. If this FSM has only one TEK, then the
value is the time of activation of this FSM.	

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKEXPIRESOLD                                     907
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the actual clock time for
expiration of the most recent TEK for this FSM.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKEXPIRESNEW                                     908
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the count of times the SS has
transmitted a Key Request message.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYREQUESTS                            909
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the count of times the SS has
received a Key Reply message, including a message whose
authentication failed.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYREPLIES                                     910
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the count of times the SS has
received a Key Reject message, including a message whose
authentication failed.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYREJECTS                                     911
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the count of times the SS has
received a TEK Invalid message, including a message whose
authentication failed.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKINVALIDS                                       912
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the count of times an
Authorization Pending (Auth Pend) event occurred in this
FSM.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKAUTHPENDS                                      913
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the enumerated description of
the Error-Code in most recent Key Reject message received
by the SS. This has value unknown(2) if the last Error-Code
value was 0, and none(1) if no Key Reject message has been
received since reboot.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYREJECTERRORCODE                     914
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the display-string in most
recent key reject message received by the SS. This is a
zero length string if no key reject message has been
received since reboot.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT8*128

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKKEYREJECTERRORSTRING           915

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the enumerated description of
the Error-Code in most recent TEK Invalid message received
by the SS. This has value unknown(2) if the last
Error-Code value was 0, and none(1) if no TEK Invalid
message has been received since reboot.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKINVALIDERRORCODE                       916
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The value of this object is the display-string in most recent TEK Invalid message received by the SS. This is a
zero length string if no TEK Invalid message has been
received since reboot.

Key Value:
The key is SAID which can be queried from #BCM_BASEBAND_SAID_LIST

Length: UINT8*128

Default: NA

Range: NA
*/
#define WMAN_IF_SS_PKMTEKINVALIDERRORSTRING                     917

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Uplink FEC code type and modulation type.

Key Value:
The key is wmanIfSsOfdmUiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMUCDFECCODETYPE                           918
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
The power boost in dB of focused contention carriers, as
described in 802.16 specification.

Key Value:
The key is wmanIfSsOfdmUiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMFOCUSCTPOWERBOOST                        919
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
This parameter determines the transmission convergence
sublayer, as described in 802.16 specification, can be enabled on a
per-burst basis for both uplink and downlink. through
DIUC/UIUC messages.

Key Value:
The key is wmanIfSsOfdmUiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMUCDTCSENABLE                                     920
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Downlink Frequency (kHz).

Key Value:
The key is wmanIfSsOfdmDiucIndex in range 0-10

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMDOWNLINKFREQUENCY                        921
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Downlink FEC code type and modulation type.


Key Value:
The key is wmanIfSsOfdmDiucIndex in range 0-10

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMDCDFECCODETYPE                           922
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
DIUC mandatory exit threshold: 0 - 63.75 dB CINR at or
below where this DIUC can no longer be used and where this
change to a more robust DIUC is required in 0.25 dB units.


Key Value:
The key is wmanIfSsOfdmDiucIndex in range 0-10

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMDIUCMANDATORYEXITTHRESH          923
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
DIUC minimum entry threshold: 0 - 63.75 dB. The minimum CINR
required to start using this DIUC when changing from a more
robust DIUC is required, in 0.25 dB units.


Key Value:
The key is wmanIfSsOfdmDiucIndex in range 0-10

Length: UINT32

Default: 0-255

Range: NA
*/
#define WMAN_IF_SS_OFDMDIUCMINENTRYTHRESH                       924
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Indicates whether transmission convergence sublayer
is enabled or disabled.


Key Value:
The key is wmanIfSsOfdmDiucIndex in range 0-10

Length: UINT32

Default: 0-1

Range: NA
*/
#define WMAN_IF_SS_OFDMTCSENABLE                                        925
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Uplink FEC code type and modulation type.


Key Value:
The key is wmanIfSsOfdmaUiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMAUCDFECCODETYPE                          926

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Reducing factor in units of 1 dB, between the power used
for this burst and power should be used for CDMA Ranging.

Key Value:
The key is wmanIfSsOfdmaUiucIndex in range 0-11

Length: UINT32

Default: 0-255

Range: NA
*/
#define WMAN_IF_SS_OFDMARANGINGDATARATIO                        927
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
This is a list of numbers, where each number is encoded by
one nibble, and interpeted as a signed integer. The nibbles
correspond in order to the list define by Table 574 in IEEE
Std 802.16-Mar2007 starting from the second line, such that
the LS nibble of the first byte corresponds to the second
line in the table. The number encoded by each nibble
represents the difference in normalized C/N relative to the
previous line in the table.

Key Value:
The key is wmanIfSsOfdmaUiucIndex in range 0-11

Length: UINT8*8

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMANORCOVERNOVERRIDE                       928
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Downlink Frequency (kHz).

Key Value:
The key is wmanIfSsOfdmaDiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMADOWNLINKFREQUENCY                       929

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
Downlink FEC code type and modulation type.

Key Value:
The key is wmanIfSsOfdmaDiucIndex in range 0-11

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_SS_OFDMADCDFECCODETYPE                          930

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
DIUC mandatory exit threshold: 0 - 63.75 dB CINR at or
below where this DIUC can no longer be used and where this
change to a more robust DIUC is required in 0.25 dB units.

Key Value:
The key is wmanIfSsOfdmaDiucIndex in range 0-11

Length: UINT32

Default: 0-255

Range: NA
*/
#define WMAN_IF_SS_OFDMADIUCMANDATORYEXITTHRESH         931

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

Description: 
DIUC minimum entry threshold: 0 - 63.75 dB. The minimum CINR
required to start using this DIUC when changing from a more
robust DIUC is required, in 0.25 dB units.

Key Value:
The key is wmanIfSsOfdmaDiucIndex in range 0-11

Length: UINT32

Default: 0-255

Range: NA
*/
#define WMAN_IF_SS_OFDMADIUCMINENTRYTHRESH                      932


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the connection identifier of service flow entry from SS and BS

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-7
*/
#define WMAN_IF_CMN_CPSSFCID						1100
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
An attribute indicating the service flow is downstream or
upstream.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST  

Length: UINT8

Default: NA 

Range: 1-2
*/
#define WMAN_IF_CMN_CPSSFDIRECTION						1101

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the connection identifier of service flow entry from SS and BS

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT8

Default: NA 

Range: 0-7
*/
#define WMAN_IF_CMN_CPSSFSTATE  						1102

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the priority assigned to a service flow. For uplink service flows, 
the BS should use this parameter when determining precedence in request service and grant generation, and 
the SS shall preferentially select contention request opportunities for priority request CIDs based on this priority

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT8

Default: NA 

Range: 0-7
*/
#define WMAN_IF_CMN_CPSTRAFFICPRIORITY				1103
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This parameter defines the peak information rate of the service. The rate is expressed in bits per second and pertains 
to the SDUs at the input to the system.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_CPSMAXSUSTAINEDRATE				1104
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This parameter defines the maximum burst size in bytes that must be accommodated for the service

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSMAXTRAFFICBURST				1105
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This parameter specifies the minimum rate(in bits per sec) reserved for this service flow

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSMINRESERVEDRATE				1106
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This parameter defines the maximum delay variation (jitter) for the connection. The value is in units of milliseconds.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA 
*/
#define WMAN_IF_CMN_CPSTOLERATEDJITTER				1107
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the maximum latency between the reception of a packet by the BS or SS 
on its network interface and the forwarding of the packet to its RF Interface. The value is in units of millisecs.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSMAXLATENCY					1108
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies whether the SDUs on the service flow are variable-length (0) or fixed-length (1) 
The parameter is used only if packing is on for the service flow. The default value is 0, i.e, variable-length SDUs

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-1
*/
#define WMAN_IF_CMN_CPSFIXEDVSVARIABLESDUIND		1109
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the length of the SDU for a fixed-length SDU service flow. This parameter is 
used only if packing is on and the service flow is indicated as carrying fixed-length SDUs. The default value is 
49 bytes, i.e, VC-switched ATM cells with PHS. The parameter is relevant for both ATM and Packet Convergence Sublayers.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default:  49

Range: NA
*/
#define WMAN_IF_CMN_CPSSDUSIZE						1110
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
Specifies the upstream scheduling service used for upstream service flow. If the referenced parameter is not present 
in the corresponding 80216 QOS parameter set of an upstream service flow, the default value of this object is 
bestEffort(2).

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSSFSCHEDULINGTYPE				1111
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
True(1) ARQ enabling is requested for the connection

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA  

Range: NA
*/
#define WMAN_IF_CMN_CPSARQENABLE					1112
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
Indicates the maximum number of unacknowledged fragments at any time

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA  

Range: 1-1024
*/
#define WMAN_IF_CMN_CPSARQWINDOWSIZE				1113
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The maximum time interval an ARQ fragment will be managed by the transmitter ARQ machine, once initial transmission of
the fragment has occurred. If transmission or retransmission of the fragment is not acknowledged by the receiver before 
the time limit is reached, the fragment is discarded. The value is indicated in units of 100us granularity. A value of 0 means infinite.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-65535
*/
#define WMAN_IF_CMN_CPSARQBLOCKLIFETIME				1114
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The maximum interval before declaring a loss of synchronization of the sender and receiver state machines.
The value is indicated in units of 100us granularity. A value of 0 means infinite.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-65535
*/
#define WMAN_IF_CMN_CPSARQSYNCLOSSTIMEOUT			1115
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
Boolean value that indicates whether or not data is to be delivered by the receiving MAC to its client application in the order in which 
data was handed off to the originating MAC.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0: Order of delivery is not preserved
	   1: Order of delivery is preserved.
*/
#define WMAN_IF_CMN_CPSARQDELIVERINORDER			1116
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
Indicates the time interval after which the ARQ window is advanced after a fragment is received. 
The value is indicated in units of 100us granualarity. A value of 0 means infinite.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-65535
*/
#define WMAN_IF_CMN_CPSARQRXPURGETIMEOUT			1117
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This encoded value of this parameter specifies the size of an ARQ block. This parameter shall be 
established by negotiation during the connection creation dialog. The ARQ block size is derived from 
the coded value by the equation,
ARQ Block size = 2^(P+4),P<=6 where P is the coded value returned by the MIB.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range of coded values: 1-6
*/
#define WMAN_IF_CMN_CPSARQBLOCKSIZE					1118
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
Minimum Tolerable Traffic Rate = R (bits/sec) with time base T(sec) means the following: Let S denote additional demand 
accumulated at the MAC SAP of the transmitter during an arbitrary time interval of the length T. Then the amount of 
data forwarded at the receiver to CS (in bits) during this interval should be not less than min {S, R * T}.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSMINRSVDTOLERABLERATE			1119
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter provides the capability to specify certain attributes for the associated service flow 
An attribute is enabled by setting the corresponding bit position to 1. Refer 11.13.12 in IEEE 802.16e-2005 for more 
information on the bit positions.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPSREQTXPOLICY					1120
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
This parameter specifies the convergence sublayer encapsulation mode

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_SFCSSPECIFICATION				1121
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The target SAID parameter indicates the SAID onto which the service flow being set up shall be mapped

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST 

Length: UINT32

Default: NA 

Range: 0-65535
*/
#define WMAN_IF_CMN_CPSTARGETSAID					1122

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
Classifier rule priority

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT8

Default:  0

Range: 0-255
*/
#define WMAN_IF_CMN_CLASSIFIERRULEPRIORITY			1123
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
The low value of a range of TOS byte values. If the referenced parameter is not present in a classifier, 
this object reports the value of 0.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT8

Default: 0

Range: 0-255
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPTOSLOW			1124
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
The 8-bit high value of a range of TOS byte values. If the referenced parameter is not present 
in a classifier, this object reports the value of 0.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT8

Default: 0

Range: 0-255
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPTOSHIGH			1125
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
The mask value is bitwise AND’ed with TOS byte in an IP packet and this value is used for the range checking of TosLow and TosHigh. 
If the referenced parameter is not present in a classifier, this object reports the value of 0.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT8

Default: 0

Range: 0-255
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPTOSMASK			1126
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object indicates the value of the IP Protocol field
required for IP packets to match this rule

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99


Length: UINT8 array of size #MIBS_MAX_PROTOCOL_LENGTH.

Default: 0

Range: 0-255
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPPROTOCOL		1127
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the value of the IP source address
required for packets to match this rule. An IP packet
matches the rule when the packet IP source address bitwise
ANDed with the wmanIfCmnClassifierRuleIpSourceMask value
equals the wmanIfCmnClassifierRuleIpSourceAddr value

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99



Length: UINT32 *4 for IPV4 and UINT32 * 16 for IPv6


Default: 0

Range: NA
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPSRCADDR			1128
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies which bits of a packet's IP source
address that are compared to match this rule. An IP packet
matches the rule when the packet source address bitwise
ANDed with the wmanIfCmnClassifierRuleIpSourceMask value equals the
wmanIfCmnClassifierRuleIpSourceAddr value.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT32 *4 for IPV4 and UINT32 * 16 for IPv6

Default: 0

Range: NA
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPSRCMASK			1129
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the value of the IP destination address required for packets to match this rule. An IP
packet matches the rule when the packet IP destination address bitwise ANDed with the wmanIfCmnClassifierRuleIpDestMask 
value equals the wmanIfCmnClassifierRuleIpDestAddr value.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT32 *4 for IPV4 and UINT32 * 16 for IPv6

Default: 0

Range: NA
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPDESTADDR		1130
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies which bits of a packet's IP
destination address that are compared to match this rule.
An IP packet matches the rule when the packet destination
address bitwise ANDed with the
wmanIfCmnClassifierRuleIpDestMask value equals the
wmanIfCmnClassifierRuleIpDestAddr value.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99


Length: UINT32 *4 for IPV4 and UINT32 * 16 for IPv6

Default: 0

Range: NA
*/
#define WMAN_IF_CMN_CLASSIFIERRULEIPDESTMASK		1131
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the low end inclusive range of
TCP/UDP source port numbers to which a packet is compared. This object is irrelevant for non-TCP/UDP IP packets

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT16

Default: 0

Range: 0-65535
*/
#define WMAN_IF_CMN_CLASSIFIERRULESRCPORTSTART		1132
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the high end inclusive range of
TCP/UDP source port numbers to which a packet is compared.
This object is irrelevant for non-TCP/UDP IP packets

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT16

Default: 0

Range: 0-65535
*/
#define WMAN_IF_CMN_CLASSIFIERRULESRCPORTEND		1133
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the low end inclusive range of
TCP/UDP destination port numbers to which a packet is
compared.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT16

Default: 0

Range: 0-65535
*/
#define WMAN_IF_CMN_CLASSIFIERRULEDSTPORTSTART		1134
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	Packet classifier rules associated with service flows

Description: 
This object specifies the high-end inclusive range of
TCP/UDP destination port numbers to which a packet is
compared.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : ClassifierRuleIndex in range 0-99

Length: UINT16

Default: 0

Range: 0-65535
*/
#define WMAN_IF_CMN_CLASSIFIERRULEDSTPORTEND		1135
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS Rule 

Description: 
The PHSF (PHS Field) is a string of bytes containing the header information to be suppressed 
by the sending CS and reconstructed by the receiving CS. The most significant byte of the string corresponds to 
the first byte of the CS-SDU.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99


Length: UINT8 array of size #MIBS_MAX_PHS_LENGTHS.

Default: NA

Range: 0-255
*/
#define WMAN_IF_CMN_PHSRULEFIELD					1136
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS Rule 

Description: 
The PHSM is an 8-bit mask that indicates which bytes in the PHS Field (PHSF) to suppress and which bytes to not suppress. 
The PHSM allows fields, such as sequence numbers or checksums (which vary in value), to be excluded from suppression 
with the constant bytes around them suppressed It is encoded as follows: 
bit 0: 0 = don't suppress the 1st byte of the suppression field 1 = suppress first byte of the suppression field bit 1: 0 = don't suppress the 2nd byte of the 
suppression field 1 = suppress second byte of the suppression field bit x: 0 = don't suppress the (x+1) byte of the 
suppression field 1 = suppress (x+1) byte of the suppression field where the length of the octet string is 
ceiling (wmanIfCmnPhsRulePhsSize/8)

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT8 array. The length of the array depends upon the PHS rule size. Maximum possible value is #MIBS_MAX_PHS_LENGTHS.

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_PHSRULEMASK						1137
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS Rule

Description: 
The value of this field - PHSS is the total number of bytes in the header to be suppressed and then 
restored in a service flow that uses PHS.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT8

Default: 0 

Range: 0-255
*/
#define WMAN_IF_CMN_PHSRULESIZE						1138
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS Rule 

Description: 
The value of this field indicates to the sending entity whether 
or not the packet header contents are to be verified prior to performing suppression
0 – PHS Verify Enable
1 – PHSVerifyDisable

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT8

Default: 0 

Range: 0-1
*/
#define WMAN_IF_CMN_PHSRULEVERIFY					1139
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the current RX Rate from SS and BS.
Before fetching the parameter we need to query for SF ID List using MIB ID
BCM_BASEBAND_SF_ID_LIST and the key value would be the SF ID of the previous
query response.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPS_CURRENT_RXRATE				1140
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	
MIB Category:
	Contains service flow managed objects that are common in BS and SS

Description: 
The value of this parameter specifies the current TX Rate from SS and BS.
Before fetching the parameter, we need to query for SF ID list using MIB ID
BCM_BASEBAND_SF_ID_LIST and the key value would be the SF ID of the previous
query response.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST

Length: UINT32

Default: NA 

Range: NA
*/
#define WMAN_IF_CMN_CPS_CURRENT_TXRATE				1141
/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS rule 

Description: 
Gives number of PHS Modified bytes.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT32

Default: NA

Range: 0-255
*/
#define WMAN_IF_CMN_PHSMODIFIEDBYTES				1142


/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS rule 

Description: 
Gives number of PHS modified packets.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_PHSMODIFIEDNUMPACKETS				1143

/**
@ingroup mib_statistics_standard

Type: Standard Statistics MIBS
	

MIB Category:
	The classifier uniquely maps packets to its associated PHS rule 

Description: 
Gives number of PHS Error number of packets.

Key Values:
This MIB should be queried with two keys as follows:
 - Key1 : SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST
 - Key2 : PHS Index in range 0-99

Length: UINT

Default: NA

Range: NA
*/
#define WMAN_IF_CMN_PHSERRORNUMPACKETS				1144




/* Beceem Propreitary Statistics MIBS Defines */
/* Range 1400 - 1699*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of frames received since system start

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMFRAMESRECVD					1400
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of frames in error. This error includes FCH error and CRC/HCS error in maps and PDUs.
	For a HARQ burst, this counter will be incremented only if last HARQ retranmission is also in error.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMFRAMESINERR					1401
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of CRC check failures, this includes MAP and PDU CRC check failures.
	The relation between #BCM_STATS_NUMCRCERR and #BCM_STATS_PDU_CRC_ERRORS is given 
	by BCM_STATS_NUMCRCERR = MIBs BCM_STATS_PDU_CRC_ERRORS + CRC errors in the maps.



Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMCRCERR							1402
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of HCS check failures, this includes MAP and PDU HCS check failures. 
	The relation between MIBs #BCM_STATS_NUMHCSERR and MIBS #BCM_STATS_PDU_HCS_ERRORS is given by 
	BCM_STATS_NUMHCSERR = MIBS BCM_STATS_PDU_HCS_ERRORS  +  HCS errors in the Dl map


Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMHCSERR							1403	
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of times CQICH information was transmitted

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMCQICHTXMTD                 	1404
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of number of bandwidth request from MS to BS
Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMBWREQUESTSSENT             	1405
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of MS Sync failures.

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMSYNCFAILURES               	1406
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of times T20 timer has expired.

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMDLCHSMARKEDUNUSABLE        	1407
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of times T12 timer has expired

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMULCHSMARKEDUNUSABLE        	1408 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever DCD is received

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMDCDSRECEIVED               	1409 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever UCD is received

Length: UINT8

Default: 0 

Range: NA
*/

#define BCM_STATS_NUMUCDSRECEIVED               	1410
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever Uplink channel configuration changes

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_CURUCDCHANGECNT               	1411
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever Downlink channel configuration changes

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_CURDCDCHANGECNT               	1412
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever authentication procedure fails

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMAUTHENTICATIONFAILURES     	1413
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Incremented whenever SBC fails

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMSBCFAILURES                	1414
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of FCH received without error

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMFCHRECEIVED                	1415
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of DLMAPs received without HCS or CRC errors

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMDLMAPSPROCESSED            	1416
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of ULMAPs received without HCS or CRC errors

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMULMAPRECEIVED              	1417 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of invalid DLMAPs received

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMBAD_DLMAPS                    	1418 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Num of Bad FCHs

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMBADFCH                     	1419
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of frames received without getting correct FCH 

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMNOFCH                      	1420
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of SDUs baseband has received from the Host for transmission

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMSDUSFROMHOST               	1421  
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received SDUs sent to host by baseband  

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMSDUSTOHOST                 	1422
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of non-data SDUs received from the host by the baseband

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMCTRLPKTSFROMHOST           	1423
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of non data SDUs sent to host by baseband

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMCTRLPKTSTOHOST             	1424
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMPDUSTRANSMITTED            	1425 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS. It includes both broadcast and unicast PDUs for the MS.


Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUM_WIMAX_MAC_PDUS_RECEIVED              	1426 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of 80216e management packets transmitted by MS

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMMGMTPKTSTRANSMITTED        	1427 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of 80216e management packets received by MS

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMMGMTPKTSRECIEVED           	1428
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of SDUs sent by the host which were rejected by the baseband due to incorrect CID mapping

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMTXPKTSREJECTED             	1429
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received on air interface and were rejected by the baseband due to incorrect CID mapping, CRC error or HCS error

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMRXPKTSREJECTED             	1430
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	DL ID CELL from preamble

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_DLIDCELL                      	1431 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Maximum transmit power in dBm

Length: SINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_TXREFPOWER                    	1432 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Delta between current transmit power and maximum transmit power in dBm

Length: SINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_TXPOWERHEADROOM               	1433
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Current transmit power in dBm

Length: SINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_TXPOWER                       	1434
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Initial ranging code start value from UCD message.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_IRCODESTART                   	1435 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Initial ranging code end value from UCD message.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_IRCODEEND                     	1436
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Bandwidth request code start. 

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BRCODESTART                   	1437 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Bandwidth request code end. 

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BRCODEEND                     	1438
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Handover ranging code start.

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_HRCODESTART                   	1439
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Handover ranging code end.

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_HRCODEEND                     	1440
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Base station ID from DCD message

Length: UINT48

Default: 0 

Range: NA
*/
#define BCM_STATS_BSID                          	1441 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Network entry state of the baseband.

Length: UINT32

Default: 0 

Range:
 - Network entry stages:
	0=wait for sync,
	1=phy sync achieved,
	2=wait for RNG RSP,
	3=wait for SBC RSP,
	4=wait for REG RSP,
	5=wait for DSA,
	6=network entry complete

*/
#define BCM_STATS_MODEMSTATE                    	1442
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Operating center frequency from DCD message.

Length: UINT32

Default: 0 

Range: Valid center frequency (KHz)
*/
#define BCM_BASEBAND_CENTERFREQ                 	1443
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink rate in kbps

Length: UINT32

Default: 0 

Range: Rate in kbps
*/
#define BCM_BASEBAND_DLRATE                     	1444
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink rate in kbps

Length: UINT32

Default: 0 

Range: Rate in kbps
*/
#define BCM_BASEBAND_ULRATE                     	1445 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of HARQ retries transmitted by the MSS.
	Subscriber station keeps track of maximum of 4 retries sent to the base station.
	Each HARQ retry counter will be incremented UL HARQ retry attempts. 
	Maximum retry attempts tracked is 4.
	Array index 0-3 corresponds to the first to fourth retransmission counter.
Length: 4 * UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_HARQRETRYSND                  	1446  
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of HARQ retries received by the MSS.
	Subscriber station keeps track of maximum of 4 retries from the base station.
	Each HARQ retry counter will be incremented DL HARQ retry attempts. 
	Maximum retry attempts tracked is 4.
	Array index 0-3 corresponds to the first to fourth retransmission counter.
Length: 4 * UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_HARQRETRYRCV                  	1447


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Array representing DL HARQ bursts successfully received (CRC-16 passed) on the four transmissions. 
	
Length: 4 * UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_HARQ_PERATTEMPT_RCV_SUCCESS	1448 

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of ARQ retries transmitted by the MS

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_ARQRETRYSND                   	1449

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of ARQ sub-bursts that have been attempted the maximum number of times with failure each time

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_ARQRETRYEXCEEDED              	1451 

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts transmitted by MS at MCS coded with QPSK 1/2

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTSNDQPSK12                  	1452  
#define BCM_STATS_PDUSNDQPSK12 BCM_STATS_BURSTSNDQPSK12 /*This is for backward compatibility only*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts transmitted by MS at MCS coded with QPSK 3/4

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTSNDQPSK34                  	1453
#define BCM_STATS_PDUSNDQPSK34 BCM_STATS_BURSTSNDQPSK34 /*This is for backward compatibility only*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts transmitted by MS at MCS coded with 16 QAM 1/2

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTSND16QAM12                 	1454
#define BCM_STATS_PDUSND16QAM12 BCM_STATS_BURSTSND16QAM12 /*This is for backward compatibility only*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts transmitted by MS at MCS coded with 16 QAM 3/4

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTSND16QAM34                 	1455
#define BCM_STATS_PDUSND16QAM34 BCM_STATS_BURSTSND16QAM34 /*This is for backward compatibility only*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with QPSK 1/2

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCVQPSK12                   	1456
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with QPSK 3/4

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCVQPSK34                  	1457
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 16 QAM 1/2

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV16QAM12                 1458 
#define BCM_STATS_BURSTPDURCV16QAM12 BCM_STATS_BURSTRCV16QAM12 /*This is for backward compatibility only*/
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 16 QAM 3/4

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV16QAM34                 	1459
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 64 QAM 1/2

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV64QAM12                 	1460
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 64 QAM 2/3

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV64QAM23                 	1461 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 64 QAM 3/4

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV64QAM34                 	1462
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bursts received by MS at MCS coded with 64 QAM 5/6

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BURSTRCV64QAM56                 	1463
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of initial ranging failures

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_INITRANGINGFAIL               	1464  
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of periodic ranging failures

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PERIODICRANGINGFAIL           	1465
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of bandwidth ranging failures

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BANDWIDTHRANGINGFAIL          	1466

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Rx -1 Antenna RSSI, Add a negative (-ve)sign to the returned value.

Length: SINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_RSSIRXPATH1			          	1467

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Rx -2 Antenna RSSI, add a negative (-ve)sign to the returned value.

Length: SINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_RSSIRXPATH2			          	1468
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of handover ranging failures

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_HANDOVERRANGINGFAIL		       	1469
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	UL Permutation

Length: UINT8

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDULPERMUTATION			1470 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Neighbor BS Intra-FA Mean CINR in steps of 1dB.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBSINTRAFAMEANCINR             	1471 

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Neighbor BS Inter-FA Mean CINR

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_BCMBSINTERFAMEANCINR						1472

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of DL HARQ bursts successfully received (CRC-16 passed) on any attempt.

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_HARQ_FINAL_RCV_SUCCESS							1473

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Array representing number of DL HARQ transactions completed on four transmissions.

Length: 4 * UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_HARQ_PERATTEMPT_RCV_TRANSCOMPLETED				1479
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of CRC16 failures on Ack disabled HARQ bursts.

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_NUMOF_ACKDISABLED_CRC16_FAILED_BURSTS			1480

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of DL HARQ transactions completed.

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_HARQ_FINAL_RCV_TRANSCOMPLETED					1481

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Array representing submap errors of size four.

Length: 4 * UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_SUBMAP_ERRORS									1482

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Lower bound on empirical probability of error of 1st DL HARQ attempt. 
	For precision calculation, the value should be divided by 10 to the power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.


Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_DL_PREHARQ_BURST_ERRORRATE_LOWER				1483				

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	HO Cancel count

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_BCMBASEBANDNUMHOCANCEL							1485 

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	HO reject count

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDNUMHOREJECT							1486
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	The mean Tx power spectral density (power per subcarrier).
	In steps of 0.25dBm

Length: SINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDMEANTXPOWER							1487
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	The cumulative DL map Length

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDCUMULATIVEDLMAPLENGTH					1488 
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	The cumulative UL map Length

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDCUMULATIVEULMAPLENGTH					1489
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Time taken for doing network entry

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_BCMBASEBANDNETWORKENTRYDURATION					1490
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
    Neighbour BS information

Length: Size of #ST_NEIGHBOUR_BWINFO

Default: NA

Range: NA
*/
#define BCM_BASEBAND_NEIGBOUR_BSINFO					1494

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	The number of downlink symbols and uplink symbols in each frame. The most significant 
	16 bits represents number of downlink symbols and the least significant 16 bits 
	represents the number of uplink symbols.

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_BASEBAND_FRAME_RATIO						1495


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink Burst Profile from DCD

Length: Size of #_ST_BCMBASEBAND_DL_BURSTPROFILE

Default: NA

Range: NA
*/
#define BCM_BASEBAND_DL_BURSTPROFILE						1496


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst profile from UCD

Length: Size of #_ST_BCMBASEBAND_UL_BURSTPROFILE

Default: NA

Range: NA
*/

#define BCM_BASEBAND_UL_BURSTPROFILE				1497

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	OFDMA connection parameters from SBC exchange

Length: Size of #_ST_BCM_SBC_CONNECTIONPARAMS

Default: NA

Range: NA
*/
#define BCM_BASEBAND_SBC_CONNECTIONPARAMS				1498


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description : 
	Downlink PER(Packer Error Rate) measurement. To get PER in percentage, divide the MIB value with 10 to the power of 4. 
	

	PDU CRC error
	 Number of CRC errors including mgmt messages

	PDU HCS errors
	 Number of HCS errors including mgmt messages

	Packets_Received = PDUs_Received + NumOfCrcErrors(#BCM_STATS_NUMCRCERR) + NumOfHcsErrors(#BCM_STATS_NUMHCSERR);
	
	Packets_InError =  NumOfCrcErrors(#BCM_STATS_NUMCRCERR) + NumOfHcsErrors(#BCM_STATS_NUMHCSERR)

	PacketErrorRate(%) = (Packets_InError * 100) / Packets_Received;
	
		
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_BASEBAND_PACKETERRORRATE						1499

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description : 
	FER(Frames Error Rate) measurement. To get FER in percentage, divide the MIB value with 10 to the power of 4.
	
	Frames in error incremented with every FCH or DLMAP error or DLMAP demod is still ON 
	when DL-ISR is started (i.e. delayed DLMAP and hence error) – this is referred as
	NumOfFramesInError in the below eq.

	DL map CRC errors
	 Number of DLMAPs in CRC error

	DL map HCS errors
	 Number of DLMAPs in HCS error


	 FrameErrorRate(%) = (NumOfFramesInError * 100) / NumOfFramesRecvd;
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_BASEBAND_FRAME_ERRORRATE						1500

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	UL BW request information

Length: Size of #_ST_UL_BW_REQ_INFO

Default: NA

Range: NA
*/
#define  BCM_BASEBAND_UL_BW_REQ_INFO						1501

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Total allocation granted to this MS

Length: Size of #_ST_UL_BW_ALLOC_INFO

Default: NA

Range: NA
*/
#define  BCM_BASEBAND_BW_ALLOCATION_INFO				1502


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Baseband SF ID list

Length: Size of #_SFID_LIST

Default: NA

Range: NA
*/
#define  BCM_BASEBAND_SF_ID_LIST						1503

/* stLinkStats */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Current RSSI information in steps of 0.25dBm

Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_CURRENT_RSSI						1504
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Current CINR information in steps of 1 dB

Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_CURRENT_CINR						1505
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Current Mac state information

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_MAC_STATE_ID						1506
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Uplink ranging code

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_UL_RANGING_CODE					1507
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Uplink ranging seed

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_UL_RANGING_SEED					1508
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Power work mode information about operation mode of power control module

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_POWER_WORKMODE					1509
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Current RSSI deviation field in steps of 0.25dB

Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_RSSI_DEVIATION_FIELD				1510
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Number of bandwidth requests

Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUMBRTH								1511
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of connections
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_CONN_COUNT							1512
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	List of transport connection identifiers
	
Length: size of #MAX_NUM_CONNECTIONS

Default: NA

Range: NA
*/
#define BCM_STATS_TRANSPORT_CID						1513

/* stDPStats */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of bytes exchanged from host
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUMBYTES_FROM_HOST				1514
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total Number of bytes to host exchanged
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUMBYTES_TO_HOST					1515
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of uplink connections
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUM_UL_CONNS						1516
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of downlink connections
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUM_DL_CONNS						1517

/* stLinkStatEx */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Mean CINR information in steps of 1dB.
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_MEAN_CINR							1518
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Deviation CINR information in steps of 1dB.
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_DEVIATION_CINR					1519
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Preamble ID of the current serving BS
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PREAMBLE_INDEX					1520
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Periodic ranging code end information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PRCODE_END						1521
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Periodic ranging code start information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PRCODE_START						1522

/* stStatistics */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host. 

Description: 
	List of downlink modulation coding schemes information. It represents total number of unicast packets for MS.
	
Length: size of #NUMBER_OF_FEC_TYPES

Default: NA

Range: NA
*/
#define BCM_STATS_DLMCS								1523
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	List of uplink modulation coding schemes information
	
Length: size of #NUMBER_OF_FEC_TYPES

Default: NA

Range: NA
*/
#define BCM_STATS_ULMCS								1524

/* stPhyStateInfo */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data zone information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_ZONE			1525
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink burst data zone information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_ZONE			1526
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data forward error correction scheme information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_FEC_SCHEME		1527
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink burst data forward error correction scheme information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_MAP_FEC_SCHEME				1528
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Phy state information frame number
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_FRAME_NUM						1529
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Phy state information about local frame number
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_LOCAL_FRAME_NUM				1530
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data duration information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_DURATION		1531
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink burst data duration information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_DURATION		1532
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data size information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_SIZE			1533
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink burst data size information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_SIZE			1534
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink ranging seed information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_RANGING_SEED				1535
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data connection identifier information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_CID				1536
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink burst data connection identifier information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_CID				1537
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink burst data uplink interval usage code information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_UIUC			1538
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host. 

Description: 
	Downlink burst data downlink interval usage code information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_DIUC			1539
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Information about forward error correction repetition used for uplink burst data
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_BURST_DATA_FEC_REPETITION	1540
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Information about forward error correction repetition used for downlink burst data
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_FEC_REPETITION	1541
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink frame ratio information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_UL_FRAME_RATIO				1542
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink frame ratio information
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_FRAME_RATIO				1543
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Information about forward error correction scheme for downlink burst data
	
Length: UINT8

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_DL_BURST_DATA_FEC_SCHEME		1544

/* tHoEventInfo */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Number of handover drops
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUM_OF_HODROPS					1545
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of handover trails
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUM_OF_HOTRAILS					1546
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of PingPong counts
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PING_PONG_COUNT					1547
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Handover type identifier information
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_HO_TYPEID							1548
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Number of intra frequency neighbours information
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUM_OF_INTRA_FANBRS				1549
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host. 

Description: 
	List of IntraFA preamble index information
	
Length: SINT32 * size of #DM_MAX_NBRS

Default: NA

Range: NA
*/
#define BCM_STATS_INTRA_FA_PREAMBLE_INDEX			1550
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	List of InterFA preamble index information
	
Length: SINT32 * size of #DM_MAX_NBRS

Default: NA

Range: NA
*/
#define BCM_STATS_INTER_FA_PREAMBLE_INDEX			1551
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Handover latency information
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_HO_LATENCY						1552

/* stStatistics */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Information about frame number to be used
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_FRAME_NUMBER						1553
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Information about current base station offset to be used
	
Length: SINT32

Default: NA

Range: NA
*/
#define BCM_STATS_CURRENT_OFFSET_BS					1554
 
 /* tHoEventInfoEx */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host. 

Description: 
	Preamble ID of the serving BS involved in the latest HO. Valid only after a HO is performed.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_SERVING_BS_PREAMBLE_INDEX			1555
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Segment ID of the serving BS involved in the latest HO. Valid only after a HO is performed.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_SERVING_BS_SECTOR_ID				1556
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Cell ID of the serving BS involved in the latest HO. Valid only after a HO is performed.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_SERVING_BS_CELL_ID					1557
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Preamble ID of the target BS involved in the latest HO. Valid only after a HO is performed.
		
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_TARGET_BS_PREAMBLE_INDEX			1558
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description:
	Segment ID of the target BS involved in the latest HO. Valid only after a HO is performed.
		
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_TARGET_BS_SECTOR_ID				1559
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Cell ID of the target BS involved in the latest HO. Valid only after a HO is performed.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_TARGET_BS_CELL_ID					1560
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Handover status information
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_HO_STATUS							1561

/* stStatistics */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Information about Phy Sync global timer block count
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PHY_SYNC_GTB_COUNT				1562
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Information about network entry global timer block count
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NW_ENTRY_GTB_COUNT				1563

/* tErrorCounter */
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of downlink Map CRC errors
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_DL_MAP_CRC_ERRORS					1564
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of downlink map header checksum errors
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_DL_MAP_HCS_ERRORS					1565
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Total number of CRC check failures for the MAC pdus received.
	The relation between MIBs #BCM_STATS_PDU_CRC_ERRORS and #BCM_STATS_NUMCRCERR is given by BCM_STATS_NUMCRCERR = MIBs BCM_STATS_PDU_CRC_ERRORS + CRC errors
in the maps or burst.

	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PDU_CRC_ERRORS					1566
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	Total number of HCS check failures for the MAC pdus received. 
	Relation between MIBS #BCM_STATS_PDU_HCS_ERRORS and #BCM_STATS_NUMHCSERR is given by BCM_STATS_NUMHCSERR = MIBS BCM_STATS_PDU_HCS_ERRORS  +  HCS errors in the Dl map

	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PDU_HCS_ERRORS					1567
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total compressed maps.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_COMPRESSED_MAP_COUNT				1568
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Total number of resyncs for the baseband.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_NUMBER_OF_RESYNC				1569
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	The Phy state information of the baseband.
	
Length: UINT32

Default: NA

Range: 1-3
*/
#define BCM_STATS_PHY_STATE_INFO				1570
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	The bandwidth information of the baseband.
	
Length: UINT32

Default: NA

Range: 0 to  4
- 0 corresponds to 5 MHz
- 1 corresponds to 7 MHz
- 2 corresponds to 8.75 MHz
- 3 corresponds to 10 MHz
- 4 corresponds to 3.5 MHz

*/
#define BCM_STATS_BW_INFO					1571
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	The PLL status information of the baseband.
	
Length: UINT32

Default: NA

Range: NA
*/
#define BCM_STATS_PLL_STATUS_INFO			1572
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Details the Tx power offset of the device
	
Length: UINT32

Default: 0

Range: NA
*/
#define BCM_STATS_TX_PWR_INFO			1573
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Details the antenna value information

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_STATS_ANTENNA_VAL_INFO		1574
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Details the several BS tracking information. 

Length: Size of #_ST_BSTRACKER_INFO

Default: 0

Range: NA
*/
#define BCM_STATS_BS_TRACK_INFO			1575

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description: 
	List of current Security Association identifiers

Length: UINT32 * MAX_SAID_ENTRIES

Default: NA

Range: NA
*/
#define BCM_BASEBAND_SAID_LIST			1576
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host 

Description : 
	Primary CINR Info in steps of 1dB.

Length: SINT32

Default: NA

Range: NA
*/
#define BCM_BASEBAND_PRIMARY_CINR		1577
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host. 

Description : 
	Diversity CINR Info in steps of 1dB.

Length: SINT32

Default: NA

Range: NA
*/
#define BCM_BASEBAND_DIVERSITY_CINR		1578


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description : 
	Downlink frame number being received

Length: UINT32

Default: 0

Range: NA
*/

#define BCM_BASEBAND_DL_FRAME_NUM				1579
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description : 
	Count the number of times that the 
	initial ranging process successfully completed.

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_INIT_RANGE_SUCCESS		1580
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of periodic ranging successes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_PERIODIC_RANGE_SUCCESS		1581
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of handover attempts

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_HO_ATTEMPTS						1582
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of handover successes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_HO_SUCCESS				1583
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received QPSK 1/2 CTC Bursts, Rep 6

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP6		1584
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received QPSK 1/2 CTC Bursts, Rep 4

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP4		1585
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received QPSK 1/2 CTC Bursts, Rep 2

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP2		1586
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received QPSK 1/2 CTC Bursts, Rep 1

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_BURSTS_REP1		1587
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of frames for which a PSC is active since power up or reset.

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_TIME_IN_SLEEP_MODE		1588

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Time in Idle mode. Represented in units of frames.

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_TIME_IN_IDLE_MODE		1589
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Time in Active mode. Represented in units of frames.

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_TIME_IN_ACTIVE_MODE		1590

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of transmitted BE bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_TX_BE_BYTES		1591
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of transmitted UGS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_TX_UGS_BYTES		1592
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of transmitted RTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_TX_RTPS_BYTES		1593
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of transmitted ERTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_TX_ERTPS_BYTES		1594
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of transmitted NRTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_TX_NRTPS_BYTES		1595

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Downlink traffic rate

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_DL_TRAFFIC_RATE			1596
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Uplink traffic rate

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_UL_TRAFFIC_RATE			1597
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received BE bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_RX_BE_BYTES		1598
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received UGS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_RX_UGS_BYTES		1599
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received RTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_RX_RTPS_BYTES		1600
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received eRTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_RX_ERTPS_BYTES		1601
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of received nRTPS bytes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_RX_NRTPS_BYTES		1602
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of the number of service flows creation failures

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_SF_FAILURES				1603
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Count of the number of service flows creation successes

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_SF_CREATION_SUCCESS		1604
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Count of the number of service flows deleted

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUMOF_SF_DELETIONS				1605
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the current ID of the cell segment

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_CURRID_CELLSEG					1606
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Contains the NAI string recieived before Authentication message exchage.
	NAI is a NULL Terminated string. If data is available then the values are valid else it is invalid.

Length: UCHAR*253

Default: 0

Range: NA
*/

#define BCM_BASEBAND_REALM_INFO						1607
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
    This is count of number of non-MIMO/MIMO Mat A/B Bursts 
	that are decoded by this MS.

	- To enable logging of this MIB on baseband modem please refer to documentation of #SUBTYPE_SET_WIMAX_OPTIONS_REQ
	
Length: #_ST_PERDLZONE_PER_MCS_BURSTS_DECODED * #NUM_DL_ZONES

Default: 0 - 0xffffffff

Range: NA
*/
#define BCM_BASEBAND_PER_DLZONE_PER_MCS_BURSTS_DECODED	1608
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
    Defines the current device status.
	Return value corresponds to the enum #_DEV_STATUS

Length: UINT32

Default: 4

Range: 0-7
*/
#define BCM_BASEBAND_DEVICE_STATUS					1609
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
    Defines the current device connection status information.
	Return value corresponds to the enum #_DEV_CONNECTION_PROGRESS_INFO
	
Length: UINT32

Default: 1

Range: NA
*/
#define BCM_BASEBAND_DEVICE_CONNECTION_STATUS		1610
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
    Defines the current device link status information.
	Return value corresponds to the enum #_LINK_STATUS_REASON

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_LINK_STATUS_REASON				1611
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the most recently used frequecies list by the 
	baseband for network search. 

	Returns the actual MRUs that are present in the format _ST_MRU_INFORMATION. 
	Maximum MRU entries can be till MAX_CHANNEL_PLAN_REF_ID_PER_CAPL.

Length: #_ST_MRU_INFORMATION * (Actual MRU Entries in the EEPROM)

Default: 0

Range: NA
*/
#define BCM_BASEBAND_MRU_INFO						1612

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the RX Histogram for the packet sizes going through the device.
	The histogram array contains packet counts based on packet lengths.
	Each index in the histogram array contains the packet count for the
	range which is computed as below:
	Packet Length Range = Index * 128 - (Index+1)*128

	For example, third entry contains the packet count for packets whose length
	greater than 128 * 3 and less than or equal to 128 * 4.

Length: #UINT32 Array * (MIBS_MAX_HIST_ENTRIES = 12)

Default: 0

Range: NA
*/
#define BCM_BASEBAND_RX_PACKETS_HISTOGRAM	1613
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the TX Histogram for the packet sizes going through the device.
	The histogram array contains packet counts based on packet lengths.
	Each index in the histogram array contains the packet count for the
	range which is computed as below:
	Packet Length Range = Index * 128 - (Index+1)*128

	For example, third entry contains the packet count for packets whose length
	greater than 128 * 3 and less than or equal to 128 * 4.

Length: #UINT32 Array * (MIBS_MAX_HIST_ENTRIES = 12)

Default: 0

Range: NA
*/
#define BCM_BASEBAND_TX_PACKETS_HISTOGRAM	1614
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the number of bytes dropped for each service flow due to rate limiting
	based on the Max Sustained Traffic Rate configured for this service flow.
	Before fetching the parameter we need to query for SF ID List using MIB ID
	BCM_BASEBAND_SF_ID_LIST. And the key value would be the SF ID of the previous
	query response. User can choose any one of the SF ID from the SF_ID_LIST returned for 
	the previous response.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST	

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUM_OF_BYTES_DROPPED_PER_SF	1615
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the number of packets dropped for each service flow due to rate limiting
	based on the Max sustained traffic rate configured for this service flow.
	Before fetching the parameter we need to query for SF ID List using MIB ID
	BCM_BASEBAND_SF_ID_LIST. And the key value would be the SF ID of the previous
	query response. User can choose any one of the SF ID from the SF_ID_LIST returned for 
	the previous response.

Key Value:
The key is SFID which can be obtained from MIB #BCM_BASEBAND_SF_ID_LIST	

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NUM_OF_PACKETS_DROPPED_PER_SF	1616
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Upper bound on empirical probability of error of 1st DL HARQ attempt.
	For precision calculation, the value should be divided by 10 to the Power of 6.
	
	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

	

Length: UINT32

Default: 0

Range: NA
*/
#define BCM_BASEBAND_DL_PREHARQ_BURST_ERRORRATE_UPPER	1617

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Signifies if the data available in the MIB ID #BCM_BASEBAND_REALM_INFO is valid or not.
	If realm info available is vaild then it is set to TRUE(1) else to FALSE(0).

Length: UINT32

Default: 0

Range: NA

*/
#define BCM_BASEBAND_NAI_INFO_VALID					1618

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Provides break up of time taken during network entry procedure. Refer to documentation of  #_ST_BCMBASEBAND_NwENTRY_TIMING_STATS

Length: #_ST_BCMBASEBAND_NwENTRY_TIMING_STATS

Default: 0

Range: NA

*/
#define BCM_BASEBAND_NW_ENTRY_TIMINING_STATS		1619


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Lower bound on empirical probability of error of final DL HARQ attempt.
	For precision calculation, the value should be divided by 10 to the power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

	
Length: UINT32

Default: 0

Range: NA

*/
#define BCM_BASEBAND_DL_POSTHARQ_BURST_ERRORRATE_LOWER	1620

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Provides the total number of UL HARQ Ack enabled burst allocations.

Length: UINT32

Default: 0

Range: NA

*/
#define BCM_BASEBAND_NUM_OF_UL_HARQ_ACK_ENABLED_BURST_ALLOCS	1621

/**
@ingroup mib_statistics_proprietary

//Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Provides the total number of UL HARQ Ack enabled new transmission.

Length: UINT32

Default: 0

Range: NA

*/
#define BCM_BASEBAND_NUM_OF_UL_HARQ_ACK_ENABLED_NEW_TX		1622

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs


MIB Category:
	These MIBs are read only from the host

Description: 
	Provides the total number of UL HARQ Ack enabled retransmission of Tx.

Length: UINT32

Default: 0

Range: NA

*/
#define BCM_BASEBAND_NUM_OF_UL_HARQ_ACK_ENABLED_RETX		1623

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Upper bound on empirical probability of error of final DL HARQ attempt.
	For precision calculation, the value should be divided by 10 to the power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

	
Length: UINT32

Default: 0

Range: NA

*/
#define	BCM_BASEBAND_DL_POSTHARQ_BURST_ERRORRATE_UPPER		1624


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the MCS Per Zone Statistics Information of the Baseband for UL and DL.

	- To enable logging of this MIB on baseband modem please refer to documentation of #SUBTYPE_SET_WIMAX_OPTIONS_REQ
	
Length: #_ST_PER_ZONE_MCS_STATS

Default: 0

Range: NA
*/
#define BCM_BASEBAND_PER_ZONE_PER_MCS_BURSTS_DECODED		1625

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the CbyN information of the baseband.

Length: #_ST_BCMBASEBAND_CbyNTable

Default: 0

Range: NA
*/
#define BCM_BASEBAND_CBYN_TABLE								1626

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the NI information of the baseband.

Length: #_ST_BCMBASEBAND_NI

Default: 0

Range: NA
*/
#define BCM_BASEBAND_NI_INFO								1627

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the scan parameters for smart antenna feature.

Length: #_ST_BCMBASEBAND_ScanParams

Default: 0

Range: NA
*/
#define BCM_BASEBAND_SCAN_PARAMS_INFO						1628

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the HO statistics information of the baseband.

Length: ST_HANDOVER_INFORMATION

Default: 0

Range: NA
*/
#define BCM_BASEBAND_HO_STATISTICS_INFO				1629

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the statistics information of IDLE mode at the baseband.

Length: ST_IDLEMODE_INFORMATION

Default: 0

Range: NA
*/
#define BCM_BASEBAND_IDLE_MODE_STATISTICS_INFO		1630

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the transmitter antenna switching information at the baseband.

Length: #_ST_TXANTSWITCH_INFORMATION

Default: 0

Range: NA
*/

#define BCM_BASEBAND_TXANTENNA_SWITCH_INFO			1631

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the MIMO mode as follows:
	 - 0  - No MIMO mode reports
	 - 40 - MIMO MODE-A
	 - 42 - MIMO MODE-B


Length: UINT32

Default: 0

Range: NA
*/

#define BCM_BASEBAND_MIMO_MODE_REPORT				1632
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the WiMAX trigger status.

Length: #_ST_TRIGGER_STATUS_INFO

Default: 0

Range: NA
*/

#define BCM_BASEBAND_TRIGGER_STATUS_INFO			1633

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Array representing upper bound on empirical conditional probability of error of four attempts.
	For precision calculation, the value should be divided by 10 to the power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

	
Length: 4 * UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_DL_PER_RX_BURSTERRORRATE_UPPER		1634

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of confirmed final DL HARQ failures. 
	For precision calculation, the value should be divided by 10 to the power of 6.	

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.


Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_DL_POST_HARQ_CONFIRMED_FAILURES	1635

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of DL HARQ transactions still in progress.
	For precision calculation, the value should be divided by 10 to the Power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.


Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_BASEBAND_DL_HARQ_TRANSACTIONS_INPROGRESS	1636

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Sum of Number of PDUs received by MS at MCS coded with QPSK 1/2 CC, CTC and Rep 1, Rep 2, Rep 4, Rep 6. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDURCVQPSK12						1637

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with QPSK 3/4 CC, CTC. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDURCVQPSK34						1638
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 16 QAM 1/2. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV16QAM12						1639
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 16 QAM 3/4. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV16QAM34						1640
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 64 QAM 1/2. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV64QAM12						1641
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 64 QAM 2/3. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV64QAM23						1642
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 64 QAM 3/4. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV64QAM34						1643
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with 64 QAM 5/6. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCV64QAM56						1644
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with QPSK 1/2 CTC and repetition coding of 1. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCVQPSK12CTCREP1				1645
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with QPSK 1/2 CTC and repetition coding of 2. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDURCVQPSK12CTCREP2				1646
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with QPSK 1/2 CTC and repetition coding of 4. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCVQPSK12CTCREP4				1647
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs received by MS at MCS coded with QPSK 1/2 CTC and repetition coding of 6. This MIB for 
PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDURCVQPSK12CTCREP6				1648
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the TxAA sounding type whether it is decimated or cyclic shift
	0 - Cyclic shift seperatability
	1 - Decimation seperatability

Length: UINT32 * 4

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_TxAA_SOUNDING_TYPE				1649
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of UL sounding zones.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_NUM_UL_SOUNDING_ZONE			1650
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Compressed map status information. Status should be interpreted as:
 - 1 : Compressed Map
 - 0 : Normal Map

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_IS_COMPRESSED_MAP				1651
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of Downlink bursts Non-Harq unicast burst error rate.
	For precision calculation, the value should be divided by 10 to the power of 6.

	- This MIB is available only if error rate MIBs logging on baseband modem is enabled 
      by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.
	
Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_DL_NONHARQ_UNICAST_BURSTERRORRATE	1652
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of Downlink bursts Non-Harq broadcast burst error rate.
	For precision calculation, the value should be divided by 10 to the power of 6.

	-This MIB is available only if error rate MIBs logging on baseband modem is enabled 
	 by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_DL_NONHARQ_BRDCAST_BURSTERRORRATE	1653
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of control messages dropped by the application.
	
Length: Size of #_ST_CONTROLMESSAGES_DROPPED

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_NUM_OF_CONTROLMESSAGES_DROPPED		1654
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with QPSK 1/2 CC, CTC and repetition coding of 1. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled. 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDUSENDQPSK12REP1						1655
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with QPSK 1/2 CC, CTC and repetition coding of 2. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled. 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDUSENDQPSK12REP2						1656
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with QPSK 1/2 CC, CTC and repetition coding of 4. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled. 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDUSENDQPSK12REP4						1657
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with QPSK 1/2 CC, CTC and repetition coding of 6. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled. 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/

#define BCM_STATS_PDUSENDQPSK12REP6						1658
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with QPSK 3/4 CC, CTC. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled. 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage. 

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSENDQPSK34							1659
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with 16 QAM 1/2. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND16QAM12						1660
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description:
	Number of PDUs transmitted by MS at MCS coded with 16 QAM 3/4. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND16QAM34						1661
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with 64 QAM 1/2.This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND64QAM12						1662
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with 64 QAM 2/3. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND64QAM23						1663
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with 64 QAM 3/4. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND64QAM34						1664
/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 
	Number of PDUs transmitted by MS at MCS coded with 64 QAM 5/6. This MIB for PDU specific stats is available only if MCS MIBs logging on baseband modem is enabled 
by sending #TYPE_SET_WIMAX_OPTIONS API request prior to usage.

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_PDUSEND64QAM56						1665

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 	 
	X = A - B 
	
	X: This is the number of HARQ retransmissions exceeded
	A: retransmission counter for final retransmissions
	B: DL HARQ bursts successfully received (CRC-16 passed) on the final transmission

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_STATS_HARQRETRYEXCEEDED						1666


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 	 
	X = A - B 
	
	X: Total number of DL HARQ bursts in error
	A: Total number of DL HARQ ACK enabled bursts including retransmissions
	B: Total number of DL HARQ bursts successfully received (CRC-16 passed) on any attempt

Length: UINT32

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_NUM_OF_DLHARQ_BURSTS_IN_ERRORS 	1667

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 	 
	X[i] = A[i] - B[i] 
	i : Range is from 0 to 3 (total of 4 counters)
	X: HARQ retry in error for each attempt
	A: Retransmission counter for transmissions attempt i
	B: DL HARQ bursts successfully received (CRC-16 passed) on the transmission attempt i

Length: sizeof(UINT32)*4

Default: 0 

Range: NA
*/
#define BCM_BASEBAND_NUM_OF_HARQ_RETRY_IN_ERRORS 		1668

/**
@ingroup mib_statistics_proprietary
	Refer to documentation for #BCM_STATS_PDURCVQPSK12CTCREP6
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_PKTS_REP6 BCM_STATS_PDURCVQPSK12CTCREP6 /*This is for backward compatibility only*/

/**
@ingroup mib_statistics_proprietary
	Refer documentation for #BCM_STATS_PDURCVQPSK12CTCREP4	
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_PKTS_REP4 BCM_STATS_PDURCVQPSK12CTCREP4 /*This is for backward compatibility only*/

/**
@ingroup mib_statistics_proprietary
	Refer documentation for #BCM_STATS_PDURCVQPSK12CTCREP2
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_PKTS_REP2 BCM_STATS_PDURCVQPSK12CTCREP2 /*This is for backward compatibility only*/

/**
@ingroup mib_statistics_proprietary
	Refer documentation for #BCM_STATS_PDURCVQPSK12CTCREP1
*/
#define BCM_BASEBAND_NUMOF_QPSK12CTC_PKTS_REP1 BCM_STATS_PDURCVQPSK12CTCREP1 /*This is for backward compatibility only*/


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 	 
	Number of packets received by the host.

Length: sizeof(ULONG64)

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMPACKETS_RECEIVED_BY_HOST		1669

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	

MIB Category:
	These MIBs are read only from the host

Description: 	 
    Number of packets sent by the host.

Length: sizeof(ULONG64)

Default: 0 

Range: NA
*/
#define BCM_STATS_NUMPACKETS_TRANSMITTED_FROM_HOST 		1670

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the MCS Per Zone Statistics Information of the Baseband for DL.

	- To enable logging of this MIB on baseband modem please refer to documentation of #SUBTYPE_SET_WIMAX_OPTIONS_REQ

Key Value:
	The key is DL zone varies from #_DL_ZTYPES_::DL_ZONETYPE_BC_PUSC_R1
	
Length: #_ST_PERZONE_DL_INFO

Default: 0

Range: 0 to #NUMBER_OF_DL_ZONE_TYPES 
*/

#define BCM_STATS_DL_ZONE_INFO		1671

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description: 
	Defines the MCS Per Zone Statistics Information of the Baseband for UL.

	- To enable logging of this MIB on baseband modem please refer to documentation of #SUBTYPE_SET_WIMAX_OPTIONS_REQ
	
Key Value:
	The key is DL zone varies from #_UL_ZONE_TYPES::SCH_ZONETYPE_UL_PUSC_NONSEGMENTED

Length: #_ST_PERZONE_UL_INFO

Default: 0

Range: 0 to #NUMBER_OF_UL_ZONE_TYPES
*/

#define BCM_STATS_UL_ZONE_INFO		1672


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	- This MIB is updated by baseband modem only in CD-Less boot mode of USB interface
	- These MIBs are read only from the host

Description: 
	Defines flash map information
	

Length: #_ST_FLASHMAPDETAILS_

Default: NA

Range: NA
*/

#define BCM_STATS_FLASH_MAP_INFO		1673


/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	- This MIB is updated by baseband modem only in CD-Less boot mode of USB interface
	- These MIBs are read only from the host

Description : 
	SCSI firmware version.	

Length: sizeof(UINT32)

Default: NA

Range: NA
*/

#define BCM_STATS_SCSI_FW_VERSION		1674


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description :
	Gives the various types of CS supported by the MS. A value of 1 indicates the each CS is supported.

	- Bit 0: 802.3/Ethernet
	- Bit 1: Reserved
	- Bit 2: 802.3/Ethernet IPv4 Traffic only 
	- Bit 3: 802.3/Ethernet IPv6 Traffic only
	- Bits 4-31: Reserved
		
Length: sizeof(UINT32)

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_ETH_CS_SUPPORT	1675

/**
@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	- These MIBs are read only from the host

Description : 
	Gives value of external device voltage on channel 1 connected to  AMUXN_VAIN[1] Pin A3.

Length: sizeof(UINT32)

Default: NA

Range: 0-255
*/

#define BCM_BASE_BAND_AMUXN_VAIN1	1676


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Tx power in 0.5 dB step
	
		
Length: SINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_TX_POWER_PT5DB  1677

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Tx power headroom in 0.5 dB step
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_TX_POWER_HEADROOM_PT5DB     1678

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of RNG-RSPs received for Initial rang code sent
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_RNGRSP_FOR_IR_CODE     1679
/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of IR codes for which no RNG-RSP has been received
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_NO_RNGRSP_FOR_IR_CODE	    1680

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of RNG-RSP received for PR code
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_RNGRSP_FOR_PR_CODE        1681

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of PR codes for which no RNG-RSP is received
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_NO_RNGRSP_FOR_PR_CODE	1682


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of HO-IND with success
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_HO_IND_SUCCESS    1683


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Time from Initial RNG Reques to REG-RSP
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_IR_REQ_TO_REG_RESP_TIME   1684

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of REG-RSP received
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_NUM_OF_REG_RESP_RCVD            1685


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of IRNG-REQ for which no REG-RSP was received
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_NUM_IR_REG_WITH_NO_REG_RSP    1686
/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Num of Init RNG-REQ since device reset	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_TO_NUM_IR_REG_SINCE_RESET 1687
/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Baseband temperature
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_TEMPERATURE   1688

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Time in seconds since power cycle
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_SECS_SINCE_POWER_CYCLE      1689

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : Number of retransmitted ARQ blocks
	
		
Length: UINT32

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_ARQ_NUM_RE_TX_BLOCKS  1690


/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : structure that gives list of scheduling types and DL CIDs in each type
	
		
Length: sizeof(#_ST_PER_SF_INFO)*#NUMBER_OF_DL_UL_SF_INFO

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_DL_SF_INFO	1691

/**

@ingroup mib_statistics_proprietary

Type: Beceem Proprietary Statistics MIBs
	
MIB Category:
	These MIBs are read only from the host

Description : structure that gives list of scheduling types and UL CIDs in each type
	
		
Length:  sizeof(#_ST_PER_SF_INFO)*#NUMBER_OF_DL_UL_SF_INFO

Default: NA

Range: NA
*/

#define BCM_BASE_BAND_UL_SF_INFO	1692


#endif /* _BCM_MIB_USER_API_H_ */
