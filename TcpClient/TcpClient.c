//
//  TcpClient.c
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 18/12/2020.
//

#include "TcpClient.h"
#include "TlsClient.h"

#define BUFF_MAX 100

#define SOCKET_NOT_INITIALIZED -1
#define SOCKET_CLOSED -2
#define SOCKET_UNEXPECTEDLY_CLOSED -3

TcpClient tcp_client = {.passcode = 0 , .passcode_token = 0};

// Callbacks
didDownloadFileFunPtrDef callback_did_download_file_funptr = NULL;

int sockfd = SOCKET_NOT_INITIALIZED;
struct sockaddr_in their_addr; /* connector's address information */
//uint32_t passcode = 0;
//uint32_t passcode_token = 0;
char app_data_path[256];

void tcp_client_send_string(int sockfd, const char* str);
//int tcp_client_receive_string(int sockfd, const char** buffer);

uint32_t should_reconnect(){
  return sockfd == SOCKET_UNEXPECTEDLY_CLOSED && tcp_client.passcode_token != 0;
}

int32_t reconnect_to_server(uint32_t code){
  if(should_reconnect() == 0) return -1;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
  }

  if (connect(sockfd, (struct sockaddr *)&their_addr, \
      sizeof(struct sockaddr)) == -1) {
      perror("connect");
      return 1;
  }
  int tls_result = tlsHanshake(sockfd);
  if(tls_result != 0){
    return -1;
  }
  char buffer[128];
  sprintf(buffer, "A{\"command\":\"reconnect\", \"code\":%d, \"code_token\":%d}", tcp_client.passcode, tcp_client.passcode_token);
  send_string_to_server(buffer);
  
  char *reply;
  if(receive_string_from_server(&reply)<0) return -1;
  int reconnect_reply = parseReonnectReply(reply);
  free(reply);
  if(reconnect_reply == 0){
    return 0;
  }
  return -1;
}

