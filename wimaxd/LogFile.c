/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// Define this to do a quick standalone test
// #define TEST_LOGFILE_FUNCTIONS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LogFile.h"
#include "Utility.h"

#define SAVE_STRING(dst, src) \
	b_strncpy(dst, src, BCHARSOF(dst)); dst[BCHARSOF(dst)-1] = 0;

#define UINT8 unsigned char

/*Renames a log file from abcdef.xyz to abcdef.xyz.old*/
void LogFile_RenameAsOldLog(STRING logFileName)
{
	B_CHAR backLogFileName[LOGFILE_MAXPATH+5] = {0,};
	b_strncpy(backLogFileName, logFileName, b_strlen(logFileName)+1);
#ifdef WIN32		
	b_strncat(backLogFileName, ".old", BCHARSOF(backLogFileName));
	DeleteFile(backLogFileName);
#else		
	b_strncat(backLogFileName, ".old", 4);
#endif
	rename(logFileName, backLogFileName);
}

/*Deletes a backup log file abcdef.xyz.old*/
void LogFile_deleteOldLog(STRING logFileName)
{
	B_CHAR backLogFileName[256] = {0,};	
	b_strncpy(backLogFileName, logFileName, b_strlen(logFileName)+1);
#ifdef WIN32
	b_strncat(backLogFileName, ".old", BCHARSOF(backLogFileName));
	DeleteFile(backLogFileName);
#else	
	b_strncat(backLogFileName, ".old", 4);
	remove(backLogFileName);
#endif
}


// Read-only properties
STRING LogFile_FileName(hLogFile_t hLogFile)
	{
	return hLogFile->bszFileName;
	}
	
BOOL LogFile_IsOpen(hLogFile_t hLogFile)
	{
	return hLogFile->bFileOpen;
	}

hLogFile_t LogFile_Create(const STRING  bszName, const STRING  bszDesc)
	{
	hLogFile_t hLogFile;

	if ((hLogFile = WcmMalloc(sizeof(stLogFile_t))) == NULL)
		return NULL;

	SAVE_STRING(hLogFile->bszFileName, bszName)
	SAVE_STRING(hLogFile->bszFileDesc, bszDesc)
	hLogFile->bszErrMsg[0]  = 0;
	hLogFile->fsFileStream  = NULL;
	hLogFile->bFileOpen     = FALSE;
	hLogFile->bBinaryMode   = FALSE;
	hLogFile->bReadOnlyMode = FALSE;

	hLogFile->hMutex = WcmMutex_Create();

	return hLogFile;

	} /* LogFile_Create */

void LogFile_Destroy(hLogFile_t hLogFile)
	{

	if (hLogFile)
		{
		WcmMutex_Destroy(hLogFile->hMutex);
		WcmFree(hLogFile);
		}

	} /* LogFile_Destroy */

// Returns NULL if writable; error message on failure
STRING LogFile_IsWritable(hLogFile_t hLogFile)
	{

	STRING bszText = NULL;

	if (!hLogFile)
		return NULL;

	WcmMutex_Wait(hLogFile->hMutex);

	if (!hLogFile->bszFileName[0])
		bszText = NULL;
	else
		{
		FILE * fs = NULL;
		if (BcsFopen(hLogFile->bszFileName, S("wb")) == NULL) {
			fclose(fs);
			bszText = NULL;
			}
		else
			bszText = StrPrintf(hLogFile->bszErrMsg, BCHARSOF(hLogFile->bszErrMsg), 
				S("Can't open ") B_SF S(": ") B_SF, hLogFile->bszFileDesc, hLogFile->bszFileName);
		}

	WcmMutex_Release(hLogFile->hMutex);

	return bszText;

	} /* LogFile_IsWritable */

STRING LogFile_Open(hLogFile_t hLogFile,  BOOL bBinaryMode, BOOL bReadOnlyMode)
	{
	STRING bszRetVal = NULL;

	if (!hLogFile)
		return NULL;

	WcmMutex_Wait(hLogFile->hMutex);

	hLogFile->fsFileStream = NULL;

	if (!hLogFile->bszFileName[0])
		bszRetVal = S("Log file name is not set");

	else
		{

		if (bBinaryMode)
			{
			if (bReadOnlyMode)
				hLogFile->fsFileStream = BcsFopenShare(hLogFile->bszFileName, S("rb"), TRUE);
			else
				hLogFile->fsFileStream = BcsFopenShare(hLogFile->bszFileName, S("wb"), FALSE);
			}
		else
			{
			if (bReadOnlyMode)
				hLogFile->fsFileStream = BcsFopenShare(hLogFile->bszFileName, S("rt"), TRUE);
			else
				hLogFile->fsFileStream = BcsFopenShare(hLogFile->bszFileName, S("wt"), FALSE);
			}

		if (hLogFile->fsFileStream == NULL)
			bszRetVal = StrPrintf(hLogFile->bszErrMsg, BCHARSOF(hLogFile->bszErrMsg), 
				S("Can't open Log File: "), hLogFile->bszFileName);
		}

	if (!bszRetVal)
		{
		hLogFile->bFileOpen     = TRUE;
		hLogFile->bBinaryMode   = bBinaryMode;
		hLogFile->bReadOnlyMode = bReadOnlyMode;
		}

	WcmMutex_Release(hLogFile->hMutex);

	return bszRetVal;

	} /* LogFile_Open */

