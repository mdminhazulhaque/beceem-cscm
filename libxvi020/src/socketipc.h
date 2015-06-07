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
 * Description	:	Socket IPC library for CAPI - APIs.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef SOCKET_IPC_H
#define SOCKET_IPC_H


#include <stdarg.h>
#include "OMADMInfo.h"
#ifdef WIN32
#include "win32/Thread.h"
#include "bcmtypes.h"
#include <winsock.h>
typedef SOCKET bcm_socket_t;
#define BCM_INVALID_SOCKET (bcm_socket_t)INVALID_SOCKET
#define BCM_SOCKET_ERROR   SOCKET_ERROR
#define BCM_SOCK_ADDR      SOCKADDR
#define BCM_IPPROTO_TCP    IPPROTO_TCP
#define BCM_BUFFER_T	   char *
#define BCM_SHUTDOWN_TYPE  SD_BOTH
#define close_socket(x)	   closesocket(x)

#endif
#ifdef LINUX

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "linux/Thread.h"
#include <wchar.h>

typedef int bcm_socket_t;
#define BCM_INVALID_SOCKET ((bcm_socket_t) -1)
#define BCM_SOCKET_ERROR   -1
#define BCM_SOCK_ADDR      struct sockaddr
#define BCM_IPPROTO_TCP    0
#define BCM_BUFFER_T       void *
#define BCM_SHUTDOWN_TYPE  SHUT_RDWR
#define close_socket(x)	   close(x)


typedef wchar_t WCHAR;
typedef WCHAR *STRING;
#define WCHARSOF(x) (sizeof(x)/sizeof(WCHAR))

#endif


#define DEFAULT_LISTEN_ADDRESS  	L"127.0.0.1"
#define DEFAULT_LISTEN_PORT			8440



#define DEFAULT_SKT_SEND_FRAGMENT_BYTE_LEN   1024
#define MAX_IPC_INTERFACES_SUPPORTED_OMA_DM_CLIENT 4
#define BCM_SOCKET_RECEIVE_ERROR -98
#define BCM_SOCKET_SEND_ERROR    -99

#define MAX_MESSAGE_LENGTH_VDM_CAPI 1024


typedef struct _BCM_VDM_IPC_INTF
{
	E_VDM_IPC_INTERFACE eInterfaceType; // Type of interface that OMA-DM cleint is interacting with ex:CAPI,CM etc.
	UINT uiCommand; //Maps to each of the CAPI dependent API on OMA-DM implementation
	INT iStatus; //Usefull while providing response for API execution
	UINT uiMessageLength;
	UCHAR uchMessage[MAX_MESSAGE_LENGTH_VDM_CAPI];

}BCM_VDM_IPC_INTF,*PBCM_VDM_IPC_INTF;


typedef struct _ST_SOCKET_INFO_
{
	bcm_socket_t socketListen;
	UINT32 u32ClientCount;
	E_VDM_IPC_INTERFACE aClientType[MAX_IPC_INTERFACES_SUPPORTED_OMA_DM_CLIENT];
	bcm_socket_t aClientsConnected[MAX_IPC_INTERFACES_SUPPORTED_OMA_DM_CLIENT];
	THREAD	aThreadPerClient[MAX_IPC_INTERFACES_SUPPORTED_OMA_DM_CLIENT];
	
}ST_SOCKET_INFO,*PST_SOCKET_INFO;

BOOL Socket_Init(void);
void Socket_DeInit(void);
bcm_socket_t Socket_Create(void);
BOOL Socket_Bind(bcm_socket_t hSocket, STRING szAddress, UINT32 u32Port);
BOOL Socket_Connect (bcm_socket_t hSocket, STRING wszAddress, UINT32 u32Port);
void Socket_Destroy(bcm_socket_t hSocket);
bcm_socket_t Socket_Accept(bcm_socket_t socListen, STRING wszRemoteAddress, UINT32 u32MaxCharCount, UINT32 *pu32ErrorCode);
INT32 Socket_Send(bcm_socket_t hSocket, const UINT8 * au8Buffer, UINT32 u32ByteLength);
INT32 Socket_WaitReceive(bcm_socket_t hSocket, UINT32 u32WaitSec, UINT32 u32WaitMs);
INT32 Socket_Receive(bcm_socket_t hSocket, UINT8 * au8Buffer, UINT32 u32ByteLength);
INT32 Socket_Close(bcm_socket_t hSocket, UINT32 *pu32ErrorCode);


#endif /* SOCKET_IPC_H */


