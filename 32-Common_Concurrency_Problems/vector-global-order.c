#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

#include "main-header.h"
#include "vector-header.h"

void vector_add(vector_t *v_dst, vector_t *v_src) {
    if (v_dst < v_src) {
        Pthread_mutex_lock(&v_dst->lock);
        Pthread_mutex_lock(&v_src->lock);
    } else if (v_dst > v_src) {
        Pthread_mutex_lock(&v_src->lock);
        Pthread_mutex_lock(&v_dst->lock);
    } else {
	// special case: src and dst are the same
        Pthread_mutex_lock(&v_src->lock);
    }
    int i;
    for (i = 0; i < VECTOR_SIZE; i++) {
        v_dst->values[i] = v_dst->values[i] + v_src->values[i];
    }
    Pthread_mutex_unlock(&v_src->lock);
    if (v_dst != v_src) 
	    Pthread_mutex_unlock(&v_dst->lock);
}

void fini() {}


#include "main-common.c"

