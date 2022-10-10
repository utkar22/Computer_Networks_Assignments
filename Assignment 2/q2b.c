//This is the program to run concurrent server program with multiple processes
//using fork system call

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

#define PORT 8080
#define SA struct 

int sockfd;
FILE *our_file;


void sigHandler(int sig_num)
{   
    printf("Shutting down the file and the socket.\n");
    fclose(our_file);
    close(sockfd);
    exit(0);
}

long int factorial(int n){
    long int fact = 1;

    for (int i = 2; i<=n; i++){
        fact = fact*i;
    }

    return fact;
}

int main(){
    signal(SIGINT, sigHandler);
    printf("This is the Concurrent Server Program using fork\n");

    int ret, clientSocket;
    struct sockaddr_in serverAddr, cliAddr;
    socklen_t addr_size;
    pid_t childpid;

    sem_t file_lock;
 
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("There was an error in connecting to the socket.\n");
        exit(1);
    }
    else{
        printf("Connected to socket successfully.\n");
    }
 
    // Initializing address structure with NULL
    memset(&serverAddr, '\0', sizeof(serverAddr));
 
    // Assign port number and IP address
    // to the socket created
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
 
    // 127.0.0.1 is a loopback address
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
    // Binding the socket id with
    // the socket structure
    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
 
    // Error handling
    if (ret < 0) {
        printf("Error in binding.\n");
        exit(1);
    }
 
    // Listening for connections (upto 10)
    if (listen(sockfd, 10) == 0) {
        printf("Listening.\n\n");
    }
 
    int cnt = 0;

    //Creating the file
    our_file = fopen("q2b_file.txt", "w");
    sem_init(&file_lock,0,1);
   
    while (1) {
 
        // Accept clients and
        // store their information in cliAddr
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr, &addr_size);
 
        // Error handling
        if (clientSocket < 0) {
            exit(1);
        }
 
        // Creates a child process
        if ((childpid = fork()) == 0) {
            
            // Closing the server socket id
            close(sockfd);
            int count = 0;

            while(1){
 
                // Send a confirmation message
                // to the client
                char inp[2000];
                read(clientSocket, inp, 2000);
                printf("Recieved message %s\n",inp);

                int num;
                num = atoi(inp);

                long int fact = factorial(num); 
                char fact_char[200];

                sprintf(fact_char,"Factorial of %d is %ld; IP Address: %u; Port: %d\n",num,fact,serverAddr.sin_addr.s_addr,serverAddr.sin_port);

                sem_wait(&file_lock);
                fprintf(our_file,"%s",fact_char);
                fflush(our_file);
                sem_post(&file_lock);


                send(clientSocket, fact_char, 200, 0);
                printf("Replied:%s\n\n",fact_char);

                count++;
                if (count == 20){
                    break;
                }
            }
        }
    }
 

    // Close the client socket id
    close(clientSocket);
    // Close the file
    fclose(our_file);
    return 0;
}
