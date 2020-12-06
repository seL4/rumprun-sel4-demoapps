/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#define _NETBSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>

int num_cores;

int set_thread_affinity(int thread_id)
{
    int core_id = thread_id % num_cores;

    /* Create NetBSD cpuset */
    cpuset_t *cpuset = cpuset_create();
    cpuset_zero(cpuset);
    cpuset_set(core_id, cpuset);

    pthread_t current_thread = pthread_self();
    return pthread_setaffinity_np(current_thread, cpuset_size(cpuset), cpuset);
}

/* A task that takes some time to complete. The id identifies distinct
   tasks for printed messages. */
double task(int id)
{
    printf("Task %d started\n", id);
    int i;
    double result = 0.0;
    for (i = 0; i < 10000000; i++) {
        result = result + cos(i) * sin(i);
    }
    return result;
}

/* Same as 'task', but meant to be called from different threads. */
void *start_thread(void *t)
{
    int id = (int) t;
    set_thread_affinity(id);
    printf("Thread %ld started\n", id);
    double result = task(id);
    printf("Thread %ld done result %e\n", id, result);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        return -1;
    }

    int num_threads = atoi(argv[1]);
    num_cores = atoi(argv[2]);
    pthread_t thread[num_threads];
    for (int i = 0; i < num_threads; i++) {
        printf("Creating thread %ld\n", i);
        int error = pthread_create(&thread[i], NULL, start_thread, (void *)i);
        if (error) {
            return -1;
        }
    }

    pthread_exit(NULL);
}