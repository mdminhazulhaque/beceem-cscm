/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

/* ----------------------------------------------------------- */
/* Beceem RSA private key encrypt functions                    */
/* ----------------------------------------------------------- */

#include <string.h>
#include <openssl/err.h>

#include "e_beceem.h"
#include "e_beceem_err.h"
#include "e_beceem_rsa.h"

static const char *test_private_key_1024bits_PEM_def[] = {
	"-----BEGIN RSA PRIVATE KEY-----",
	"MIICXAIBAAKBgQC6GuaX4IYZskiIBb38PkBZElkVqHOp7kTqtEjrqCof8kruJDKh",
	"ngdrKGDRe4ZcWFkKdNwKeguFn1H8wER9/7yXBNzs60Dd39ywoYw6sv5E72z8f+O9",
	"6s+6JitA77bZFiolM2P3OcGnYoyfwwSDSnb/C4oAuj0QSXmQNHPvE1mkbwIDAQAB",
	"AoGBALHiWJq+/y68nFt3sZrTHcqFoST0eGnq13DHlhO6D1mtzDVH4CVe2ZRxaemR",
	"emH2CFXYUSKqeSzllgZS+KS+ttC5jVs6CR+mg/YlaZJHh98YiD0Ah3gE4yBnyRHY",
	"imXBQPDAKlzVBNgPBPwO+WEkTO3Z7Sy/wqNKDUmPzDZpQgxBAkEA213wOj3HnLdY",
	"8A3PwbzaK+/PEBRAEXDiTBhWA9MWIUh3Lwc9mtKb+SkbGgAtRFbMMmwDm6OWEF/V",
	"ApIwUDdR3wJBANku/6tH8wthUQSo3RaIr9G87Ild7X8n2NhkbssMQGMv9OiqhAWi",
	"XX98RWf9zhOafSY2gZtk2kQmDNfNqYWLn3ECQGJKjBo8llVVHeep+J4xdXxcXmYw",
	"/KgT8YHxIZFjIrZVFvoef28A0VxqG7ehvqaxUZuCJHe3a8T4e6RERpuavfcCQGo+",
	"b5weqmraxIN/CAjJ/RihTyUz6cbWe2IAX6K875KWFzzqjYMkFpES/fENXA6C+M4E",
	"xAp8n7nO02GOnmtSDkECQF+xypXf+8DhkpSIvBoN1CLJ4/tUY4y7JFbcqHu5J4iS",
	"+xM7EQMeDiHazsiKz3XbqpWDYw+NdsCWL60EjdfCdTE=",
	"-----END RSA PRIVATE KEY-----",
	NULL
	};
	
