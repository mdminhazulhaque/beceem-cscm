
/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

#ifndef E_BECEEM_RSA_H
#define E_BECEEM_RSA_H

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

void beceem_set_rsa(void);
EVP_PKEY *beceem_load_privkey(ENGINE* e, const char* key_id,
			UI_METHOD *ui_method, void *callback_data);

#endif

