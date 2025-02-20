#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_threads.h"

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t {
    sem_t queue;
    sem_t rw_lock;
    sem_t mutex;
    int reader_count;
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    Sem_init(&rw->queue, 1);
    Sem_init(&rw->rw_lock, 1);
    Sem_init(&rw->mutex, 1);
    rw->reader_count = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    Sem_wait(&rw->queue);      // 进入全局队列
    Sem_wait(&rw->mutex);
    if (rw->reader_count == 0) {
        Sem_wait(&rw->rw_lock); // 第一个读者获取写锁
    }
    rw->reader_count++;
    Sem_post(&rw->mutex);
    Sem_post(&rw->queue);       // 释放队列，允许后续线程排队
}

void rwlock_release_readlock(rwlock_t *rw) {
    Sem_wait(&rw->mutex);
    rw->reader_count--;
    if (rw->reader_count == 0) {
        Sem_post(&rw->rw_lock);
    }
    Sem_post(&rw->mutex);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    Sem_wait(&rw->queue);
    Sem_wait(&rw->rw_lock);
    Sem_post(&rw->queue);
}

void rwlock_release_writelock(rwlock_t *rw) {
     Sem_post(&rw->rw_lock);
}

//
// Don't change the code below (just use it!)
// 

int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_readlock(&lock);
	printf("read %d\n", value);
	rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
	rwlock_acquire_writelock(&lock);
	value++;
	printf("write %d\n", value);
	rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
	Pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
	Pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
	Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
	Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}

