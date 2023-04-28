#include "udp.hpp"
#include "arguments.hpp"
#include "parsing.hpp"

#define BUFSIZE 258

void signalHandlerUDP(int signal) {
    exit(signal);
}

std::string UDP(struct connection con) {
    char buf[BUFSIZE];
    int server_socket, bytestx, bytesrx;
    socklen_t clientlen;
    struct sockaddr_in client_address, server_address;
    int optval;

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

    optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));


    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(con.server_hostname);
    server_address.sin_port = htons((unsigned short)con.port_number);
	
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
    {
        perror("ERROR: binding");
        exit(EXIT_FAILURE);
    }

    while(1) 
    {   
        /* Waiting for messages from clients */
        bzero(buf, BUFSIZE);
        clientlen = sizeof(client_address);
        bytesrx = recvfrom(server_socket, buf, BUFSIZE, 0, (struct sockaddr *) &client_address, &clientlen);
        if (bytesrx < 0) 
            perror("ERROR: recvfrom:");
    
        /* Message validity */
        if (buf[0] != '\0') {
            char error[] = "ERROR: Invalid message";
            buf[0] = '\1';
            buf[1] = '\1';
            buf[2] = strlen(error);
            strcpy(buf+3, error);
        }
        else {
            /* Prepare message for parsing */
            buf[strlen(buf)] = '\0';
            std::string message(buf+2);
            message = message.substr(0, buf[1]);
            std::string response = parse_message(con.mode, message);

            if (response.find('E') != std::string::npos) {
                bzero(buf, BUFSIZE);
                buf[0] = '\1';
                buf[1] = '\1';
                buf[2] = strlen(response.c_str());
                strcpy(buf+3, response.c_str());
            } else {
                bzero(buf, BUFSIZE);
                buf[0] = '\1';
                buf[1] = '\0';
                buf[2] = strlen(response.c_str());
                strcpy(buf+3, response.c_str());
            }
        }
        
        /* Send response to client */
        gethostbyaddr((const char *)&client_address.sin_addr.s_addr, sizeof(client_address.sin_addr.s_addr), AF_INET);
              
        inet_ntoa(client_address.sin_addr);
        
        bytestx = sendto(server_socket, buf, strlen(buf+3)+3, 0, (struct sockaddr *) &client_address, clientlen);
        if (bytestx < 0) {
            perror("ERROR: sendto:");
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket);
    return 0;
}