static const char *test_private_key_2048bits_PEM_def[] = {
	"-----BEGIN RSA PRIVATE KEY-----",
	"MIIEpAIBAAKCAQEAuYmFqK9Hb+6pNZRAkZiTIZMh3QMLUoFpxxfh4MoV781tjhiG",
	"dQeRxqON2LbmVt54Mc/VpdqQSGEhhdeT79MtJw9RszRdjek4bJpOtOvmG06WnqGk",
	"53hO/XeWHabPocQMsDQ4zGRosWXta8FGxwmT32CF1u+pzolbUEZQ08cjaZc/0aK7",
	"chPCVXzulj3bWXtfDlXumg1o2lCwg1aGoMcczmfXEFddoEyhiMM80MtsGz8L3gAG",
	"/yhHDMXj9kzR5tiKl9oZDurvKhyePlcwq9gwPcyvcv7YN+xGU9ahDJv0viokp48j",
	"wjVxCpEffK6ToDZir7Z4OwTYxHx/RZ1OaH2XZQIDAQABAoIBAQCHNE4wZLq8OMEa",
	"CFUdRIBDgUK2GhfQIfqBWUgDVYCy5ZKmb/zbn459oIYnnPEx8KDKO9hHKoTmGdIi",
	"BL0LkWpgAmvZGZnrZj719WTRtu5Si+x64PSYsTVuxAyiwwKfsYxFg2i4mZspW0uu",
	"sIPxW2tk5qPhw3iMx8/BKdyQ15dYw96U/iAdl6VGhGPxx665GR+PGlxCgrVNqTFN",
	"417V6woLZANM3r6OGYpBRuGhsB/Z/pblVDFvJH9xqaXp4cohBXxR11FaQ9TSob6x",
	"Nw1zWppzhWT74JLLTNMKDMNR7GlI+S3SzavyZyU6X5tuQF9wu/4mEfqPY/jDMGED",
	"SfJtTnIpAoGBAO5m+9hNOS20DGXThsWBwTVvAXRPYpiAOnwSil9haTk4fO5YGK4l",
	"aazaRN1a3gRKfV/f89kkAa+juiCrusnXJG506meRxjIinydedS4SPtgdM2URGOIk",
	"bK4MybpA+H+FZOiHD8nFNWYpfOXtAcWmGP0JcvZqRTh2B2XoFPyBl3q7AoGBAMc7",
	"kA6TOZJ8UMboXkssj6FGbGukeu+nh9FdW3FJ4mswjYyifgbLtaMtqB1PjymESIXh",
	"cxgK/2dx6raRfVRo8eLeuOoupppz1KNcQViOeHm/MPFvwHegQLSN2d/SxOvnROHW",
	"UBRxTamXXp3wp9pwdAFIIpdTFpIFL5h40oJBomRfAoGAGFfjt36LLCOKcItc46YQ",
	"OIiROXMxuii90BWEk6C6weEGwL+2JXIkjEXtajzxFrcUfkm7+Jw4n9q0Esn2VM3n",
	"JnBBzzmPZ2cpoTd/96hdSmwQ520LgWv+vPcJUj94IG1alHPWJOBCR6BM5J2C5M/F",
	"FXF1j7viPZWvw7QjEHWbJikCgYAwgZvtMIMP2xx/kQuEGfFOPqse+gjEWHGE08nz",
	"px1tI4ETAGzWkrpCZFdN65Tbcs2FawJulB7ivKV8laeB6ZsatUcoHPuLFzBqJYfz",
	"W5yRZ68u2RpvBaE7VF37VIGFPeVTZq5Qi9VwuN3cbj8vu7lCpqFlXmFcY87WvHPk",
	"Kr6y2QKBgQC80Iczmf4wur1rCagUClqmj1SEPZs3QqH3GzyZM18bAAtXXkxsS93W",
	"oGitdmAqgziipx+aRF+AHIcmzNIJcV3zj/Hbt8aOYHRb3TLfaJXJPhiEeqbKgygI",
	"k5p3qjnY5YFxW6TOILiglINP+oO9xPP704g97soPfgcDiPt93J5AFg==",
	"-----END RSA PRIVATE KEY-----",
	NULL
	};

/* RSA stuff */
#ifndef OPENSSL_NO_RSA
static int beceem_private_encrypt (int flen, const unsigned char *from, unsigned char *to,
			    RSA *rsa, int padding);
static int beceem_gets(BIO *bp, char *buf, int size);
// To avoid conflict with openssl/pem.h header file
#define PEM_read_bio PEM_read_bio_static
static int PEM_read_bio(BIO *bp, char **name, char **header, unsigned char **data,
				long *len);

/* Our internal RSA_METHOD that we provide pointers to */
static RSA_METHOD beceem_rsa_def =
	{
	"Beceem RSA method",
	NULL,
	NULL,
	beceem_private_encrypt, 
	NULL,
	NULL,
	NULL, 
	NULL,
	NULL, 
	RSA_METHOD_FLAG_NO_CHECK,	// Don't check public / private key match
	NULL, 
	NULL, 
	NULL, 
	NULL  
	};
#endif

static const RSA_METHOD *method_RSA_PKCS1_SSLeay = NULL;

