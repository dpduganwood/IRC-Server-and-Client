
#include <stdio.h>
#include <gtk/gtk.h>
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>

char * host;
char * user;
char * password;
char * sport;
int port;
GtkListStore * list_rooms;
GtkListStore * list_users;
GtkWidget *roomField;
GtkWidget *usernameField;
GtkWidget *passwordField;
GtkWidget *messageField;
GtkWidget *messageViewField;
GtkWidget *errorMessageField;
GtkWidget *table;
GtkTextBuffer* usernameBuffer;
GtkTextBuffer* passwordBuffer;
GtkTextBuffer* room;

using namespace std;

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

int lastMessage = 0;

int open_client_socket(char * host, int port) {
	// Initialize socket address structure
	struct  sockaddr_in socketAddress;
	
	// Clear sockaddr structure
	memset((char *)&socketAddress,0,sizeof(socketAddress));
	
	// Set family to Internet 
	socketAddress.sin_family = AF_INET;
	
	// Set port
	socketAddress.sin_port = htons((u_short)port);
	
	// Get host table entry for this host
	struct  hostent  *ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		perror("gethostbyname");
		exit(1);
	}
	
	// Copy the host ip address to socket address structure
	memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);
	
	// Get TCP transport protocol entry
	struct  protoent *ptrp = getprotobyname("tcp");
	if ( ptrp == NULL ) {
		perror("getprotobyname");
		exit(1);
	}
	
	// Create a tcp socket
	int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0) {
		perror("socket");
		exit(1);
	}
	
	// Connect the socket to the specified server
	if (connect(sock, (struct sockaddr *)&socketAddress,
		    sizeof(socketAddress)) < 0) {
		perror("connect");
		exit(1);
	}
	
	return sock;
}

int sendCommand(char *  host, int port, char * command, char * response) {
        int sock = open_client_socket( host, port);
        if (sock<0) {
                return 0;
        }
        // Send command
        write(sock, command, strlen(command));
        write(sock, "\r\n",2);
        //Print copy to stdout
        write(1, command, strlen(command));
        write(1, "\r\n",2);
        // Keep reading until connection is closed or MAX_REPONSE
        int n = 0;
        int len = 0;
        while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
                len += n;
        }
        response[len]='\0';
        printf("response:\n%s\n", response);
        close(sock);
        return 1;
}


void printUsage()
{
	printf("Usage: talk-client host port user password\n");
	exit(1);
}

/*
void add_user() {
	// Try first to add user in case it does not exist.
	char response[ MAX_RESPONSE ];
	sendCommand(host, port, "ADD-USER", user, password, "", response);
	
	if (!strcmp(response,"OK\r\n")) {
		printf("User %s added\n", user);
	}
}
*/

void create_account() {
	const gchar *userText = gtk_entry_get_text(GTK_ENTRY (usernameField));
	const gchar *passwordText = gtk_entry_get_text(GTK_ENTRY (passwordField));
}

void login() {

}

void enter_room() {
}

void leave_room() {
}

void get_messages() {

}

void send_message(char * msg) {
}

void print_users_in_room() {
}

void print_users() {
}

void printPrompt() {
	printf("talk> ");
	fflush(stdout);
}

void printHelp() {
	printf("Commands:\n");
	printf(" -who   - Gets users in room\n");
	printf(" -users - Prints all registered users\n");
	printf(" -help  - Prints this help\n");
	printf(" -quit  - Leaves the room\n");
	printf("Anything that does not start with \"-\" will be a message to the chat room\n");
}

void * getMessagesThread(void * arg) {
	// This code will be executed simultaneously with main()
	// Get messages to get last message number. Discard the initial Messages
	
	while (1) {
		// Get messages after last message number received.

		// Print messages

		// Sleep for ten seconds
		usleep(2*1000*1000);
	}
}

void startGetMessageThread()
{
	pthread_create(NULL, NULL, getMessagesThread, NULL);
}

/*----------------------------------------------------------*/	

void update_list_rooms() {
    GtkTreeIter iter;
	GtkTreeModel *tree;
	
	string command = "LIST-ROOMS";
	string commandLine;
	char* ch;
	char* u;
	char* p;

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	string s1(u);
	string s2(p);

	commandLine = command + " " + s1 + " " + s2;
	ch = strdup(commandLine.c_str());
	char response[MAX_RESPONSE];
	if (s1 != "" && s2 != "") {
		sendCommand("localhost", 8500, ch, response);
		if (strcmp(response, "No rooms") != 0) {
			gchar *msg = (gchar*) response;
			gtk_list_store_clear(GTK_LIST_STORE(list_rooms));
			gtk_list_store_append (GTK_LIST_STORE (list_rooms), &iter);
			gtk_list_store_set (GTK_LIST_STORE (list_rooms), &iter, 0, msg, -1);
		}
	}
}

