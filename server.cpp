//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux

//Pooja Durgi
//PD21K
#include <iostream>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
#include <algorithm>
//#include <iostream>
#include <wait.h>
#include <map>

using namespace std;
#include <string> //strlen
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros


#define BUFFER_SIZE 1024
#define MAX_CLIENTS 7
#define max_users 7

int loginflag=0;
//vector<string> usernames;
vector<int> clientsocketfds;

std::vector<std::string> usernames;

int wordcount=0;
int master_socket;


typedef struct {
    int SOCK_FD;
    char user[BUFFER_SIZE];
} Client;

void handle_login(char* username, int clientsocketfd, int clientport);
void handle_chat(char* username, int clientsocketfd,int broadcastflag,char* cmessage);
void *clientrequest_handler(void *arg);
void message_broadcast(Client *sender, const char *message);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
    
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) 
		printf("===child %d terminated.\n", pid);
	return ;
}
void handle_chat(char* username, int clientsocketfd,int broadcastflag,char* cmessage){

    cout<<"\nhello\t" <<username<<endl;
    if(broadcastflag==1)
    {
    int i=0;
        for (i=0;i<3;i++)
        {
        send(clientsocketfds[i],cmessage,strlen(cmessage),0);
        }

    }
}

void handle_ctrl_c(int signum) {
    printf("===Received SIGINT (Ctrl-C), closing socket and terminating\n");
	close(master_socket);
	exit(1);
    // Handle the signal here
}

void handle_login(char* username, int clientsocketfd, int clientport)
{

char loginmsg[1024]="===Sorry, this username already exists. Please choose another username to login";
char maxusermsg[1024]="===sorry, max limit of users reached, cannot login";
// vector<string> usernames;
char afterlogin[1400]="===1.send a chat message in the format of chat [@username] some_message, where @username is optional or\n2.Logout";
char commands[1024];
bool exists = false;
if(usernames.size()==3){
send(clientsocketfd, maxusermsg, strlen(maxusermsg),0);
close(clientsocketfd);
}
  for (int i = 0; i < usernames.size(); i++) {
    if (usernames[i] == username) {
      exists = true;
      break;
    }
  }
  if (exists) {
    send(clientsocketfd , loginmsg , strlen(loginmsg) , 0 );
    //cout << "Sorry, this username already exists. Please choose another username.\n";
  } else {
	loginflag=1;
    usernames.push_back(username);
    clientsocketfds.push_back(clientsocketfd);
    send(clientsocketfd, afterlogin, strlen(afterlogin),0);
    wordcount++;
    //char buffer[1024]; // buffer to store incoming message
  } //return loginflag;
}


pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
Client clients[MAX_CLIENTS];
int client_count = 0;

int main(int argc, char *argv[]) {
    struct sigaction abc;
    signal(SIGINT, handle_ctrl_c);
    abc.sa_handler = sig_chld;
	sigemptyset(&abc.sa_mask);
	abc.sa_flags = 0;
	char command[1024];
    int t;



    if (argc != 2) {
        fprintf(stderr, "=== Usage error: %s <config_file>\n", argv[0]);
        exit(1);
    }

    FILE *configfile = fopen(argv[1], "r");
    if (!configfile) {
        cout<<"Error opening config file"<<endl;
        exit(1);
    }

    int port;
    //read from configuration file
    fscanf(configfile, "port: %d", &port);
    fclose(configfile);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cout<<"===Socket creation error===="<<endl;
        exit(1);
    }

    struct sockaddr_in serversaddr;
    memset(&serversaddr, 0, sizeof(serversaddr));
    serversaddr.sin_family = AF_INET;
    serversaddr.sin_addr.s_addr = INADDR_ANY;
    serversaddr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&serversaddr, sizeof(serversaddr)) < 0) {
        cout<<"===Binding failed=="<<endl;
        exit(1);
    }
    //print server details

    printf("=====Server is running with ip address %s: port number %d\n", inet_ntoa(serversaddr.sin_addr), ntohs(serversaddr.sin_port));

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        cout<<"==================Unable to listen connection requests===========\n"<<endl;
        cout<<"listen:"<<endl;
        exit(1);
    }

    fd_set readfds;
    int max_sd;
    int max_socketfds[30];
    while (1) {
        //clear the socket set

        FD_ZERO(&readfds);
        //add master socket to set

        FD_SET(server_fd, &readfds);
        max_sd = server_fd;
        //add child sockets to set
        int newclientflag=0;
        //
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].SOCK_FD > 0) {
                FD_SET(clients[i].SOCK_FD, &readfds);
                max_sd = (clients[i].SOCK_FD > max_sd) ? clients[i].SOCK_FD : max_sd;
            }
        }
        //wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("Select error");
            cout<<"select function error"<<endl;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in clients_address;
            wordcount++;
            int addr_len = sizeof(clients_address);
            newclientflag=1;

            int nextclients_fd = accept(server_fd, (struct sockaddr *)&clients_address, (socklen_t *)&addr_len);
            if (nextclients_fd < 0) {
                cout<<"accept"<<endl;
                continue;
            }
            //inform user of socket number - used in send and receive commands

            pthread_mutex_lock(&clients_mutex);

            if (client_count + 1 > MAX_CLIENTS) {
                cout<< "====Maximum client connection limit reached. Server rejected.\n"<<endl;
                close(nextclients_fd);
                wordcount++;
                pthread_mutex_unlock(&clients_mutex);
                continue;
            }
            //if(new_scket > )

            clients[client_count].SOCK_FD = nextclients_fd;
            strcpy(clients[client_count].user, "====User has not logged in still===");
            wordcount++;
            pthread_t tid;
            pthread_create(&tid, NULL, clientrequest_handler, &clients[client_count]);

            client_count++;
            wordcount++;

            pthread_mutex_unlock(&clients_mutex);

            

            
        }
    }

    return 0;
}

