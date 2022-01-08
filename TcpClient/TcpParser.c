//
//  TcpParser.c
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 25/11/2021.
//

#include "TcpParser.h"


int parseListDir(cJSON *message_json){
  cJSON *path = cJSON_GetObjectItemCaseSensitive(message_json, "path");
  const char *filepath = NULL;
  cJSON *out_json = NULL;
  if (cJSON_IsString(path)){
    filepath = path->valuestring;
  }
  list_dir(filepath, &out_json);
  send_json_to_server(out_json);
  cJSON_Delete(out_json);
  return 0;
}

int parseDownloadFromDevice(cJSON *message_json){
  cJSON *path = cJSON_GetObjectItemCaseSensitive(message_json, "path");
  if (!(cJSON_IsString(path))){
    printf("error");
    return -1;
  }
  send_file(path->valuestring);
  return 0;
}

int parseUploadToDevice(cJSON *message_json){
//  cJSON *path =
//  cJSON_GetObjectItemCaseSensitive(message_json, "payload");
//  if (!(cJSON_IsString(path))){
//    printf("error");
//    return -1;
//  }
  cJSON *payload_json = cJSON_GetObjectItemCaseSensitive(message_json, "payload");
  cJSON *filepath = cJSON_GetObjectItemCaseSensitive(payload_json, "filepath");
  const char *filename = strdup(filepath->valuestring);
  
  save_file_from_json(message_json);
  
  callback_did_download_file_funptr(filename);
  
  cJSON *ack_json = cJSON_CreateObject();
  cJSON_AddItemToObject(ack_json, "filepath", cJSON_CreateString(filename));
  cJSON_AddItemToObject(ack_json, "command", cJSON_CreateString("upload"));
  cJSON_AddItemToObject(ack_json, "result", cJSON_CreateNumber(0));
  send_json_to_server(ack_json);
  cJSON_Delete(ack_json);
  return 0;
}

int parseRemoveFileFromDevice(cJSON *message_json){
  cJSON *path = cJSON_GetObjectItemCaseSensitive(message_json, "path");
  const char *filepath = NULL;
  cJSON *out_json = NULL;
  if (cJSON_IsString(path)){
    filepath = path->valuestring;
  }
  remove_file(filepath, &out_json);
  send_json_to_server(out_json);
  cJSON_Delete(out_json);
  return 0;
}

int parseConnectReply(cJSON *message_json, TcpClient *tcp_client){
  tcp_client->passcode = cJSON_GetNumberValue(cJSON_GetObjectItem(message_json, "code"));
  tcp_client->passcode_token = cJSON_GetNumberValue(cJSON_GetObjectItem(message_json, "code_token"));
  cJSON_free(message_json);
//  if (!(cJSON_IsString(path))){
//    printf("error");
//    return -1;
//  }
  
  return 0;
}

int parseReonnectReply(const char * const message){
  cJSON *message_json = cJSON_Parse(message);
  cJSON *command = cJSON_GetObjectItemCaseSensitive(message_json, "command");
  if (!(cJSON_IsString(command) && (command->valuestring != NULL))){
    printf("error");
    return -1;
  }
  if(strcmp(command->valuestring, "reconnect")){
    return -1;
  }
  double result = cJSON_GetNumberValue(cJSON_GetObjectItem(message_json, "result"));
  cJSON_free(message_json);
  if(result == 0){
    return 0;
  }
  if(result > 0){
    return result;
  }
  return -1;
}



int parseTcpMessage(const char * const message, TcpClient *tcp_client){
  cJSON *message_json = cJSON_Parse(message);
  cJSON *command = cJSON_GetObjectItemCaseSensitive(message_json, "command");
  if (!(cJSON_IsString(command) && (command->valuestring != NULL))){
    printf("error");
    return -1;
  }
  int retval = 0;
  printf("Parsing command:%s\n", command->valuestring);
//  cJSON *replyJson = cJSON_CreateObject();
  if(!strcmp(command->valuestring, "list-files")){
    retval=parseListDir(message_json);
  }
  else if(!strcmp(command->valuestring, "download")){
    retval=parseDownloadFromDevice(message_json);
  }
  else if(!strcmp(command->valuestring, "upload")){
    retval=parseUploadToDevice(message_json);
  }
  else if(!strcmp(command->valuestring, "connect")){
    retval=parseConnectReply(message_json, tcp_client);
  }
  else if(!strcmp(command->valuestring, "remove")){
    retval=parseRemoveFileFromDevice(message_json);
  }
//  else if(!strcmp(command->valuestring, "reconnect")){
//    retval=parseReonnectReply(message_json, tcp_client);
//  }
  return retval;
}
