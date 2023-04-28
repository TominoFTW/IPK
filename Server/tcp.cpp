#include <sstream>

#include "tcp.hpp"
#include "arguments.hpp"
#include "parsing.hpp"

#define BUFSIZE 1024
#define MAX_CLIENTS 16

int client_sockets[MAX_CLIENTS] = {0};
int client_hellos[MAX_CLIENTS] = {-1};
int running = 1;

void signalHandlerTCP(int signal) {
    (void)signal;
    running = 0;
}

std::string TCP(struct connection con) {
    int server_socket;
    int n, i;
    char buf[BUFSIZE];
    struct sockaddr_in server_address;
    std::string response;

    for (i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(con.server_hostname);
    server_address.sin_port = htons((unsigned short)con.port_number);

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("ERROR: binding");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 1) < 0) {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    /* Set up the fd_set for select */
    fd_set read_fds;
    int max_fd = server_socket;
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);

    /* Accept incoming connections and handle them */
    while (running) {
        /* Use select to wait for incoming connections or data from clients */
        fd_set temp_fds = read_fds;

        if (select(max_fd+1, &temp_fds, NULL, NULL, NULL) < 0) {
            continue;
        }

        /* Check for incoming connections */
        if (FD_ISSET(server_socket, &temp_fds)) {
            sockaddr_in client_address;
            socklen_t client_address_size = sizeof(client_address);
            int client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_size);

            if (client_socket < 0) {
                perror("ERROR: accept");
                continue;
            }


            /* Add the client socket to the list of client sockets */
            bool added = false;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_socket;
                    client_hellos[i] = 0;
                    added = true;
                    break;
                }
            }
            if (!added) {
                std::cerr << "Client limit reached" << std::endl;
                continue;
            }

            /* Add the client socket to the fd_set for select */
            FD_SET(client_socket, &read_fds);
            if (client_socket > max_fd) {
                max_fd = client_socket;
            }

        }

        /* Check for data from clients */
        for (int fd = 0; fd <= max_fd; fd++) {
            if (fd == server_socket || !FD_ISSET(fd, &temp_fds))
                continue;

            bzero(buf, BUFSIZE);
            int hello = 0;

            std::string response;
            /* Receive message from the client */
            do {
                n = recv(fd, buf, BUFSIZE, 0);
                if (n < 0) {
                    perror("ERROR: recv");
                    exit(EXIT_FAILURE);
                }
                if (n == 0) {
                    break;
                }
                response += buf;
            } while (n == BUFSIZE);

            while (true) {
                if (response.find('\n') == std::string::npos) {
                    break;
                }
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == fd) {
                        hello = client_hellos[i];
                        break;
                    }
                }
                std::string message = response.substr(0, response.find('\n'));
                response.erase(0, response.find('\n') + 1);

                /* HELLO and its first HELO */
                if (!strcmp(message.c_str(), "HELLO") and !hello) {
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (client_sockets[i] == fd) {
                            client_hellos[i] = 1;
                            break;
                        }
                    }
                    send(fd, "HELLO\n", 6, 0);
                    continue;
                }
                /* query message */
                else if (hello and strcmp(message.c_str(), "BYE")) {
                    message = parse_message(con.mode, message);

                    /* query - ERROR */
                    if (message.find('E') != std::string::npos) {
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            if (client_sockets[i] == fd) {
                                client_sockets[i] = 0;
                                client_hellos[i] = 0;
                                break;
                            }
                        }
                        send(fd, "BYE\n", 4, 0);
                        close(fd);
                        FD_CLR(fd, &read_fds);
                        break;
                    }
                    /* query - VALID */
                    else {
                        bzero(buf, BUFSIZE);
                        strcpy(buf, "RESULT ");
                        strcpy(buf+7, message.c_str());
                        buf[strlen(buf)] = '\n';
                        buf[strlen(buf)] = '\0';
                        if (send(fd, buf, strlen(buf), 0) < 0) {
                            perror("ERROR: send");
                            exit(EXIT_FAILURE);
                            close(fd);
                            FD_CLR(fd, &read_fds);
                        }
                        bzero(buf, BUFSIZE);
                        continue;
                    }
                }
                /* BYE message or anything else */
                else {
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (client_sockets[i] == fd) {
                            client_sockets[i] = 0;
                            client_hellos[i] = 0;
                            break;
                        }
                    }
                    send(fd, "BYE\n", 4, 0);
                    close(fd);
                    FD_CLR(fd, &read_fds);
                    break;
                }
            }
        }
    }

    /* Final cleanup */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            send(client_sockets[i], "BYE\n", 4, 0);
            close(client_sockets[i]);
        }
    }

    close(server_socket);

    return "hehe";
}
