#ifndef BECEEM_WIMAX_DYNAMIC_LOGGER
#define BECEEM_WIMAX_DYNAMIC_LOGGER

/********************************************************************/
/*						DYNAMIC LOGGING Subsystem                   */
/********************************************************************/

#define DYN_LOGGING_LOGFILENAME			"./bcm_sdk.log"
#define DYN_LOGGING_LOGFILE_MAXSIZE		2*1024*1024
#define SYSLOG_PREFIX_STR				"BECEEM_WiMAX_API"

enum {
	TM_FMT_RAW = 0,
	TM_FMT_COOKED
};
#ifndef WIN32
#define INLINE_LINKAGE inline
#else
#define INLINE_LINKAGE __forceinline
#endif

// TODO : ver for Win32 & MacOS
#define MAX_TIMESTAMP_BUFFER_SIZE	256


#define STD_OUTPUT NULL          /*stdout*/
#define STD_ERROR_OUTPUT NULL   /*stderr*/
#ifndef WIN32
#define GET_TIMESTAMP(format)			\
({										\
	time_t t;							\
	struct time *tm;					\
	UINT32 u32time_uSec=0;              \
	CHAR __tmstamp[MAX_TIMESTAMP_BUFFER_SIZE];				\
	struct timeval tv;					\
	memset (__tmstamp, 0, sizeof (__tmstamp));			\
	if (format == TM_FMT_COOKED) {		\
		t = time (NULL);					\
		tm = (struct time *)localtime (&t);				\
		strftime ((PVOID)__tmstamp, sizeof (__tmstamp), "%c", (struct tm *)tm);	\
	}	\
	else if (format == TM_FMT_RAW) {		\
		if (gettimeofday (&tv, 0) != -1){	\
            u32time_uSec=tv.tv_usec;        \
			snprintf ((PVOID)&__tmstamp, sizeof (__tmstamp), "%010ld.%06d", tv.tv_sec,u32time_uSec );	\
          } \
	}	\
	&__tmstamp;		\
})
#elif WIN32 
INLINE_LINKAGE VOID GET_TIMESTAMP(PUCHAR pTimeStampBuffer)
 {
        				
         /*get local time*/
		SYSTEMTIME sysTime;\
		GetLocalTime(&sysTime);
		sprintf_s ((CHAR *)pTimeStampBuffer, MAX_TIMESTAMP_BUFFER_SIZE, "%02d:%02d:%02d_%03d_%X ", sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond,
				sysTime.wMilliseconds,
				GetCurrentThreadId());	
             
} 
#endif


#define GETPID	GetCurrentProcID()


/* IMP! DEVELOPERS ensure your message fits within this max size; esp. applies
 * to dumping hex, etc..
 */
#define MAX_DPRINT_BUFFER_SIZE		2048

#ifdef WIN32
#define NEWLINE		"\r\n"
#else
#define NEWLINE		"\n"

#ifdef MACOS 
#define SYSLOG_LEVEL	LOG_NOTICE
#else
#define SYSLOG_LEVEL	LOG_INFO
#endif


#endif
/* Quick Print for quick 'n dirty debug :-)
 * Useful to do stuff like
 *  QP("\n");
 * in code where you just want to see if control has reached that point...
 */
//#ifdef DEBUG
//#define QP_ON	1		// make 0 to turn OFF
//#endif

