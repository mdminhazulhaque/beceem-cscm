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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#include <Time.h>
#endif
#include "SocketFunctions.h"
#include "Utility.h"

void WcmSocket_DeInit(void)
	{
#ifdef WIN32
	WSACleanup();
#endif
	} /* WcmSocket_Init */

BOOL WcmSocket_Init(void)
	{

#ifdef WIN32
	WSADATA wsaData;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != NO_ERROR)
		return FALSE;
#endif

	return TRUE;

	} /* WcmSocket_Init */

WcmSocket_t WcmSocket_Create(void)
	{

	return (WcmSocket_t) socket(AF_INET, SOCK_STREAM, CSCM_IPPROTO_TCP);

	} /* WcmSocket_Create */

BOOL WcmSocket_Bind (WcmSocket_t hSocket, STRING bszAddress, UINT32 u32Port)
	{

	struct sockaddr_in saListen;
	char szAddress[20];

	// Bind to an address
	memset(&saListen, 0, sizeof(saListen));
	saListen.sin_family = AF_INET;
	b_bcstombs(szAddress, bszAddress, sizeof(szAddress)); 

	saListen.sin_addr.s_addr = inet_addr(szAddress);
	saListen.sin_port        = htons((UINT16) u32Port);
	if (bind( hSocket, (CSCM_SOCK_ADDR*) &saListen, sizeof(saListen)) == CSCM_SOCKET_ERROR) 
		{
		close_socket(hSocket);
		return FALSE;
		}

	// Listen on socket
	if (listen( hSocket, 1 ) == CSCM_SOCKET_ERROR)
		{
		close_socket(hSocket);
		return FALSE;		
		}

	return TRUE;

	} /* WcmSocket_Bind */

BOOL WcmSocket_Connect (WcmSocket_t hSocket, STRING bszAddress, UINT32 u32Port)
	{

	struct sockaddr_in saConnect;
	char szAddress[20];

	// Connect to an address
	memset(&saConnect, 0, sizeof(saConnect));
	saConnect.sin_family = AF_INET;
	b_bcstombs(szAddress, bszAddress, sizeof(szAddress)); 

	saConnect.sin_addr.s_addr = inet_addr(szAddress);
	saConnect.sin_port        = htons((UINT16) u32Port);

	// Connect to server
	if ( connect( hSocket, (CSCM_SOCK_ADDR*) &saConnect, sizeof(saConnect) ) == CSCM_SOCKET_ERROR) 
		return FALSE;

	return TRUE;

	} /* WcmSocket_Connect */

void WcmSocket_Destroy (WcmSocket_t hSocket)
	{

	if (hSocket)
		close_socket(hSocket);

	} /* WcmSocket_Destroy */

WcmSocket_t WcmSocket_Accept(WcmSocket_t socListen, STRING bszRemoteAddress, UINT32 u32MaxCharCount, UINT32 *pu32ErrorCode)
	{
	struct sockaddr_in saAccept;
#ifdef WIN32
	int i32SocAddrLen = (int) sizeof(saAccept);
#else
	size_t sizSocAddrLen = sizeof(saAccept);
#endif
	char *szAddress;
	WcmSocket_t socAccept;

#ifdef WIN32
	socAccept = accept(socListen, (CSCM_SOCK_ADDR *) &saAccept, &i32SocAddrLen); 
	if (socAccept == CSCM_INVALID_SOCKET && pu32ErrorCode)
		*pu32ErrorCode = WSAGetLastError();
#else
	socAccept = accept(socListen, (CSCM_SOCK_ADDR *) &saAccept, &sizSocAddrLen); 
	if (socAccept == CSCM_INVALID_SOCKET && pu32ErrorCode)
		*pu32ErrorCode = errno;
#endif

	szAddress = inet_ntoa(saAccept.sin_addr);
	if (bszRemoteAddress && u32MaxCharCount > 2)
		b_mbstobcs(bszRemoteAddress, inet_ntoa(saAccept.sin_addr), u32MaxCharCount);

	return socAccept;

	} /* WcmSocket_Accept */

INT32 WcmSocket_Send(WcmSocket_t hSocket, const UINT8 *au8Buffer, UINT32 u32ByteLength)
	{

	// Fragment
	INT32 i32RetVal, i32SendCount = 0;
	UINT32 n, m, u32FragLength = 0, u32MaxFragLength = DEFAULT_SKT_SEND_FRAGMENT_BYTE_LEN;
	UINT8 u8ConvertBuffer[DEFAULT_SKT_SEND_FRAGMENT_BYTE_LEN];
	UINT32 u32ConvertBufferLength = 0;
	const UINT8 *au8Src;
	UINT8 *au8Dst;
	BOOL bBigEndian, bBcharSize4;
	UINT32 u32One = 1;

	bBigEndian  = *((UINT8 *) &u32One) == 0;
	bBcharSize4 = sizeof(B_CHAR) == 4;

	if (hSocket == CSCM_INVALID_SOCKET)
		return CSCM_INVALID_SOCKET;

	u32MaxFragLength = (DEFAULT_SKT_SEND_FRAGMENT_BYTE_LEN / 2) * sizeof(B_CHAR);
			
	for (n=0; n < u32ByteLength; n += u32MaxFragLength)
		{
	
		if (u32ByteLength - n > u32MaxFragLength)
			u32FragLength = u32MaxFragLength;
		else 
			u32FragLength = u32ByteLength - n;
		
		au8Src = &au8Buffer[n];
		au8Dst = &u8ConvertBuffer[0];
		u32ConvertBufferLength = 0;
		for (m=0; m < u32FragLength; m+=sizeof(B_CHAR))
			{
#ifdef ENABLE_WIDE_CHAR
			if (bBigEndian)
				{
				au8Dst[0] = bBcharSize4 ? au8Src[3] : au8Src[1];
				au8Dst[1] = bBcharSize4 ? au8Src[2] : au8Src[0];
				}
			else
				{
				au8Dst[0] = au8Src[0];
				au8Dst[1] = au8Src[1];
				}
#else
			au8Dst[0] = au8Src[0];
			au8Dst[1] = 0;
#endif
			au8Dst += 2;
			au8Src += sizeof(B_CHAR);
			u32ConvertBufferLength += 2;
			}

		i32RetVal = send(hSocket, (const CSCM_BUFFER_T) u8ConvertBuffer, u32ConvertBufferLength, 0);

		if (i32RetVal == CSCM_SOCKET_ERROR)
			{
			i32SendCount = CSCM_SOCKET_SEND_ERROR;
			break;
			}
		else
			i32SendCount += (i32RetVal / 2) * sizeof(B_CHAR);

		}

	return i32SendCount;

	} /* WcmSocket_Send */

