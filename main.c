#include <stdio.h>
#include "TcpClient/TcpClient.h"
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc < 3){
        printf("Please provide IP and port\n");
        printf("E.g. ./CloudOnMobileCoreClient cloudon.cc 9293\n");
        return 0;
    }
    printf("Bootstraping");
    uint32_t code = 0;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Can'tget Current working dir");
    }

    char path[256];
    sprintf(path, "%s/%s", cwd, "hosted_files");
    struct stat st = {0};
    int res = 0;
    if (stat(path, &st) == -1) {
        res = mkdir(path, 0777);
    }

    printf("Current working dir: %s\n", cwd);
    setup_environment(path);
    uint32_t connection = connect_to_server(argv[1], atoi(argv[2]), &code);
    if(connection != 0){
        printf("Can't connect to the server. Exiting...\n");
        return 0;
    }
    printf("Running with code: %d\n", code);
    runEndlessServer();
    printf("Exiting");

}