void update_list_users() {
    GtkTreeIter iter;
	GtkTreeModel *tree;

	string command = "GET-ALL-USERS";
	string commandLine;
	char* ch;
	char* u;
	char* p;

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	string s1(u);
	string s2(p);

	commandLine = command + " " + s1 + " " + s2;
	ch = strdup(commandLine.c_str());
	char response[MAX_RESPONSE];
	if (s1 != "" && s2 != "") {
		sendCommand("localhost", 8500, ch, response);
		if (strcmp(response, "No users") != 0) {
			gchar* msg = (gchar*) response;
			gtk_list_store_clear(GTK_LIST_STORE(list_users));
			gtk_list_store_append (GTK_LIST_STORE (list_users), &iter);
			gtk_list_store_set (GTK_LIST_STORE (list_users), &iter, 0, msg, -1);
		}
	}
}

void update_all_messages() {
    string commandGet = "GET-MESSAGES";
	string commandL;
	char* text;

	char* u;
	char* p;
	char* r;
	char* m;
	char response[MAX_RESPONSE];

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	r = (char*) gtk_entry_get_text((GtkEntry*)roomField);
	string s1(u);
	string s2(p);
	string s3(r);
	
	commandL = commandGet + " " + s1 + " " + s2 + " -1 " + s3;
	text = strdup(commandL.c_str());

	if (s1 != "" && s2 != "" && s3 != "") {
		sendCommand("localhost", 8500, text, response);
		string error(response);
		string e1("ERROR (User not in room)\r\n");

		string e2("NO-NEW-MESSAGES\r\n");
		if ((error.compare(e1) != 0) && (error.compare(e2) != 0)) {
			gchar g[1000];
			strcpy(g, response);	
			GtkTextBuffer *buf = gtk_text_buffer_new((GtkTextTagTable*)table);
			gtk_text_buffer_set_text(buf, g, strlen(response));
			gtk_text_view_set_buffer((GtkTextView*)messageViewField, buf);
		}
	}
}

/* Create the list of "messages" */
static GtkWidget *create_list( const char * titleColumn, GtkListStore *model )
{
    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    //GtkListStore *model;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, 
				    GTK_POLICY_AUTOMATIC);
   
    //model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);
   
    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes (titleColumn,
                                                       cell,
                                                       "text", 0,
                                                       NULL);
  
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
	  		         GTK_TREE_VIEW_COLUMN (column));

    return scrolled_window;
}
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static void insert_text( GtkTextBuffer *buffer, const char * initialText )
{
   GtkTextIter iter;
 
   gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
   gtk_text_buffer_insert (buffer, &iter, initialText,-1);
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( const char * initialText )
{
   GtkWidget *scrolled_window;
   GtkWidget *view;
   GtkTextBuffer *buffer;

   view = gtk_text_view_new ();
   buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
		   	           GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);

   gtk_container_add (GTK_CONTAINER (scrolled_window), view);
   insert_text (buffer, initialText);

   gtk_widget_show_all (scrolled_window);

   return scrolled_window;
}

void leaveRoom_callback() {
	string command = "LEAVE-ROOM";
	string commandLeave = "SEND-MESSAGE";
	string commandLine;
	string commandL;
	char* text;
	char* ch;
	char* u;
	char* p;
	char* r;
	char response[MAX_RESPONSE];

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	r = (char*) gtk_entry_get_text((GtkEntry*)roomField);
	string s1(u);
	string s2(p);
	string s3(r);

	commandL = commandLeave + " " + s1 + " " + s2 + " " + s3 + " " + "(Left the room)";
	text = strdup(commandL.c_str());
	sendCommand("localhost", 8500, text, response);

	commandLine = command + " " + s1 + " " + s2 + " " + s3;
	ch = strdup(commandLine.c_str());
	sendCommand("localhost", 8500, ch, response);

	//clear the messages field when the user leaves the room
	string err = "";
	char* rep = strdup(err.c_str());
	gchar g[1000];
	strcpy(g, rep);	
	GtkTextBuffer *buf = gtk_text_buffer_new((GtkTextTagTable*)table);
	gtk_text_buffer_set_text(buf, g, strlen(response));
	gtk_text_view_set_buffer((GtkTextView*)messageViewField, buf);
}

void createRoom_callback() {
	string command = "CREATE-ROOM";
	string commandLine;
	char* ch;
	char* u;
	char* p;
	char* r;

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	r = (char*) gtk_entry_get_text((GtkEntry*)roomField);
	string s1(u);
	string s2(p);
	string s3(r);

	commandLine = command + " " + s1 + " " + s2 + " " + s3;
	ch = strdup(commandLine.c_str());
	char response[MAX_RESPONSE];
	sendCommand("localhost", 8500, ch, response);
	update_list_rooms();
}

