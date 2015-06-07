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
 * Description	:	OMA-DM tree structure related defines and structures.
 * Author		:
 * Reviewer		:
 *
 */



#ifndef BCM_OMADM_TREE_STRUCT_H
#define BCM_OMADM_TREE_STRUCT_H


#include "Typedefs.h"
/**
This file contains the basic OMA-DM tree structure as defined in WIMAX-OMA-DM-TreePopulation-version2.8

*/
#define MANUFATURER_NAME_LEAF_SIZE	64
#define DEVICE_ID_LEAF_SIZE	16
#define MODEL_NUMBER_LEAF_SIZE  32
#define OMA_DM_CLIENT_VERSION_LEAF_SIZE 24
#define LANGUAGE_LEAF_SIZE    48

typedef struct ST_DevInfo_
{
/*Ext and bearer nodes are not supported*/
	UINT8 DevId[DEVICE_ID_LEAF_SIZE];
	UINT8 Man[MANUFATURER_NAME_LEAF_SIZE];
	UINT8 Mod[MODEL_NUMBER_LEAF_SIZE ];
	UINT8 Dmv[OMA_DM_CLIENT_VERSION_LEAF_SIZE];
	UINT8 Lang[LANGUAGE_LEAF_SIZE];
}ST_DevInfo,*PST_DevInfo;


#define CONNECTION_REFERENCES_LEAF_SIZE 128
typedef struct ST_ToConRef_
{
	UINT8 ConRef[CONNECTION_REFERENCES_LEAF_SIZE];
}ST_ToConRef,*PST_ToConRef;

#define PORT_NUMBER_LEAF_SIZE 8
typedef struct ST_Port_
{
	UINT8 PortNbr[PORT_NUMBER_LEAF_SIZE];
}ST_Port,*PST_Port;

#define APP_ADDRESS_LEAF_SIZE 128
#define APP_ADDRESS_TYPE_LEAF_SIZE 8
#define APP_PORT_NUMBER_OF_NODES 1

typedef struct ST_AppAddr_
{

	UINT8 Addr[APP_ADDRESS_LEAF_SIZE];
	

	UINT8 AddrType[APP_ADDRESS_TYPE_LEAF_SIZE];	


	ST_Port Port[APP_PORT_NUMBER_OF_NODES];
	
}ST_AppAddr,*PST_AppAddr;


#define APP_AUTHLEVEL_LEAF_SIZE 24
#define APP_AUTHTYPE_LEAF_SIZE 24
#define APP_AUTHNAME_LEAF_SIZE 64
#define APP_AUTHSECRET_LEAF_SIZE 32
#define APP_AUTHDATA_LEAF_SIZE 128


typedef struct ST_AppAuth_
{	

	
	UINT8 AAuthLevel[APP_AUTHLEVEL_LEAF_SIZE];

	UINT8 AAuthType[APP_AUTHTYPE_LEAF_SIZE];

	UINT8 AAuthName[APP_AUTHNAME_LEAF_SIZE];

	UINT8 AAuthSecret[APP_AUTHSECRET_LEAF_SIZE];

	UINT8 AAuthData[APP_AUTHDATA_LEAF_SIZE];

}ST_AppAuth,*PST_AppAuth;

#define DMACC_APPID_LEAF_SIZE 16
#define DMACC_SERVERID_LEAF_SIZE 64
#define DMACC_NAME_LEAF_SIZE 64
#define DMACC_PREFCONREF_LEAF_SIZE 64
#define DMACC_TOCONNECTIONREF_NUMBER_OF_NODES 1
#define DMACC_APPADDR_NUMBER_OF_NODES 1
#define DMACC_AUTHPREF_LEAF_SIZE  64
#define DMACC_APPAUTH_NUMBER_OF_NODES 2


typedef struct ST_DMAcc_
{
	
	UINT8 AppID[DMACC_APPID_LEAF_SIZE];

	UINT8 ServerID[DMACC_SERVERID_LEAF_SIZE];

	UINT8 Name[DMACC_NAME_LEAF_SIZE];

	UINT8 PrefConRef[DMACC_PREFCONREF_LEAF_SIZE];

	ST_ToConRef ToConRef[DMACC_TOCONNECTIONREF_NUMBER_OF_NODES];

	ST_AppAddr AppAddr[DMACC_APPADDR_NUMBER_OF_NODES];

	UINT8 AAuthPref[DMACC_AUTHPREF_LEAF_SIZE];

	ST_AppAuth AppAuth[DMACC_APPAUTH_NUMBER_OF_NODES];

	/*Ext not supported*/

 }ST_DMAcc,*PST_DMAcc;

