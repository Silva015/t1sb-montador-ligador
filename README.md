# Montador e Ligador para Linguagem Assembly Inventada

## Descrição Geral

Este projeto implementa um **Montador** e um **Ligador** para uma linguagem Assembly inventada. O Montador realiza o pré-processamento e a conversão de código fonte Assembly em código objeto. O Ligador combina múltiplos arquivos objeto em um único executável, resolvendo referências externas e simbolizando tabelas de definições e uso.

---

## Requisitos

- Compilador C++ compatível com C++17.
- Sistema operacional Linux ou compatível com Makefile.
- Ferramenta Make instalada.

---

## Instruções de Compilação

Para compilar o projeto, execute:

```bash
make
```

Isso gerará o executável `programa` no diretório raiz.

Para limpar os arquivos gerados (binário e objetos intermediários), use:

```bash
make clean
```

---

## Modos de Execução

### Montador

O Montador possui dois modos de execução:

1. Para pré-processar um arquivo Assembly (`.asm`) e gerar um arquivo pré-processado (`.pre`):
    ```bash
    ./montador arquivo.asm
    ```

2. Para montar um arquivo pré-processado (`.pre`) e gerar um arquivo objeto (`.obj`):
    ```bash
    ./montador arquivo.pre
    ```

### Ligador

O Ligador combina dois arquivos objeto (`.obj`) e gera um executável (`.e`):

```bash
./ligador arquivo1.obj arquivo2.obj arquivo_saida.e
```

---

## Formato dos Arquivos

### Arquivo `.asm`
Deve conter as seções `TEXT` e `DATA`, além das diretivas e instruções suportadas:
- Diretivas:
  - `PUBLIC LABEL`: Exporta um símbolo para outros arquivos.
  - `EXTERN LABEL`: Importa um símbolo de outro arquivo.
  - `SPACE N`: Reserva `N` palavras de memória.
  - `CONST N`: Atribui o valor `N` a uma posição de memória.
- Instruções:
  `ADD`, `SUB`, `MUL`, `DIV`, `JMP`, `JMPP`, `JMPN`, `JMPZ`, `COPY`, `LOAD`, `STORE`, `INPUT`, `OUTPUT`, `STOP`.

### Arquivo `.pre`
Versão pré-processada do `.asm`, sem comentários e espaços desnecessários. Exemplo:
```asm
SECTION TEXT
PUBLIC START
START: ADD VAR1
JMP END
SECTION DATA
VAR1: CONST 10
END: STOP
```

### Arquivo `.obj`
Código objeto gerado pelo Montador, contendo:
- Tabelas de Uso (símbolos externos).
- Tabelas de Definição (símbolos públicos).
- Código numérico em uma única linha.

### Arquivo `.e`
Arquivo executável gerado pelo Ligador. Combinação de dois arquivos `.obj`.

---

## Exemplo de Uso

### Código Assembly de Entrada (`exemplo.asm`):
```asm
SECTION TEXT
PUBLIC START
START: ADD VAR1
JMP END
SECTION DATA
VAR1: CONST 10
END: STOP
```

### Comandos:
1. Pré-processar:
    ```bash
    ./montador exemplo.asm
    ```
    Gera: `exemplo.pre`

2. Montar:
    ```bash
    ./montador exemplo.pre
    ```
    Gera: `exemplo.obj`

3. Combinar com outro objeto:
    ```bash
    ./ligador exemplo.obj outro.obj executavel.e
    ```
    Gera: `executavel.e`

---

## Estrutura do Projeto

```
.
├── include/       # Arquivos de cabeçalho
├── src/           # Código fonte
├── Makefile       # Script de compilação
├── README.md      # Instruções do projeto
├── exemplo.asm    # Arquivo Assembly de exemplo
```

---

## Possíveis Erros

- **Rótulo redefinido**: Quando um rótulo é declarado mais de uma vez.
- **Instrução inválida**: Instrução desconhecida ou mal formatada.
- **Referências externas não resolvidas**: Quando um símbolo importado (`EXTERN`) não é encontrado nos arquivos combinados.
- **Diretivas mal formatadas**:
  - `PUBLIC` ou `EXTERN` sem rótulo.
  - `SPACE` ou `CONST` sem valores válidos.

---

## Contato

Desenvolvido por **[Arthur Silva Carneiro]**.  
Email: [tutuscarneiro@gmail.com]
Matrícula: [202006321]

---
```

