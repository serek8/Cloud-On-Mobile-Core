//
//  TcpParser.h
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 25/11/2021.
//

#ifndef TcpParser_h
#define TcpParser_h

#include <stdio.h>
#include <string.h>
#include "TcpClient.h"
#include "cJSON.h"
#include "FileManager/FileManager.h"

//int parseTcpMessage(const char * const message);
int parseTcpMessage(const char * const message, TcpClient *tcp_client);
int parseReonnectReply(const char * const message);

#endif /* TcpParser_h */
