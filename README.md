# Montador e Ligador para Linguagem Assembly

## Descrição Geral

Este projeto implementa um **Montador** e um **Ligador** para uma linguagem Assembly inventada, conforme especificado no Trabalho Prático 1 de Software Básico.

- O **Montador** realiza a montagem de programas Assembly em dois modos:
  - **Pré-processamento:** Gera um arquivo `.pre` a partir de um código Assembly `.asm`.
  - **Montagem:** Gera um arquivo `.obj` a partir de um código pré-processado `.pre`.
- O **Ligador** combina dois arquivos `.obj` em um único executável `.e`, resolvendo referências externas e gerando o código final.

---

## Requisitos

- Sistema operacional: **Linux** ou compatível.
- Compilador **GCC** com suporte a **C++17**.
- Ferramenta **Make** para gerenciamento de compilação.

---

## Compilação

Para compilar o projeto, utilize o comando:

```bash
make
```

Isso gera os executáveis `montador` e `ligador` no diretório raiz.

Para limpar os arquivos compilados, utilize:

```bash
make clean
```

---

## Modos de Execução

### Montador

#### Pré-processamento

Para gerar um arquivo pré-processado `.pre` a partir de um código Assembly `.asm`:

```bash
./montador arquivo.asm
```

#### Montagem

Para gerar um arquivo objeto `.obj` a partir de um código pré-processado `.pre`:

```bash
./montador arquivo.pre
```

### Ligador

Para combinar dois arquivos objeto `.obj` e gerar um executável `.e`:

```bash
./ligador arquivo1.obj arquivo2.obj
```

O executável gerado terá o nome `arquivo1.e`.

---

## Estrutura dos Arquivos

### `.asm` - Código Assembly

- **Seções:**
  - `SECTION TEXT`: Contém o código executável.
  - `SECTION DATA`: Contém os dados necessários.
- **Diretivas Suportadas:**
  - `PUBLIC`: Exporta símbolos para outros módulos.
  - `EXTERN`: Importa símbolos de outros módulos.
  - `CONST`: Define um valor constante.
  - `SPACE`: Reserva espaço de memória.
- **Instruções Suportadas:**
  - Aritméticas: `ADD`, `SUB`, `MULT`, `DIV`.
  - Saltos: `JMP`, `JMPP`, `JMPN`, `JMPZ`.
  - Entrada/Saída: `INPUT`, `OUTPUT`.
  - Controle: `STOP`.

### `.pre` - Código Pré-processado

- Arquivo limpo gerado pelo pré-processador, sem comentários e com a seção `TEXT` antes da seção `DATA`.

### `.obj` - Código Objeto

- Contém:
  - **Tabela de Uso:** Símbolos externos referenciados.
  - **Tabela de Definição:** Símbolos públicos definidos no módulo.
  - **Tabela de Realocação:** Indica posições que precisam de ajustes.
  - **Código Máquina:** Representação numérica do programa.

### `.e` - Executável

- Arquivo único gerado pelo Ligador contendo o código combinado de dois módulos.

---

## Exemplo de Execução

### Código Assembly (`mod1.asm`)

```asm
SECTION TEXT
PUBLIC START
START: ADD VAR1
JMP END
SECTION DATA
VAR1: CONST 10
END: STOP
```

### Passos

1. Pré-processar:
    ```bash
    ./montador mod1.asm
    ```
    Gera: `mod1.pre`

2. Montar:
    ```bash
    ./montador mod1.pre
    ```
    Gera: `mod1.obj`

3. Ligar com outro módulo (`mod2.obj`):
    ```bash
    ./ligador mod1.obj mod2.obj
    ```
    Gera: `mod1.e`

---

## Detecção de Erros

O programa detecta os seguintes erros:

1. **Montador:**
   - Rótulos redefinidos.
   - Rótulos ausentes ou inválidos.
   - Instruções ou diretivas inválidas.
   - Número incorreto de operandos.

2. **Ligador:**
   - Símbolos externos não resolvidos.
   - Tabelas de uso ou definições mal formatadas.

---

## Estrutura do Projeto

```
.
├── include/       # Cabeçalhos do projeto
├── src/           # Código fonte
├── exemplos/      # Exemplos de código Assembly
├── Makefile       # Arquivo de compilação
├── README.md      # Instruções do projeto
```

---

## Contato

- **Aluno:** Arthur Silva Carneiro  
- **Matrícula:** 202006321  
- **Email:** tutuscarneiro@gmail.com  

---

### Especificações Atendidas

1. **Montador:**
   - Aceita **maiúsculas e minúsculas**.
   - Remove **comentários** e **espaços desnecessários**.
   - Suporta instruções e diretivas conforme especificado.

2. **Ligador:**
   - Suporta até dois módulos.
   - Gera um executável compatível com o simulador.

**O projeto foi testado e está funcional conforme os exemplos fornecidos.**