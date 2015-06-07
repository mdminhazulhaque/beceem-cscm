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
 * Description	:	OMA-DM internal definitions.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BCM_OMA_DMINTERFACE_H
#define BCM_OMA_DMINTERFACE_H

typedef enum _E_VDM_IPC_INTERFACE
{
	INVALID_INTF_VDM,	
	 CAPI_INTF,
	 CM_INTF
}E_VDM_IPC_INTERFACE,*PE_VDM_IPC_INTERFACE;


typedef enum _E_BCM_VDM_IPC_INTF_COMMANDS
{
	eProvisioningOperation = 0,
	ePackageUpdate =1,
	eLinkup=2,
	eLinkdown=3
}E_BCM_VDM_IPC_INTF_COMMANDS,*PE_BCM_VDM_IPC_INTF_COMMANDS;


/*! 
 * Session Types
 */
typedef enum _E_BCM_VDM_SessionType{
	BCM_VDMVDM_SessionType_Boot,	//!< Bootstrap session
	BCM_VDMVDM_SessionType_DM,		//!< DM session
	BCM_VDMVDM_SessionType_DL		//!< DL session
} E_BCM_VDM_SessionType;

/*!
 * Session States
 */
typedef enum _E_BCM_VDM_SessionState{
	BCM_VDMVDM_SessionState_Started, 	//!< Session has started
	BCM_VDMVDM_SessionState_Complete,	//!< Session completed successfully
	BCM_VDMVDM_SessionState_Aborted	//!< Session aborted
} E_BCM_VDM_SessionState;


typedef struct _ST_SESSION_STATE_INFO
{
	E_BCM_VDM_SessionState eSessionState;
	E_BCM_VDM_SessionType eSessionType;
}ST_SESSION_STATE_INFO,*PST_SESSION_STATE_INFO;


#endif /* BCM_OMA_DMINTERFACE_H */
