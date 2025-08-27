# 🧵 Exercício: Problema Produtor–Consumidor com Buffer Circular

Este exercício tem como objetivo praticar **programação concorrente** em C, utilizando **threads (pthread)**, **mutexes** e **variáveis de condição** para resolver o clássico problema **Produtor–Consumidor**.

---

## 🎯 Objetivo

Você deve implementar um programa em C que:

1. Crie **produtores** e **consumidores** (threads).
2. Use um **buffer circular de tamanho fixo** para compartilhar dados entre eles.
3. Utilize **exclusão mútua** e **sincronização** para evitar condições de corrida.
4. Simule atrasos de produção e consumo para observar a interleaving entre threads.
5. Exiba **estatísticas de execução** ao final.

---

## 🔄 O que é um Buffer Circular?

Um **buffer circular** (ou **fila circular**) é uma estrutura de dados de tamanho fixo que funciona como uma fila (**FIFO – First In, First Out**), mas organizada de forma **circular**:

- Ele tem duas "pontas":
  - **`in`** → posição onde o próximo **produtor** irá inserir.
  - **`out`** → posição de onde o próximo **consumidor** irá retirar.
- O **índice `in` e `out`** "dão a volta" no final do array.  
  Exemplo: se o buffer tem tamanho `5`, após `in = 4`, o próximo item entra em `in = 0`.  
  Isso é feito com a operação:  
  ```c
  in = (in + 1) % BUFFER_SIZE
  out = (out + 1) % BUFFER_SIZE
  ```
- Um contador (`count`) controla quantos elementos existem no buffer no momento.

📌 **Analogia:** imagine um **carrossel de 5 cadeiras**.  
- O produtor sempre senta a próxima pessoa na **próxima cadeira vazia** (`in`).  
- O consumidor sempre pega a próxima pessoa sentada na **cadeira seguinte** (`out`).  
- Quando chegam no fim, voltam para a primeira cadeira.  
- O carrossel nunca ultrapassa sua lotação (não pode ter mais que 5 pessoas).

---

## ⚙️ Como funciona neste exercício?

- **Produtores**:
  - Criam itens (ex.: valores inteiros únicos).
  - Colocam esses itens no buffer circular na posição `in`.
  - Se o buffer estiver **cheio**, precisam esperar até que um consumidor retire algo.

- **Consumidores**:
  - Retiram itens do buffer na posição `out`.
  - Processam o item (simulação com `usleep`).
  - Se o buffer estiver **vazio**, precisam esperar até que um produtor insira algo.

📌 Assim, produtores e consumidores **compartilham o mesmo buffer**, mas **não acessam as mesmas posições ao mesmo tempo**, pois os índices `in` e `out` controlam os papéis de cada um.

---

## 📂 Estrutura do Código

O projeto está dividido em três arquivos:

- `main.c` → código principal, cria e gerencia threads.
- `buffer.h` → definição do buffer circular e suas funções.
- `buffer.c` → implementação das funções do buffer.

---

## 🚦 Passos do Exercício

### 1. Estrutura do Buffer (`buffer.h`)
- Declarar:
  - Um **mutex** para exclusão mútua.
  - Duas **variáveis de condição**:
    - `not_full` → usada pelos produtores para esperar espaço no buffer.
    - `not_empty` → usada pelos consumidores para esperar itens disponíveis.

---

### 2. Inicialização e Destruição (`buffer.c`)
- Implementar `buffer_init`:
  - Inicializar `in`, `out`, `count` em `0`.
  - Inicializar o mutex e as variáveis de condição (`pthread_mutex_init`, `pthread_cond_init`).

- Implementar `buffer_destroy`:
  - Destruir o mutex e as variáveis de condição (`pthread_mutex_destroy`, `pthread_cond_destroy`).

---

### 3. Inserção de Itens – Produtor (`buffer_put`)
Passos:
1. Trancar o mutex (`pthread_mutex_lock`).
2. Enquanto o buffer estiver cheio, esperar (`pthread_cond_wait` em `not_full`).
3. Inserir o valor em `buffer[in]`.
4. Atualizar `in = (in + 1) % BUFFER_SIZE` e `count++`.
5. Sinalizar que o buffer não está vazio (`pthread_cond_signal` em `not_empty`).
6. Liberar o mutex (`pthread_mutex_unlock`).

---

### 4. Remoção de Itens – Consumidor (`buffer_get`)
Passos:
1. Trancar o mutex.
2. Enquanto o buffer estiver vazio, esperar (`pthread_cond_wait` em `not_empty`).
3. Ler o valor em `buffer[out]`.
4. Atualizar `out = (out + 1) % BUFFER_SIZE` e `count--`.
5. Sinalizar que o buffer não está cheio (`pthread_cond_signal` em `not_full`).
6. Liberar o mutex.
7. Retornar o valor removido.

---

### 5. Verificações Auxiliares
- `buffer_is_empty` → retorna se `count == 0` (usar mutex para thread safety).
- `buffer_is_full` → retorna se `count == BUFFER_SIZE` (usar mutex).

---

### 6. Funções das Threads (`main.c`)
- **Produtor (`producer`)**:
  - Gera `items` valores únicos (exemplo: `id * 1000 + i`).
  - Insere cada valor no buffer (`buffer_put`).
  - Faz `usleep(delay_ms * 1000)` para simular tempo de produção.

- **Consumidor (`consumer`)**:
  - Retira `items` valores do buffer (`buffer_get`).
  - Faz `usleep(delay_ms * 1000)` para simular tempo de consumo.

---

### 7. Função Principal (`main`)
- Ler parâmetros da linha de comando:
  ```bash
  ./main <#producers> <#consumers> <#items> <producer_delay_ms> <consumer_delay_ms>
  ```

---

## ✅ Critérios de Conclusão
Seu programa está correto se:
- Produtores e consumidores conseguem trabalhar em paralelo sem corromper o buffer.
- O buffer nunca excede o limite definido.
- Não ocorre **deadlock**.
- As estatísticas finais estão coerentes com os parâmetros fornecidos.

---

## 🔧 Dicas
- Use `printf` dentro das funções `buffer_put` e `buffer_get` para acompanhar a execução.
- Varie o número de produtores, consumidores e delays para observar diferentes interações.
- Teste casos:
  - Mais produtores que consumidores.
  - Mais consumidores que produtores.
  - Nenhum delay (máxima concorrência).
  - Delays altos (produção ou consumo lento).

---