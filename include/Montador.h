#ifndef MONTADOR_H
#define MONTADOR_H

#include <string>
#include <unordered_map>

// Estrutura para representar uma instrução
struct Instrucao
{
    int opcode;  // Código numérico da instrução
    int tamanho; // Tamanho em palavras
};

// Estrutura para representar uma diretiva
struct Diretiva
{
    int tamanho; // Tamanho em palavras, pode ser variável
};

class Montador
{
public:
    // Método principal para execução do Montador
    void executar(const std::string &arquivoEntrada, const std::string &modo);

private:
    // Tabelas para instruções e diretivas
    std::unordered_map<std::string, Instrucao> tabelaInstrucoes;
    std::unordered_map<std::string, Diretiva> tabelaDiretivas;

    // Inicializa as tabelas de instruções e diretivas
    void inicializarTabelaInstrucoes();
    void inicializarTabelaDiretivas();

    // Realiza o pré-processamento do arquivo
    void preProcessar(const std::string &arquivoEntrada, const std::string &arquivoSaida);

    // Realiza a montagem do código objeto
    void montar(const std::string &arquivoPre, const std::string &arquivoObj);

    // Primeira e segunda passagens do Montador
    void primeiraPassagem(const std::string &arquivoPre, std::unordered_map<std::string, int> &tabelaSimbolos);
    void segundaPassagem(const std::string &arquivoPre, const std::string &arquivoObj, const std::unordered_map<std::string, int> &tabelaSimbolos);

    // Reporta erros encontrados durante o processamento
    void reportarErro(const std::string &mensagem, int linha);
};

#endif