typedef struct ST_URI_
{
	UINT32 MaxDepth;
	UINT32 MaxTotLen;
	UINT32 MaxSegLen;
	
}ST_URI,*PST_URI;


#define DEVDETAIL_DEVTYPE_LEAF_SIZE 32
#define DEVDETAIL_OEM_LEAF_SIZE 48
#define DEVDETAIL_FWV_LEAF_SIZE 32
#define DEVDETAIL_SWV_LEAF_SIZE 32
#define DEVDETAIL_HWV_LEAF_SIZE 32

typedef struct ST_DevDetail_
{
/*Ext and bearer nodes are not supported*/
ST_URI URI;
UINT8 DevType[DEVDETAIL_DEVTYPE_LEAF_SIZE];
UINT8 OEM[DEVDETAIL_OEM_LEAF_SIZE];
UINT8 FwV[DEVDETAIL_FWV_LEAF_SIZE];
UINT8 SwV[DEVDETAIL_SWV_LEAF_SIZE];
UINT8 HwV[DEVDETAIL_HWV_LEAF_SIZE];
UINT32 LrgObj;

}ST_DevDetail,*PST_DevDetail;


#define SPLOCK_OPERATOR_LEAF_SIZE 128
#define SPLOCK_LOCK_LEAF_SIZE 64

typedef struct ST_SPLock_
{
	UINT32      LockStatus;
	UINT8 Operator[SPLOCK_OPERATOR_LEAF_SIZE];
	UINT32  VerNbr;
	UINT8 Lock[SPLOCK_LOCK_LEAF_SIZE]; 
}ST_SPLock,*PSPLock;
#define WIMAXRADIO_MAN_LEAF_SIZE 24
#define WIMAXRADIO_MOD_LEAF_SIZE 64
#define WIMAXRADIO_FWV_LEAF_SIZE 24
#define WIMAXRADIO_HWV_LEAF_SIZE 24
#define WIMAXRADIO_SWV_LEAF_SIZE 24
#define WIMAXRADIO_MACADDR_LEAF_SIZE 6
#define WIMAXRADIO_TO_FUMO_REF_LEAF_SIZE 64

typedef struct ST_WIMAXRadioModule_
{
	UINT8 Man[WIMAXRADIO_MAN_LEAF_SIZE];
	UINT8 Mod[WIMAXRADIO_MOD_LEAF_SIZE];
	UINT8 FwV[WIMAXRADIO_FWV_LEAF_SIZE];
	UINT8 HwV[WIMAXRADIO_HWV_LEAF_SIZE];
	UINT8 SwV[WIMAXRADIO_SWV_LEAF_SIZE];

	UINT8 MACAddress[WIMAXRADIO_MACADDR_LEAF_SIZE];
	
	ST_SPLock SPLock;
	UINT8 TO_FUMO_REF[WIMAXRADIO_TO_FUMO_REF_LEAF_SIZE];
	
}ST_WIMAXRadioModule,*PWIMAXRadioModule;

#define TERMINALEQUIPMENT_DEVID_LEAF_SIZE 20
#define TERMINALEQUIPMENT_DEVTYPE_LEAF_SIZE 32
#define TERMINALEQUIPMENT_MAN_LEAF_SIZE 64
#define TERMINALEQUIPMENT_MOD_LEAF_SIZE 64
#define TERMINALEQUIPMENT_FWV_LEAF_SIZE 32
#define TERMINALEQUIPMENT_HWV_LEAF_SIZE 32
#define TERMINALEQUIPMENT_SWV_LEAF_SIZE 32

typedef struct ST_TerminalEquipment_
{
	/*Ext and bearer nodes are not supported*/
	UINT8 DevID[TERMINALEQUIPMENT_DEVID_LEAF_SIZE];
	UINT8 DevType[TERMINALEQUIPMENT_DEVTYPE_LEAF_SIZE];
	UINT8 Man[TERMINALEQUIPMENT_MAN_LEAF_SIZE];
	UINT8 Mod[TERMINALEQUIPMENT_MOD_LEAF_SIZE];
	UINT8 FwV[TERMINALEQUIPMENT_FWV_LEAF_SIZE];
	UINT8 HwV[TERMINALEQUIPMENT_HWV_LEAF_SIZE];
	UINT8 SwV[TERMINALEQUIPMENT_SWV_LEAF_SIZE];
	
}ST_TerminalEquipment,*PST_TerminalEquipment;


