//
// Created by jan on 8/28/24.
//
#pragma once
#include <bits/socket.h>

#include "Allgames.h"
#ifndef CLIENT_H
#define CLIENT_H
class Client {
public:

    Client(const int opt, std::string host, const int port, const int ai_family, const bool automatic_player, const char seat): opt(opt),host(host), port(port), ai_family(ai_family), automaticPlayer(automatic_player) {
      this->seat = fromChar(seat);
    }

    void run() {
        makeConnection();
        send_IAM(socket_fd, reinterpret_cast<const char *>(static_cast<char>(seat)));

        //making poll to be able to concurrently take messages from stdin and tcp connection

        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;
        fds[1].fd = socket_fd;
        fds[1].events = POLLIN;
        std::string message;
        while (true) {
            int ret = poll(fds, 2, -1);
            if (ret < 0) {
                syserr("poll");
            }
            if (fds[0].revents & POLLIN) {
                message=readall(STDIN_FILENO);
                handleStdin(message);
            }
            if (fds[1].revents & POLLIN) {
                if((message=readall(socket_fd)).empty()) syserr("disconnected from server");
                //split_and_execute(message, dealwith_TRICK);
                //TODO dont know what is going on here
            }
        }


        close(socket_fd);
    }
private:
    int opt;
    std::string host;
    int port = 0;
    int ai_family = AF_UNSPEC;
    bool automaticPlayer = false;
    PlayersSides seat;
    int socket_fd = 0;
    pollfd fds[2];

    void handleStdin(const std::string &message) {
        Card c;

        if(message=="cards\n"){std::cout<<"Twoje karty na reku: "<<all_games.players[me].cardsToString()<<"\n";}
        else if(message=="tricks\n"){trics_print();}
        else if(message[0]=='!'){
            try{c  = Card(message.substr(1, message.size()-2));}
            catch(std::invalid_argument &e){std::cout<<"Invalid card code\n";}
            play_card(socket_fd, c);
        }
        else {std::cout<<"Invalid command\n";}
    }

    void makeConnection() {
        socket_fd = socket(ai_family, SOCK_STREAM, 0);
        if (socket_fd < 0) {syserr("cannot create a socket");}
        sockaddr_in server_address = get_server_address(host.c_str(), port);
        if (connect(socket_fd, reinterpret_cast<sockaddr *>(&server_address),
                     sizeof(server_address)) < 0) {
            syserr("cannot connect to the server");}
        fcntl(socket_fd, F_SETFL, O_NONBLOCK);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    }
};
#endif //CLIENT_H
