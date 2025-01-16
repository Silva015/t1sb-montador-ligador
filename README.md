# [Repositório no GitHub](https://github.com/Silva015/t1sb-montador-ligador)

## Montador e Ligador para Linguagem Assembly

### Descrição Geral

Este projeto implementa um Montador e um Ligador para uma linguagem Assembly inventada, conforme especificado no Trabalho Prático 1 de Software Básico.

- **Montador** realiza a montagem de programas Assembly em dois modos:
  - **Pré-processamento**: Gera um arquivo `.pre` a partir de um código Assembly `.asm`.
  - **Montagem**: Gera um arquivo `.obj` a partir de um código pré-processado `.pre`.
- **Ligador** combina dois arquivos `.obj` em um único executável `.e`, resolvendo referências externas e gerando o código final (apenas para programas com `BEGIN` e `END`).

### Requisitos

- Sistema operacional: Linux ou compatível.
- Compilador g++ com suporte a C++17.
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

Para combinar dois arquivos objeto `.obj` e gerar um executável `.e` (apenas para códigos com `BEGIN` e `END`):

```sh
./ligador arquivo1.obj arquivo2.obj
```

O executável gerado terá o nome `arquivo1.e`.

### Exemplos Práticos

#### Caso 1: Código sem `BEGIN` e `END`

**Código Fonte (codigo.asm)**

```assembly
SECTION TEXT
ROT: INPUT N1
COPY N2,N1 ;isso e um comentario
STOP
SECTION DATA
N1: SPACE
N2: SPACE
```

**Código Pré-processado (codigo.pre)**

```assembly
SECTION TEXT
ROT: INPUT N1
COPY N2, N1
STOP
SECTION DATA
N1: SPACE
N2: SPACE
```

**Código Montado (codigo.obj)**

```
12 6 9 7 6 14 0 0
```

Códigos que não possuem as diretivas `BEGIN` e `END` não podem ser ligados, então seu último estágio é o `.obj`.

#### Caso 2: Código com `BEGIN` e `END` (Ligação Possível)

**Módulo A (mod1.asm)**

```assembly
;Código fonte do módulo A:
SECTION TEXT
MOD_A: BEGIN
Y: EXTERN
MOD_B: EXTERN
PUBLIC VAL
PUBLIC L1
INPUT Y
LOAD VAL
ADD Y
STORE Y + 2
JMPP MOD_B
L1: STOP
END
SECTION DATA
VAL: CONST 5
```

**Código Pré-processado (mod1.pre)**

```assembly
SECTION TEXT
MOD_A: BEGIN
Y: EXTERN
MOD_B: EXTERN
PUBLIC VAL
PUBLIC L1
INPUT Y
LOAD VAL
ADD Y
STORE Y + 2
JMPP MOD_B
L1: STOP
END
SECTION DATA
VAL: CONST 5
```

**Código Montado (mod1.obj)**

```
D, VAL 11
D, L1 10
D, MOD_A 0
U, MOD_B 9
U, Y  7
U, Y 1
U, Y 5
R, 0 0 0 1 0 0 0 0 0 0 0 0 
12 0 10 11 1 0 11 2 7 0 14 5 
```

**Módulo B (mod2.asm)**

```assembly
;Código fonte do módulo B:
SECTION TEXT
MOD_B: BEGIN
VAL: EXTERN
L1: EXTERN
PUBLIC Y
PUBLIC MOD_B
OUTPUT Y
OUTPUT VAL
OUTPUT Y + 2
JMP L1
END
SECTION DATA
Y: SPACE 3
```

**Código Pré-processado (mod2.pre)**

```assembly
SECTION TEXT
MOD_B: BEGIN
VAL: EXTERN
L1: EXTERN
PUBLIC Y
PUBLIC MOD_B
OUTPUT Y
OUTPUT VAL
OUTPUT Y + 2
JMP L1
END
SECTION DATA
Y: SPACE 3
```

**Código Montado (mod2.obj)**

```
D, Y 8
D, MOD_B 0
U, Y  5
U, L1 7
U, VAL 3
R, 0 1 0 0 0 0 0 0 0 0 0 
13 8 13 0 13 2 5 0 0 0 0 
```

**Código Ligado (mod1.e)**

Após ligar `mod1.obj` e `mod2.obj`, o código executável gerado é:

```
12 20 10 11 1 20 11 22 7 12 14 5 13 20 13 11 13 22 5 10 0 0 0 
```

- **Aluno**: Arthur Silva Carneiro
- **Matrícula**: 202006321
- **Email**: tutuscarneiro@gmail.com
