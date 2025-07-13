# MyPascalCompiler

Claro, aqui está uma sugestão de `README.md` para o seu projeto, com base nos arquivos que você enviou.

-----

# MyPascalCompiler

Um compilador simples para um subconjunto da linguagem de programação Pascal, desenvolvido em C. Este projeto foi criado como um exercício acadêmico para demonstrar os conceitos fundamentais da construção de compiladores.

## Funcionalidades

  * **Analisador Léxico**: Converte o código-fonte de um arquivo `.pas` em uma sequência de tokens.
  * **Analisador Sintático (Parser)**: Verifica se a sequência de tokens segue a gramática da linguagem Pascal.
  * **Analisador Semântico**: Utiliza uma tabela de símbolos para verificar a semântica do código, como a declaração de variáveis e o uso correto de tipos.
  * **Tratamento de Erros**: Reporta erros de sintaxe e semântica encontrados durante a compilação.

## Linguagem Suportada

O compilador suporta um subconjunto da linguagem Pascal, que inclui as seguintes características:

  * **Estrutura de programa**: `PROGRAM`, `VAR`, `BEGIN`, `END`.
  * **Tipos de dados**: `INTEGER`, `REAL`, `DOUBLE`, `BOOLEAN`.
  * **Estruturas de controle**: `IF...THEN...ELSE`, `WHILE...DO`, `REPEAT...UNTIL`.
  * **Procedimentos e Funções**: Declaração de `PROCEDURE` e `FUNCTION` com passagem de parâmetros por valor e por referência (`VAR`).
  * **Expressões**: Expressões aritméticas, lógicas e relacionais.
  * **Comentários**: Comentários de bloco usando `{` e `}`.

## Estrutura do Projeto

```
.
├── mypas.c           # Arquivo principal (entry point) do compilador
├── lexer.c           # Implementação do analisador léxico
├── lexer.h           # Definições do analisador léxico
├── parser.c          # Implementação do analisador sintático/semântico
├── parser.h          # Definições do analisador sintático
├── symtab.c          # Implementação da tabela de símbolos
├── symtab.h          # Definições da tabela de símbolos
├── keywords.c        # Implementação das palavras-chave
├── keywords.h        # Definições das palavras-chave
└── ...
```

  * `mypas.c`: O ponto de entrada do compilador. Responsável por ler o arquivo de entrada e iniciar o processo de compilação.
  * `lexer.c`/`lexer.h`: Responsáveis pela análise léxica. O código é lido e quebrado em tokens.
  * `parser.c`/`parser.h`: O coração do compilador. Realiza a análise sintática, verificando se a estrutura do programa está correta, e a análise semântica, validando o significado do código.
  * `symtab.c`/`symtab.h`: Implementam a tabela de símbolos, que armazena informações sobre os identificadores (variáveis, funções, etc.) do programa.
  * `keywords.c`/`keywords.h`: Gerenciam as palavras reservadas da linguagem.

## Como Compilar e Executar

Para compilar o projeto, você pode usar um compilador C como o GCC.

```bash
# Compile o projeto usando o Makefile
make mypas

# Execute o compilador passando um arquivo .pas como argumento
./mypas seu_programa.pas

# Para limpar os arquivos gerados pela compilação
make clean
# ou
make mostlyclean
```

O compilador irá analisar o arquivo `seu_programa.pas` e imprimir uma mensagem de sucesso ou os erros encontrados.
