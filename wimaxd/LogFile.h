/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <stdio.h>
#include "CscmTypes.h"
#include "ThreadFunctions.h"

#define LOGFILE_MAXPATH   256
#define LOGFILE_MAXDESC   256
#define LOGFILE_MAXERRMSG 256

#define LOGFILE_BINARY_MODE     TRUE
#define LOGFILE_TEXT_MODE       FALSE
#define LOGFILE_READ_ONLY_MODE  TRUE
#define LOGFILE_READ_WRITE_MODE FALSE

// Private data
typedef struct 
	{
	B_CHAR bszFileName[LOGFILE_MAXPATH];
	B_CHAR bszFileDesc[LOGFILE_MAXDESC];
	B_CHAR bszErrMsg[LOGFILE_MAXERRMSG];
	FILE * fsFileStream;
	BOOL bFileOpen;
	BOOL bBinaryMode;
	BOOL bReadOnlyMode;
	WcmMutex_t hMutex;
	UINT32 u32LogFileMaxSize; 
	UINT32 u32LogFileCurrentSize; 
	} stLogFile_t;

typedef stLogFile_t *hLogFile_t;

hLogFile_t LogFile_Create(const STRING  bszName, const STRING   bszDesc);
void LogFile_Destroy(hLogFile_t hLogFile);

// Default is text mode with read write access
STRING LogFile_Open(hLogFile_t hLogFile, BOOL bBinaryMode, BOOL bReadOnlyMode);
void LogFile_Close(hLogFile_t hLogFile);

UINT32 LogFile_ReadBytes(hLogFile_t hLogFile, UINT8 * u8Buffer, UINT32 u32NumBytes);
UINT32 LogFile_WriteBytes(hLogFile_t hLogFile, UINT8 * u8Buffer, UINT32 u32NumBytes);
UINT32 LogFile_WriteText(hLogFile_t hLogFile, STRING   bszBuffer);
#define DEFREAD(stLogFile, v)  {return LogFile_ReadBytes(stLogFile, (UINT8 *) v, sizeof(*v)) == sizeof(*v);}

BOOL LogFile_ReadUint8 (hLogFile_t hLogFile, UINT8 * pu8Value);
BOOL LogFile_ReadUint32(hLogFile_t hLogFile, UINT32 * pu32Value);

#define DEFWRITE(stLogFile, v) {return LogFile_WriteBytes(stLogFile, (UINT8 *) &v, sizeof(v)) == sizeof(v);}
BOOL LogFile_WriteUint8 (hLogFile_t hLogFile, UINT8 u8Value);
BOOL LogFile_WriteUint32(hLogFile_t hLogFile, UINT32 u32Value);

BOOL LogFile_ReadSkip(hLogFile_t hLogFile, UINT32 u32NumBytes);
BOOL LogFile_WriteZero(hLogFile_t hLogFile, UINT32 u32NumBytes);

void LogFile_Flush(hLogFile_t hLogFile);
BOOL LogFile_Eof(hLogFile_t hLogFile);
STRING  LogFile_IsWritable(hLogFile_t hLogFile);

// Read-only properties
STRING  LogFile_FileName(hLogFile_t hLogFile);
BOOL LogFile_IsOpen(hLogFile_t hLogFile);

void LogFile_deleteOldLog(STRING logFileName);
void LogFile_RenameAsOldLog(STRING logFileName);


#endif

