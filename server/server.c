#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "../socket_utils/sock_utils.h" 
#include "../socket_utils/sock_utils.c" 

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    bool acceptedSuccess;
};

struct AcceptedSocket acceptedConnections[10];
int countConnections = 0;

void *handlingAcceptedSockets(void *arg) {
    int socketFD = *(int*)arg; 
    free(arg);  // Free the allocated memory for the socket descriptor
  
    char buffer[1024];

    while (true) {
        ssize_t msgReceived = recv(socketFD, buffer, 1024, 0);

        if (msgReceived > 0) {
            buffer[msgReceived] = 0;
            printf("%s\n", buffer); 
            
            for (int i = 0; i < countConnections; i++) {
                if (acceptedConnections[i].acceptedSocketFD != socketFD) {
                    send(acceptedConnections[i].acceptedSocketFD, buffer, strlen(buffer), 0);
                }
            }
        }
        if (msgReceived == 0) {
            break;
        }
    }
    close(socketFD);
    pthread_exit(NULL); 
}

int main() {
    int server_FD = CreateTCPIpv4Socket();
    struct sockaddr_in *serverAddress = CreateTCPIpv4Address("", 8080);

    if (bind(server_FD, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) == 0) {
        printf("Socket was bound successfully\n");
    } else {
        perror("Bind failed");
        exit(1); 
    }

    if (listen(server_FD , 10) < 0) {
        perror("Listen failed");
        return 1; 
    } else {
        printf("Listening on -----> \n"); 
    }

    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int *new_clientSocketFD = malloc(sizeof(int)); // Allocate memory for the socket descriptor
        *new_clientSocketFD = accept(server_FD, (struct sockaddr*)&clientAddress, &clientAddressSize);

        if (*new_clientSocketFD == -1) {
            perror("Client socket accept failed!! \n");
            free(new_clientSocketFD); // Free memory on error
            continue; 
        } else {
            printf("Client socket accepted...\n");
        }

        acceptedConnections[countConnections].acceptedSocketFD = *new_clientSocketFD;
        acceptedConnections[countConnections].address = clientAddress;
        acceptedConnections[countConnections].acceptedSuccess = true;
        countConnections++;

        pthread_t thread;
        pthread_create(&thread, NULL, handlingAcceptedSockets, new_clientSocketFD); 
        pthread_detach(thread); 
    }

    shutdown(server_FD, SHUT_RDWR);
    return 0;
}