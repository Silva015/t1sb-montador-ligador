// Montador.h

#ifndef MONTADOR_H
#define MONTADOR_H

#include <string>
#include <unordered_map>
#include <vector>

// Estrutura para armazenar faixa de linhas no MDT
struct MacroInfo
{
    int startIndex; // primeira linha no MDT
    int endIndex;   // última linha no MDT (inclusivo)
};

// Ainda temos Instrucao, Diretiva etc.
struct Instrucao
{
    int opcode;
    int tamanho;
    int numOperandos;
};

struct Diretiva
{
    int tamanho;
};

// Classe Montador
class Montador
{
public:
    Montador() noexcept;
    void executar(const std::string &arquivoEntrada, const std::string &modo);

private:
    // --------------------------------------------
    // Tabelas de instruções e diretivas (como antes)
    // --------------------------------------------
    std::unordered_map<std::string, Instrucao> tabelaInstrucoes;
    std::unordered_map<std::string, Diretiva> tabelaDiretivas;

    // --------------------------------------------
    // MNT e MDT
    // --------------------------------------------
    // MNT: NomeDaMacro -> MacroInfo (start/end no MDT)
    std::unordered_map<std::string, MacroInfo> MNT;

    // MDT: Vetor com todas as linhas de todas as macros
    std::vector<std::string> MDT;

    // --------------------------------------------
    // Métodos auxiliares
    // --------------------------------------------
    void inicializarTabelaInstrucoes();
    void inicializarTabelaDiretivas();
    void preProcessar(const std::string &arquivoEntrada, const std::string &arquivoSaida);
    void montar(const std::string &arquivoPre, const std::string &arquivoObj);
    void primeiraPassagem(const std::string &arquivoPre, std::unordered_map<std::string, int> &tabelaSimbolos);
    void segundaPassagem(const std::string &arquivoPre,
                         const std::string &arquivoObj,
                         const std::unordered_map<std::string, int> &tabelaSimbolos);

    // Expansão recursiva de macros
    void expandirLinha(const std::string &linha,
                       std::vector<std::string> &linhasExpandidas);

    bool rotuloValido(const std::string &rotulo);
    bool valorConstValido(const std::string &str);
    int converterStringParaInt(const std::string &str);
    void reportarErro(const std::string &mensagem, int linha);
};

#endif