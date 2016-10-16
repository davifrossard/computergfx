# Compilador
CC1 = g++
CC1_FLAGS = -w -Wall
CC2 = g++
CC2_FLAGS = -lGLU -lGL -lglut -lm -w -Wall -std=c++0x
TXML = $(wildcard TinyXML/*.cpp)
TXMLO = $(TXML:.cpp=.o)
ASGC = $(wildcard *.cpp Classes/*.cpp)
EXEC = trabalhocg
RUNARGS = ./

all: $(EXEC)

$(EXEC): $(TXMLO) $(ASGC)
	$(CC2) $(ASGC) $(TXMLO) $(CC2_FLAGS) -o $(EXEC) $(FLAGS)

run: $(EXEC)
	./$(EXEC) $(RUNARGS)

%.o: %.cpp
	$(CC1) -c $(CC1_FLAGS) $(FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(TXMLO)
