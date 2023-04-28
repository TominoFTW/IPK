#include <iostream>
#include <string>
#include <csignal>

#pragma once

struct connection {
    const char *server_hostname;
    int port_number;
    const char *mode;
};

void print_help();

struct connection arg_parse(int argc, char *const argv[]);
