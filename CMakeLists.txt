cmake_minimum_required(VERSION 3.10)

# set the project name
project(CloudOnMobileCoreClient)

include_directories(CloudOnMobileCoreClient 
    TcpClient,
    "include"
    ".")

set(SOURCE_FILES
    TcpClient/TcpParser.c
    TcpClient/TcpClient.c
    TcpClient/TlsClient.c
    cJSON.c
    cJSON_Utils.c
    main.c)


add_executable(CloudOnMobileCoreClient ${SOURCE_FILES})

target_link_libraries(CloudOnMobileCoreClient
    ../boringssl/build/ssl/libssl.a)
