#include "Server.h"

void Server::print() const {
    std::cout << "Port: " << port << std::endl;
    std::cout << "File: " << fileName << std::endl;
    std::cout << "Timeout: " << socketTimeOut << " seconds" << std::endl;
}

void Server::checkTimeouts() { // check if active connections send right message on time
    for (int i = 1; i < CONNECTIONS_MAX; i++) {
        if (poll_descriptors[i].fd != -1 && currentTime() - connectionsDesc[i].connTime >
            1000 * socketTimeOut) {
            printf("ending connection (%d)\n", i);
            close(poll_descriptors[i].fd);
            poll_descriptors[i].fd = -1;
        }
    }
}
