/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

/* Written by Corinne Dive-Reclus(cdive@baltimore.com)
* 
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer. 
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*
* 3. All advertising materials mentioning features or use of this
*    software must display the following acknowledgment:
*    "This product includes software developed by the OpenSSL Project
*    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
*
* 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
*    endorse or promote products derived from this software without
*    prior written permission. For written permission, please contact
*    licensing@OpenSSL.org.
*
* 5. Products derived from this software may not be called "OpenSSL"
*    nor may "OpenSSL" appear in their names without prior written
*    permission of the OpenSSL Project.
*
* 6. Redistributions of any form whatsoever must retain the following
*    acknowledgment:
*    "This product includes software developed by the OpenSSL Project
*    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
*
* Written by Corinne Dive-Reclus(cdive@baltimore.com)
*
* Copyright@2001 Baltimore Technologies Ltd.
* All right Reserved.
*																								*	
*		THIS FILE IS PROVIDED BY BALTIMORE TECHNOLOGIES ``AS IS'' AND																			*
*		ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE					* 
*		IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE				*
*		ARE DISCLAIMED.  IN NO EVENT SHALL BALTIMORE TECHNOLOGIES BE LIABLE						*
*		FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL				*
*		DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS					*
*		OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)					*
*		HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT				*
*		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY				*
*		OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF					*
*		SUCH DAMAGE.																			*
====================================================================*/

#ifdef WIN32
#define DLL_EXPORT	__declspec ( dllexport )
#else
#define DLL_EXPORT
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef OPENSSL_NO_HW
#ifndef OPENSSL_NO_HW_BECEEM

#define extern
#include "e_beceem.h"
#undef extern

#include "e_beceem_rsa.h"
#include "e_beceem_err.h"
#include "e_beceem_digests.h"
#include "e_beceem_ciphers.h"

#define BECEEM_LIB_NAME "Beceem engine"
#define ONE_MEGA_BYTES  1048576

static int beceem_engine_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void));
static int beceem_engine_destroy(ENGINE *e);
static int beceem_engine_init(ENGINE *e);
static int beceem_engine_finish(ENGINE *e);

static BIO *logstream = NULL;
static BIO *debugstream = NULL;
static char beceem_ts_buffer[64];

#define DEBUG_LOG_FILE_NAME_MAXLEN 256
static char debug_log_file_name[DEBUG_LOG_FILE_NAME_MAXLEN+1] = {0};
static int logFileMaxSizeMB;
// #define DEFAULT_DEBUG
#ifdef DEFAULT_DEBUG
#define DEBUG_FILE_NAME "c:/Beceem/CSCM/engine_beceem.log"
#endif

#define DEBUG_SUCCESS  0
#define DEBUG_FAILURE  -1

//#define DEBUG_FILE_DUMP
static int beceem_debug_open(void);
static int beceem_debug_close(void);
#ifdef DEBUG_FILE_DUMP
static int beceem_debug_file_dump(const char * cFileName);
#endif

#define BECEEM_CMD_PIN			ENGINE_CMD_BASE

static const ENGINE_CMD_DEFN beceem_cmd_defns [ ] =
{
	{ BECEEM_CMD_PIN,
	  "PIN",
	  "Contains pointers for RSA_private_encrypt function and Beceem API structure",
	  ENGINE_CMD_FLAG_STRING},
	{ 0, NULL, NULL, 0 }
} ;

static const char *engine_beceem_id   = "opensc"; // Pretend to be the opensc engine
static const char *engine_beceem_name = "Beceem hardware engine support";

static void beceem_set_structures(void)
	{

	// AES 128 CBC and AES 256 CBC ciphers
	beceem_set_ciphers();

	// SHA1 digest
	beceem_set_digests();

	// RSA private key encrypt
	beceem_set_rsa();

	} // beceem_set_structures

/* ----------------------------------------------------------- */
/* Beceem engine implementation                                */
/* ----------------------------------------------------------- */

/* As this is only ever called once, there's no need for locking
 * (indeed - the lock will already be held by our caller!!!) */

