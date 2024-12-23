#ifndef LIGADOR_H
#define LIGADOR_H

#include <string>
#include <unordered_map>

class Ligador
{
public:
    void executar(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida);

private:
    void combinarArquivos(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida);

    // Declaração da função para ler tabelas de uso e definições
    void lerTabelas(std::ifstream &arquivo, std::unordered_map<std::string, int> &tabelaUso, std::unordered_map<std::string, int> &tabelaDefinicoes);
};

#endif
