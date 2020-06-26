#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX_CONNECTIONS = 10;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


int main(int argc, char *argv[]) {

    if(argc != 3) {
        printf("USAGE: server [Hostname] [Port] \n");
        return 1;
    }

    char* host = argv[1];
    char* port = argv[2];    

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* results;
    struct sigaction sa;
    struct sockaddr_storage connectionAddr;

    int errCode = getaddrinfo(NULL, port, &hints, &results);

    if(errCode != 0) {

        fprintf(stderr, "getaddrinfo error: %s (%i) \n", gai_strerror(errCode), errCode); //Prints an error to the std output stream
        return 2;
    }

    int sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    int optval = 1;
 
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    if(sock == -1) {
        fprintf(stderr, "Failed to create a socket and initialize server");
    }


    if(bind(sock, results->ai_addr, results->ai_addrlen) != 0 ) {
        printf("Bind error");
    }

    if(results == NULL) {
        printf("Bind failed");
    }
    
    if(listen(sock, MAX_CONNECTIONS) != 0) {
        printf("Listen error"); 

    } 


    freeaddrinfo(results);

    socklen_t addr_size;

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    
    int incomingSock = -1;
    //Start accepting connections
    while(true) {
        addr_size = sizeof connectionAddr;
        incomingSock = accept(sock, (struct sockaddr*)&connectionAddr,&addr_size);

        if(incomingSock == -1) {
            //printf("No connection found...");
            continue;
        }
        std::cout << "Incomming connection";
        break;
    }



    const char* message = "Hello";

    if ( send(incomingSock, message, 5, 0) == -1 ) {
        printf("SEND ERROR");
    }

    printf("Server established connection");

    close(incomingSock);
    


}
