//
// Created by jan on 6/8/24.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "klient_f.h"
int waiting_for_a_card = -1;
struct sockaddr_in get_server_address(char const *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0) {
        fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;   // IPv4
    send_address.sin_addr.s_addr =       // IP address
            ((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

void trics_print(){
    Game g = all_games.currentGame();
    std::cout<<"Tricks won by me: \n";
    int n=0;
    for(int i=0;i<14;i++)
    {
        if(g.tricks[i].getWinner()==me)
        {std::cout<<"Trick number: "<<i<<", cards "<<g.tricks[i].cardsToString()<<"\n";n++;}
    }
    if(n==0)std::cout<<"None :(\n";
}

bool dealwith_IAM(std::string content)
{
    if(content.length() != 1) return false;
    std::cout << "IAM " << content << std::endl;
    return true;
}

void dealWith_BUSY(const std::string& content)
{
    std::cout << "Place busy, list of busy places received: " << content << "\n";
}

void dealWith_DEAL(const std::string& content)
{
    int deal_type;
    char first_seat;
    char cards[256];
    if(sscanf(content.c_str(), "%d%c%s", &deal_type, &first_seat, cards) != 3) return;
    std::cout<< "New deal "<< deal_type <<": staring place "<< first_seat <<", your cards: "<< cards << ".\n";
    std::vector<Card> karty;
    karty =cardStringToArray2(cards);
    if(karty.size()!=TRICK_NUMBER) return;
    all_games.players[me].setCards(karty);
    all_games.AddNewGame(Game(deal_type, fromChar(first_seat)));
}

bool dealwith_TRICK(const std::string &content)
{
    int trick_number;
    std::string cards;
    split_tricknum_rest(content, trick_number, cards);
    std::cout << "Trick: ("<<trick_number<< ") " << cards << std::endl;
    std::cout << "Available: " << all_games.players[me].cardsToString() << std::endl;
    std::cout << "Choose a card to play (write !<card code><enter> for example !2D<enter> : \n";
    waiting_for_a_card=trick_number;
    Card karty[4];
    int n = cardStringToArray(cards, karty, 4);
    Trick lewa(me-n, trick_number, 0); //TODO has to add trick type here
    for(int i=0; i<n; i++) lewa.addCard(karty[i]);
    //all_games.currentGame()->addTrick()
    return true;
}
bool dealwith_TAKEN(const std::string &content)
{
    int trick_number;
    std::string cards;
    char winner_seat;
    // std::cout << "TAKEN " << content << "\n";
    split_tricknum_rest(content, trick_number, cards);
    winner_seat = content[content.size()-1];
    cards=cards.substr(0, cards.size()-1);
    std::cout<< "TAKEN " << trick_number << " " << cards << " " << winner_seat << std::endl;
    Trick lewa(N, trick_number, 0 ); //TODO has to add trick type here
    Card karty[4];
    if(4!=cardStringToArray(cards, karty, 4)) return false;
    for(auto i : karty) lewa.addCard(i);
    lewa.setWinner(fromChar(winner_seat));
    all_games.currentGame().addTrick(lewa);
    std::cout<< "A trick " << trick_number << " is taken by " << winner_seat << ", cards " << lewa.cardsToString() << "\n";
    return true;
}

bool dealwith_WRONG(std::string content)
{
    int trick_number;
    if(sscanf(content.c_str(), "%d", &trick_number) != 1) return false;
    std::cout<< "Wrong message received in trick "<<trick_number<<".\n";
    std::cout << "Try again trick: ("<<trick_number<< ") " << all_games.currentGame().tricks[trick_number].cardsToString() << std::endl;
    all_games.players[me].wrongCard(all_games.currentGame().tricks[trick_number].getCards()[me]);
    std::cout << "Available: " << all_games.players[me].cardsToString() << std::endl;
    std::cout << "Choose a card to play (write !<card code><enter> for example !2D<enter> : \n";
    waiting_for_a_card=trick_number;

    return true;
}

bool dealwith_SCORE(std::string content)
{
    int score1, score2, score3, score4;
    if(sscanf(content.c_str(), "%d%d%d%d", &score1, &score2, &score3, &score4) != 4) return false;
    std::cout << "SCORE " << score1 << " " << score2 << " " << score3 << " " << score4 << std::endl;
    return true;
}

bool dealwith_TOTAL(std::string content)
{
    int score1, score2, score3, score4;
    if(sscanf(content.c_str(), "%d%d%d%d", &score1, &score2, &score3, &score4) != 4) return false;
    std::cout << "TOTAL " << score1 << " " << score2 << " " << score3 << " " << score4 << std::endl;
    return true;
}


bool play_card(int socket_fd, Card card){
    if(waiting_for_a_card==-1) {std::cout<<"Not waiting for a card\n"; return false;}
    if(!all_games.players[me].playCard(card)) {std::cout<<"You dont have this card\n"; return false;}
    send_TRICK(socket_fd,waiting_for_a_card,card.toString().c_str());
    waiting_for_a_card = -1;
    return true;
}


void split_and_execute(std::string message, bool (*dealwith_TRICK) (std::string))
{
    std::string delimiter = "\r\n";
    size_t pos = 0;
    std::string token;
    while ((pos = message.find(delimiter)) != std::string::npos) {
        token = message.substr(0, pos);
        message.erase(0, pos + delimiter.length());
        if(token.substr(0, 3) == "IAM") dealwith_IAM(token.substr(3));
        else if(token.substr(0, 5) == "TOTAL") dealwith_TOTAL(token.substr(5));
        else if(token.substr(0, 5) == "SCORE") dealwith_SCORE(token.substr(5));
        else if(token.substr(0, 5) == "TAKEN") dealwith_TAKEN(token.substr(5));
        else if(token.substr(0, 5) == "WRONG") dealwith_WRONG(token.substr(5));
        else if(token.substr(0, 5) == "TRICK") dealwith_TRICK(token.substr(5));
        else if(token.substr(0, 4) == "DEAL") dealWith_DEAL(token.substr(4));
        else if(token.substr(0, 4) == "BUSY") dealWith_BUSY(token.substr(4));
        else continue;
    }
}