#ifndef WIN32
#define QP(fmt_str, args...) do \
			{\
			if(STD_OUTPUT)\
				fprintf (STD_OUTPUT,"%s.%d: " fmt_str, __FUNCTION__, __LINE__, ##args);\
			}\
			while(0)
#elif WIN32

#define QP(fmt_str,...) do\
			{\
			if(STD_OUTPUT)\
				fprintf (STD_OUTPUT,"%s.%d: " fmt_str,__FUNCTION__, __LINE__, __VA_ARGS__);\
			}\
			while(0)

#endif

// for the truly lazy ;-)
#define DP	QP("\n")

#ifndef WIN32 
#define DPRINT(iCode, iPath, szFmt, args...)		\
({										\
	BOOL __bRet = FALSE;			\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	}\
	 else \
	{ \
		snprintf ((PVOID)pPrintBuffer, MAX_DPRINT_BUFFER_SIZE, "%s.%d: " szFmt, __FUNCTION__, __LINE__, ##args);		\
		strncat((PVOID)pPrintBuffer, NEWLINE, 3);			\
			__bRet = DebugPrint_Print (GTraceDebug, (UCHAR)iCode, iPath, (PVOID)pPrintBuffer);		\
		free (pPrintBuffer);	\
	}	\
	__bRet;		\
})
#endif
#ifdef WIN32

#define DPRINT(iCode, iPath, szFmt, ...)		\
    do \
        {\
										\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	} else	{ \
	    	sprintf_s (pPrintBuffer, MAX_DPRINT_BUFFER_SIZE, "%s.%d: " szFmt, __FUNCTION__, __LINE__, __VA_ARGS__);		\
    		strcat_s(pPrintBuffer, MAX_DPRINT_BUFFER_SIZE,NEWLINE);	\
            DebugPrint_Print(GTraceDebug, (UCHAR)iCode, iPath, (PVOID)pPrintBuffer);\
			free (pPrintBuffer);	\
	}	\
\
}\
while(0)
 
#endif

#ifndef WIN32 
#define DPRINT_HEX_DUMP(iCode, iPath, szFmt, args...)		\
({										\
	BOOL __bRet = FALSE;			\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	}\
	 else \
	{ \
		snprintf ((PVOID)pPrintBuffer, MAX_DPRINT_BUFFER_SIZE, " : " szFmt, ##args);		\
		strncat((PVOID)pPrintBuffer, NEWLINE, 3);			\
			__bRet = DebugPrint_Print (GTraceDebug, (UCHAR)iCode, iPath, (PVOID)pPrintBuffer);		\
		free (pPrintBuffer);	\
	}	\
	__bRet;		\
})
#endif
#ifdef WIN32

#define DPRINT_HEX_DUMP(iCode, iPath, szFmt, ...)		\
    do \
        {\
										\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	} else	{ \
	    	sprintf_s (pPrintBuffer, MAX_DPRINT_BUFFER_SIZE, " : " szFmt, __VA_ARGS__);		\
    		strcat_s(pPrintBuffer, MAX_DPRINT_BUFFER_SIZE,NEWLINE);	\
            DebugPrint_Print(GTraceDebug, (UCHAR)iCode, iPath, (PVOID)pPrintBuffer);\
			free (pPrintBuffer);	\
	}	\
\
}\
while(0)
 
