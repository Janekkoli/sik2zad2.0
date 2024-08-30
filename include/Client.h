//
// Created by jan on 8/28/24.
//
#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cinttypes>
#include <netdb.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <arpa/inet.h>
#include "Allgames.h"
#include "common.h"
#include "err.h"
#include <stdexcept>
#include <iostream>
#include <sys/poll.h>
#include <sys/poll.h>
#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    Client(const int opt, std::string host, const int port, const int ai_family, const bool automatic_player, const char seatChar);

    void run();

private:
    int opt;
    std::string host;
    int port = 0;
    int ai_family = AF_UNSPEC;
    bool automaticPlayer = false;
    PlayersSides seat;
    int socket_fd = 0;
    pollfd fds[2];
    int waiting_for_a_card = -1;
    All_Games all_games = All_Games();
    char const *server_ip;
    uint16_t server_port;

    char const *client_ip;
    uint16_t client_port;

    void split_and_execute(std::string &message);

    void automatic();

    void handleDisconnect();

    void handleStdin(const std::string &message);

    void makeConnection();

    sockaddr_in get_server_address(char const *host, const uint16_t port);

    void trics_print();

    void dealWith_BUSY(const std::string content) //save for automatic
    ;

    void dealWith_DEAL(const std::string content) //safe with automatic player
    ;

    void automaticPlayCard(int trickNum);

    bool dealwith_TRICK(const std::string content);

    bool dealwith_TAKEN(const std::string content) //safe for automatic
    ;

    bool dealwith_WRONG(std::string content);

    bool dealwith_SCORE(std::string content);

    bool dealwith_TOTAL(std::string content);


    bool play_card(Card card, int trickNumber);
};
#endif //CLIENT_H
