#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h>

#define NUM_PRODUTORES 3
#define NUM_CONSUMIDORES 1
#define TAM_BUFFER 5

int buffer[TAM_BUFFER];
int contadorItems = 0; // qtd de itens que estão no buffer
int produtoresAtivos;

pthread_mutex_t mutexDoBuffer;
pthread_cond_t condBufferCheio;
pthread_cond_t condBufferComEspaco;

typedef struct{
    int id;
    int totalAProduzir; 
} produtorArgs;

void *produtor(void *args) {
    produtorArgs *pArgs = (produtorArgs *)args; //pega os argumentos
    int totalAProduzir = pArgs->totalAProduzir; 
    long tId = (long) pthread_self(); //pega o tId da thread

    for (int i = 0; i < totalAProduzir; i++){
        int valorProduto = (rand() % 1000) + 1;

        pthread_mutex_lock(&mutexDoBuffer); //pega o mutex e garante a exclusão mútua

        while(contadorItems == TAM_BUFFER) {
            pthread_cond_wait(&condBufferComEspaco, &mutexDoBuffer);
        }

        buffer[contadorItems] = valorProduto;
        contadorItems++;

        printf("(P) TID: %ld | VALOR: R$ %-4d | ITERACAO: %d/%d | Itens no Buffer: %d\n", tId, valorProduto, i + 1, totalAProduzir, contadorItems);

        if (contadorItems == TAM_BUFFER) {
            pthread_cond_signal(&condBufferCheio);
        }

        pthread_mutex_unlock(&mutexDoBuffer);
        sleep((rand() % 5) + 1);
    }

    pthread_mutex_lock(&mutexDoBuffer);
    produtoresAtivos--;
    if (produtoresAtivos == 0) {
        pthread_cond_broadcast(&condBufferCheio); 
    }    
    pthread_mutex_unlock(&mutexDoBuffer);

    printf("(P) TID: %ld finalizou a produção.\n", tId);
    free(args); 
    return NULL;
}

void *consumidor(void *args) {
    long tId = (long) pthread_self();
    int iteracaoDeConsumo = 1;

    while (1) {
        pthread_mutex_lock(&mutexDoBuffer);

        while (contadorItems < TAM_BUFFER && produtoresAtivos > 0) {
            printf("(C) TID: %ld | Esperando o buffer encher (Itens: %d/%d)...\n", tId, contadorItems, TAM_BUFFER);
            pthread_cond_wait(&condBufferCheio, &mutexDoBuffer);
        }

        //se não tiver mais produtores ele sai do loop
        if (contadorItems < TAM_BUFFER && produtoresAtivos == 0) {
            pthread_mutex_unlock(&mutexDoBuffer);
            break;
        }

        printf("(C) TID: %ld | Buffer cheio. Consumindo 5 itens...\n", tId);

        //calcula a média dos valores no buffer
        long long soma = 0;
        for (int i = 0; i < TAM_BUFFER; i++) {
            soma += buffer[i];
        }
        double media = (double)soma / TAM_BUFFER;

        printf("(C) TID: %ld | MEDIA: R$ %.2f | ITERACAO DE CONSUMO: %d\n", tId, media, iteracaoDeConsumo++);

        contadorItems = 0;
        pthread_cond_broadcast(&condBufferComEspaco);
        pthread_mutex_unlock(&mutexDoBuffer);
    }
    
    printf("(C) TID: %ld finalizou.\n", tId);
    return NULL;
}

int main() {
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    srand(time(NULL));

    pthread_t threadsProdutoras[NUM_PRODUTORES];
    pthread_t threadsConsumidoras[NUM_CONSUMIDORES];

    pthread_mutex_init(&mutexDoBuffer, NULL);
    pthread_cond_init(&condBufferCheio, NULL);
    pthread_cond_init(&condBufferComEspaco, NULL);

    produtoresAtivos = NUM_PRODUTORES;

    printf("Iniciando sistema com %d produtores e %d consumidores.\n", NUM_PRODUTORES, NUM_CONSUMIDORES);
    printf("Tamanho do buffer: %d\n\n", TAM_BUFFER);

    //cria as threads produtoras
    for (int i = 0; i < NUM_PRODUTORES; i++){
        produtorArgs *args = malloc(sizeof(produtorArgs));
        args->id = i + 1;
        args->totalAProduzir = (rand() % 11) + 20;

        if (pthread_create(&threadsProdutoras[i], NULL, produtor, args) != 0) {
            perror("Falha ao criar a thread produtora.");
            return 1;
        }
    }

    //cria as threads consumidoras
    for (int i = 0; i < NUM_CONSUMIDORES; i++){
        if (pthread_create(&threadsConsumidoras[i], NULL, consumidor, NULL) != 0) {
            perror("Falha ao criar a thread produtora.");
            return 1;
        }
    }
    
    for (int i = 0; i < NUM_PRODUTORES; i++) {
        pthread_join(threadsProdutoras[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(threadsConsumidoras[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim); 

    double tempo_decorrido;
    tempo_decorrido = (fim.tv_sec - inicio.tv_sec);
    tempo_decorrido += (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;

    pthread_mutex_destroy(&mutexDoBuffer);
    pthread_cond_destroy(&condBufferCheio);
    pthread_cond_destroy(&condBufferComEspaco);

    printf("\nTodas as threads finalizaram. Programa encerrado com sucesso.\n");

    printf("Tempo total de execução: %.2f segundos\n", tempo_decorrido);

    return 0;
}