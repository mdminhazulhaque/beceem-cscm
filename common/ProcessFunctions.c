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

// NOTE: This module is for use only on Linux operating systems

// Define this for standalone self-test
// #define TEST_PROCESS_FUNCTIONS

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TEST_PROCESS_FUNCTIONS
#include <errno.h>
#include <signal.h>
#endif

#include "ProcessFunctions.h"

static INT32 ConvertCmdLine(const char *szCmdLine, char *szBuffer, UINT32 u32SizBuffer);

#ifdef TEST_PROCESS_FUNCTIONS
static INT32 PrintCmdLine(const char *szBuffer, INT32 i32BufLen);
#endif

// --------------------------------------------------------------
// Given a command line, find all matching processes
//
// Input parameters:
//	   szCmdLine - Command line for which PIDs will be found
//	   u32SizPid - Maximum allocated size of au32Pid array
//
// Output parametes:
//     au32Pid   - Array of PID values
//
// Returns number of matching Pid's found or -1, on error
// --------------------------------------------------------------

#define MAX_PID_STRING 50
#define MAX_CMD_LINE   512

INT32 FindPid(char *szCmdLine, pid_t *aPid, UINT32 u32SizPid)
	{

	DIR *dirp;
	struct dirent *dp;
	char *sp;
	char szPidString[MAX_PID_STRING];
	char szUserCmdLine[MAX_CMD_LINE];
	char szProcCmdLine[MAX_CMD_LINE];
	BOOL bNumeric;
	INT32  i32ProcCmdLen = 0;
	INT32  i32UserCmdLen = 0;
	UINT32 u32NumPid = 0;
	FILE *fp;
#ifdef TEST_PROCESS_FUNCTIONS
	int n;
#endif

	// Convert the command line
	i32UserCmdLen = ConvertCmdLine(szCmdLine, szUserCmdLine, MAX_CMD_LINE);
#ifdef TEST_PROCESS_FUNCTIONS
	if (i32UserCmdLen < 0)
		printf("ERROR parsing command line \'%s\'\n", szCmdLine);
	else if (i32UserCmdLen == 0)
		printf("WARNING: empty command line \'%s\'\n", szCmdLine);
	else
		PrintCmdLine(szUserCmdLine, i32UserCmdLen);
#endif
	if (i32UserCmdLen <= 0)
		return -1;

	// Open the /proc directory
	if ((dirp = opendir("/proc")) == NULL)
		{
#ifdef TEST_PROCESS_FUNCTIONS
		printf("Can't open /proc\n");
#endif
		return -1;
		}

	// Search for matching PID entries
	while ((dp = readdir(dirp)) != NULL && dp->d_name != NULL)
		{
		// Process only numeric directory names
		bNumeric = TRUE;
		for (sp=dp->d_name; *sp != 0 && bNumeric; sp++)
			{	
			if (!isdigit(*sp))
				bNumeric = FALSE;
			}

		// Extract the command line for each PID
		// Compare it to the user command line
		// and build a list of matching PIDs
		if (bNumeric)
			{
			// Construct the file name
			strncpy(szPidString, "/proc/",   MAX_PID_STRING);
			strncat(szPidString, dp->d_name, MAX_PID_STRING);
			strncat(szPidString, "/cmdline", MAX_PID_STRING);
			// Open and read the command line string from each file
			if ((fp = fopen(szPidString, "r")) != NULL)
				{
				i32ProcCmdLen = fread(szProcCmdLine, 1, MAX_CMD_LINE, fp);
				fclose(fp);
				if (i32ProcCmdLen == i32UserCmdLen &&
					!memcmp(szProcCmdLine, szUserCmdLine, i32UserCmdLen))
					{
					if (u32NumPid < u32SizPid)
						{
						aPid[u32NumPid] = (pid_t) atoi(dp->d_name);
#ifdef TEST_PROCESS_FUNCTIONS
						printf("%d: ", (UINT32) aPid[u32NumPid]);
						PrintCmdLine(szProcCmdLine, i32ProcCmdLen);	
#endif
						u32NumPid++;		
						}
					}
				} // if ((fp = fopen 
			} // if (bNumeric
		} // while ((dp = readdir

	closedir(dirp);

	return u32NumPid;

} // FindPid 
	
// Returns:
//     Length of converted result string
//     -1, on error
//
static INT32 ConvertCmdLine(const char *szCmdLine, char *szBuffer, UINT32 u32SizBuffer)
	{

	char *saveptr = NULL;
	char *szLocalCopy, *szArg, *szLine;
	UINT32 u32NumChars = 0, u32ArgLen = 0;
	UINT32 u32CmdLineLen = 0;

	// Create a local copy of the command line	
	u32CmdLineLen = strlen(szCmdLine);
	if (u32CmdLineLen > 1024)
		return -1;
	if ((szLocalCopy = malloc(u32CmdLineLen + 1)) == NULL)
		return -1;
	strcpy(szLocalCopy, szCmdLine);
	szLine = szLocalCopy;

	// Parse and copy the args	
	do {
		szArg = strtok_r(szLine, "\t ", &saveptr);
		szLine = NULL;
		if (szArg != NULL)
			{
			u32ArgLen = strlen(szArg) + 1; // Includes NUL at the end
			if (u32ArgLen > 1 && u32NumChars + u32ArgLen < u32SizBuffer)
				{
				strcpy(&szBuffer[u32NumChars], szArg);
				u32NumChars += u32ArgLen;
				}
			else
				{
				free(szLocalCopy);
				return -1;
				}
			}
		} while (szArg != NULL);

	free(szLocalCopy);

	return u32NumChars;

	} // ConvertCmdLine

#ifdef TEST_PROCESS_FUNCTIONS
// Print the command line, as returned by /proc/<pid>/cmdline
static INT32 PrintCmdLine(const char *szBuffer, INT32 i32BufLen)
{
	char c;
	INT32 n;

	for (n=0; n<i32BufLen; n++)
		{
		c = szBuffer[n];
		if (c >= '!' && c <= '~')
			printf("%c", c);
		else
			printf("<%02X>", c);
		}
	printf("\n");

} // PrintCmdLine

// test program
#define MAX_PID_LIST 20
int main (int argc, char *argv[])
{
	char * szCmdLine = "dhclient eth1";
	INT32 i32CmdLen;
	pid_t aPid[MAX_PID_LIST];
	UINT32 u32NumPid;
	char c;
	int n, iRet;

	// Execute command
	printf("Executing \'%s\'\n", szCmdLine);
	system(szCmdLine);
	
	// Find the pid's for this command line
	u32NumPid = FindPid(szCmdLine, aPid, MAX_PID_LIST);
	printf("Found %u matching PIDs\n", u32NumPid);

	// List pid's
	for (n=0; n<u32NumPid; n++)
		{
		printf("Killing %u ", aPid[n]);
		errno = 0;
		iRet = kill(aPid[n], SIGTERM);
		printf("(ret = %d) ", iRet);
		if (iRet != 0)
			{
			if (errno == EPERM)
				printf(" ERROR: No permission\n");
			else if (errno == ESRCH)
				printf(" ERROR: No such process\n");
			else 
				printf(" ERROR: Other code = %d\n");
			}
		else
			printf("OK \n");
		}

	return 0;	
} // main

#endif
