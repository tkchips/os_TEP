## 32 Common Concurrency Problems

### Questions

** First let’s make sure you understand how the programs generally work, and some of the key options. Study the code in vector-deadlock.c, as well as in main-common.c and related files. Now, run ./vector-deadlock -n 2 -l 1 -v, which instantiates two threads (-n 2), each of which does one vector add (-l 1), and does so in verbose mode (-v). Make sure you understand the output. How does the output change from run to run?**

A:
```c
$ ./vector-deadlock -n 2 -l 1 -v
->add(0, 1)
<-add(0, 1)
              ->add(0, 1)
              <-add(0, 1)
```

**2. Now add the -d flag, and change the number of loops (-l) from 1 to higher numbers. What happens? Does the code (always) deadlock?**

A: 不总是死锁，小概率发生；分析：

- 线程 1:
- 调用：vector_add(v1,v2)，获取 v1 的锁，切换到线程 2
- 线程 2:
- 调用：vector_add(v2,v1)，获取 v2 的锁，然后尝试获取 v1 的锁，最终循环等待造成死锁
如果所有线程都按照以下顺序获取锁，那么就不会死锁：

```c
Pthread_mutex_lock(&v_dst->lock);
Pthread_mutex_lock(&v_src->lock);
```

但如过两个线程以相反的顺序获取锁，那么就可能引发死锁：

```c
/* thread1 */ 
Pthread_mutex_lock(&v_dst->lock);
Pthread_mutex_lock(&v_src->lock);

/* thread2 */ 
Pthread_mutex_lock(&v_src->lock);
Pthread_mutex_lock(&v_dst->lock);
```


**How does changing the number of threads (-n) change the outcome of the program? Are there any values of -n that ensure no deadlock occurs?**

A: 死锁概率变大， -n 0 、-n 1 不会发生死锁

**Now examine the code in vector-global-order.c. First, make sure you understand what the code is trying to do; do you understand why the code avoids deadlock? Also, why is there a special case in this vector add() routine when the source and destination vectors are the same?**

A: 使用地址大小作为区分保证获取锁的顺序一致，避免死锁；并且还考虑了源地址和目的地址一样的情况,当源地址和目标地址一致时，无法通过大小的判断得到获取锁的顺序，所以必须要单独处理这种情况

**Now run the code with the following flags: -t -n 2 -l 100000 -d. How long does the code take to complete? How does the total time change when you increase the number of loops, or the number of threads?**

A: 0.03s 

```c
-l/-n        2      4       8     16     32     64   
100000      0.03   0.09   0.06   0.14   0.33   0.55
200000      0.07   0.17   0.17   0.29   0.62   1.14
400000      0.12   0.29   0.34   0.61   1.13   2.68
800000      0.26   0.59   0.70   1.36   2.80   4.96
1600000     0.41   1.41   1.53   2.48   4.54   8.96
3200000     0.98   2.78   2.76   4.71   9.74   19.93
```

**What happens if you turn on the parallelism flag (-p)? How much would you expect performance to change when each thread is working on adding different vectors (which is what -p enables) versus working on the same ones?**

A: 

```c
-l/-n        2      4      8      16     32     64   
100000      0.01   0.01   0.02   0.03   0.06   0.11
200000      0.02   0.03   0.04   0.07   0.12   0.22
400000      0.04   0.06   0.09   0.15   0.24   0.47
800000      0.08   0.13   0.15   0.26   0.45   0.91
1600000     0.16   0.26   0.31   0.49   0.96   1.80
3200000     0.31   0.48   0.57   1.02   1.88   3.53
```

**Now let’s study vector-try-wait.c. First make sure you understand the code. Is the first call to pthread mutex trylock() really needed? Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?**

A: no, 换成lock更好；见图“vector-try-wait.png”

**Now let’s look at vector-avoid-hold-and-wait.c. What is the main problem with this approach? How does its performance compare to the other versions, when running both with -p and without it?**

A: 直接获取全局锁，只有全局锁的竞争,锁的粒度更大，性能比vector-try-wait.c 不加-p的都要差
```c
with -p

-l/-n        2      4      8      16     32     64   
100000      0.03   0.05   0.10   0.17   0.34   0.81
200000      0.04   0.10   0.19   0.33   0.69   1.73
400000      0.07   0.16   0.41   0.67   1.40   3.30
800000      0.15   0.38   0.73   1.27   2.76   6.76
1600000     0.29   0.82   1.48   2.53   5.85   13.91
3200000     0.55   1.55   3.05   5.28   11.68  27.75
```
**Finally, let’s look at vector-nolock.c. This version doesn’t use locks at all; does it provide the exact same semantics as the other versions? Why or why not?**

A： 提供了，因为使用了fetch-and-add操作，所以每次调用add操作，都是原子操作，不会出现数据竞争。
```c
-l/-n       2       4       8      16     32     64   
100000      0.05   0.08   0.15   0.29   0.55   1.14
200000      0.11   0.15   0.30   0.60   1.09   2.26
400000      0.28   0.32   0.58   1.13   2.25   4.52
800000      0.51   0.57   1.14   2.30   4.69   9.20
1600000     0.95   1.17   2.29   4.52   9.00   18.41
3200000     1.95   2.36   4.58   9.10   18.27  36.29
```
**Now compare its performance to the other versions, both when threads are working on the same two vectors (no -p) and when each thread is working on separate vectors (-p). How does this no-lock version perform?**
A：

```c
./vector-nolock -t -n 2 -l 100000
Time: 0.11 seconds
./vector-nolock -t -n 2 -l 100000 -p
Time: 0.06 seconds

./vector-nolock -t -n 4 -l 100000
Time: 0.21 seconds
./vector-nolock -t -n 4 -l 100000 -p
Time: 0.10 seconds
```