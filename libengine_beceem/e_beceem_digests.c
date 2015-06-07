/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

/* ----------------------------------------------------------- */
/* Beceem SHA1 interception functions                          */
/* ----------------------------------------------------------- */

#include <string.h>

#include "e_beceem.h"
#include "e_beceem_digests.h"

static const int digest_nids[] = {NID_sha1};
static const int num_digest_nids = 1;

static const EVP_MD * openssl_EVP_sha1_p = NULL;
static EVP_MD beceem_sha1_md;

#define SHA1_MD_SIZE           20
#define MAC_SECRET_SIZE        20
#define MAC_SECRET_BUFFER_SIZE 64
#define SEQ_NO_SIZE             8
#define TLS_HEADER_SIZE         5

enum {STATE_IDLE, STATE_MAC_SECRET, STATE_SEQ_NO, STATE_APP_DATA, STATE_FOUND};
static UINT32 u32HmacState = STATE_IDLE;
static UINT8  au8MacSecret[MAC_SECRET_SIZE];
static UINT8  au8SeqNo[SEQ_NO_SIZE];

static int beceem_sha1_update(EVP_MD_CTX *ctx, const void *data, size_t count);

void beceem_set_digests(void)
	{
	openssl_EVP_sha1_p = EVP_sha1();
	memcpy(&beceem_sha1_md, openssl_EVP_sha1_p, sizeof(beceem_sha1_md));
	beceem_sha1_md.update = beceem_sha1_update;
	} // beceem_set_digests

// -----------------------------------------------------------------------
// Tells openssl that the Beceem engine has a SHA1 message digest
// -----------------------------------------------------------------------

int beceem_engine_digest (ENGINE *e, const EVP_MD **digest, const int **nids, int nid)
	{
	// Return list of digests
	if (digest == NULL)
		{
		if (nids != NULL)
			{
			*nids = digest_nids;
			return num_digest_nids;
			}
		else
			return -1;
		}

	// return digest
	else if (nid == NID_sha1)
		{
		*digest = &beceem_sha1_md;
		return 1;
		}

	// return error
	else
		{
		*digest = NULL;
		return 0;
		}

	// return 0 on error
	return 0;

	} /* beceem_engine_digests */

// -----------------------------------------------------------------------
// Extracts the MAC secret and sequence number from the SHA1 update calls
// -----------------------------------------------------------------------
static int beceem_sha1_update(EVP_MD_CTX *ctx, const void *data, size_t count)
	{

	int n;
	int bFound0x36, bFound0x5c;
	UINT8 u8Code;

	if (inner_nai_change_enabled)
		{
		if (count == MAC_SECRET_BUFFER_SIZE && 
			(u32HmacState == STATE_IDLE || u32HmacState == STATE_MAC_SECRET))
			{

			UINT8 au8Buffer[MAC_SECRET_BUFFER_SIZE];
			memcpy(au8Buffer, data, count);
			bFound0x36 = 1;
			bFound0x5c = 1;
			for (n=MAC_SECRET_SIZE; n<MAC_SECRET_BUFFER_SIZE; n++)
				{
				if (au8Buffer[n] != 0x36)
					bFound0x36 = 0;
				if (au8Buffer[n] != 0x5c)
					bFound0x5c = 0;
				}

			if (bFound0x36 || bFound0x5c)
				{
				u8Code = bFound0x36 ? 0x36 : 0x5c;
				for (n=0; n<MAC_SECRET_BUFFER_SIZE; n++)
					au8Buffer[n] ^= u8Code;
				beceem_debug_hexdump("MAC Secret", au8Buffer, MAC_SECRET_SIZE);
				memcpy(au8MacSecret, au8Buffer, MAC_SECRET_SIZE);
				u32HmacState = STATE_MAC_SECRET;
				}
			else
				{
				u32HmacState = STATE_IDLE;
				}
			}

		else if (count == SEQ_NO_SIZE && u32HmacState == STATE_MAC_SECRET)
			{
			beceem_debug_hexdump("Sequence number", data, SEQ_NO_SIZE);
			memcpy(au8SeqNo, data, SEQ_NO_SIZE);
			u32HmacState = STATE_SEQ_NO;
			}
		else if (count == TLS_HEADER_SIZE && u32HmacState == STATE_SEQ_NO)
			{
			UINT8 *au8Data = (UINT8 *) data;
			if (au8Data[0] == 0x17 &&  // TLS content type
				au8Data[1] == 0x03 &&  // TLS version
				au8Data[2] == 0x01 &&  // TLS version
				(au8Data[3] != 0 || au8Data[4] != 0)) // TLS record length
				{
				beceem_debug_hexdump("Application data TLS record header", au8Data, TLS_HEADER_SIZE);
				if (cscm_set_crypto_field != NULL)
					{
					cscm_set_crypto_field(ctx, ID_SHA1_HMAC_SECRET, au8MacSecret, MAC_SECRET_SIZE);
					cscm_set_crypto_field(ctx, ID_SEQUENCE_NUMBER,  au8SeqNo,     SEQ_NO_SIZE);
					OPENSSL_cleanse(au8MacSecret, MAC_SECRET_SIZE);
					OPENSSL_cleanse(au8SeqNo,     SEQ_NO_SIZE);
					}
				u32HmacState = STATE_IDLE;
				}
			else
				{
				u32HmacState = STATE_IDLE;
				}
			}
		else
			{
			u32HmacState = STATE_IDLE;
			}
		}

	return openssl_EVP_sha1_p->update(ctx, data, count);

	} // beceem_sha1_update
