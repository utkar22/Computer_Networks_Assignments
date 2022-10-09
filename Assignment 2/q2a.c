//This is the program to run the sequential server.
//For each request, it computes the factorial of the number,
//and stores it in the file and sends it back to the client as well

#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 8080
#define SA struct sockaddr

long int factorial(int n){
    long int fact = 1;

    for (int i = 2; i<=n; i++){
        fact = fact*i;
    }

    return fact;
}

int main(){
    printf("This is the Sequential Server Program\n");
    
    int socket_id, connection_id, len;
    struct sockaddr_in servaddr, cli;
   
    //Creating the socket
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        printf("The socket failed to connect.\n");
        exit(0);
    }
    else{
        printf("The socket was created successfully.\n");
    }
    bzero(&servaddr, sizeof(servaddr));
   
    //Assigning IP and Port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    //Binding
    if ((bind(socket_id, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("The socket binding failed.\n");
        exit(0);
    }
    else{
        printf("The socket was binded successfully.\n");
    }
   
    //Listening and verification
    if ((listen(socket_id, 5)) != 0) {
        printf("Listen failed.\n");
        exit(0);
    }
    else{
        printf("The server is listening.\n");
    }
    len = sizeof(cli);
   
    //Accepting data packet from client
    connection_id = accept(socket_id, (SA*)&cli, &len);
    if (connection_id < 0) {
        printf("Client was not accepted by the server.\n");
        exit(0);
    }
    else{
        printf("The server accepted the client.\n");
    }

    close(socket_id);

    //Creating the file
    FILE *our_file = fopen("q2a_file.txt", "w");
    

    while (1){
        char inp[2000];
		read(connection_id, inp, 2000);
        printf("Recieved message %s\n",inp);

        int num;
        num = atoi(inp);

        long int fact = factorial(num); 
        char fact_char[200];

        sprintf(fact_char,"Factorial of %d is %ld; IP Address: %u; Port: %d\n",num,fact,servaddr.sin_addr.s_addr,servaddr.sin_port);

        fprintf(our_file,"%s",fact_char);

        send(connection_id, fact_char, 200, 0);
        printf("Replied:\n%s\n\n",fact_char);

        if (num == 20){
            printf("Shutting down server.\n");
            break;
        }
    }

    fclose(our_file);
    close(socket_id);
    return 0;
}
