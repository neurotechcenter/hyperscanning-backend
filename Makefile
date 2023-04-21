CC = g++
LIBS = 
LIBDIRS = 
INC = 

OBJ = port.o client.o statemachine.o game.o

%.o : %.cpp
	$(CC) -c $(LIBS) $(LIBDIRS) $(INC) $< -o $@

all: $(OBJ)
	$(CC) application.cpp $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o application