static int bind_beceem(ENGINE *e)
{
	RSA_METHOD *pst_beceem_rsa_method;

	beceem_debug_printf("^^^^^ Entering bind_beceem ^^^^^");

	// Initialize externals
	cscm_set_crypto_field       = NULL;
	cscm_rsa_private_encrypt    = NULL;
	cscm_context                = NULL;
	private_key_encrypt_enabled = 0;
	inner_nai_change_enabled    = 0;
    debug_enabled               = 0;
	private_key_bits			= 2048;

	// Set various pointers to structures
	beceem_set_structures();
	pst_beceem_rsa_method = (RSA_METHOD *) pvoid_st_beceem_rsa_method;

	// Initilize
	if(!ENGINE_set_id(e, engine_beceem_id)     ||
	   !ENGINE_set_name(e, engine_beceem_name) ||
#ifndef OPENSSL_NO_RSA
	   !ENGINE_set_RSA(e, pst_beceem_rsa_method)         ||
#endif
	   !ENGINE_set_ciphers(e, beceem_engine_cipher) ||
	   !ENGINE_set_digests(e, beceem_engine_digest) ||
	   !ENGINE_set_destroy_function(e, beceem_engine_destroy) ||
	   !ENGINE_set_init_function(e, beceem_engine_init)       ||
	   !ENGINE_set_finish_function(e, beceem_engine_finish)   ||
	   !ENGINE_set_ctrl_function(e, beceem_engine_ctrl)       ||
	   !ENGINE_set_cmd_defns(e, beceem_cmd_defns)             ||
#ifndef OPENSSL_NO_RSA
	   !ENGINE_set_load_privkey_function(e, beceem_load_privkey) ||
#endif
	   !ENGINE_set_default_ciphers(e) ||
	   !ENGINE_set_default_digests(e) ||
	   0)
		{
		beceem_debug_printf("^^^^^ Leaving bind_beceem WITH ERROR EXIT ^^^^^");
		return 0;
		}

	/* Ensure the Beceem error handling is set up */
	ERR_load_BECEEM_strings();

	beceem_debug_printf("^^^^^ Leaving bind_beceem ^^^^^");

	return 1;

} // bind_beceem(ENGINE *e)

#ifndef OPENSSL_NO_DYNAMIC_ENGINE
static int bind_helper(ENGINE *e, const char *id)
	{
	beceem_debug_open();
	beceem_debug_printf("\n\n");
	beceem_debug_printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	beceem_debug_printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

	beceem_debug_printf("bind_helper - id = %s", id);

	if(id && (strcmp(id, engine_beceem_id) != 0))
		return 0;
	if(!bind_beceem(e))
		return 0;
	return 1;
	}       
IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(bind_helper)
#endif

