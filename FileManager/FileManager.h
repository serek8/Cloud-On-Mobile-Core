//
//  FileManager.h
//  CloudOnMobile
//
//  Created by Cloud On Mobile Team on 24/11/2021.
//

#ifndef FileManager_h
#define FileManager_h

#include <stdio.h>
#include "cJSON.h"
#include "base64/b64.h"
#include "TcpClient/TcpClient.h"

int list_dir(const char* path, cJSON **out_json);
int send_file(const char* path);
int save_file_from_json(cJSON *message_json);
int list_dir_locally(const char* path, char **out);
#endif /* FileManager_h */