void LogFile_Close(hLogFile_t hLogFile)
	{

	if (!hLogFile)
		return;

	WcmMutex_Wait(hLogFile->hMutex);
	if (hLogFile->bFileOpen && hLogFile->fsFileStream != NULL)
		fclose(hLogFile->fsFileStream);

	hLogFile->bFileOpen     = FALSE;
	hLogFile->bBinaryMode   = FALSE;
	hLogFile->bReadOnlyMode = FALSE;

	hLogFile->fsFileStream = NULL;
	WcmMutex_Release(hLogFile->hMutex);
	}

void LogFile_Flush(hLogFile_t hLogFile) 
	{
	if (!hLogFile)
		return;
	WcmMutex_Wait(hLogFile->hMutex);
	fflush(hLogFile->fsFileStream);
	WcmMutex_Release(hLogFile->hMutex);

	}

BOOL LogFile_Eof(hLogFile_t hLogFile)
	{

	BOOL bRetVal = FALSE;

	if (!hLogFile)
		return TRUE;

	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen && hLogFile->fsFileStream == NULL)
		bRetVal = TRUE;
	else 
		bRetVal = feof(hLogFile->fsFileStream)!= 0;
	WcmMutex_Release(hLogFile->hMutex);

	return bRetVal;

	} /* LogFile_Eof */

BOOL LogFile_ReadUint8(hLogFile_t hLogFile, UINT8 * pu8Value) {

	UINT32 u32RetVal;
	if (!hLogFile)
		return FALSE;

	u32RetVal = LogFile_ReadBytes(hLogFile, pu8Value, sizeof(UINT8));

	return u32RetVal == sizeof(UINT8);

	} /* LogFile_ReadUint8 */

BOOL LogFile_ReadUint32(hLogFile_t hLogFile, UINT32 * pu32Value) {

	UINT32 u32RetVal;
	if (!hLogFile)
		return FALSE;

	u32RetVal = LogFile_ReadBytes(hLogFile, (UINT8 *) pu32Value, sizeof(UINT32));

	return u32RetVal == sizeof(UINT32);

	} /* LogFile_ReadUint32 */

UINT32 LogFile_ReadBytes(hLogFile_t hLogFile, UINT8 *u8Buffer, UINT32 u32NumBytes)
	{

	UINT32 u32RetVal;

	if (!hLogFile)
		return 0;

	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen || hLogFile->fsFileStream == NULL)
		u32RetVal = 0;
	else
		u32RetVal = (UINT32) fread(u8Buffer, 1, u32NumBytes, hLogFile->fsFileStream);
	WcmMutex_Release(hLogFile->hMutex);

	return u32RetVal;

	} /* LogFile_Read */

BOOL LogFile_WriteUint8(hLogFile_t hLogFile, UINT8 u8Value) {

	UINT32 u32RetVal;
	if (!hLogFile)
		return FALSE;
	u32RetVal = LogFile_WriteBytes(hLogFile, (UINT8 *) &u8Value, sizeof(UINT8));
	return u32RetVal == sizeof(UINT8);

	} /* LogFile_WriteUint8 */

BOOL LogFile_WriteUint32(hLogFile_t hLogFile, UINT32 u32Value) {

	UINT32 u32RetVal;
	if (!hLogFile)
		return FALSE;
	u32RetVal = LogFile_WriteBytes(hLogFile, (UINT8 *) &u32Value, sizeof(UINT32));
	return u32RetVal == sizeof(UINT32);

	} /* logFile_WriteUint32 */

UINT32 LogFile_WriteBytes(hLogFile_t hLogFile, UINT8 *u8Buffer, UINT32 u32NumBytes)
	{

	UINT32 u32RetVal;

	if (!hLogFile)
		return 0;

	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen || !hLogFile->bBinaryMode || 
		 hLogFile->bReadOnlyMode || hLogFile->fsFileStream == NULL)
		u32RetVal = 0;
	else
		u32RetVal = (UINT32) fwrite(u8Buffer, 1, u32NumBytes, hLogFile->fsFileStream);
	WcmMutex_Release(hLogFile->hMutex);

	return u32RetVal;

	} /* LogFile_Write */

