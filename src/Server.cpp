#include "Server.h"
#include <chrono>
#include <iostream>
#include "err.h"

Server::Server(const uint16_t port, const std::string &fileName, const time_t socketTimeOut) {
    this->port = port;
    this->fileName = fileName;
    this->socketTimeOut = socketTimeOut;
    readPlayFile();
    //install_signal_handler(SIGINT, catch_int, SA_RESTART);
}

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

                ssize_t received_bytes = read(poll_descriptors[i].fd, buffer, BUFFER_SIZE);
                //makeReport(server_ip,server_port, connectionsDesc[i].client_ip, connectionsDesc[i].client_port, std::string(buffer,received_bytes));
                reportF(poll_descriptors[i].fd, std::string(buffer, received_bytes),true);
                //TODO add correct server data
                std::string message = connectionsDesc[i].message_buffor.addToBuffor(buffer,received_bytes);
                if (received_bytes < 0) {
                    error("error when reading message from connection %d", i);
                    closeConnection(i);
                } else if (received_bytes == 0) {
                    closeConnection(i);
                } else {
                    //recived message
                    handleMessage(i, message);
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
    int client_fd = accept(poll_descriptors[0].fd, reinterpret_cast<struct sockaddr *>(&client_address), &sizeAdr);
    char *client_ip = inet_ntoa(client_address.sin_addr);
    uint16_t client_port = ntohs(client_address.sin_port);
    //printf("accepted connection from %s:%" PRIu16 "\n", client_ip, client_port);
    if (client_fd < 0) {
        syserr("accept");
    }

    // Searching for a free slot.
    bool accepted = false;
    for (int i = 1; i < CONNECTIONS_MAX; i++) {
        if (poll_descriptors[i].fd == -1) {
            //printf("received new connection (%d)\n", i);
            poll_descriptors[i].fd = client_fd;
            poll_descriptors[i].events = POLLIN;
            connectionsDesc[i].connTime = currentTime();
            connectionsDesc[i].player = None; // no player yet
            connectionsDesc[i].client_ip=client_ip; //setting client ip
            connectionsDesc[i].client_port=client_port; //setting client port
            accepted = true;
            break;
        }
    }
    if (!accepted) {
        close(client_fd);
        printf("too many clients\n");
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
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);
    server_ip = inet_ntoa(server_address.sin_addr);
    //std::cout<<"Listening: "<< server_ip<< " "<< port<<"\n";
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
            all_games.lastGame().setCards(side, line);
        }
    }

    file.close();
}

long Server::currentTime() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                duration).count();
    return milliseconds;
}

void Server::closeConnection(const int i) {
    //std::cout<<"closing connection: "<<i<<std::endl;
    if(connectionsDesc[i].player!=None) { playersConnectionsNumbers[connectionsDesc[i].player]=0; all_games.players[connectionsDesc[i].player].fd = 0; } //delete player from active players
    connectionsDesc[i].isDealSend = false;
    connectionsDesc[i].player = None;
    close(poll_descriptors[i].fd);
    poll_descriptors[i].fd = -1;


}

void Server::handleMessage(const int connNumber, const std::string &messageOriginal) {
    //std::cout<<"Message: "<<messageOriginal<<"from connection: "<<connNumber<<std::endl;
    std::string message = messageOriginal;
    size_t pos = 0;
    std::string token;
    std::string delimiter = DELIMITER;
    while ((pos = message.find(DELIMITER)) != std::string::npos) {
        token = message.substr(0, pos);
        message.erase(0, pos + delimiter.length());
        if(token.substr(0, 3) == "IAM" && connectionsDesc[connNumber].player==None) dealwith_IAM(connNumber, token.substr(3)); //only client without player can send IAM...
        else if(connectionsDesc[connNumber].player == None) {closeConnection(connNumber);} // close connection if client without player sends sth else than IAM
        else if(token.substr(0, 5) == "TRICK") dealwith_TRICK(token.substr(5), connectionsDesc[connNumber].player, connNumber);
    }
}

void Server::dealwith_TRICK(const std::string &content, const PlayersSides side, const int connNumber) {
    int trickNumber;
    std::string cardString;
    split_tricknum_rest(content, trickNumber, cardString);
    //std::cout<<"Trick message received: "<<trickNumber<<" "<<cardString<<std::endl;
    if(trickNumber>13 || trickNumber<1) return;
    Card card;
    try{ card = Card(cardString);} catch(std::exception& e) {
        send_WRONG(poll_descriptors[connNumber].fd, trickNumber,true);return;}
    if(!all_games.playCard(side, card, trickNumber )) {
        send_WRONG(poll_descriptors[connNumber].fd, trickNumber,true);return;
    }
    //card was correctly played
    if(all_games.currentGame().finished) //new game is coming so i need to send total scores to players
    {
        all_games.sendTotalMess();
        if(all_games.games.size()==1) {finish = true; return;}
        startNewGame(); //here i start new game if game ends
    }
    else {
        sendTrickMess(); //so im sending message to next person
    }
}

void Server::dealwith_IAM(int connNumber, std::string content) {
    //std::cout<<"IAM: "<<content<<" number: "<<connNumber<<std::endl;
    PlayersSides seat;
    try {seat = fromChar(content[0]);} catch (std::invalid_argument &e) {closeConnection(connNumber);return;} // invalid seat
    if(playersConnectionsNumbers[seat]!=0) { //there is already active player i have to send busy
        std::string busySeats;
        for(PlayersSides s: {N,E,S,W}) {
            if(playersConnectionsNumbers[s]!=0) busySeats+=static_cast<char>(s);
        }
        send_BUSY(poll_descriptors[connNumber].fd, busySeats,true);
        closeConnection(connNumber);
        return;
    }
    all_games.players[seat].fd = poll_descriptors[connNumber].fd; //players know his fd number
    playersConnectionsNumbers[seat] = connNumber;
    connectionsDesc[connNumber].player = seat;
    checkPlayersReadyAndSendThemDeals();
}

void Server::checkPlayersReadyAndSendThemDeals() {
    if(!areAllClientsConnected()) return;
    for(PlayersSides s: {N,E,S,W}) {
        if(connectionsDesc[playersConnectionsNumbers[s]].isDealSend) continue;
        connectionsDesc[playersConnectionsNumbers[s]].isDealSend = true;
        all_games.sendDealMess(s, poll_descriptors[playersConnectionsNumbers[s]].fd);
    }
    sendTrickMess();  //send Trick mess again when new player reconnects ???? looks like it makes sense
}

void Server::sendTrickMess() {
    if(!areAllClientsConnected()) return;
    repeatingCurrentTrickTime = currentTime();
    all_games.sendTrickMess();
}

void Server::startNewGame() {
    repeatingCurrentTrickTime = 0;
    for(PlayersSides s: {N,E,S,W})
    {connectionsDesc[playersConnectionsNumbers[s]].isDealSend= false; } //has to send new deals
    all_games.startNewGame();
    checkPlayersReadyAndSendThemDeals(); //if starting new game sending deals can be necessary
}

void Server::checkIfSendTrickMess() {
    if(repeatingCurrentTrickTime == 0) return;
    if(currentTime()-repeatingCurrentTrickTime > 1000*socketTimeOut) {
        sendTrickMess();
    }

}

bool Server::areAllClientsConnected() {
    for(PlayersSides s: {N,E,S,W}) {
        if(playersConnectionsNumbers[s]==0) return false;
    }
    return true;
}
