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
 * Description	:	Beceem defined types.
 * Author		:
 * Reviewer		:
 *
 */


#ifndef BCM_TYPES_H
#define BCM_TYPES_H

#ifdef WIN32
#include <windows.h>
#include <basetsd.h>
#else

#include <netinet/in.h>

typedef void VOID;
typedef void *PVOID;
typedef void *LPVOID;

typedef signed char CHAR;
typedef signed char* PCHAR;

typedef unsigned char UCHAR;
typedef unsigned char *PUCHAR;
typedef unsigned char BYTE;

typedef signed short SHORT;
typedef signed short *PSHORT;

typedef unsigned short USHORT;
typedef unsigned short *PUSHORT;

typedef signed int INT, INT32;
typedef signed int *PINT;

typedef unsigned int UINT;
typedef unsigned int *PUINT;


typedef long long ULONG64;
typedef unsigned int DWORD;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;

typedef float FLOAT;

typedef void *HANDLE;

typedef UCHAR BOOLEAN,BOOL;

#define INFINITE 0xFFFFFFFF
#define INVALID_HANDLE_VALUE (void *)-1
#define TRUE 1
#define FALSE 0

#endif

typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef signed short	SINT16;
typedef unsigned short	UINT16;
typedef signed int		SINT32;
typedef unsigned int	UINT32;
typedef wchar_t        WCHAR;
typedef WCHAR         *STRING;

#define WCHARS2BYTES(x) ((x) * sizeof(WCHAR))
#define BYTES2WCHARS(x) ((x) / sizeof(WCHAR))
#define WCHARSOF(x)	    (sizeof(x) / sizeof(WCHAR))


#endif /* BCM_TYPES_H */

