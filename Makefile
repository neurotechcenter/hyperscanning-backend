CC = g++
LIBS = 
LIBDIRS = 
INC = 

OBJ = port.o client.o statemachine.o game.o params.o

%.o : %.cpp
	$(CC) -c $(LIBS) $(LIBDIRS) $(INC) $< -o $@

all: $(OBJ)
	$(CC) application.cpp $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o application

threep: $(OBJ)
	$(CC) application3P.cpp $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o application3P
