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
 * Description	:	OMA-DM tree user APIs.
 * Author		:
 * Reviewer		:
 *
 */


/**
@file OMADMUserApi.h
*/

#ifndef _BCM_OMADM_USER_API_H_
#define _BCM_OMADM_USER_API_H_
typedef intptr_t UADDTYPE1;

#include "OMADMTreeStruct.h"
#define OFFSET_TO_FIELD(parentType,fieldName) (UADDTYPE1)&(((parentType*)0)->fieldName)

#define OMA_DM_MAJOR_VERSION   4
#define OMA_DM_MINOR_VERSION   0



/*offsets for structure ST_ROOT */
#define OMA_DM_ST_ROOT_U32VERSION                                              (1 << 8) | 1  
#define OMA_DM_ST_ROOT_U32STRUCT_SIZE                                          (1 << 8) | 2  
#define OMA_DM_ST_ROOT_DEV_INFO                                                (1 << 8) | 3  
#define OMA_DM_ST_ROOT_DMACC                                                   (1 << 8) | 4  
#define OMA_DM_ST_ROOT_DEV_DETAIL                                              (1 << 8) | 5  
#define OMA_DM_ST_ROOT_WIMAX                                                   (1 << 8) | 6  
#define OMA_DM_ST_ROOT_WIMAXSUPP                                               (1 << 8) | 7  
#define OMA_DM_ST_ROOT_IP                                                      (1 << 8) | 8  
#define OMA_DM_ST_ROOT_APPS                                                    (1 << 8) | 9  
#define OMA_DM_ST_ROOT_FUMO                                                    (1 << 8) | 10  
#define OMA_DM_ST_ROOT_MRUDATA                                                 (1 << 8) | 11  


/*offsets for structure ST_DevInfo */
#define OMA_DM_ST_DEV_INFO_DEV_ID                                              (2 << 8) | 1  
#define OMA_DM_ST_DEV_INFO_MAN                                                 (2 << 8) | 2  
#define OMA_DM_ST_DEV_INFO_MOD                                                 (2 << 8) | 3  
#define OMA_DM_ST_DEV_INFO_DMV                                                 (2 << 8) | 4  
#define OMA_DM_ST_DEV_INFO_LANG                                                (2 << 8) | 5  


/*offsets for structure ST_DMAcc */
#define OMA_DM_ST_DMACC_APP_ID                                                 (3 << 8) | 1  
#define OMA_DM_ST_DMACC_SERVER_ID                                              (3 << 8) | 2  
#define OMA_DM_ST_DMACC_NAME                                                   (3 << 8) | 3  
#define OMA_DM_ST_DMACC_PREF_CON_REF                                           (3 << 8) | 4  
#define OMA_DM_ST_DMACC_TO_CON_REF                                             (3 << 8) | 5  
#define OMA_DM_ST_DMACC_APP_ADDR                                               (3 << 8) | 6  
#define OMA_DM_ST_DMACC_AAUTH_PREF                                             (3 << 8) | 7  
#define OMA_DM_ST_DMACC_APP_AUTH                                               (3 << 8) | 8  


/*offsets for structure ST_ToConRef */
#define OMA_DM_ST_TO_CON_REF_CON_REF                                           (4 << 8) | 1  


/*offsets for structure ST_AppAddr */
#define OMA_DM_ST_APP_ADDR_ADDR                                                (5 << 8) | 1  
#define OMA_DM_ST_APP_ADDR_ADDR_TYPE                                           (5 << 8) | 2  
#define OMA_DM_ST_APP_ADDR_PORT                                                (5 << 8) | 3  


/*offsets for structure ST_Port */
#define OMA_DM_ST_PORT_PORT_NBR                                                (6 << 8) | 1  


