CC = g++
LIBS = 
LIBDIRS = 
INC = 

OBJ = port.o client.o statemachine.o game.o params.o

%.o : %.cpp
	$(CC) -c $(LIBS) $(LIBDIRS) $(INC) $< -o $@

all: application base_application modify_params save_trial edit_states poker


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

poker: $(OBJ)
	$(CC) examples/poker.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -g -o poker

app1P: $(OBJ)
	$(CC) examples/app1p.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -g -o app1P

pong: $(OBJ)
	$(CC) examples/pong.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -g -o pong

tictactoe: $(OBJ)
	$(CC) examples/tictactoe.cpp -I. $(OBJ) $(LIBS) $(LIBDIRS) $(INC) -g -o tictactoe