int32_t connect_to_server(const char *ip, int port, uint32_t *code){
  
//    struct sockaddr_in their_addr; /* connector's address information */
    struct hostent *he = gethostbyname(ip);
  
    // DNS error
    if(he == NULL){
      return -1;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(port);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

    if (connect(sockfd, (struct sockaddr *)&their_addr, \
        sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return 1;
    }
    int tls_result = tlsHanshake(sockfd);
    if(tls_result != 0){
      return -1;
    }
    char buffer[100];
    sprintf(buffer, "A{\"command\":\"connect\"}");
    send_string_to_server(buffer);
    
    char *recv_data;
    int recv_data_len = receive_string_from_server(&recv_data);
    if(recv_data_len <= 0){
      return -1;
    }
    // Make sure it's invokes correct parser
    parseTcpMessage(recv_data, &tcp_client);
    *code = tcp_client.passcode; // because the integer of code is at 8th position
    return 0;
}



int32_t receive_whole_data_from_server(uint8_t **data){
    char *recv_data;
    int32_t recv_data_len = receive_string_from_server((char**)&recv_data);
    if(recv_data_len <= 0){
        return -1;
    }
    *data = (uint8_t*)recv_data;
    return recv_data_len;
}

int send_json_to_server(cJSON *message_json){
//  cJSON *message_json = cJSON_CreateObject();
//  cJSON_AddItemToObject(message_json, "payload", payload);
////  cJSON_AddObjectToObject(json, "payload");
//  cJSON_AddItemToObject(message_json, "type", cJSON_CreateString("list-files"));
//  cJSON_AddItemToObject(message_json, "command", cJSON_CreateString("list-files"));
//  cJSON_AddItemToArray(dirjson, file);
//  var request = Request(type: "forward", command: "list-files", payload: fileNames);
  const char *json_string = cJSON_Print(message_json);
//  printf("json: %s", cJSON_Print(;));
  send_string_to_server(json_string);
  free((void*)json_string);
//  cJSON_Delete(message_json);
  return 0;
}

//cJSON_Delete(message_json);
//int32_t send_string_to_server(const char* str){
//    uint32_t len = (uint32_t)strlen(str);
//    send(sockfd, &len, sizeof(len), 0);
//    if (send(sockfd, str, len, 0) == -1){
//        perror("send_string_to_server error");
////        exit (1);
//        return -1;
//    }
//  return 0;
//}
int32_t send_string_to_server(const char* str){
    uint32_t len = (uint32_t)strlen(str);
  SSL_write(ssl, &len, sizeof(len));
//    send(sockfd, &len, sizeof(len), 0);
//  send(sockfd, str, len, 0) == -1
    if ( SSL_write(ssl, str, len) == -1){
        perror("send_string_to_server error");
//        exit (1);
        return -1;
    }
  return 0;
}



void tcp_client_send_string(int sockfd, const char* str){
    uint32_t len = (uint32_t)strlen(str);
    send(sockfd, &len, sizeof(len), 0);
    if (send(sockfd, str, len, 0) == -1){
        perror("send");
        exit (1);
    }
}

//int32_t receive_string_from_server(char** buffer){
//    int32_t msg_len = 0;
//    int32_t bytes_to_read = 0;
//    int recv_res = recv(sockfd, &msg_len, sizeof(uint32_t), 0);
//    if (recv_res == -1){
//        sockfd = SOCKET_UNEXPECTEDLY_CLOSED;
//        printf("Oh dear, something went wrong with recv()! %s\n", strerror(errno));
//        return -1;
//    }
//    if(recv_res == 0){
//      sockfd = 0;
//    }
//    bytes_to_read = msg_len;
//    char *new_buffer = malloc(msg_len);
//    *buffer = new_buffer;
//
//    while (bytes_to_read > 0) {
//        size_t numbytes = recv(sockfd, new_buffer, bytes_to_read, 0);
//        if (numbytes == -1){
//            return -1;
//        }
//        bytes_to_read = bytes_to_read - (uint32_t)numbytes;
//        new_buffer += numbytes;
//    }
//    return msg_len;
//}

int32_t receive_string_from_server(char** buffer){
    int32_t msg_len = 0;
    int32_t bytes_to_read = 0;

    int recv_res = SSL_read(ssl, &msg_len, sizeof(uint32_t));
//    int recv_res = recv(sockfd, &msg_len, sizeof(uint32_t), 0);
    if (recv_res == -1){
        sockfd = SOCKET_UNEXPECTEDLY_CLOSED;
        printf("Oh dear, something went wrong with recv()! %s\n", strerror(errno));
        return -1;
    }
    if(recv_res == 0){
      sockfd = 0;
    }
    bytes_to_read = msg_len;
    char *new_buffer = malloc(msg_len);
    if(new_buffer == NULL){
      return -1;
    }
    *buffer = new_buffer;
    
    while (bytes_to_read > 0) {
//        size_t numbytes = recv(sockfd, new_buffer, bytes_to_read, 0);
        size_t numbytes = SSL_read(ssl, new_buffer, bytes_to_read);
        if (numbytes == -1){
            return -1;
        }
        bytes_to_read = bytes_to_read - (uint32_t)numbytes;
        new_buffer += numbytes;
    }
    return msg_len;
}

//int32_t receive_whole_data_from_server(const uint8_t** buffer)
//  return receive_string_from_server(buffer);
//}

void create_demo_files(void){
  FILE *fp;
  char text[256];
  sprintf(text, "%s/%s", app_data_path, "Tutorial notes.txt");
  fp  = fopen (text, "w");
  fclose(fp);
  sprintf(text, "%s/%s", app_data_path, "Vacation Mexico.jpg");
  fp  = fopen (text, "w");
  fclose(fp);
  sprintf(text, "%s/%s", app_data_path, "Resume.pdf");
  fp  = fopen (text, "w");
  fclose(fp);
  
}

void setup_environment(const char* data_path){
  strcpy(app_data_path, data_path);
//  create_demo_files();
}


int runEndlessServer(void){
  char * message = NULL;
  while (1) {
    int ret = receive_string_from_server(&message);
    if(ret < 0) break;
    parseTcpMessage(message, &tcp_client);
  }
  return -1;
}