#endif
// For error messages in DPRINT path, init & cleanup paths.. as we can't rely on DPRINT
// in these.
#ifndef WIN32
#define DPRINT_OTHER(fmt_str, args...)	\
({	\
	if(STD_ERROR_OUTPUT)\
		fprintf (STD_ERROR_OUTPUT, "%s.%d: " fmt_str, __FUNCTION__, __LINE__, ##args);	\
	syslog (SYSLOG_LEVEL, "%s.%d: " fmt_str, __FUNCTION__, __LINE__, ##args);	\
})
#else
#define DPRINT_OTHER(fmt_str, ...)	\
    do \
{	\
    PCHAR pPrintBuffer=NULL;\
	if(STD_ERROR_OUTPUT)\
	fprintf (STD_ERROR_OUTPUT, "%s.%d: " fmt_str, __FUNCTION__, __LINE__, __VA_ARGS__);	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
    if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	} else	{ \
		sprintf_s(pPrintBuffer,MAX_DPRINT_BUFFER_SIZE, "%s.%d: " fmt_str, __FUNCTION__, __LINE__,__VA_ARGS__);		\
		strcat_s((PVOID)pPrintBuffer, MAX_DPRINT_BUFFER_SIZE,NEWLINE);			\
    	OutputDebugString(pPrintBuffer);	\
    	free (pPrintBuffer);	\
}  \
        }\
    while(0)

#endif
#ifndef WIN32
#define DPRINT_COMMONAPI(iCode, szFmt, args...)		\
({										\
	BOOL __bRet = FALSE;			\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	} else	{ \
		snprintf ((PVOID)pPrintBuffer, MAX_DPRINT_BUFFER_SIZE, "%s.%d: " szFmt, __FUNCTION__, __LINE__, ##args);		\
		strncat((PVOID)pPrintBuffer, NEWLINE, 3);			\
		__bRet = DebugPrint_Print (GTraceDebug, (UCHAR)iCode, DEBUG_PATH_CAPI, (PVOID)pPrintBuffer);		\
		free (pPrintBuffer);	\
	}	\
	__bRet;		\
})
#else
#define DPRINT_COMMONAPI(iCode, szFmt, ...)		\
    do \
{										\
	PCHAR pPrintBuffer=NULL;			\
	\
	pPrintBuffer = (CHAR *)calloc (sizeof (CHAR), MAX_DPRINT_BUFFER_SIZE);		\
	if (!pPrintBuffer)	{ \
		printf ("DPRINT: Out of memory.\n");	\
	} else	{ \
	    sprintf_s(pPrintBuffer,MAX_DPRINT_BUFFER_SIZE,"%s.%d: " szFmt, __FUNCTION__, __LINE__, __VA_ARGS__);\
		strcat_s(pPrintBuffer,MAX_DPRINT_BUFFER_SIZE, NEWLINE);			\
		DebugPrint_Print(GTraceDebug, (UCHAR)iCode, DEBUG_PATH_CAPI, (PVOID)pPrintBuffer);\
		free (pPrintBuffer);	\
	}	\
}\
while(0)
#endif

#ifndef WIN32
#define SECURE_PRINT_FN(buffer,buffersize,fmt_spr, args...) \
    do \
        {\
    snprintf ((PVOID)buffer,buffersize,fmt_spr,##args);\
        }\
        while(0);

#else
#define SECURE_PRINT_FN(buffer,buffersize,fmt_spr, ...) \
    do \
        {\
    sprintf_s ((PVOID)buffer,buffersize,fmt_spr,__VA_ARGS__);\
        }\
        while(0);
        
#endif

#define HEX_DUMP(buffer,buffersize) \
	do \
	{\
	int numchunks =  buffersize/(MAX_DPRINT_BUFFER_SIZE -48);\
	PST_SHM_LOG_REC pstRec;\
	pstRec = (PST_SHM_LOG_REC) GetThisProcessShmLogRec(GpWiMAX);\
	if(pstRec)\
		{\
		if(pstRec->uDebugFlag & DEBUG_FINE)\
			{\
				if(numchunks == 0)\
				{\
				 HexDump(buffer,buffersize,(PCHAR)__FUNCTION__,__LINE__);\
				}\
				else \
				{ \
            		int i = 0; \
            		  for(i = 0; i <= numchunks; i++) \
            		   { \
							HexDump(((UCHAR*)buffer+(MAX_DPRINT_BUFFER_SIZE -48)*i),buffersize,(PCHAR)__FUNCTION__,__LINE__);\
						} \
				}\
			}\
		}\
	}\
	while(0);


/*--------------------------- Shared Memory etc for DYNAMIC LOGGING/DEBUG facility */
#ifndef WIN32
    #define DYN_LOGGING_SHM_KEY		0xbecee
#else
    #define DYN_LOGGING_SHM_KEY		"BECEEM_SHARED_LOGGING_DBG_DATA"
#endif 

typedef struct _ST_SHM_HDR {
	UINT				uNumProcessesAlive;
	INT32				iShmId; // require to save this in order to cleanup shm
} ST_SHM_HDR, *PST_SHM_HDR;


// This struct ST_SHM_LOG is only in shared memory & therefore exists truly globally
typedef struct _ST_SHM_LOG {
	ST_SHM_HDR			stShmHdr;			// header struct, does not repeat
	PST_SHM_LOG_REC		pstShmLogRec;		// ptr to start of the shmem array of log records 
											//  (repeats, one for each process alive)
} ST_SHM_LOG, *PST_SHM_LOG;

// This struct ST_LOGGING is embedded ito the BECEEMWIMAX struct & therefore exists per process
typedef struct _ST_LOGGING {
	PST_SHM_LOG			pstShmLog;			// ptr to start of the shmem
	UINT				nIndexPos;
	UINT				bInited;		// TRUE if shared memory exists 
	PMUTEX 				ptSemShmLock;	// named semaphore for concurrency protection on the shmem
	
} ST_LOGGING, *PST_LOGGING;

#endif

