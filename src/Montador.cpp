#include "../include/Montador.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <regex>
#include <cctype>

// ============================================================================
// Estrutura de informações de instrução
// Agora armazenamos também o número de operandos esperado (numOperandos).
// ----------------------------------------------------------------------------
struct InstrucaoInfo
{
    int opcode;
    int tamanho;      // Quantas palavras de memória esta instrução ocupa
    int numOperandos; // Quantos operandos a instrução deve receber
};

// ============================================================================
// Funções auxiliares para validação
// ----------------------------------------------------------------------------

// Verifica se um rótulo (label) é válido conforme as regras:
// 1) Não pode começar com número
// 2) Pode ter letras, dígitos ou underscore
// 3) Pelo menos 1 caractere
bool Montador::rotuloValido(const std::string &rotulo)
{
    if (rotulo.empty())
        return false;

    // Primeiro caractere deve ser letra ou underscore
    if (!std::isalpha(rotulo[0]) && rotulo[0] != '_')
        return false;

    // Demais podem ser alfanuméricos ou underscore
    for (size_t i = 1; i < rotulo.size(); i++)
    {
        if (!std::isalnum(rotulo[i]) && rotulo[i] != '_')
            return false;
    }
    return true;
}

std::string trim(const std::string &str)
{
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos)
    {
        return "";
    }
    return str.substr(start, end - start + 1);
}

