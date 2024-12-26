#ifndef LIGADOR_H
#define LIGADOR_H

#include <string>
#include <vector>

// Estruturas para Tabela de Uso e Tabela de Definições
struct TabelaUso
{
    std::string simbolo;
    int endereco;
};

struct TabelaDefinicao
{
    std::string simbolo;
    int endereco;
};

class Ligador
{
public:
    /**
     * Executa o processo de ligação.
     * @param arquivo1 Primeiro arquivo objeto.
     * @param arquivo2 Segundo arquivo objeto (opcional).
     * @param arquivoSaida Arquivo executável de saída.
     */
    void executar(const std::string &arquivo1, const std::string &arquivo2, const std::string &arquivoSaida);

private:
    /**
     * Lê tabelas de uso, definições e código de um arquivo objeto.
     * @param arquivo Arquivo de entrada.
     * @param tabelaUso Vetor de tabela de uso.
     * @param tabelaDefinicao Vetor de tabela de definições.
     * @param codigo Vetor de código.
     * @param fatorCorrecao Fator de correção de endereço.
     */
    void lerTabelas(std::ifstream &arquivo, std::vector<TabelaUso> &tabelaUso,
                    std::vector<TabelaDefinicao> &tabelaDefinicao, std::vector<int> &codigo, int &fatorCorrecao);
};

#endif
