//
//  TcpClient.h
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 18/12/2020.
//

#ifndef TcpClient_h
#define TcpClient_h

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "cJSON.h"

typedef struct {
  uint32_t passcode;
  uint32_t passcode_token;
} TcpClient;

#define KB (1024)
#define MB (1024*KB)
#define GB (1024*MB)

#include <TcpParser.h>

typedef void (*didDownloadFileFunPtrDef)(const char*);
extern didDownloadFileFunPtrDef callback_did_download_file_funptr;

int32_t connect_to_server(const char *ip, int port, uint32_t *code);
int runEndlessServer(void);
uint32_t should_reconnect(void);
int32_t reconnect_to_server(uint32_t code);
int send_json_to_server(cJSON *message_json);
int32_t receive_whole_data_from_server(uint8_t **data);
//int32_t receive_whole_data_from_server2(const uint8_t** buffer)

int32_t send_string_to_server(const char* str);
int receive_string_from_server(char** buffer);

void setup_environment(const char* data_path);

//void tcp_client_send_string(int sockfd, const char* str);

extern char app_data_path[256];


#endif /* TcpClient_h */
