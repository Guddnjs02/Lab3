/* hellothread.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep 함수 등을 위해 필요할 수 있음

// 쓰레드가 실행할 함수
void *hello_thread (void *arg)
{
    printf ("Thread: Hello, World!\n");
    return arg;
}

int main()
{
    pthread_t tid;
    int status;

    // 1. 쓰레드 생성
    // pthread_create(&쓰레드ID, 속성, 실행함수, 인자)
    status = pthread_create (&tid, NULL, hello_thread, NULL);
    
    if (status != 0) {
        perror ("Create thread");
        exit (1);
    }

    // 2. 메인 쓰레드 종료
    // pthread_exit을 사용하면 메인 쓰레드만 종료되고, 생성된 자식 쓰레드는 계속 실행될 수 있습니다.
    // (만약 exit(0)이나 return 0을 쓰면 프로세스 전체가 종료되어 자식 쓰레드도 강제 종료될 수 있음)
    pthread_exit (NULL);
}
