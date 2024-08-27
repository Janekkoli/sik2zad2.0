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
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {  //TODO is it really neccessary?
        syserr("listen");
    }
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLIN;
    for (int i = 1; i < CONNECTIONS_MAX; ++i) {
        poll_descriptors[i].fd = -1;
        poll_descriptors[i].events = POLLIN;
    }
    startNewGame(); //start first game
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
        checkIfSendTrickMess();  //check if current trick should be sended again
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
        if (poll_descriptors[i].fd != -1 && (currentTime() - connectionsDesc[i].connTime > 1000 * socketTimeOut) && connectionsDesc[i].player == None) {
            closeConnection(i);
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
            connectionsDesc[i].player = None; // no player yet
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
    std::cout<<"closing connection: "<<i<<std::endl;
    if(connectionsDesc[i].player!=None) { playersConnectionsNumbers[connectionsDesc[i].player]=0; all_games.players[connectionsDesc[i].player].fd = 0; } //delete player from active players
    connectionsDesc[i].isDealSend = false;
    connectionsDesc[i].player = None;
    close(poll_descriptors[i].fd);
    poll_descriptors[i].fd = -1;


}

void Server::dealwith_IAM(int connNumber, std::string content) {
    std::cout<<"IAM: "<<content<<" number: "<<connNumber<<std::endl;
    PlayersSides seat;
    try {seat = fromChar(content[0]);} catch (std::invalid_argument &e) {closeConnection(connNumber);return;} // invalid seat
    if(playersConnectionsNumbers[seat]!=0) { //there is already active player i have to send busy
        std::string busySeats;
        for(PlayersSides s: {N,E,S,W}) {
            if(playersConnectionsNumbers[s]!=0) busySeats+=static_cast<char>(s);
        }
        send_BUSY(poll_descriptors[connNumber].fd, busySeats.c_str());
        closeConnection(connNumber);
        return;
    }
    all_games.players[seat].fd = poll_descriptors[connNumber].fd; //players know his fd number
    playersConnectionsNumbers[seat] = connNumber;
    connectionsDesc[connNumber].player = seat;
    checkPlayersReadyAndSendThemDeals();
}

void Server::checkIfSendTrickMess() {
    if(repeatingCurrentTrickTime == 0) return;
    if(currentTime()-repeatingCurrentTrickTime > 1000*socketTimeOut) {
        sendTrickMess();
    }

}
