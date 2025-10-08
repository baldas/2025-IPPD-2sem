#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Variáveis globais para armazenar o rank (ID) do processo
// e o tamanho (número total de processos) do comunicador MPI.
int rank, size;

// Parâmetros do problema, lidos da linha de comando.
// cutoff: O número máximo de iterações para determinar se um ponto converge.
// min_x, max_x, min_y, max_y: Coordenadas que definem a janela de visualização.
// dx, dy: A largura e altura da janela de visualização (calculado a partir das coordenadas).
int cutoff;
double min_x, max_x, min_y, max_y, dx, dy;

/**
 * @brief Calcula o módulo de um número complexo (distância da origem).
 * O número é representado por suas partes real (x) e imaginária (y).
 */
double modulus(double x, double y) {
  return sqrt(x * x + y * y);
}

/**
 * @brief Eleva um número complexo ao quadrado.
 * A operação (x + iy)^2 resulta em (x^2 - y^2) + i(2xy).
 * Os ponteiros *x e *y são atualizados com o novo valor.
 */
void self_mul(double *x, double *y) {
  double ox = *x * *x - *y * *y;
  double oy = *x * *y + *y * *x;
  *x = ox;
  *y = oy;
}

/**
 * @brief Função principal de cálculo.
 * Para cada ponto na lista `points`, calcula as iterações de Mandelbrot
 * até que o módulo exceda 2 ou o `cutoff` seja atingido.
 * O resultado (número de iterações) é salvo no array `mset`.
 */
void compute_mandelbrot(double *points, int npts, int mset[]) {
  // Itera sobre cada ponto fornecido
  for (int i = 0; i < npts; ++i) {
    double px, py;
    px = points[i * 2];      // Coordenada x do ponto
    py = points[i * 2 + 1];  // Coordenada y do ponto

    int iteration = 0;
    double zx = 0;
    double zy = 0;

    // A iteração principal: z_{n+1} = z_n^2 + c
    // onde z começa em 0 e c é o ponto (px, py).
    while (iteration < cutoff) {
      self_mul(&zx, &zy);
      zx += px;
      zy += py;
      double mod = modulus(zx, zy);

      // Se o módulo for > 2, o ponto "escapou" e não pertence ao conjunto.
      if (mod > 2.0f) break;

      iteration++;
    }

    // Se a iteração atingiu o limite (cutoff), consideramos que o ponto
    // pertence ao conjunto (marcado com -1). Caso contrário, salvamos
    // o número de iterações que levou para escapar.
    if (iteration == cutoff)
      mset[i] = -1;
    else
      mset[i] = iteration;
  }
}

int main(int argc, char **argv) {
  // INSTRUÇÃO: Inicialize o ambiente MPI.
  // Todo programa MPI deve chamar esta função antes de qualquer outra função MPI.
  MPI_Init(&argc, &argv);

  double start, stop;

  // Leitura dos parâmetros da linha de comando.
  // Este trecho já está pronto e não precisa de modificação.
  char *stopstring;
  min_x = strtod(argv[1], &stopstring);
  max_x = strtod(argv[2], &stopstring);
  min_y = strtod(argv[3], &stopstring);
  max_y = strtod(argv[4], &stopstring);
  dx = max_x - min_x;
  dy = max_y - min_y;
  cutoff = atoi(argv[5]);
  int p_count = atoi(argv[6]);  // Pontos em uma dimensão (grid total é p_count * p_count)

  // INSTRUÇÃO: Obtenha o rank do processo e o tamanho do comunicador.
  // O rank é o ID único do processo (de 0 a size-1).
  // O size é o número total de processos (-np X na linha de comando).
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  /****************************************************************************
   * DESAFIO DE PARALELIZAÇÃO:
   * O código abaixo é puramente sequencial. APENAS UM PROCESSO (rank 0)
   * faz todo o trabalho. Sua tarefa é modificar esta seção para que o
   * trabalho seja distribuído entre todos os processos disponíveis.
   ****************************************************************************/

  // PONTO DE MELHORIA 1: Geração de Pontos
  // Atualmente, um único array `points` é alocado e preenchido com
  // TODAS as coordenadas. Isso consome muita memória e não é escalável.
  //
  // SUA TAREFA: Modifique esta seção. Em vez de um processo gerar todos
  // os pontos, cada processo deve gerar APENAS a porção de pontos pela
  // qual ele será responsável. Calcule um `start_index` e um `local_npts`
  // para cada processo.
  double *points = NULL;
  points = (double *)malloc((p_count * p_count * 2) * sizeof(double));

  for (int yp = 0; yp < p_count; ++yp) {
    double py = min_y + dy * yp / p_count;
    for (int xp = 0; xp < p_count; ++xp) {
      double px = min_x + dx * xp / p_count;
      int lid = yp * p_count * 2 + xp * 2;
      points[lid] = px;
      points[lid + 1] = py;
    }
  }

  // Barreira de sincronização. Útil para garantir que a medição de tempo
  // comece de forma justa para todos os processos.
  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  // O número total de pontos
  int npts = p_count * p_count;

  // PONTO DE MELHORIA 2: Alocação de Memória para Resultados
  // Assim como `points`, o array `mset` também é alocado para conter
  // TODOS os resultados. Isso deve ser mudado.
  //
  // SUA TAREFA: Cada processo deve alocar um `local_mset` com tamanho
  // suficiente apenas para os seus pontos locais (`local_npts`).
  int *mset = malloc(npts * sizeof(int));

  // PONTO DE MELHORIA 3: Computação
  // A função `compute_mandelbrot` é chamada com a lista completa de pontos.
  //
  // SUA TAREFA: Chame esta função com a lista de pontos LOCAL de cada
  // processo (`local_points`, `local_npts`, `local_mset`).
  compute_mandelbrot(points, npts, mset);

  // PONTO DE MELHORIA 4: Coleta de Resultados
  // Após o cálculo local, os resultados estão espalhados entre os processos.
  //
  // SUA TAREFA: Use uma operação de comunicação coletiva, como `MPI_Gather`,
  // para que o processo de rank 0 receba os `local_mset` de todos os
  // outros processos e os junte em um array final `global_mset`.

  MPI_Barrier(MPI_COMM_WORLD);
  stop = MPI_Wtime();

  // O processo rank 0 imprime o tempo de execução.
  if (rank == 0) {
    printf("Tempo gasto: %g s\n", stop - start);
  }

  // PONTO DE MELHORIA 5: Escrita do Arquivo
  // Apenas o processo rank 0, que agora possui todos os resultados no
  // `global_mset`, deve executar esta seção.
  if (rank == 0) {
    FILE *fd = fopen("mandel.out", "w+");
    for (int yp = 0; yp < p_count; ++yp) {
      for (int xp = 0; xp < p_count; ++xp)
        fprintf(fd, "%d ", mset[yp * p_count + xp]);
      fprintf(fd, "\n");
    }
    fclose(fd);
  }

  // Limpeza da memória. Lembre-se de liberar `local_points`, `local_mset`
  // e, no rank 0, o `global_mset`.
  free(points);
  free(mset);

  // INSTRUÇÃO: Finalize o ambiente MPI.
  // Esta função deve ser a última chamada MPI em seu programa.
  MPI_Finalize();
  return 0;
}