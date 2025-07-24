#define _POSIX_C_SOURCE 199309L 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_TERMS 2000000000LL
#define NUM_THREADS 2

#define PARTIAL_NUM_TERMS (NUM_TERMS / NUM_THREADS)

double total_sum = 0.0;
pthread_mutex_t sum_mutex; 

typedef struct {
    long long start_term;
    int thread_id;
} thread_args_t;

void *partial_pi_sum(void *args) {
    thread_args_t *thread_data = (thread_args_t *)args;
    long long start_k = thread_data->start_term;
    long long end_k = start_k + PARTIAL_NUM_TERMS;
    double local_sum = 0.0; 
    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (long long k = start_k; k < end_k; k++) {
        if (k % 2 == 0) {
            local_sum += 1.0 / (2 * k + 1);
        } else {
            local_sum -= 1.0 / (2 * k + 1);
        }
    }

    pthread_mutex_lock(&sum_mutex);
    total_sum += local_sum;
    pthread_mutex_unlock(&sum_mutex);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double time_taken = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    
    printf("TID: %ld: %.2f s\n", (long)pthread_self(), time_taken);
    
    double *return_time = malloc(sizeof(double));
    *return_time = time_taken;
    
    free(args); 
    return return_time;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    struct timespec start_process, end_process;
    double sum_of_threads_time = 0.0;

    pthread_mutex_init(&sum_mutex, NULL);

    printf("Iniciando cálculo paralelo com %d threads...\n\n", NUM_THREADS);

    clock_gettime(CLOCK_MONOTONIC, &start_process);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args_t *args = malloc(sizeof(thread_args_t));
        args->thread_id = i;
        args->start_term = (long long)i * PARTIAL_NUM_TERMS;

        if (pthread_create(&threads[i], NULL, partial_pi_sum, args) != 0) {
            perror("Falha ao criar a thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        void *thread_time_ptr;
        pthread_join(threads[i], &thread_time_ptr);
        
        sum_of_threads_time += *(double*)thread_time_ptr;
        free(thread_time_ptr); 
    }

    double pi_approximation = 4.0 * total_sum;

    clock_gettime(CLOCK_MONOTONIC, &end_process);

    double process_time_taken = (end_process.tv_sec - start_process.tv_sec) + (end_process.tv_nsec - start_process.tv_nsec) / 1e9;

    pthread_mutex_destroy(&sum_mutex);

    printf("\n--- Resultado (Paralelo) ---\n");
    printf("Valor de PI aproximado: %.9f\n", pi_approximation);
    printf("Total Processo (Paralelo): %.2f s\n", process_time_taken);
    printf("Total Threads: %.2f s\n", sum_of_threads_time);

    return 0;
}