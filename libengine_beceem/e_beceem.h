/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

#ifndef E_BECEEM_H
#define E_BECEEM_H

typedef unsigned int  UINT32;
typedef unsigned char UINT8;
#undef NULL
#ifndef NULL
#define NULL ((void *) 0)
#endif

int beceem_debug_printf(const char *format, ...);
int beceem_debug_hexdump(const char * cLabel, const unsigned char * ucData, int iLength);

// Values are shared with the connection manager
enum {ID_AES_128_KEY, ID_AES_256_KEY, ID_AES_128_IV, ID_AES_256_IV, 
      ID_SHA1_HMAC_SECRET, ID_SEQUENCE_NUMBER, ID_LAST_CRYPTO};

typedef int (*PFN_CSCM_RSA_PRIVATE_ENCRYPT) 
	(void *context, 
	 const unsigned char *au8RSAMsgIn,  unsigned int u32RSAMsgInLen,
	 unsigned char *au8RSAMsgOut, unsigned int u32EAPMsgOutMaxLen);
extern PFN_CSCM_RSA_PRIVATE_ENCRYPT cscm_rsa_private_encrypt;

typedef int (*PFN_CSCM_SET_CRYPTO_FIELD)
	(void *context,
	 unsigned int  uiFieldID,
	 const unsigned char *aucFieldData,
	 unsigned int  uiFieldDataLen);
extern PFN_CSCM_SET_CRYPTO_FIELD  cscm_set_crypto_field;

extern const char **test_private_key_PEM;
extern int private_key_encrypt_enabled;
extern int inner_nai_change_enabled;
extern int debug_enabled;
extern int private_key_bits;
extern void *cscm_context;
extern void *pvoid_st_beceem_rsa_method;

#endif
