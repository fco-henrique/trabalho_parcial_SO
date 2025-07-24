#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

#define NUM_TERMS 2000000000LL

int main(void) {
    double sum = 0.0;
    struct timespec start, end;

    printf("Iniciando cálculo sequencial com %lld termos...\n", NUM_TERMS);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (long long k = 0; k < NUM_TERMS; k++) {
        sum += ((k % 2 == 0) ? 1.0 : -1.0) / (2 * k + 1);
    }

    double pi_approximation = 4.0 * sum;

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_taken = (end.tv_sec - start.tv_sec) + 
                        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("\n--- Resultado (Sequencial) ---\n");
    printf("Valor de PI aproximado: %.9f\n", pi_approximation);
    printf("Total Processo (Sequencial): %.2f s\n", time_taken);

    return 0;
}