/*offsets for structure ST_AppAuth */
#define OMA_DM_ST_APP_AUTH_AAUTH_LEVEL                                         (7 << 8) | 1  
#define OMA_DM_ST_APP_AUTH_AAUTH_TYPE                                          (7 << 8) | 2  
#define OMA_DM_ST_APP_AUTH_AAUTH_NAME                                          (7 << 8) | 3  
#define OMA_DM_ST_APP_AUTH_AAUTH_SECRET                                        (7 << 8) | 4  
#define OMA_DM_ST_APP_AUTH_AAUTH_DATA                                          (7 << 8) | 5  


/*offsets for structure ST_DevDetail */
#define OMA_DM_ST_DEV_DETAIL_URI                                               (8 << 8) | 1  
#define OMA_DM_ST_DEV_DETAIL_DEV_TYPE                                          (8 << 8) | 2  
#define OMA_DM_ST_DEV_DETAIL_OEM                                               (8 << 8) | 3  
#define OMA_DM_ST_DEV_DETAIL_FW_V                                              (8 << 8) | 4  
#define OMA_DM_ST_DEV_DETAIL_SW_V                                              (8 << 8) | 5  
#define OMA_DM_ST_DEV_DETAIL_HW_V                                              (8 << 8) | 6  
#define OMA_DM_ST_DEV_DETAIL_LRG_OBJ                                           (8 << 8) | 7  


/*offsets for structure ST_URI */
#define OMA_DM_ST_URI_MAX_DEPTH                                                (9 << 8) | 1  
#define OMA_DM_ST_URI_MAX_TOT_LEN                                              (9 << 8) | 2  
#define OMA_DM_ST_URI_MAX_SEG_LEN                                              (9 << 8) | 3  


/*offsets for structure ST_WIMAX */
#define OMA_DM_ST_WIMAX_WIMAXRADIO_MODULE                                      (10 << 8) | 1  
#define OMA_DM_ST_WIMAX_TERMINAL_EQUIPMENT                                     (10 << 8) | 2  
#define OMA_DM_ST_WIMAX_TO_WIMAX_REF                                           (10 << 8) | 3  
#define OMA_DM_ST_WIMAX_DEV_CAP                                                (10 << 8) | 4  


/*offsets for structure ST_WIMAXRadioModule */
#define OMA_DM_ST_WIMAXRADIO_MODULE_MAN                                        (11 << 8) | 1  
#define OMA_DM_ST_WIMAXRADIO_MODULE_MOD                                        (11 << 8) | 2  
#define OMA_DM_ST_WIMAXRADIO_MODULE_FW_V                                       (11 << 8) | 3  
#define OMA_DM_ST_WIMAXRADIO_MODULE_HW_V                                       (11 << 8) | 4  
#define OMA_DM_ST_WIMAXRADIO_MODULE_SW_V                                       (11 << 8) | 5  
#define OMA_DM_ST_WIMAXRADIO_MODULE_MACADDRESS                                 (11 << 8) | 6  
#define OMA_DM_ST_WIMAXRADIO_MODULE_SPLOCK                                     (11 << 8) | 7  
#define OMA_DM_ST_WIMAXRADIO_MODULE_TO_FUMO_REF                                (11 << 8) | 8  


/*offsets for structure ST_SPLock */
#define OMA_DM_ST_SPLOCK_LOCK_STATUS                                           (12 << 8) | 1  
#define OMA_DM_ST_SPLOCK_OPERATOR                                              (12 << 8) | 2  
#define OMA_DM_ST_SPLOCK_VER_NBR                                               (12 << 8) | 3  
#define OMA_DM_ST_SPLOCK_LOCK                                                  (12 << 8) | 4  


/*offsets for structure ST_TerminalEquipment */
#define OMA_DM_ST_TERMINAL_EQUIPMENT_DEV_ID                                    (13 << 8) | 1  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_DEV_TYPE                                  (13 << 8) | 2  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_MAN                                       (13 << 8) | 3  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_MOD                                       (13 << 8) | 4  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_FW_V                                      (13 << 8) | 5  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_HW_V                                      (13 << 8) | 6  
#define OMA_DM_ST_TERMINAL_EQUIPMENT_SW_V                                      (13 << 8) | 7  


