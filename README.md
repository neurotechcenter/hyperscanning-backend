# hyperscanning-backend

Find information about the architecture and examples of BCI2000 Hyperscanning here: [BCI2000 Hyperscanning](https://www.bci2000.org/mediawiki/index.php/BCI2000_Hyperscanning)

# Documentation

# Client
Backend implementation of a client connection.

## Constructor:
### `Client::Client( int socket, int timeout, std::string ip, int port )`  
`int socket` : File descriptor of the client connection  
`int timeout` : no longer in use  
`std::string ip` : IPv4 Address of the client  
`int port` : Port of the client  

## Methods:
### `bool Client::GetUpdatedStates()`  
Updates the clients internal state machine (aka server-side-client state machine), `Client::states`. Changes are tracked in `Client::stateChanges`. Blocks for 2ms to check if the client wrote to the socket before moving on. Also checks if the client disconnected.\

Returns:  
false if client disconnected. true otherwise.  

---

### `bool Client::SendStates( StateMachine otherStates )`  
Updates the client with new states. Sends `otherStates.message()`, preceded by the size of the message.  
`StateMachine otherStates` : State machine with states to send to client. All of the states in the state machine will be sent.  
returns:  
true if states were sent successfully. false otherwise.  

---

### `bool Client::Matches( Client* o )`  
returns ture if `Client* o` has the same IP Address and Port as it  

## Members
### `int Client::connection` 
File descriptor of client connection  

---

### `std::string Client::ip_address` 
IP Address of client connected   

---

### `int Client::port` 
Port of client connected  

---

### `char Client::ClientNumber` 
ClientNumber id of the connected client  

---

### `StateMachine* Client::states` 
Server-side-client state machine  

---

### `StateMachine* Client::stateChanges` 
Tracker for `Client::states`  

# Game
Object that connects `Client`s and runs the game loop for them

## Constructor
### `Game::Game( Port port, std::string params )`  
`Port port` : Port object that this games clients will connect to  
`std::string params` : `std::string` containing BCI2000 parameter file to be sent to each client  

## Methods
### `bool Game::Connect( Client* client )`  
Connects a client to the game. Calls `Game::ConnectClient()` and `Game::ValidateStates()` with given client and adds them to the list of connected clients.  
`Client* client` : Client object to connect to the game  
returns:  
true unless client does not exist.  

---

### `void Game::ConnectClient( Client* client )`  
Sends greeting message with client number id and BCI2000 parameters defined in `Client::params`.   
`Client* client` : Client to connect to the game  

---

### `void Game::ValidateStates( Client* client )`  
Waits for client to send shared states list. Validates that this client has the same shared states as the other client(s) or saves this clients shared states as the standard if it is the first one.  
`Client* client` : Client to validate  

---

### `void Game::AddClient( Client* client )`  
Adds a client to the list of clients connected to the game  
`Client* client` : Client to add  

---

### `StateMachine Game::Loop()`  
Executes `Game::Update` untill a client disconnects  
returns:  
`Game::masterStates`  

---

### `bool Game::Update()`  
Executes one iterance of the server loop. Reads clients, reconciles, and sends client updates.
returns:  
true unless client disconnected  

---

### `bool Game::ReadClients()`  
Reads game state updates from the clients  
returns:  
false if client disconnected, true otherwise

---

### `void Game::Reconcile()`  
Reconciles client game state updates

---

### `bool Game::SendToClients()`  
Sends game state updates to clients  
returns:  
false if client disconnected, true otherwise



## Members
### `Port Game::port` 
(unused) Port object from which this game's clients are connecting <br>  

---

### `std::string Game::params`
`std::string` containing BCI2000 parameters to be sent to each client during `Game::ConnectClient`  

---

### `std::string Game::sharedStates` 
list of shared states used to validate clients when they are connecting  

---

### `std::vector<Client*> Game::clients` 
list of connected clients  

---

### `StateMachine Game::masterStates` 
Master state machine of the game  

# Param
One BCI2000 parameter line  

## Constructor
`Param::Param( std::string line, std::string name, std::string value, int length = 1, int width = 1 )`  
`std::string line` : Everything after BCI2000 parameter "="  
`std::string name` : Name of the BCI2000 parameter  
`std::string value` : value of the BCI2000 parameter  
`int length` : length of the BCI2000 parameter if list or matrix (default 1)  
`int width` : width of the BCI2000 parameter if matrix (default 1)  

## Members
### `std::string Param::line` 
Everything after BCI2000 parameter "="  

---

### `std::string Param::name` 
Name of the BCI2000 parameter  

---

### `std::string Param::value` 
value of the BCI2000 parameter  

---

### `int Param::length` 
length of the BCI2000 parameter if list or matrix (default 1)  

---

### `int Param::width` 
width of the BCI2000 parameter if matrix (default 1)  


# Params
Object representation of a BCI2000 parameter file  

## Constructors
### `Params::Params( std::string file );  
Parameter object from file
`std::string file` : BCI2000 parameter file to read

---

### `Params::Params()`  
Blank parameter object

## Methods
### `Param* Params::GetParam( std::string name )`
Get `Param` object for a specific parameter  
Arguments:  
`std::string name` : Name of the parameter to get  

Returns:
Pointer to a `Param` object containing the parameter information

---

### `void Params::AddParam( std::string param )`
Adds the specified parameter to the represented parameter file  

Arguments:  
`std::string param` : Full parameter line to add  

---

### `void Params::AddParam( std::string location, std::string type, std::string name, std::string value, std::string defaultValue = "%", std::string lowRange = "%", std::string highRange = "%", std::string comment = "default comment", std::string width = "", std::string height = "" )`
Adds the specified parameter to the represented parameter file

Arguments:  
`std::string location` : Location of parameter in BCI2000 config window, e.g. Application:Sequence or Source  
`std::string type` : Type of parameter, e.g. int, intlist, or matrix  
`std::string name` : Name of the parameter  
`std::string value` : Value of the parameter  
`std::string defaultValue` : default value of the parameter (defaults to "%", which is no defualt value)  
`std::string lowRange` : low range of the parameter value (defaults to "%", which is no low range)  
`std::string highRange` : high range of the parameter value (defaults to "%", which is no high range)  
`std::string comment` : comment describing the parameter (defaults to "default comment")  
`std::string width` : Width (if list or matrix) of the dataset. Can be a number or "{" or "[" contained list  
`std::string height` : Height (if matrix) of the dataset. Can be a number or "{" or "[" contained list  

---

### `void Params::AddParam( Param param )`
Adds the specified parameter to the represetnted parameter file from a parameter object  

Arguments:  
`Param param` : parameter object to add  


# Port
Object that manages socket connections and a port opening

## Constructor
### `Port::Port( int port, int to )`
Constructs a Port object at a specified port  
Arguments:  
`int port` : Number of the port to open the sockets at  
`int to` : (NOTHING) 

## Methods
### `Client* Port::WaitForClient()`
Blocks until client connects  
Returns:
Pointer to the `Client` object for the client that connected  

---

### `Client* Port::CheckForClient()`
Checks if a client is currently attempting to connect  
Returns:
Pointer to the `Client` object fo the client that connected, if a client was waiting, otherwise `nullptr`  

## Members
### `bool Port::open` : whether or not the port is currently open for business

# StateMachine
State machine object that stores states with names and values

## Constructor
### `StateMachine::StateMachine()`  
Creates new blank state machine  

## Methods
### `const char* StateMachine::GetState( std::string name )`  
Gets the value of a state from the state machine  

Arguments:  
`std::string name` : Name of the state to retrieve  

Retruns:  
`const char*` with the value of the state  

---

### `bool StateMachine::SetState( std::string name, std::string value )`
Sets a state to a value. Creates a new state if one does not exist.  

Arguments:  
`std::string name` : Name of the state to create or change  
`std::string value` : Value to change or create the state to  

Returns:  
`false` if the state was set to a new value, otherwise `true`  

---

### `void StateMachine::Interpret( const char* buffer, StateMachine* tracker )`
Interpret a client message, and update states accordingly. Tracks changes to any states.  

Arguments:  
`const char* buffer` : Buffer containing message to interpret  
`StateMachine* tracker` : Pointer to the state machine that changes will be recorded in.  

--- 

### `std::string StateMachine::GetMessage()`
Gets a message in the client-server data exchange format containing all of the states within the state machine  

Returns:  
`std::string` containing the message  


