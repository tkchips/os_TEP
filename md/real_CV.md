# real_CV

## 30 CONDITION VARIABLES

### Questions

* ##### P: Producer
* ##### C: Consumer
* ##### EOS = END_OF_STREAM

##### Q: Our first question focuses on main-two-cvs-while.c (the working solution). First, study the code. Do you think you have an understanding of what should happen when you run the program?

A: 真的很难理解，其实不是代码有多么复杂，

* 而是这堆代码追求复用性以至于把一个通用的main函数放在一个.c文件中并且在最后include，
* 或者一个单独的头文件中定义一堆全局变量和宏定义，
* 又或者说一个头文件用到了pthread_mutex_t， printf等内容但又没有包含这些变量，函数的头文件；而是依赖在include这个头文件的外部的include来引入头文件。。。
* 另外，一些代码的缩进也很诡异。。所以我动手修改了下代码，让这些代码变得human-readable一点.

##### Q: Run with one producer and one consumer, and have the producer produce a few values. Start with a buffer (size 1), and then increase it. How does the behavior of the code change with larger buffers? (or does it?)

##### What would you predict num full to be with different buffer sizes (e.g., -m 10) and different numbers of produced items (e.g., -l 100), when you change the consumer sleep string from default (no sleep) to -C 0,0,0,0,0,0,1?

A: 使用如下指令：

* ./main-two-cvs-while -l 5 -m 1 -p 1 -c 1 -v -t   Start with a buffer :1
* ./main-two-cvs-while -l 5 -m 5 -p 1 -c 1 -v -t   increase to 5
* ./main-two-cvs-while -l 5 -m 10 -p 1 -c 1 -v -t  increase to 10

发现并没有什么区别，使用如下指令：

* ./main-two-cvs-while -l 5 -m 5 -p 1 -c 1 -v -t -C 0,0,0,0,0,0,1
  * 可见C几乎拿不到锁，在P EOS 后才能获取到锁将数据输出
* ./main-two-cvs-while -l 100 -m 5 -p 1 -c 1 -v -t -C 0,0,0,0,0,0,1
  * 同上，在P填满缓冲区后，C才能获取到一次锁，并消费一次数据
* ./main-two-cvs-while -l 5 -m 10 -p 1 -c 1 -v -t -C 0,0,0,0,0,0,1
  * 同第一次

##### Q: If possible, run the code on different systems (e.g., a Mac and Linux). Do you see different behavior across these systems?

A: Don't have a Mac

##### Q: Let’s look at some timings. How long do you think the following execution, with one producer, three consumers, a single-entry shared buffer, and each consumer pausing at point c3 for a second, will take? ./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v -t

A: 10s? But the output is 12s;
  Test:

* 2,2,2  22s
* 5,5,5  60s

##### Q: Now change the size of the shared buffer to 3 (-m 3). Will this make any difference in the total time?

A: The output is 11s

##### Q: Now change the location of the sleep to c6 (this models a consumer taking something off the queue and then doing something with it), again using a single-entry buffer. What time do you predict in this case? ./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10 -v -t

A: The output is 5s

##### Q: Finally, change the buffer size to 3 again (-m 3). What time do you predict now?

A: The output is 5s

##### Q: Now let’s look at main-one-cv-while.c. Can you configure a sleep string, assuming a single producer, one consumer, and a buffer of size 1, to cause a problem with this code?



##### Now change the number of consumers to two. Can you construct sleep strings for the producer and the consumers so as to cause a problem in the code?

##### Now examine main-two-cvs-if.c. Can you cause a problem to happen in this code? Again consider the case where there is only one consumer, and then the case where there is more than one.

##### Finally, examine main-two-cvs-while-extra-unlock.c. What problem arises when you release the lock before doing a put or a get? Can you reliably cause such a problem to happen, given the sleep strings? What bad thing can happen?
