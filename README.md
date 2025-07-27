# Questão 02 - Cálculo de PI com Série de Leibniz (Sequencial e Paralelo)

## Compilação
Use o compilador `gcc` com a flag `-pthread` para o código paralelo.
```bash
# Compilar versão sequencial
gcc q02_p1_pi_sequencial.c -o pi_seq -O2

# Compilar versão paralela
gcc q02_p2_pi_paralelo.c -o pi_par -pthread -O2
```

---
## Execução
### Versão Sequencial

```bash
./pi_seq
```

### Versão Paralela

```bash
./pi_par
```

---
## Compilar e Executar os 2

```bash
gcc -o pi_seq q02_p1_pi_sequencial.c -O2
gcc -o pi_par q02_p2_pi_paralelo.c -pthread -O2
```

```bash
./pi_seq
./pi_par
```
ou

```bash
./pi_seq && ./pi_par
```

---
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
