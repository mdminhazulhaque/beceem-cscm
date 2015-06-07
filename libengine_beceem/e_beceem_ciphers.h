/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

#ifndef E_BECEEM_CIPHERS_H
#define E_BECEEM_CIPHERS_H

/* ----------------------------------------------------------- */
/* Beceem AES 128 and 256 Key and IV interception functions    */
/* ----------------------------------------------------------- */

#include <openssl/engine.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

void beceem_set_ciphers(void);
int beceem_engine_cipher (ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid);

#endif
