/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef BECEEM_CSCM_LOGIN_H
#define BECEEM_CSCM_LOGIN_H

#include "BeceemCscmTop.h"

BOOL Login_CheckValid(BeceemCSCM_t hCSCM);
BOOL Login_IsIdentityValid(BeceemCSCM_t hCSCM);
BOOL Login_IsPasswordValid(BeceemCSCM_t hCSCM);

#endif

