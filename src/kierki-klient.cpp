//
// Created by jan on 6/7/24.
//

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include "Client.h"

void print_usage() {
    std::cout << "Usage: program -h <host> -p <port> [-4] [-6] [-N|-E|-S|-W] [-a]" << std::endl;
}


//TODO ipv6 not working !!!!
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

    auto client = Client(opt, host, port, ai_family, automatic_player, seat);
    client.run();

    // conecting via tcp

    // start of the game





    return EXIT_SUCCESS;
}
