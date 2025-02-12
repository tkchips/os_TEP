## 31 SENAPHORES

### Questions

**The first problem is just to implement and test a solution to the fork/join problem, as described in the text. Even though this solution is described in the text, the act of typing it in on your own is worthwhile; even Bach would rewrite Vivaldi, allowing one soon-to-be master to learn from an existing one. See fork-join.c for details. Add the call sleep(1) to the child to ensure it is working.**

A: 
```c
/* fork-join.c */
void *child(void *arg) {
    printf("child\n");
    // use semaphore here
    sleep(1);
    sem_post(&s);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");
    // init semaphore here
    Sem_init(&s, 0);
    Pthread_create(&p, NULL, child, NULL);
    // use semaphore here
    sem_wait(&s);
    printf("parent: end\n");
    return 0;
}
/*
$ ./fork-join 
parent: begin
child
parent: end
*/
```

**Let's now generalize this a bit by investigating the rendezvous problem. The problem is as follows: you have two threads, each of which are about to enter the rendezvous point in the code. Neither should exit this part of the code before the other enters it. Consider using two semaphores for this task, and see rendezvous.c for details.**

A:
```c
/* rendezvous.c */
void *child_1(void *arg) {
    printf("child 1: before\n");
    // what goes here?
    Sem_post(&s2);
    Sem_wait(&s1);
    printf("child 1: after\n");
    return NULL;
}

void *child_2(void *arg) {
    printf("child 2: before\n");
    // what goes here?
    Sem_post(&s1);
    Sem_wait(&s2);
    printf("child 2: after\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("parent: begin\n");
    // init semaphores here
    Sem_init(&s1, 0);
    Sem_init(&s2, 0);
    Pthread_create(&p1, NULL, child_1, NULL);
    Pthread_create(&p2, NULL, child_2, NULL);
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("parent: end\n");
    return 0;
}
/*
$ ./rendezvous 
parent: begin
child 1: before
child 2: before
child 2: after
child 1: after
parent: end
*/
```

**Now go one step further by implementing a general solution to barrier synchronization. Assume there are two points in a sequential piece of code, called P1 and P2. Putting a barrier between P1 and P2 guarantees that all threads will execute P1 before any one thread executes P2. Your task: write the code to implement a barrier() function that can be used in this manner. It is safe to assume you know N (the total number of threads in the running program) and that all N threads will try to enter the barrier. Again,you should likely use two semaphores to achieve the solution, and some other integers to count things. See barrier.c for details.**

A:
```c
/* barrier.c */
typedef struct __barrier_t {
    // add semaphores and other information here
    sem_t s, m;
    int count, num_threads;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    // initialization code goes here
    b->count = 0; 
    b->num_threads = num_threads;
    Sem_init(&(b->m), 1);
    Sem_init(&(b->s), 0);
}

void barrier(barrier_t *b) {
    // barrier code goes here
    Sem_wait(&(b->m));
    b->count++;
    if (b->count == b->num_threads) {
        for(int i = 0; i < b->num_threads; i++)
            Sem_post(&(b->s));
    }
    Sem_post(&(b->m));
    Sem_wait(&(b->s));
}
/*
output:
$ ./barrier 3
parent: begin
child 0: before
child 1: before
child 2: before
child 2: after
child 0: after
child 1: after
parent: end
*/
```

**Now let's solve the reader-writer problem, also as described in the text. In this first take, don’t worry about starvation. See the code in reader-writer.c for details. Add sleep() calls to your code to demonstrate it works as you expect. Can you show the existence of the starvation problem?**

A:
```c
/* reader-writer.c */
typedef struct __rwlock_t {
    sem_t mutex, w;
    ssize_t reader_count;
} rwlock_t;


void rwlock_init(rwlock_t *rw) {
    Sem_init(&(rw->mutex), 1);
    Sem_init(&(rw->w), 1);
    rw->reader_count = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    Sem_wait(&(rw->mutex));
    rw->reader_count++;
    if (rw->reader_count == 1) {
        Sem_wait(&(rw->w));
    }
    Sem_post(&(rw->mutex));
}

void rwlock_release_readlock(rwlock_t *rw) {
    Sem_wait(&(rw->mutex));
    rw->reader_count--;
    if (rw->reader_count == 0) {
        Sem_post(&(rw->w));
    }
    Sem_post(&(rw->mutex));
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    Sem_wait(&(rw->w));
}

void rwlock_release_writelock(rwlock_t *rw) {;
    Sem_post(&(rw->w));
}
/*
output:
./reader-writer 3 3 1
begin
write 1
write 2
write 3
read 3
read 3
read 3
end: value 3
*/
```

**Let's look at the reader-writer problem again, but this time, worry about starvation. How can you ensure that all readers and writers eventually make progress? See reader-writer-nostarve.c for details.**

A:
```c
/* reader-writer-nostarve.c */
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
/*
output:
$ ./reader-writer-nostarve 1 1 10
begin
read 0
write 1
read 1
write 2
write 3
write 4
read 4
write 5
write 6
read 6
write 7
write 8
write 9
read 9
write 10
read 10
read 10
read 10
read 10
read 10
end: value 10

```

**Use semaphores to build a no-starve mutex, in which any thread that tries to acquire the mutex will eventually obtain it. See the code in mutex-nostarve.c for more information.**

A:
```c
/* mutex-nostarve.c */

/*
output:
$ 
```

**Liked these problems? See Downey’s free text for more just like them. And don’t forget, have fun! But, you always do when you write code, no?**