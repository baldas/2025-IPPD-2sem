// buffer.c
#include "buffer.h"

#include <stdio.h>

void buffer_init(circular_buffer_t *buf) {
  buf->in = 0;
  buf->out = 0;
  buf->count = 0;

  // TODO: Inicializar mutex e condition variables
  // Usar pthread_mutex_init() e pthread_cond_init()
}

void buffer_destroy(circular_buffer_t *buf) {
  // TODO: Destruir mutex e condition variables
  // Usar pthread_mutex_destroy() e pthread_cond_destroy()
}

void buffer_put(circular_buffer_t *buf, int value) {
  // TODO: Implementar a função do produtor
  // Passos:
  // 1. Adquirir o mutex
  // 2. Enquanto buffer estiver cheio, aguardar na condition variable "empty"
  // 3. Inserir o item no buffer circular (usar buf->in)
  // 4. Atualizar buf->in = (buf->in + 1) % BUFFER_SIZE
  // 5. Incrementar buf->count
  // 6. Sinalizar condition variable "full"
  // 7. Liberar o mutex

  printf("[PRODUCER] Inseriu %d (buffer count: %d)\n", value, buf->count);
}

int buffer_get(circular_buffer_t *buf) {
  int value;

  // TODO: Implementar a função do consumidor
  // Passos:
  // 1. Adquirir o mutex
  // 2. Enquanto buffer estiver vazio, aguardar na condition variable "full"
  // 3. Remover o item do buffer circular (usar buf->out)
  // 4. Atualizar buf->out = (buf->out + 1) % BUFFER_SIZE
  // 5. Decrementar buf->count
  // 6. Sinalizar condition variable "empty"
  // 7. Liberar o mutex

  printf("[CONSUMER] Removeu %d (buffer count: %d)\n", value, buf->count);
  return value;
}

int buffer_is_empty(circular_buffer_t *buf) {
  // TODO: Implementar verificação thread-safe
  // Dica: precisa adquirir/liberar mutex
  return 0;  // placeholder
}

int buffer_is_full(circular_buffer_t *buf) {
  // TODO: Implementar verificação thread-safe
  // Dica: precisa adquirir/liberar mutex
  return 0;  // placeholder
}
