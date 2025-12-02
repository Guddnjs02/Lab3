/* web_server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080       // 80번 포트는 루트 권한이 필요하므로 8080 사용
#define BUF_SIZE 4096

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

// 클라이언트 요청 처리 함수
void handle_request(int clnt_sock) {
    char buf[BUF_SIZE];
    char method[10];
    char file_name[100];
    char cgi_params[100];
    int read_len;

    // 1. 요청 읽기
    read_len = read(clnt_sock, buf, BUF_SIZE - 1);
    if (read_len == -1) return;
    buf[read_len] = 0;

    // 2. 요청 라인 파싱 (Method URL Protocol)
    // 예: "GET /index.html HTTP/1.1"
    sscanf(buf, "%s %s", method, file_name);

    printf("[Request] %s %s\n", method, file_name);

    // 3. CGI 프로그램 실행 요청인지 확인 (.cgi로 끝나는지)
    if (strstr(file_name, ".cgi") != NULL) {
        // HTTP 헤더 전송
        char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        write(clnt_sock, header, strlen(header));

        // CGI 실행 및 결과 전송 (popen 사용)
        // 실제로는 보안상 위험하지만 실습용으로 구현
        char command[100];
	sprintf(command, "./%s", file_name + 1);
	FILE *fp = popen(command, "r");
        if (fp != NULL) {
            while (fgets(buf, BUF_SIZE, fp) != NULL) {
                write(clnt_sock, buf, strlen(buf));
            }
            pclose(fp);
        } else {
            char *err = "<h1>CGI Execution Failed</h1>";
            write(clnt_sock, err, strlen(err));
        }
    }
    // 4. POST 요청 처리 (간단한 에코 기능)
    else if (strcmp(method, "POST") == 0) {
        char *body_start = strstr(buf, "\r\n\r\n");
        if (body_start != NULL) {
            body_start += 4; // 헤더 끝부분으로 이동
            
            char response[BUF_SIZE];
            sprintf(response, 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<h1>POST Method Received</h1>"
                "<p>Your Data: %s</p>", body_start);
            write(clnt_sock, response, strlen(response));
        }
    }
    // 5. GET 요청 처리 (정적 파일 전송)
    else if (strcmp(method, "GET") == 0) {
        if (strcmp(file_name, "/") == 0) // 루트 요청 시 index.html
            strcpy(file_name, "/index.html");

        char path[100] = "."; // 현재 디렉토리 기준
        strcat(path, file_name);

        int fd = open(path, O_RDONLY);
        if (fd != -1) {
            // 헤더 전송
            char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            write(clnt_sock, header, strlen(header));
            
            // 파일 내용 전송
            while ((read_len = read(fd, buf, BUF_SIZE)) > 0) {
                write(clnt_sock, buf, read_len);
            }
            close(fd);
        } else {
            // 404 Not Found
            char *header = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
                           "<h1>404 File Not Found</h1>";
            write(clnt_sock, header, strlen(header));
        }
    }
    
    close(clnt_sock);
}

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("Web Server started on port %d...\n", PORT);

    while (1) {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) continue;
        
        printf("Connected Request.\n");
        handle_request(clnt_sock);
    }
    return 0;
}
