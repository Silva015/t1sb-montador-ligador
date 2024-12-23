#include "../include/Ligador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

void Ligador::executar(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida)
{
    std::cout << "Ligando arquivos: " << arquivo1 << ", " << arquivo2 << " em " << arquivoSaida << std::endl;
    combinarArquivos(arquivo1, arquivo2, arquivoSaida);
}

void Ligador::combinarArquivos(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida)
{
    std::ifstream arq1(arquivo1), arq2(arquivo2);
    std::ofstream saida(arquivoSaida);

    if (!arq1.is_open() || !arq2.is_open() || !saida.is_open())
    {
        std::cerr << "Erro ao abrir arquivos para o Ligador.\n";
        return;
    }

    // Tabelas de uso e definições
    std::unordered_map<std::string, int> tabelaUso1, tabelaDefinicoes1;
    std::unordered_map<std::string, int> tabelaUso2, tabelaDefinicoes2;

    // Ler tabelas dos arquivos
    lerTabelas(arq1, tabelaUso1, tabelaDefinicoes1);
    lerTabelas(arq2, tabelaUso2, tabelaDefinicoes2);

    // Combinar tabelas de definições
    std::unordered_map<std::string, int> tabelaGlobalDefinicoes = tabelaDefinicoes1;
    int deslocamento = tabelaUso1.size(); // Tamanho do primeiro código

    for (const auto &definicao : tabelaDefinicoes2)
    {
        if (tabelaGlobalDefinicoes.count(definicao.first))
        {
            std::cerr << "Erro: símbolo duplicado '" << definicao.first << "' em tabelas de definições.\n";
            exit(EXIT_FAILURE);
        }
        tabelaGlobalDefinicoes[definicao.first] = definicao.second + deslocamento; // Ajustar endereços
    }

    // Resolver referências externas
    for (const auto &uso : tabelaUso1)
    {
        if (tabelaGlobalDefinicoes.count(uso.first) == 0)
        {
            std::cerr << "Erro: referência externa não resolvida '" << uso.first << "'.\n";
            exit(EXIT_FAILURE);
        }
    }
    for (const auto &uso : tabelaUso2)
    {
        if (tabelaGlobalDefinicoes.count(uso.first) == 0)
        {
            std::cerr << "Erro: referência externa não resolvida '" << uso.first << "'.\n";
            exit(EXIT_FAILURE);
        }
    }

    // Combinar códigos em uma única linha com ajuste de realocação
    std::string codigo1, codigo2;
    std::getline(arq1, codigo1);
    std::getline(arq2, codigo2);

    // Escrever o código combinado no arquivo de saída
    saida << codigo1 << " ";
    std::istringstream iss(codigo2);
    int instrucao;
    while (iss >> instrucao)
    {
        saida << (instrucao + deslocamento) << " ";
    }

    arq1.close();
    arq2.close();
    saida.close();
    std::cout << "Ligação concluída com sucesso!\n";
}

void Ligador::lerTabelas(std::ifstream &arquivo, std::unordered_map<std::string, int> &tabelaUso, std::unordered_map<std::string, int> &tabelaDefinicoes)
{
    std::string linha;

    // Ler tabela de uso
    if (std::getline(arquivo, linha))
    {
        std::istringstream usoStream(linha);
        std::string simbolo;
        int endereco;
        while (usoStream >> simbolo >> endereco)
        {
            tabelaUso[simbolo] = endereco;
        }
    }

    // Ler tabela de definições
    if (std::getline(arquivo, linha))
    {
        std::istringstream defStream(linha);
        std::string simbolo;
        int endereco;
        while (defStream >> simbolo >> endereco)
        {
            tabelaDefinicoes[simbolo] = endereco;
        }
    }
}
