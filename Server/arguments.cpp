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

#include "arguments.hpp"

void print_help() {
    fprintf(stderr,"usage: ./ipkcpd -h <server_hostname> -p <port_number> -m <mode>\n");
    fprintf(stderr,"\tserver_hostname - IPv4 address\n\tport_number - 1024-65535\n\tmode - tcp/udp\n");
    return;
}

struct connection arg_parse(int argc, char *const argv[]){
    // ./ipkcpc --help
    if ((argc == 2 and !strcmp(argv[1], "--help")) or argc != 7) {
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
