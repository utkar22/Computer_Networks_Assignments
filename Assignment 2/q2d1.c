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

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 10

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
    
    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, ret_val, i;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    int all_connections[MAX_CONNECTIONS];

    sem_t file_lock;

    /*Get the socket server fd*/
    server_fd = create_tcp_server_socket();
    if (server_fd == -1){
        printf("Failed to create a server\n");
        return -1;
    }
    else{
        printf("Created server successfully.\n");
    }

    /*Initialise all_connections and set the first entry to server fd*/
    for (i = 0;i< MAX_CONNECTIONS; i++){
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

    //Creating the file
    our_file = fopen("q2d1_file.txt", "w");
    sem_init(&file_lock,0,1);

    while(1){
        FD_ZERO(&read_fd_set);
        /*Set the fd_set before passing it to the select call*/
        for (i=0; i< MAX_CONNECTIONS; i++){
            if (all_connections[i]>=0){
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /*Invoke select() and then wait! */
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events*/
        if (ret_val >=0){
            /* Check if the fd with event is the server fd*/
            if (FD_ISSET(server_fd, &read_fd_set)){
                /* accept the new connection*/
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0){
                    for (i=0; i<MAX_CONNECTIONS; i++){
                        if (all_connections[i]<0){
                            all_connections[i] = new_fd;
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val) continue;
            }

            /* Check if the fd with event is a non-server fd*/
            for (i=1; i<MAX_CONNECTIONS; i++){
                if ((all_connections[i]>0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))){
                        /*read incoming data*/
                        ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);
                        if (ret_val == 0){
                            close(all_connections[i]);
                            all_connections[i] = -1; /* Connection is now closed*/
                        }
                        if (ret_val>0){
                            printf("Recieved message: %s\n",buf);
                            char fact_char[200];
                            int num = atoi(buf);
                            long fact = factorial(num);
                            sprintf(fact_char,"Factorial of %d is %ld; IP Address: %u; Port: %d\n",num,fact,saddr.sin_addr.s_addr,saddr.sin_port);
                            
                            sem_wait(&file_lock);
                            fprintf(our_file,"%s",fact_char);
                            fflush(our_file);
                            sem_post(&file_lock);

                            send(all_connections[i], fact_char, 200, 0);
                            printf("Replied: %s\n\n",fact_char);

                        }
                        if (ret_val == -1){
                            break;
                        }
                    }
                    ret_val--;
                    if(!ret_val) continue;
            }
        }
    }

    /*Close all the sockets*/
    for (i=0; i<MAX_CONNECTIONS; i++){
        if (all_connections[i]>0){
            close(all_connections[i]);
        }
    }

    // Close the file
    fclose(our_file);

    return 0;
}