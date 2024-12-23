#include <iostream>
#include <fstream>
#include <string>
#include "../include/Montador.h"
#include "../include/Ligador.h"

void exibirAjudaMontador()
{
    std::cout << "Uso do Montador:\n";
    std::cout << "./montador arquivo.asm    - Gera arquivo pré-processado (arquivo.pre)\n";
    std::cout << "./montador arquivo.pre    - Gera arquivo objeto (arquivo.obj)\n";
}

void exibirAjudaLigador()
{
    std::cout << "Uso do Ligador:\n";
    std::cout << "./ligador prog1.obj prog2.obj - Gera arquivo executável (prog1.e)\n";
}

bool arquivoExiste(const std::string &nomeArquivo)
{
    std::ifstream arquivo(nomeArquivo);
    return arquivo.good();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Erro: argumentos insuficientes.\n";
        std::cerr << "Use './montador' ou './ligador' com os argumentos corretos.\n";
        return 1;
    }

    std::string modo = argv[0]; // Nome do programa chamado (montador ou ligador)

    if (modo.find("montador") != std::string::npos)
    {
        if (argc != 3)
        {
            std::cerr << "Erro: argumentos insuficientes para o Montador.\n";
            exibirAjudaMontador();
            return 1;
        }

        std::string arquivoEntrada = argv[2];
        if (!arquivoExiste(arquivoEntrada))
        {
            std::cerr << "Erro: arquivo '" << arquivoEntrada << "' não encontrado.\n";
            return 1;
        }

        Montador montador;
        if (arquivoEntrada.rfind(".asm") == arquivoEntrada.length() - 4)
            montador.executar(arquivoEntrada, "pre");
        else if (arquivoEntrada.rfind(".pre") == arquivoEntrada.length() - 4)
            montador.executar(arquivoEntrada, "obj");
        else
        {
            std::cerr << "Erro: extensão do arquivo não reconhecida. Use .asm ou .pre.\n";
            exibirAjudaMontador();
            return 1;
        }
    }
    else if (modo.find("ligador") != std::string::npos)
    {
        if (argc != 4)
        {
            std::cerr << "Erro: argumentos insuficientes para o Ligador.\n";
            exibirAjudaLigador();
            return 1;
        }

        std::string arquivo1 = argv[1];
        std::string arquivo2 = argv[2];
        std::string arquivoSaida = argv[3];

        if (!arquivoExiste(arquivo1))
        {
            std::cerr << "Erro: arquivo '" << arquivo1 << "' não encontrado.\n";
            return 1;
        }
        if (!arquivoExiste(arquivo2))
        {
            std::cerr << "Erro: arquivo '" << arquivo2 << "' não encontrado.\n";
            return 1;
        }

        Ligador ligador;
        ligador.executar(arquivo1, arquivo2, arquivoSaida);
    }
    else
    {
        std::cerr << "Erro: programa não reconhecido. Use './montador' ou './ligador'.\n";
        return 1;
    }

    return 0;
}