void beceem_set_rsa(void)
	{

#ifndef OPENSSL_NO_RSA
	RSA_METHOD *pst_beceem_rsa_method;

	pst_beceem_rsa_method = &beceem_rsa_def;
	pvoid_st_beceem_rsa_method = (void *) pst_beceem_rsa_method;

	beceem_debug_printf("RSA default method = %p", RSA_get_default_method());
	beceem_debug_printf("Beceem RSA method  = %p", pst_beceem_rsa_method);

	beceem_debug_printf("Setting default method to beceem_rsa");
	RSA_set_default_method(pst_beceem_rsa_method);

	method_RSA_PKCS1_SSLeay = RSA_PKCS1_SSLeay();
	beceem_debug_printf("RSA PKCS1 SSLeay method = %p", method_RSA_PKCS1_SSLeay);

	if (method_RSA_PKCS1_SSLeay)
	{
		memcpy(pst_beceem_rsa_method, method_RSA_PKCS1_SSLeay, sizeof(beceem_rsa_def));
		pst_beceem_rsa_method->name          = "Beceem RSA method (overlaid on RSA_PKCS1_SSLeay)";
		pst_beceem_rsa_method->rsa_priv_enc  = beceem_private_encrypt;
		pst_beceem_rsa_method->flags         = RSA_METHOD_FLAG_NO_CHECK;
	}
#endif


	} // beceem_set_rsa

#ifndef OPENSSL_NO_RSA

/*
* Does what OpenSSL rsa_priv_enc does.
*/
static int beceem_private_encrypt(int flen, const unsigned char *from, unsigned char *to,
			    RSA *rsa, int padding)
{

	int padlen = 0, iLen=0;
	unsigned char *padbuf=NULL;
	char *sPadding;

	if (!private_key_encrypt_enabled)
		{
		beceem_debug_printf("beceem_private_encrypt: using RSA_PKCS1_SSLeay->rsa_priv_enc");
		return method_RSA_PKCS1_SSLeay->rsa_priv_enc(flen, from, to, rsa, padding);
		}

	beceem_debug_printf("^^^^^ Entering beceem_private_encrypt ^^^^^");
	beceem_debug_printf("==== cscm_rsa_private_encrypt = %p - cscm_context = %p", 
		cscm_rsa_private_encrypt, cscm_context);

	switch(padding)
		{
		case RSA_NO_PADDING:
			sPadding = "RSA_NO_PADDING";
			break;

		case RSA_PKCS1_PADDING:
			sPadding = "RSA_PKCS1_PADDING";
			break;

		default:
			sPadding = "ERROR: Unsupported padding mode";
			break;
		}

	beceem_debug_printf("beceem_private_encrypt: padding = %s", sPadding);
	beceem_debug_hexdump("beceem_private_encrypt: source' data", from, flen);

	padlen = BN_num_bytes(rsa->n);
	padbuf = OPENSSL_malloc(padlen);
	if(padbuf == NULL)
		{
		BECEEMerr(BECEEM_F_BECEEM_RSA_PRIV_ENC, ERR_R_MALLOC_FAILURE);
		return 0;
		}

	switch (padding)
		{
		case RSA_PKCS1_PADDING:
			iLen = RSA_padding_add_PKCS1_type_1(padbuf,padlen,from,flen);
			break;
		case RSA_NO_PADDING:
			iLen = RSA_padding_add_none(padbuf,padlen,from,flen);
			break;
		default:
			BECEEMerr(BECEEM_F_BECEEM_RSA_PRIV_ENC, BECEEM_R_UNKNOWN_PADDING_TYPE);
			iLen = 0;
			break;
		}

	if (iLen <= 0)
		beceem_debug_printf("beceem_private_encrypt: padding error = %d", iLen);
	else
		{
		beceem_debug_hexdump("beceem_private_encrypt: padded data", padbuf, padlen);
		if (cscm_rsa_private_encrypt != NULL && cscm_context != NULL)
			{
			beceem_debug_printf("beceem_private_encrypt: using cscm_rsa_private_encrypt");
			iLen = cscm_rsa_private_encrypt(cscm_context, padbuf, padlen, to, padlen);
			}
		else
			{
			beceem_debug_printf("beceem_private_encrypt: using RSA_PKCS1_SSLeay->rsa_priv_enc");
			iLen = method_RSA_PKCS1_SSLeay->rsa_priv_enc(padlen, padbuf, to, rsa, RSA_NO_PADDING);
			}

		if (iLen <= 0)
			beceem_debug_printf("beceem_private_encrypt: rsa_priv_enc returned error = %d", iLen);
		else
			beceem_debug_hexdump("beceem_private_encrypt: encrypted data", to, iLen);
		}

	if (padbuf != NULL)
		{
		OPENSSL_cleanse(padbuf, padlen);
		OPENSSL_free(padbuf);
		}

	beceem_debug_printf("^^^^^ Leaving beceem_private_encrypt ^^^^^");

	return iLen;

} // beceem_private_encrypt

