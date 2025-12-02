/* bounded_buffer.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5      // 버퍼 크기 (제한 버퍼)
#define NUM_PRODUCERS 2    // 생산자 수 (2명 이상)
#define NUM_CONSUMERS 2    // 소비자 수 (2명 이상)
#define MAX_ITEMS 10       // 각 생산자가 만들 아이템 수

// 공유 버퍼 및 동기화 변수 구조체
typedef struct {
    int item[BUFFER_SIZE];  // 원형 버퍼
    int in;                 // 데이터 넣을 인덱스
    int out;                // 데이터 꺼낼 인덱스
    int count;              // 현재 버퍼에 있는 아이템 개수
    
    pthread_mutex_t mutex;      // 상호 배제를 위한 뮤텍스
    pthread_cond_t not_full;    // 버퍼가 가득 차지 않았음을 알리는 조건변수
    pthread_cond_t not_empty;   // 버퍼가 비어있지 않음을 알리는 조건변수
} buffer_t;

// 전역 버퍼 초기화 (PTHREAD_..._INITIALIZER 매크로 사용)
buffer_t bb = { 
    .in = 0, .out = 0, .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER
};

// 아이템 생산 (임의의 데이터 생성)
int produce_item(int id) {
    int item = rand() % 100; // 0~99 사이 난수
    sleep(rand() % 2);       // 잠시 대기 (생산 시간 시뮬레이션)
    printf("[Producer %d] Produced: %d\n", id, item);
    return item;
}

// 아이템 소비 (데이터 처리)
void consume_item(int id, int item) {
    sleep(rand() % 3);       // 잠시 대기 (소비 시간 시뮬레이션)
    printf("\t\t[Consumer %d] Consumed: %d\n", id, item);
}

// 버퍼에 아이템 삽입 (생산자 호출)
void insert_item(int item) {
    pthread_mutex_lock(&bb.mutex); // 1. 락 획득

    // 2. 버퍼가 꽉 찼으면 빈 공간이 생길 때까지 대기 (not_full)
    while (bb.count >= BUFFER_SIZE) {
        printf("   (Buffer FULL. Producer waiting...)\n");
        pthread_cond_wait(&bb.not_full, &bb.mutex);
    }

    // 3. 아이템 삽입
    bb.item[bb.in] = item;
    bb.in = (bb.in + 1) % BUFFER_SIZE;
    bb.count++;
    
    // 4. 대기 중인 소비자에게 신호 전송 (not_empty)
    pthread_cond_signal(&bb.not_empty);
    
    pthread_mutex_unlock(&bb.mutex); // 5. 락 해제
}

// 버퍼에서 아이템 제거 (소비자 호출)
int remove_item() {
    int item;

    pthread_mutex_lock(&bb.mutex); // 1. 락 획득

    // 2. 버퍼가 비었으면 아이템이 들어올 때까지 대기 (not_empty)
    while (bb.count <= 0) {
        printf("   (Buffer EMPTY. Consumer waiting...)\n");
        pthread_cond_wait(&bb.not_empty, &bb.mutex);
    }

    // 3. 아이템 꺼내기
    item = bb.item[bb.out];
    bb.out = (bb.out + 1) % BUFFER_SIZE;
    bb.count--;

    // 4. 대기 중인 생산자에게 신호 전송 (not_full)
    pthread_cond_signal(&bb.not_full);

    pthread_mutex_unlock(&bb.mutex); // 5. 락 해제
    
    return item;
}

// 생산자 쓰레드 함수
void *producer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < MAX_ITEMS; i++) {
        int item = produce_item(id);
        insert_item(item);
    }
    printf("[Producer %d] Done.\n", id);
    return NULL;
}

// 소비자 쓰레드 함수
void *consumer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < MAX_ITEMS; i++) {
        int item = remove_item();
        consume_item(id, item);
    }
    printf("\t\t[Consumer %d] Done.\n", id);
    return NULL;
}

int main() {
    pthread_t prod[NUM_PRODUCERS];
    pthread_t cons[NUM_CONSUMERS];
    int p_ids[NUM_PRODUCERS];
    int c_ids[NUM_CONSUMERS];
    int i;

    srand(time(NULL)); // 난수 초기화

    printf("--- Bounded Buffer Problem (P:%d, C:%d) ---\n", NUM_PRODUCERS, NUM_CONSUMERS);

    // 생산자 쓰레드 생성
    for (i = 0; i < NUM_PRODUCERS; i++) {
        p_ids[i] = i + 1;
        if (pthread_create(&prod[i], NULL, producer, &p_ids[i]) != 0) {
            perror("Producer create failed");
            exit(1);
        }
    }

    // 소비자 쓰레드 생성
    for (i = 0; i < NUM_CONSUMERS; i++) {
        c_ids[i] = i + 1;
        if (pthread_create(&cons[i], NULL, consumer, &c_ids[i]) != 0) {
            perror("Consumer create failed");
            exit(1);
        }
    }

    // 모든 쓰레드가 종료될 때까지 대기
    for (i = 0; i < NUM_PRODUCERS; i++) pthread_join(prod[i], NULL);
    for (i = 0; i < NUM_CONSUMERS; i++) pthread_join(cons[i], NULL);

    printf("--- All threads finished. ---\n");
    return 0;
}
