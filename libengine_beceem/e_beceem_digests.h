/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2008-2009-2009 Beceem Communications, Inc.          */
/*                                                      */
/*	Carlos Puig (cpuig@beceem.com)                      */
/*	August 2009                                         */
/* ---------------------------------------------------- */

#ifndef E_BECEEM_DIGESTS_H
#define E_BECEEM_DIGESTS_H

/* ----------------------------------------------------------- */
/* Beceem SHA1 interception functions    */
/* ----------------------------------------------------------- */

#include <openssl/engine.h>
#include <openssl/pem.h>

void beceem_set_digests(void);
int beceem_engine_digest (ENGINE *e, const EVP_MD **digest, const int **nids, int nid);

#endif
