/* chat_server.c */
/* Multi-Client Chat Server using select() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
#define MAX_CLNT 256
#define PORT 9090

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i, j;
    char buf[BUF_SIZE];

    // 1. 서버 소켓 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    // 2. 주소 할당 (Bind) & 대기 (Listen)
    // (SO_REUSEADDR 옵션을 주어 서버 재시작 시 Time-wait 에러 방지)
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // 3. select 설정을 위한 fd_set 초기화
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads); // 서버 소켓(연결 요청 감시용) 등록
    fd_max = serv_sock;

    printf("Chat Server Started on port %d...\n", PORT);

    while (1) {
        cpy_reads = reads; // 원본 보존을 위해 복사
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // 4. 변화 감지 (select)
        // fd_max+1 범위 내에서 이벤트가 발생한 소켓 개수 반환
        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break; // 오류
        
        if (fd_num == 0) continue; // 타임아웃 (변화 없음)

        // 5. 변화가 발생한 소켓 찾기
        for (i = 0; i < fd_max + 1; i++) {
            if (FD_ISSET(i, &cpy_reads)) { // i번 소켓에 변화가 있다면?
                
                // Case A: 서버 소켓에 변화가 있음 -> "새로운 연결 요청"
                if (i == serv_sock) {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    
                    FD_SET(clnt_sock, &reads); // 관찰 대상에 새 클라이언트 추가
                    if (fd_max < clnt_sock) fd_max = clnt_sock;
                    printf("Connected client: %d \n", clnt_sock);
                }
                // Case B: 클라이언트 소켓에 변화가 있음 -> "메시지 수신"
                else {
                    str_len = read(i, buf, BUF_SIZE);
                    
                    if (str_len == 0) { // 읽은 길이가 0이면 "연결 종료"
                        FD_CLR(i, &reads); // 관찰 대상에서 삭제
                        close(i);
                        printf("Closed client: %d \n", i);
                    } else {
                        // 받은 메시지를 모든 클라이언트에게 전송 (Broadcast)
                        // (자기 자신(i)과 서버 소켓(serv_sock)은 제외)
                        for (j = 0; j < fd_max + 1; j++) {
                            if (FD_ISSET(j, &reads) && j != serv_sock && j != i)
                                write(j, buf, str_len);
                        }
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
