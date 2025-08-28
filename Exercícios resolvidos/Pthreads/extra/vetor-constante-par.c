#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

long N;
float constante;
long num_threads;
float *A;

// nossa thread
void *thread(void *arg)
{
  long tid = (long)arg;

  long block_size = N / num_threads;

  long i_inicial, i_final;

  // t = 4, N = 100
  // t0 - 0  - 24
  // t1 - 25 - 49
  // t2 - 50 - 74
  // t3 - 75 - 99
  i_inicial = tid*block_size;
  i_final = i_inicial+block_size;

  // se eu for a última, pego os elementos restantes
  if (tid == num_threads-1) {
    i_final = N;
  }

  // kernel para paralelizar
  for (size_t i = i_inicial; i < i_final; i++) {
    A[i] = A[i] * constante;
  }
}


int main(int argc, char **argv) 
{
  struct timeval start, stop;
  pthread_t *thread_handles;


  if (argc < 4) {
    fprintf(stderr, "Uso: %s <tamanho> <constante> <num-threads>\n", argv[0]);
    return 1;
  }

  N = strtoull(argv[1], NULL, 10);
  constante = atof(argv[2]);
  num_threads = strtoull(argv[3], NULL, 10);
  
  thread_handles = malloc(num_threads*sizeof(pthread_t));

  A = malloc(N * sizeof(float));
  if (!A) {
    perror("malloc");
    return 1;
  }

  // Inicializa o vetor com valores de exemplo
  for (size_t i = 0; i < N; i++) {
    A[i] = (float)i;
  }

  // Multiplica cada elemento pela constante
  gettimeofday(&start, NULL);

  /* FORK Cria as threads informadas na linha de comando */
  long i;
  for (i=0; i<num_threads; i++) {
    if (pthread_create(&thread_handles[i], NULL, thread, (void *)i) != 0) {
      fprintf(stderr, "Nao consegui criar a thread\n");
      exit(-1);
    }
  }

  /* JOIN */
  for (i=0; i<num_threads; i++)
  {
    pthread_join(thread_handles[i], NULL);
  }

  gettimeofday(&stop, NULL);
  
  free(thread_handles);

  double temp = \
    (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
    ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));

  fprintf(stdout, "Tempo gasto = %g ms\n", temp);

  // Exibe os primeiros elementos
  for (size_t i = 0; i < (N < 10 ? N : 10); i++) {
    printf("A[%zu] = %.2f\n", i, A[i]);
  }

  free(A);
  return 0;
}
