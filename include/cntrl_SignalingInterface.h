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
 * Description	:	Control Signalling Interface Structures and Defines
 * Author		:
 * Reviewer		:
 *
 */


#ifndef CNTRL_SIGNALING_INTERFACE_
#define CNTRL_SIGNALING_INTERFACE_


#ifdef BECEEM_TARGET

#include <mac_common.h>
#include <msg_Dsa.h>
#include <msg_Dsc.h>
#include <msg_Dsd.h>
#include <sch_definitions.h>
using namespace Beceem;

#else

#define B_UINT8 unsigned char
#define B_UINT16 unsigned short
#define B_UINT32 unsigned int
#define B_SINT8  char
#define B_SINT16  short
#define B_SINT32  int

#define DSA_REQ 11
#define DSA_RSP 12
#define DSA_ACK 13
#define DSC_REQ 14
#define DSC_RSP 15
#define DSC_ACK 16
#define DSD_REQ 17
#define DSD_RSP 18
#define DSD_ACK 19
#define MAX_CLASSIFIERS_IN_SF  4 

#endif

#define MAX_STRING_LEN 20
#define MAX_PHS_LENGTHS 255
#define VENDOR_PHS_PARAM_LENGTH 10
#define MAX_NUM_ACTIVE_BS 10
#define AUTH_TOKEN_LENGTH	10
#define NUM_HARQ_CHANNELS	16	//Changed from 10 to 16 to accomodate all HARQ channels
#define VENDOR_CLASSIFIER_PARAM_LENGTH 1 //Changed the size to 1 byte since we do not use it
#define  VENDOR_SPECIF_QOS_PARAM 1
#define VENDOR_PHS_PARAM_LENGTH	10
#define MBS_CONTENTS_ID_LENGTH	10
#define GLOBAL_SF_CLASSNAME_LENGTH 6

#define TYPE_OF_SERVICE_LENGTH				3	
#define IP_MASKED_SRC_ADDRESS_LENGTH			32
#define IP_MASKED_DEST_ADDRESS_LENGTH		32
#define PROTOCOL_SRC_PORT_RANGE_LENGTH		4
#define PROTOCOL_DEST_PORT_RANGE_LENGTH		4
#define ETHERNET_DEST_MAC_ADDR_LENGTH		12
#define ETHERNET_SRC_MAC_ADDR_LENGTH		12	
#define NUM_ETHERTYPE_BYTES  3
#define NUM_IPV6_FLOWLABLE_BYTES 3


    
////////////////////////////////////////////////////////////////////////////////
////////////////////////Structure Definitions///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// \brief class cCPacketClassificationRule
#ifdef BECEEM_TARGET
class CCPacketClassificationRuleSI{
	public:
		/// \brief Constructor for the class
	CCPacketClassificationRuleSI():
		u8ClassifierRulePriority(mClassifierRulePriority),
		u8IPTypeOfServiceLength(mIPTypeOfService),	
		u8Protocol(mProtocol),
		u8IPMaskedSourceAddressLength(0),
		u8IPDestinationAddressLength(0),
		u8ProtocolSourcePortRangeLength(0),
		u8ProtocolDestPortRangeLength(0),
		u8EthernetDestMacAddressLength(0),
		u8EthernetSourceMACAddressLength(0),
		u8EthertypeLength(0),
		u16UserPriority(mUserPriority),
		u16VLANID(mVLANID),
		u8AssociatedPHSI(mAssociatedPHSI),
		u16PacketClassificationRuleIndex(mPacketClassifierRuleIndex),
		u8VendorSpecificClassifierParamLength(mVendorSpecificClassifierParamLength),	
		u8IPv6FlowLableLength(mIPv6FlowLableLength),
		u8ClassifierActionRule(mClassifierActionRule)
        		
		{}
#else
struct _stCPacketClassificationRuleSI{
#endif

	/**  
	Description: 
	16bit UserPriority of the service flow

	Length: UINT16

	Default: 0

	Range: 0 to 7
	
	Usage: NOT mandatory	
	*/
    B_UINT16                        u16UserPriority;

	/**  
	Description: 
	16bit VLANID of the service flow

	Length: UINT16

	Default: 0

	Range: NA

	Usage: NOT mandatory
	*/    
	B_UINT16                        u16VLANID;
	
	/**  
	Description: 
	16bit Packet classification rule index of the service flow

	Length: UINT16

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/    
	B_UINT16                        u16PacketClassificationRuleIndex;
	
	/**  
	Description: 
	8bit Classifier rule priority of the service flow

	Length: UINT8

	Default: 0

	Range: 0 - 255

	Usage: Not Mandatory
	*/ 
	B_UINT8                         u8ClassifierRulePriority;

	/**  
	Description: 
	Length of IP TypeOfService field

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/ 	
	B_UINT8                         u8IPTypeOfServiceLength;

	/**  
	Description: 
	3bytes IP TypeOfService

	Length: UINT8 array of length TYPE_OF_SERVICE_LENGTH = 3

	Default: NA

	Range: NA

	Usage: NOT mandatory
	*/ 		
	B_UINT8                         u8IPTypeOfService[TYPE_OF_SERVICE_LENGTH];

	/**  
	Description: 
	Protocol used in the classification of service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage : NOT mandatory
	*/ 			
    B_UINT8                         u8Protocol;

