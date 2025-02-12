#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//
/*typedef struct __ns_mutex_t {
    sem_t mutex;
    sem_t queue;
    int current_idx;
    int next_idx;    
    int wait_count;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    Sem_init(&(m->mutex), 1);
    Sem_init(&(m->queue), 0);
    m->current_idx = 0;
    m->next_idx = 0;
}

void ns_mutex_acquire(ns_mutex_t *m) {
    Sem_wait(&(m->mutex));
    int idx = m->next_idx++;
    int not_mine = 0;
    printf("worker[%lu]: self_idx: %d, next_idx: %d\n", pthread_self(), idx, m->next_idx);
    Sem_post(&(m->mutex));
    while (idx != m->current_idx) {
        printf("worker[%lu]: wait\n", pthread_self());
        if (not_mine) {
            not_mine = 0;
            sched_yield();
        } else {
            Sem_wait(&(m->queue));
        }
        if (idx != m->current_idx) { 
            Sem_post(&(m->queue));
            not_mine = 1;
            printf("worker[%lu]: not mine, next\n", pthread_self());
        }
            
    }
    printf("worker[%lu]: wake\n", pthread_self());
}

void ns_mutex_release(ns_mutex_t *m) {
    Sem_wait(&(m->mutex));
    m->current_idx++;
    Sem_post(&(m->mutex));  
    printf("worker[%lu]: post\n", pthread_self());
    Sem_post(&(m->queue));
}*/
// 上面使用了sched_yield(); 下面给出一种更好的解决方案：
typedef struct __ns_mutex_t {
    int room1; // number of threads in the waiting room one.
    int room2;
    sem_t mutex; // lock counters
    sem_t t1; // turnstile
    sem_t t2;
} ns_mutex_t;
void ns_mutex_init(ns_mutex_t *m) {
    m->room1 = 0;
    m->room2 = 0;
    Sem_init(&m->mutex, 1);
    Sem_init(&m->t1, 1);
    Sem_init(&m->t2, 0);
}
void ns_mutex_acquire(ns_mutex_t *m) {
    Sem_wait(&(m->mutex));
    m->room1++;
    Sem_post(&(m->mutex));
    
    Sem_wait(&(m->t1));

    Sem_wait(&(m->mutex));
    m->room2++;
    m->room1--;
    if (m->room1 == 0) {
        Sem_post(&(m->mutex));
        Sem_post(&(m->t2));
    } else {
        Sem_post(&(m->mutex));
        Sem_post(&(m->t1));
    }
    Sem_wait(&(m->t2));
    m->room2--;
}
void ns_mutex_release(ns_mutex_t *m) {
    if (m->room2 == 0) {
        Sem_post(&(m->t1));
    } else {
        Sem_post(&(m->t2));
    }
}



ns_mutex_t lock;

void *worker(void *arg) {

    printf("worker[%lu]: begin\n", pthread_self());
    ns_mutex_acquire(&lock);
    printf("worker[%lu]: working\n", pthread_self());
    ns_mutex_release(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 3);
    int num_worksers = atoi(argv[1]);
    int loops = atoi(argv[2]);

    pthread_t pr[num_worksers];

    ns_mutex_init(&lock);

    printf("parent: begin\n");

    int i;
    for (int i = 0; i < num_worksers; i++)
	    Pthread_create(&pr[i], NULL, worker, NULL);

    for (i = 0; i < num_worksers; i++)
	    Pthread_join(pr[i], NULL);

    printf("parent: end\n");
    return 0;
}

