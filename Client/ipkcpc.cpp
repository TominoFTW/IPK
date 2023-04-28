/**
 * @file ipkcpc.cpp
 * @brief Client for IPK project 1
 * @author Tomas Behal xbehal02
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <csignal>
#include <sys/time.h>

#define BUFSIZE 1024
int client_socket;

struct connection {
    const char *server_hostname;
    int port_number;
    const char *mode;
};

void print_help() {
    fprintf(stderr,"usage: ./ipkcpc -h <server_hostname> -p <port_number> -m <mode>\n");
    fprintf(stderr,"\tserver_hostname - IPv4 address\n\tport_number - 1024-65535\n\tmode - tcp/udp\n");
}

void signalHandlerTCP(int signal) {
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE);
    send(client_socket, "BYE\n", 4, 0);
    recv(client_socket, buffer, BUFSIZE, 0);
    if (!strcmp(buffer, "BYE\n")){
        close(client_socket);
        fprintf(stdout, "%s", buffer);
        exit(signal);
    }
    // else server is not responding or sent something else
    exit(signal);
}

void signalHandlerUDP(int signal) {
    close(client_socket);
    exit(signal);
}

struct connection arg_parse(int argc, char *const argv[]){
    // ./ipkcpc --help
    if ((argc == 2 && !strcmp(argv[1], "--help")) || argc != 7) {
        print_help();
        exit(EXIT_FAILURE);
    }
    struct connection con = {NULL, 0, NULL};
    int opt;
    while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
        switch (opt) {
            case 'h':
                con.server_hostname = optarg;
                break;
            case 'p':
                con.port_number = atoi(optarg);
                break;
            case 'm':
                con.mode = optarg;
                break;
            case '?':
                print_help();
                exit(EXIT_FAILURE);
        }
    }
    return con;
}

int main (int argc, char * argv[]) {
    int bytestx, bytesrx;
    socklen_t serverlen;
    struct hostent *server;
    struct sockaddr_in server_address;
    char buf[BUFSIZE];
    struct connection con = {NULL, 0, NULL};

    // Check validity of command line arguments
    con = arg_parse(argc, argv);
    // If some arguments are missing
    if (con.server_hostname == NULL || con.port_number < 1024
        || con.port_number > 65535 || con.mode == NULL) {
        print_help();
        exit(EXIT_FAILURE);
    }

    // Invalid port number
    if (con.port_number < 1024 || con.port_number > 65535) {
        fprintf(stderr,"ERROR: port %d out of range\n", con.port_number);
        print_help();
        exit(EXIT_FAILURE);
    }

    // Invalid mode
    if (!strcmp(con.mode, "tcp")) {
        std::signal(SIGINT, signalHandlerTCP);
    }
    else if (!strcmp(con.mode, "udp")) {
        std::signal(SIGINT, signalHandlerUDP);
    }
    else{
        fprintf(stderr,"ERROR: no such mode as %s\n", con.mode);
        print_help();
        exit(EXIT_FAILURE);
    }

    // Get server address via DNS
    if ((server = gethostbyname(con.server_hostname)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", con.server_hostname);
        exit(EXIT_FAILURE);
    }

    // IP address of server and port number
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(con.port_number);

    if (!strcmp(con.mode, "tcp")) {
        // Create socket
        if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
        }

        // Connect to server
        if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
            perror("ERROR: connect");
            exit(EXIT_FAILURE);        
        }
        while(true) {
            // Get message from user
            bzero(buf, BUFSIZE);
            if (fgets(buf, BUFSIZE, stdin) == NULL) {
                send(client_socket, "BYE\n", 4, 0);
                recv(client_socket, buf, BUFSIZE, 0);
                fprintf(stdout, "%s", buf);
                break;
            }
            
            // Validate message length
            if (buf [1022] != '\0' && buf [1022] != '\n'){
                send(client_socket, "BYE\n", 4, 0);
                bzero(buf, BUFSIZE);
                recv(client_socket, buf, BUFSIZE, 0);
                while(getchar() != '\n');
                fprintf(stdout, "%s", buf);
                fprintf(stderr, "ERROR: message too long\n");
                close(client_socket);
                exit(EXIT_FAILURE);
            }

            // Send message to server
            bytestx = send(client_socket, buf, strlen(buf), 0);
            if (bytestx < 0) {
                perror("ERROR in sendto");
                exit(EXIT_FAILURE);
            }
            
            bzero(buf, BUFSIZE);

            // Receive message from server
            bytesrx = recv(client_socket, buf, BUFSIZE, 0);
            if (bytesrx < 0) {
                perror("ERROR in recvfrom");
                exit(EXIT_FAILURE);
            }
            
            fprintf(stdout, "%s", buf);
            // If server sent BYE, close socket and exit
            if (!strcmp(buf, "BYE\n")) {
                break;
            }

        }
            

    }
    else if (!strcmp(con.mode,"udp")) {
        // Create socket
        if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
            perror("ERROR: socket");
            exit(EXIT_FAILURE);
	    }
        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

        while(true) {
            // Get message from user
            bzero(buf, BUFSIZE);
            if (fgets(buf, BUFSIZE, stdin) == NULL) {
                break;
            }

            // Prepare message to send
            buf[strlen(buf)-1] = '\0';
            serverlen = sizeof(server_address);
            int length = strlen(buf);

            // Validate message length
            if (length > 255) {
                fprintf(stderr, "ERROR: message too long\n");
                close(client_socket);
                exit(EXIT_FAILURE);
            }

            char* buffer = new char[length + 2];
            buffer[0] = '\0';
            buffer[1] = length;

            memcpy(buffer + 2, buf, length);

            // Send message to server
            bytestx = sendto(client_socket, buffer, strlen(buf)+2, 0, (const struct sockaddr *) &server_address, serverlen);
            if (bytestx < 0) {
                perror("ERROR: sendto");
                exit(EXIT_FAILURE);
            }

            // Delete temporary buffer
            bzero(buf, BUFSIZE);
            delete[] buffer;
            
            // Receive message from server
            bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *) &server_address, &serverlen);
            if (bytesrx < 0) {
                perror("ERROR: recvfrom");
                exit(EXIT_FAILURE);
            }

            // Print message from server depending if is's OK or ERR message
            if (buf[1] == 0)
                fprintf(stdout, "OK:%s\n", buf + 3);
            else
                fprintf(stdout, "ERR:%s\n", buf + 3);

        }
    }
    
    close(client_socket);
    return 0;
}