	/**  
	Description: 
	Length of IP masked source address

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/ 				
	B_UINT8                         u8IPMaskedSourceAddressLength;

	/**  
	Description: 
	IP masked source address used in classification for the service flow

	Length: UINT8 Array of length IP_MASKED_SRC_ADDRESS_LENGTH = 32

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/ 					
	B_UINT8                         u8IPMaskedSourceAddress[IP_MASKED_SRC_ADDRESS_LENGTH];

	/**  
	Description: 
	Length of IP destination address

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/ 					
	B_UINT8                         u8IPDestinationAddressLength;

	/**  
	Description: 
	IP destination address used in classification for the service flow

	Length: UINT8 Array of length IP_MASKED_DEST_ADDRESS_LENGTH = 32

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/ 						
	B_UINT8                         u8IPDestinationAddress[IP_MASKED_DEST_ADDRESS_LENGTH];

	/**  
	Description: 
	Length of protocol source port range

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/ 							
	B_UINT8                         u8ProtocolSourcePortRangeLength;

	/**  
	Description: 
	Protocol source port range used in the service flow

	Length: UINT8 array of length PROTOCOL_SRC_PORT_RANGE_LENGTH = 4

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/ 								
	B_UINT8                         u8ProtocolSourcePortRange[PROTOCOL_SRC_PORT_RANGE_LENGTH];	

	/**  
	Description: 
	Length of protocol dest port range

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/	
	B_UINT8                         u8ProtocolDestPortRangeLength;

	/**  
	Description: 
	Protocol dest port range used in the service flow

	Length: UINT8 array of length PROTOCOL_DEST_PORT_RANGE_LENGTH = 4

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/		
	B_UINT8                         u8ProtocolDestPortRange[PROTOCOL_DEST_PORT_RANGE_LENGTH];
	
	/**  
	Description : 
	Length of Ethernet destination MAC address field

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/		
	B_UINT8                         u8EthernetDestMacAddressLength;
	
	/**  
	Description: 
	Ethernet destination MAC address used in classification of the service flow

	Length: UINT8 Array of length ETHERNET_DEST_MAC_ADDR_LENGTH = 12

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/		
	B_UINT8                         u8EthernetDestMacAddress[ETHERNET_DEST_MAC_ADDR_LENGTH];
	
	/**  
	Description: 
	Length of Ethernet source MAC address field

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not Mandatory
	*/		
	B_UINT8                         u8EthernetSourceMACAddressLength;
	
	/**  
	Description: 
	Ethernet source MAC address used in classification of the service flow

	Length: UINT8 array of length ETHERNET_SRC_MAC_ADDR_LENGTH = 12

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/			
	B_UINT8                         u8EthernetSourceMACAddress[ETHERNET_SRC_MAC_ADDR_LENGTH];
	
	/**  
	Description: 
	Length of Ethertype field

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/			
	B_UINT8                         u8EthertypeLength;
	
	/**  
	Description: 
	3bytes Ethertype of the service flow

	Length: UINT8 array of length NUM_ETHERTYPE_BYTES = 3

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/			
	B_UINT8                         u8Ethertype[NUM_ETHERTYPE_BYTES];
	
	/**  
	Description: 
	8bit Associated PHSI of the service flow

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Required if PHS rule is associated with the PCR
	*/				
	B_UINT8                         u8AssociatedPHSI;
	
	/**  
	Description: 
	Length of vendor specific classifier param length of the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/					
	B_UINT8                         u8VendorSpecificClassifierParamLength;
	
	/**  
	Description: 
	Vendor specific classifier param of the service flow

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/						
	B_UINT8                         u8VendorSpecificClassifierParam[VENDOR_CLASSIFIER_PARAM_LENGTH];	
	
	/**  
	Description: 
	Length Of IPv6 Flow lable of the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Set to number of bytes in IPV6
	*/							
	B_UINT8                         u8IPv6FlowLableLength;
	
	/**  
	Description: 
	IPv6 Flow lable of the service flow

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/								
	B_UINT8                         u8IPv6FlowLable[NUM_IPV6_FLOWLABLE_BYTES];    
	
	/**  
	Description: 
	Action associated with the classifier rule

	Length: UINT8

	Default: 0

	Range: 0 or 1

	Usage: Not Mandatory
	*/									
	B_UINT8							u8ClassifierActionRule;	      	

/**  
	Description: 
		Validity enable/disable field for SF

	Length: UINT16

	Default: 0

	Range: NA

	Usage:
	Bit 0: PKT_CLASSIFICATION_USER_PRIORITY_VALID
	Bit 1: PKT_CLASSIFICATION_VLANID_VALID                           
	*/											
	B_UINT16                         u16ClassificationsValid;		
	
};
#ifndef BECEEM_TARGET
typedef struct _stCPacketClassificationRuleSI CCPacketClassificationRuleSI,stCPacketClassificationRuleSI, *pstCPacketClassificationRuleSI;
#endif
    
/// \brief class CPhsRuleSI
#ifdef BECEEM_TARGET
class CPhsRuleSI{
	public:
		/// \brief Constructor for the class
		CPhsRuleSI():
			u8PHSI(mPHSI),
			u8PHSFLength(0),
			u8PHSMLength(0),
			u8PHSS(mPHSS),
			u8PHSV(mPHSV),
			u8VendorSpecificPHSParamsLength(mVendorSpecificPHSParamLength){}
#else
typedef struct _stPhsRuleSI {
#endif

	/**  
	Description: 
	8bit PHS index of the service flow

	Length: UINT8

	Default: 0

	Range: 1 to 255

	Usage: Not Mandatory
	*/    
	B_UINT8                         u8PHSI;

	/**  
	Description: 
	8bit Length of PHSF 

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/    
	B_UINT8                         u8PHSFLength;

	/**  
	Description: 
	String of bytes containing header information to be supressed by the sending CS and reconstructed by the receiving CS

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/    	
	B_UINT8                         u8PHSF[MAX_PHS_LENGTHS];

	/**  
	Description: 
	Length of the PHS mask 

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/    		
	B_UINT8                         u8PHSMLength;

	/**  
	Description: 
	PHS mask for the SF

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/    			
	B_UINT8                         u8PHSM[MAX_PHS_LENGTHS];

	/**  
	Description: 
	8bit Total number of bytes to be supressed for the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not Mandatory
	*/    				
	B_UINT8                         u8PHSS;

	/**  
	Description: 
	8bit Indicates whether or not packet header contents need to be verified prior to supression

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/    					
	B_UINT8                         u8PHSV;

	/**  
	Description: 
	Length of Vendor specific PHS param of the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not Mandatory
	*/    						
	B_UINT8                         u8VendorSpecificPHSParamsLength;

	/**  
	Description: 
	Vendor specific PHS param of the service flow

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not mandatory
	*/    							
	B_UINT8                         u8VendorSpecificPHSParams[VENDOR_PHS_PARAM_LENGTH];    
	B_UINT8                         u8Padding[2];
	
#ifdef BECEEM_TARGET
};
#else
}stPhsRuleSI,*pstPhsRuleSI;
typedef stPhsRuleSI CPhsRuleSI;
#endif
    
/// \brief structure cConvergenceSLTypes
#ifdef BECEEM_TARGET
class CConvergenceSLTypes{
	public:
		/// \brief Constructor for the class
		CConvergenceSLTypes():
		u8ClassfierDSCAction(mClassifierDSCAction),	
		u8PhsDSCAction	(mPhsDSCAction)
		{}
#else
struct _stConvergenceSLTypes{	
#endif

