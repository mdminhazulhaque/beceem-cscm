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
 * Description	:	Socket IPC library for CAPI.
 * Author		:
 * Reviewer		:
 *
 */


#ifdef WIN32
#include <winsock2.h>
#include <Time.h>
#endif
#include "socketipc.h"
void Socket_DeInit(void)
	{
#ifdef WIN32
	WSACleanup();
#endif
	} /* Socket_Init */



typedef  unsigned short widechar_t;


static size_t beceem_wcstombs(char *s, const widechar_t *pwcs, size_t n);
static size_t beceem_mbstowcs(widechar_t *pwcs, const char *s, size_t n);



static size_t beceem_wcstombs(char *s, const widechar_t *pwcs, size_t n)
{
        int count = 0;

        if (n != 0) {
                do {
                        if ((*s++ = (char) *pwcs++) == 0)
                                break;
                        count++;
                } while (--n != 0);
        }

        return count;
}

static size_t beceem_mbstowcs(widechar_t *pwcs, const char *s, size_t n)
{
        int count = 0;

        if (n != 0) {
                do {
                        if ((*pwcs++ = (widechar_t) *s++) == 0)
                                break;
                        count++;
                } while (--n != 0);
        }

        return count;
}









BOOL Socket_Init(void)
{

	#ifdef WIN32
	WSADATA wsaData;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
		return FALSE;
	#endif

return TRUE;

} 

bcm_socket_t Socket_Create(void)
	{

	return (bcm_socket_t) socket(AF_INET, SOCK_STREAM, BCM_IPPROTO_TCP);

	} /* Socket_Create */


BOOL Socket_Bind (bcm_socket_t hSocket, STRING wszAddress, UINT32 u32Port)
	{

	struct sockaddr_in saListen;
	char szAddress[20] = {0};

	// Bind to an address
	saListen.sin_family = AF_INET;
#ifdef MS_VISUAL_CPP
	wcstombs_s(&sizRetVal, szAddress, sizeof(szAddress), wszAddress, _TRUNCATE); 
#else
	beceem_wcstombs(szAddress, (widechar_t*)wszAddress, sizeof(szAddress)); 
#endif
	saListen.sin_addr.s_addr = inet_addr(szAddress);
	saListen.sin_port        = htons((UINT16) u32Port);
	if (bind( hSocket, (BCM_SOCK_ADDR*) &saListen, sizeof(saListen)) == BCM_SOCKET_ERROR) 
		{
		close_socket(hSocket);
		return FALSE;
		}

	// Listen on socket
	if (listen( hSocket, 1 ) == BCM_SOCKET_ERROR)
		{
		close_socket(hSocket);
		return FALSE;		
		}

	return TRUE;

	} /* Socket_Bind */

BOOL Socket_Connect (bcm_socket_t hSocket, STRING wszAddress, UINT32 u32Port)
	{

	struct sockaddr_in saConnect;
	char szAddress[20];


	// Connect to an address
	saConnect.sin_family = AF_INET;
#ifdef MS_VISUAL_CPP
	wcstombs_s(&sizRetVal, szAddress, sizeof(szAddress), wszAddress, _TRUNCATE); 
#else
	beceem_wcstombs(szAddress, (widechar_t *)wszAddress, sizeof(szAddress)); 
#endif
	saConnect.sin_addr.s_addr = inet_addr(szAddress);
	saConnect.sin_port        = htons((UINT16) u32Port);

	// Connect to server
	if ( connect( hSocket, (BCM_SOCK_ADDR*) &saConnect, sizeof(saConnect) ) == BCM_SOCKET_ERROR) 
		return FALSE;

	return TRUE;

	} /* Socket_Connect */

void Socket_Destroy (bcm_socket_t hSocket)
	{

	if (hSocket)
		close_socket(hSocket);

	} /* Socket_Destroy */

