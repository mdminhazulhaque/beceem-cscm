/* ---------------------------------------------------- */
/*	Beceem Wireless Connection Manager                  */
/*	Copyright 2006-2007 Beceem Communications, Inc.     */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	February 2007                                       */
/* ---------------------------------------------------- */

#include "CscmString.h"
#include "BeceemCscmLogin.h"

BOOL Login_IsIdentityValid(BeceemCSCM_t hCSCM)
	{		

	UINT32 u32Len = (UINT32) b_strlen(hCSCM->bszIdentity);
	return u32Len >= MIN_IDENTITY_LENGTH && u32Len <= MAX_IDENTITY_LENGTH;

	} /* Login_IsIdentityValid */

BOOL Login_IsPasswordValid(BeceemCSCM_t hCSCM)
	{

	UINT32 u32Len = (UINT32) b_strlen(hCSCM->bszPassword);
	return !hCSCM->bUsePasswordField || (u32Len >= MIN_PASSWORD_LENGTH && u32Len <= MAX_PASSWORD_LENGTH);

	} /* Login_IsPasswordValid */

// -----------------------------------------------------------------------
BOOL Login_CheckValid(BeceemCSCM_t hCSCM) 
	{
	
	BOOL bIdentityValid = Login_IsIdentityValid(hCSCM);
	BOOL bPasswordValid = Login_IsPasswordValid(hCSCM);

	if (!(bIdentityValid && bPasswordValid))
		{
		hCSCM->bszIdentity = EMPTY_STRING;
		hCSCM->bszPassword = EMPTY_STRING;
		}

	if (!bIdentityValid)
		PrintDiag(DIAG_ERROR, hCSCM->hAPI, S("***** ERROR: Invalid user identity '") B_SF S("' (length must be between %d and %d)\n"),
			hCSCM->bszIdentity, MIN_IDENTITY_LENGTH, MAX_IDENTITY_LENGTH);

	if (!bPasswordValid)
		PrintDiag(DIAG_ERROR, hCSCM->hAPI, S("***** ERROR: Invalid password '") B_SF S("' (length must be between %d and %d)\n"),
			hCSCM->bszPassword, MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);

	return bIdentityValid && bPasswordValid;

	} /* Login_CheckValid */

