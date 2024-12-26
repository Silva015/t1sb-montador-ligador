CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRC_MONTADOR = src/main_montador.cpp src/Montador.cpp
SRC_LIGADOR = src/main_ligador.cpp src/Ligador.cpp

OBJ_MONTADOR = $(SRC_MONTADOR:.cpp=.o)
OBJ_LIGADOR = $(SRC_LIGADOR:.cpp=.o)

all: montador ligador

montador: $(OBJ_MONTADOR)
	$(CXX) $(CXXFLAGS) -o montador $(OBJ_MONTADOR)

ligador: $(OBJ_LIGADOR)
	$(CXX) $(CXXFLAGS) -o ligador $(OBJ_LIGADOR)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f montador ligador $(OBJ_MONTADOR) $(OBJ_LIGADOR)
