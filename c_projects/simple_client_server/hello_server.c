#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
    int serv_sock; // 서버 소켓
    int clnt_sock; // client 소켓

    struct sockaddr_in serv_addr; 
    struct sockaddr_in clnt_addr;
    // sockaddr_in은 주소 체계가 ipv4인 AF_INET 인 경우의 소켓주소를 담는 구조체. 

    socklen_t clnt_addr_size; 
    // sys/socket.h 에 정의. socklen_t라는 type의 client_addr길이를 저장할 변수.

    char message[] = "Hello World!";

    if(argc != 2) // 추가 인자 하나(port)가 없으면 예외처리.
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // socket 생성. (PF_INET == ProtocolFamily Internet) 성공시 socket descriptor(file descriptor, fd)를 반환.
    if(serv_sock == -1)
        error_handling("Socket() error");

    //주소 초기화, IP Addr, Port 지정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET; // type: IPV4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // htol: Host to Network Long(Host system의 Byte Order에 맞게 data변환) ip address set to 사용가능한 랜카드의 IP주소 아무거나 지정.
    serv_addr.sin_port = htons(atoi(argv[1])); // 실행 시 입력했던 port 지정.

    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    // fd와 server 주소를 binding
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1) // queue len == 5인 대기열 생성.
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    // accept()로 client가 connect요청한 것을 받음. server socket에 client를 연결. 후 clnt_sock fd에 할당.

    if(clnt_sock == -1)
        error_handling("accept() error");

    write(clnt_sock, message, sizeof(message)); // client fd에 write (client에 메세지 보냄.)
    close(clnt_sock);
    close(serv_sock);
    // client, server socket 닫음.
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
