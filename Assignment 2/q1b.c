//This is the program to run a concurrent client.
//It utilises threads to generate n number of requests.

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080
#define SA struct sockaddr


void * thread_func(){
    int socket_id, connection_id;
    struct sockaddr_in servaddr;
 
    //Creating the socket
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        exit(0);
    }

    //Clearing the file
    bzero(&servaddr, sizeof(servaddr));
 
    //Assigning the IP and the Port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    //Connecting the client socket with the server socket
    if (connect(socket_id, (SA*)&servaddr, sizeof(servaddr))!= 0) {
        exit(0);
    }

    //int i = rand()%9 + 1;
    int len;

    for (int i = 1; i<=20; i++){
        
        char fact[200];

        if (i<10){
            len = 2;
        }
        else{
            len = 3;
        }

        char num[len];
        sprintf(num,"%d",i);

        send(socket_id, num, len, 0);
        printf("Sent message: %s\n",num);

        read(socket_id, fact, 200);
        printf("Recieved message:\n%s\n\n",fact);

        sleep(rand()%3);
    }

    close(socket_id);
}


int main(){
    printf("Concurrent Clients using threads.\n");
    int total_threads;
    printf("Total number of threads you want: ");
    scanf("%d",&total_threads);
    printf("\n");

    pthread_t arr[total_threads];

    for (int i = 0; i<total_threads; i++){
        pthread_create(&arr[i], NULL, thread_func, NULL);
    }

    for (int i = 0; i<total_threads; i++){
        pthread_join(arr[i],NULL);
    }
    
    return 0;
}

//Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/