typedef struct ST_IPCap_
{
	UINT32 IPV4;
	UINT32 IPV6;
	UINT32 CMIPV4;
	UINT32 CMIPV6;
	
}ST_IPCap,*PST_IPCap;

typedef struct ST_ClientInitiated_
{
	UINT32 PollingSupported;
	UINT32 PollingInterval;
}ST_ClientInitiated,*PST_ClientInitiated;
typedef struct ST_UpdateMethods_
{
	UINT32 ServerInitiated;
	ST_ClientInitiated ClientInitiated;
}ST_UpdateMethods,*PST_UpdateMethods;


typedef struct ST_DevCap_
{
	ST_IPCap  IPCap;
	ST_UpdateMethods UpdateMethods;
		
}ST_DevCap,*PST_DevCap;



#define WIMAX_WIMAXRADIO_NUMBER_OF_NODES 1
#define WIMAX_WIMAX_REF_LEAF_SIZE 64
typedef struct ST_WIMAX_
{
	ST_WIMAXRadioModule WIMAXRadioModule[WIMAX_WIMAXRADIO_NUMBER_OF_NODES];

	ST_TerminalEquipment TerminalEquipment;
	
	UINT8				TO_WIMAX_REF[WIMAX_WIMAX_REF_LEAF_SIZE];

	ST_DevCap DevCap;
	
	
}ST_WIMAX,*PST_WIMAX;


#define MAX_CAPL_ENTRIES_PER_NSP		6
#define MAX_RAPL_ENTRIES_PER_NSP		4 
#define MAX_CHANNEL_PLAN_ENTRIES_PER_NSP  40
#define MAX_SIZE_OPERATORNAME  128 
#define MAX_HNSP_IDS_PER_OPERATOR  1
#define CHANNEL_PLAN_PREAMBLE_LEAF_SIZE 16
#define CHANNEL_REFID_LEAF_SIZE  8
typedef struct _ST_ChannelPlanEntries
{
UINT8  Id[CHANNEL_REFID_LEAF_SIZE];
UINT32 FirstFreq;
UINT32 LastFreq;
UINT32 NextFreqStep;
UINT8  Preambles[CHANNEL_PLAN_PREAMBLE_LEAF_SIZE];
UINT32 BW;
UINT32 FFTSize;
UINT32 DuplexMode;

}ST_ChannelPlanEntries,*PST_ChannelPlanEntries;

typedef struct _ST_ChannelPlan_
{
	ST_ChannelPlanEntries Entries[MAX_CHANNEL_PLAN_ENTRIES_PER_NSP];
	UINT32 BW;
	UINT32 FFTSize;
	UINT32 DuplexMode;

}ST_ChannelPlan,*PST_ChannelPlan;


typedef struct _ST_ChPlanRefIds_
{
	UINT8 RefId[CHANNEL_REFID_LEAF_SIZE];
}ST_ChPlanRefIds,*PST_ChPlanRefIds;


#define MAX_CHANNEL_PLAN_REF_ID_PER_CAPL 40
typedef struct _ST_CAPLEntries
{
	UINT32 NAP_ID;
	UINT32 Priority;
	ST_ChPlanRefIds  ChPlanRefIds[MAX_CHANNEL_PLAN_REF_ID_PER_CAPL];
		
}ST_CAPLEntries,*PST_CAPLEntries;


typedef struct _ST_CAPL
{
	ST_CAPLEntries Entries[MAX_CAPL_ENTRIES_PER_NSP];
	UINT32 Any;
}ST_CAPL,*PST_CAPL;

typedef struct _ST_RAPLEntries_
{
	UINT32 V_NSP_ID;
	UINT32 Priority;
		
}ST_RAPLEntries,*PST_RAPLEntries;

typedef struct _ST_RAPL
{
	ST_RAPLEntries Entries[MAX_RAPL_ENTRIES_PER_NSP];
	UINT32 Any;
}ST_RAPL,*PST_RAPL;

typedef struct  _ST_H_NSP_
{
	UINT32 H_NSP_ID;
}ST_H_NSP,*PST_H_NSP;

typedef struct  _ST_NetworkParameters_
{
	ST_H_NSP H_NSP[MAX_HNSP_IDS_PER_OPERATOR];
	ST_CAPL CAPL;
	ST_RAPL RAPL;
	ST_ChannelPlan ChannelPlan;
	UINT8 OperatorName[MAX_SIZE_OPERATORNAME];
	UINT32 PollingInterval;
}ST_NetworkParameters, *PST_NetworkParameters;



