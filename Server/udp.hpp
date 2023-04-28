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

#pragma once

void signalHandlerUDP(int signal);

std::string UDP(struct connection con);
