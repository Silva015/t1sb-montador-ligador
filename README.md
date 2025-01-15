Aqui está a prévia do README no formato Markdown renderizado como seria exibido, por exemplo, no GitHub:
# [Repositório no GitHub](https://github.com/Silva015/t1sb-montador-ligador)

## Montador e Ligador para Linguagem Assembly

### Descrição Geral

Este projeto implementa um Montador e um Ligador para uma linguagem Assembly inventada, conforme especificado no Trabalho Prático 1 de Software Básico.

- **Montador** realiza a montagem de programas Assembly em dois modos:
  - **Pré-processamento**: Gera um arquivo `.pre` a partir de um código Assembly `.asm`.
  - **Montagem**: Gera um arquivo `.obj` a partir de um código pré-processado `.pre`.
- **Ligador** combina dois arquivos `.obj` em um único executável `.e`, resolvendo referências externas e gerando o código final.

### Requisitos

- Sistema operacional: Linux ou compatível.
- Compilador GCC com suporte a C++17.
- Ferramenta Make para gerenciamento de compilação.

### Compilação

Para compilar o projeto, utilize o comando:

```sh
make
```

Isso gera os executáveis `montador` e `ligador` no diretório raiz.

Para limpar os arquivos compilados, utilize:

```sh
make clean
```

### Modos de Execução

#### Montador

**Pré-processamento**

Para gerar um arquivo pré-processado `.pre` a partir de um código Assembly `.asm`:

```sh
./montador arquivo.asm
```

**Montagem**

Para gerar um arquivo objeto `.obj` a partir de um código pré-processado `.pre`:

```sh
./montador arquivo.pre
```

#### Ligador

Para combinar dois arquivos objeto `.obj` e gerar um executável `.e`:

```sh
./ligador arquivo1.obj arquivo2.obj
```

O executável gerado terá o nome `arquivo1.e`.

### Estrutura dos Arquivos

- **`.asm` - Código Assembly**
  - Seções:
    - `SECTION TEXT`: Contém o código executável.
    - `SECTION DATA`: Contém os dados necessários.
  - Diretivas Suportadas:
    - `PUBLIC`: Exporta símbolos para outros módulos.
    - `EXTERN`: Importa símbolos de outros módulos.
    - `CONST`: Define um valor constante.
    - `SPACE`: Reserva espaço de memória.
    - `BEGIN`: Define o ponto inicial de execução do programa. Deve ser utilizado para identificar a instrução inicial dentro de `SECTION TEXT`.
  - Instruções Suportadas:
    - Aritméticas: `ADD`, `SUB`, `MUL`, `DIV`. (Nota: `MUL` é utilizado em vez de `MULT`.)
    - Saltos: `JMP`, `JMPP`, `JMPN`, `JMPZ`.
    - Entrada/Saída: `INPUT`, `OUTPUT`.
    - Controle: `STOP`.

- **`.pre` - Código Pré-processado**
  - Arquivo limpo gerado pelo pré-processador, sem comentários e com a seção `TEXT` antes da seção `DATA`.

- **`.obj` - Código Objeto**
  - Contém:
    - Tabela de Uso: Símbolos externos referenciados.
    - Tabela de Definição: Símbolos públicos definidos no módulo.
    - Tabela de Realocação: Indica posições que precisam de ajustes.
    - Código Máquina: Representação numérica do programa.

- **`.e` - Executável**
  - Arquivo único gerado pelo Ligador contendo o código combinado de dois módulos.

### Exemplo de Execução

**Código Assembly (`mod1.asm`)**

```assembly
SECTION TEXT
BEGIN START
PUBLIC START
START: ADD VAR1
JMP END
SECTION DATA
VAR1: CONST 10
END: STOP
```

**Passos**

1. Pré-processar:

    ```sh
    ./montador mod1.asm
    ```

    Gera: `mod1.pre`

2. Montar:

    ```sh
    ./montador mod1.pre
    ```

    Gera: `mod1.obj`

3. Ligar com outro módulo (`mod2.obj`):

    ```sh
    ./ligador mod1.obj mod2.obj
    ```

    Gera: `mod1.e`

### Contato

- **Aluno**: Arthur Silva Carneiro
- **Matrícula**: 202006321
- **Email**: tutuscarneiro@gmail.com