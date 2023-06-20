CC = g++
LIBS = 
LIBDIRS = 
INC = 

OBJ = port.o client.o statemachine.o game.o params.o

%.o : %.cpp
	$(CC) -c $(LIBS) $(LIBDIRS) $(INC) $< -o $@

all: application base_application modify_params save_trial edit_states


application: $(OBJ)
	$(CC) application.cpp $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o application

base_application: $(OBJ)
	$(CC) examples/base_application.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o base_application

modify_params: $(OBJ)
	$(CC) examples/modify_params.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o modify_params

save_trial: $(OBJ)
	$(CC) examples/save_trial.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o save_trial

edit_states: $(OBJ)
	$(CC) examples/edit_states.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -o edit_states