EVP_PKEY *beceem_load_privkey(ENGINE* e, const char* key_id,
			UI_METHOD *ui_method, void *callback_data)
	{

	char *name = NULL;
	char *header = NULL;
	unsigned char *data = NULL;
	int i;
	long len = 0;
	EVP_PKEY *pkey;
	const RSA_METHOD *meth1;

	if (private_key_encrypt_enabled)
		{
		beceem_debug_printf("^^^^^ Entering beceem_load_privkey ^^^^^");
		beceem_debug_printf("==== cscm_rsa_private_encrypt = %p - cscm_context = %p", 
			cscm_rsa_private_encrypt, cscm_context);

		if (private_key_bits != 1024 && private_key_bits != 2048)
			{
			beceem_debug_printf("ERROR: Invalid private key bits = %d", private_key_bits);
			beceem_debug_printf("       Private key bits must be 1024 or 2048");
			beceem_debug_printf("       Using default value of 2048 bits");
			private_key_bits = 2048;
			}

		test_private_key_PEM  = private_key_bits == 1024 ? 
			test_private_key_1024bits_PEM_def : test_private_key_2048bits_PEM_def;

		beceem_debug_printf("beceem_load_privkey: Loading %d bit test private key ...", private_key_bits);

		for (i=0; test_private_key_PEM[i] != NULL; i++)
			beceem_debug_printf("%s", test_private_key_PEM[i]);
		beceem_debug_printf("\n");
		}

	beceem_gets(NULL, NULL, 0); // Zero the line pointer

	meth1 = RSA_get_default_method();
	beceem_debug_printf("RSA default method = %p - beceem method = %p", meth1, pvoid_st_beceem_rsa_method);

	if (!PEM_read_bio(NULL, &name, &header, &data, &len))
		return NULL;

	pkey = d2i_PrivateKey(EVP_PKEY_RSA, NULL, (const unsigned char **) &data, len);

	if (private_key_encrypt_enabled)
		{
		beceem_debug_printf("beceem_load_privkey: d2i_PrivateKey returned %p", pkey);
		beceem_debug_printf("pkey->pkey.rsa->meth = %p", pkey->pkey.rsa->meth);
		beceem_debug_printf("pkey->pkey.rsa->meth->flags = %d", pkey->pkey.rsa->meth->flags);

		beceem_debug_printf("^^^^^ Leaving beceem_load_privkey ^^^^^");
		}

	return pkey;

	} /* beceem_load_privkey */

// ======================================================================
// START PEM_read_bio - Copied from openssl 0.9.8g library verbatim
// ======================================================================

static int beceem_gets(BIO *bp, char *buf, int size)
	{

	static int iLine = 0;

	if (bp != NULL)
		return 0;

	if (buf == NULL)
		{
		iLine = 0;
		return 0;
		}

	if (test_private_key_PEM[iLine] == NULL)
		return 0;

	strncpy(buf, test_private_key_PEM[iLine++], size);
	buf[size-1] = 0;
	return (int) strlen(buf);

	} // beceem_gets

