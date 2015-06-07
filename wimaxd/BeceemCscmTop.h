#pragma once

#ifndef BECEEM_CSCM_TOP_H
#define BECEEM_CSCM_TOP_H

/* ---------------------------------------------------- */
/*	Beceem Wireless Connection Manager                  */
/*	Copyright 2006 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2006                                           */
/* ---------------------------------------------------- */

#include "BeceemAPI.h"
#include "ThreadFunctions.h"

typedef enum {eNoReason, 
              eAlreadyConnReason,
			  eAuthInProgReason,
			  eSearchInProgReason,
			  eAutoReConnReason} DisableConnectReason_t;

typedef struct {

		STRING	bszCSCMIniFileFileName;
		BOOL	bWiMAXOpen;
		BOOL	bInitialBasebandStatusRequestIssued;
		BOOL	bInitialBasebandStatusResponseWait;
		BOOL	bReadingIniFile;
		BOOL	bAuthenticating;
		BOOL	bInvalidFWVersionWarningShown;
		BOOL	bInvalidDLLVersionWarningShown;
		BOOL	bReadyToAutoConnect;
		BOOL	bSearchIsAsync;
		BOOL	bSearchInProgress;
		BOOL 	bAutonomousScanParamsInitialized;
		BOOL	bAuthDone; 
		BOOL	bDisableConnect;
		DisableConnectReason_t eDisableConnectReason;

		UINT32  u32AuthenticatingISRCount;
		UINT32	u32PostFormShownDelayMs;
		UINT32	u32BasebandStatusRequestTimeMs;
		UINT32  u32BasebandStatusRequestIntervalMs;
		UINT32	u32CheckDeviceOpenTimeMs;
		UINT32	u32AutoReConnectTimeMs;
		UINT32  u32AutoReConnectIntervalMs;
		UINT32	u32AutoReDisconnectTimeMs;
		UINT32	u32AutoReDisconnectIntervalMs;

		UINT32  u32NumberOfConnectAttempts;
		UINT32	u32NumberOfAuthCompleted;
		UINT32	u32NumberOfAuthOK;
		UINT32	u32NumberOfAuthTimedOut;
		UINT32  u32NumberOfAuthOtherFailure;

		UINT8	*pu8Buffer;

		STRING  bszIdentity;
		STRING	bszPassword;
		BOOL	bUsePasswordField;

		BeceemAPI_t hAPI;
		WcmMutex_t  mutexBeceemAPI;

		WcmThread_t  threadTimerAuthentication;
		WcmThread_t  threadTimerCheckTimeouts;
		WcmThread_t  threadTimerPeriodicAction;
		WcmEvent_t   eventExitTimerAuthentication;
		WcmEvent_t   eventExitTimerCheckTimeouts;
		WcmEvent_t   eventExitTimerPeriodicAction;
		WcmSocket_t  socketListen;

	} stBeceemCSCM_t;

typedef stBeceemCSCM_t *BeceemCSCM_t;

BeceemCSCM_t CSCM_Create(STRING bszConfFilePath, BOOL bDebugMode);
void CSCM_Destroy(BeceemCSCM_t hCSCM);

STRING CSCM_Authenticate(BeceemCSCM_t hCSCM, BOOL bReAuthenticate);
void CSCM_AbortAuthentication(BeceemCSCM_t hCSCM, BOOL bSendEAPComplete);
void CSCM_ClearAllCounts(BeceemCSCM_t hCSCM);
void CSCM_DownloadFirmwareOrConfigFile(BeceemCSCM_t hCSCM, UINT32 u32ImageType);		
void CSCM_StartNetworkSearch(BeceemCSCM_t hCSCM);
void CSCM_UpdateBeceemAPIOptions(BeceemCSCM_t hCSCM);
void CSCM_CheckFileNameOptions(BeceemCSCM_t hCSCM);

void CSCM_AuthenticationTimerISR(BeceemCSCM_t hCSCM);
void CSCM_CheckTimeoutsTimerISR(BeceemCSCM_t hCSCM);
void CSCM_PeriodicActionTimerISR(BeceemCSCM_t hCSCM);

STRING CSCM_SelectBSForConnect(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs, BOOL *pbError);
void CSCM_Connect_Command(BeceemCSCM_t hCSCM, BOOL bSendResponseToClient);
void CSCM_Disconnect_Command(BeceemCSCM_t hCSCM);
void CSCM_NetworkSearch_Command(BeceemCSCM_t hCSCM, STRING abszArgStrings[], UINT32 u32NumberOfArgs);

void CSCM_UpdateOptions(BeceemCSCM_t hCSCM);

CSCM_THREAD_FUNCTION CSCM_ThreadTimerAuthentication(void *pvThreadParam);
CSCM_THREAD_FUNCTION CSCM_ThreadTimerCheckTimeouts(void *pvThreadParam);
CSCM_THREAD_FUNCTION CSCM_ThreadTimerPeriodicAction(void *pvThreadParam);

#endif
