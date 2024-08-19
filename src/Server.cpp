#include "Server.h"
#include <chrono>
#include <iostream>
void Server::print() const {
    std::cout << "Port: " << port << std::endl;
    std::cout << "File: " << fileName << std::endl;
    std::cout << "Timeout: " << socketTimeOut << " seconds" << std::endl;
}

void Server::run() {
    createSocket();
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        syserr("listen");
    }
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLIN;
    for (int i = 1; i < CONNECTIONS_MAX; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN;
    }

    do {
        for (auto &[fd, events, revents] : poll_descriptors) {
            revents = 0;
        }

        // After Ctrl-C the main socket is closed.
        if (finish && poll_descriptors[0].fd >= 0) {
            close(poll_descriptors[0].fd);
            poll_descriptors[0].fd = -1;
        }

        const int poll_status = poll(poll_descriptors, CONNECTIONS_MAX,
                                     static_cast<int>(socketTimeOut)); // checking after timeout if everybody is still connected
        if (poll_status == -1) {
            if (errno == EINTR) {
                printf("interrupted system call\n");
            } else {
                syserr("poll");
            }
        }
        checkTimeouts();
        if (!finish && (poll_descriptors[0].revents & POLLIN)) {
            // New connection: new client is accepted.
            handleNewConnection();
        }
        // Serve data connections.
        for (int i = 1; i < CONNECTIONS_MAX; ++i) {
            if (poll_descriptors[i].fd != -1 &&
                (poll_descriptors[i].revents & (POLLIN | POLLERR))) {

                ssize_t received_bytes =
                        read(poll_descriptors[i].fd, buffer, BUFFER_SIZE);

                if (received_bytes < 0) {
                    error("error when reading message from connection %d", i);
                    closeConnection(i);
                } else if (received_bytes == 0) {
                    printf("ending connection (%d)\n", i);
                    closeConnection(i);
                } else {
                    //recived message
                    handleMessage(i, std::string(buffer, received_bytes));

                }
            }
        }

    } while (!finish);
    if (poll_descriptors[0].fd >= 0) {
        close(poll_descriptors[0].fd);
    }
}

void Server::checkTimeouts() { // check if active connections send right message on time
    for (int i = 1; i < CONNECTIONS_MAX; i++) {
        if (poll_descriptors[i].fd != -1 && currentTime() - connectionsDesc[i].connTime >
            1000 * socketTimeOut && connectionsDesc[i].player == nullptr) {
            printf("ending connection (%d)\n", i);
            close(poll_descriptors[i].fd);
            poll_descriptors[i].fd = -1;
        }
    }
}

void Server::handleNewConnection() {
    struct sockaddr_in client_address{};
    socklen_t sizeAdr = sizeof(client_address);
    const int client_fd = accept(poll_descriptors[0].fd, reinterpret_cast<struct sockaddr *>(&client_address), &sizeAdr);
    if (client_fd < 0) {
        syserr("accept");
    }

    // Searching for a free slot.
    bool accepted = false;
    for (int i = 1; i < CONNECTIONS_MAX; i++) {
        if (poll_descriptors[i].fd == -1) {
            printf("received new connection (%d)\n", i);
            poll_descriptors[i].fd = client_fd;
            poll_descriptors[i].events = POLLIN;
            connectionsDesc[i].connTime = currentTime();
            connectionsDesc[i].player = nullptr; // no player yet
            accepted = true;
            break;
        }
    }
    if (!accepted) {
        close(client_fd);
        printf("too many clients\n");
    } else {
        char const *client_ip = inet_ntoa(client_address.sin_addr);
        const uint16_t client_port = ntohs(client_address.sin_port);
        printf("accepted connection from %s:%" PRIu16 "\n", client_ip,
               client_port);
    }
}

void Server::createSocket() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    // Bind the socket to a concrete address.
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET; // IPv4 how to do it with client side
    server_address.sin_addr.s_addr =
            htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<struct sockaddr *>(&server_address),
             (socklen_t)sizeof server_address) < 0) {
        syserr("bind");
    }
}

void Server::readPlayFile() {
    std::ifstream file;
    file.open(this->fileName);
    std::string line;
    if (!file.is_open()) {
        fatal("Error: Could not open file.");
    }
    while (std::getline(file, line)) {
        all_games.addGame(Game((int)(line[0] - '0'), fromChar(line[1])));
        for (const PlayersSides side : {N, E, S, W}) {
            std::getline(file, line);
            all_games.lastGame()->setCards(side, line);
        }
    }

    file.close();
}

long Server::currentTime() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                duration)
            .count();
    return milliseconds;
}

void Server::closeConnection(const int i) {
    connectionsDesc[i].player = nullptr;
    close(poll_descriptors[i].fd);
    poll_descriptors[i].fd = -1;
}