// This defines a local version of BIO_gets
#define BIO_gets beceem_gets
static int PEM_read_bio(BIO *bp, char **name, char **header, unsigned char **data,
	     long *len)
	{
	EVP_ENCODE_CTX ctx;
	int end=0,i,k,bl=0,hl=0,nohead=0;
	char buf[256];
	BUF_MEM *nameB;
	BUF_MEM *headerB;
	BUF_MEM *dataB,*tmpB;
	
	nameB=BUF_MEM_new();
	headerB=BUF_MEM_new();
	dataB=BUF_MEM_new();
	if ((nameB == NULL) || (headerB == NULL) || (dataB == NULL))
		{
		BUF_MEM_free(nameB);
		BUF_MEM_free(headerB);
		BUF_MEM_free(dataB);
		PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE);
		return(0);
		}

	buf[254]='\0';
	for (;;)
		{
		i=BIO_gets(bp,buf,254);

		if (i <= 0)
			{
			PEMerr(PEM_F_PEM_READ_BIO,PEM_R_NO_START_LINE);
			goto err;
			}

		while ((i >= 0) && (buf[i] <= ' ')) i--;
		buf[++i]='\n'; buf[++i]='\0';

		if (strncmp(buf,"-----BEGIN ",11) == 0)
			{
			i = (int) strlen(&(buf[11]));

			if (strncmp(&(buf[11+i-6]),"-----\n",6) != 0)
				continue;
			if (!BUF_MEM_grow(nameB,i+9))
				{
				PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE);
				goto err;
				}
			memcpy(nameB->data,&(buf[11]),i-6);
			nameB->data[i-6]='\0';
			break;
			}
		}
	hl=0;
	if (!BUF_MEM_grow(headerB,256))
		{ PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE); goto err; }
	headerB->data[0]='\0';
	for (;;)
		{
		i=BIO_gets(bp,buf,254);
		if (i <= 0) break;

		while ((i >= 0) && (buf[i] <= ' ')) i--;
		buf[++i]='\n'; buf[++i]='\0';

		if (buf[0] == '\n') break;
		if (!BUF_MEM_grow(headerB,hl+i+9))
			{ PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE); goto err; }
		if (strncmp(buf,"-----END ",9) == 0)
			{
			nohead=1;
			break;
			}
		memcpy(&(headerB->data[hl]),buf,i);
		headerB->data[hl+i]='\0';
		hl+=i;
		}

	bl=0;
	if (!BUF_MEM_grow(dataB,1024))
		{ PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE); goto err; }
	dataB->data[0]='\0';
	if (!nohead)
		{
		for (;;)
			{
			i=BIO_gets(bp,buf,254);
			if (i <= 0) break;

			while ((i >= 0) && (buf[i] <= ' ')) i--;
			buf[++i]='\n'; buf[++i]='\0';

			if (i != 65) end=1;
			if (strncmp(buf,"-----END ",9) == 0)
				break;
			if (i > 65) break;
			if (!BUF_MEM_grow_clean(dataB,i+bl+9))
				{
				PEMerr(PEM_F_PEM_READ_BIO,ERR_R_MALLOC_FAILURE);
				goto err;
				}
			memcpy(&(dataB->data[bl]),buf,i);
			dataB->data[bl+i]='\0';
			bl+=i;
			if (end)
				{
				buf[0]='\0';
				i=BIO_gets(bp,buf,254);
				if (i <= 0) break;

				while ((i >= 0) && (buf[i] <= ' ')) i--;
				buf[++i]='\n'; buf[++i]='\0';

				break;
				}
			}
		}
	else
		{
		tmpB=headerB;
		headerB=dataB;
		dataB=tmpB;
		bl=hl;
		}
	i = (int) strlen(nameB->data);
	if (	(strncmp(buf,"-----END ",9) != 0) ||
		(strncmp(nameB->data,&(buf[9]),i) != 0) ||
		(strncmp(&(buf[9+i]),"-----\n",6) != 0))
		{
		PEMerr(PEM_F_PEM_READ_BIO,PEM_R_BAD_END_LINE);
		goto err;
		}

	EVP_DecodeInit(&ctx);
	i=EVP_DecodeUpdate(&ctx,
		(unsigned char *)dataB->data,&bl,
		(unsigned char *)dataB->data,bl);
	if (i < 0)
		{
		PEMerr(PEM_F_PEM_READ_BIO,PEM_R_BAD_BASE64_DECODE);
		goto err;
		}
	i=EVP_DecodeFinal(&ctx,(unsigned char *)&(dataB->data[bl]),&k);
	if (i < 0)
		{
		PEMerr(PEM_F_PEM_READ_BIO,PEM_R_BAD_BASE64_DECODE);
		goto err;
		}
	bl+=k;

	if (bl == 0) goto err;
	*name=nameB->data;
	*header=headerB->data;
	*data=(unsigned char *)dataB->data;
	*len=bl;
	OPENSSL_free(nameB);
	OPENSSL_free(headerB);
	OPENSSL_free(dataB);
	return(1);
err:
	BUF_MEM_free(nameB);
	BUF_MEM_free(headerB);
	BUF_MEM_free(dataB);
	return(0);
	}

// =====================================================================
// END PEM_read_bio - Copied from openssl 0.9.8g library verbatim
// =====================================================================

#endif
