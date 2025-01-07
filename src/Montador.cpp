#include "../include/Montador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <regex>
#include <cctype>

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

std::string trim(const std::string &str)
{
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string upperCase(const std::string &str)
{
    std::string result;
    for (char c : str)
    {
        result += std::toupper(c);
    }
    return result;
}

std::string removeSpaces(std::string &str)
{
    auto res = std::unique(str.begin(), str.end(), [](char lhs, char rhs)
                           { return (lhs == rhs) && (lhs == ' '); });
    str.erase(res, str.end());
    return str;
}

std::string superTrim(const std::string &str)
{
    auto trimmed = trim(str);
    size_t pos = trimmed.find(',');
    if (pos != std::string::npos)
    {
        auto before = trim(trimmed.substr(0, pos));
        auto after = trim(trimmed.substr(pos + 1));
        return before + ", " + after;
    }
    return trimmed;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
    size_t pos_start = 0, pos_end;
    std::vector<std::string> tokens;
    while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos)
    {
        tokens.push_back(trim(str.substr(pos_start, pos_end - pos_start)));
        pos_start = pos_end + delimiter.length();
    }
    tokens.push_back(trim(str.substr(pos_start)));
    return tokens;
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
    std::string sectionText, sectionData;
    std::string labelPendente = ""; // Armazena rótulos sozinhos temporariamente

    while (std::getline(arquivo, linha))
    {
        // Remove comentários
        linha = linha.substr(0, linha.find(';'));
        linha = trim(linha); // Remove espaços extras no início e no fim

        if (linha.empty())
            continue; // Ignorar linhas vazias

        // Detectar e organizar seções
        if (linha == "SECTION TEXT")
        {
            sectionText += "SECTION TEXT\n";
            continue;
        }

        if (linha == "SECTION DATA")
        {
            sectionData += "SECTION DATA\n";
            continue;
        }

        // Normalizar espaços internos e formatar a linha
        linha = removeSpaces(linha);
        linha = superTrim(linha);
        linha = upperCase(linha); // Converte para caixa alta

        // Verificar se é um rótulo sozinho
        if (linha.back() == ':' && linha.find(' ') == std::string::npos)
        {
            labelPendente = linha; // Armazena o rótulo para combinar com a próxima linha
            continue;
        }

        // Combinar rótulo pendente com a instrução
        if (!labelPendente.empty())
        {
            linha = labelPendente + " " + linha;
            labelPendente = "";
        }

        // Adicionar à seção apropriada
        if (sectionData.empty())
        {
            sectionText += linha + "\n";
        }
        else
        {
            sectionData += linha + "\n";
        }
    }

    // Remove a última nova linha desnecessária em sectionData
    if (!sectionData.empty() && sectionData.back() == '\n')
    {
        sectionData.pop_back();
    }

    // Gravar no arquivo pré-processado
    arquivoPre << sectionText << sectionData;

    arquivo.close();
    arquivoPre.close();

    std::cout << "Arquivo pré-processado gerado em: " << arquivoSaida << "\n";
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

    // Buffers para armazenar diferentes partes do arquivo de saída
    std::ostringstream bufferDefinicoes;
    std::ostringstream bufferCodigoObjeto;
    std::ostringstream bufferTabelaUso;
    std::ostringstream bufferTabelaRealocacao;

    // Adiciona símbolos públicos à tabela de definições no buffer
    for (const auto &simbolo : tabelaSimbolos)
    {
        if (simbolo.second >= 0) // Símbolos definidos localmente
        {
            bufferDefinicoes << "D, " << simbolo.first << " " << simbolo.second << "\n";
        }
    }

    int contadorPosicao = 0; // Contador de posição inicializado
    std::unordered_map<std::string, std::vector<int>> usoSimbolos;

    std::string linha;
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

        // Processa diretivas
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
                    bufferCodigoObjeto << "0 ";
                    bufferTabelaRealocacao << "0 ";
                    contadorPosicao++;
                }
            }
            else if (instrucao == "CONST")
            {
                if (!(iss >> operandos))
                {
                    reportarErro("Diretiva CONST requer um valor.", contadorPosicao);
                }
                bufferCodigoObjeto << operandos << " ";
                bufferTabelaRealocacao << "0 ";
                contadorPosicao++;
            }
            continue;
        }

        // Processa instruções
        if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
        {
            const auto &instrucaoInfo = tabelaInstrucoes[instrucao];
            bufferCodigoObjeto << instrucaoInfo.opcode << " "; // Escreve opcode
            bufferTabelaRealocacao << "0 ";                    // Opcode não requer realocação
            contadorPosicao++;                                 // Incrementa posição para o opcode

            // Processa operandos
            while (std::getline(iss, operandos, ','))
            {
                operandos.erase(std::remove(operandos.begin(), operandos.end(), ' '), operandos.end()); // Remove espaços extras

                bool enderecoRelativo = operandos.find('+') != std::string::npos;
                int deslocamento = 0;
                if (enderecoRelativo)
                {
                    deslocamento = std::stoi(operandos.substr(operandos.find('+') + 1));
                    operandos = operandos.substr(0, operandos.find('+'));
                }

                if (tabelaSimbolos.find(operandos) == tabelaSimbolos.end())
                {
                    reportarErro("Símbolo não definido: " + operandos, contadorPosicao);
                }

                int endereco = tabelaSimbolos.at(operandos);
                if (endereco == -1) // Símbolo externo
                {
                    usoSimbolos[operandos].push_back(contadorPosicao);
                    bufferCodigoObjeto << deslocamento << " "; // Placeholder para o ligador, ajustado com deslocamento
                    bufferTabelaRealocacao << "1 ";            // Requer realocação
                }
                else
                {
                    bufferCodigoObjeto << (endereco + deslocamento) << " ";
                    bufferTabelaRealocacao << "1 "; // Requer realocação
                }
                contadorPosicao++; // Incrementa posição para o operando
            }
        }
    }

    // Gera tabela de uso no buffer
    for (const auto &[simbolo, enderecos] : usoSimbolos)
    {
        for (int endereco : enderecos)
        {
            bufferTabelaUso << "U, " << simbolo << " " << endereco << "\n";
        }
    }

    // Escreve buffers no arquivo de saída na ordem correta
    arquivoSaida << bufferDefinicoes.str();
    arquivoSaida << bufferTabelaUso.str();
    arquivoSaida << "R, " << bufferTabelaRealocacao.str() << "\n";
    arquivoSaida << bufferCodigoObjeto.str() << "\n";

    arquivo.close();
    arquivoSaida.close();
}

void Montador::reportarErro(const std::string &mensagem, int linha)
{
    std::cerr << "Erro na linha " << linha << ": " << mensagem << "\n";
    std::cerr << "Abortando execução...\n";
    exit(EXIT_FAILURE); // Termina o programa após o erro
}
