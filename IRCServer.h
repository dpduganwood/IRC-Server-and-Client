
#ifndef IRC_SERVER
#define IRC_SERVER

#define PASSWORD_FILE "password.txt"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class IRCServer {

	struct UserInfo { // a struct containing all of a user's information
		char* username;
		char* password;
		char* room; // name of room user is in
	};

	struct Messages {  // a struct containing all the messages for one room		
		string username[100]; //list of usernames for this room paried with messages
		string message[1000]; //list of messages for this room
		string roomPlace; // name of room where messages are located (only these messages!)
		int messageCount; //message count for this room
	};	
	
private:
	int open_server_socket(int port);

public:
	void initialize();
	bool checkPassword(int fd, const char * user, const char * password);
	void processRequest( int socket );
	void addUser(int fd, const char * user, const char * password, const char * args);
	void createRoom(int fd, const char * user, const char * password, const char * args);
	void listRooms(int fd, const char * user, const char * password, const char * args);
	void enterRoom(int fd, const char * user, const char * password, const char * args);
	void leaveRoom(int fd, const char * user, const char * password, const char * args);
	void sendMessage(int fd, const char * user, const char * password, const char * args);
	void getMessages(int fd, const char * user, const char * password, const char * args);
	void getUsersInRoom(int fd, const char * user, const char * password, const char * args);
	void getAllUsers(int fd, const char * user, const char * password, const char * args);
	void login(int fd, const char * user, const char * password, const char * args);
	void runServer(int port);
	UserInfo clients[100];
	Messages messages[100];
	int numRooms; //which is equal to the number of Messages structs
	int maxMessages;
	int numUsers;
	IRCServer();
};

#endif
