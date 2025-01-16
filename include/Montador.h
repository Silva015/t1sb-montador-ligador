#ifndef MONTADOR_H
#define MONTADOR_H

#include <string>
#include <unordered_map>

// -- Declaração da função trim:
std::string trim(const std::string &str);

// -- Suas structs e a classe Montador continuam aqui...

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

class Montador
{
public:
    Montador() noexcept;
    void executar(const std::string &arquivoEntrada, const std::string &modo);

private:
    std::unordered_map<std::string, Instrucao> tabelaInstrucoes;
    std::unordered_map<std::string, Diretiva> tabelaDiretivas;

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
};

#endif