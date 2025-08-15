# 🕒 Medindo Tempo de Execução em C — Do Básico ao Avançado

Este documento apresenta **4 formas diferentes** de medir tempo de execução em C, indo do mais simples ao mais genérico.  
Cada abordagem será explicada com **código, comentários e prós/contras**.

---

## 1️⃣ `gettimeofday` simples

A forma mais direta de medir tempo em C é usar a função `gettimeofday()` da `<sys/time.h>`:

```c
#include <stdio.h>
#include <sys/time.h>

void tarefa_lenta() {
    for (volatile int i = 0; i < 100000000; i++);
}

int main() {
    struct timeval start, stop;

    gettimeofday(&start, NULL); // pega o tempo atual
    tarefa_lenta();             // executa a função a medir
    gettimeofday(&stop, NULL);  // pega o tempo novamente

    long sec  = stop.tv_sec - start.tv_sec;
    long usec = stop.tv_usec - start.tv_usec;
    double elapsed = sec + usec * 1e-6;

    printf("Tempo decorrido: %.6f segundos\n", elapsed);

    return 0;
}
```

### Como funciona?
- `gettimeofday` retorna o tempo em segundos (`tv_sec`) e microssegundos (`tv_usec`) desde 1º jan 1970 (epoch).
- Fazemos a diferença entre **antes** e **depois** para obter o tempo gasto.

✅ **Prós:**  
- Simples e fácil de entender.  
- Não envolve ponteiros, macros ou varargs.

❌ **Contras:**  
- Repete código para cada função que você quiser medir.  
- Pouca reutilização.

---

## 2️⃣ Callback com `void*`

Podemos criar uma função genérica `exec()` que recebe **um ponteiro para função** e executa a medição.

```c
#include <stdio.h>
#include <sys/time.h>

// Definição: função que recebe void* e retorna void*
typedef void* (*func_ptr_t)(void*);

void* exec(func_ptr_t func, void* arg) {
    struct timeval start, stop;

    gettimeofday(&start, NULL);
    void* result = func(arg); // executa callback
    gettimeofday(&stop, NULL);

    long sec  = stop.tv_sec - start.tv_sec;
    long usec = stop.tv_usec - start.tv_usec;
    double elapsed = sec + usec * 1e-6;

    printf("Tempo decorrido: %.6f segundos\n", elapsed);

    return result;
}

// Funções exemplo
void* tarefa(void* arg) {
    int n = *(int*)arg;
    for (volatile int i = 0; i < n; i++);
    return NULL;
}

int main() {
    int loops = 100000000;
    exec(tarefa, &loops);
    return 0;
}
```

### O que é `void*`?
- É um **ponteiro genérico** que pode apontar para qualquer tipo.  
- Precisa de cast para o tipo correto ao acessar.

✅ **Prós:**  
- Reutilizável: qualquer função com assinatura `void* f(void*)` pode ser medida.  
- Suporta passagem de argumento único genérico.

❌ **Contras:**  
- Limita a assinatura da função medida (`void* (void*)`).  
- Necessário fazer cast de tipos, perdendo **segurança de tipo**.

---

## 3️⃣ Funções Variádicas com `<stdarg.h>`

Agora, tornamos a função de medição **mais genérica** usando *varargs*.  
O cabeçalho `<stdarg.h>` permite receber número variável de argumentos.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>

// Ponteiro para função que aceita va_list
typedef void* (*varfunc_t)(va_list args);

void* measure_time(varfunc_t func, ...) {
    struct timeval start, stop;
    va_list args;
    va_start(args, func);

    gettimeofday(&start, NULL);
    void* result = func(args);
    gettimeofday(&stop, NULL);

    va_end(args);

    long sec  = stop.tv_sec - start.tv_sec;
    long usec = stop.tv_usec - start.tv_usec;
    double elapsed = sec + usec * 1e-6;

    printf("Tempo decorrido: %.6f segundos\n", elapsed);

    return result;
}

