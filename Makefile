CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRC = src/main.cpp src/Montador.cpp src/Ligador.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = programa

all: $(TARGET)

# Regra para construir o programa principal
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra geral para compilar arquivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(TARGET) $(OBJ)

# Fim do Makefile
