//
// Created by jan on 6/7/24.
//

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "klient_f.h"
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
void print_usage() {
    std::cout << "Usage: program -h <host> -p <port> [-4] [-6] [-N|-E|-S|-W] [-a]" << std::endl;
}

int main(int argc, char *argv[]) {
    int opt;
    std::string host;
    int port = 0;
    int ai_family = AF_UNSPEC;
    bool automatic_player = false;
    char seat = '\0';
    // Parsowanie argumentów za pomocą getopt
    while ((opt = getopt(argc, argv, "h:p:46NESWa")) != -1) {
        switch (opt) {
            case 'h':
                if (host.empty()) {
                    host = optarg;
                }
                break;
            case 'p':
                if (port == 0) {
                    port = std::stoi(optarg);
                }
                break;
            case '4':
                if (ai_family == AF_UNSPEC) {
                    ai_family = AF_INET;
                }
                break;
            case '6':
                if (ai_family == AF_UNSPEC) {
                    ai_family = AF_INET6;
                }
                break;
            case 'N':
            case 'E':
            case 'S':
            case 'W':
                if (seat == '\0') {
                    seat = (char)opt;
                }
                break;
            case 'a':
                if (!automatic_player) {
                    automatic_player = true;
                }
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }
    }

    // Sprawdzenie obowiązkowych argumentów
    if (host.empty() || port == 0 || seat == '\0') {
        print_usage();
        return EXIT_FAILURE;
    }

    // Wyświetlenie wyników
    std::cout << "Host: " << host << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Seat: " << seat << std::endl;
    if (automatic_player) {
        std::cout << "Mode: Automatic Player" << std::endl;
    } else {
        std::cout << "Mode: Intermediary" << std::endl;
    }
    if (ai_family == AF_INET) {
        std::cout << "Using IPv4" << std::endl;
    } else if (ai_family == AF_INET6) {
        std::cout << "Using IPv6" << std::endl;
    } else {
        std::cout << "IP Version: Unspecified (AF_UNSPEC)" << std::endl;
    }

    // conecting via tcp
    int socket_fd = socket(ai_family, SOCK_STREAM, 0);
    if (socket_fd < 0) {syserr("cannot create a socket");}
    struct sockaddr_in server_address = get_server_address(host.c_str(), port);
    if (connect(socket_fd, (struct sockaddr *) &server_address,
                (socklen_t) sizeof(server_address)) < 0) {
        syserr("cannot connect to the server");
    }
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    // start of the game

    me = fromChar(seat);
    send_IAM(socket_fd, &seat);


    struct pollfd fds[2];
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
        if (fds[1].revents & POLLIN) {
            if((message=readall(socket_fd)).empty()) syserr("disconnected from server");
            split_and_execute(message, dealwith_TRICK);
        }
    }


    close(socket_fd);
    return EXIT_SUCCESS;
}
