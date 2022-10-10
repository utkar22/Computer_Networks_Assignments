#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <signal.h>
#include <poll.h>

#define DATA_BUFFER 10000
#define MAX_CONNECTIONS 20

struct sockaddr_in saddr;
FILE *our_file;

void sigHandler(int sig_num)
{   
    printf("Shutting down the file and the socket.\n");
    fclose(our_file);
    exit(0);
}

long int factorial(int n){
    long int fact = 1;

    for (int i = 2; i<=n; i++){
        fact = fact*i;
    }

    return fact;
}

int create_tcp_server_socket(){
    int fd, ret_val;

    /* Create a TCP socket*/
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1){
        printf("Socket creation failed.\n");
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /*Initialize the socket address structure*/
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in))!=0){
        printf("Binding failed.\n");
        close(fd);
        return -1;
    }
    else{
        printf("Binded successfully.\n");
    }

    if (listen(fd,5)!=0){
        printf("Listen failed.\n");
        close(fd);
        return -1;
    }
    else{
        printf("Listening.\n");
    }
    
    return fd;
}

int main(){
    signal(SIGINT, sigHandler);
    sem_t file_lock;

    struct sockaddr_in new_addr;
    int server_fd, ret_val, i;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    int all_connections[MAX_CONNECTIONS];
    struct pollfd *pfds;
    int nfds = MAX_CONNECTIONS - 1;
    int num_open_fds = nfds;
    struct pollfd pollfds[MAX_CONNECTIONS + 1];

    /*Get the socket server fd*/
    server_fd = create_tcp_server_socket();
    if (server_fd == -1){
        printf("Failed to create a server\n");
        return -1;
    }
    else{
        printf("Created server successfully.\n");
    }

    pollfds[0].fd = server_fd;
    pollfds[0].events = POLLIN;
    int useClient = 0;

    for (int i = 1; i<MAX_CONNECTIONS; i++){
        pollfds[i].fd = 0;
        pollfds[i].events = POLLIN;
    }

    //Creating the file
    our_file = fopen("q2d2_file.txt", "w");
    sem_init(&file_lock,0,1);

    while(1){
        /*Invoke poll() and then wait!*/
        ret_val = poll(pollfds, MAX_CONNECTIONS, 5000);

        /* poll() woke up, Identify the fd that has events*/
        if (ret_val >=0){
            if (pollfds[0].revents & POLLIN){
                int new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0){
                    for (int i = 1; i<MAX_CONNECTIONS; i++){
                        if (pollfds[i].fd == 0){
                            pollfds[i].fd = new_fd;
                            pollfds[i].events = POLLIN;
                            useClient++;
                            break;
                        }
                    }
                }
                ret_val--;
                if (!ret_val){
                    continue;
                }
            }
            /*Check if the fd with event is a non-server fd*/
            for (i=1; i<MAX_CONNECTIONS; i++){
                if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN){
                    memset(buf, 0, sizeof(buf));
                    /*Read incoming data*/
                    int bufSize = read(pollfds[i].fd,buf, DATA_BUFFER -1);

                    if (bufSize == -1){
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                    }
                    else if (bufSize == 0){
                        close(pollfds[i].fd);
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                    }
                    else{
                        //Message in buf
                        printf("Recieved message: %s\n",buf);
                        char fact_char[200];
                        int num = atoi(buf);
                        long fact = factorial(num);
                        sprintf(fact_char,"Factorial of %d is %ld; IP Address: %u; Port: %d\n",num,fact,saddr.sin_addr.s_addr,saddr.sin_port);
                        
                        sem_wait(&file_lock);
                        fprintf(our_file,"%s",fact_char);
                        fflush(our_file);
                        sem_post(&file_lock);

                        send(pollfds[i].fd, fact_char, 200, 0);
                        printf("Replied: %s\n\n",fact_char);
                    }
                }
            }
        }
        else{
            printf("Failed to connect.");
        }
    }

    return 0;
}