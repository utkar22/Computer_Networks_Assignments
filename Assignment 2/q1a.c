#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define SA struct sockaddr


int main(){
    printf("This is the sequential client program.\n");

    int socket_id, connection_id;
    struct sockaddr_in servaddr;
 
    //Creating the socket
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        printf("The socket creation failed.\n");
        exit(0);
    }
    else{
        printf("The socket was created successfully.\n");
    }

    //Clearing the file
    bzero(&servaddr, sizeof(servaddr));
 
    //Assigning the IP and the Port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
 
    //Connecting the client socket with the server socket
    if (connect(socket_id, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        printf("The connection with the server failed.\n");
        exit(0);
    }
    else{
        printf("Connected to the server successfully.\n");
    }

    printf("Starting sending messages\n");
    for (int i = 1; i<=20; i++){
        int len;
        char fact[200];

        if (i<10){
            len = 1;
        }
        else{
            len = 2;
        }

        char num[len];
        sprintf(num,"%d",i);

        send(socket_id, num, len, 0);
        printf("Sent message: %s\n",num);

        read(socket_id, fact, 200);
        printf("Recieved message:\n%s\n\n",fact);

    }

    printf("Closing the socket\nExiting\n");
    close(socket_id);
    return 0;
}

//Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