#define SERVER_REALM_LEAF_SIZE 128
typedef struct ST_SERVER_REALMS_
{
	UINT8 SERVER_REALM[SERVER_REALM_LEAF_SIZE];
}ST_SERVER_REALMS,*PST_SERVER_REALMS;

typedef struct _ST_EAP_AKA_
{
	UINT32 USE_CHECK_CODE;
}ST_EAP_AKA,*PST_EAP_AKA;

#define CERT_CERT_TYPE_LEAF_SIZE 32
typedef struct ST_CERT_
{
	UINT8 CERT_TYPE[CERT_CERT_TYPE_LEAF_SIZE];
	UINT32 SER_NUM;
	UINT32 THUMBPRINT;
}ST_CERT,*PST_CERT;

#define EAP_USERIDENTITY_LEAF_SIZE 64
#define EAP_PROVISIONED_PSEUDO_IDENTITY_LEAF_SIZE 128
#define EAP_PASSWORD_LEAF_SIZE 64
#define EAP_REALM_LEAF_SIZE 64
#define EAP_SERVER_REALMS_NUMBER_OF_NODE 1
#define EAP_CERT_NUMBER_OF_NODE 2

typedef struct  ST_EAP_
{
	UINT32 METHOD_TYPE;
	UINT32 VENDOR_ID;
	UINT32 VENDOR_TYPE;
	UINT8 UserIdentity[EAP_USERIDENTITY_LEAF_SIZE];
	UINT8 PROVISIONED_PSEUDO_IDENTITY[EAP_PROVISIONED_PSEUDO_IDENTITY_LEAF_SIZE];
	UINT8 PASSWORD[EAP_PASSWORD_LEAF_SIZE];
	UINT8 REALM[EAP_REALM_LEAF_SIZE];
	UINT8 USE_PRIVACY;
	UINT32 ENCAPS;
	UINT32 VFY_SERVER_REALM;
	ST_SERVER_REALMS SERVER_REALMS[EAP_SERVER_REALMS_NUMBER_OF_NODE];
	ST_EAP_AKA  EAP_AKA;
	ST_CERT CERT[EAP_CERT_NUMBER_OF_NODE];
}ST_EAP,*PST_EAP;

#define OTHERSUBSCRIPTIONS_EAP_NUMBER_OF_NODE 1
#define OTHERSUBSCRIPTIONS_NAME_LEAF_SIZE 64
typedef struct ST_OtherSubscriptions_
{
	UINT8 Name[OTHERSUBSCRIPTIONS_NAME_LEAF_SIZE];
	UINT32 Activated;
	ST_EAP EAP[OTHERSUBSCRIPTIONS_EAP_NUMBER_OF_NODE];
}ST_OtherSubscriptions,*PST_OtherSubscriptions;


#define PRIMARY_NAME_LEAF_SIZE 64
#define PRIMARY_EAP_NUMBER_OF_NODE 1

typedef struct ST_Primary_
{
	UINT8 Name[PRIMARY_NAME_LEAF_SIZE];
	UINT32 Activated;
	ST_EAP EAP[PRIMARY_EAP_NUMBER_OF_NODE];
}ST_Primary,*PST_Primary;

#define SUBSCRIPTIONPARAMETERS_OTHERSUBSCRIPTIONS_NUMBER_OF_NODES 5
typedef struct ST_SubscriptionParameters_
{
ST_Primary Primary;

ST_OtherSubscriptions OtherSubscriptions[SUBSCRIPTIONPARAMETERS_OTHERSUBSCRIPTIONS_NUMBER_OF_NODES];

}ST_SubscriptionParameters,*PST_SubscriptionParameters;

#define ROOTCA_CERTID_LEAF_SIZE 64
#define ROOTCA_SIGNATURE_LEAF_SIZE 64
#define ROOTCA_CERTIFICATE_LEAF_SIZE 128

typedef struct ST_RootCA_
{
	UINT8 CertID[ROOTCA_CERTID_LEAF_SIZE];
	UINT8 Signature[ROOTCA_SIGNATURE_LEAF_SIZE];
	UINT8 Certificate[ROOTCA_CERTIFICATE_LEAF_SIZE];
}ST_RootCA,*PST_RootCA;
#define CONTACTS_TRIGGER_LEAF_SIZE 64
#define CONTACTS_URI_LEAF_SIZE 128
#define CONTACTS_TEXT_LEAF_SIZE 64