// Verifica se um valor de CONST é válido (aceita decimal +/-, ou hexa 0x...).
bool Montador::valorConstValido(const std::string &str)
{
    std::string s = trim(str);
    if (s.empty())
        return false;

    // Remove o sinal '-' caso exista,
    // sem salvar em nenhuma variável
    if (s[0] == '-')
    {
        s.erase(0, 1);
        if (s.empty())
            return false;
    }

    // Se começa com "0x" ou "0X", deve ser hex
    if (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    {
        s.erase(0, 2);
        if (s.empty())
            return false;
        for (char c : s)
        {
            if (!std::isxdigit(c))
                return false;
        }
        return true; // valor hexa válido
    }
    else
    {
        // Verifica se é decimal (só dígitos)
        for (char c : s)
        {
            if (!std::isdigit(c))
                return false;
        }
        return true; // valor decimal válido
    }
}

// Converte string (que já validamos como decimal ou hex) para inteiro.
int Montador::converterStringParaInt(const std::string &str)
{
    std::string s = trim(str);
    bool negative = false;
    if (!s.empty() && s[0] == '-')
    {
        negative = true;
        s.erase(0, 1);
    }

    int base = 10;
    // Se começa com 0x/0X, converte como hexa
    if (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    {
        base = 16;
        s.erase(0, 2);
    }

    int valor = std::stoi(s, nullptr, base);
    return (negative ? -valor : valor);
}

// ============================================================================
// Inicialização das tabelas de instruções e diretivas
// Agora incluindo o número de operandos esperado no construtor da instruçãoInfo.
// ----------------------------------------------------------------------------
void Montador::inicializarTabelaInstrucoes()
{
    //                  nome       { opcode, tamanhoMem, numOperandos }
    tabelaInstrucoes = {
        {"ADD", {1, 2, 1}},
        {"SUB", {2, 2, 1}},
        {"MUL", {3, 2, 1}},
        {"DIV", {4, 2, 1}},
        {"JMP", {5, 2, 1}},
        {"JMPN", {6, 2, 1}},
        {"JMPP", {7, 2, 1}},
        {"JMPZ", {8, 2, 1}},
        {"COPY", {9, 3, 2}}, // COPY ocupa 3 palavras (opcode + 2 operandos)
        {"LOAD", {10, 2, 1}},
        {"STORE", {11, 2, 1}},
        {"INPUT", {12, 2, 1}},
        {"OUTPUT", {13, 2, 1}},
        {"STOP", {14, 1, 0}} // STOP não tem operandos
    };
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

// ============================================================================
// Função principal de execução do Montador
// ----------------------------------------------------------------------------
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

// ============================================================================
// Funções utilitárias de manipulação de strings
// ----------------------------------------------------------------------------

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

void Montador::expandirLinha(
    const std::string &linha,
    const std::unordered_map<std::string, MacroDefinition> &MNT,
    std::vector<std::string> &linhasExpandidas)
{
    // Convertemos a linha para maiúsculas para padronizar buscas na MNT
    std::string upperLine = upperCase(trim(linha));

    // Verifica se a linha inteira corresponde ao nome de alguma macro
    auto it = MNT.find(upperLine);
    if (it != MNT.end())
    {
        // É macro. Para cada linha do corpo, chamamos recursivamente expandirLinha.
        for (const auto &subLine : it->second.linhas)
        {
            expandirLinha(subLine, MNT, linhasExpandidas);
        }
    }
    else
    {
        // Não é macro, então apenas adicionamos esta linha como está.
        linhasExpandidas.push_back(linha);
    }
}

// ============================================================================
// Pré-processamento (gera .pre) - remove comentários, normaliza espaços, etc.
// ----------------------------------------------------------------------------
void Montador::preProcessar(const std::string &arquivoEntrada, const std::string &arquivoSaida)
{
    std::ifstream arquivo(arquivoEntrada);
    std::ofstream arquivoPre(arquivoSaida);

    if (!arquivo.is_open() || !arquivoPre.is_open())
    {
        std::cerr << "Erro ao abrir arquivos para pré-processamento.\n";
        return;
    }

    // Limpa a tabela de macros da classe, se quiser começar do zero
    MNT.clear();

    bool dentroDeMacro = false;
    std::string macroAtual;
    MacroDefinition macroTemp; // <= do tipo da classe Montador (mesmo nome)

    std::vector<std::string> linhasExpandidas;

    std::string linha;
    while (std::getline(arquivo, linha))
    {
        // 1) Remover comentários
        size_t posComent = linha.find(';');
        if (posComent != std::string::npos)
        {
            linha = linha.substr(0, posComent);
        }
        linha = trim(linha);
        if (linha.empty())
            continue;

        // 2) Se estamos definindo uma macro
        if (dentroDeMacro)
        {
            if (upperCase(trim(linha)) == "ENDMACRO")
            {
                // Insere no MNT (da classe) usando uppercase como chave
                MNT[upperCase(macroAtual)] = macroTemp;

                dentroDeMacro = false;
                macroAtual.clear();
                macroTemp.linhas.clear();
            }
            else
            {
                macroTemp.linhas.push_back(linha);
            }
            continue;
        }

        // 3) Detectar "NOME: MACRO" ou "NOME MACRO"
        {
            std::istringstream iss(linha);
            std::string token1, token2;
            iss >> token1;

            bool definindoMacro = false;
            if (!token1.empty() && token1.back() == ':')
            {
                token1.pop_back();
                iss >> token2;
                if (upperCase(token2) == "MACRO")
                {
                    definindoMacro = true;
                    macroAtual = token1;
                }
            }
            else
            {
                iss >> token2;
                if (upperCase(token2) == "MACRO")
                {
                    definindoMacro = true;
                    macroAtual = token1;
                }
            }

            if (definindoMacro)
            {
                dentroDeMacro = true;
                macroTemp.linhas.clear();
                continue;
            }
        }

        // 4) Linha que não define macro -> expandir (recursivo) para linhasExpandidas
        expandirLinha(linha, MNT, linhasExpandidas);
    }

    arquivo.close();

    // 5) A partir daqui, seu pré-processamento "antigo" (SECTION TEXT, etc.)
    std::string sectionText, sectionData;
    std::string labelPendente;
    for (auto &rawLine : linhasExpandidas)
    {
        rawLine = removeSpaces(rawLine);
        rawLine = superTrim(rawLine);
        rawLine = upperCase(rawLine);

        if (rawLine == "SECTION TEXT")
        {
            sectionText += "SECTION TEXT\n";
            continue;
        }
        if (rawLine == "SECTION DATA")
        {
            sectionData += "SECTION DATA\n";
            continue;
        }

        if (!rawLine.empty() && rawLine.back() == ':' && rawLine.find(' ') == std::string::npos)
        {
            labelPendente = rawLine;
            continue;
        }

        if (!labelPendente.empty())
        {
            rawLine = labelPendente + " " + rawLine;
            labelPendente.clear();
        }

        if (sectionData.empty())
            sectionText += rawLine + "\n";
        else
            sectionData += rawLine + "\n";
    }

    if (!sectionData.empty() && sectionData.back() == '\n')
    {
        sectionData.pop_back();
    }

    arquivoPre << sectionText << sectionData;
    arquivoPre.close();
}

// ============================================================================
// Montar (chama primeira e segunda passagem, ou gera .obj direto se não tiver BEGIN/END)
// ----------------------------------------------------------------------------
void Montador::montar(const std::string &arquivoPre, const std::string &arquivoObj)
{
    std::unordered_map<std::string, int> tabelaSimbolos;

    // Primeira passagem
    primeiraPassagem(arquivoPre, tabelaSimbolos);

    // Verifica se BEGIN e END estão presentes
    std::ifstream arquivo(arquivoPre);
    bool possuiBeginEnd = false;
    std::string linha;
    while (std::getline(arquivo, linha))
    {
        if (linha.find("BEGIN") != std::string::npos || linha.find("END") != std::string::npos)
        {
            possuiBeginEnd = true;
            break;
        }
    }
    arquivo.close();

    // Se tiver BEGIN/END, gera objeto com as infos de ligacao
    if (possuiBeginEnd)
    {
        segundaPassagem(arquivoPre, arquivoObj, tabelaSimbolos);
    }
    else
    {
        // Gera .obj simples (sem infos de ligacao)
        std::ifstream entrada(arquivoPre);
        if (!entrada.is_open())
        {
            std::cerr << "Erro ao abrir arquivo de entrada.\n";
            return;
        }

        std::ofstream saida(arquivoObj);
        if (!saida.is_open())
        {
            std::cerr << "Erro ao abrir arquivo de saída.\n";
            return;
        }

        std::ostringstream codigoMontado;
        while (std::getline(entrada, linha))
        {
            std::istringstream iss(linha);
            std::string rotulo, instrucao, operandos;

            // Se há rótulo
            iss >> rotulo;
            if (!rotulo.empty() && rotulo.back() == ':')
            {
                iss >> instrucao;
            }
            else
            {
                instrucao = rotulo;
            }

            // Se a instrução faz parte da tabela de instruções
            if (tabelaInstrucoes.find(instrucao) != tabelaInstrucoes.end())
            {
                const auto &info = tabelaInstrucoes[instrucao];
                codigoMontado << info.opcode << " ";

                // Coleta operandos
                std::vector<std::string> listaOperandos;
                while (std::getline(iss, operandos, ','))
                {
                    operandos = trim(operandos);
                    listaOperandos.push_back(operandos);
                }

                // Verifica número de operandos
                if ((int)listaOperandos.size() != info.numOperandos)
                {
                    reportarErro("Número de operandos incorreto para a instrução " + instrucao, 0);
                }

                // Para cada operando, converte para posição (com suporte a N3+1)
                for (auto &op : listaOperandos)
                {
                    bool temMais = (op.find('+') != std::string::npos);
                    int desloc = 0;
                    if (temMais)
                    {
                        size_t plusPos = op.find('+');
                        std::string deslocStr = op.substr(plusPos + 1);
                        desloc = std::stoi(deslocStr);
                        // "op" passa a ser só "N3"
                        op = op.substr(0, plusPos);
                    }

                    if (tabelaSimbolos.find(op) != tabelaSimbolos.end())
                    {
                        // Soma o deslocamento
                        int baseAddr = tabelaSimbolos[op];
                        codigoMontado << (baseAddr + desloc) << " ";
                    }
                    else
                    {
                        reportarErro("Símbolo não definido: " + op, 0);
                    }
                }
            }
            else if (instrucao == "CONST")
            {
                iss >> operandos;
                // Valida e converte
                if (!valorConstValido(operandos))
                {
                    reportarErro("Valor inválido em CONST: " + operandos, 0);
                }
                int val = converterStringParaInt(operandos);
                codigoMontado << val << " ";
            }
            else if (instrucao == "SPACE")
            {
                // Verifica se há argumento (ex: SPACE 2)
                std::string argSpace;
                // Tente ler um possível valor depois de SPACE
                if (iss >> argSpace)
                {
                    int n = std::stoi(argSpace); // Converta
                    // Gere n vezes "0 "
                    for (int i = 0; i < n; i++)
                    {
                        codigoMontado << "0 ";
                    }
                }
                else
                {
                    // Se não houver argumento, gere apenas 1 espaço
                    codigoMontado << "0 ";
                }
            }
            // Ignora outras diretivas (BEGIN, END, etc.) aqui,
            // pois neste modo elas não são usadas.
        }

        saida << codigoMontado.str() << "\n";
        entrada.close();
        saida.close();
    }
}

// ============================================================================
// Primeira Passagem - constroi tabela de símbolos e detecta erros iniciais
// ----------------------------------------------------------------------------
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
        std::string maybeRotulo, instrucao;
        iss >> maybeRotulo;

        // Se termina com ':', é um rótulo
        if (!maybeRotulo.empty() && maybeRotulo.back() == ':')
        {
            maybeRotulo.pop_back(); // remove ':'

            // Valida rótulo
            if (!rotuloValido(maybeRotulo))
            {
                reportarErro("Rótulo inválido: " + maybeRotulo, linhaAtual);
            }

            // Verifica redefinição
            if (tabelaSimbolos.count(maybeRotulo))
            {
                reportarErro("Rótulo redefinido: " + maybeRotulo, linhaAtual);
            }

            // Lê próxima token como instrucao
            iss >> instrucao;

            // Se for EXTERN
            if (instrucao == "EXTERN")
            {
                // Marca como externo
                tabelaSimbolos[maybeRotulo] = -1;
                linhaAtual++;
                continue;
            }
            else
            {
                // Armazena rótulo com posição
                tabelaSimbolos[maybeRotulo] = contadorPosicao;
            }
        }
        else
        {
            // Se não é rótulo, a "instrucao" é esse token
            instrucao = maybeRotulo;
        }

        // Se for EXTERN sem rótulo
        if (instrucao == "EXTERN")
        {
            // Espera um rótulo como argumento
            std::string argumento;
            if (!(iss >> argumento))
            {
                reportarErro("Diretiva EXTERN requer um rótulo.", linhaAtual);
            }
            if (tabelaSimbolos.count(argumento))
            {
                reportarErro("Rótulo redefinido em EXTERN: " + argumento, linhaAtual);
            }
            tabelaSimbolos[argumento] = -1;
            linhaAtual++;
            continue;
        }

        // Se é instrução
        if (tabelaInstrucoes.count(instrucao))
        {
            // Incrementa PC pelo tamanho da instrução
            contadorPosicao += tabelaInstrucoes[instrucao].tamanho;
        }
        // Se é diretiva
        else if (tabelaDiretivas.count(instrucao))
        {
            if (instrucao == "CONST")
            {
                contadorPosicao++; // Cada um ocupa 1 palavra
            }
            else if (instrucao == "SPACE")
            {
                std::string argumento; // <--- Adicione esta linha
                int num = 1;           // se não tiver argumento, default = 1
                if (iss >> argumento)
                {
                    // converta para int
                    num = std::stoi(argumento);
                }
                // Some esse valor ao contadorPosicao
                contadorPosicao += num;
            }
            // Seção TEXT ou DATA não incrementa
        }
        else if (instrucao == "SECTION")
        {
            std::string argumentoSection;
            if (!(iss >> argumentoSection) ||
                (argumentoSection != "TEXT" && argumentoSection != "DATA"))
            {
                reportarErro("Diretiva SECTION inválida.", linhaAtual);
            }
        }
        else if (!instrucao.empty())
        {
            // Instrução ou diretiva inválida
            reportarErro("Instrução ou diretiva inválida: " + instrucao, linhaAtual);
        }

        linhaAtual++;
    }

    arquivo.close();
}

// ============================================================================
// Segunda Passagem - gera .obj completo (tabela de uso, defs, realocação, etc.)
// ----------------------------------------------------------------------------
void Montador::segundaPassagem(const std::string &arquivoPre,
                               const std::string &arquivoObj,
                               const std::unordered_map<std::string, int> &tabelaSimbolos)
{
    std::ifstream arquivo(arquivoPre);
    std::ofstream arquivoSaida(arquivoObj);

    if (!arquivo.is_open() || !arquivoSaida.is_open())
    {
        std::cerr << "Erro ao abrir arquivos na segunda passagem.\n";
        return;
    }

    std::ostringstream bufferDefinicoes;
    std::ostringstream bufferTabelaUso;
    std::ostringstream bufferTabelaRealocacao;
    std::ostringstream bufferCodigoObjeto;

    // Adiciona símbolos locais à tabela de definições
    for (const auto &simbolo : tabelaSimbolos)
    {
        if (simbolo.second >= 0) // Apenas locais
        {
            bufferDefinicoes << "D, " << simbolo.first << " " << simbolo.second << "\n";
        }
    }

    int contadorPosicao = 0;
    std::unordered_map<std::string, std::vector<int>> usoSimbolos;

    std::string linha;
    int linhaAtual = 1;

    while (std::getline(arquivo, linha))
    {
        std::istringstream iss(linha);
        std::string talvezRotulo, instrucao;

        iss >> talvezRotulo;
        if (!talvezRotulo.empty() && talvezRotulo.back() == ':')
        {
            iss >> instrucao; // Rótulo encontrado
        }
        else
        {
            instrucao = talvezRotulo;
        }

        if (tabelaDiretivas.count(instrucao))
        {
            if (instrucao == "SPACE")
            {
                int tamanho = 1;
                std::string argumento;
                if (iss >> argumento)
                {
                    tamanho = std::stoi(argumento);
                }
                for (int i = 0; i < tamanho; ++i)
                {
                    bufferCodigoObjeto << "0 ";
                    bufferTabelaRealocacao << "0 ";
                    contadorPosicao++;
                }
            }
            else if (instrucao == "CONST")
            {
                std::string valor;
                if (!(iss >> valor))
                {
                    reportarErro("CONST requer um valor.", linhaAtual);
                }
                bufferCodigoObjeto << converterStringParaInt(valor) << " ";
                bufferTabelaRealocacao << "0 ";
                contadorPosicao++;
            }
            linhaAtual++;
            continue;
        }

        if (tabelaInstrucoes.count(instrucao))
        {
            const auto &infoInstrucao = tabelaInstrucoes.at(instrucao);

            bufferCodigoObjeto << infoInstrucao.opcode << " ";
            bufferTabelaRealocacao << "0 ";
            contadorPosicao++;

            std::vector<std::string> operandos;
            std::string operando;
            while (std::getline(iss, operando, ','))
            {
                operando = trim(operando);
                operandos.push_back(operando);
            }

            if ((int)operandos.size() != infoInstrucao.numOperandos)
            {
                reportarErro("Número de operandos incorreto para " + instrucao, linhaAtual);
            }

            for (auto &op : operandos)
            {
                int deslocamento = 0;
                if (op.find('+') != std::string::npos)
                {
                    size_t pos = op.find('+');
                    deslocamento = std::stoi(op.substr(pos + 1));
                    op = op.substr(0, pos);
                }

                if (tabelaSimbolos.count(op) == 0)
                {
                    usoSimbolos[op].push_back(contadorPosicao);
                    bufferCodigoObjeto << deslocamento << " ";
                    bufferTabelaRealocacao << "0 ";
                }
                else
                {
                    int enderecoBase = tabelaSimbolos.at(op);
                    if (enderecoBase == -1)
                    {
                        usoSimbolos[op].push_back(contadorPosicao);
                        bufferCodigoObjeto << deslocamento << " ";
                        bufferTabelaRealocacao << "0 ";
                    }
                    else
                    {
                        bufferCodigoObjeto << (enderecoBase + deslocamento) << " ";
                        bufferTabelaRealocacao << "1 ";
                    }
                }
                contadorPosicao++;
            }
        }
        linhaAtual++;
    }

    for (const auto &uso : usoSimbolos)
    {
        for (int endereco : uso.second)
        {
            bufferTabelaUso << "U, " << uso.first << " " << endereco << "\n";
        }
    }

    arquivoSaida << bufferDefinicoes.str();
    arquivoSaida << bufferTabelaUso.str();
    arquivoSaida << "R, " << bufferTabelaRealocacao.str() << "\n";
    arquivoSaida << bufferCodigoObjeto.str() << "\n";

    arquivo.close();
    arquivoSaida.close();
}

// ============================================================================
// Relatar erro e abortar
// ----------------------------------------------------------------------------
void Montador::reportarErro(const std::string &mensagem, int linha)
{
    std::cerr << "Erro na linha " << linha << ": " << mensagem << "\n";
    std::cerr << "Abortando execução...\n";
    exit(EXIT_FAILURE);
}