/*offsets for structure ST_DevCap */
#define OMA_DM_ST_DEV_CAP_IPCAP                                                (14 << 8) | 1  
#define OMA_DM_ST_DEV_CAP_UPDATE_METHODS                                       (14 << 8) | 2  


/*offsets for structure ST_IPCap */
#define OMA_DM_ST_IPCAP_IPV4                                                   (15 << 8) | 1  
#define OMA_DM_ST_IPCAP_IPV6                                                   (15 << 8) | 2  
#define OMA_DM_ST_IPCAP_CMIPV4                                                 (15 << 8) | 3  
#define OMA_DM_ST_IPCAP_CMIPV6                                                 (15 << 8) | 4  


/*offsets for structure ST_UpdateMethods */
#define OMA_DM_ST_UPDATE_METHODS_SERVER_INITIATED                              (16 << 8) | 1  
#define OMA_DM_ST_UPDATE_METHODS_CLIENT_INITIATED                              (16 << 8) | 2  


/*offsets for structure ST_ClientInitiated */
#define OMA_DM_ST_CLIENT_INITIATED_POLLING_SUPPORTED                           (17 << 8) | 1  
#define OMA_DM_ST_CLIENT_INITIATED_POLLING_INTERVAL                            (17 << 8) | 2  


/*offsets for structure ST_WIMAXSupp */
#define OMA_DM_ST_WIMAXSUPP_OPERATOR                                           (18 << 8) | 1  


/*offsets for structure ST_Operator */
#define OMA_DM_ST_OPERATOR_NETWORK_PARAMETERS                                  (19 << 8) | 1  
#define OMA_DM_ST_OPERATOR_SUBSCRIPTION_PARAMETERS                             (19 << 8) | 2  
#define OMA_DM_ST_OPERATOR_ROOT_CA                                             (19 << 8) | 3  
#define OMA_DM_ST_OPERATOR_CONTACTS                                            (19 << 8) | 4  
#define OMA_DM_ST_OPERATOR_TO_IP_REF                                           (19 << 8) | 5  


/*offsets for structure ST_NetworkParameters */
#define OMA_DM_ST_NETWORK_PARAMETERS_H_NSP                                     (20 << 8) | 1  
#define OMA_DM_ST_NETWORK_PARAMETERS_CAPL                                      (20 << 8) | 2  
#define OMA_DM_ST_NETWORK_PARAMETERS_RAPL                                      (20 << 8) | 3  
#define OMA_DM_ST_NETWORK_PARAMETERS_CHANNEL_PLAN                              (20 << 8) | 4  
#define OMA_DM_ST_NETWORK_PARAMETERS_OPERATOR_NAME                             (20 << 8) | 5  
#define OMA_DM_ST_NETWORK_PARAMETERS_POLLING_INTERVAL                          (20 << 8) | 6  


/*offsets for structure ST_H_NSP */
#define OMA_DM_ST_H_NSP_H_NSP_ID                                               (21 << 8) | 1  


/*offsets for structure ST_CAPL */
#define OMA_DM_ST_CAPL_ENTRIES                                                 (22 << 8) | 1  
#define OMA_DM_ST_CAPL_ANY                                                     (22 << 8) | 2  


/*offsets for structure ST_CAPLEntries */
#define OMA_DM_ST_CAPLENTRIES_NAP_ID                                           (23 << 8) | 1  
#define OMA_DM_ST_CAPLENTRIES_PRIORITY                                         (23 << 8) | 2  
#define OMA_DM_ST_CAPLENTRIES_CH_PLAN_REF_IDS                                  (23 << 8) | 3  


/*offsets for structure ST_ChPlanRefIds */
#define OMA_DM_ST_CH_PLAN_REF_IDS_REF_ID                                       (24 << 8) | 1  


/*offsets for structure ST_RAPL */
#define OMA_DM_ST_RAPL_ENTRIES                                                 (25 << 8) | 1  
#define OMA_DM_ST_RAPL_ANY                                                     (25 << 8) | 2  