static int threadsafe=1;
static int beceem_engine_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void))
{
	int  iRet = 1, iLen = 0;  // 1 = SUCCESS and 0 = FAILURE
	char *pin = (char *) p;
	char *sp0 = NULL, *sp1 = NULL;
	unsigned int u32Pointer0, u32Pointer1, u32Pointer2;

	beceem_debug_printf("^^^^^ Entering beceem_engine_ctrl ^^^^^");
	beceem_debug_printf("beceem_engine_ctrl: cmd = %d - i = %d - p = %p - f = %p", cmd, i, p, f);

	switch(cmd)
	{
		case ENGINE_CTRL_SET_LOGSTREAM:
		{
			BIO *bio = (BIO *)p;
			CRYPTO_w_lock(CRYPTO_LOCK_ENGINE);
			if (logstream)
			{
				BIO_free(logstream);
				logstream = NULL;
			}
			if (CRYPTO_add(&bio->references,1,CRYPTO_LOCK_BIO) > 1)
				logstream = bio;
			else
				BECEEMerr(BECEEM_F_BECEEM_CTRL, BECEEM_R_BIO_WAS_FREED);
		}
		CRYPTO_w_unlock(CRYPTO_LOCK_ENGINE);
		break;

	/* This will prevent the initialisation function from "installing"
	 * the mutex-handling callbacks, even if they are available from
	 * within the library (or were provided to the library from the
	 * calling application). This is to remove any baggage for
	 * applications not using multithreading. */
	case ENGINE_CTRL_CHIL_NO_LOCKING:
		CRYPTO_w_lock(CRYPTO_LOCK_ENGINE);
		threadsafe = 0;
		CRYPTO_w_unlock(CRYPTO_LOCK_ENGINE);
		break;

	case BECEEM_CMD_PIN:

		if (pin)
			beceem_debug_printf("beceem_engine_ctrl: pin = %s", pin);
		else
			beceem_debug_printf("beceem_engine_ctrl: ERROR - pin is NULL pointer");

		cscm_rsa_private_encrypt = NULL;
		cscm_set_crypto_field    = NULL;
		cscm_context = NULL;
		private_key_encrypt_enabled = 0;
		inner_nai_change_enabled = 0;
		debug_enabled = 0;
		private_key_bits = 2048;
		debug_log_file_name[0] = 0;

		if (pin == NULL || *pin == 0)
			iRet = 0;
		else if (sscanf(pin, "%x %x %x %d %d %d %d %d", &u32Pointer0, &u32Pointer1, &u32Pointer2, 
				&private_key_encrypt_enabled, &inner_nai_change_enabled, &debug_enabled, 
					&private_key_bits, &logFileMaxSizeMB) != 8)
			iRet = 0;
		else
			{
#ifdef WIN32
// ULongToPtr() is defined in basetsd.h
			cscm_rsa_private_encrypt = (PFN_CSCM_RSA_PRIVATE_ENCRYPT) ULongToPtr(u32Pointer0);
			cscm_set_crypto_field    = (PFN_CSCM_SET_CRYPTO_FIELD)    ULongToPtr(u32Pointer1);
			cscm_context             = (void *)                       ULongToPtr(u32Pointer2);
#else
			cscm_rsa_private_encrypt = (PFN_CSCM_RSA_PRIVATE_ENCRYPT) u32Pointer0;
			cscm_set_crypto_field    = (PFN_CSCM_SET_CRYPTO_FIELD)    u32Pointer1;
			cscm_context             = (void *)                       u32Pointer2;
#endif
			logFileMaxSizeMB *= ONE_MEGA_BYTES; //Convert to bytes.
			// Debug log file name is enclosed in apostrophes ('  ')
			sp0 = strchr(pin, '\'');
			if (sp0 != NULL)
				sp1 = strchr(sp0+1, '\'');
			if (sp0 != NULL && sp1 != NULL)
				{
				iLen = (int) (sp1 - sp0) - 1;
				if (iLen > DEBUG_LOG_FILE_NAME_MAXLEN)
					{
					iRet = 0;
					debug_log_file_name[0] = 0;
					}
				else
					{
					strncpy(debug_log_file_name, sp0+1, DEBUG_LOG_FILE_NAME_MAXLEN);
					debug_log_file_name[iLen] = 0;
					}
				}

			// Disable private key encryption via API
			if (!private_key_encrypt_enabled)
				cscm_rsa_private_encrypt = NULL;

			// Disable inner nai change
			if (!inner_nai_change_enabled)
				cscm_set_crypto_field = NULL;

			// Enable/disable debug
			if (debug_enabled)
				beceem_debug_open();
			else
				beceem_debug_close();

			// print options
			beceem_debug_printf("=============================================================");
			beceem_debug_printf("Beceem engine is beginning new TLS session");
			beceem_debug_printf("=============================================================");
			if (pin != NULL)
				{
				beceem_debug_printf("Options decoded from PIN:");
				beceem_debug_printf("PIN value received:         = %s", pin);
				beceem_debug_printf("cscm_rsa_private_encrypt    = %08X", u32Pointer0);
				beceem_debug_printf("cscm_set_crypto_field       = %08X", u32Pointer1);
				beceem_debug_printf("cscm_context                = %08X", u32Pointer2);
				beceem_debug_printf("private_key_encrypt_enabled = %d", private_key_encrypt_enabled);
				beceem_debug_printf("inner_nai_change_enabled    = %d", inner_nai_change_enabled);
				beceem_debug_printf("debug_enabled               = %d", debug_enabled);
				beceem_debug_printf("private_key_bits            = %d", private_key_bits);
				beceem_debug_printf("");
				}
			}

		break;

	/* The command isn't understood by this engine */
	default:
		BECEEMerr(BECEEM_F_BECEEM_CTRL,
			ENGINE_R_CTRL_COMMAND_NOT_IMPLEMENTED);
		iRet= 0;
		break;
		}
	
	beceem_debug_printf("^^^^^ Leaving beceem_engine_ctrl ^^^^^");

	return iRet;

} /* beceem_engine_ctrl */

/* Destructor (complements the "ENGINE_beceem_engine()" constructor) */
static int beceem_engine_destroy(ENGINE *e)
{
	beceem_debug_printf("^^^^^ Entering beceem_engine_destroy ^^^^^");
	ERR_unload_BECEEM_strings();
	beceem_debug_printf("^^^^^ Leaving beceem_engine_destroy ^^^^^");

	return 1;
}

/* (de)initialisation functions. */
static int beceem_engine_init(ENGINE *e)
{

	beceem_debug_printf("^^^^^ Entering beceem_engine_init ^^^^^");

	beceem_set_structures();

	beceem_debug_printf("^^^^^ Leaving beceem_engine_init ^^^^^");

	return 1;

} /* beceem_engine_init */

