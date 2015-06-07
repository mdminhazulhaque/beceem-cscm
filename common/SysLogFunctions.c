/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------
// Functions for writing to system log
// -------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

// CscmTypes.h must appear before any use of LINUX_OR_OSX 
#include "CscmTypes.h"

#ifdef LINUX_OR_OSX
#include <syslog.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "SysLogFunctions.h"

void SysLogOpen(void)
	{
#ifdef LINUX_OR_OSX
	openlog("Beceem CM Server", LOG_PID | LOG_NDELAY, LOG_USER);
#endif
	} /* SysLogOpen */

void SysLogWPrintf(UINT32 u32Level, STRING bszFormat, ...)
	{
	STRING bszPrefix;
	B_CHAR bszMessage[256];
#ifdef WIN32
	B_CHAR bszBuffer[320];
#endif
	va_list args;

	switch (u32Level)
		{
		case SYSLOG_INFO:
			bszPrefix = S("INFO");
			break;
		case SYSLOG_WARNING:
			bszPrefix = S("WARNING");
			break;
		case SYSLOG_ERROR:
			bszPrefix = S("ERROR");
			break;
		case SYSLOG_FATAL_ERROR:
			bszPrefix = S("FATAL ERROR");
			break;
		case SYSLOG_DEBUG:
			bszPrefix = S("DEBUG");
			break;
		default:
			bszPrefix = EMPTY_STRING;
			break;
		}

	va_start(args, bszFormat);

#ifdef MS_VISUAL_CPP
	b_vsnprintf(bszMessage, BCHARSOF(bszMessage), bszFormat, args);
	b_snprintf_s(bszBuffer, BCHARSOF(bszBuffer), _TRUNCATE, S("Beceem CM Server [") B_SF S("] - ") B_SFNL,
		bszPrefix, bszMessage);
	OutputDebugString(bszBuffer);
#else
	b_vsnprintf(bszMessage, BCHARSOF(bszMessage), bszFormat, args);
#ifdef ENABLE_WIDE_CHAR
	syslog(LOG_NOTICE, "[%ls] - %ls\n", bszPrefix, bszMessage);
#else
	syslog(LOG_NOTICE, "[%s] - %s\n", bszPrefix, bszMessage);
#endif
#endif

	} /* SysLogPrintf */

void SysLogClose(void)
	{
#ifdef LINUX_OR_OSX
	closelog();
#endif
	} /* SysLogClose */
