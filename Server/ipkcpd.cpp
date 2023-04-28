#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define BUFSIZE 512
int server_socket;


#include "udp.hpp"
#include "tcp.hpp"
#include "parsing.hpp"
#include "ipkcpd.hpp"
#include "arguments.hpp"


int main (int argc, const char * argv[]) {
    std::string message;
    std::string response;

    struct connection con = arg_parse(argc, (char *const *)argv);

    /* IP not set or invalid */
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

    if (!strcmp(con.mode, "udp")) {
        response = UDP(con);
    }
    else if (!strcmp(con.mode, "tcp")) {
        response = TCP(con);
    }

    return 0;
}