void *clientrequest_handler(void *arg) {
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];
	int temp=0;
    int is_logged_in = 0;
    int msgcount=0;
    int i,j;
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client->SOCK_FD, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            cout<<"======Client has disconnected=====\n"<<endl;
            close(client->SOCK_FD);
		temp++;
            pthread_mutex_lock(&clients_mutex);
            client->SOCK_FD = -1;
            pthread_mutex_unlock(&clients_mutex);
            msgcount++;
            break;
        }

        char command[BUFFER_SIZE];
        sscanf(buffer, "%s", command);
        int serverflag=0;
        char login[30] ="login";

        if (strcmp(command, login) == 0) {
             
            if(!is_logged_in) {
                char user[BUFFER_SIZE];
                sscanf(buffer + strlen(command), "%s", user);
                serverflag=1;

                pthread_mutex_lock(&clients_mutex);
                strcpy(client->user, user);
                pthread_mutex_unlock(&clients_mutex);
                is_logged_in = 1;
                msgcount++;

                serverflag=1;
                printf("User '%s' has logged in with socket number %d=====\n",user,client->SOCK_FD);
            }
            else{

                cout<<"===User is already logged in====\n"<<endl;

            }

        } else if (strcmp(command, "logout") == 0) {
            if (!is_logged_in) {
                cout<<" ====you have log in first , hence cannot log you out\n"<<endl;
            } else {
                pthread_mutex_lock(&clients_mutex);
                strcpy(client->user, "unknown");
                pthread_mutex_unlock(&clients_mutex);
                wordcount++;

                is_logged_in = 0;

                printf("===User '%s' has logged out with socket number %d .This socket number is free for usuage\n", client->user, client->SOCK_FD);
            }
        } else if (strcmp(command, "chat") == 0) {
            if (!is_logged_in) {
                cout<<"Cannot chat without logging in\n"<<endl;
            } else {
                char message[BUFFER_SIZE];
                strcpy(message, buffer + strlen(command) + 1);

                cout<<"===Received message=== %s\n"<< message<<endl;
                message_broadcast(client, message);
                wordcount++;
            }
        } else {
            cout<<"===Unknown command: %s\n"<<command<<endl;
        }
    }

    return NULL;
}

void message_broadcast(Client *sender, const char *message) {
    pthread_mutex_lock(&clients_mutex);
    int j;
    int client[30];
    int i;
    char buffer[BUFFER_SIZE];
    int newflag=0;
    char target_user[BUFFER_SIZE] = {0};
    int is_private_message = 0;
    string s;
 //   char messagesent[800];

    if (message[0] == '@') {
        is_private_message = 1;
        sscanf(message + 1, "%s", target_user);
    }

            for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].SOCK_FD != -1 && clients[i].SOCK_FD != sender->SOCK_FD && strlen(clients[i].user) > 0 && strcmp(clients[i].user, "anonymous") != 0 && (is_private_message == 0 || strcmp(clients[i].user, target_user) == 0)) {
                snprintf(buffer, BUFFER_SIZE, "===%s >> %s===", sender->user, message + (is_private_message ? strlen(target_user) + 2 : 0));
                send(clients[i].SOCK_FD, buffer, strlen(buffer), 0);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        wordcount++;
}


