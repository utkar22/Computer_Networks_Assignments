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
#include <sys/epoll.h>

#define DATA_BUFFER 10000
#define MAX_CONNECTIONS 11

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
    int server_fd, ret_val, i, efd, s;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    struct pollfd *pfds;
    int nfds = MAX_CONNECTIONS-1;
    int num_open_fds = nfds;
    struct epoll_event pollfd;
    struct epoll_event *pollfds;

    /*Get the socket server fd*/
    server_fd = create_tcp_server_socket();
    if (server_fd == -1){
        printf("Failed to create the server\n");
        return -1;
    }
    else{
        printf("Created the server successfully.\n");
    }

    efd = epoll_create1(0);
    pollfd.data.fd = server_fd;
    pollfd.events = EPOLLIN;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, server_fd, &pollfd);
    if (s==-1){
        perror("Error epoll_ctl");
        abort();
    }

    /*Buffer where events are returned*/
    pollfds = (struct epoll_event*)calloc(MAX_CONNECTIONS, sizeof(pollfd));

    for (int i = 0; i<MAX_CONNECTIONS; i++){
        pollfds[i].data.fd = 0;
        pollfds[i].events = EPOLLIN;
    }

    //Creating the file
    our_file = fopen("q2d3_file.txt", "w");
    sem_init(&file_lock,0,1);

    while (1){
        int new_fd;
        /*Invoke epoll_wait() then wait*/
        ret_val = epoll_wait(efd, pollfds, MAX_CONNECTIONS, -1);

        if (ret_val<=0){
            break;
        }

        for (int i = 0; i<ret_val; ++i){
            if ((pollfds[i].events & EPOLLIN) && pollfds[i].data.fd == server_fd){
                
                int new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);

                if (new_fd == -1){
                    perror("accept");
                    exit(EXIT_FAILURE);
                }   

                pollfd.data.fd = new_fd;
                pollfd.events = EPOLLIN;


                s = epoll_ctl(efd, EPOLL_CTL_ADD, new_fd, &pollfd);
                if (s==-1){
                    perror("Error epoll_ctl");
                    abort();
                }
            }
            else if(pollfds[i].events & EPOLLIN){
                bzero(buf,sizeof(buf));
                int bufSize = read(pollfds[i].data.fd, buf, DATA_BUFFER);
                if (bufSize == -1){
                    pollfds[i].data.fd = 0;
                    pollfds[i].events = 0;
                    printf("Error reading");
                }
                else if (bufSize == 0){
                    close(pollfds[i].data.fd);
                    pollfds[i].data.fd = 0;
                    pollfds[i].events = 0;
                    break;
                }
                else{
                    //STUFF
                    printf("Recieved message: %s\n",buf);
                    char fact_char[200];
                    int num = atoi(buf);
                    long fact = factorial(num);
                    sprintf(fact_char,"Factorial of %d is %ld; IP Address: %u; Port: %d\n",num,fact,saddr.sin_addr.s_addr,saddr.sin_port);
                    
                    
                    sem_wait(&file_lock);
                    fprintf(our_file,"%s",fact_char);
                    fflush(our_file);
                    sem_post(&file_lock);
                    

                    send(pollfds[i].data.fd, fact_char, 200, 0);
                    printf("Replied: %s\n\n",fact_char);
                }
            }
        }
    }
    

    free(pollfds);
    close(server_fd);
    return 0;
}