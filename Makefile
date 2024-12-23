# Variáveis
CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

# Fontes e objetos
SRC_MONTADOR = src/main_montador.cpp src/Montador.cpp
SRC_LIGADOR = src/main_ligador.cpp src/Ligador.cpp
OBJ_MONTADOR = $(SRC_MONTADOR:.cpp=.o)
OBJ_LIGADOR = $(SRC_LIGADOR:.cpp=.o)

# Executáveis
EXEC_MONTADOR = montador
EXEC_LIGADOR = ligador

# Alvo principal: compilar ambos
all: $(EXEC_MONTADOR) $(EXEC_LIGADOR)

# Compilar o montador
$(EXEC_MONTADOR): $(OBJ_MONTADOR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_MONTADOR)

# Compilar o ligador
$(EXEC_LIGADOR): $(OBJ_LIGADOR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_LIGADOR)

# Regras para compilar arquivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(EXEC_MONTADOR) $(EXEC_LIGADOR) src/*.o