/*offsets for structure ST_RAPLEntries */
#define OMA_DM_ST_RAPLENTRIES_V_NSP_ID                                         (26 << 8) | 1  
#define OMA_DM_ST_RAPLENTRIES_PRIORITY                                         (26 << 8) | 2  


/*offsets for structure ST_ChannelPlan */
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES                                         (27 << 8) | 1  
#define OMA_DM_ST_CHANNEL_PLAN_BW                                              (27 << 8) | 2  
#define OMA_DM_ST_CHANNEL_PLAN_FFTSIZE                                         (27 << 8) | 3  
#define OMA_DM_ST_CHANNEL_PLAN_DUPLEX_MODE                                     (27 << 8) | 4  


/*offsets for structure ST_ChannelPlanEntries */
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_ID                                      (28 << 8) | 1  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_FIRST_FREQ                              (28 << 8) | 2  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_LAST_FREQ                               (28 << 8) | 3  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_NEXT_FREQ_STEP                          (28 << 8) | 4  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_PREAMBLES                               (28 << 8) | 5  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_BW                                      (28 << 8) | 6  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_FFTSIZE                                 (28 << 8) | 7  
#define OMA_DM_ST_CHANNEL_PLAN_ENTRIES_DUPLEX_MODE                             (28 << 8) | 8  


/*offsets for structure ST_SubscriptionParameters */
#define OMA_DM_ST_SUBSCRIPTION_PARAMETERS_PRIMARY                              (29 << 8) | 1  
#define OMA_DM_ST_SUBSCRIPTION_PARAMETERS_OTHER_SUBSCRIPTIONS                  (29 << 8) | 2  


/*offsets for structure ST_Primary */
#define OMA_DM_ST_PRIMARY_NAME                                                 (30 << 8) | 1  
#define OMA_DM_ST_PRIMARY_ACTIVATED                                            (30 << 8) | 2  
#define OMA_DM_ST_PRIMARY_EAP                                                  (30 << 8) | 3  


/*offsets for structure ST_EAP */
#define OMA_DM_ST_EAP_METHOD_TYPE                                              (31 << 8) | 1  
#define OMA_DM_ST_EAP_VENDOR_ID                                                (31 << 8) | 2  
#define OMA_DM_ST_EAP_VENDOR_TYPE                                              (31 << 8) | 3  
#define OMA_DM_ST_EAP_USER_IDENTITY                                            (31 << 8) | 4  
#define OMA_DM_ST_EAP_PROVISIONED_PSEUDO_IDENTITY                              (31 << 8) | 5  
#define OMA_DM_ST_EAP_PASSWORD                                                 (31 << 8) | 6  
#define OMA_DM_ST_EAP_REALM                                                    (31 << 8) | 7  
#define OMA_DM_ST_EAP_USE_PRIVACY                                              (31 << 8) | 8  
#define OMA_DM_ST_EAP_ENCAPS                                                   (31 << 8) | 9  
#define OMA_DM_ST_EAP_VFY_SERVER_REALM                                         (31 << 8) | 10  
#define OMA_DM_ST_EAP_SERVER_REALMS                                            (31 << 8) | 11  
#define OMA_DM_ST_EAP_EAP_AKA                                                  (31 << 8) | 12  
#define OMA_DM_ST_EAP_CERT                                                     (31 << 8) | 13  


/*offsets for structure ST_SERVER_REALMS */
#define OMA_DM_ST_SERVER_REALMS_SERVER_REALM                                   (32 << 8) | 1  


/*offsets for structure ST_EAP_AKA */
#define OMA_DM_ST_EAP_AKA_USE_CHECK_CODE                                       (33 << 8) | 1  


/*offsets for structure ST_CERT */
#define OMA_DM_ST_CERT_CERT_TYPE                                               (34 << 8) | 1  
#define OMA_DM_ST_CERT_SER_NUM                                                 (34 << 8) | 2  
#define OMA_DM_ST_CERT_THUMBPRINT                                              (34 << 8) | 3  


