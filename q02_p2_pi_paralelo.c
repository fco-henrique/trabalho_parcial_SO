#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define TOTAL_TERMOS 2000000000LL
#define NUM_THREADS 16
#define TERMOS_POR_THREAD (TOTAL_TERMOS / NUM_THREADS)

double soma_total = 0.0;

pthread_mutex_t mutex_soma;

typedef struct {
    int id_da_thread;
    long long termo_inicial;
} argumentos_thread;

double calcular_soma_parcial(long long termo_inicial) {
    long long termo_final = termo_inicial + TERMOS_POR_THREAD;
    double soma_local = 0.0;

    double sinal = (termo_inicial % 2 == 0) ? 1.0 : -1.0;

    for (long long k = termo_inicial; k < termo_final; k++) {
        soma_local += sinal / (2 * k + 1);
        sinal *= -1.0;
    }

    return soma_local;
}

void *funcao_da_thread(void *args) {
    argumentos_thread *dados = (argumentos_thread *)args;

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    double soma_parcial = calcular_soma_parcial(dados->termo_inicial);

    pthread_mutex_lock(&mutex_soma);
    soma_total += soma_parcial;
    pthread_mutex_unlock(&mutex_soma);

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) +
                         (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("TID: %ld: %.2fs\n", (long)pthread_self(), tempo_gasto);

    double *tempo_ptr = malloc(sizeof(double));
    *tempo_ptr = tempo_gasto;

    free(args);
    return tempo_ptr;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    double tempo_total_threads = 0.0;
    struct timespec inicio_total, fim_total;

    pthread_mutex_init(&mutex_soma, NULL);

    printf("\nIniciando cálculo paralelo com %d threads...\n", NUM_THREADS);

    clock_gettime(CLOCK_MONOTONIC, &inicio_total);

    for (int i = 0; i < NUM_THREADS; i++) {
        argumentos_thread *args = malloc(sizeof(argumentos_thread));
        args->id_da_thread = i;
        args->termo_inicial = (long long)i * TERMOS_POR_THREAD;

        if (pthread_create(&threads[i], NULL, funcao_da_thread, args) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        void *tempo_ptr;
        pthread_join(threads[i], &tempo_ptr);

        tempo_total_threads += *(double *)tempo_ptr;
        free(tempo_ptr);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim_total);
    double tempo_total = (fim_total.tv_sec - inicio_total.tv_sec) +
                         (fim_total.tv_nsec - inicio_total.tv_nsec) / 1e9;

    double pi_aproximado = 4.0 * soma_total;

    pthread_mutex_destroy(&mutex_soma);
    printf("\n--- Resultado ---\n");
    printf("Tempo Processo (Paralelo): %.2fs\n", tempo_total);
    printf("Total Threads: %.2fs\n", tempo_total_threads);
    printf("Valor do PI: %.9f\n", pi_aproximado);

    return 0;
}