static int beceem_engine_finish(ENGINE *e)
{
	
	beceem_debug_printf("^^^^^ Entering beceem_engine_finish ^^^^^");

	if (logstream)
		BIO_free(logstream);

	beceem_debug_printf("^^^^^ Leaving beceem_engine_finish ^^^^^");

	// beceem_debug_close();

	return 1;

} /* beceem_engine_finish */

// ======================================================================
// Beceem debug logging functions
// ======================================================================

// Returns the current date and time
static char * beceem_ts(void)
	{
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(beceem_ts_buffer, sizeof(beceem_ts_buffer), "\n%x %X: ", timeinfo);

	return beceem_ts_buffer;

	} //beceem_ts

// Open the debug log file
static int beceem_debug_open(void)
	{

#ifdef DEFAULT_DEBUG
	if (!debugstream)
		debugstream = BIO_new_file(DEBUG_FILE_NAME, "a");
#else
	{
	int statRet;
	struct stat buffer;
	statRet = stat(debug_log_file_name, &buffer);
	if(statRet == 0 && logFileMaxSizeMB >0 && buffer.st_size >= logFileMaxSizeMB)
	{
		char backLogFileName[DEBUG_LOG_FILE_NAME_MAXLEN+5] = {0,};

		if (debugstream)
			BIO_free(debugstream);
		debugstream = NULL;
		strncpy(backLogFileName, debug_log_file_name, strlen(debug_log_file_name)+1);
#ifdef WIN32		
		strncat_s(backLogFileName, (sizeof(backLogFileName)/sizeof(char)), ".old", _TRUNCATE);
		DeleteFile(backLogFileName);
#else		
		strncat(backLogFileName, ".old", 4);
#endif
		rename(debug_log_file_name, backLogFileName);
	}

	}

	if (!debugstream && debug_enabled && debug_log_file_name[0])
		debugstream = BIO_new_file(debug_log_file_name, "a");
#endif

	if (!debugstream)
		return DEBUG_FAILURE;
	else
		return DEBUG_SUCCESS;

	} // beceem_debug_open(...)

// Close the debug log file
static int beceem_debug_close(void)
	{

	int iRetVal;

	if (debugstream)
		iRetVal = BIO_free(debugstream);
	else
		iRetVal = DEBUG_SUCCESS;
	debugstream = NULL;

	return iRetVal;

	} // beceem_debug_close(...)

// Print to the debug log file
int beceem_debug_printf(const char *format, ...)
	{

	va_list args;
	va_start(args, format);

	if (debugstream)
		{
		int i; // Dummy variable to avoid Linux compiler warning
		BIO_printf(debugstream, beceem_ts());
		BIO_vprintf(debugstream, format, args);
		i = BIO_flush(debugstream);
		}

	return DEBUG_SUCCESS;

	} // beceem_debug_printf(...)

// Dump a byte buffer to the debug log file
#define BYTES_PER_LINE 16
int beceem_debug_hexdump(const char * cLabel, const unsigned char * ucData, int iLength)
	{

	int n;

	if (!debugstream)
		return DEBUG_SUCCESS;

	beceem_debug_printf("Hex dump start: %s (%d bytes)", cLabel, iLength);

	for (n=0; n<iLength; n++)
		{
		if (n % BYTES_PER_LINE == 0)
			BIO_printf(debugstream, "\n%04X:", n);
		BIO_printf(debugstream, " %02X", ucData[n]);
		}

	beceem_debug_printf("Hex dump end: %s\n", cLabel);

	return DEBUG_SUCCESS;

	} // beceem_debug_hexdump

#ifdef DEBUG_FILE_DUMP
// Copy the contents of a file to the debug log file
static int beceem_debug_file_dump(const char * cFileName)
	{

	FILE *fp;
	char line[200];

	if (!debugstream)
		return DEBUG_SUCCESS;

	fp = fopen(cFileName, "r");
	if (fp == NULL)
		{
		beceem_debug_printf("ERROR: Can't open file %s", cFileName);
		return DEBUG_FAILURE;
		}

	beceem_debug_printf("Printing file: %s", cFileName);

	BIO_printf(debugstream, "\n++++++++++++++++++++++++++++++ Start Dump ++++++++++++++++++++++++++++++\n");
	while (fgets(line, sizeof(line), fp) != NULL)
		BIO_printf(debugstream, line);
	BIO_printf(debugstream, "++++++++++++++++++++++++++++++  End Dump  ++++++++++++++++++++++++++++++\n");

	fclose(fp);

	return DEBUG_SUCCESS;

	} // beceem_debug_hexdump
#endif

#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#endif

#endif /* !OPENSSL_NO_HW_Beceem */
#endif /* !OPENSSL_NO_HW */