	/**  
	Description: 
	8bit Classfier DSC action of the service flow

	Length: UINT8

	Default: 0xFF

	Range: NA

	Usage: NOT mandatory
	*/    
	B_UINT8                         u8ClassfierDSCAction;  

	/**  
	Description: 
	8bit PHS DSC action of the service flow

	Length: UINT8

	Default: 0xFF

	Range: NA

	Usage: NOT Mandatory
	*/    
    B_UINT8                         u8PhsDSCAction;    

	/**  
	Description: 
	16bit padding

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Not mandatory
	*/	
	B_UINT8                         u8Padding[2];     
	
    /// \brief class cCPacketClassificationRule
#ifdef BECEEM_TARGET
    CCPacketClassificationRuleSI      cCPacketClassificationRule;
#else
    stCPacketClassificationRuleSI     cCPacketClassificationRule;
#endif
    /// \brief class CPhsRuleSI
#ifdef BECEEM_TARGET
    CPhsRuleSI				cPhsRule;
#else
     struct _stPhsRuleSI		cPhsRule;
  #endif
};
#ifndef BECEEM_TARGET
typedef struct _stConvergenceSLTypes stConvergenceSLTypes,CConvergenceSLTypes, *pstConvergenceSLTypes;
#endif
    
    
/// \brief structure CServiceFlowParamSI
#ifdef BECEEM_TARGET
class CServiceFlowParamSI{
	public:
		/// \brief Constructor for the class
		CServiceFlowParamSI():
			u32SFID(mSFid),
			u16CID(mCid),
			u8ServiceClassNameLength(mServiceClassNameLength),
			u8MBSService(mMBSService),
			u8QosParamSet(mQosParamSetType),
			u8TrafficPriority(mTrafficPriority),
			u32MaxSustainedTrafficRate(mMaximumSustainedTrafficRate),
			u32MaxTrafficBurst(mMaximumTrafficBurst),
			u32MinReservedTrafficRate(mMinimumReservedTrafficRate),
			u8ServiceFlowSchedulingType(mServiceFlowSchedulingType),
			u8RequesttransmissionPolicy(mRequestTransmissionPolicy),
			u32ToleratedJitter(mToleratedJitter),
			u32MaximumLatency(mMaximumLatency),
			u8FixedLengthVSVariableLengthSDUIndicator	
			(mFixedLengthVSVariableLength),
			u8SDUSize(mSDUSize),
			u16TargetSAID(mTargetSAID),
			u8ARQEnable(mARQEnable),						                  
			u16ARQWindowSize(mARQWindowSize),				                   
			u16ARQRetryTxTimeOut(mARQRetryTimeOutTxDelay),		               
			u16ARQRetryRxTimeOut(mARQRetryTimeOutRxDelay),				      
			u16ARQBlockLifeTime(mARQBlockLifeTime),						      
			u16ARQSyncLossTimeOut(mARQSyncLossTimeOut),			            
			u8ARQDeliverInOrder(mARQDeliverInOrder),				          
			u16ARQRxPurgeTimeOut(mARQRXPurgeTimeOut),
			u16ARQBlockSize(mARQBlockSize),							           
			u8RxARQAckProcessingTime(mRxARQAckProcessingTime),
			u8CSSpecification(mCSSpecification),				                
			u8TypeOfDataDeliveryService(mTypeOfDataDeliveryService),            
			u16SDUInterArrivalTime(mSDUInterArrivalTime),					   
			u16TimeBase(mTimeBase),								         
			u8PagingPreference(mPagingPreference),			                    
			u8MBSZoneIdentifierassignment(mMBSZoneIdentifierassignmentLength),		            
			u8TrafficIndicationPreference(mTrafficIndicationPreference),
			u8GlobalServicesClassNameLength(mGlobalServicesClassNameLength),
			u8SNFeedbackEnabled(mSNFeedbackEnabled),
			u8FSNSize(mFSNSize),
			u8CIDAllocation4activeBSsLength(mCIDAllocation4activeBSsLength),
			u16UnsolicitedGrantInterval(mUnsolicitedGrantInterval),
			u16UnsolicitedPollingInterval(mUnsolicitedPollingInterval),
			u8PDUSNExtendedSubheader4HarqReordering(mPDUSNExtendedSubheader4HarqReordering),
			u8MBSContentsIDLength(mMBSContentsIDLength),
			u8HARQServiceFlows(mHARQServiceFlows),
			u8AuthTokenLength(mAuthTokenLength),
			u8HarqChannelMappingLength(mHarqChannelMappingLength),			
			u8VendorSpecificQoSParamLength(mVendorSpecificQoSParamLength),
            bValid(FALSE),
			u8TotalClassifiers() 
{}		
    
