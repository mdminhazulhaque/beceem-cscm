/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#include <CscmTypes.h>

#define MAX_KEYWORD_CHARS 15
#define DELIMITERS  S(" \t")

#define BECEEM_CSCM_SERVICE_NAME S("BeceemCMServer")

UINT32 CommandID(const STRING bszCommand);
UINT32 ParseCommandLine(STRING bszCommand, 
				STRING abszArgStrings[], UINT32 u32MaxNumArgs, UINT32 *u32ActualNumArgs);
stCmdInformation_t *GetCmdInfo(UINT32 u32ID);

#ifdef LINUX_OR_OSX
void InstallSignalHandlers(void);
#endif
