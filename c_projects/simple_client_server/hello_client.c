#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if(argc!=3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() err");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    // 실행 시 입력한 첫번째 인자(server ip 주소)로 Ip 지정.
    serv_addr.sin_port = htons(atoi(argv[2])); 
    // 실행 시 입력한 두번째 인자로 server port 지정.

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    // connect 메서드로 Server와 연결.
        error_handling("connect() err!");

    str_len = read(sock, message, sizeof(message) - 1);
    if(str_len == -1)
        error_handling("read() err!");

    printf("Message from server: %s \n", message);
    close(sock);
    return 0;
}

void error_handling(char * message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}

