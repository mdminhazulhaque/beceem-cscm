/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

/* ----------------------------------------------------------- */
/* Beceem AES 128 and 256 Key and IV interception functions    */
/* ----------------------------------------------------------- */

#include <string.h>

#include "e_beceem.h"
#include "e_beceem_ciphers.h"

static const int cipher_nids[] = {NID_aes_128_cbc, NID_aes_256_cbc};
static const int num_cipher_nids = 2;

static const EVP_CIPHER * openssl_EVP_aes_128_cbc_p = NULL;  // Points to original openssl cipher
static const EVP_CIPHER * openssl_EVP_aes_256_cbc_p = NULL;
static EVP_CIPHER beceem_aes_128_cbc_cipher;                 // Local copy with new init() and do_ciper() functions
static EVP_CIPHER beceem_aes_256_cbc_cipher;

static int beceem_aes_128_cbc_init(EVP_CIPHER_CTX *ctx, const unsigned char *key,
		    const unsigned char *iv, int enc);
static int beceem_aes_256_cbc_init(EVP_CIPHER_CTX *ctx, const unsigned char *key,
		    const unsigned char *iv, int enc);
static int beceem_aes_128_cbc_do_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
			 const unsigned char *in, unsigned int inl);
static int beceem_aes_256_cbc_do_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
			 const unsigned char *in, unsigned int inl);

#define AES_128_KEY_SIZE 16
#define AES_256_KEY_SIZE 32

/* ----------------------------------------------------------- */
/* Beceem AES 128 and 256 Key and IV interception functions    */
/* ----------------------------------------------------------- */

void beceem_set_ciphers(void)
	{

	openssl_EVP_aes_128_cbc_p = EVP_aes_128_cbc();
	memcpy(&beceem_aes_128_cbc_cipher, openssl_EVP_aes_128_cbc_p, sizeof(beceem_aes_128_cbc_cipher));
	beceem_aes_128_cbc_cipher.init      = beceem_aes_128_cbc_init;  
	beceem_aes_128_cbc_cipher.do_cipher = beceem_aes_128_cbc_do_cipher;

	openssl_EVP_aes_256_cbc_p = EVP_aes_256_cbc();
	memcpy(&beceem_aes_256_cbc_cipher, openssl_EVP_aes_256_cbc_p, sizeof(beceem_aes_256_cbc_cipher));
	beceem_aes_256_cbc_cipher.init      = beceem_aes_256_cbc_init;  
	beceem_aes_256_cbc_cipher.do_cipher = beceem_aes_256_cbc_do_cipher;

	} // beceem_set_ciphers

int beceem_engine_cipher (ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid)
	{

	beceem_debug_printf("beceem_engine_cipher: e=%p cipher=%p nids=%p nid=%d\n",
		e, cipher, nids, nid);

	// Return list of ciphers
	if (cipher == NULL)
		{
		if (nids != NULL)
			{
			*nids = cipher_nids;
			return num_cipher_nids;
			}
		else
			{
			*nids = NULL;
			return -1;
			}
		}

	// return cipher
	else if (nid == NID_aes_128_cbc)
		{
		*cipher = &beceem_aes_128_cbc_cipher;
		return 1;
		}

	// return cipher
	else if (nid == NID_aes_256_cbc)
		{
		*cipher = &beceem_aes_256_cbc_cipher;
		return 1;
		}

	// return error
	else
		{
		*cipher = NULL;
		return 0;
		}

	} /* beceem_engine_ciphers */

