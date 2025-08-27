// main.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "buffer.h"

circular_buffer_t shared_buffer;

typedef struct {
  int id;
  int items;
  int delay_ms;
} thread_args_t;

void *producer(void *arg) {
  thread_args_t *args = (thread_args_t *)arg;

  // TODO: Implementar loop do produtor
  // Para cada item:
  // 1. Gerar um valor único (ex: args->id * 1000 + i)
  // 2. Chamar buffer_put()
  // 3. Fazer sleep se delay_ms > 0 (usar usleep(delay_ms * 1000))

  printf("Producer %d terminou\n", args->id);
  return NULL;
}

void *consumer(void *arg) {
  thread_args_t *args = (thread_args_t *)arg;

  // TODO: Implementar loop do consumidor
  // Para cada item:
  // 1. Chamar buffer_get()
  // 2. Fazer sleep se delay_ms > 0 (usar usleep(delay_ms * 1000))

  printf("Consumer %d terminou\n", args->id);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    fprintf(stderr, "Uso: %s <#producers> <#consumers> <#items> <producer_delay_ms> <consumer_delay_ms>\n", argv[0]);
    fprintf(stderr, "Exemplo: %s 2 2 20 100 150\n", argv[0]);
    exit(-1);
  }

  int num_producers = atoi(argv[1]);
  int num_consumers = atoi(argv[2]);
  int total_items = atoi(argv[3]);
  int producer_delay = atoi(argv[4]);
  int consumer_delay = atoi(argv[5]);

  printf("=== EXERCÍCIO: PRODUTOR-CONSUMIDOR COM BUFFER CIRCULAR ===\n");
  printf("Produtores: %d, Consumidores: %d, Itens: %d\n", num_producers, num_consumers, total_items);
  printf("Delays: Producer %dms, Consumer %dms\n", producer_delay, consumer_delay);
  printf("Tamanho do buffer: %d\n\n", BUFFER_SIZE);

  buffer_init(&shared_buffer);

  // TODO: Alocar arrays para threads e argumentos
  // Dica: malloc para (num_producers + num_consumers) threads

  struct timeval start, end;
  gettimeofday(&start, NULL);

  // TODO: Criar threads produtoras
  // Para cada produtor:
  // 1. Configurar argumentos (id, items = total_items/num_producers, delay)
  // 2. Chamar pthread_create() com função producer

  // TODO: Criar threads consumidoras
  // Para cada consumidor:
  // 1. Configurar argumentos (id, items = total_items/num_consumers, delay)
  // 2. Chamar pthread_create() com função consumer

  // TODO: Aguardar todas as threads terminarem
  // Usar pthread_join() para cada thread criada

  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

  printf("\nEstatísticas:\n");
  printf("Tempo total: %.3f segundos\n", elapsed);
  printf("Itens processados: %d\n", total_items);
  printf("Buffer final vazio: %s\n", buffer_is_empty(&shared_buffer) ? "Sim" : "Não");

  buffer_destroy(&shared_buffer);

  // TODO: Liberar memória alocada (free)

  return 0;
}