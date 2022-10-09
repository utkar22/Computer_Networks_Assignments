//This is the program to run concurrent server program with multiple processes
//using threads

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define SA struct sockaddr

int sockfd;
struct sockaddr_in serverAddr;
pid_t childpid;
sem_t file_lock;
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

void * thread_func(void *cS){
    
    int clientSocket;
    clientSocket = *(int*) cS;
    //printf("Socket ID there: %d\n",clientSocket);
    // Closing the server socket id
    //close(sockfd);

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
    sem_post(&file_lock);
    


    send(clientSocket, fact_char, 200, 0);
    printf("Replied:%s\n\n",fact_char);
}

int main(){
    signal(SIGINT, sigHandler);

    printf("This is the Concurrent Server Program using threads\n");

    int ret;
    int clientSocket;
    struct sockaddr_in cliAddr;
    socklen_t addr_size;
 
    
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
    our_file = fopen("q2c_file.txt", "w");
    sem_init(&file_lock,0,1);
   
    while (1) {
 
        // Accept clients and
        // store their information in cliAddr
        
        clientSocket = accept(sockfd, (struct sockaddr*)&cliAddr, &addr_size);
 
        // Error handling
        if (clientSocket < 0) {
            printf("There was some error in connection.\n");
            exit(1);
        }

 
        // Creates a thread
        pthread_t thread;
        //printf("Socket ID here: %d\n",clientSocket);

        //int *cS = malloc(sizeof(*cS));
        pthread_create(&thread, NULL, thread_func, (void*)&clientSocket);
        pthread_join(thread, NULL);
        fflush(our_file);
    }

    //sleep(60);
 

    // Close the client socket id
    close(clientSocket);
    // Close the file
    fclose(our_file);
    return 0;
}
