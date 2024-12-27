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
        {"SPACE", {1}},  // Pode variar, ajustado durante o processamento
        {"CONST", {1}},  // Sempre ocupa 1 palavra
        {"BEGIN", {0}},  // Não ocupa espaço
        {"END", {0}},    // Não ocupa espaço
        {"EXTERN", {0}}, // Não ocupa espaço
        {"PUBLIC", {0}}  // Não ocupa espaço
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

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string rotulo, instrucao;

        // Verifica e separa rótulo, se houver
        iss >> rotulo;
        if (!rotulo.empty() && rotulo.back() == ':')
        {
            rotulo.pop_back(); // Remove o ':' do rótulo
            iss >> instrucao;  // Lê a instrução após o rótulo

            if (instrucao == "EXTERN")
            {
                // Se EXTERN, registra o rótulo como externo
                if (tabelaSimbolos.count(rotulo))
                {
                    reportarErro("Rótulo redefinido em EXTERN: " + rotulo, linhaAtual);
                }
                tabelaSimbolos[rotulo] = -1; // Marca como externo
                linhaAtual++;
                continue;
            }

            // Se não for EXTERN, registra o rótulo normalmente
            if (tabelaSimbolos.count(rotulo))
            {
                reportarErro("Rótulo redefinido: " + rotulo, linhaAtual);
            }
            tabelaSimbolos[rotulo] = contadorPosicao;
        }
        else
        {
            instrucao = rotulo;
        }

        // Tratamento para EXTERN sem rótulo
        if (instrucao == "EXTERN")
        {
            std::string argumentoLabel;
            if (!(iss >> argumentoLabel))
            {
                reportarErro("Diretiva EXTERN requer um argumento (LABEL).", linhaAtual);
            }

            if (tabelaSimbolos.count(argumentoLabel))
            {
                reportarErro("Rótulo redefinido em EXTERN: " + argumentoLabel, linhaAtual);
            }

            // Registra o argumento como símbolo externo
            tabelaSimbolos[argumentoLabel] = -1; // Marca como externo
            linhaAtual++;
            continue;
        }

        // Processamento das demais instruções e diretivas
        if (tabelaInstrucoes.count(instrucao))
        {
            contadorPosicao += tabelaInstrucoes[instrucao].tamanho;
        }
        else if (tabelaDiretivas.count(instrucao))
        {
            if (instrucao == "SPACE" || instrucao == "CONST")
            {
                contadorPosicao++;
            }
        }
        else if (instrucao == "SECTION")
        {
            std::string argumentoSection;
            if (!(iss >> argumentoSection) || (argumentoSection != "TEXT" && argumentoSection != "DATA"))
            {
                reportarErro("Diretiva SECTION inválida.", linhaAtual);
            }
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

    // Adiciona símbolos públicos à tabela de definições no arquivo de saída
    for (const auto &simbolo : tabelaSimbolos)
    {
        if (simbolo.second >= 0) // Símbolos definidos localmente
        {
            arquivoSaida << "D, " << simbolo.first << " " << simbolo.second << "\n";
        }
    }

    // Adiciona símbolos externos à tabela de uso
    for (const auto &simbolo : tabelaSimbolos)
    {
        if (simbolo.second == -1) // Símbolo externo
        {
            arquivoSaida << "U, " << simbolo.first << "\n";
        }
    }

    int linhaAtual = 1;
    int contadorPosicao = 0; // Posição atual no código gerado
    std::string linha;
    bool emSectionText = false;
    bool emSectionData = false;

    // Gera tabela de realocação
    std::ostringstream tabelaRealocacao;

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string rotulo, instrucao, operandos;

        // Verifica e ignora rótulo, se houver
        iss >> rotulo;
        if (!rotulo.empty() && rotulo.back() == ':')
        {
            iss >> instrucao; // Lê a instrução após o rótulo
        }
        else
        {
            instrucao = rotulo;
        }

        // Processa diretivas SECTION
        if (instrucao == "SECTION")
        {
            std::string argumentoSection;
            if (!(iss >> argumentoSection))
            {
                reportarErro("Diretiva SECTION requer um argumento (TEXT ou DATA).", linhaAtual);
            }

            if (argumentoSection == "TEXT")
            {
                emSectionText = true;
                emSectionData = false;
            }
            else if (argumentoSection == "DATA")
            {
                emSectionText = false;
                emSectionData = true;
            }
            else
            {
                reportarErro("Diretiva SECTION inválida: " + argumentoSection, linhaAtual);
            }
            linhaAtual++;
            continue;
        }

        // Processa diretivas
        if (tabelaDiretivas.find(instrucao) != tabelaDiretivas.end())
        {
            if (instrucao == "SPACE")
            {
                if (!emSectionData)
                {
                    reportarErro("Diretiva SPACE fora da SECTION DATA.", linhaAtual);
                }

                int tamanho = 1;
                if (iss >> operandos)
                {
                    tamanho = std::stoi(operandos);
                }
                for (int i = 0; i < tamanho; i++)
                {
                    arquivoSaida << "0 ";
                    tabelaRealocacao << "0 "; // SPACE não requer realocação
                }
                contadorPosicao += tamanho;
            }
            else if (instrucao == "CONST")
            {
                if (!emSectionData)
                {
                    reportarErro("Diretiva CONST fora da SECTION DATA.", linhaAtual);
                }

                if (!(iss >> operandos))
                {
                    reportarErro("Diretiva CONST requer um valor.", linhaAtual);
                }
                arquivoSaida << operandos << " ";
                tabelaRealocacao << "0 "; // CONST não requer realocação
                contadorPosicao++;
            }
            continue;
        }

        // Processa instruções
        if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            if (!emSectionText)
            {
                reportarErro("Instruções devem estar na SECTION TEXT.", linhaAtual);
            }

            const auto &instrucaoInfo = tabelaInstrucoes[instrucao];
            arquivoSaida << instrucaoInfo.opcode << " "; // Escreve opcode
            tabelaRealocacao << "0 ";                    // Opcode não requer realocação

            contadorPosicao++;

            // Processa operandos
            while (std::getline(iss, operandos, ','))
            {
                operandos.erase(std::remove(operandos.begin(), operandos.end(), ' '), operandos.end()); // Remove espaços extras

                bool enderecoRelativo = operandos.find('+') != std::string::npos;
                if (enderecoRelativo)
                {
                    operandos = operandos.substr(0, operandos.find('+'));
                }

                if (tabelaSimbolos.find(operandos) == tabelaSimbolos.end())
                {
                    reportarErro("Símbolo não definido: " + operandos, linhaAtual);
                }

                int endereco = tabelaSimbolos.at(operandos);

                if (endereco == -1) // Símbolo externo
                {
                    // Registra o uso e adiciona um placeholder
                    arquivoSaida << "0 ";     // Placeholder para o ligador
                    tabelaRealocacao << "1 "; // Requer realocação
                }
                else
                {
                    arquivoSaida << endereco + (enderecoRelativo ? 2 : 0) << " ";
                    tabelaRealocacao << "1 "; // Requer realocação
                }

                contadorPosicao++;
            }
            continue;
        }

        linhaAtual++;
    }

    // Escreve tabela de realocação no final do arquivo de saída
    arquivoSaida << "\nR, " << tabelaRealocacao.str() << "\n";

    arquivo.close();
    arquivoSaida.close();
}

void Montador::reportarErro(const std::string &mensagem, int linha)
{
    std::cerr << "Erro na linha " << linha << ": " << mensagem << "\n";
    std::cerr << "Abortando execução...\n";
    exit(EXIT_FAILURE); // Termina o programa após o erro
}
