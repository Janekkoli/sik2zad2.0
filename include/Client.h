//
// Created by jan on 8/28/24.
//
#pragma once
#include <bits/socket.h>
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
#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    Client(const int opt, std::string host, const int port, const int ai_family, const bool automatic_player, const char seatChar): opt(opt), host(host),
                port(port), ai_family(ai_family), automaticPlayer(automatic_player) {
        this->seat = fromChar(seatChar);}

    void run() {
        makeConnection();
        char seatChar = toChar(seat);
        send_IAM(socket_fd, &seatChar, automaticPlayer);
        if (automaticPlayer) {
            //if player is automatic everything is easier
            automatic();
            close(socket_fd);

            return;
        }

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
                message = readall(STDIN_FILENO);
                handleStdin(message);
            }
            if (fds[1].revents & POLLIN) {
                if ((message = readall(socket_fd)).empty()) handleDisconnect();
                split_and_execute(message);
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
    int waiting_for_a_card = -1;
    All_Games all_games = All_Games();
    char const *server_ip;
    uint16_t server_port;

    char const *client_ip;
    uint16_t client_port;

    void split_and_execute(std::string &message) {
        //std::cout<<"MESSAGE RECIVED: "<<message<<"\n";
        std::string delimiter = DELIMITER;
        size_t pos = 0;
        std::string token;
        while ((pos = message.find(delimiter)) != std::string::npos) {
            token = message.substr(0, pos);
            message.erase(0, pos + delimiter.length());
            if (token.substr(0, 5) == "TOTAL") dealwith_TOTAL(token.substr(5));
            else if (token.substr(0, 5) == "SCORE") dealwith_SCORE(token.substr(5));
            else if (token.substr(0, 5) == "TAKEN") dealwith_TAKEN(token.substr(5));
            else if (token.substr(0, 5) == "WRONG") dealwith_WRONG(token.substr(5));
            else if (token.substr(0, 5) == "TRICK") dealwith_TRICK(token.substr(5));
            else if (token.substr(0, 4) == "DEAL") dealWith_DEAL(token.substr(4));
            else if (token.substr(0, 4) == "BUSY") dealWith_BUSY(token.substr(4));
            //std::cout<<"NOwertogo\n";
        }
        //std::cout<<"Xdddtu\n";
    }

    void automatic() {
        //Automatic player will not handle stdin
        //std::cout << "running automatic player XD\n";
        std::string message;
        while (true) {
            //std::cout<<"start\n";
            if ((message = readallBlocking(socket_fd)).empty()) handleDisconnect();
            reportF(socket_fd, message, true);

            //std::cout<<message<<"\n";
            split_and_execute(message);
        }
    }

    void handleDisconnect() {
        if(all_games.currentGame().currentTrickNum == 13 && all_games.currentGame().tricks[13].points()!=-1)
        {close(socket_fd);exit(EXIT_SUCCESS);}
        syserr("disconnected from server");
    }

    void handleStdin(const std::string &message) {
        Card c;

        if (message == "cards\n") { std::cout << "Twoje karty na reku: " << all_games.players[seat].cardsToString() << "\n"; }
        else if (message == "tricks\n") { trics_print(); } else if (message[0] == '!') {
            try { c = Card(message.substr(1, message.size() - 2)); } catch (std::invalid_argument &e) { std::cout << "Invalid card code\n"; }
            play_card(c, waiting_for_a_card);
        } else { std::cout << "Invalid command\n"; }
    }

    void makeConnection() {
        socket_fd = socket(ai_family, SOCK_STREAM, 0);
        if (socket_fd < 0) { syserr("cannot create a socket"); }
        struct sockaddr_in server_address = get_server_address(host.c_str(), port);
        server_ip = inet_ntoa(server_address.sin_addr);
        server_port = ntohs(server_address.sin_port);
        //std::cout<<"Connected to server: "<<server_ip<<":"<<server_port<<"\n";
        if (connect(socket_fd, (struct sockaddr *)&server_address, (socklen_t)sizeof(server_address)) < 0) {
            syserr("cannot connect to the server");
        }
        if(!automaticPlayer) {
            fcntl(socket_fd, F_SETFL, O_NONBLOCK);
            fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        }
    }

    sockaddr_in get_server_address(char const *host, const uint16_t port) {
        addrinfo hints;
        memset(&hints, 0, sizeof(addrinfo));
        hints.ai_family = AF_INET; // IPv4
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo *address_result;
        int errcode = getaddrinfo(host, NULL, &hints, &address_result);
        if (errcode != 0) {
            fatal("getaddrinfo: %s", gai_strerror(errcode));
        }

        sockaddr_in send_address;
        send_address.sin_family = AF_INET; // IPv4
        send_address.sin_addr.s_addr = // IP address
                ((sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr;
        send_address.sin_port = htons(port); // port from the command line

        freeaddrinfo(address_result);

        return send_address;
    }

    void trics_print() {
        Game &g = all_games.currentGame();
        std::cout << "Tricks won by me: \n";
        int n = 0;
        for (int i = 1; i < 14; i++) {
            if (g.tricks[i].getWinner() == seat && g.tricks[i].points()!=-1) {
                std::cout << "Trick number: " << i << ", cards " << g.tricks[i].cardsToString() << "\n";
                n++;
            }
        }
        if (n == 0)std::cout << "None :(\n";
    }

    void dealWith_BUSY(const std::string content) //save for automatic
    {
        std::cout << "Place busy, list of busy places received: " << content << "\n";
        exit(EXIT_SUCCESS); //TODO NOt sure
    }

    void dealWith_DEAL(const std::string content) //safe with automatic player
    {
        int deal_type;
        char firstSeatChar;
        PlayersSides firstSeat;
        char cardString[256];
        if (sscanf(content.c_str(), "%d%c%s", &deal_type, &firstSeatChar, cardString) != 3) return;

        std::vector<Card> karty;
        karty = cardStringToArray2(cardString);
        if (karty.size() != TRICK_NUMBER) return; //wrong deal was send
        try { firstSeat = fromChar(firstSeatChar); } catch (std::invalid_argument &e) { return; }
        if (deal_type > 7 || deal_type < 1) return;
        all_games.games.pop_front(); //has to pop last game
        all_games.AddNewGame(Game(deal_type, firstSeat));
        //all_games.startNewGame();
        all_games.players[seat].setCards(karty);
        if (!automaticPlayer) { std::cout << "New deal " << deal_type << ": starting place " << firstSeatChar << ", your cards: " << all_games.players[seat].cardsToString() << ".\n"; }
    }

    void automaticPlayCard(int trickNum) {
        Trick& trick = all_games.currentGame().tricks[trickNum];
        //std::cout<<"My cards: "<<all_games.players[seat].cardsToString()<<"trick: "<<trick.cardsToString()<<"trick color: "<<trick.trickColor()<<"\n";
        for(Card c: all_games.players[seat].cards) {
            //std::cout<<"card: "<<c.toString()<<"\n";
            if(all_games.players[seat].canPlayThisCard(c, trick.trickColor())) {
                //std::cout<<"Automatic card chose: "<< trick.cardsToString()<< " i want to play: "<< c.toString()<< "\n";
                play_card(c,trickNum);
                break;
            }
        }
    }

    bool dealwith_TRICK(const std::string content) {
        int trick_number;
        std::string cardString;
        split_tricknum_rest(content, trick_number, cardString);
        waiting_for_a_card = trick_number;
        std::vector<Card> cards = cardStringToArray2(cardString);
        int n = cards.size();
        //std::cout<<n<<" "<<seat<<"\n";
        Trick trick = Trick(seat - n, trick_number, all_games.currentGame().gameType);
        trick.addCards(cards);
        //trick.print();
        all_games.currentGame().tricks[trick_number]=trick;
        if(!automaticPlayer) {
            std::cout << "Trick: (" << trick_number << ") " << trick.cardsToString() << std::endl;
            std::cout << "Available: " << all_games.players[seat].cardsToString() << std::endl;
            std::cout << "Choose a card to play (write !<card code><enter> for example !2D<enter> : \n";
        }
        else{
            automaticPlayCard(trick_number);
        }
        //all_games.currentGame()->addTrick()
        return true;
    }

    bool dealwith_TAKEN(const std::string content) //safe for automatic
    {
        int trick_number;
        std::string cardString;
        char winner_seat;
        // std::cout << "TAKEN " << content << "\n";
        split_tricknum_rest(content, trick_number, cardString);
        winner_seat = content[content.size() - 1];
        cardString = cardString.substr(0, cardString.size() - 1);
        //DEBUG_COUT( "TAKEN " << trick_number << " " << cardString << " " << winner_seat << std::endl)
        Trick trick = Trick(all_games.currentGame().tricks[trick_number-1].getWinner(), trick_number, all_games.currentGame().gameType); //trick first player is winner of last trick
        std::vector<Card> cards = cardStringToArray2(cardString);
        if(cards.size()!=4) return false; //wrong taken message
        //std::cout<<"dealwithTaken: ";
        //for(Card c:cards) {
        //    std::cout<<c.toString()<<" ";
        //}
        //std::cout<<cardString<<"\n";
        trick.addCards(cards);
        trick.setWinner(fromChar(winner_seat));
        all_games.currentGame().tricks[trick_number] = trick;
        if(!automaticPlayer) std::cout << "A trick " << trick_number << " is taken by " << winner_seat << ", cards " << trick.cardsToString() << "\n";
        all_games.players[seat].playCard(trick.cards[seat]); //makes sure that card is played
        all_games.currentGame().currentTrickNum = trick_number;
        return true;
    }

    bool dealwith_WRONG(std::string content) {
        if (automaticPlayer) {syserr("Wrong card played very bad :(\n");return true;} //this shouldnt happend
        int trick_number;
        if (sscanf(content.c_str(), "%d", &trick_number) != 1) return false;
        all_games.players[seat].wrongCard(all_games.currentGame().tricks[trick_number].getCards()[seat]);
        all_games.currentGame().tricks[trick_number].wrongCard();
        std::cout << "Wrong message received in trick " << trick_number << ".\n";
        std::cout << "Try again trick: (" << trick_number << ") " << all_games.currentGame().tricks[trick_number].cardsToString() << std::endl;

        std::cout << "Available: " << all_games.players[seat].cardsToString() << std::endl;
        std::cout << "Choose a card to play (write !<card code><enter> for example !2D<enter> : \n";
        waiting_for_a_card = trick_number;

        return true;
    }

    bool dealwith_SCORE(std::string content) {
        if (automaticPlayer) return true;
        int score1, score2, score3, score4;
        char place1, place2, place3, place4;
        sscanf(content.c_str(), "%c%d%c%d%c%d%c%d",&place1, &score1,&place2, &score2,&place3, &score3,&place4, &score4);
        std::cout<<"The scores are:\n";
        std::cout<<place1<<" | "<<score1<<"\n";
        std::cout<<place2<<" | "<<score2<<"\n";
        std::cout<<place3<<" | "<<score3<<"\n";
        std::cout<<place4<<" | "<<score4<<"\n";
        return true;
    }

    bool dealwith_TOTAL(std::string content) {
        if (automaticPlayer) return true;
        int score1, score2, score3, score4;
        char place1, place2, place3, place4;
        sscanf(content.c_str(), "%c%d%c%d%c%d%c%d",&place1, &score1,&place2, &score2,&place3, &score3,&place4, &score4);
        std::cout<<"The total scores are:\n";
        std::cout<<place1<<" | "<<score1<<"\n";
        std::cout<<place2<<" | "<<score2<<"\n";
        std::cout<<place3<<" | "<<score3<<"\n";
        std::cout<<place4<<" | "<<score4<<"\n";
        return true;
    }


    bool play_card(Card card, int trickNumber) {
        if (waiting_for_a_card == -1) {
            std::cout << "Not waiting for a card\n";
            return false;
        }
        if (!all_games.players[seat].playCard(card)) {
            std::cout << "You dont have this card\n";
            return false;
        }
        all_games.currentGame().tricks[trickNumber].addCard(card);
        send_TRICK(socket_fd, waiting_for_a_card, card.toString().c_str(), automaticPlayer);
        waiting_for_a_card = -1;
        return true;
    }
};
#endif //CLIENT_H
