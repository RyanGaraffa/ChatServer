#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"

#define clientnum 10
#define maxlogininfo 10
//#define bufferSize 1024

int thrdarr[clientnum]= {0};
char buffer[1024];
pthread_mutex_t mylock;

struct connection_t{
	int clientSocket;
	int idx;
	struct sockaddr clientAddress;
	socklen_t clientAddressLength;
};

void Death(){
	while(1){
		sleep(10);
	}
}


void *client(void *ptr){

	struct connection_t *conn;
	conn = (struct connection_t *)ptr;
	int thidx = conn->idx;
	thrdarr[thidx] = 1;
	int len;

	//Will hold the  username and Password for this client Thread
	char *UserName;
	char *PassWord;
	char un[20];
	char pw[20];
	char *toSend;

	//Accept a username input
	//pthread_mutex_lock(&mylock);
        memset(buffer, 0, sizeof(buffer));
        if ( (len = recv(conn->clientSocket, buffer, sizeof(buffer), 0)) == -1) {
                printf("len = %d\n", len);
                perror("Failed to receive message.");
                exit(EXIT_FAILURE);
        }

	printf("recieved successfully %s\n", buffer);

	//Traverse through stored login information and check to see if we have it already
	for(struct node *temp = head; temp != NULL; temp = temp->next){

//		printf("Made it into For loop Current Username: %s\n", temp->username);
		if(strcmp(buffer, temp->username) == 0){
			//We have a valid username input
			printf("We have Valid This Username In our System:  %s\n", buffer);

			//prompt for password
			memset(buffer, 0, sizeof(buffer));
			toSend = "Please Enter the password for this Username";
			if(send(conn->clientSocket, toSend, (8*sizeof(toSend)), 0) == -1){
                	        printf("Error\n");
        	                exit(EXIT_FAILURE);
	                }

			//Recieve input for password
		        memset(buffer, 0, sizeof(buffer));
			if ( (len = recv(conn->clientSocket, buffer, sizeof(buffer), 0)) == -1) {
                		printf("len = %d\n", len);
                		perror("Failed to receive message.");
                		exit(EXIT_FAILURE);
        		}
			//Check if current node's password is == to input
			if(strcmp(temp->password, buffer) == 0){
				//Allow access to Chat Server
				strcpy(un, temp->username);
				strcpy(pw, temp->password);
				//pthread_mutex_unlock(&mylock);
				break; //Will break this username input while loop and send to Main Chat Server Loop
			}
			else{
				//Invalid Password, will close connection

				printf("Invalid Password, kicking attempting user\n");
				//close(conn->clientSocket);
 			        printf("Client socket killed.\n");
			        //free(conn);
				Death();//Has Thread sleep forever >:D
			}
		}
		if(temp->next == NULL){

			//printf("Made it to second if in client for loop\n");

			//We have to add the username to our linked list
			UserName = buffer; //Has to set UserName pointer = to whatever is in buffer
			strcpy(un, buffer);
			//memset(buffer, 0, sizeof(buffer));
			//memset(toSend, 0, sizeof(toSend));
			toSend = "This Username does not exist, Please create a new Password";
                        if(send(conn->clientSocket, toSend, (8*sizeof(toSend)), 0) == -1){
                                printf("Error\n");
                                exit(EXIT_FAILURE);
                        }

			//Recieve input for password
			memset(buffer, 0, sizeof(buffer));
                        if ( (len = recv(conn->clientSocket, buffer, sizeof(buffer), 0)) == -1) {
                                printf("len = %d\n", len);
                                perror("Failed to receive message.");
                                exit(EXIT_FAILURE);
                        }

			strcpy(pw, buffer);
			AddNode(un, pw);
			//pw = buffer;
			//pthread_mutex_unlock(&mylock);
			break; //Will break this username input while loop and send to Main Chat Server Loop
		}
	}

	//AddNode(un, pw);
	printf("Username: %s logged in with password: %s\n", un, pw);
	Print();
	printf("This Should Print after Login has occured\n");

	//Send Final Confirmation Message
	toSend = "Login Was Successful";
        if(send(conn->clientSocket, toSend, (2*sizeof(toSend)), 0) == -1){
                printf("Error\n");
                exit(EXIT_FAILURE);
        }



	//Main client While Loop
	while(1){
		//printf("Made it to While Loop\n");
		//pthread_mutex_lock(&mylock);
		memset(buffer, 0, sizeof(buffer));
		//sleep(2);
		//printf("right before recieve\n");
		if ( (len = recv(conn->clientSocket, buffer, sizeof(buffer), 0)) == -1) {  
			printf("len = %d\n", len);
			perror("Failed to receive message.");
			exit(EXIT_FAILURE);
		}
		else{
		//Print Thread ID, username, and message
	        printf("%d-%s: %s\n", thidx, un, buffer);
		}

		if(strcmp(buffer, "EXIT") == 0){
			memset(buffer, 0, sizeof(buffer));
			break;
		}

/*		if(send(conn->clientSocket, buffer, sizeof(buffer), 0) == -1){
                        printf("Error\n");
                        exit(EXIT_FAILURE);
                }*/

		//pthread_mutex_unlock(&mylock);
		sleep(2);

	}//closes while loop


	close(conn->clientSocket);
        printf("Client socket closed.\n");
	free(conn);
}//closes client function



