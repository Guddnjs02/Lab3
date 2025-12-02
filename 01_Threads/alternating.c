/* alternating.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define LOOP_COUNT 10

// 공유 자원 및 동기화 도구
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

// 이진 플래그 (Turn): 0 = 부모 차례, 1 = 자식 차례
int turn = 0; 

// 자식 쓰레드 함수
void *child_func(void *arg) {
    for (int i = 0; i < LOOP_COUNT; i++) {
        pthread_mutex_lock(&mutex); // 1. 락 획득

        // 2. 내 차례(1)가 아니면 대기
        // (while 루프를 쓰는 이유: 가짜 기상(Spurious Wakeup) 방지)
        while (turn != 1) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 3. 작업 수행
        printf("hello child  (%d)\n", i + 1);

        // 4. 차례 변경 (부모에게 넘김)
        turn = 0;

        // 5. 대기 중인 쓰레드 깨우기 & 락 해제
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tid;
    int i;

    // 자식 쓰레드 생성
    if (pthread_create(&tid, NULL, child_func, NULL) != 0) {
        perror("pthread_create failed");
        exit(1);
    }

    // 부모 쓰레드 작업 (메인)
    for (i = 0; i < LOOP_COUNT; i++) {
        pthread_mutex_lock(&mutex); // 1. 락 획득

        // 2. 내 차례(0)가 아니면 대기
        while (turn != 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 3. 작업 수행
        printf("hello parent (%d)\n", i + 1);

        // 4. 차례 변경 (자식에게 넘김)
        turn = 1;

        // 5. 대기 중인 쓰레드 깨우기 & 락 해제
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    // 자식 쓰레드 종료 대기
    pthread_join(tid, NULL);

    // 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
