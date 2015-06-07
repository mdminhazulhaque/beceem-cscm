/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	July 2008                                           */
/* ---------------------------------------------------- */

#ifndef PRINT_CONTEXT_H
#define PRINT_CONTEXT_H

#include <stdarg.h>
#include <stdio.h>
#include "CscmTypes.h"
#include "LogFile.h"
#include "ThreadFunctions.h"

#define PRINT_CONTEXT_PRINTF_BUFFER_SIZE  2048

typedef struct {

	STRING bszTextBuffer;
	UINT32 u32NextBufferChar;  // In units of B_CHAR
	UINT32 u32TextBufferSize;  // In units of B_CHAR
	UINT32 u32BufferBlockSize; // In units of B_CHAR

	BOOL bEnabled;
	BOOL bBuffered;
	BOOL bPrintConsole;
	FILE *fPrintConsole;
	hLogFile_t hLogFile;
	STRING bszLogFileName;
	WcmMutex_t hMutex;

	} stPrintContext_t;

typedef stPrintContext_t *PrintContext_t;

PrintContext_t Print_Create(BOOL bPrintConsole, UINT32 u32InitBufferSize, UINT32 u32BufferBlockSize);
void Print_Destroy(PrintContext_t hPrint);
                                                                                                                                             
BOOL Print_Formatted(PrintContext_t hPrint, const STRING bszFormat, ...);
BOOL Print_Formatted_ArgPtr(PrintContext_t hPrint, const STRING bszFormat, va_list args);
void Print_Append (PrintContext_t hPrint, const STRING bszText);
void Print_Flush (PrintContext_t hPrint);         
UINT32 Print_GetTextLength(PrintContext_t hPrint);
STRING Print_GetText(PrintContext_t hPrint, UINT32 *pu32TextLength);
void Print_Clear (PrintContext_t hPrint);
void Print_LabeledString(PrintContext_t hPrint, 
				const STRING bszLabel, const STRING bszValue, BOOL bNewline);
void Print_Error(PrintContext_t hPrint, const STRING bszErrorMsg);
void Print_Newline(PrintContext_t hPrint);
void Print_ByteArray(PrintContext_t hPrint, const STRING bszLabel, 
				const UINT8 bArray[], UINT32 u32Length, BOOL bNewline);
BOOL Print_GetEnabled(PrintContext_t hPrint);
void Print_SetEnabled(PrintContext_t hPrint, BOOL bEnabled);
BOOL Print_GetBuffered(PrintContext_t hPrint);
void Print_SetBuffered(PrintContext_t hPrint, BOOL bBuffered);

BOOL Print_OpenLogFile(PrintContext_t hPrint, STRING bszLogFileName, UINT32 u32LogMaxSize);
BOOL Print_CloseLogFile(PrintContext_t hPrint);

#endif