int main(){

	pthread_mutex_init(&mylock, NULL);

	struct connection_t *connt;

	printf("Server started.\n");
	const char *portNumber = "8080";
    	const int backlog = 2;
    	int serverSocket;

    	struct addrinfo hints;
    	memset(&hints, 0, sizeof(struct addrinfo));

    	struct addrinfo *results;
    	struct addrinfo *record;

    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_protocol = IPPROTO_TCP;

	pthread_t th[10];

	struct connection_t * connect;
	const size_t bufferSize = 1024;

    	if ((getaddrinfo(NULL, portNumber, &hints, &results)) != 0) {
        	perror("Failed to translate server socket.");
	        exit(EXIT_FAILURE);
    	}

    	printf("Server socket translated.\n");

    	for (record = results; record != NULL; record = record->ai_next) {
	        serverSocket = socket(record->ai_family, record->ai_socktype, record->ai_protocol);
        	if (serverSocket == -1) continue;
		int enable = 1;
        	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        	if (bind(serverSocket, record->ai_addr, record->ai_addrlen) == 0) break;
        	close(serverSocket);
    	}



    if (record == NULL) {
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(results);
    printf("Server socket created and bound.\n");

    if (listen(serverSocket, backlog) == -1) {
        exit(EXIT_FAILURE);
    }

        AddNode("Ramapo", "123");
        printf("First UserName and Password were initialized\n");

    	printf("Server started listening.\n");

	int id = 1;
	int i = 1;

    	while (1) {

        	printf("Server still running.\n");

	        //int clientSocket;
        	//struct sockaddr clientAddress;
        	//socklen_t clientAddressLength = sizeof(clientAddress);

		connect = (struct connection_t *)malloc(sizeof(struct connection_t));

        	if ((connect->clientSocket = accept(serverSocket, &connect->clientAddress, &connect->clientAddressLength)) < 0) {
        	    	perror("Failed to accept client socket.");
        	    	exit(EXIT_FAILURE);
        	}

        	printf("Client socket accepted.\n");

		//int id = 1;

		while(id < clientnum){
			//printf("Main While Loop Lap\n");
			if(thrdarr[id] == 0){
				//printf("Made it to If: id = %d", id);
				connect->idx = id;
				pthread_create(&th[id], NULL, client, (void *)connect);
				break;
			}
			id++;
		}
		printf("Main While Loop -> just created a thread\n");
		sleep(1);
	}

                for(; i<10; i++){
			printf("About to Join th[i]: %d\n", i);
                        pthread_join(th[i], NULL);
			printf("Joined\n");
                }

		//sleep(2);

		printf("About to Loop to top of main while loop\n");
	//}//closes main while loop (in main but also where everything happens)

	pthread_mutex_destroy(&mylock);

    return 0;

}