//
// Grab the AES 128 Key during init 
//
static int beceem_aes_128_cbc_init(EVP_CIPHER_CTX *ctx, const unsigned char *key,
		    const unsigned char *iv, int enc)
	{

	if (openssl_EVP_aes_128_cbc_p == NULL)
		return 0;
	
	if (ctx->encrypt && inner_nai_change_enabled)
		{	
		if (cscm_set_crypto_field != NULL)
			cscm_set_crypto_field(ctx, ID_AES_128_KEY, key, ctx->cipher->key_len);
		beceem_debug_printf("In beceem_aes_128_cbc_init: %s (Context = %p)", 
			ctx->encrypt ? "encrypt" : "decrypt", ctx);
		beceem_debug_hexdump("AES 128 Key", key, ctx->cipher->key_len);
		}

	return openssl_EVP_aes_128_cbc_p->init(ctx, key, iv, enc);

	} /* beceem_aes_128_cbc_init */

// 
// Grab the AES 256 key during init 
//
static int beceem_aes_256_cbc_init(EVP_CIPHER_CTX *ctx, const unsigned char *key,
		    const unsigned char *iv, int enc)
	{

	if (openssl_EVP_aes_256_cbc_p == NULL)
		return 0;
	
	if (ctx->encrypt && inner_nai_change_enabled)
		{
		if (cscm_set_crypto_field != NULL)
			cscm_set_crypto_field(ctx, ID_AES_256_KEY, key, ctx->cipher->key_len);
		beceem_debug_printf("In beceem_aes_256_cbc_init: %s (Context = %p)", 
			ctx->encrypt ? "encrypt" : "decrypt", ctx);
		beceem_debug_hexdump("AES 256 Key", key, ctx->cipher->key_len);
		}

	return openssl_EVP_aes_256_cbc_p->init(ctx, key, iv, enc);

	} /* beceem_aes_256_cbc_init */

// 
// Grab the current AES 128 working IV during encryption
//
static int beceem_aes_128_cbc_do_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
			 const unsigned char *in, unsigned int inl)
	{
	int ret;

	if (openssl_EVP_aes_128_cbc_p == NULL)
		return 0;

	if (ctx->encrypt && inner_nai_change_enabled)
		{
		if (cscm_set_crypto_field != NULL)
			cscm_set_crypto_field(ctx, ID_AES_128_IV, ctx->iv, ctx->cipher->iv_len);
		beceem_debug_printf("In beceem_aes_128_cbc_do_cipher: %s (Context = %p)", 
			ctx->encrypt ? "encrypt" : "decrypt", ctx);
		beceem_debug_hexdump("AES 128 working IV", ctx->iv, ctx->cipher->iv_len);
		beceem_debug_hexdump("Input data:\n", in, inl);
		}

	ret = openssl_EVP_aes_128_cbc_p->do_cipher(ctx, out, in, inl);

	if (ctx->encrypt && inner_nai_change_enabled)
		{
		beceem_debug_hexdump("Output data:\n", out, inl);
		}

	return ret;

	} /* beceem_aes_128_cbc_do_cipher */

// 
// Grab the current AES 2568 working IV during encryption
//
static int beceem_aes_256_cbc_do_cipher(EVP_CIPHER_CTX *ctx, unsigned char *out,
			 const unsigned char *in, unsigned int inl)
	{
	int ret;

	if (openssl_EVP_aes_256_cbc_p == NULL)
		return 0;

	if (ctx->encrypt && inner_nai_change_enabled)
		{
		if (cscm_set_crypto_field != NULL)
			cscm_set_crypto_field(ctx, ID_AES_256_IV, ctx->iv, ctx->cipher->iv_len);
		beceem_debug_printf("In beceem_aes_256_cbc_do_cipher: %s (Context = %p)\n", 
			ctx->encrypt ? "encrypt" : "decrypt", ctx);
		beceem_debug_hexdump("AES 256 working IV", ctx->iv, ctx->cipher->iv_len);
		beceem_debug_hexdump("Input data:\n", in, inl);
		}

	ret = openssl_EVP_aes_256_cbc_p->do_cipher(ctx, out, in, inl);

	if (ctx->encrypt && inner_nai_change_enabled)
		{
		beceem_debug_hexdump("Output data:\n", out, inl);
		}

	return ret;

	} /* beceem_aes_256_cbc_do_cipher */
