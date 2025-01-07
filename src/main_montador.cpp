#include <iostream>
#include <fstream>
#include <string>
#include "../include/Montador.h"

void exibirAjudaMontador()
{
    std::cout << "Uso do Montador:\n";
    std::cout << "./montador arquivo.asm    - Gera arquivo pré-processado (arquivo.pre)\n";
    std::cout << "./montador arquivo.pre    - Gera arquivo objeto (arquivo.obj)\n";
}

bool arquivoExiste(const std::string &nomeArquivo)
{
    std::ifstream arquivo(nomeArquivo);
    return arquivo.good();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Erro: argumentos insuficientes para o Montador.\n";
        exibirAjudaMontador();
        return 1;
    }

    std::string arquivoEntrada = argv[1];
    if (!arquivoExiste(arquivoEntrada))
    {
        std::cerr << "Erro: arquivo '" << arquivoEntrada << "' não encontrado.\n";
        return 1;
    }

    Montador montador;

    if (arquivoEntrada.rfind(".asm") == arquivoEntrada.length() - 4)
    {
        montador.executar(arquivoEntrada, "pre");
    }
    else if (arquivoEntrada.rfind(".pre") == arquivoEntrada.length() - 4)
    {
        montador.executar(arquivoEntrada, "obj");
    }
    else if (arquivoEntrada.rfind(".txt") == arquivoEntrada.length() - 4)
    {
        std::cerr << "Erro: arquivos .txt não podem ser processados pelo montador.\n";
        return 1;
    }

    else
    {
        std::cerr << "Erro: extensão do arquivo não reconhecida. Use .asm ou .pre.\n";
        exibirAjudaMontador();
        return 1;
    }

    return 0;
}
