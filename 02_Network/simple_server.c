/* simple_server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9000
#define BUF_SIZE 1024

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[BUF_SIZE];
    int str_len;

    // 1. 소켓 생성 (IPv4, TCP)
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    // 2. 주소 정보 초기화 (IP, PORT 설정)
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 내 컴퓨터의 모든 IP 허용
    serv_addr.sin_port = htons(PORT);

    // 3. 소켓에 주소 할당 (Bind)
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // 4. 연결 대기 상태 진입 (Listen)
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("Server is waiting on port %d...\n", PORT);

    // 5. 연결 수락 (Accept) - 클라이언트가 올 때까지 여기서 대기(Block)함
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept() error");
    
    printf("Client connected!\n");

    // 6. 데이터 송수신 (Echo)
    // 클라이언트가 보낸 데이터를 읽어서 그대로 다시 보냄
    while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) {
        write(clnt_sock, message, str_len);
    }

    // 7. 소켓 종료
    close(clnt_sock);
    close(serv_sock);
    printf("Connection closed.\n");
    
    return 0;
}