// Funções exemplo
void* soma_inteiros(va_list args) {
    int a = va_arg(args, int);
    int b = va_arg(args, int);
    int* res = malloc(sizeof(int));
    *res = a + b;
    return res;
}

void* concat_strings(va_list args) {
    const char* s1 = va_arg(args, const char*);
    const char* s2 = va_arg(args, const char*);
    char* res = malloc(strlen(s1) + strlen(s2) + 1);
    sprintf(res, "%s%s", s1, s2);
    return res;
}

int main() {
    int* r1 = measure_time(soma_inteiros, 42, 58);
    printf("Soma = %d\n", *r1);
    free(r1);

    char* r2 = measure_time(concat_strings, "Hello, ", "World!");
    printf("Concat = %s\n", r2);
    free(r2);

    return 0;
}
```

### O que é `<stdarg.h>`?
- `va_list` → tipo usado para armazenar parâmetros variádicos.
- `va_start` → inicializa a lista de argumentos.
- `va_arg` → lê o próximo argumento, informando o tipo.
- `va_end` → finaliza o uso.

✅ **Prós:**  
- Aceita qualquer quantidade e tipo de argumentos.  
- Mais flexível que `void*`.

❌ **Contras:**  
- Complexidade maior.  
- Perde **checagem de tipo** (erros só aparecem em runtime).

---

## 4️⃣ Macro Variádica com `__VA_ARGS__` e `do { } while(0)`

Macros variádicas permitem escrever algo como:

```c
#define MEASURE_TIME(func, ...)  ...
```

E capturar qualquer lista de argumentos.  
Usamos o truque `do { ... } while(0)` para tornar a macro segura em `if/else`.

```c
#include <stdio.h>
#include <sys/time.h>

#define MEASURE_TIME(func, ...)                                       \
    do {                                                              \
        struct timeval _start, _stop;                                 \
        gettimeofday(&_start, NULL);                                  \
        func(__VA_ARGS__);                                            \
        gettimeofday(&_stop, NULL);                                   \
        long _sec  = _stop.tv_sec - _start.tv_sec;                    \
        long _usec = _stop.tv_usec - _start.tv_usec;                  \
        double _elapsed = _sec + _usec * 1e-6;                        \
        printf("[TIME] %s levou %.6f segundos\n", #func, _elapsed);   \
    } while (0)

// Funções exemplo
void tarefa_intensa(int n) {
    for (volatile int i = 0; i < n; i++);
}

int soma(int a, int b) {
    return a + b;
}

int main() {
    MEASURE_TIME(tarefa_intensa, 100000000);

    MEASURE_TIME(printf, "Soma = %d\n", soma(5, 7));

    return 0;
}
```

### Como funciona?
- `__VA_ARGS__` → substituído pela lista de argumentos.
- `#func` → transforma o nome da função em *string literal* para imprimir.
- `do { ... } while(0)` → faz com que a macro seja um **bloco único**, seguro para `if/else`.

✅ **Prós:**  
- Não altera assinatura da função medida.  
- Simples de usar: parece função normal.  
- Não precisa de `<stdarg.h>`.

❌ **Contras:**  
- É apenas substituição de texto → erros de tipo só aparecem na função chamada.  
- Difícil de debugar quando macro é muito grande.

---

## 📊 Comparativo

| Método               | Flexibilidade | Segurança de Tipo | Complexidade | Reutilização |
|----------------------|--------------|-------------------|--------------|--------------|
| `gettimeofday` simples | Baixa        | Alta              | Baixa        | Baixa        |
| Callback `void*`     | Média        | Baixa (casts)     | Média        | Alta         |
| `<stdarg.h>`         | Alta         | Baixa             | Alta         | Alta         |
| Macro `__VA_ARGS__`  | Alta         | Igual à função    | Média        | Alta         |

---

## 💡 Conclusão

- Para código rápido e simples → **Método 1**.  
- Para reaproveitar lógica de medição → **Método 2**.  
- Para máxima flexibilidade (multi-args genérico) → **Método 3**.  
- Para sintaxe limpa e sem alterar funções existentes → **Método 4**.
