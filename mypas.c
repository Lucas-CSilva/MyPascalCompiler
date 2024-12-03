#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <parser.h>
#include <lexer.h>

FILE *src;

int has_pascal_extension(const char *filename) {
    const char *ext = strrchr(filename, '.'); // Encontra a última ocorrência de '.'

    return ext != NULL && strcmp(ext, ".pas") == 0; // Verifica se é ".pas";
}

// int main(/*int argc, char *argv[]*/) 
int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }

    // Valida se o arquivo tem a extensão .pas
    if (!has_pascal_extension(argv[1])) 
    {
        fprintf(stderr, "Erro: O arquivo deve ter a extensão .pas\n");
        return 1;
    }

    // Tenta abrir o arquivo
    src = fopen(argv[1], "r");
    if (src == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    lookahead = gettoken(src);
    
    program();

    printf("\tcompilation completed with %d error(s)\n", error_count);
}