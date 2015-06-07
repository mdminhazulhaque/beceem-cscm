/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// -------------------------------------------------------
// System-dependent socket functions: WIN32
// -------------------------------------------------------

#ifndef SOCKET_FUNCTIONS_H
#define SOCKET_FUNCTIONS_H

#include <stdarg.h>
#include <CscmTypes.h>

#ifdef WIN32

#include <winsock.h>
typedef SOCKET WcmSocket_t;
#define CSCM_INVALID_SOCKET ((WcmSocket_t) INVALID_SOCKET)
#define CSCM_SOCKET_ERROR   SOCKET_ERROR
#define CSCM_SOCK_ADDR      SOCKADDR
#define CSCM_IPPROTO_TCP    IPPROTO_TCP
#define CSCM_BUFFER_T	   char *
#define CSCM_SHUTDOWN_TYPE  SD_BOTH
#define close_socket(x)	   closesocket(x)

#endif
#ifdef LINUX_OR_OSX

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

typedef int WcmSocket_t;
#define CSCM_INVALID_SOCKET ((WcmSocket_t) -1)
#define CSCM_SOCKET_ERROR   -1
#define CSCM_SOCK_ADDR      struct sockaddr
#define CSCM_IPPROTO_TCP    0
#define CSCM_BUFFER_T       void *
#define CSCM_SHUTDOWN_TYPE  SHUT_RDWR
#define close_socket(x)	   close(x)

#endif

#include "CscmTypes.h"

#define DEFAULT_SKT_PRINTF_BUFFER_CHAR_LEN   1536
#define DEFAULT_SKT_SEND_FRAGMENT_BYTE_LEN   1024

#define CSCM_SOCKET_RECEIVE_ERROR -98
#define CSCM_SOCKET_SEND_ERROR    -99

BOOL WcmSocket_Init(void);
void WcmSocket_DeInit(void);
WcmSocket_t WcmSocket_Create(void);
BOOL WcmSocket_Bind(WcmSocket_t hSocket, STRING szAddress, UINT32 u32Port);
BOOL WcmSocket_Connect (WcmSocket_t hSocket, STRING bszAddress, UINT32 u32Port);
void WcmSocket_Destroy(WcmSocket_t hSocket);
WcmSocket_t WcmSocket_Accept(WcmSocket_t socListen, STRING bszRemoteAddress, UINT32 u32MaxCharCount, UINT32 *pu32ErrorCode);
INT32 WcmSocket_Send(WcmSocket_t hSocket, const UINT8 * au8Buffer, UINT32 u32ByteLength);
INT32 WcmSocket_WaitReceive(WcmSocket_t hSocket, UINT32 u32WaitSec, UINT32 u32WaitMs);
INT32 WcmSocket_Receive(WcmSocket_t hSocket, UINT8 * au8Buffer, UINT32 u32ByteLength);
INT32 WcmSocket_Close(WcmSocket_t hSocket, UINT32 *pu32ErrorCode);
INT32 WcmSocket_WPrintf(WcmSocket_t hSocket, const STRING bszFormat, ...);
INT32 WcmSocket_VWPrintf(WcmSocket_t hSocket, const STRING bszFormat, va_list args);

#endif

