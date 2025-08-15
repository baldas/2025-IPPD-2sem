# 🧵 Threads em C com Pthreads – Guia Prático e Didático

Este guia aborda os principais conceitos e dificuldades relacionados ao uso de threads em C com a biblioteca Pthreads.

## 📋 Conteúdo

1. [Interface do pthread_create e pthread_join](#1️⃣-pthread_create--criando-threads)
2. [Comandos Linux mais usados](#5️⃣-comandos-linux-básicos-para-c-e-pthreads)
3. [Conceito de threads e memória compartilhada](#4️⃣-conceito-de-thread)

---

## 1️⃣ pthread_create – Criando threads

### Protótipo da função:
```c
int pthread_create(
    pthread_t *thread_handle,
    const pthread_attr_t *attr,
    void *(*start_routine)(void *),
    void *arg
);
```

### Parâmetros:

#### 1. `pthread_t *thread_handle`
- Onde o identificador da nova thread será armazenado
- Pense como "o número de RG" da thread
- É ponteiro porque pthread_create precisa escrever o identificador dentro dessa variável

#### 2. `const pthread_attr_t *attr`
- Atributos especiais da thread (tamanho de pilha, afinidade com CPU, etc.)
- Quase sempre usamos `NULL` para padrão

#### 3. `void *(*start_routine)(void *)`
- Endereço da função que a thread vai executar
- **Por que `void* func(void*)`?**
  - `void*` (retorno) → a função pode retornar qualquer tipo, representado como ponteiro genérico
  - `void*` (parâmetro) → a função pode receber qualquer tipo de dado, também como ponteiro genérico
  - Formato fixo = pthread_create consegue chamar sua função sem adivinhar sua assinatura

#### 4. `void *arg`
- Um argumento genérico para a função da thread
- **Pode ser:**
  - `NULL` → se a função start_routine não precisar de parâmetros
  - Ponteiro para void, int, float, double, short, etc.
  - Ponteiro para struct
  - Qualquer tipo, desde que condiga com o parâmetro que a função start_routine espera

> ⚠️ **Importante:** Manter compatibilidade entre o tipo de retorno e parâmetro da função start_routine

### 🔍 O que acontece dentro de pthread_create (alto nível)

```c
int pthread_create(pthread_t *th, const pthread_attr_t *attr,
                   void *(*func)(void *), void *arg) {
    // 1. Cria espaço para a pilha da nova thread
    // 2. Configura contexto inicial da CPU
    // 3. Guarda 'func' e 'arg' para usar quando a thread começar
    // 4. Gera um identificador único e escreve em *th
    // 5. Agenda a thread para rodar chamando: func(arg);
}
```

---

## 2️⃣ pthread_join – Esperando a thread terminar

### Protótipo da função:
```c
int pthread_join(pthread_t thread_handle, void **retval);
```

### Parâmetros:

#### 1. `pthread_t thread_handle`
- Identificador da thread que queremos esperar

#### 2. `void **retval`
- Endereço de um ponteiro onde será guardado o valor retornado pela thread
- **Por que ponteiro duplo (`void**`)?**
  - A função da thread retorna `void*`
  - pthread_join precisa escrever o valor retornado dentro de uma variável sua
  - Variável → ponteiro. Para modificar dentro da função → ponteiro para ponteiro

### 🔍 O que acontece dentro de pthread_join (alto nível)

```c
int pthread_join(pthread_t th, void **saida) {
    // 1. Espera até a thread 'th' terminar
    // 2. Captura o retorno da função da thread: void* retorno_thread
    // 3. Escreve dentro de (*saida): *saida = retorno_thread;
}
```

---

## 3️⃣ Exemplo completo

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* minha_funcao(void *arg) {
    int valor = *(int*)arg; // converte de void* para int*
    printf("Thread recebeu: %d\n", valor);
  
    // Usamos malloc pois retornar endereço de variável local pode dar erro
    int *resultado = malloc(sizeof(int)); 
    *resultado = valor * 2; // cálculo
    return resultado; // será recebido no pthread_join
}

int main() {
    pthread_t tid;
    int dado = 21;

    pthread_create(&tid, NULL, minha_funcao, &dado);

    void *retorno;
    pthread_join(tid, &retorno);

    printf("Resultado: %d\n", *(int*)retorno);
    free(retorno);
    
    return 0;
}
```

---

## 4️⃣ Conceito de Thread

- **Thread** = fluxo de execução dentro do mesmo processo

### 🔄 Compartilhado entre threads do mesmo processo:
- Código
- Variáveis globais
- Heap (memória alocada com malloc)
- Arquivos abertos

### 🔒 Privado para cada thread:
- Pilha de execução (variáveis locais)
- Registradores da CPU

> 💡 **Dica:** Threads podem compartilhar dados diretamente, mas é preciso cuidado com sincronização para evitar condições de corrida.

---

## 5️⃣ Comandos Linux básicos para C e Pthreads

| Comando | Descrição |
|---------|-----------|
| `cd pasta` | Entra na pasta |
| `ls` | Lista arquivos |
| `mkdir nome` | Cria pasta |
| `rm arquivo` | Remove arquivo |
| `cp orig dest` | Copia arquivo |
| `mv orig dest` | Move/renomeia arquivo |
| `gcc arquivo.c -o programa -pthread` | Compila com suporte a Pthreads |
| `./programa` | Executa programa |

---

## 🚀 Compilação e Execução

```bash
# Compilar o programa
gcc meu_programa.c -o meu_programa -pthread

# Executar o programa
./meu_programa
```

> ⚠️ **Importante:** Sempre use a flag `-pthread` ao compilar programas que usam Pthreads.

---

## 📚 Recursos Adicionais

- [Documentação oficial POSIX Threads](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)

---

**Desenvolvido para auxiliar no aprendizado de programação concorrente em C** 🎓