/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                                  */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

// Define this to do a quick standalone test
//#define TEST_PRINT_CONTEXT_FUNCTIONS

#include <stdlib.h>
#include "PrintContext.h"
#include "LogFile.h"
#include "Utility.h"

static BOOL Print_Reserve (PrintContext_t hPrint, UINT32 u32SizeNeeded);

// ==============================================================================
// Print context create & destroy functions
// ==============================================================================

PrintContext_t Print_Create(BOOL bPrintConsole, UINT32 u32InitBufferSize, UINT32 u32BufferBlockSize)
	{

	PrintContext_t hPrint;

	if ((hPrint = WcmMalloc(sizeof(stPrintContext_t))) == NULL)
		return NULL;

	if ((hPrint->bszTextBuffer = WcmMalloc(BCHARS2BYTES(u32InitBufferSize))) == NULL)
		return NULL;

	// Initialize the text buffer
	hPrint->u32TextBufferSize = u32InitBufferSize;
	hPrint->u32NextBufferChar = 0;
	b_memset(hPrint->bszTextBuffer, 0, u32InitBufferSize);

	hPrint->u32BufferBlockSize = u32BufferBlockSize;

	// Initialize the rest
	hPrint->hLogFile       = NULL;
	hPrint->bszLogFileName = NULL;
	hPrint->bEnabled       = TRUE;
	hPrint->bBuffered      = FALSE;
	hPrint->bPrintConsole  = bPrintConsole;

	if (bPrintConsole)
#ifdef WIN32
		hPrint->fPrintConsole = stdout;
#else
#ifdef ENABLE_WIDE_CHAR
		{
		if (fwide(stdout,0) > 0)
			hPrint->fPrintConsole = stdout;
		else 
			{
			hPrint->fPrintConsole = fopen("/dev/tty", "w");
			if (hPrint->fPrintConsole != NULL)
				fwide(hPrint->fPrintConsole, 1);
			}
		}
#else
		{
		hPrint->fPrintConsole = stdout;
		}
#endif
#endif
	else
		hPrint->fPrintConsole = NULL;

	// Create the unnamed mutex
	hPrint->hMutex = WcmMutex_Create();

	return hPrint;

	} /* Print_Create */

void Print_Destroy(PrintContext_t hPrint)
	{

	if (hPrint)
		{
		Print_CloseLogFile(hPrint);
		WcmFree(hPrint->bszTextBuffer);
		WcmMutex_Destroy(hPrint->hMutex);
		WcmFree(hPrint);
		}

	} /* Print_Destroy */

BOOL Print_OpenLogFile(PrintContext_t hPrint, STRING bszLogFileName, UINT32 u32LogMaxSize) 
	{

	UINT32 u32LogFileNameBufferChars;

	// Do nothing if a log file of the same name is already open
	if (hPrint->hLogFile != NULL && hPrint->bszLogFileName != NULL && 
		b_strcmp(bszLogFileName, hPrint->bszLogFileName) == 0)
		return TRUE;

	// Close the existing log file
	Print_CloseLogFile(hPrint);

	// Save the logfile name
	
	u32LogFileNameBufferChars = (UINT32) b_strlen(bszLogFileName) + 1;
	if ((hPrint->bszLogFileName = WcmMalloc(BCHARS2BYTES(u32LogFileNameBufferChars))) == NULL)
		return FALSE;
	b_strncpy(hPrint->bszLogFileName, bszLogFileName, u32LogFileNameBufferChars);

	// Create the new one
	WcmMutex_Wait(hPrint->hMutex);
	hPrint->hLogFile = LogFile_Create(bszLogFileName, S(""));
	if (hPrint->hLogFile == NULL)
		{
		WcmMutex_Release(hPrint->hMutex);
		Print_CloseLogFile(hPrint);
		return FALSE;
		}

	// Open the log file, if any
	LogFile_Open(hPrint->hLogFile, LOGFILE_TEXT_MODE, LOGFILE_READ_WRITE_MODE);
	/* set the log file size, and delete the old log file if present.*/
	hPrint->hLogFile->u32LogFileMaxSize = u32LogMaxSize *ONE_MEGA_BYTES; 
	hPrint->hLogFile->u32LogFileCurrentSize = 0;
	LogFile_deleteOldLog(hPrint->hLogFile->bszFileName);
	WcmMutex_Release(hPrint->hMutex);

	return TRUE; 

	} /* Print_OpenLogFile */

