


//Example code: A client side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
//PD21K
//Pooja Durgi

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <map>
#include <signal.h>
#include <netdb.h>
#include <vector>

using namespace std;
map<string, int> user_sockets;
map<int,string> socket_user;
int iterator;

int sendflag=0;
#define BUFFER_SIZE 1024
const unsigned MAXBUFLEN = 512;
int sockfd;
int client_socket = 0;
bool logged_in = false;

void send_bytes(char* message);
void *get_msg(void *arg);
void close_and_exit();
void print_messages();


void handle_ctrl_c(int signum) {
    cout<<"Received SIGINT (Ctrl-C) closing socket and terminating... \n"<<endl;
	close(client_socket);
	exit(1);
    // Handle the signal here
}

void send_bytes(char* message)
{
 cout<<"sending messages to serverside"<<endl;


}

void close_and_exit() {
    close(client_socket);
    exit(0);
}

void print_messages(){
 if(sendflag==1){
     cout<<"input received from client"<< endl;
     //send()
    // send(client_socket, buffer, strlen(buffer), 0);

 }

}

void *process_connection(void *arg) {
	int n;
	char buf[MAXBUFLEN];
	pthread_detach(pthread_self());
	while (1) {
		n = read(sockfd, buf, MAXBUFLEN);
		if (n <= 0) {
			if (n == 0) {
				cout << "Server rejected connection" << endl;
			} else {
				cout << "something wrong" << endl;
			}
			close(sockfd);
			// we directly exit the whole process.
			exit(1);
		}
		buf[n] = '\0';
		cout << buf << endl;
	}
}
//const unsigned serv_port = 5100;

int main(int argc, char *argv[]) {
    // Read config file
    FILE *config_file = fopen("clientconfig.txt", "r");
    if (config_file == NULL) {
        perror("Error opening config file");
        exit(1);
    }

    char server_address[32];
    int server_port;

    fscanf(config_file, "servhost: %s\n", server_address);
    fscanf(config_file, "servport: %d", &server_port);
    fclose(config_file);

    struct sockaddr_in server_addr;

    
    //=========signal====;
    signal(SIGINT, handle_ctrl_c);

    // ===========Create socket============
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed try again");
        exit(1);
    }

    printf("You are now connected to server\n");
    printf("you can now perform :\n");
    printf("1. Login\n2. Logout\n3. Exit\n4. Chat\n");
    cout<<"==============================================\n"<<endl;
    //printf("Any other commands other than these are unknown commands.\n");
    //printf("Kindly go ahead and start typing your commands.\n");
    
    pthread_t tid;
    pthread_create(&tid, NULL, get_msg, NULL);

    
    while (1) {
        cout<<"Enter >> ";
        char buffer[BUFFER_SIZE];
        //get input from command line
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 
        //set the string terminating NULL byte on the end
					//of the data read
					char login[50] ="login";
					char username[50];
                    //std :: string s;
				//	int i,j;
				//	buffer[valread] = '\0';
					std :: string s(buffer);
        // exit implementation
        //check if logged out first
        sendflag=1;
        if (strcmp(buffer, "exit") == 0) {
            if (logged_in) {
                printf("========Kindly log out first ,before exiting.==============\n");
                continue;
            }
            cout<<"=============user typed exit..exiting==========="<<endl;
            close_and_exit();
        }
        sendflag=1;
        // =======================login command
        //set the login flag
        if (strncmp(buffer, "login", 5) == 0) {
            logged_in = true;
        }

        // =====================logout command
        //first check if logged in
        sendflag=1;
        if (strcmp(buffer, "logout") == 0) {
            if (!logged_in) {
                printf("You did not log in, hence cannot log out.\n");
                continue;
            }
            logged_in = false;
        }
        /*
        if(sendflag==2)
        {
            send(client_socket, buffer, strlen(buffer), 0);
        }*/
        // ===========chat command
        //check the login flag
        if (strncmp(buffer, "chat", 4) == 0) {
            if (!logged_in) {
                cout<<"Kindly login first to chat.\n"<<endl;
                continue;
            }
        }
        sendflag=1;
        // Send message
        send(client_socket, buffer, strlen(buffer), 0);
    }

    return 0;
}

void *get_msg(void *arg) {
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("========Disconnected from server==========\n");
            close_and_exit();
        }

        printf("%s\n", buffer);
    }
}




