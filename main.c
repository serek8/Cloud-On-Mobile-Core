#include <stdio.h>
#include "TcpClient/TcpClient.h"

int main(){
    printf("Hej");
    uint32_t code = 0;
    uint32_t a = connect_to_server("sd", 45, &code);
}
