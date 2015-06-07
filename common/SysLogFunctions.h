
/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------
// Functions for writing to system log
// -------------------------------------------------------

#ifndef DEBUG_FUNCTIONS_H
#define DEBUG_FUNCTIONS_H

#define SYSLOG_INFO        0
#define SYSLOG_WARNING     1
#define SYSLOG_ERROR       2
#define SYSLOG_FATAL_ERROR 3
#define SYSLOG_DEBUG       4

void SysLogOpen(void);
void SysLogWPrintf(UINT32 u32Level, STRING bszFormat, ...);
void SysLogClose(void);

#endif