UINT32 LogFile_WriteText(hLogFile_t hLogFile, STRING bszBuffer)
	{

	UINT32 u32RetVal;
	if (!hLogFile || !bszBuffer)
		return 0;
	
	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen || hLogFile->bBinaryMode || 
		 hLogFile->fsFileStream == NULL || hLogFile->bReadOnlyMode)
	{
		u32RetVal = 0;
	}
	else
	{
		if(hLogFile->u32LogFileMaxSize > 0 && 
				hLogFile->u32LogFileCurrentSize >= hLogFile->u32LogFileMaxSize)
		{
				hLogFile->u32LogFileCurrentSize = 0;				
				fclose(hLogFile->fsFileStream);
				LogFile_RenameAsOldLog(hLogFile->bszFileName);
				hLogFile->fsFileStream = BcsFopenShare(hLogFile->bszFileName, S("wt"), FALSE);
				if(!hLogFile->fsFileStream)
				{
					hLogFile->bFileOpen = FALSE;
					return 0;
				}			
		}
		u32RetVal = (UINT32) b_fputs(bszBuffer, hLogFile->fsFileStream);
		hLogFile->u32LogFileCurrentSize += (UINT32) b_strlen(bszBuffer);
	}
	WcmMutex_Release(hLogFile->hMutex);

	return u32RetVal;

	} /* LogFile_WriteText */

BOOL LogFile_ReadSkip(hLogFile_t hLogFile, UINT32 u32NumBytes)
	{
	BOOL bRetVal = FALSE;

	if (!hLogFile)
		return FALSE;

	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen || 
		hLogFile->fsFileStream == NULL || 
		feof(hLogFile->fsFileStream) != 0)
		bRetVal = FALSE;
	else
		bRetVal = fseek(hLogFile->fsFileStream, u32NumBytes, SEEK_CUR) == 0;
	WcmMutex_Wait(hLogFile->hMutex);

	return bRetVal;

	} /* LogFile_ReadSkip */

BOOL LogFile_WriteZero(hLogFile_t hLogFile, UINT32 u32NumBytes)
	{
	UINT32 u32Zero = 0;
	UINT8 * u8Buffer;
	BOOL bRetVal;

	if (!hLogFile)
		return FALSE;

	WcmMutex_Wait(hLogFile->hMutex);
	if (!hLogFile->bFileOpen || hLogFile->bReadOnlyMode || hLogFile->fsFileStream == NULL)
		bRetVal = FALSE;
	else if (u32NumBytes <= 0)
		bRetVal = TRUE;
	else if (u32NumBytes <= 4)
		bRetVal = LogFile_WriteBytes(hLogFile, (UINT8 *) &u32Zero, u32NumBytes) == u32NumBytes;
	else
		{
		if ((u8Buffer = (UINT8 *) WcmMalloc(u32NumBytes)) == NULL)
			return FALSE;
		memset(u8Buffer, 0, u32NumBytes);
		bRetVal = LogFile_WriteBytes(hLogFile, u8Buffer, u32NumBytes) == u32NumBytes;
		WcmFree(u8Buffer);
		}
	WcmMutex_Release(hLogFile->hMutex);

	return bRetVal;

	} /* LogFile_WriteZero */

#ifdef TEST_LOGFILE_FUNCTIONS

#define logfile_wprintf

#define LOGFILE_CALL(f)		\
	{						\
	STRING bszErrMsg;		\
	bszErrMsg = f;			\
	if (bszErrMsg)			\
		{					\
		logfile_wprintf(S("ERROR: ") B_SF, bszErrMsg);	\
		return 1;			\
		}					\
	}

int main(void)
	{

	hLogFile_t hLogFile;

	logfile_wprintf(S("\nLogFile.cpp quick test\n"));

	hLogFile = LogFile_Create(S("test.log"), S("Test log file"));
	if (hLogFile == NULL)
		logfile_wprintf(S("LogFile initialization failed.\n"));

	LOGFILE_CALL( LogFile_IsWritable(hLogFile) )
	logfile_wprintf(B_SF S(" is writable\n"), LogFile_FileName(hLogFile));

	LOGFILE_CALL( LogFile_Open(hLogFile, LogFileReadWriteMode) )
	logfile_wprintf(B_SF S(" is open\n"), LogFile_FileName(hLogFile));

	LogFile_Close(hLogFile);
	logfile_wprintf(B_SF S(" is closed\n"), LogFile_FileName(hLogFile));

	PressEnterToContinue();
	}
#endif
