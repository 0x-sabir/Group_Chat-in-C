#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../socket_utils/sock_utils.h" 
#include "../socket_utils/sock_utils.c" 


// void* receiveAndPrintThread(void* socketFDPtr) 
// {
//     int socketFD = *(int*) socketFDPtr;
//     char buffer[1024];

//         while (true) {
//         ssize_t messageReceived = recv(socketFD, buffer, sizeof(buffer), 0); 

//         if (messageReceived > 0) {
//             buffer[messageReceived] = '\0'; 
//             printf("Response was: %s\n", buffer); 
//         } else if (messageReceived == 0) {
//             printf("Connection closed by server.\n");
//             break; 
//         } else {
//             perror("recv failed");
//             break;
//         }
//     }

//     close(socketFD);
//     pthread_exit(NULL);

// }


// int main() {

//     int socketFD = CreateTCPIpv4Socket();
//     struct sockaddr_in *address = CreateTCPIpv4Address("127.0.0.1", 8080);

//     int result = connect(socketFD, (struct sockaddr*)address, sizeof(*address));

//     if(result == 0)
//         printf("connection was successful\n");
//     else
//         printf("Connection to server failed!!");

//     pthread_t listenerThread;
//     if (pthread_create(&listenerThread, NULL, receiveAndPrintThread, &socketFD) != 0) {
//         perror("Thread creation failed");
//         close(socketFD); 
//         return 1; 
//     }


//     char *name = NULL;
//     size_t nameSize = 0;
//     printf("please enter your username?\n");
//     ssize_t nameCount = getline(&name, &nameSize, stdin);
//     name[nameCount-1] = 0;

//     printf("type your message to server Or (type exit)...\n");

//     char *message = NULL;
//     size_t msgSize = 0;
//     char buffer[1024];

    
//     while(true) {
//         ssize_t charCount = getline(&message, &msgSize, stdin);
//         message[charCount-1] = 0;

//         sprintf(buffer, "%s:%s", name, message);

//         if(charCount > 0) {
//             if(strcmp(message, "exit") == 0) {
//                 free(name);
//                 free(message);
//                 break;
//             }

//             ssize_t amountSent = send(socketFD, buffer, strlen(buffer), 0);
//             if (amountSent <= 0) {
//             perror("send failed");
//             break;
//         }
//         }
//     }
 
//     close(socketFD);

//     return 0;
// }


void* receiveAndPrintThread(void* socketFDPtr) {
    int socketFD = *(int*)socketFDPtr;
    free(socketFDPtr); // Free the allocated memory
    char buffer[1024];

    while (true) {
        ssize_t messageReceived = recv(socketFD, buffer, sizeof(buffer), 0);

        if (messageReceived > 0) {
            buffer[messageReceived] = '\0'; 
            printf("Response was: %s\n", buffer); 
        } else if (messageReceived == 0) {
            printf("Connection closed by server.\n");
            break; 
        } else {
            perror("recv failed");
            break;
        }
    }

    close(socketFD);
    pthread_exit(NULL);
}

int main() {
    int socketFD = CreateTCPIpv4Socket();
    struct sockaddr_in *address = CreateTCPIpv4Address("127.0.0.1", 8080);

    int result = connect(socketFD, (struct sockaddr*)address, sizeof(*address));

    if (result == 0) {
        printf("connection was successful\n");
    } else {
        perror("Connection to server failed");
        close(socketFD);
        return 1;
    }

    int *socketFDCopy = malloc(sizeof(int));
    *socketFDCopy = socketFD;

    pthread_t listenerThread;
    if (pthread_create(&listenerThread, NULL, receiveAndPrintThread, socketFDCopy) != 0) {
        perror("Thread creation failed");
        free(socketFDCopy);
        close(socketFD); 
        return 1; 
    }

    char *name = NULL;
    size_t nameSize = 0;
    printf("Please enter your username:\n");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    if (nameCount > 1) {
        name[nameCount - 1] = 0; // Remove newline
    } else {
        printf("Username cannot be empty. Exiting.\n");
        free(name);
        close(socketFD);
        return 1;
    }

    printf("Type your message to server or (type 'exit')...\n");

    char *message = NULL;
    size_t msgSize = 0;
    char buffer[1024];

    while (true) {
        ssize_t charCount = getline(&message, &msgSize, stdin);
        if (charCount <= 1) continue; // Ignore empty messages
        message[charCount - 1] = 0; // Remove newline

        if (strcmp(message, "exit") == 0) {
            printf("Exiting...\n");
            free(name);
            free(message);
            break;
        }

        snprintf(buffer, sizeof(buffer), "%s: %s", name, message);

        ssize_t amountSent = send(socketFD, buffer, strlen(buffer), 0);
        if (amountSent <= 0) {
            perror("send failed");
            break;
        }
    }

    close(socketFD);
    return 0;
}