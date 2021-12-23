//
//  TlsClient.h
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 01/12/2021.
//

#ifndef TlsClient_h
#define TlsClient_h

#include <stdio.h>

#include <openssl/conf.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

int tlsHanshake(int fd);

extern SSL *ssl;

#endif /* TlsClient_h */