BOOL Print_CloseLogFile(PrintContext_t hPrint)
	{

	hLogFile_t hLF = hPrint->hLogFile;
	
	if (hPrint->hLogFile != NULL)
		{
		WcmMutex_Wait(hPrint->hMutex);
		hPrint->hLogFile = NULL;
		WcmMutex_Release(hPrint->hMutex);
		LogFile_Close(hLF);
		LogFile_Destroy(hLF);
		}

	if (hPrint->bszLogFileName != NULL)
		{
		WcmFree(hPrint->bszLogFileName);
		hPrint->bszLogFileName = NULL;
		}
			
	return TRUE;

	} /* Print_CloseLogFile */

// ==============================================================================
// START Thread Safe Section
// ==============================================================================

// Returns TRUE, if successful; FALSE, if print line is too long.
BOOL Print_Formatted(PrintContext_t hPrint, const STRING bszFormat, ...) {

	va_list args;

	va_start(args, bszFormat);

	return Print_Formatted_ArgPtr(hPrint, bszFormat, args);

	} /* Print_Formatted */

// Returns TRUE, if successful; FALSE, if print line is too long.
BOOL Print_Formatted_ArgPtr(PrintContext_t hPrint, const STRING bszFormat, va_list args) {

	B_CHAR bszBuffer[PRINT_CONTEXT_PRINTF_BUFFER_SIZE];
	UINT32 u32CharsPrinted;

	if (!hPrint)
		return FALSE;

	u32CharsPrinted = b_vsnprintf(bszBuffer, BCHARSOF(bszBuffer), bszFormat, args);

	Print_Append(hPrint, bszBuffer);

	return u32CharsPrinted < PRINT_CONTEXT_PRINTF_BUFFER_SIZE - 2;

	} /* Print_Formatted_ArgPtr */

void Print_Append (PrintContext_t hPrint, const STRING bszAddText) {

	UINT32 u32TextLength;

	if (!hPrint || !bszAddText)
		return;

	u32TextLength = (UINT32) b_strlen(bszAddText);

	if (u32TextLength > 0)
		{
		WcmMutex_Wait(hPrint->hMutex);
		if (Print_Reserve(hPrint, u32TextLength))
			{
			b_memcpy(hPrint->bszTextBuffer + hPrint->u32NextBufferChar, bszAddText, u32TextLength);
			hPrint->u32NextBufferChar += u32TextLength;
			hPrint->bszTextBuffer[hPrint->u32NextBufferChar] = 0;
			}
		WcmMutex_Release(hPrint->hMutex);
		}

	if (!hPrint->bBuffered)
		Print_Flush(hPrint);

	} /* Print_Append */

void Print_Flush(PrintContext_t hPrint) {

	hLogFile_t hLogFile;
	if (!hPrint)
		return;

	// Write to log file
	WcmMutex_Wait(hPrint->hMutex);
	hLogFile = hPrint->hLogFile;
	if (hLogFile != NULL && hPrint->bEnabled && !hLogFile->bReadOnlyMode)
		{
		if (hLogFile->bBinaryMode)
			LogFile_WriteBytes(hPrint->hLogFile, 
				(UINT8 *) hPrint->bszTextBuffer, BCHARS2BYTES(hPrint->u32NextBufferChar));
		else {
			LogFile_WriteText(hPrint->hLogFile, hPrint->bszTextBuffer);
			if (hPrint->bPrintConsole && hPrint->fPrintConsole != NULL) {
				b_fprintf(hPrint->fPrintConsole, B_SF, hPrint->bszTextBuffer);
				fflush(hPrint->fPrintConsole);
				}
			}
		LogFile_Flush(hPrint->hLogFile);
		
		hPrint->bszTextBuffer[0]  = 0;
		hPrint->u32NextBufferChar = 0;
		}
	WcmMutex_Release(hPrint->hMutex);

	// Clear the buffer
	Print_Clear(hPrint);

	} /* Print_Flush */