/*offsets for structure ST_OtherSubscriptions */
#define OMA_DM_ST_OTHER_SUBSCRIPTIONS_NAME                                     (35 << 8) | 1  
#define OMA_DM_ST_OTHER_SUBSCRIPTIONS_ACTIVATED                                (35 << 8) | 2  
#define OMA_DM_ST_OTHER_SUBSCRIPTIONS_EAP                                      (35 << 8) | 3  


/*offsets for structure ST_RootCA */
#define OMA_DM_ST_ROOT_CA_CERT_ID                                              (36 << 8) | 1  
#define OMA_DM_ST_ROOT_CA_SIGNATURE                                            (36 << 8) | 2  
#define OMA_DM_ST_ROOT_CA_CERTIFICATE                                          (36 << 8) | 3  


/*offsets for structure ST_Contacts */
#define OMA_DM_ST_CONTACTS_TYPE                                                (37 << 8) | 1  
#define OMA_DM_ST_CONTACTS_TRIGGER                                             (37 << 8) | 2  
#define OMA_DM_ST_CONTACTS_URI                                                 (37 << 8) | 3  
#define OMA_DM_ST_CONTACTS_TEXT                                                (37 << 8) | 4  


/*offsets for structure ST_IP */
#define OMA_DM_ST_IP_IPV4                                                      (38 << 8) | 1  
#define OMA_DM_ST_IP_IPV6                                                      (38 << 8) | 2  
#define OMA_DM_ST_IP_MIPV4                                                     (38 << 8) | 3  
#define OMA_DM_ST_IP_MIPV6                                                     (38 << 8) | 4  


/*offsets for structure ST_IPV4 */
#define OMA_DM_ST_IPV4_AUTOCONFIG                                              (39 << 8) | 1  
#define OMA_DM_ST_IPV4_IPADDR                                                  (39 << 8) | 2  
#define OMA_DM_ST_IPV4_NET_MASK                                                (39 << 8) | 3  
#define OMA_DM_ST_IPV4_GATEWAY                                                 (39 << 8) | 4  
#define OMA_DM_ST_IPV4_AUTO_DNS                                                (39 << 8) | 5  


/*offsets for structure ST_IPV6 */
#define OMA_DM_ST_IPV6_AUTOCONFIG                                              (40 << 8) | 1  
#define OMA_DM_ST_IPV6_IPADDR                                                  (40 << 8) | 2  
#define OMA_DM_ST_IPV6_NET_MASK                                                (40 << 8) | 3  
#define OMA_DM_ST_IPV6_GATEWAY                                                 (40 << 8) | 4  
#define OMA_DM_ST_IPV6_AUTO_DNS                                                (40 << 8) | 5  


/*offsets for structure ST_MIPv4 */
#define OMA_DM_ST_MIPV4_SHARED_SECRET                                          (41 << 8) | 1  
#define OMA_DM_ST_MIPV4_PROTOCOL                                               (41 << 8) | 2  


/*offsets for structure ST_SharedSecret */
#define OMA_DM_ST_SHARED_SECRET_NAI                                            (42 << 8) | 1  


/*offsets for structure ST_NAI */
#define OMA_DM_ST_NAI_USERNAME                                                 (43 << 8) | 1  
#define OMA_DM_ST_NAI_REALM                                                    (43 << 8) | 2  
#define OMA_DM_ST_NAI_USE_PSEUDO                                               (43 << 8) | 3  


/*offsets for structure ST_Protocolv4 */
#define OMA_DM_ST_PROTOCOLV4_REV_TUN                                           (44 << 8) | 1  
#define OMA_DM_ST_PROTOCOLV4_RETRY_COUNT                                       (44 << 8) | 2  
#define OMA_DM_ST_PROTOCOLV4_RETRY_TIMER                                       (44 << 8) | 3  
#define OMA_DM_ST_PROTOCOLV4_REG_PERIOD_REL                                    (44 << 8) | 4  
#define OMA_DM_ST_PROTOCOLV4_REG_PERIOD                                        (44 << 8) | 5  
#define OMA_DM_ST_PROTOCOLV4_NAT_TRAVERSAL                                     (44 << 8) | 6  


