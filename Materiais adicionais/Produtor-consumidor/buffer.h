// buffer.h
#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>

#define BUFFER_SIZE 5

typedef struct {
  int buffer[BUFFER_SIZE];
  int in;     // próxima posição para inserir
  int out;    // próxima posição para remover
  int count;  // número de itens no buffer

  // TODO: Declarar mutex e condition variables necessárias
  // Dica: precisa de 1 mutex e 2 condition variables (empty e full)

} circular_buffer_t;

void buffer_init(circular_buffer_t *buf);
void buffer_destroy(circular_buffer_t *buf);
void buffer_put(circular_buffer_t *buf, int value);
int buffer_get(circular_buffer_t *buf);
int buffer_is_empty(circular_buffer_t *buf);
int buffer_is_full(circular_buffer_t *buf);

#endif