UINT32 Print_GetTextLength(PrintContext_t hPrint)
	{
	UINT32 u32TextLength;

	WcmMutex_Wait(hPrint->hMutex);
	u32TextLength = hPrint->u32NextBufferChar;
	WcmMutex_Release(hPrint->hMutex);

	return u32TextLength;

	} /* Print_GetTextLength */

// NOTE: This allocates a buffer that must be freed when no longer needed
STRING Print_GetText(PrintContext_t hPrint, UINT32 *pu32TextLength)
	{
	STRING bszText = NULL;
	UINT32 u32TextLength;

	if (!hPrint)
		return NULL;

	WcmMutex_Wait(hPrint->hMutex);
	u32TextLength = hPrint->u32NextBufferChar;
	WcmMutex_Release(hPrint->hMutex);

	if ((bszText = WcmMalloc(BCHARS2BYTES(u32TextLength + 1))) == NULL)
		return NULL;
	if (u32TextLength > 0)
		{
		WcmMutex_Wait(hPrint->hMutex);
		b_memcpy(bszText, hPrint->bszTextBuffer, (size_t) u32TextLength);
		WcmMutex_Release(hPrint->hMutex);
		}
	bszText[u32TextLength] = 0;

	if (pu32TextLength)
		*pu32TextLength = u32TextLength;

	return bszText;

	} /* Print_GetText */

void Print_Clear(PrintContext_t hPrint) {

	WcmMutex_Wait(hPrint->hMutex);
	if (hPrint->bEnabled) {
		hPrint->bszTextBuffer[0]  = 0;
		hPrint->u32NextBufferChar = 0;
		}
	WcmMutex_Release(hPrint->hMutex);

	} /* Print_Clear */

void Print_LabeledString(PrintContext_t hPrint, const STRING bszLabel, const STRING bszValue, BOOL bNewline) {

	Print_Append(hPrint, bszLabel);
	Print_Append(hPrint, bszValue);
	if (bNewline)
		Print_Newline(hPrint);

	} /* Print_LabeledString */

void Print_Error (PrintContext_t hPrint, const STRING bszErrMsg)
	{

	Print_Formatted(hPrint, S("ERROR: ") B_SFNL, bszErrMsg);

	} /* Print_Error */

void Print_Newline(PrintContext_t hPrint) {

	Print_Append(hPrint, S("\n"));

	} /* Print_Newline */

void Print_ByteArray(PrintContext_t hPrint, const STRING bszLabel, const UINT8 au8Data[], UINT32 u32Length, BOOL bNewline) {

	UINT32 n;
	B_CHAR bszBuffer[10];

	Print_Append (hPrint, bszLabel);

	for (n=0; n<u32Length; n++) {
		StrPrintf(bszBuffer, BCHARSOF(bszBuffer), S(" %02X"), au8Data[n]);
		Print_Append(hPrint, bszBuffer);
		}

	if (bNewline)
		Print_Newline(hPrint);

	} /* Print_ByteArray */

// Get the 'Enabled' attribute
BOOL Print_GetEnabled(PrintContext_t hPrint)
	{

	BOOL bRetVal;

	if (!hPrint)
		return FALSE;
	else
		{
		WcmMutex_Wait(hPrint->hMutex);
		bRetVal = hPrint->bEnabled;
		WcmMutex_Release(hPrint->hMutex);
		return bRetVal;
		}

	} /* Print_GetEnabled */

