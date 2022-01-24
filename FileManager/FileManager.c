//
//  FileManager.c
//  CloudOnMobile
//
//  Created by Cloud-On-Mobile Team on 24/11/2021.
//

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "FileManager.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifdef __APPLE__
#ifndef st_mtim
#define st_mtim st_mtimespec
#endif
#endif

int list_dir_add_file(cJSON *dirjson, struct stat *stat_entry, struct dirent *dir_entry){
  cJSON *file = cJSON_CreateObject();
  cJSON_AddItemToArray(dirjson, file);
  cJSON_AddItemToObject(file, "name", cJSON_CreateString(dir_entry->d_name));
  cJSON_AddItemToObject(file, "filename", cJSON_CreateString(dir_entry->d_name)); // todo: remove
  cJSON_AddItemToObject(file, "size", cJSON_CreateNumber(stat_entry->st_size));
  // cJSON_AddItemToObject(file, "date_epoch", cJSON_CreateNumber(stat_entry->st_mtimespec.tv_sec));
  cJSON_AddItemToObject(file, "date_epoch", cJSON_CreateNumber(stat_entry->st_mtim.tv_sec));
  cJSON_AddItemToObject(file, "dir", cJSON_CreateBool((dir_entry->d_type & DT_DIR) ? true : false));
  
  
//  const char *x = b64_encode(filename, strlen(filename));
//  cJSON_AddItemToObject(file, "base", cJSON_CreateString(x));
  return 0;
}

//int send_json_to_server(cJSON *json){
//  const char *listjson = cJSON_Print(json);
//  printf("json: %s", cJSON_Print(json));
//  send_string_to_server(json);
//  free((void*)listjson);
//  cJSON_Delete(json);
//  return 0;
//}

unsigned int add_slash_if_needed(char* path, int len){
  if(path[len-1] == '/'){
    return len;
  }
  path[len] = '/';
  path[++len] = 0;
  return len;
}

int list_dir(const char* dir_path, cJSON **out_json){
    if(dir_path == NULL){
      dir_path = strdup(app_data_path);
    }
    cJSON *dirjson = cJSON_CreateArray();
    DIR *dp = opendir(dir_path);
    if (dp == NULL){
        perror("opendir");
        return -1;
    }
    struct dirent *entry;
    struct stat file_stats;
    char file_path[FILENAME_MAX];
    strcpy(file_path, dir_path);
    unsigned long dir_path_len = strlen(dir_path);
  
    // Add slash
    dir_path_len = add_slash_if_needed(file_path, dir_path_len);
    
    while((entry = readdir(dp))) {
      // Exclude . and .. files
      if(entry->d_name[0] == '.' ||
         (entry->d_name[0] == '.' && entry->d_name[1] == '.')){
        continue;
      }
      
      // Check if the filame isn't too long
      if(strlen(entry->d_name) + dir_path_len > FILENAME_MAX-1){
        continue;
      }
      
      // Prepare the whole filepath
      strcpy(file_path+dir_path_len, entry->d_name);
      if (stat(file_path, &file_stats)){
        continue;
      }
      list_dir_add_file(dirjson, &file_stats, entry);
    }

    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddItemToObject(message_json, "payload", dirjson);
    cJSON_AddItemToObject(message_json, "type", cJSON_CreateString("forward"));
    cJSON_AddItemToObject(message_json, "command", cJSON_CreateString("list-files"));
    closedir(dp);
    *out_json = message_json;
    return 0;
}

int remove_file(const char* path, cJSON **out_json){
    char file_path[FILENAME_MAX];
    strcpy(file_path, app_data_path);
    unsigned long path_len = strlen(file_path);
  
    // Add slash
    path_len = add_slash_if_needed(file_path, (int)path_len);
    strcpy(file_path+path_len, path);
    
    int result = 0;
    if(remove(file_path) != 0){
      result = -1;
    }

    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddItemToObject(message_json, "type", cJSON_CreateString("forward"));
    cJSON_AddItemToObject(message_json, "command", cJSON_CreateString("remove"));
    cJSON_AddItemToObject(message_json, "result", cJSON_CreateNumber(result));
    if(result != 0){
      cJSON_AddItemToObject(message_json, "error_message", cJSON_CreateString("Can't remove file"));
    }
    *out_json = message_json;
    return 0;
}


int list_dir_locally(const char* path, char **out){
    if(path == NULL){
      path = strdup(app_data_path);
    }
    cJSON *json = NULL;
    list_dir(path, &json);
  
    cJSON *json_files_array = cJSON_GetObjectItem(json, "payload");
  
    char *strjson = cJSON_Print(json_files_array);
    *out = strjson;
    cJSON_free(json);
    return (int)strlen(strjson);
}

int is_directory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int save_file_from_json(cJSON *message_json){
  cJSON *payload_json = cJSON_GetObjectItemCaseSensitive(message_json, "payload");
  cJSON *bytes = cJSON_GetObjectItemCaseSensitive(payload_json, "bytes");
  cJSON *filepath = cJSON_GetObjectItemCaseSensitive(payload_json, "filepath");
  
//  if (!(cJSON_IsString(filepath) && (filepath->valuestring != NULL))){
//    printf("error");
//    return -1;
//  }
  char path_buffer[256];
  if(filepath->valuestring[0] != '/'){
    sprintf (path_buffer, "%s/%s", app_data_path, filepath->valuestring);
  }
  else{
    strcpy(path_buffer, filepath->valuestring);
  }
  int fd = open(path_buffer, O_WRONLY | O_CREAT | O_TRUNC, 0640);
  if(fd < 0){
    return -1;
  }
  size_t debase64_bytes_len = 0;
  unsigned char *debase64_bytes = b64_decode_ex(bytes->valuestring, strlen(bytes->valuestring), &debase64_bytes_len);
  
  write(fd, debase64_bytes, debase64_bytes_len);
  free(debase64_bytes);
  cJSON_Delete(message_json);
  close(fd);
  return -1;
}

int send_file(const char* path){
    char path_buffer[256];
    if(path[0] != '/'){
      sprintf (path_buffer, "%s/%s", app_data_path, path);
    }
    else{
      strcpy(path_buffer, path);
    }
    if(is_directory(path_buffer)){
      return -1;
    }
    const int buffer_size = 16 * MB;
    void *buffer = malloc(buffer_size);
    int fd = open(path_buffer, O_RDONLY);
    if(fd < 0){
      return -1;
    }
    size_t read_bytes = read(fd, buffer, buffer_size);
    if(read_bytes == -1){
      return -1;
    }
    const char *base64_bytes = b64_encode(buffer, read_bytes);
    const char *filename_only = strrchr(path_buffer, '/') +1;
  
    cJSON *payload_json = cJSON_CreateObject();
    cJSON_AddItemToObject(payload_json, "filename", cJSON_CreateString(filename_only));
    cJSON_AddItemToObject(payload_json, "bytes", cJSON_CreateStringReference(base64_bytes));
  
    cJSON *message_json = cJSON_CreateObject();
    cJSON_AddItemToObject(message_json, "payload", payload_json);
    cJSON_AddItemToObject(message_json, "type", cJSON_CreateString("forward"));
    cJSON_AddItemToObject(message_json, "command", cJSON_CreateString("download"));
    send_json_to_server(message_json);
    cJSON_Delete(message_json);
    return 0;
}

