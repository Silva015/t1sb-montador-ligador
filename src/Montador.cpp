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
        {"MULT", {3, 2}},
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

Montador::Montador() noexcept
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
    bool encontrouSectionText = false;
    bool emSectionText = false;
    bool emSectionData = false;

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);

        // Verifica as diretivas SECTION
        if (linha.find("SECTION") != std::string::npos)
        {
            if (linha.find("TEXT") != std::string::npos)
            {
                emSectionText = true;
                emSectionData = false;
                encontrouSectionText = true;
            }
            else if (linha.find("DATA") != std::string::npos)
            {
                emSectionText = false;
                emSectionData = true;
            }
            else
            {
                reportarErro("Diretiva SECTION inválida.", linhaAtual);
            }
            linhaAtual++;
            continue;
        }

        std::string rotulo, instrucao;
        iss >> rotulo;

        // Verifica e trata rótulos
        if (rotulo.back() == ':')
        {
            rotulo.pop_back();
            if (tabelaSimbolos.count(rotulo))
            {
                reportarErro("Rótulo redefinido: " + rotulo, linhaAtual);
            }
            tabelaSimbolos[rotulo] = contadorPosicao;
            iss >> instrucao;
        }
        else
        {
            instrucao = rotulo;
        }

        // Validação de diretivas e instruções
        if (tabelaDiretivas.find(instrucao) != tabelaDiretivas.end())
        {
            if (instrucao == "SPACE" || instrucao == "CONST")
            {
                if (!emSectionData)
                {
                    reportarErro("Diretiva de dados fora da seção DATA.", linhaAtual);
                }
                contadorPosicao++;
            }
        }
        else if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            if (!emSectionText)
            {
                reportarErro("Instrução fora da seção TEXT.", linhaAtual);
            }
            contadorPosicao += tabelaInstrucoes[instrucao].tamanho;
        }
        else
        {
            reportarErro("Instrução ou diretiva inválida: " + instrucao, linhaAtual);
        }

        linhaAtual++;
    }

    arquivo.close();

    // Verificação final: a seção TEXT é obrigatória
    if (!encontrouSectionText)
    {
        reportarErro("Seção TEXT ausente no programa.", 0);
    }
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
    bool emSectionText = false;
    bool emSectionData = false;

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string instrucao, operandos;

        iss >> instrucao;

        // Verifica se é uma seção
        if (instrucao == "SECTIONTEXT")
        {
            emSectionText = true;
            emSectionData = false;
            continue;
        }
        else if (instrucao == "SECTIONDATA")
        {
            emSectionText = false;
            emSectionData = true;
            continue;
        }

        if (tabelaDiretivas.find(instrucao) != tabelaDiretivas.end())
        {
            if (instrucao == "SPACE")
            {
                int tamanho = 1;
                if (iss >> operandos)
                {
                    tamanho = std::stoi(operandos);
                }
                for (int i = 0; i < tamanho; i++)
                {
                    arquivoSaida << "00 ";
                }
            }
            else if (instrucao == "CONST")
            {
                if (!(iss >> operandos))
                {
                    reportarErro("Diretiva CONST requer um valor.", 0);
                }
                arquivoSaida << operandos << " ";
            }
        }
        else if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            if (!emSectionText)
            {
                reportarErro("Instruções devem estar na SECTION TEXT.", 0);
            }
            arquivoSaida << tabelaInstrucoes[instrucao].opcode << " ";

            if (iss >> operandos)
            {
                if (tabelaSimbolos.find(operandos) == tabelaSimbolos.end())
                {
                    reportarErro("Símbolo não definido: " + operandos, 0);
                }
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
