#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include<semaphore.h>
#define PORT 8080
#define SA struct sockaddr

sem_t lock;
int total_threads = 10;

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

    int i = rand()%10;

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

    close(socket_id);
    sem_post(&lock);
}


int main(){
    pthread_t arr[20];

    sem_init(&lock, 0, 10);

    for (int i = 0; i<20; i++){
        printf("Thread number:%d; Free Threads:%d",i,lock);
        sem_wait(&lock);
        pthread_create(&arr[i], NULL, thread_func, NULL);
    }

    sleep(30);
    
    return 0;
}

//Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/