#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
    //서버로 보낸 데이터 바이트 수, 서버로 받은 데이터 바이트수 체크 변수 추가
	int str_len, recv_len, recv_cnt;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	
	while(1) 
	{
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;
        
        //클라이언트가 서버로 데이터를 전송. 반환값으로 전송한 데이터 크기를 받는다.
		str_len=write(sock, message, strlen(message));
		printf("str_len(서버로 보낸 전체 바이트수) : %d \n" , str_len);
        
        //클라이언트가 수신해야 할 데이터의 크기를 미리 알고 있다.
        //예를 들어 크기가 20바이트인 문자열을 전송했다면
        //20바이트를 수신할 때까지 반복해서 read함수를 호출하면 된다.
		recv_len=0;
		while(recv_len<str_len)
		{
            
            //이전 예제에서는 단순히 read 함수를 한 번 호출하고 말았던 것을
            //이 예제에서는 전송한 데이터의 크기만큼 데이터를 수신하기 위해서 read 함수를 반복 호출 하고 있다.
            //따라서 정확히 전송한 바이트 크기만큼 데이터를 수신할 수 있게 되었다.
            recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
            printf("recv_cnt(한번 읽은바이트수) : %d \n" , recv_cnt);
            
            if(recv_cnt==-1)
				error_handling("read() error!");
			
            //서버에서 보낸 데이터를 읽은 바이트 수만큼 더하기
            recv_len+=recv_cnt;
            printf("recv_len(누적 읽은바이트수) : %d \n" , recv_len);
        }
		
        // 단, 서버에서 널문자를 제외하고 보냄을 유의
        // 버에서 수신한 문자열 맨 뒤에 널문자 추가
		message[recv_len]=0;
		printf("Message from server: %s", message);
	}
	
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
