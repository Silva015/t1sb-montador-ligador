#include <iostream>
#include <fstream>
#include <string>
#include "../include/Ligador.h"

void exibirAjudaLigador()
{
    std::cout << "Uso do Ligador:\n";
    std::cout << "./ligador prog1.obj [prog2.obj] - Gera arquivo executável (prog1.e)\n";
    std::cout << "Nota: prog2.obj é opcional.\n";
}

bool arquivoExiste(const std::string &nomeArquivo)
{
    std::ifstream arquivo(nomeArquivo);
    return arquivo.good();
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Erro: argumentos insuficientes ou em excesso para o Ligador.\n";
        exibirAjudaLigador();
        return 1;
    }

    std::string arquivo1 = argv[1];
    std::string arquivo2 = (argc == 3) ? argv[2] : "";
    std::string arquivoSaida = arquivo1.substr(0, arquivo1.rfind(".")) + ".e";

    if (!arquivoExiste(arquivo1))
    {
        std::cerr << "Erro: arquivo '" << arquivo1 << "' não encontrado.\n";
        return 1;
    }
    if (!arquivo2.empty() && !arquivoExiste(arquivo2))
    {
        std::cerr << "Erro: arquivo '" << arquivo2 << "' não encontrado.\n";
        return 1;
    }

    Ligador ligador;
    ligador.executar(arquivo1, arquivo2, arquivoSaida);

    return 0;
}
