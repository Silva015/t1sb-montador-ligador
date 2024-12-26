#include "../include/Ligador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

// Função para ler as tabelas de uso, definições e o código de um arquivo objeto
void Ligador::lerTabelas(std::ifstream &arquivo, std::vector<TabelaUso> &tabelaUso,
                         std::vector<TabelaDefinicao> &tabelaDefinicao, std::vector<int> &codigo, int &fatorCorrecao)
{
    std::string linha;

    // Lê a tabela de uso
    while (std::getline(arquivo, linha) && linha.find("U,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3)); // Ignora "U, "
        std::string simbolo;
        int endereco;
        while (iss >> simbolo >> endereco)
        {
            tabelaUso.push_back({simbolo, endereco});
        }
    }

    // Lê a tabela de definições
    while (std::getline(arquivo, linha) && linha.find("D,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3)); // Ignora "D, "
        std::string simbolo;
        int endereco;
        while (iss >> simbolo >> endereco)
        {
            tabelaDefinicao.push_back({simbolo, endereco});
        }
    }

    // Lê a tabela de realocação
    if (std::getline(arquivo, linha) && linha.find("R,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3)); // Ignora "R, "
        int valor;
        while (iss >> valor)
        {
            fatorCorrecao = valor; // Realocação sempre ajustada
        }
    }

    // Lê o código
    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        int palavra;
        while (iss >> palavra)
        {
            codigo.push_back(palavra);
        }
    }
}

// Função principal do ligador
void Ligador::executar(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida)
{
    std::ifstream arq1(arquivo1), arq2;
    std::ofstream saida(arquivoSaida);

    if (!arq1.is_open())
    {
        std::cerr << "Erro ao abrir o arquivo: " << arquivo1 << "\n";
        return;
    }

    bool segundoArquivoPresente = !arquivo2.empty();
    if (segundoArquivoPresente)
    {
        arq2.open(arquivo2);
        if (!arq2.is_open())
        {
            std::cerr << "Erro ao abrir o arquivo: " << arquivo2 << "\n";
            return;
        }
    }

    // Tabelas e código dos módulos
    std::vector<TabelaUso> tabelaUso1, tabelaUso2;
    std::vector<TabelaDefinicao> tabelaDefinicao1, tabelaDefinicao2;
    std::vector<int> codigo1, codigo2;

    int fatorCorrecao1 = 0, fatorCorrecao2 = 0;

    // Lê o primeiro arquivo objeto
    lerTabelas(arq1, tabelaUso1, tabelaDefinicao1, codigo1, fatorCorrecao1);

    if (segundoArquivoPresente)
    {
        fatorCorrecao2 = codigo1.size();
        lerTabelas(arq2, tabelaUso2, tabelaDefinicao2, codigo2, fatorCorrecao2);
    }

    // Tabela global de definições
    std::unordered_map<std::string, int> tabelaGlobalDefinicoes;
    for (const auto &definicao : tabelaDefinicao1)
    {
        tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco;
    }
    if (segundoArquivoPresente)
    {
        for (const auto &definicao : tabelaDefinicao2)
        {
            if (tabelaGlobalDefinicoes.count(definicao.simbolo))
            {
                std::cerr << "Erro: símbolo duplicado '" << definicao.simbolo << "'.\n";
                return;
            }
            tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco + fatorCorrecao2;
        }
    }

    // Corrige endereços para tabelas de uso
    for (auto &uso : tabelaUso1)
    {
        if (!tabelaGlobalDefinicoes.count(uso.simbolo))
        {
            std::cerr << "Erro: símbolo não definido '" << uso.simbolo << "'.\n";
            return;
        }
        codigo1[uso.endereco] = tabelaGlobalDefinicoes[uso.simbolo];
    }

    if (segundoArquivoPresente)
    {
        for (auto &uso : tabelaUso2)
        {
            if (!tabelaGlobalDefinicoes.count(uso.simbolo))
            {
                std::cerr << "Erro: símbolo não definido '" << uso.simbolo << "'.\n";
                return;
            }
            codigo2[uso.endereco] = tabelaGlobalDefinicoes[uso.simbolo];
        }
    }

    // Escreve o código final no arquivo de saída
    for (const auto &valor : codigo1)
    {
        saida << valor << " ";
    }
    if (segundoArquivoPresente)
    {
        for (const auto &valor : codigo2)
        {
            saida << valor << " ";
        }
    }

    arq1.close();
    if (segundoArquivoPresente)
    {
        arq2.close();
    }
    saida.close();
}
