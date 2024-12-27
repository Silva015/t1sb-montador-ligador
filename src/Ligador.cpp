#include "../include/Ligador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>

// Função para ler as tabelas de uso, definições, realocação e código de um arquivo objeto
void Ligador::lerTabelas(std::ifstream &arquivo, std::vector<TabelaUso> &tabelaUso,
                         std::vector<TabelaDefinicao> &tabelaDefinicao, std::vector<int> &codigo,
                         std::vector<int> &tabelaRealocacao)
{
    std::string linha;

    while (std::getline(arquivo, linha) && linha.find("U,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3));
        std::string simbolo;
        int endereco;
        while (iss >> simbolo >> endereco)
        {
            tabelaUso.push_back({simbolo, endereco});
        }
    }

    while (std::getline(arquivo, linha) && linha.find("D,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3));
        std::string simbolo;
        int endereco;
        while (iss >> simbolo >> endereco)
        {
            tabelaDefinicao.push_back({simbolo, endereco});
        }
    }

    if (std::getline(arquivo, linha) && linha.find("R,") != std::string::npos)
    {
        std::istringstream iss(linha.substr(3));
        int valor;
        while (iss >> valor)
        {
            tabelaRealocacao.push_back(valor);
        }
    }

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

    std::vector<TabelaUso> tabelaUso1, tabelaUso2;
    std::vector<TabelaDefinicao> tabelaDefinicao1, tabelaDefinicao2;
    std::vector<int> codigo1, codigo2;
    std::vector<int> tabelaRealocacao1, tabelaRealocacao2;
    int fatorCorrecao2 = 0;

    // Lê tabelas e códigos do primeiro módulo
    lerTabelas(arq1, tabelaUso1, tabelaDefinicao1, codigo1, tabelaRealocacao1);

    if (segundoArquivoPresente)
    {
        fatorCorrecao2 = codigo1.size();
        lerTabelas(arq2, tabelaUso2, tabelaDefinicao2, codigo2, tabelaRealocacao2);
    }

    std::unordered_map<std::string, int> tabelaGlobalDefinicoes;

    // Adicionar definições do primeiro módulo
    for (const auto &definicao : tabelaDefinicao1)
    {
        tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco;
    }

    // Adicionar definições do segundo módulo com fator de correção
    if (segundoArquivoPresente)
    {
        for (const auto &definicao : tabelaDefinicao2)
        {
            tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco + fatorCorrecao2;
        }
    }

    auto corrigirModulo = [&](std::vector<int> &codigo, std::vector<TabelaUso> &tabelaUso,
                              std::vector<int> &tabelaRealocacao, int fatorCorrecao)
    {
        for (auto &uso : tabelaUso)
        {
            if (tabelaGlobalDefinicoes.count(uso.simbolo))
            {
                codigo[uso.endereco] = tabelaGlobalDefinicoes[uso.simbolo];
            }
            else
            {
                std::cerr << "Erro: símbolo não definido '" << uso.simbolo << "'.\n";
                return;
            }
        }

        for (size_t i = 0; i < tabelaRealocacao.size(); ++i)
        {
            if (tabelaRealocacao[i] == 1)
            {
                codigo[i] += fatorCorrecao;
            }
        }
    };

    corrigirModulo(codigo1, tabelaUso1, tabelaRealocacao1, 0);

    if (segundoArquivoPresente)
    {
        corrigirModulo(codigo2, tabelaUso2, tabelaRealocacao2, fatorCorrecao2);
    }

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