// Set the 'Enabled' attribute
void Print_SetEnabled(PrintContext_t hPrint, BOOL bEnabled)
	{

	if (!hPrint)
		return;
	else
		{
		WcmMutex_Wait(hPrint->hMutex);
		hPrint->bEnabled = bEnabled;
		WcmMutex_Release(hPrint->hMutex);
		}

	} /* Print_SetEnabled */

// Gets the 'Buffered' attribute
BOOL Print_GetBuffered(PrintContext_t hPrint)
	{

	BOOL bRetVal;
	if (!hPrint)
		return FALSE;
	else
		{
		WcmMutex_Wait(hPrint->hMutex);
		bRetVal = hPrint->bBuffered;
		WcmMutex_Release(hPrint->hMutex);
		return bRetVal;
		}

	} /* Print_GetBuffered */

// Sets the 'Buffered' attribute
void Print_SetBuffered(PrintContext_t hPrint, BOOL bBuffered)
	{

	if (!hPrint)
		return;
	else
		{
		WcmMutex_Wait(hPrint->hMutex);
		hPrint->bBuffered = bBuffered;
		WcmMutex_Release(hPrint->hMutex);
		}

	} /* Print_SetBuffered */

// ==============================================================================
// END Thread Safe Section
// ==============================================================================


// ==============================================================================
// Not thread safe: Must be protected by outer Print_Lock() and Print_Unlock() calls
// ==============================================================================

static BOOL Print_Reserve (PrintContext_t hPrint, UINT32 u32SizeNeeded)
	{

	UINT32 u32ReservedBufferSize = hPrint->u32NextBufferChar + u32SizeNeeded;
	if (u32ReservedBufferSize < hPrint->u32TextBufferSize)
		return TRUE;
	else 
		{
		UINT32 u32NewBufferSize = (u32ReservedBufferSize/hPrint->u32BufferBlockSize + 1)*hPrint->u32BufferBlockSize; 
		STRING bszNewBuffer;
		if ((bszNewBuffer = (STRING ) WcmMalloc(BCHARS2BYTES(u32NewBufferSize))) == NULL)
			return FALSE;
		b_memcpy(bszNewBuffer, hPrint->bszTextBuffer, hPrint->u32NextBufferChar);
		WcmFree(hPrint->bszTextBuffer);
		hPrint->bszTextBuffer = bszNewBuffer;
		hPrint->u32TextBufferSize = u32NewBufferSize;
		return TRUE;
		}

	} /* Print_Reserve */
 

#ifdef TEST_PRINT_CONTEXT_FUNCTIONS

#define test_wprintf b_printf

int main(void)
	{
	UINT32 u32TextLength;
	hLogFile_t      hLogFile;
	PrintContext_t hPrint;
	STRING			bszText;

	test_wprintf(S("\nPrint Context Test\n\n"));

	test_wprintf(S("Creating and opening log file\n"));
	hLogFile = LogFile_Create(S("print_test.log"), S("PrintContext test log file"));
	LogFile_Open(hLogFile, LOGFILE_TEXT_MODE, LOGFILE_READ_WRITE_MODE);

	test_wprintf(S("Creating print context\n"));
	hPrint   = Print_Create(2048, 1024);

	test_wprintf(S("Appending a few lines of text\n"));
	Print_SetBuffered(hPrint, TRUE);
	Print_Append(hPrint, S("This is line 1\n"));
	Print_Formatted(hPrint, S("This is line 2: %d %d %d\n"), 8, 9, 10);
	bszText = Print_GetText(hPrint, &u32TextLength);
	test_wprintf(bszText);
	WcmFree(bszText);
	test_wprintf(S("Text length was %d\n"), u32TextLength);
	Print_Flush(hPrint);
	Print_SetBuffered(hPrint, FALSE);

	test_wprintf(S("Destroying print context\n"));
	Print_Destroy(hPrint);

	test_wprintf(S("Closing and Destroying log file\n"));
	LogFile_Close(hLogFile);
	LogFile_Destroy(hLogFile);

	PressEnterToContinue();

	} /* main */


#endif