/*offsets for structure ST_MIPv6 */
#define OMA_DM_ST_MIPV6_SHARED_SECRET                                          (45 << 8) | 1  
#define OMA_DM_ST_MIPV6_PROTOCOL                                               (45 << 8) | 2  


/*offsets for structure ST_Protocolv6 */
#define OMA_DM_ST_PROTOCOLV6_ROUTE_OPT                                         (46 << 8) | 1  
#define OMA_DM_ST_PROTOCOLV6_RETRY_COUNT                                       (46 << 8) | 2  
#define OMA_DM_ST_PROTOCOLV6_RETRY_TIMER                                       (46 << 8) | 3  
#define OMA_DM_ST_PROTOCOLV6_BINDING_LIFE_TIME_REL                             (46 << 8) | 4  
#define OMA_DM_ST_PROTOCOLV6_BINDING_LIFE_TIME                                 (46 << 8) | 5  
#define OMA_DM_ST_PROTOCOLV6_SIGNALING_PROTECTION                              (46 << 8) | 6  


/*offsets for structure ST_Apps */
#define OMA_DM_ST_APPS_PUI                                                     (47 << 8) | 1  
#define OMA_DM_ST_APPS_PRI                                                     (47 << 8) | 2  
#define OMA_DM_ST_APPS_SIP_PASSWORD                                            (47 << 8) | 3  
#define OMA_DM_ST_APPS_HOME_DOMAIN                                             (47 << 8) | 4  
#define OMA_DM_ST_APPS_SIP_PROXY_SERVER                                        (47 << 8) | 5  
#define OMA_DM_ST_APPS_CONFERENCE_URI                                          (47 << 8) | 6  
#define OMA_DM_ST_APPS_VO_IP_ACTIVATION                                        (47 << 8) | 7  
#define OMA_DM_ST_APPS_T1                                                      (47 << 8) | 8  
#define OMA_DM_ST_APPS_T2                                                      (47 << 8) | 9  
#define OMA_DM_ST_APPS_T4                                                      (47 << 8) | 10  
#define OMA_DM_ST_APPS_REG_REFRESH_INTVL                                       (47 << 8) | 11  
#define OMA_DM_ST_APPS_SESSION_REFRESH_INTVL                                   (47 << 8) | 12  


/*offsets for structure ST_FUMO */
#define OMA_DM_ST_FUMO_PKG_NAME                                                (48 << 8) | 1  
#define OMA_DM_ST_FUMO_PKG_VERSION                                             (48 << 8) | 2  
#define OMA_DM_ST_FUMO_UPDATE                                                  (48 << 8) | 3  
#define OMA_DM_ST_FUMO_DOWNLOAD                                                (48 << 8) | 4  
#define OMA_DM_ST_FUMO_DOWNLOAD_AND_UPDATE                                     (48 << 8) | 5  
#define OMA_DM_ST_FUMO_STATE                                                   (48 << 8) | 6  


/*offsets for structure ST_Update */
#define OMA_DM_ST_UPDATE_PKG_DATA                                              (49 << 8) | 1  


/*offsets for structure ST_Download */
#define OMA_DM_ST_DOWNLOAD_PKG_URL                                             (50 << 8) | 1  


/*offsets for structure ST_DownloadAndUpdate */
#define OMA_DM_ST_DOWNLOAD_AND_UPDATE_PKG_URL                                  (51 << 8) | 1  


/*offsets for structure ST_MRU_TABLE */
#define OMA_DM_ST_MRU_TABLE_UCH_MRUDATA                                        (52 << 8) | 1  


#endif /* _BCM_OMADM_USER_API_H_ */
