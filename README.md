# hyperscanning-backend

Find information about the architecture and examples of BCI2000 Hyperscanning here: [BCI2000 Hyperscanning](https://www.bci2000.org/mediawiki/index.php/BCI2000_Hyperscanning)

##Documentation

###Client
Backend implementation of a client connection.

####Constructor:
`Client::Client( int socket, int timeout, std::string ip, int port )`
`int socket` : File descriptor of the client connection
`int timeout` : no longer in use
`std::string ip` : IPv4 Address of the client
`int port` : Port of the client

####Methods:
`bool Client::GetUpdatedStates()`
Updates the clients internal state machine (aka server-side-client state machine), `Client::states`. Changes are tracked in `Client::stateChanges`. Blocks for 2ms to check if the client wrote to the socket before moving on. Also checks if the client disconnected.

Returns:
false if client disconnected. true otherwise.

`bool Client::SendStates( StateMachine otherStates )`
Updates the client with new states. Sends `otherStates.message()`, preceded by the size of the message.
`StateMachine otherStates` : State machine with states to send to client. All of the states in the state machine will be sent.
returns:
true if states were sent successfully. false otherwise.

`bool Client::Matches( Client* o )`
returns ture if `Client* o` has the same IP Address and Port as it

####Members
`int Client::connection` : File descriptor of client connection
`std::string Client::ip_address` : IP Address of client connected
`int Client::port` : Port of client connected
`char Client::ClientNumber` : ClientNumber id of the connected client
`StateMachine* Client::states` : Server-side-client state machine
`StateMachine* Client::stateChanges` : Tracker for `Client::states`

###Game
Object that connects `Client`s and runs the game loop for them

####Constructor
`Game::Game( Port port, std::string params )`
`Port port` : Port object that this games clients will connect to
`std::string params` : `std::string` containing BCI2000 parameter file to be sent to each client

####Methods
`bool Game::Connect( Client* client )`
Connects a client to the game. Calls `Game::ConnectClient()` and `Game::ValidateStates()` with given client and adds them to the list of connected clients.
`Client* client` : Client object to connect to the game
returns
true unless client does not exist.

`void Game::ConnectClient( Client* client )`
Sends greeting message with client number id and BCI2000 parameters defined in `Client::params`. 
`Client* client` : Client to connect to the game

`void Game::ValidateStates( Client* client )`
Waits for client to send shared states list. Validates that this client has the same shared states as the other client(s) or saves this clients shared states as the standard if it is the first one.
`Client* client` : Client to validate

`Game::AddClient( Client* client )`
Adds a client to the list of clients connected to the game
`Client* client` : Client to add

`StateMachine Game::Loop()`
Executes `Game::Update` untill a client disconnects
returns:
`Game::masterStates`

`bool Game::Update()`
Executes one iterance of the server loop. Reads clients, reconciles, and sends client updates.
returns:
true unless client disconnected

####Members
`Port Game::port` : (unused) Port object from which this game's clients are connecting
`std::string Game::params`: `std::string` containing BCI2000 parameters to be sent to each client during `Game::ConnectClient`
`std::string Game::sharedStates` : list of shared states used to validate clients when they are connecting
`std::vector<Client*> Game::clients` : list of connected clients
`StateMachine Game::masterStates` : Master state machine of the game

###Param
One BCI2000 parameter line

####Constructor
`Param::Param( std::string line, std::string name, std::string value, int length = 1, int width = 1 )`
`std::string line` : Everything after BCI2000 parameter "="
`std::string name` : Name of the BCI2000 parameter
`std::string value` : value of the BCI2000 parameter
`int length` : length of the BCI2000 parameter if list or matrix (default 1)
`int width` : width of the BCI2000 parameter if matrix (default 1)

####Members
`std::string Param::line` : Everything after BCI2000 parameter "="
`std::string Param::name` : Name of the BCI2000 parameter
`std::string Param::value` : value of the BCI2000 parameter
`int Param::length` : length of the BCI2000 parameter if list or matrix (default 1)
`int Param::width` : width of the BCI2000 parameter if matrix (default 1)



