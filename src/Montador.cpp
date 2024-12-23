#include "../include/Montador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

void Montador::inicializarTabelaInstrucoes()
{
    tabelaInstrucoes = {
        {"ADD", {1, 2}},
        {"SUB", {2, 2}},
        {"MUL", {3, 2}},
        {"DIV", {4, 2}},
        {"JMP", {5, 2}},
        {"JMPN", {6, 2}},
        {"JMPP", {7, 2}},
        {"JMPZ", {8, 2}},
        {"COPY", {9, 3}},
        {"LOAD", {10, 2}},
        {"STORE", {11, 2}},
        {"INPUT", {12, 2}},
        {"OUTPUT", {13, 2}},
        {"STOP", {14, 1}}};
}

void Montador::inicializarTabelaDiretivas()
{
    tabelaDiretivas = {
        {"SPACE", {1}}, // Pode variar, ajustado durante o processamento
        {"CONST", {1}}, // Sempre ocupa 1 palavra
        {"BEGIN", {0}}, // Não ocupa espaço
        {"END", {0}}    // Não ocupa espaço
    };
}

Montador::Montador()
{
    inicializarTabelaInstrucoes();
    inicializarTabelaDiretivas();
}

void Montador::executar(const std::string &arquivoEntrada, const std::string &modo)
{
    if (modo == "pre")
    {
        std::string arquivoSaida = arquivoEntrada.substr(0, arquivoEntrada.rfind(".")) + ".pre";
        preProcessar(arquivoEntrada, arquivoSaida);
    }
    else if (modo == "obj")
    {
        std::string arquivoSaida = arquivoEntrada.substr(0, arquivoEntrada.rfind(".")) + ".obj";
        montar(arquivoEntrada, arquivoSaida);
    }
    else
    {
        std::cerr << "Erro: modo inválido para o Montador.\n";
    }
}

void Montador::preProcessar(const std::string &arquivoEntrada, const std::string &arquivoSaida)
{
    std::ifstream arquivo(arquivoEntrada);
    std::ofstream arquivoPre(arquivoSaida);

    if (!arquivo.is_open() || !arquivoPre.is_open())
    {
        std::cerr << "Erro ao abrir arquivos para pré-processamento.\n";
        return;
    }

    std::string linha;
    std::string secaoTexto, secaoDados;

    while (std::getline(arquivo, linha))
    {
        // Remove comentários
        linha = linha.substr(0, linha.find(';'));

        // Normaliza para maiúsculas
        std::transform(linha.begin(), linha.end(), linha.begin(), ::toupper);

        // Preserva espaços para instruções como COPY ARG1,ARG2
        if (!linha.empty())
        {
            if (linha.find("SECTIONTEXT") != std::string::npos)
                secaoTexto += linha + "\n";
            else if (linha.find("SECTIONDATA") != std::string::npos)
                secaoDados += linha + "\n";
            else
                (secaoDados.empty() ? secaoTexto : secaoDados) += linha + "\n";
        }
    }

    arquivoPre << secaoTexto << secaoDados;
    arquivo.close();
    arquivoPre.close();
}

void Montador::montar(const std::string &arquivoPre, const std::string &arquivoObj)
{
    std::unordered_map<std::string, int> tabelaSimbolos;
    primeiraPassagem(arquivoPre, tabelaSimbolos);
    segundaPassagem(arquivoPre, arquivoObj, tabelaSimbolos);
}

void Montador::primeiraPassagem(const std::string &arquivoPre, std::unordered_map<std::string, int> &tabelaSimbolos)
{
    std::ifstream arquivo(arquivoPre);
    if (!arquivo.is_open())
    {
        reportarErro("Erro ao abrir arquivo na primeira passagem.", 0);
    }

    int contadorPosicao = 0;
    int linhaAtual = 1;
    std::string linha;

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string rotulo, instrucao, operandos;

        iss >> rotulo;

        // Verifica se é um rótulo
        if (rotulo.back() == ':')
        {
            rotulo.pop_back();
            if (tabelaSimbolos.count(rotulo))
            {
                reportarErro("Rótulo redefinido: " + rotulo, linhaAtual);
            }
            else
            {
                tabelaSimbolos[rotulo] = contadorPosicao;
            }
            iss >> instrucao;
        }
        else
        {
            instrucao = rotulo;
        }

        // Diretivas `PUBLIC` e `EXTERN`
        if (instrucao == "PUBLIC")
        {
            iss >> operandos;
            if (operandos.empty())
            {
                reportarErro("Diretiva PUBLIC requer um rótulo.", linhaAtual);
            }
            // Marca o rótulo como público para o ligador
            tabelaSimbolos[operandos] = -1; // `-1` indica símbolo exportado
        }
        else if (instrucao == "EXTERN")
        {
            iss >> operandos;
            if (operandos.empty())
            {
                reportarErro("Diretiva EXTERN requer um rótulo.", linhaAtual);
            }
            // Marca o rótulo como externo
            tabelaSimbolos[operandos] = -2; // `-2` indica símbolo importado
        }
        // Verifica outras diretivas e instruções
        else if (tabelaDiretivas.find(instrucao) != tabelaDiretivas.end())
        {
            // (Implementação da lógica de `SPACE` e `CONST`)
        }
        else if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            contadorPosicao += tabelaInstrucoes[instrucao].tamanho;
        }
        else
        {
            reportarErro("Instrução ou diretiva inválida: " + instrucao, linhaAtual);
        }

        linhaAtual++;
    }

    arquivo.close();
}

void Montador::segundaPassagem(const std::string &arquivoPre, const std::string &arquivoObj, const std::unordered_map<std::string, int> &tabelaSimbolos)
{
    std::ifstream arquivo(arquivoPre);
    std::ofstream arquivoSaida(arquivoObj);

    if (!arquivo.is_open() || !arquivoSaida.is_open())
    {
        std::cerr << "Erro ao abrir arquivos na segunda passagem.\n";
        return;
    }

    std::string linha;
    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string instrucao, operandos;

        iss >> instrucao >> operandos;

        if (tabelaDiretivas.find(instrucao) != tabelaDiretivas.end())
        {
            if (instrucao == "SPACE")
            {
                int tamanho = operandos.empty() ? 1 : std::stoi(operandos);
                for (int i = 0; i < tamanho; i++)
                {
                    arquivoSaida << "00 ";
                }
            }
            else if (instrucao == "CONST")
            {
                arquivoSaida << operandos << " ";
            }
        }
        else if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            arquivoSaida << tabelaInstrucoes[instrucao].opcode << " ";
            if (!operandos.empty() && tabelaSimbolos.find(operandos) != tabelaSimbolos.end())
            {
                arquivoSaida << tabelaSimbolos.at(operandos) << " ";
            }
        }
        else
        {
            reportarErro("Instrução ou diretiva inválida na segunda passagem: " + instrucao, 0);
        }
    }

    arquivo.close();
    arquivoSaida.close();
}

void Montador::reportarErro(const std::string &mensagem, int linha)
{
    std::cerr << "Erro na linha " << linha << ": " << mensagem << "\n";
    std::cerr << "Abortando execução...\n";
    exit(EXIT_FAILURE); // Termina o programa após o erro
}
