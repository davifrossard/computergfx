# Compilador
CC1 = g++
CC1_FLAGS = -w -Wall -std=c++11 -lm
CC2 = g++
CC2_FLAGS = -lGLU -lGL -lglut -lm -w -Wall -std=c++11
CC3_FLAGS = -lGLU -lGL -lglut -lm -w -Wall -std=c++11 -lpthread `pkg-config --cflags --libs opencv`
TXML = $(wildcard Includes/TinyXML/*.cpp)
TXMLO = $(TXML:.cpp=.o)
LDR = $(wildcard Includes/Loader/*.cpp)
LDRO = $(LDR:.cpp=.o)
ASGC = $(wildcard Includes/*.cpp)
ASGO = $(ASGC:.cpp=.o)
CVF = main_cv.cpp minimal_cv.cpp
CVFO = $(CVFILES:.cpp=.o)
DFLT = main.cpp
EXEC = trabalhocg
EXEC2 = trabalhocgcv
RUNARGS = ./

all: $(EXEC)

cv: $(EXEC2)

$(EXEC): $(TXMLO) $(ASGO) $(LDRO) $(DFLT)
	$(CC2) $(DFLT) $(ASGO) $(TXMLO) $(LDRO) $(CC2_FLAGS) -o $(EXEC)

$(EXEC2): $(TXMLO) $(ASGO) $(LDRO) $(CVF)
	$(CC2) $(CVF) $(ASGO) $(TXMLO) $(LDRO) $(CC3_FLAGS) -o $(EXEC2)

run: $(EXEC)
	./$(EXEC) $(RUNARGS)

runcv: $(EXEC2)
	./$(EXEC2) $(RUNARGS)

%.o: %.cpp
	$(CC1) -c $(CC1_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(EXEC2) $(TXMLO) $(ASGO) $(LDRO)
