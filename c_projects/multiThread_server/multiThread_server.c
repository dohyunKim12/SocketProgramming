#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *message);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
    int serv_sock; // 서버 소켓
    int clnt_sock; // client 소켓

    struct sockaddr_in serv_addr; 
    struct sockaddr_in clnt_addr;
    // sockaddr_in은 주소 체계가 ipv4인 AF_INET 인 경우의 소켓주소를 담는 구조체. 

    int clnt_addr_size;
    pthread_t t_id;

    if(argc != 2) // 추가 인자 하나(port)가 없으면 예외처리.
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);

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

    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        // accept()로 client가 connect요청한 것을 받음. server socket에 client를 연결. 후 clnt_sock fd에 할당.

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id); // detach는 thread가 종료되었을 때, 자원 회수를 위한 것.
        printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

void * handle_clnt(void * arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];
                
    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
        send_msg(msg, str_len);
                   
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)   // remove disconnected client
    {
        if(clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char * msg, int len)   // send to all
{
    int i;
    pthread_mutex_lock(&mutx);
    for(i=0; i<clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}
