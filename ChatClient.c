#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(){

        const char *hostname = "127.0.0.1";
        const char *portNumber = "8080";

        int clientSocket; //socketFD
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        struct addrinfo *results; //const for head
        struct addrinfo *record; //temp for traversing
        int error;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        error = getaddrinfo(hostname, portNumber, &hints, &results);
        //do error checking
        if(error >= 0){
                printf("client: getaddressinfo() succesful\n");
        }

        //traversing the list
        for(record = results; record != NULL; record = record->ai_next){
                clientSocket = socket(record->ai_family, record->ai_socktype, 0);
                if(clientSocket == -1){
                        continue;
                }
                if(connect(clientSocket,record->ai_addr,record->ai_addrlen)!= -1){
                        break;
                }
                close(clientSocket);
        }
        //socket() and connect() both were sucess
        //send()
        if(record == NULL){
                printf("Error\n");
                exit(EXIT_FAILURE);
        }
        freeaddrinfo(results);
        printf("socket status:created and connected\n");

	//Collect Username input from user
	char username[20];// = "Hello";
	printf("Please Enter A Username input\n");
	scanf("%19s", username);

	printf("Collected Username Input %s\n", username);

        if(send(clientSocket, username, strlen(username), 0) == -1){
                printf("Error\n");
                exit(EXIT_FAILURE);
        }
        else{
                printf("Msg sent successfully\n");
        }

	sleep(1);

	//Collect Password input

	//Collect Respone Message from Server
	char buffer[1024];
	int len;
	if ( (len = recv(clientSocket, buffer, sizeof(buffer), 0)) == -1){
                printf("len = %d\n", len);
                perror("Failed to receive message.");
                exit(EXIT_FAILURE);
        }

	printf("%s\n", buffer);

	//Collect Password input
	char password[20];// = "Hello";
        printf("Please Enter A Password input\n");
        scanf("%19s", password);

	printf("Collected Password Input %s\n", username);

        if(send(clientSocket, password, strlen(password), 0) == -1){
                printf("Error\n");
                exit(EXIT_FAILURE);
        }
        else{
                printf("Msg sent successfully\n");
        }

	//Recieve Final Confirmation Message
	memset(buffer, 0, sizeof(buffer));
        if ( (len = recv(clientSocket, buffer, sizeof(buffer), 0)) == -1){
                printf("len = %d\n", len);
                perror("Failed to receive message.");
                exit(EXIT_FAILURE);
        }

	printf("Confirmation MSG:   %s\n", buffer);

	if(strcmp(buffer, "Login Was Succes") == 0){
		printf("You have Successfully Logged in, You may begin chatting\n");
	}
	else{
		printf("Closing ClientSocket\n");
		close(clientSocket);
		exit(EXIT_FAILURE);
	}

	while(1){
		//memset(message, 0, sizeof(message));
		char message[20];
		printf("\nPlease enter a message\n");
		scanf("%19s", message);
		//memset(message, 0, sizeof(message));
		if(send(clientSocket, message, strlen(message), 0) == -1){
                	printf("Error\n");
                	exit(EXIT_FAILURE);
        	}
		else{
			printf("Msg sent successfully\n");
		}
		sleep(1);
		memset(message, 0, sizeof(message));
	}

        close(clientSocket);
        return 0;
}