typedef struct ST_Contacts_
{
	UINT32 Type;
	UINT8 Trigger[CONTACTS_TRIGGER_LEAF_SIZE];
	UINT8 URI[CONTACTS_URI_LEAF_SIZE];
	UINT8 Text[CONTACTS_TEXT_LEAF_SIZE];

}ST_Contacts,*PST_Contacts;

#define OPERATOR_TO_IP_REF_LEAF_SIZE 64
#define OPERATOR_CONTACTS_NUMBER_OF_NODE 2
#define OPERATOR_ROOTCA_NUMBER_OF_NODE 1

typedef struct ST_Operator_
{
ST_NetworkParameters NetworkParameters;
ST_SubscriptionParameters  SubscriptionParameters;
ST_RootCA RootCA[OPERATOR_ROOTCA_NUMBER_OF_NODE];
ST_Contacts Contacts[OPERATOR_CONTACTS_NUMBER_OF_NODE];
UINT8 TO_IP_REF[OPERATOR_TO_IP_REF_LEAF_SIZE];
}ST_Operator,*PST_Operator;

#define IPV4_IPADDR_LEAF_SIZE 32
#define IPV4_NETMASK_LEAF_SIZE 32
#define IPV4_GATEWAY_LEAF_SIZE 32

typedef struct ST_IPV4_
{
	UINT32  Autoconfig;
	UINT8 IPAddr[IPV4_IPADDR_LEAF_SIZE];
	UINT8 NetMask[IPV4_NETMASK_LEAF_SIZE];
	UINT8 Gateway[IPV4_GATEWAY_LEAF_SIZE];
	UINT32  AutoDNS;
			
}ST_IPV4,*PST_IPV4;


#define IPV6_IPADDR_LEAF_SIZE 128
#define IPV6_NETMASK_LEAF_SIZE 128
#define IPV6_GATEWAY_LEAF_SIZE 128

typedef struct ST_IPV6_
{
	UINT32 Autoconfig;
	UINT8 IPAddr[IPV6_IPADDR_LEAF_SIZE];
	UINT8 NetMask[IPV6_NETMASK_LEAF_SIZE];
	UINT8 Gateway[IPV6_GATEWAY_LEAF_SIZE];
	UINT32  AutoDNS;
			
}ST_IPV6,*PST_IPV6;
#define NAI_USERNAME_LEAF_SIZE 64
#define NAI_REALM_LEAF_SIZE 64
typedef struct ST_NAI_
{
	UINT8 Username[NAI_USERNAME_LEAF_SIZE];
	UINT8 Realm[NAI_REALM_LEAF_SIZE];
	UINT32 UsePseudo;
}ST_NAI,*PST_NAI;

#define SHAREDSECRET_NAI_NUMBER_OF_NODE 1
typedef struct ST_SharedSecret_
{
	ST_NAI NAI[SHAREDSECRET_NAI_NUMBER_OF_NODE];
	
}ST_SharedSecret,*PST_SharedSecret;
#define PROTOCOL_NATRAVERSAL_LEAF_SIZE 128
typedef struct ST_Protocol_v4
{
	UINT32 RevTun;
	UINT32  RetryCount;
	UINT32  RetryTimer;
	UINT32  RegPeriodRel;
	UINT32 RegPeriod;
	UINT8 NatTraversal[PROTOCOL_NATRAVERSAL_LEAF_SIZE];
}ST_Protocolv4,*PST_Protocolv4;
	
typedef struct ST_Protocol_v6
{
	UINT32  RouteOpt;
	UINT32  RetryCount;
	UINT32  RetryTimer;
	UINT32  BindingLifeTimeRel;
	UINT32  BindingLifeTime;
	UINT8   SignalingProtection;
	
}ST_Protocolv6,*PST_Protocolv6;

typedef struct ST_MIPv4_
{
	ST_SharedSecret SharedSecret;
	ST_Protocolv4  Protocol;

}ST_MIPv4,*PST_MIPv4;

typedef struct ST_MIPv6_
{
	ST_SharedSecret SharedSecret;
	ST_Protocolv6  Protocol;

}ST_MIPv6,*PST_MIPv6;

typedef struct ST_IP_
{

ST_IPV4 IPV4;
ST_IPV6 IPV6;
ST_MIPv4 MIPv4;
ST_MIPv6 MIPv6;

}ST_IP,*PST_IP;
#define WIMAXSUPP_OPERATOR_NUMBER_OF_NODE 1 
typedef struct ST_WIMAXSupp_
{
	
	ST_Operator Operator[WIMAXSUPP_OPERATOR_NUMBER_OF_NODE];
	
}ST_WIMAXSupp,*PST_WIMAXSupp;

