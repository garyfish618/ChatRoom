#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <limits.h>
#include <unistd.h>

#define MAXDATASIZE 100

// This is the client

int main() {
    char buf[MAXDATASIZE];

    // ---- Inputs to getaddrinfo ----
    char hostname[HOST_NAME_MAX];
    const char* port = "3490";

    std::cout << "Enter hostname to connect to" << "\n"; 
    std::cin >> hostname;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;


    // --- Output to getaddrinfo ---

    struct addrinfo* results;

    int errCode = getaddrinfo(hostname, port, &hints, &results); //Resolves hostname


    if(errCode != 0) {

        fprintf(stderr, "getaddrinfo error: %s (%i) \n", gai_strerror(errCode), errCode); //Prints an error to the std output stream
        return 2;
    }

    struct addrinfo info = *results;
    struct sockaddr_in addressInfo = *((sockaddr_in*) (info.ai_addr));

    char ipAddress[INET_ADDRSTRLEN];
    
    inet_ntop(results->ai_family, &(addressInfo.sin_addr), ipAddress, results->ai_addrlen);

    std::cout << ipAddress << "\n";

    int sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

    if(sock == -1) {
        fprintf(stderr, "Failed to create a socket with the given address");
        return 2;

    }

    std::cout << "Socket created" << "\n";

    struct addrinfo *nextAddr;
    for(nextAddr = results; nextAddr != NULL; nextAddr = nextAddr->ai_next ) {
        std::cout << "Trying next result" << "\n";
        if(connect(sock, results->ai_addr, results->ai_addrlen) == -1) {
        
            fprintf(stderr, "Failed to connect");
            continue;

        }
        std::cout << "Connected" << "\n";
        break;
    }

    freeaddrinfo(results);

    int bytesRead = recv(sock, &buf, MAXDATASIZE, 0);

    buf[bytesRead] = '\0';

    std::cout << buf << "\n";
    close(sock);

}