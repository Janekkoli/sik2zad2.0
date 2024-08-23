//
// Created by jan on 6/7/24.
//

#pragma once
#ifndef ZAD2_SIK_COMMON_H
#define ZAD2_SIK_COMMON_H
#include <string>
#include <list>
#include <cstddef>
#include <iostream>
#include <sys/types.h>
#include <stdexcept>
#include <map>
#include "err.h"
#include "Card.h"
#include <unistd.h>
#include <vector>

uint16_t read_port(char const *string);
ssize_t	readn(int fd, void *vptr, size_t n);
ssize_t	writen(int fd, const void *vptr, size_t n);
std::string readall(int fd);
int send_IAM(int fd, const char *seat);

int send_BUSY(int fd, const char *busy_seats);

int send_DEAL(int fd, int deal_type, char first_seat, const std::string &cards);

int send_TRICK(int fd, int trick_number, const char *cards);

int send_WRONG(int fd, int trick_number);

int send_TAKEN(int fd, int trick_number, const char *cards, const char *winner_seat);

int send_SCORE(int fd, int score1, int score2, int score3, int score4);

int send_TOTAL(int fd, int score1, int score2, int score3, int score4);

enum PlayersSides {N='N', E='E', S='S', W='W', None = 'X'};

// inline bool operator==(PlayersSides &lhs, PlayersSides &rhs) {
//     return static_cast<char>(lhs) == static_cast<char>(rhs);
// }
// inline bool operator!=(PlayersSides &lhs, PlayersSides &rhs) {
//     return static_cast<char>(lhs) != static_cast<char>(rhs);
// }




inline PlayersSides operator++(PlayersSides &c) {
    switch (c) {
        case N:
            c = E;
            break;
        case E:
            c = S;
            break;
        case S:
            c = W;
            break;
        case W:
            c = N;
            break;
        default: return None;
    }
    return c;
}

inline PlayersSides operator--(PlayersSides &c) {
    switch (c) {
        case N:
            return W;
        case E:
            return N;
        case S:
            return E;
        case W:
            return S;
        default: return None;
    }
}
inline PlayersSides operator+(PlayersSides c, int i) {
    switch (c) {
        case N:
            return static_cast<PlayersSides>(i);
        case E:
            return static_cast<PlayersSides>(i+1);
        case S:
            return static_cast<PlayersSides>(i+2);
        case W:
            return static_cast<PlayersSides>(i+3);
        default: return None;
    }
}
inline PlayersSides operator-(PlayersSides c, int i) {
    switch (c) {
        case N:
            return static_cast<PlayersSides>(i);
        case E:
            return static_cast<PlayersSides>(i-1);
        case S:
            return static_cast<PlayersSides>(i-2);
        case W:
            return static_cast<PlayersSides>(i-3);
        default: return None;
    }
}

PlayersSides fromChar(char c);

extern PlayersSides me;

inline std::vector<Card> cardStringToArray2(const std::string& cardsString)
{
    std::string k;
    std::vector<Card> wyn;
    for(auto & c : cardsString){
        k+=c;
        if(c=='C' || c=='D' || c=='H' || c=='S'){
            try{wyn.emplace_back(k);}
            catch(std::invalid_argument &e){ return {};}
            k = "";
        }
    }
    return {};
}




void split_and_execute(std::string message, bool (*dealwith_TRICK) (std::string));
void split_tricknum_rest(const std::string& content, int &trick_number, std::string &cards);
#endif //ZAD2_SIK_COMMON_H
