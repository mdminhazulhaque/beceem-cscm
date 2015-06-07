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
 * Description	:	MAC Management Message Names and Definitions
 * Author		:
 * Reviewer		:
 *
 */


#ifndef MACMGMTNAMES_H
#define MACMGMTNAMES_H


/*  */
/*  MAC MGMT message names */
/*  */

typedef struct stMACMgmtMsgInfo
{
	unsigned char	ucType;
	char			cMsgName[60];
	unsigned char	ucConnection;
}stMACMgmtMsgInfo, *pstMACMgmtMsgInfo;


#define UCD			0

#define DCD			1

#define DL_MAP		2
#define UL_MAP		3
#define RNG_REQ		4
#define RNG_RSP		5
#define REG_REQ		6
#define REG_RSP		7

#define PKM_REQ		9
#define PKM_RSP		10

#define DSA_REQ		11
#define DSA_RSP		12
#define DSA_ACK		13

#define DSC_REQ		14
#define DSC_RSP		15
#define DSC_ACK		16

#define DSD_REQ		17
#define DSD_RSP		18

#define MCA_REQ		21
#define MCA_RSP		22

#define DBPC_REQ	23
#define DBPC_RSP	24

#define RES_CMD		25

#define SBC_REQ		26
#define SBC_RSP		27

#define CLK_CMP		28
#define DREG_CMD	29
#define DSX_RVD		30
#define TFTP_CPLT	31
#define TFTP_RSP	32
#define ARQ_Feedback	33
#define ARQ_Discard		34
#define ARQ_Reset		35

#define REP_REQ		36
#define REP_RSP		37

#define FPC			38

#define MSH_NCFG	39
#define MSH_NENT	40
#define MSH_DSCH	41
#define MSH_CSCH	42
#define MSH_CSCF	43

#define AAS_FBCK_REQ	44
#define AAS_FBCK_RSP	45
#define AAS_Beam_Select	46
#define AAS_BEAM_REQ	47
#define AAS_BEAM_RSP	48

#define DREG_REQ		49

#define MOB_SLP_REQ		50
#define MOB_SLP_RSP		51

#define MOB_TRF_IND		52
#define MOB_NBR_ADV		53

#define MOB_SCN_REQ		54
#define MOB_SCN_RSP		55

#define MOB_BSHO_REQ	56
#define MOB_MSHO_REQ	57
#define MOB_BSHO_RSP	58

#define MOB_HO_IND		59
#define MOB_SCAN_REPORT	60
#define MOB_PAG_ADV		61
#define MBS_MAP			62

#define PMC_REQ			63
#define PMC_RSP			64
#define PRC_LT_CTRL		65

#define MOB_ASC_REP		66


static stMACMgmtMsgInfo MACMgmtMsgs[] =
{
	{UCD			,"Uplink Channel Descriptor",0},
	{DCD			,"Downlink Channel Descriptor",0},

	{DL_MAP			,"Downlink Access Definition",0},
	{UL_MAP			,"Uplink Access Definition",0},

	{RNG_REQ		,"Ranging Request",0},

	{RNG_RSP		,"Ranging Response",0},

	{REG_REQ		,"Registration Request",0},

	{REG_RSP		,"Registration Response",0},

	{PKM_REQ		,"Privacy Key Management Request",0},
	{PKM_RSP		,"Privacy Key Management Response",0},
	{DSA_REQ		,"Dynamic Service Addition Request",0},
	{DSA_RSP		,"Dynamic Service Addition Response",0},
	{DSA_ACK		,"Dynamic Service Addition Acknowledge",0},

	{DSC_REQ		,"Dynamic Service Change Request",0},
	{DSC_RSP		,"Dynamic Service Change Response",0},
	{DSC_ACK		,"Dynamic Service Change Acknowledge",0},

	{DSD_REQ		,"Dynamic Service Deletion Request",0},
	{DSD_RSP		,"Dynamic Service Deletion Response",0},

	{MCA_REQ		,"Multicast Assignment Request",0},
	{MCA_RSP		,"Multicast Assignment Response",0},

	{DBPC_REQ		,"Downlink Burst Profile Change Request",0},
	{DBPC_RSP		,"Downlink Burst Profile Change Response",0},

	{RES_CMD		,"Reset Command",0},

	{SBC_REQ		,"SS Basic Capability Request",0},
	{SBC_RSP		,"SS Basic Capability Response",0},

	{CLK_CMP		,"SS network clock comparision",0},
	{DREG_CMD		,"De/Re-register Command",0},
	{DSX_RVD		,"DSx Received Message",0},
	{TFTP_CPLT		,"Config File TFTP Complete Message",0},
	{TFTP_RSP		,"Config File TFTP Complete Response",0},
	{ARQ_Feedback	,"Standalone ARQ Feedback",0},
	{ARQ_Discard	,"ARQ Discard message",0},
	{ARQ_Reset		,"ARQ Reset message",0},

	{REP_REQ		,"Channel measurement Report Request",0},
	{REP_RSP		,"Channel measurement Report Response",0},

	{FPC			,"Fast Power Control",0},
	{MSH_NCFG		,"Mesh Network Configuration",0},
	{MSH_NENT		,"Mesh Network Entry",0},
	{MSH_DSCH		,"Mesh Distributed Schedule",0},
	{MSH_CSCH		,"Mesh Centralized Schedule",0},
	{MSH_CSCF		,"Mesh Centralized Schedule Configuration",0},

	{AAS_FBCK_REQ	,"AAS Feedback Request",0},
	{AAS_FBCK_RSP	,"AAS Feedback Response",0},
	{AAS_Beam_Select,"AAS Beam Select message",0},
	{AAS_BEAM_REQ	,"AAS Beam Request message",0},
	{AAS_BEAM_RSP	,"AAS Beam Response message",0},

	{DREG_REQ		,"SS De-Registration message",0},

	{MOB_SLP_REQ	,"sleep request message",0},
	{MOB_SLP_RSP	,"sleep response message",0},

	{MOB_TRF_IND	,"traffic indication message",0},
	{MOB_NBR_ADV	,"neighbour advertisement message",0},

	{MOB_SCN_REQ	,"scanning interval allocation request",0},
	{MOB_SCN_RSP	,"scanning interval allocation response",0},

	{MOB_BSHO_REQ	,"BS HO request message",0},
	{MOB_MSHO_REQ	,"MS HO request message",0},
	{MOB_BSHO_RSP	,"BS HO response message",0},

	{MOB_HO_IND		,"HO indication message",0},
	{MOB_SCAN_REPORT,"Scanning result report message",0},
	{MOB_PAG_ADV	,"BS broadcast paging message",0},
	{MBS_MAP		,"MBAS MAP message",0},

	{PMC_REQ		,"Power control mode change request message",0},
	{PMC_RSP		,"Power control mode change response message",0},
	{PRC_LT_CTRL	,"Setup/Tear-down of long term MIMO precoding",0},

	{MOB_ASC_REP	,"Association result report message",0}

};


#endif /* MACMGMTNAMES_H */