	void ComputeMacOverhead(B_UINT8	u8SecOvrhead);
	B_UINT16	GetMacOverhead() { return 	u16MacOverhead; }
#else
typedef struct _stServiceFlowParamSI{
#endif //end of ifdef BECEEM_TARGET
	/**  
	Description: 
	32bit Service flow identifier

	Length: UINT32

	Default: 0 

	Range: 1 - 4,294,967,295

	Usage: To be assigned by the BS
	*/
    	B_UINT32                        u32SFID;
    
	/**  
	Description: 
	32bit Maximum sustained traffic rate of the service flow

	Length: UINT32

	Default: 0

	Range: Depends on the application rate

	Unit: Bits per second

	Usage: Needs to be filled by the user for relevant scheduling types
	*/    
    B_UINT32                        u32MaxSustainedTrafficRate;

	/**  
	Description: 
	32bit Maximum traffic burst allowed for the service flow

	Length: UINT32

	Default: 0

	Range: NA

	Unit: Bytes	
	
	Usage: Not mandatory
	*/    
    B_UINT32                        u32MaxTrafficBurst;

	/**  
	Description: 
	32bit Minimum reserved traffic rate of the service flow

	Length: UINT32

	Default: 0

	Range: NA

	Unit: Bits per second	

	Usage: To be filled by user for relevent scheduling types
	*/    
    B_UINT32                        u32MinReservedTrafficRate;
    
	/**  
	Description: 
	32bit Tolerated jitter of the service flow

	Length: UINT32

	Default: NA

	Range: NA

	Unit: milliseconds

	Usage: Mandatory for UGS and ERTPS
	*/    	
    B_UINT32                        u32ToleratedJitter;
    
	/**  
	Description : 
	32bit Maximum latency of the service flow

	Length: UINT32

	Default: 0xFFFFFFFF

	Range: NA

	Unit: milliseconds	

	Usage: Mandatory for UGS, RTPS and ERTPS
	*/    	   
	B_UINT32                        u32MaximumLatency;
    
	/**  
	Description: 
	16bit CID of the service flow

	Length: UINT16

	Default: 0

	Range: NA

	Usage: To be assigned by the BS 
	*/    	   	
	B_UINT16                        u16CID;

	/**  
	Description: 
	16bit SAID on which the Service Flow Being Set up shall be Mapped

	
	Note:

	This field can be used in index based mode or non-index based mode based on 
	PKMV2Enable field in configuration file.


	- Index Based TargetSAID in MS initiated DSA REQs:

		prerequisite: PKMV2Enable = 0x3(1st bit set to 1)
		
		In  case the application would pass the index to the target SAID. The target SAID is 
		used to index into SAID table by MAC layer to get the actual SAID.
		If the requested said index is >= 0 && < 12 && the corresponding SAID 
		in the SAID Table is VALID(NOT 0Xffffffff),
		set the corresponding SAID in the MS initiated DSA REQ TargetSAID TLV([145/146].17) or else
		use the default 0xFFFF.
		
		Note that Index based Target SAID conversion will not be applied on the BS initiated DSA RSP.


	- Non Index based TargetSAID setting is configured:

		prerequisite : PKMV2Enable = 1

		Use the target SAID as such in MS initiated DSA REQ TargetSAID TLV([145/146].17). 
		Applications has to fill the actual SAID. 
	
	Length: UINT16

	Default: 0xFFFF

	Range: NA

	Usage: To be given by the BS
	*/    	   	     
    B_UINT16                        u16TargetSAID;

	/**  
	Description: 
	16bit ARQ window size

	Length: UINT16

	Default: 0

	Range: NA

	Usage: Mandatory for ARQ connections-user to fill this
	*/    	   	     
    B_UINT16                        u16ARQWindowSize;

	/**  
	Description: 
	16bit Total Tx delay including sending, receiving and processing delays

	Length: UINT16

	Default: 500(Corr2) and 5000(Corr1) 
	
	Range: NA

	Unit: microseconds

	Usage: With respect to MS, mandatory for ARQ enabled DL flows
	*/    	   	        
    B_UINT16                        u16ARQRetryTxTimeOut;

	/**  
	Description: 
	16bit Total Rx delay including sending, receiving and processing delays

	Length: UINT16

	Default: 600(Corr2) and 6000(Corr1)

	Range: NA

	Unit: microseconds

	Usage: With respect to MS, mandatory for ARQ enabled UL flows
	*/    	   	        
    B_UINT16                        u16ARQRetryRxTimeOut;

	/**  
	Description: 
	16bit ARQ block lifetime

	Length: UINT16

	Default: 0 -> Infinite

	Range: NA

	Unit: microseconds	

	Usage: NOT mandatory 
	*/    	   	            
    B_UINT16                        u16ARQBlockLifeTime;

	/**  
	Description: 
	16bit ARQ sync loss timeout

	Length: UINT16

	Default: 0 -> Infinite

	Range: NA

	Unit: microseconds		

	Usage: NOT mandatory 
	*/    	   	            
    B_UINT16                        u16ARQSyncLossTimeOut;

	/**  
	Description: 
	16bit ARQ purge timeout

	Length: UINT16

	Default: 0 -> Infinite

	Range: NA

	Unit: microseconds		

	Usage: NOT mandatory 
	*/    	   	            
	B_UINT16                        u16ARQRxPurgeTimeOut;

	/**  
	Description: 
	16bit Size of an ARQ block

	Length: UINT16

	Default: 0 for Corr1 and 0x64 for Corr2

	Range: NA

	Unit: Bytes		

	Usage: NOT mandatory 
	*/    	   	            
	B_UINT16                        u16ARQBlockSize;

	/**  
	Description: 
	16bit Nominal interval b/w consecutive SDU arrivals at MAC SAP

	Length: UINT16

	Default: 0

	Range: NA

	Unit: resolution of 0.5 ms

	Usage: NOT mandatory 
	*/    	   	            
	B_UINT16                        u16SDUInterArrivalTime;

	/**  
	Description: 
	16bit Specifies the time base for rate measurement

	Length: UINT16

	Default: 0

	Range: NA

	Unit: milliseconds

	Usage: NOT mandatory 
	*/    	   	            
	B_UINT16                        u16TimeBase;

	/**  
	Description: 
	16bit Interval b/w successive grant oppurtunities

	Length: UINT16

	Default: 0

	Range: NA

	Unit: milliseconds	

	Usage: To be filled by user for relevent scheduling types
	*/    	   	            	
	B_UINT16                        u16UnsolicitedGrantInterval;  

	/**  
	Description: 
	16bit Interval b/w successive polling grant opportunities

	Length: UINT16

	Default: 0

	Range: NA

	Unit : milliseconds		

	Usage: To be filled by user for relevent scheduling types
	*/    	   	            	
	B_UINT16						u16UnsolicitedPollingInterval;	

	/**  
	Description: 
	Reserved: Beceem internal use only

	Length: UINT16

	Default: None

	Range: None

	Usage: Beceem internal use only
	*/    	   	            	
	B_UINT16						u16MacOverhead;

	/**  
	Description: 
	16bit  MBS contents identifier array

	Length: Array of 10 Unsigned INT16

	Default: NA

	Range: NA

	Usage: NOT mandatory 
	*/    	   	            	
	B_UINT16						u16MBSContentsID[MBS_CONTENTS_ID_LENGTH]; 
   
	/**  
	Description: 
	Length of MBS contents identifier array

	Length: UINT16

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    	   	            	
	B_UINT8							u8MBSContentsIDLength;
	
	/**  
	Description: 
	Length of ServiceClassName of the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    	   	            	
    B_UINT8                         u8ServiceClassNameLength;

	/**  
	Description: 
	32bytes ServiceClassName of the service flow

	Length: UINT8 Array

	Default: NA

	Range: NA

	Usage: NOT mandatory 
	*/    	   	            	
	B_UINT8                         u8ServiceClassName[32];
	
	/**  
	Description: 
	8bit Indicates whether or not MBS service is requested for this serivce flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    
	B_UINT8							u8MBSService;
	
	/**  
	Description: 
	8bit QOS Parameter set specifies proper application of QoS paramters to provisioned, admitted and active sets

	Length: UINT8

	Default: 1

	Range: NA

	Usage : Right value to be filled by the user  
	*/        
    B_UINT8                         u8QosParamSet;
	
	/**  
	Description: 
	8bit Traffic priority of the service flow

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/       
    B_UINT8                         u8TrafficPriority;

	/**  
	Description: 
	8bit Uplink grant scheduling type of the service flow

	Length: UINT8

	Default: 2 -> BE

	Range: 2 to 6

	Usage: NOT mandatory 
	*/          
	B_UINT8                         u8ServiceFlowSchedulingType;

	/**  
	Description: 
	8bit Request transmission policy of the service flow

	Length: UINT8

	Default: 16

	Range: NA

	Usage: NOT mandatory 
	*/    
	B_UINT8							u8RequesttransmissionPolicy; 

	/**  
	Description: 
	8bit Specifies whether SDUs for this service flow are of fixed length or variable length

	Length: UINT8

	Default: 0 -> Variable Length

	Range: 0 or 1

	Usage: NOT mandatory 
	*/    	
    B_UINT8                         u8FixedLengthVSVariableLengthSDUIndicator;

	/**  
	Description: 
	8bit Number of bytes of the SDU for a fixed length SDU service flow

	Length: UINT8

	Default: 49

	Range: NA

	Usage: NOT mandatory 
	*/    	
	B_UINT8                         u8SDUSize;

	/**  
	Description: 
	8bit Indicates whether or not ARQ is requested for this connection

	Length: UINT8

	Default: 0

	Range: NA

	Usage: Mandatory - to be filled in by user
	*/    	
    B_UINT8                         u8ARQEnable;

	/**  
	Description: 
	8bit Indicates whether or not data has to be delivered in order to higher layer

	Length: UINT8

	Default: 1

	Range: NA

	Usage: NOT mandatory 
	*/    			
    B_UINT8                         u8ARQDeliverInOrder;
	
	/**  
	Description: 
	8bit Receiver ARQ ACK processing time

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT Mandatory 
	*/    		    
	B_UINT8                         u8RxARQAckProcessingTime;	
	
	/**  
	Description: 
	8bit Convergence sublayer specification of the service flow

	Length: UINT8

	Default: 1 -> IPV4

	Range: NA

	Usage: Mandatory for DSA-REQ - to be filled by user
	*/    		   
    B_UINT8                         u8CSSpecification;

	/**  
	Description: 
	8 bit Type of data delivery service

	Length: UINT8

	Default: 0xFF

	Range: 0 to 4

	Usage: To be filled by user for relevent scheduling types  
	*/    		       	
	B_UINT8                         u8TypeOfDataDeliveryService;

	/**  
	Description: 
	8bit Specifies whether a service flow may generate paging

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           	
	B_UINT8                         u8PagingPreference;
   
	/**  
	Description: 
	8bit Indicates the MBS zone through which the connection or virtual connection is valid

	Length: UINT8

	Default: 0

	Range: NA

	Usage : NOT mandatory 
	*/    		           
    B_UINT8                         u8MBSZoneIdentifierassignment; 
    
	/**  
	Description: 
	8bit Specifies whether traffic on SF should generate MOB_TRF_IND to MS in sleep mode

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8                         u8TrafficIndicationPreference; 
    
	/**  
	Description: 
	8bit Specifies the length of global service class name array
	
	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8                         u8GlobalServicesClassNameLength;	
    
	/**  
	Description: 
	6 bytes -Specifies the predefined Global QoS parameter set encoding for this SF

	Length: UINT8 Array of length GLOBAL_SF_CLASSNAME_LENGTH = 6

	Default: NA

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8                         u8GlobalServicesClassName[GLOBAL_SF_CLASSNAME_LENGTH]; 
    
	/**  
	Description: 
	8bit Indicates whether or not SN feedback is enabled for the connection

	Length: UINT8

	Default: 0 -> Disabled

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8                         u8SNFeedbackEnabled;	
    
	/**  
	Description: 
	Indicates the size of the fragment sequence number for the connection

	Length: UINT16

	Default: 1 -> 11 bit FSN 

	Range: NA

	Usage: NOT Mandatory 
	*/    		           
	B_UINT8                         u8FSNSize;  
    
	/**  
	Description: 
	Number of CIDs allocated in the active BS list

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8							u8CIDAllocation4activeBSsLength;	
   
	/**  
	Description: 
	8bit List of CIDs in the active BS list

	Length: UINT8 Array of length MAX_NUM_ACTIVE_BS = 10

	Default: NA

	Range: NA

	Usage: NOT Mandatory 
	*/    		           
	B_UINT8							u8CIDAllocation4activeBSs[MAX_NUM_ACTIVE_BS];
    
	/**  
	Description: 
	Specifies if PDU extended subheader should be applied on every PDU on this connection

	Length: UINT8

	Default: 0 -> No support for PDU SN

	Range: NA

	Usage: NOT Mandatory 
	*/    		           
	B_UINT8                         u8PDUSNExtendedSubheader4HarqReordering; 
    
	/**  
	Description: 
	8bit Specifies whether the connection uses HARQ or not

	Length: UINT8

	Default: 0 -> NON-HARQ

	Range: NA

	Usage: NOT Mandatory 
	*/    		           
	B_UINT8                         u8HARQServiceFlows; 
    
	/**  
	Description: 
	8bit Specifies the length of authorization token

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           
	B_UINT8							u8AuthTokenLength;
    
	/**  
	Description: 
	Specifies the authorization token

	Length: UINT8 Array of length AUTH_TOKEN_LENGTH = 10

	Default: NA

	Range: NA

	Usage: Relevant for MBS enabled service flows.
	*/    		           
 	B_UINT8							u8AuthToken[AUTH_TOKEN_LENGTH];

	/**  
	Description: 
	Specifies number of HARQ channels used to carry data length

	Length: UINT8

	Default: 0

	Range: NA

	Usage: NOT mandatory 
	*/    		           	
	B_UINT8							u8HarqChannelMappingLength;
	
	/**  
	Description: 
	Specifies the HARQ channel mapping 

	Length: UINT8 Array of length NUM_HARQ_CHANNELS = 16

	Default: NA

	Range: NA

	Usage: NOT mandatory 
	*/    		           		
	B_UINT8							u8HARQChannelMapping[NUM_HARQ_CHANNELS]; 
	
	/**  
	Description: 
	8bit Length of vendor specific QoS params 

	Length: UINT8 Array

	Default: 0

	Range: NA

	Usage: NOT Mandatory 
	*/    		           			
    B_UINT8                         u8VendorSpecificQoSParamLength;

	/**  
	Description: 
	 1byte  Vendor specific QoS param of the service flow

	Length: UINT8 Array of length VENDOR_SPECIF_QOS_PARAM = 1

	Default: NA

	Range: NA

	Usage: NOT mandatory 
	*/    		           				
    B_UINT8                          u8VendorSpecificQoSParam[VENDOR_SPECIF_QOS_PARAM];

	// indicates total classifiers in the SF
	B_UINT8                         u8TotalClassifiers;  /**< Total number of valid classifiers*/
	
	B_UINT8							bValid;/**<  Validity flag */

	/**  
	Description: 
	1byte application specified tolerable packet error rate. Can be sent
	 in MS-init DSA-REQ /DSC-REQ.

	@image html TolerablPacketerrorRate.JPG

	 
	Length: UINT8 

	Default: 254

	Range: 0(Min) to 11(Max)

	Usage: Not used currently 
	*/ 		           				
	
	B_UINT8				u8AppTolerablePer;	 

#ifdef BECEEM_TARGET
/**
Structure for Convergence SubLayer Types with a maximum of 4 classifiers
*/
	CConvergenceSLTypes		cConvergenceSLTypes[MAX_CLASSIFIERS_IN_SF];
#else
/**
Structure for Convergence SubLayer Types with a maximum of 4 classifiers
*/
	stConvergenceSLTypes		cConvergenceSLTypes[MAX_CLASSIFIERS_IN_SF];
#endif

#ifdef BECEEM_TARGET    
};
#else
} stServiceFlowParamSI, *pstServiceFlowParamSI;
typedef stServiceFlowParamSI CServiceFlowParamSI;
#endif

/**
structure stLocalSFAddRequest
*/
typedef struct _stLocalSFAddRequest{
#ifdef BECEEM_TARGET
	   _stLocalSFAddRequest( ) :
	   	u8Type(0x00),  eConnectionDir(0x00), 
		u16TID(0x0000), u16CID(0x0000),  u16VCID(0x0000)
	   		{}
#endif
	   	
	/**  
	Description: 
	8bit Type of the message sent

	Length: UINT8

	Default: NA

	Range: 
	 - DSA_REQ(11)

	Usage: To be filled by the user
	*/    	   	
	B_UINT8                         u8Type;
	
	/**  
	Description: 
	8bit Direction of the connection 

	Length: UINT8

	Default: NA

	Range: 
		0 - Downlink
		1-  Uplink

	Usage: To be filled by the User
	*/    	   	
	B_UINT8      eConnectionDir;		

	/**  
	Description: 
	16bit Transaction identifier

	Length: UINT16

	Default: NA

	Range: 0 to 0x7FFF for MS-initiated and 0x8000 to 0xFFFF fro BS-initiated

	Usage: To be assigned by the application for each new transaction
	*/    	   		
	B_UINT16                        u16TID;/**<  16bit TID*/

	/**  
	Description: 
	16bit Connection identifier

	Length: UINT16

	Default: NA

	Range: NA

	Usage: Should be assigned by the BS
	*/    	   			
    B_UINT16                        u16CID; /**<  16bit CID*/
	
	/**  
	Description: 
	16bit Virtual connection identifier

	Length: UINT16

	Default: NA

	Range: 4 to 19

	Usage: Should be assigned by the Firmware
	*/    	 
	B_UINT16                        u16VCID;	/**<  16bit VCID*/	
    
#ifdef BECEEM_SIGNALLING_INTERFACE_API
	stServiceFlowParamSI sfParameterSet;/**<  structure ParameterSet*/
#endif
    
#ifdef BECEEM_TARGET
    CServiceFlowParamSI              *psfParameterSet;
#else
	stServiceFlowParamSI	*psfParameterSet;/**<  This field should not be used by the appplication*/	
#endif
    
#ifdef USING_VXWORKS
    USE_MEMORY_MANAGER();
#endif
}stLocalSFAddRequest, *pstLocalSFAddRequest;


/** 
structure stLocalSFAddIndication
*/
typedef struct _stLocalSFAddIndication{
#ifdef BECEEM_TARGET
	   _stLocalSFAddIndication( ) :
	   	u8Type(0x00),  eConnectionDir(0x00), 
		u16TID(0x0000), u16CID(0x0000),  u16VCID(0x0000)
	   		{}
#endif

	/**  
	Description: 
	8bit Type of the message sent

	Length: UINT8

	Default: NA

	Range: 
	 - DSA_REQ(11)
	 - DSA_RSP(12)
	 - DSA_ACK(13)
	 - DSC_REQ(14)
	 - DSC_RSP(15)
	 - DSC_ACK(16)

	Usage: To be filled by the User
	*/    	   	
    B_UINT8                         u8Type;

	/**  
	Description: 
	8bit Direction of the connection 

	Length: UINT8

	Default: NA

	Range: 0, 1 or 2

	Usage: To be filled by the User
	*/    	   	
	B_UINT8      eConnectionDir;

	/**  
	Description : 
	16bit Transaction identifier

	Length: UINT16

	Default: NA

	Range: 0 to 0x7FFF for MS-initiated and 0x8000 to 0xFFFF fro BS-initiated

	Usage: To be assigned by the application for each new transaction
	*/    	   			
	B_UINT16                         u16TID;

	/**  
	Description: 
	16bit Connection identifier

	Length: UINT16

	Default: NA

	Range: NA

	Usage: Should be assigned by the BS
	*/    	 
    B_UINT16                        u16CID;
	
	/**  
	Description: 
	16bit Virtual connection identifier

	Length: UINT16

	Default: NA

	Range: 4 to 19

	Usage: Should  be assigned by the Firmware
	*/    	 
    B_UINT16                        u16VCID; 
   
#ifdef 	BECEEM_SIGNALLING_INTERFACE_API
	stServiceFlowParamSI              sfAuthorizedSet;/**<  AuthorizedSet of type #_stServiceFlowParamSI*/	
    stServiceFlowParamSI              sfAdmittedSet;/**<  AdmittedSet of type #_stServiceFlowParamSI*/
    stServiceFlowParamSI              sfActiveSet;/**<  sfActiveSet of type #_stServiceFlowParamSI*/
#endif
   
    
#ifdef BECEEM_TARGET
    CServiceFlowParamSI              *psfAuthorizedSet;
    /// \brief structure AdmittedSet
    CServiceFlowParamSI              *psfAdmittedSet;
    /// \brief structure ActiveSet
    CServiceFlowParamSI              *psfActiveSet;
#else
    stServiceFlowParamSI              *psfAuthorizedSet;/**<  This field should not be used by the appplication*/			
    stServiceFlowParamSI              *psfAdmittedSet;	/**<  This field should not be used by the appplication*/	
    stServiceFlowParamSI              *psfActiveSet;	/**<  This field should not be used by the appplication*/	

#endif
	/**  
	Description: 
	8bit Confirmation code
	In the case of request from the MS being rejected by the base station, this field will contain the reason for rejection.
	MS f/w also sets this to non-zero value to indicate rejection of flow after validation checks of certain SF params.
	See below the values from 802.16 standards.
	
	Length: UINT8

	Default: NA

	Range: 0 to 17

	Usage: To be sent by the receipient of DSX-REQ/RSP

	@image html ConfirmationCodes.JPG
	
	*/    	
	B_UINT8							u8CC;	/**<  Confirmation Code*/	
	B_UINT8							u8Padd;		/**<  8-bit Padding */
	B_UINT16						u16Padd;	/**< 16 bit Padding */	
#ifdef USING_VXWORKS
    USE_MEMORY_MANAGER(); 
#endif
}stLocalSFAddIndication;


typedef struct _stLocalSFAddIndication *pstLocalSFAddIndication;
/**
structure stLocalSFChangeRequest is same as structure stLocalSFAddIndication
*/
typedef struct _stLocalSFAddIndication stLocalSFChangeRequest, *pstLocalSFChangeRequest;
/**
structure stLocalSFChangeIndication is same as structure stLocalSFAddIndication
*/
typedef struct _stLocalSFAddIndication stLocalSFChangeIndication, *pstLocalSFChangeIndication;

/**
structure stLocalSFDeleteRequest
*/
typedef struct _stLocalSFDeleteRequest{
#ifdef BECEEM_TARGET
	   _stLocalSFDeleteRequest( ) :
	   	u8Type(0x00),  u8Padding(0x00),
		u16TID(0x0000), u32SFID (0x00000000)
	   		{}
#endif

	/**  
	Description: 
	8bit Type of the message sent

	Length: UINT8

	Default: NA

	Range:
	 - DSD_REQ(17)

	Usage: To be filled by the user
	*/    	   	
	B_UINT8                         u8Type; 

	/**  
	Description: 
	8bit Padding

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not to be used
	*/    	  
	B_UINT8                         u8Padding;	 	

	/**  
	Description: 
	16bit Transaction identifier

	Length: UINT16

	Default: NA

	Range: 0 to 0x7FFF for MS-initiated and 0x8000 to 0xFFFF fro BS-initiated

	Usage: To be assigned by the application for each new transaction
	*/    	   			
	B_UINT16						u16TID;

	/**  
	Description: 
	32bit Service flow identifier

	Length: UINT32

	Default: NA

	Range: 1 - 4,294,967,295

	Usage: To be filled by the User
	*/    
    B_UINT32                        u32SFID;     
#ifdef USING_VXWORKS
    USE_MEMORY_MANAGER();
#endif
}stLocalSFDeleteRequest, *pstLocalSFDeleteRequest;

/**
structure stLocalSFDeleteIndication
*/
typedef struct stLocalSFDeleteIndication{
#ifdef BECEEM_TARGET
	   stLocalSFDeleteIndication( ) :
	   	u8Type(0x00),  u8Padding(0x00),
		u16TID(0x0000), u16CID(0x0000),  
		u16VCID(0x0000),u32SFID (0x00000000)
	   		{}
#endif

	/**  
	Description: 
	8bit Type of the message sent

	Length: UINT8

	Default: NA

	Range:
	 - DSD_REQ(17)
	 - DSD_RSP(18)

	Usage: To be filled by the user
	*/    	   	
	B_UINT8                         u8Type;	

	/**  
	Description: 
	8bit Padding

	Length: UINT8

	Default: NA

	Range: NA

	Usage: Not to be used
	*/    	  
	B_UINT8                         u8Padding;	

	/**  
	Description: 
	16bit Transaction identifier

	Length: UINT16

	Default: NA

	Range: 0 to 0x7FFF for MS-initiated and 0x8000 to 0xFFFF fro BS-initiated

	Usage: To be assigned by the application for each new transaction
	*/    			
	B_UINT16						u16TID;		

	/**  
	Description: 
	16bit Connection identifier

	Length: UINT16

	Default: NA

	Range: NA

	Usage: Should be sent from BS
	*/  	     
	B_UINT16                        u16CID;		

	/**  
	Description: 
	16bit Virtual connection identifier

	Length: UINT16

	Default: NA

	Range: 4 to 19

	Usage: should be assigned by the firmware
	*/    	     
	B_UINT16                        u16VCID;	

	/**  
	Description: 
	32bit Service flow identifier

	Length: UINT32

	Default: NA

	Range: 1 - 4,294,967,295

	Usage: To be filled by the User
	*/    
    B_UINT32                        u32SFID;	

	/**  
	Description: 
	8bit Confirmation code
	In the case of request from the MS being rejected by the base station this field will contain the reason for rejection.
	See below the values from 802.16 standards.
	
	Length: UINT8

	Default: NA

	Range: 0 to 17

	Usage : To be sent by the receipient of DSD-REQ

	@image html ConfirmationCodes.JPG
	
	*/    	
	B_UINT8                         u8ConfirmationCode;	

	/**  
	Description: 
	3 byte padding

	Length: UINT8 array of 3

	Default: NA

	Range: NA

	Usage: NOT to be used
	*/    	
	B_UINT8                         u8Padding1[3];		
#ifdef USING_VXWORKS
    USE_MEMORY_MANAGER();
#endif
}stLocalSFDeleteIndication;


#endif /* CNTRL_SIGNALING_INTERFACE_ */