bcm_socket_t Socket_Accept(bcm_socket_t socListen, STRING wszRemoteAddress, UINT32 u32MaxCharCount, UINT32 *pu32ErrorCode)
	{
	struct sockaddr_in saAccept;
#ifdef WIN32
	int i32SocAddrLen = (int) sizeof(saAccept);
#else
	socklen_t sizSocAddrLen = sizeof(saAccept);
#endif
	char *szAddress;
	bcm_socket_t socAccept;
#ifdef MS_VISUAL_CPP
	size_t iSize;
#endif

#ifdef WIN32
	socAccept = accept(socListen, (BCM_SOCK_ADDR *) &saAccept, &i32SocAddrLen); 
	if (socAccept == BCM_INVALID_SOCKET && pu32ErrorCode)
		*pu32ErrorCode = WSAGetLastError();
	
#else
	socAccept = accept(socListen, (BCM_SOCK_ADDR *) &saAccept, &sizSocAddrLen); 
	if (socAccept == BCM_INVALID_SOCKET && pu32ErrorCode)
		*pu32ErrorCode = errno;
#endif

	szAddress = inet_ntoa(saAccept.sin_addr);
	if (wszRemoteAddress && u32MaxCharCount > 2)
#ifdef MS_VISUAL_CPP
		mbstowcs_s(&iSize, wszRemoteAddress, u32MaxCharCount, szAddress, _TRUNCATE);
#else
		beceem_mbstowcs((widechar_t *)wszRemoteAddress, inet_ntoa(saAccept.sin_addr), u32MaxCharCount);
#endif

	return socAccept;

	} /* Socket_Accept */

INT32 Socket_Send(bcm_socket_t hSocket, const UINT8 *au8Buffer, UINT32 u32ByteLength)
	{

	// Fragment
	INT32 i32RetVal = 0;

	if (hSocket == BCM_INVALID_SOCKET)
		return BCM_INVALID_SOCKET;
	i32RetVal = send(hSocket, (const BCM_BUFFER_T) au8Buffer, u32ByteLength, 0);

		if (i32RetVal == BCM_SOCKET_ERROR)
			{
		i32RetVal = BCM_SOCKET_SEND_ERROR;
			}

	return i32RetVal;


	} /* Socket_Send */
	

// Returns: 1, if data are waiting
//          0, if wait timed out
INT32 Socket_WaitReceive(bcm_socket_t hSocket, UINT32 u32WaitSec, UINT32 u32WaitMs)
	{

	struct timeval stTimeout;
	fd_set  stFdSet;

	stTimeout.tv_sec  = u32WaitSec;
	stTimeout.tv_usec = u32WaitMs * 1000;

	FD_ZERO(&stFdSet);

#ifdef MS_VISUAL_CPP
#pragma warning (push)
#pragma warning (disable: 4127)
#endif

#if 0 // function is not used
	FD_SET(hSocket, &stFdSet);
#endif

#ifdef MS_VISUAL_CPP
#pragma warning (pop)
#endif

	return select((int) (hSocket+1), &stFdSet, NULL, NULL, &stTimeout);

	} /* Socket_WaitReceive */

INT32 Socket_Receive(bcm_socket_t hSocket, UINT8 *au8Buffer, UINT32 u32ByteLength)
	{

int iRetVal;
	UINT32 u32RecvLength = u32ByteLength;
	if (hSocket != BCM_INVALID_SOCKET)
		{
		iRetVal = recv(hSocket, (BCM_BUFFER_T) au8Buffer, u32RecvLength, 0);
		if (iRetVal == BCM_SOCKET_ERROR || iRetVal % 2 != 0)
			return BCM_SOCKET_RECEIVE_ERROR;
		else
			return iRetVal;

		}
	else
		return BCM_INVALID_SOCKET;

	} /* Socket_Receive */

INT32 Socket_Close(bcm_socket_t hSocket, UINT32 *pu32ErrorCode)
	{
	int iResult;

	if (hSocket != BCM_INVALID_SOCKET)
		{
		iResult = shutdown(hSocket, BCM_SHUTDOWN_TYPE);
		if (iResult == BCM_SOCKET_ERROR && pu32ErrorCode) 
#ifdef WIN32
			*pu32ErrorCode = WSAGetLastError();
#endif
#ifdef LINUX
			*pu32ErrorCode = errno; 
#endif

		close_socket(hSocket);

		return iResult;
		}
	else
		return 0;

	} /* Socket_Close */




