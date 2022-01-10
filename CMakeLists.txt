cmake_minimum_required(VERSION 3.10)

# set the project name
project(CloudOnMobileCoreClient)

include_directories(CloudOnMobileCoreClient 
    base64
    FileManager,
    TcpClient,
    "include"
    ".")

add_compile_options(-g)

set(SOURCE_FILES
    base64/buffer.c
    base64/encode.c
    base64/decode.c
    FileManager/FileManager.c
    TcpClient/TcpParser.c
    TcpClient/TcpClient.c
    TcpClient/TlsClient.c
    cJSON.c
    cJSON_Utils.c
    main.c)


add_executable(CloudOnMobileCoreClient ${SOURCE_FILES})

target_link_libraries(CloudOnMobileCoreClient
    pthread
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/linux/ssl/libssl.a
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/linux/crypto/libcrypto.a
    )
