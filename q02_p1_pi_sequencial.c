#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>

#define TOTAL_TERMOS 2000000000LL

int main(void) {
    double soma = 0.0;
    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long long k = 0; k < TOTAL_TERMOS; k++) {
        double sinal = (k % 2 == 0) ? 1.0 : -1.0;
        soma += sinal / (2 * k + 1);
    }

    double pi_aproximado = 4.0 * soma;

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) +
                         (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("\n--- Resultado ---\n");
    printf("Tempo Processo (Sequencial): %.2fs\n", tempo_gasto);
    printf("Valor do PI: %.9f\n", pi_aproximado);

    return 0;
}
