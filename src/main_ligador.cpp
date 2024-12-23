#include <iostream>
#include <fstream>
#include <string>
#include "../include/Ligador.h"

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

    return 0;
}
