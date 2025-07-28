# Questão 01 - Problema Produtor Consumidor (5 e 7 Threads)

## Compilar e Executar os 2

```bash
gcc -o prodcon_p1 q01_p1_prodcon.c -pthread
gcc -o prodcon_p2 q01_p2_prodcon.c -pthread
```

```bash
./prodcon_p1
./prodcon_p2
```

## Detalhes da Implementação
A implementação da primeira questão foi dividida em duas partes, com a primeira utilizando três threads produtoras e uma consumidora, já a segunda utilizava seis threads produtoras e duas consumidoras, tendo as duas propostas um buffer que comportava cinco intens por vez.
Em questão de código, as duas propostas seguem um padrão identico, sem alteração nas estruturas, apenas alterando o número de threads a serem trabalhadas. Implementando um mutex e semáforos para garantirem a exclusão mútua e a utilização de conds. 
* **5 Threads:** Aqui temos um equilibrio, com três produtoras garantimos que não tenhamos muitas threads 'dormindo', esperando sua vez de preencher o buffer, e com apenas uma consumidora, garantimos que apenas ela tenha acesso ao buffer. Criando assim um ciclo de preenche -> esvazia -> repete.
* **7 Threads:** Aqui começamos a ter problemas, já que com seis produtoras temos muitas threads esperando a sua vez gerando uma alta contenção no mutex, dando a ele a responsabilidade de segurar muitas threads de uma só vez, e com duas consumidoras temos a inutilização de uma delas, já que usando o ```pthread_cond_signal``` 'acordamos' apenas uma das consumidoras, tendo assim apenas uma consumindo por vez, mesmo tendo duas consumidoras implementadas no sistema.

# Questão 02 - Cálculo de PI com Série de Leibniz (Sequencial e Paralelo)

## Compilar e Executar os 2

```bash
gcc -o pi_seq q02_p1_pi_sequencial.c -O2
gcc -o pi_par q02_p2_pi_paralelo.c -pthread -O2
```

```bash
./pi_seq
./pi_par
```

## Resultados (Comparação de Desempenho)
Tabela comparando o tempo de execução do processo e o tempo total das threads, conforme o número de threads utilizadas:
| Threads | Tempo Processo (s) | Tempo Threads (s) |
| ------- | ------------------ | ----------------- |
| 2       | 1.25               | 2.49              |
| 4       | 0.63               | 2.53              |
| 6       | 0.45               | 2.64              |
| 8       | 0.50               | 3.29              |
| 16      | 0.45               | 6.35              |
| 32      | 0.45               | 13.04             |