void createAccount_callback() {
	string command = "ADD-USER";
	string commandLine;
	char* ch;
	char* u;
	char* p;

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	string s1(u);
	string s2(p);

	commandLine = command + " " + s1 + " " + s2;
	ch = strdup(commandLine.c_str());
	char response[MAX_RESPONSE];
	sendCommand("localhost", 8500, ch, response);
}

void enterRoom_callback() {
	string command = "ENTER-ROOM";
	string commandEnter = "SEND-MESSAGE";
	string commandLine;
	string commandL;
	char* text;
	char* ch;
	char* u;
	char* p;
	char* r;
	char response[MAX_RESPONSE];

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	r = (char*) gtk_entry_get_text((GtkEntry*)roomField);
	string s1(u);
	string s2(p);
	string s3(r);

	commandLine = command + " " + s1 + " " + s2 + " " + s3;
	ch = strdup(commandLine.c_str());
	sendCommand("localhost", 8500, ch, response);
	
	commandL = commandEnter + " " + s1 + " " + s2 + " " + s3 + " " + "(Entered the room)";
	text = strdup(commandL.c_str());
	sendCommand("localhost", 8500, text, response);
}

void login_callback() {
	string command = "LOGIN";
	string commandLine;
	char* ch;
	char* u;
	char* p;

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	string s1(u);
	string s2(p);

	commandLine = command + " " + s1 + " " + s2;
	ch = strdup(commandLine.c_str());
	char response[MAX_RESPONSE];
	sendCommand("localhost", 8500, ch, response);
	update_list_users();
}

void send_callback() {
	string commandSend = "SEND-MESSAGE";
	string commandGet = "GET-MESSAGES";
	string commandLine;
	string commandL;
	char* ch;
	char* text;
	char* u;
	char* p;
	char* r;
	char* m;
	char response[MAX_RESPONSE];

	u = (char*) gtk_entry_get_text((GtkEntry*)usernameField);
	p = (char*) gtk_entry_get_text((GtkEntry*)passwordField);
	r = (char*) gtk_entry_get_text((GtkEntry*)roomField);
	m = (char*) gtk_entry_get_text((GtkEntry*)messageField);
	string s1(u);
	string s2(p);
	string s3(r);
	string s4(m);

	commandLine = commandSend + " " + s1 + " " + s2 + " " + s3 + " " + s4;
	ch = strdup(commandLine.c_str());
	sendCommand("localhost", 8500, ch, response);
	
	commandL = commandGet + " " + s1 + " " + s2 + " -1 " + s3;
	text = strdup(commandL.c_str());
	sendCommand("localhost", 8500, text, response);

	string blank = "";
	char* b = strdup(blank.c_str());
	gchar* bl = (gchar*) b;
	gtk_entry_set_text((GtkEntry*)messageField, b);
	update_all_messages();
}

static gboolean
time_handler(GtkWidget *widget)
{
  if (widget->window == NULL) return FALSE;

  gtk_widget_queue_draw(widget);

  //fprintf(stderr, "Hi\n");
  update_list_rooms();
  update_list_users();
  update_all_messages();

  return TRUE;
}

