#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h>   


#define N_PRODUTORES 6
#define N_CONSUMIDORES 2
#define TAMANHO_BUFFER 5


int buffer[TAMANHO_BUFFER];
int contador_itens = 0; 
int produtores_ativos; 

pthread_mutex_t buffer_mutex;
pthread_cond_t cond_buffer_cheio;     
pthread_cond_t cond_buffer_nao_cheio; 

typedef struct {
    int id;
    int total_produzir;
} produtor_args;

void *produtor(void *args) {
    produtor_args *p_args = (produtor_args *)args;
    int total_produzir = p_args->total_produzir;
    long tid = (long)pthread_self(); 

    for (int i = 0; i < total_produzir; i++) {
        int valor_produto = (rand() % 1000) + 1;

        pthread_mutex_lock(&buffer_mutex);

        while (contador_itens == TAMANHO_BUFFER) {
            pthread_cond_wait(&cond_buffer_nao_cheio, &buffer_mutex);
        }

        buffer[contador_itens] = valor_produto;
        contador_itens++;

        printf("(P) TID: %ld | VALOR: R$ %-4d | ITERACAO: %d/%d | Itens no Buffer: %d\n", tid, valor_produto, i + 1, total_produzir, contador_itens);

        if (contador_itens == TAMANHO_BUFFER) {
            pthread_cond_signal(&cond_buffer_cheio);
        }

        pthread_mutex_unlock(&buffer_mutex);

        sleep((rand() % 5) + 1);
    }

    pthread_mutex_lock(&buffer_mutex);
    produtores_ativos--;
    if (produtores_ativos == 0) {
        pthread_cond_broadcast(&cond_buffer_cheio); 
    }
    pthread_mutex_unlock(&buffer_mutex);

    printf("(P) TID: %ld finalizou a produção.\n", tid);
    free(args); 
    return NULL;
}

void *consumidor(void *args) {
    long tid = (long)pthread_self();
    int iteracao_consumo = 1;

    while (1) {
        pthread_mutex_lock(&buffer_mutex);

        while (contador_itens < TAMANHO_BUFFER && produtores_ativos > 0) {
            printf("(C) TID: %ld | Esperando o buffer encher (Itens: %d/%d)...\n", tid, contador_itens, TAMANHO_BUFFER);
            pthread_cond_wait(&cond_buffer_cheio, &buffer_mutex);
        }
        
        if (contador_itens < TAMANHO_BUFFER && produtores_ativos == 0) {
             pthread_mutex_unlock(&buffer_mutex);
             break; 
        }

        printf("(C) TID: %ld | Buffer cheio. Consumindo 5 itens...\n", tid);

        long long soma = 0;
        for (int i = 0; i < TAMANHO_BUFFER; i++) {
            soma += buffer[i];
        }
        double media = (double)soma / TAMANHO_BUFFER;
        
        printf("(C) TID: %ld | MEDIA: R$ %.2f | ITERACAO DE CONSUMO: %d\n", tid, media, iteracao_consumo++);

        contador_itens = 0;
        
        pthread_cond_broadcast(&cond_buffer_nao_cheio); 

        pthread_mutex_unlock(&buffer_mutex);
    }

    printf("(C) TID: %ld finalizou.\n", tid);
    return NULL;
}


int main(void) {
    srand(time(NULL));

    pthread_t threads_produtoras[N_PRODUTORES];
    pthread_t threads_consumidoras[N_CONSUMIDORES];

    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_cond_init(&cond_buffer_cheio, NULL);
    pthread_cond_init(&cond_buffer_nao_cheio, NULL);
    
    produtores_ativos = N_PRODUTORES;

    printf("Iniciando sistema com %d produtores e %d consumidores.\n", N_PRODUTORES, N_CONSUMIDORES);
    printf("Tamanho do buffer: %d\n\n", TAMANHO_BUFFER);


    for (int i = 0; i < N_PRODUTORES; i++) {
        produtor_args *args = malloc(sizeof(produtor_args));
        args->id = i + 1;
        args->total_produzir = (rand() % 11) + 20; 
        
        if (pthread_create(&threads_produtoras[i], NULL, produtor, args) != 0) {
            perror("Falha ao criar thread produtora");
            return 1;
        }
    }

    for (int i = 0; i < N_CONSUMIDORES; i++) {
        if (pthread_create(&threads_consumidoras[i], NULL, consumidor, NULL) != 0) {
            perror("Falha ao criar thread consumidora");
            return 1;
        }
    }

    for (int i = 0; i < N_PRODUTORES; i++) {
        pthread_join(threads_produtoras[i], NULL);
    }
    for (int i = 0; i < N_CONSUMIDORES; i++) {
        pthread_join(threads_consumidoras[i], NULL);
    }


    pthread_mutex_destroy(&buffer_mutex);
    pthread_cond_destroy(&cond_buffer_cheio);
    pthread_cond_destroy(&cond_buffer_nao_cheio);

    printf("\nTodas as threads finalizaram. Programa encerrado com sucesso.\n");

    return 0;
}