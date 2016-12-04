# Compilador
CC1 = g++
CC1_FLAGS = -w -Wall -std=c++11 -lm
CC2 = g++
CC2_FLAGS = -lGLU -lGL -lglut -lm -w -Wall -std=c++11 -lpthread `pkg-config --cflags --libs opencv`
TXML = $(wildcard TinyXML/*.cpp)
TXMLO = $(TXML:.cpp=.o)
LDR = $(wildcard Loader/*.cpp)
LDRO = $(LDR:.cpp=.o)
ASGC = $(wildcard *.cpp)
EXEC = trabalhocg
RUNARGS = ./

all: $(EXEC)

$(EXEC): $(TXMLO) $(ASGC) $(LDRO)
	$(CC2) $(ASGC) $(TXMLO) $(LDRO) $(CC2_FLAGS) -o $(EXEC) $(FLAGS)

run: $(EXEC)
	./$(EXEC) $(RUNARGS)

%.o: %.cpp
	$(CC1) -c $(CC1_FLAGS) $(FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(TXMLO) $(LRDO)
