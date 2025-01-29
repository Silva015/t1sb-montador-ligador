#ifndef MONTADOR_H
#define MONTADOR_H

#include <string>
#include <unordered_map>
#include <vector>

// Definição de Instrucao
struct Instrucao
{
    int opcode;
    int tamanho;
    int numOperandos;
};

// Definição de Diretiva
struct Diretiva
{
    int tamanho;
};
// ----------------------------------------------------------
// Estrutura auxiliar para armazenar as linhas de uma macro
// ----------------------------------------------------------
struct MacroDefinition
{
    std::vector<std::string> linhas;
};

class Montador
{
public:
    Montador() noexcept;
    void executar(const std::string &arquivoEntrada, const std::string &modo);

private:
    // ----------------------------------------------------------
    // Tabelas de instruções e diretivas
    // ----------------------------------------------------------
    std::unordered_map<std::string, Instrucao> tabelaInstrucoes;
    std::unordered_map<std::string, Diretiva> tabelaDiretivas;

    // ----------------------------------------------------------
    // Tabela de macros (MNT)
    // chave: nome da macro
    // valor: as linhas do corpo da macro
    // ----------------------------------------------------------
    std::unordered_map<std::string, MacroDefinition> MNT;

    // ----------------------------------------------------------
    // Métodos auxiliares
    // ----------------------------------------------------------
    void inicializarTabelaInstrucoes();
    void inicializarTabelaDiretivas();
    void preProcessar(const std::string &arquivoEntrada, const std::string &arquivoSaida);
    void montar(const std::string &arquivoPre, const std::string &arquivoObj);
    void primeiraPassagem(const std::string &arquivoPre, std::unordered_map<std::string, int> &tabelaSimbolos);
    void segundaPassagem(const std::string &arquivoPre,
                         const std::string &arquivoObj,
                         const std::unordered_map<std::string, int> &tabelaSimbolos);

    void reportarErro(const std::string &mensagem, int linha);

    bool rotuloValido(const std::string &rotulo);
    bool valorConstValido(const std::string &str);
    int converterStringParaInt(const std::string &str);
    // Função para expandir recursivamente uma linha que pode ser chamada de macro.
    void expandirLinha(
        const std::string &linha,
        const std::unordered_map<std::string, MacroDefinition> &MNT,
        std::vector<std::string> &linhasExpandidas);
};

#endif