int main( int   argc,
          char *argv[] )
{
    GtkWidget *window;
    GtkWidget *roomList;
	GtkWidget *userList;
	GtkWidget *messages;
	GtkWidget *myMessage;
	GtkWidget *scrollWindow;

    gtk_init (&argc, &argv);
   
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Paned Windows");
    g_signal_connect (window, "destroy",
	              G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_widget_set_size_request (GTK_WIDGET (window), 450, 400);

    // Create a table to place the widgets. Use a 7x4 Grid (7 rows x 4 columns)
    table = gtk_table_new (7, 4, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_table_set_row_spacings(GTK_TABLE (table), 5);
    gtk_table_set_col_spacings(GTK_TABLE (table), 5);
    gtk_widget_show (table);
   
    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    messageField = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE (table), messageField, 0, 4, 7, 8);
	gtk_widget_show(messageField);

    // Add messages text. Use columns 0 to 4 (exclusive) and rows 4 to 7 (exclusive) 
    messageViewField = gtk_text_view_new();
	//gtk_table_attach_defaults(GTK_TABLE (table), messageViewField, 0, 4, 3, 7);
	gtk_text_view_set_editable((GtkTextView*)messageViewField, FALSE);
	gtk_text_view_set_cursor_visible((GtkTextView*)messageViewField, FALSE);


	//add scroll window
	scrollWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrollWindow), messageViewField);
	gtk_table_attach_defaults(GTK_TABLE (table), scrollWindow, 0, 4, 3, 7);
	gtk_widget_show(scrollWindow);
	gtk_widget_show(messageViewField);

    // Add send button. Use columns 0 to 1 (exclusive) and rows 4 to 7 (exclusive)
    GtkWidget *send_button = gtk_button_new_with_label ("Send");
    gtk_table_attach_defaults(GTK_TABLE (table), send_button, 0, 1, 8, 9); 
    gtk_widget_show (send_button);
    
	//leave room button
	GtkWidget *leaveRoom_button = gtk_button_new_with_label ("Leave Room");
    gtk_table_attach_defaults(GTK_TABLE (table), leaveRoom_button, 4, 6, 9, 10); 
    gtk_widget_show (leaveRoom_button);

	//create room button
	GtkWidget *createRoom_button = gtk_button_new_with_label ("Create Room");
    gtk_table_attach_defaults(GTK_TABLE (table), createRoom_button, 0, 2, 9, 10); 
    gtk_widget_show (createRoom_button);

	//Create account button
	GtkWidget *createAccount_button = gtk_button_new_with_label ("Create Account");
    gtk_table_attach_defaults(GTK_TABLE (table), createAccount_button, 4, 6, 7, 8); 
    gtk_widget_show (createAccount_button);

	//Error resonse field
	/*
	errorMessageField = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE (table), errorMessageField, 4, 6, 8, 9);
	gtk_entry_set_editable((GtkEntry*)errorMessageField, FALSE);
	gtk_widget_show(errorMessageField);
	*/

	//Enter room button
	GtkWidget *enterRoom_button = gtk_button_new_with_label ("Enter Room");
    gtk_table_attach_defaults(GTK_TABLE (table), enterRoom_button, 2, 4, 9, 10); 
    gtk_widget_show (enterRoom_button);

	//Login button
	GtkWidget *login_button = gtk_button_new_with_label ("Login");
    gtk_table_attach_defaults(GTK_TABLE (table), login_button, 4, 6, 6, 7); 
    gtk_widget_show (login_button);

	// Add list of rooms. Use columns 0 to 4 (exclusive) and rows 0 to 4 (exclusive)
    list_rooms = gtk_list_store_new (1, G_TYPE_STRING);
    roomList = create_list ("Rooms", list_rooms);
    gtk_table_attach_defaults (GTK_TABLE (table), roomList, 2, 4, 0, 3);
    gtk_widget_show (roomList);

	//user list
	list_users = gtk_list_store_new (1, G_TYPE_STRING);
    userList = create_list ("Users", list_users);
    gtk_table_attach_defaults (GTK_TABLE (table), userList, 0, 2, 0, 3);
    gtk_widget_show (userList);

	//room textfield
	roomField = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE (table), roomField, 4, 6, 1, 2);
	gtk_widget_show(roomField);

	//username textfield
	usernameField = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE (table), usernameField, 4, 6, 3, 4);
	gtk_widget_show(usernameField);

	//password textfield
	passwordField = gtk_entry_new();
	gtk_table_attach_defaults(GTK_TABLE (table), passwordField, 4, 6, 5, 6);
	gtk_entry_set_visibility((GtkEntry*) passwordField, FALSE); 
	gtk_widget_show(passwordField);

	
	g_signal_connect(leaveRoom_button,"clicked", G_CALLBACK(leaveRoom_callback), (gpointer)"Leave Room");

	g_signal_connect(createRoom_button,"clicked", G_CALLBACK(createRoom_callback), (gpointer)"Create Room");

	g_signal_connect(createAccount_button,"clicked", G_CALLBACK(createAccount_callback), (gpointer)"Create Account");

	g_signal_connect(enterRoom_button,"clicked", G_CALLBACK(enterRoom_callback), (gpointer)"Enter Room");

	g_signal_connect(login_button,"clicked", G_CALLBACK(login_callback), (gpointer)"Login");

	g_signal_connect(send_button,"clicked", G_CALLBACK(send_callback), (gpointer)"Send");
	

	GtkWidget *roomLabel= gtk_label_new("Room");
	GtkWidget *usernameLabel = gtk_label_new("Username");
	GtkWidget *passwordLabel = gtk_label_new("Password");


	gtk_table_attach_defaults(GTK_TABLE (table), roomLabel, 4, 6, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE (table), usernameLabel, 4, 6, 2, 3);
	gtk_table_attach_defaults(GTK_TABLE (table), passwordLabel, 4, 6, 4, 6);
	

    gtk_widget_show (roomLabel);
    gtk_widget_show (usernameLabel);
    gtk_widget_show (passwordLabel);

    gtk_widget_show (table);
    gtk_widget_show (window);

    g_timeout_add(3000, (GSourceFunc) time_handler, (gpointer) window);

    gtk_main ();

	return 0;
}