// Returns: 1, if data are waiting
//          0, if wait timed out
INT32 WcmSocket_WaitReceive(WcmSocket_t hSocket, UINT32 u32WaitSec, UINT32 u32WaitMs)
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

	FD_SET(hSocket, &stFdSet);

#ifdef MS_VISUAL_CPP
#pragma warning (pop)
#endif

	return select((int) (hSocket+1), &stFdSet, NULL, NULL, &stTimeout);

	} /* WcmSocket_WaitReceive */

INT32 WcmSocket_Receive(WcmSocket_t hSocket, UINT8 *au8Buffer, UINT32 u32ByteLength)
	{

	int n, iRetVal;
	UINT32 u32RecvLength = u32ByteLength;
	UINT8  *au8Src;
#ifdef ENABLE_WIDE_CHAR
	UINT16 *au16Src, *au16Dst;
	UINT8  au8Temp;
#else
	UINT8  *au8Dst;
#endif
	BOOL bBigEndian, bBcharSize4;
	UINT32 u32One = 1;

	bBigEndian = *((UINT8 *) &u32One) == 0;
	bBcharSize4 = sizeof(B_CHAR) == 4;

	if (bBcharSize4)
		u32RecvLength = u32ByteLength/2;

	if (hSocket != CSCM_INVALID_SOCKET)
		{
		iRetVal = recv(hSocket, (CSCM_BUFFER_T) au8Buffer, u32RecvLength, 0);
		if (iRetVal == CSCM_SOCKET_ERROR || iRetVal % 2 != 0)
			return CSCM_SOCKET_RECEIVE_ERROR;
		else
			{

#ifdef ENABLE_WIDE_CHAR
			// Net format is little endian for this application
			if (bBigEndian)
				{
				au8Src = &au8Buffer[0];
				for (n=0; n<iRetVal; n+=2)
					{
					au8Temp   = au8Src[0];
					au8Src[0] = au8Src[1];
					au8Src[1] = au8Temp;
					au8Src += 2;
					}
				}

			if (bBcharSize4)
				{
				au16Src = (UINT16 *) &au8Buffer[iRetVal-2];
				au16Dst = (UINT16 *) &au8Buffer[2*(iRetVal-2)];
				for (n=0; n<iRetVal; n+=2)
					{
					if (bBigEndian)
						{
						au16Dst[1] = au16Src[0];
						au16Dst[0] = 0;
						}
					else
						{
						au16Dst[0] = au16Src[0];
						au16Dst[1] = 0;
						}
					au16Src -= 1;
					au16Dst -= 2;
					}
				return 2*iRetVal;
				}
			else
				return iRetVal;
#else
			// Net format is little endian for this application
			au8Src = &au8Buffer[0];
			au8Dst = &au8Buffer[0];
			for (n=0; n<iRetVal; n+=2)
				{
				*au8Dst++ = au8Src[n];
				}
			return iRetVal / 2;
#endif
			}
		}
	else
		return CSCM_INVALID_SOCKET;

	} /* WcmSocket_Receive */

INT32 WcmSocket_Close(WcmSocket_t hSocket, UINT32 *pu32ErrorCode)
	{
	int iResult;

	if (hSocket != CSCM_INVALID_SOCKET)
		{
		iResult = shutdown(hSocket, CSCM_SHUTDOWN_TYPE);
		if (iResult == CSCM_SOCKET_ERROR && pu32ErrorCode) 
#ifdef WIN32
			*pu32ErrorCode = WSAGetLastError();
#endif
#ifdef LINUX_OR_OSX
			*pu32ErrorCode = errno; 
#endif

		close_socket(hSocket);

		return iResult;
		}
	else
		return 0;

	} /* WcmSocket_Close */

INT32 WcmSocket_WPrintf(WcmSocket_t hSocket, const STRING bszFormat, ...)
	{

	va_list args;

	va_start(args, bszFormat);

	return WcmSocket_VWPrintf(hSocket, bszFormat, args);

	} /* WcmSocket_WPrintf */

INT32 WcmSocket_VWPrintf(WcmSocket_t hSocket, const STRING bszFormat, va_list args)
	{

	B_CHAR bszBuffer[DEFAULT_SKT_PRINTF_BUFFER_CHAR_LEN];

	b_vsnprintf(bszBuffer, DEFAULT_SKT_PRINTF_BUFFER_CHAR_LEN, bszFormat, args);

	return WcmSocket_Send(hSocket, (UINT8 *) bszBuffer, (UINT32) BCHARS2BYTES(b_strlen(bszBuffer) + 1));

	} /* WcmSocket_VWPrintf */


