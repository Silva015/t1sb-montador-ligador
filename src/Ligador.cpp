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

    // Lê a tabela de uso
    while (std::getline(arquivo, linha))
    {
        if (linha.find("U,") == 0)
        {
            std::istringstream iss(linha.substr(3));
            std::string simbolo;
            int endereco;
            while (iss >> simbolo >> endereco)
            {
                tabelaUso.push_back({simbolo, endereco});
            }
        }
        else if (linha.find("D,") == 0)
        {
            // Lê a tabela de definições
            std::istringstream iss(linha.substr(3));
            std::string simbolo;
            int endereco;
            while (iss >> simbolo >> endereco)
            {
                tabelaDefinicao.push_back({simbolo, endereco});
            }
        }
        else if (linha.find("R,") == 0)
        {
            // Lê a tabela de realocação
            std::istringstream iss(linha.substr(3));
            int valor;
            while (iss >> valor)
            {
                tabelaRealocacao.push_back(valor);
            }
        }
        else
        {
            // Lê o código objeto
            std::istringstream iss(linha);
            int palavra;
            while (iss >> palavra)
            {
                codigo.push_back(palavra);
            }
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

    // print all tables

    std::cout << "Tabela de Uso 1:\n";
    for (const auto &uso : tabelaUso1)
    {
        std::cout << uso.simbolo << " " << uso.endereco << "\n";
    }

    if (segundoArquivoPresente)
    {
        std::cout << "Tabela de Uso 2:\n";
        for (const auto &uso : tabelaUso2)
        {
            std::cout << uso.simbolo << " " << uso.endereco << "\n";
        }
    }

    std::cout << "Tabela de Definição 1:\n";
    for (const auto &definicao : tabelaDefinicao1)
    {
        std::cout << definicao.simbolo << " " << definicao.endereco << "\n";
    }

    if (segundoArquivoPresente)
    {
        std::cout << "Tabela de Definição 2:\n";
        for (const auto &definicao : tabelaDefinicao2)
        {
            std::cout << definicao.simbolo << " " << definicao.endereco << "\n";
        }
    }

    std::cout << "Código 1:\n";
    for (const auto &valor : codigo1)
    {
        std::cout << valor << " ";
    }
    std::cout << "\n";

    if (segundoArquivoPresente)
    {
        std::cout << "Código 2:\n";
        for (const auto &valor : codigo2)
        {
            std::cout << valor << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Tabela de Realocação 1:\n";
    for (const auto &valor : tabelaRealocacao1)
    {
        std::cout << valor << " ";
    }
    std::cout << "\n";

    if (segundoArquivoPresente)
    {
        std::cout << "Tabela de Realocação 2:\n";
        for (const auto &valor : tabelaRealocacao2)
        {
            std::cout << valor << " ";
        }
        std::cout << "\n";
    }

    // Constrói a tabela global de definições
    std::unordered_map<std::string, int> tabelaGlobalDefinicoes;

    // Adiciona definições do primeiro módulo
    for (const auto &definicao : tabelaDefinicao1)
    {
        tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco;
    }

    // Adiciona definições do segundo módulo com fator de correção
    if (segundoArquivoPresente)
    {
        for (const auto &definicao : tabelaDefinicao2)
        {
            tabelaGlobalDefinicoes[definicao.simbolo] = definicao.endereco + fatorCorrecao2;
        }
    }

    // Corrige os endereços nos códigos dos módulos
    auto corrigirModulo = [&](std::vector<int> &codigo, std::vector<TabelaUso> &tabelaUso,
                              std::vector<int> &tabelaRealocacao, int fatorCorrecao)
    {
        // Corrige endereços das entradas na tabela de uso
        for (const auto &uso : tabelaUso)
        {
            if (tabelaGlobalDefinicoes.count(uso.simbolo))
            {
                // Substitui o placeholder pelo endereço global do símbolo
                codigo[uso.endereco] = tabelaGlobalDefinicoes[uso.simbolo];
            }
            else
            {
                std::cerr << "Erro: símbolo não definido '" << uso.simbolo << "'.\n";
                exit(EXIT_FAILURE);
            }
        }

        // Corrige os endereços realocados
        for (size_t i = 0; i < tabelaRealocacao.size(); ++i)
        {
            if (tabelaRealocacao[i] == 1)
            {
                codigo[i] += fatorCorrecao; // Realoca apenas quando indicado
            }
        }
    };

    corrigirModulo(codigo1, tabelaUso1, tabelaRealocacao1, 0);

    if (segundoArquivoPresente)
    {
        corrigirModulo(codigo2, tabelaUso2, tabelaRealocacao2, fatorCorrecao2);
    }

    // Gera o código executável
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
