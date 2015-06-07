/* ---------------------------------------------------- */
/*  Client-Server Connection Manager                    */
/*  Copyright 2009 Beceem Communications, Inc.          */
/*                                                      */
/*  Carlos Puig (cpuig@beceem.com)                      */
/*  July 2009                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------
// System-dependent functions for Linux process management`
// -------------------------------------------------------

#include <sys/types.h>
#include "CscmTypes.h"

INT32 FindPid(char *szCmdLine, pid_t *aPid, UINT32 u32SizPid);
