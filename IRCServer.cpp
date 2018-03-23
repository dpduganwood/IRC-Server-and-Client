
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cstring>
#include "IRCServer.h"
using namespace std;

int QueueLength = 5;

IRCServer::IRCServer() {
	//clients = new UserInfo();
	//messages = new Messages();
	numRooms = 0;
	numUsers = 0;
	maxMessages = 100;
}

int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);
  
	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			     (char *) &optval, sizeof( int ) );
	
	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}
	
	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
	while ( 1 ) {
		
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);
		
		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
	
}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the comand received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) {

		if (newChar == '\n' && prevChar == '\r') {
			break;
		}
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	
	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
        commandLine[ commandLineLength ] = 0;

	printf("RECEIVED: %s\n", commandLine);

	std::string x(commandLine);
	char* tok;
	char* words[100];
	int i = 0;
	tok = strtok(commandLine, " ");
	int count = 0;
	int total = 0;
	while (tok != NULL) {
		words[i] = tok;
		if (i < 3) {
			while (tok[count] != '\0') {
				count++;
			}
			total += count;
			count = 0;
		}
		tok = strtok(NULL, " ");
		i++;
	}
	total += 3;
	if (i == 3) 
		words[3] = strdup(" ");
	
	if (i > 4) {
		std::string y = x.substr(total);
		std::strcpy(words[3], y.c_str());
	}
	
	const char * command = words[0];
	const char * user = words[1];
	const char * password = words[2];
	const char * args = words[3];

	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRooms(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else if (!strcmp(command, "LOGIN")) {
		login(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer
	//const char * msg =  "OK\n";
	//write(fd, msg, strlen(msg));

	close(fd);	
}

void
IRCServer::initialize()
{
	// Open password file

	// Initialize users in room

	// Initalize message list
	
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
	// Here check the password
	return true;
}

void
IRCServer::login(int fd, const char * user, const char* password, const char* args) 
{
	for (int i = 0; i < numUsers; i++) {
		if ((strcmp(clients[i].username, user) == 0) && (strcmp(clients[i].password, password) == 0)) {
			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}
	const char * msg =  "DENIED\r\n";
	write(fd, msg, strlen(msg));
	
	return;
}

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
	// Here add a new user. For now always return OK.

	for (int i = 0; i < numUsers; i++) {
		if (strcmp(clients[i].username, user) == 0) {
			const char * msg =  "USER ALREADY EXISTS\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}

	clients[numUsers].username = strdup(user);
	clients[numUsers].password = strdup(password);
	numUsers++;

	const char * msg =  "OK\r\n";
	write(fd, msg, strlen(msg));

	return;		
}

void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args)
{
	
	messages[numRooms].roomPlace = std::string(args);
	messages[numRooms].messageCount = 0;
	numRooms++;
	const char * msg =  "OK\r\n";
	write(fd, msg, strlen(msg));
	return;
}

void
IRCServer::listRooms(int fd, const char * user, const char * password, const char * args)
{	
	if (numRooms == 0){
		const char * msg =  "No rooms";
		write(fd, msg, strlen(msg));
		return;
	}
	for (int i = 0; i < numRooms; i++) {
		char* msg = new char[messages[i].roomPlace.length()+1];
		std:strcpy(msg, messages[i].roomPlace.c_str());
		msg = ("%s\r\n", msg);
		write(fd, strdup(strcat(strdup(msg), strdup("\r\n"))), strlen(msg)+2);
	}
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}	

	string r = string(args);	
	bool error = true;	
	for (int i = 0; i < numRooms; i++) {
		if (messages[i].roomPlace.compare(r) == 0) {
			error = false;
			break;
		}
	}
	if (error == true) {
		const char* msg = "ERROR (No room)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
	for (int j = 0; j < numUsers; j++) {
		if ((strcmp(clients[j].username, strdup(user)) == 0) && (strcmp(clients[j].password, strdup(password)) == 0)) {
			clients[j].room = strdup(args);
			const char * msg =  "OK\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}
	return;
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}	
	for (int j = 0; j < numUsers; j++) {
		if ((strcmp(clients[j].username, strdup(user)) == 0) && (strcmp(clients[j].password, strdup(password)) == 0)) {
			if (clients[j].room != NULL && strcmp(clients[j].room, strdup(args)) == 0) {			
				clients[j].room = NULL;
				const char * msg =  "OK\r\n";
				write(fd, msg, strlen(msg));	
				return;
			}
			else {
				const char * msg =  "ERROR (No user in room)\r\n";
				write(fd, msg, strlen(msg));	
				return;
			}
		}
	}
	write(fd, "\r\n", 2);
	return;
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}

	char theMessage[1000];
	std::string x(args);
	int sp = x.find(" ");
	std::string y = x.substr(sp+1);
	std::strcpy(theMessage, y.c_str());
	
	std::string roomName = x.substr(0, sp);

		
	int i;
	bool error = true;	
	for (i = 0; i < numRooms; i++) { //find the correct room	
		if (messages[i].roomPlace.compare(roomName) == 0) {
			break;
		}
	}
	for (int k = 0; k < numUsers; k++) { //check if user is in room
			if (clients[k].room != NULL) {
				string clientR = string(clients[k].room);
				if ((clientR.compare(roomName) == 0) && (strcmp(clients[k].username, strdup(user)) == 0)) {	
					const char * msg =  "OK\r\n";
					write(fd, msg, strlen(msg));
					error = false;
					break;
				}
			}
	}
	if (error == true){
		const char * msg =  "ERROR (user not in room)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}

	for (int j = 0; j < numUsers; j++) { //find the correct user and store the message
		if (clients[j].room != NULL){
			string clientRoom = string(clients[j].room);
			if ((clientRoom.compare(roomName) == 0)) {			
				messages[i].username[messages[i].messageCount] = string(user);
				messages[i].message[messages[i].messageCount] = string(strdup(theMessage));
				(messages[i].messageCount)++;
				return;		
			}
		}
	} 
	return;
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}

	std::string x(args);
	int sp = x.find(" ");
	std::string roomName = x.substr(sp+1);

	std::string num = x.substr(0, sp);
	int n = atoi(num.c_str());
	
	bool error = true;
	for (int i = 0; i < numRooms; i++) { //find the correct room	
		if (messages[i].roomPlace.compare(roomName) == 0) {
			break;
		}
	}
	for (int k = 0; k < numUsers; k++) { //check if user is in room
			if (clients[k].room != NULL) {
				string clientR = string(clients[k].room);
				if ((clientR.compare(roomName) == 0) && (strcmp(clients[k].username, strdup(user)) == 0)) {	
					error = false;
					break;
				}
			}
		}
	if (error == true){
		const char * msg =  "ERROR (User not in room)\r\n";
		write(fd, msg, strlen(msg));
		return;
	}
	
	
	for (int j = 0; j < numRooms; j++) { 
		if (messages[j].roomPlace.compare(roomName) == 0) {
			//const char * msg =  "OK\r\n";
			//write(fd, msg, strlen(msg));
			if ((n+1) >= messages[j].messageCount) {
				const char * msg =  "NO-NEW-MESSAGES\r\n";
				write(fd, msg, strlen(msg));
				return;
			}
			for (int k = n+1; k < messages[j].messageCount; k++) {
				char* m1 = new char[messages[j].username[k].length()+3];
				char* m2 = new char[messages[j].message[k].length()+2];
				strcpy(m1, (" "+messages[j].username[k]+" ").c_str());
				strcpy(m2, (messages[j].message[k]+"\r\n").c_str());
				char p[100];
				sprintf(p, "%d", k);				
				char* msg1 = ("%s", p);
				write(fd, msg1, strlen(msg1));
				char* msg2 = ("%s", m1);
				write(fd, msg2, strlen(msg2));
				char* msg3 = ("%s", m2);
				write(fd, msg3, strlen(msg3));
			}	
			write(fd, "\r\n", 2);		
			return;
		}
	}
	return;
}

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}
	UserInfo temp;
	for (int i = 0; i < numUsers-1; i++) {
		for (int j = 0; j < numUsers-i-1; j++) {
			if (strcmp(clients[j].username, clients[j+1].username) > 0) {
				temp.username = clients[j].username;
				clients[j].username = clients[j+1].username;
				clients[j+1].username = temp.username;	
				temp.password = clients[j].password;
				clients[j].password = clients[j+1].password;
				clients[j+1].password = temp.password;
				temp.room = clients[j].room;
				clients[j].room = clients[j+1].room;
				clients[j+1].room = temp.room;
			}
		}
	}

	for (int j = 0; j < numUsers; j++) { 
		if (clients[j].room != NULL && strcmp(strdup(args), clients[j].room) == 0) {
			char* msg =  ("%s\r\n", clients[j].username);
			write(fd, strdup(strcat(strdup(msg), strdup("\r\n"))), strlen(msg)+2);
		}
	} 
	write(fd, "\r\n", 2);
	return;
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password, const  char * args)
{	
	if (numUsers == 0){
		const char * msg =  "No users";
		write(fd, msg, strlen(msg));
		return;
	}
		
	for (int j = 0; j < numUsers; j++) {
		if (strcmp(clients[j].password, strdup(password)) == 0) {
			if (strcmp(clients[j].username, strdup(user)) == 0) 
				break;
		}
		if (j == (numUsers-1)){
			const char* msg = "ERROR (Wrong password)\r\n";
			write(fd, msg, strlen(msg));
			return;
		}
	}	

	UserInfo temp;
	for (int i = 0; i < numUsers-1; i++) {
		for (int j = 0; j < numUsers-i-1; j++) {
			if (strcmp(clients[j].username, clients[j+1].username) > 0) {
				temp.username = clients[j].username;
				clients[j].username = clients[j+1].username;
				clients[j+1].username = temp.username;
				temp.password = clients[j].password;
				clients[j].password = clients[j+1].password;
				clients[j+1].password = temp.password;
				temp.room = clients[j].room;
				clients[j].room = clients[j+1].room;
				clients[j+1].room = temp.room;
			}
		}
	}
	
	for (int k = 0; k < numUsers; k++) {
		char* msg =  ("%s\r\n", clients[k].username);
		write(fd, strdup(strcat(strdup(msg), strdup("\r\n"))), strlen(msg)+2);
	}
	write(fd, "\r\n", 2);
	return;
}