#define APPS_PUI_LEAF_SIZE 128
#define APPS_PRI_LEAF_SIZE 128
#define APPS_SIP_PASSWORD_LEAF_SIZE 128
#define APPS_HOME_DOMAIN_LEAF_SIZE 128
#define APPS_SIP_PROXY_SERVER_LEAF_SIZE 128
#define APPS_CONFERENCE_URI_LEAF_SIZE 128


typedef struct ST_Apps_
{
	UINT8 PUI[APPS_PUI_LEAF_SIZE];
	UINT8 PRI[APPS_PRI_LEAF_SIZE];
	UINT8 SIP_Password[APPS_SIP_PASSWORD_LEAF_SIZE];
	UINT8 Home_Domain[APPS_HOME_DOMAIN_LEAF_SIZE];
	UINT8 SIP_Proxy_Server[APPS_SIP_PROXY_SERVER_LEAF_SIZE];
	UINT8 Conference_URI[APPS_CONFERENCE_URI_LEAF_SIZE];
	UINT32 VoIP_Activation;
	UINT32 T1;
	UINT32 T2;
	UINT32 T4;
	UINT32 REG_Refresh_Intvl;
	UINT32 Session_Refresh_Intvl;
}ST_Apps,*PST_Apps;

#define UPDATE_PKGDATA_LEAF_SIZE 4

typedef struct ST_Update_
{
UINT8 PkgData[UPDATE_PKGDATA_LEAF_SIZE]; 
}ST_Update,*PST_Update;
#define DOWNLOADANDUPDATE_PKGURL_LEAF_SIZE 4

typedef struct ST_DownloadAndUpdate_
{
	UINT8 PkgURL[DOWNLOADANDUPDATE_PKGURL_LEAF_SIZE]; 
}ST_DownloadAndUpdate,*PST_DownloadAndUpdate;
#define DOWNLOAD_PKGURL_LEAF_SIZE 256
typedef struct ST_Download_
{
	UINT8 PkgURL[DOWNLOAD_PKGURL_LEAF_SIZE];
	 
}ST_Download,*PST_Download;

#define FUMO_PKGNAME_LEAF_SIZE 64
#define FUMO_PKGVERSION_LEAF_SIZE 32
typedef struct ST_FUMO_
{
UINT8 PkgName[FUMO_PKGNAME_LEAF_SIZE];
UINT8 PkgVersion[FUMO_PKGVERSION_LEAF_SIZE];
ST_Update Update;
ST_Download Download;
ST_DownloadAndUpdate DownloadAndUpdate;
UINT32 State;
}ST_FUMO,*PST_FUMO;

#define MRU_TABLE_LEAF_SIZE 400
typedef struct _ST_MRU_TABLE_
{
	UINT8 uchMRUData[MRU_TABLE_LEAF_SIZE];
}ST_MRU_TABLE,*PST_MRU_TABLE;

typedef struct _ST_OperatorChannelPlanSettings
{
    UINT32 u32NumOfNetworkParamFields;
    ST_NetworkParameters astNetworkParameters[1];
}ST_OperatorChannelPlanSettings,*P_ST_OperatorChannelPlanSettings;


/*-----Root Tree containing all the subtrees/nodes and leafs----------------------*/
#define OMA_DM_TREE_DMACC_NUMBER_OF_NODE 1
#define OMA_DM_TREE_APPS_NUMBER_OF_NODE 1
#define OMA_DM_TREE_FUMO_NUMBER_OF_NODE 1

typedef struct _ST_ROOT
{
UINT32 u32Version;
UINT32 u32StructSize;
ST_DevInfo DevInfo;
ST_DMAcc DMAcc[OMA_DM_TREE_DMACC_NUMBER_OF_NODE];
ST_DevDetail DevDetail;
ST_WIMAX  WIMAX;
ST_WIMAXSupp WIMAXSupp; 
ST_IP IP;
ST_Apps Apps[OMA_DM_TREE_APPS_NUMBER_OF_NODE];
ST_FUMO FUMO[OMA_DM_TREE_FUMO_NUMBER_OF_NODE];
ST_MRU_TABLE MRUData;
}ST_ROOT,*PST_ROOT;



#endif /* BCM_OMADM_TREE_STRUCT_H */
