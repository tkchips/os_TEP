#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_pages> <num_tries>\n", argv[0]);
        return 1;
    }

    int num_pages = atoi(argv[1]);
    int num_tries = atoi(argv[2]);
    int page_size = getpagesize();
    int jump = page_size / sizeof(int);
    volatile int *a;

    // 动态减少大页数的尝试次数
    if (num_pages >= 4096) {
        num_tries = 1000;
    } else if (num_pages >= 1024) {
        num_tries = 5000;
    }

    // 分配内存并检查
    size_t total_size = num_pages * page_size;
    if (posix_memalign((void **)&a, page_size, total_size) != 0) {
        fprintf(stderr, "Error: Failed to allocate %zu bytes\n", total_size);
        return 1;
    }

    // 初始化数组
    for (int i = 0; i < num_pages * jump; i += jump) {
        a[i] = 0;
    }

    // 绑定到 CPU 0
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        perror("sched_setaffinity");
        free((void*)a);
        return 1;
    }

    // 高精度计时


    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int t = 0; t < num_tries; ++t) {
        for (int i = 0; i < num_pages * jump; i += jump) {
            a[i] += 1;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end); 

    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    double avg_time = (double)elapsed_ns / (num_tries * num_pages);
    

    /*struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int t = 0; t < num_tries; t++) {
        for (int i = 0; i < num_pages * jump; i += jump) {
            a[i] += 1;
        }
    }
    gettimeofday(&end, NULL);

    long long elapsed_us = (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec);
    double avg_time = (double)elapsed_us / (num_tries * num_pages);
    
    
    printf("%d %.6f\n", num_pages, avg_time);*/

    free((void*)a);
    return 0;
}