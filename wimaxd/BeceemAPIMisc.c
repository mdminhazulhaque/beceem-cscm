/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include "BeceemAPI.h"

// -----------------------------------------------------------------------------------------
// void BeceemAPI_PrintConStat(STRING bszFormat, ...)
//
// Provides printf(...) access to the 'ConStat' print context
//
void BeceemAPI_PrintConStat(BeceemAPI_t hAPI, STRING bszFormat, ...)
	{

	va_list args;

	va_start(args, bszFormat);

	if (hAPI == NULL)
		return;

	Print_Formatted_ArgPtr(hAPI->hPrintConStat, bszFormat, args);

	} /* BeceemAPI_PrintConStat */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_PrintDiag(hAPI, STRING bszFormat, ...)
//
// Provides printf(...) access to the 'Diag' print context
//
void BeceemAPI_PrintDiag(UINT32 u32Level, BeceemAPI_t hAPI, STRING bszFormat, ...)
	{

	va_list args;

	if (hAPI == NULL)
		return;

	if (u32Level <= hAPI->u32CSCMDebugLogLevel)
		{
		//Print_Append(hAPI->hPrintDiag, BeceemAPI_DateTime(hAPI));
		va_start(args, bszFormat);

		Print_Formatted_ArgPtr(hAPI->hPrintDiag, bszFormat, args);
		//Print_Newline(hAPI->hPrintDiag);
		}

	} /* BeceemAPI_PrintDiag */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_PrintDiag_ArgPtr(hAPI, STRING bszFormat, ...)
//
// Provides printf(...) access to the 'Diag' print context
//
void BeceemAPI_PrintDiag_ArgPtr(UINT32 u32Level, BeceemAPI_t hAPI, STRING bszFormat, va_list args)
	{

	if (hAPI == NULL)
		return;

	if (u32Level <= hAPI->u32CSCMDebugLogLevel)
		{
		Print_Formatted_ArgPtr(hAPI->hPrintDiag, bszFormat, args);
		//Print_Newline(hAPI->hPrintDiag);
		}

	} /* BeceemAPI_PrintDiag */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_PrintStats(STRING bszFormat, ...)
//
// Provides printf(...) access to the 'Stats' print context
//
void BeceemAPI_PrintStats(BeceemAPI_t hAPI, STRING bszFormat, ...)
	{

	va_list args;

	va_start(args, bszFormat);

	if (hAPI == NULL)
		return;

	Print_Formatted_ArgPtr(hAPI->hPrintStats, bszFormat, args);

	} /* BeceemAPI_PrintStats */

// -----------------------------------------------------------------------------------------
// void BeceemAPI_PrintVersions(STRING bszFormat, ...)
//
// Provides printf(...) access to the 'Versions' print context
//
void BeceemAPI_PrintVersions(BeceemAPI_t hAPI, STRING bszFormat, ...)
	{

	va_list args;

	va_start(args, bszFormat);

	if (hAPI == NULL)
		return;

	Print_Formatted_ArgPtr(hAPI->hPrintVersions, bszFormat, args);

	} /* BeceemAPI_PrintVersions */
