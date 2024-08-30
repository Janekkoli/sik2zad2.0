//
// Created by jan on 6/7/24.
//

#pragma once
#ifndef ZAD2_SIK_COMMON_H
#define ZAD2_SIK_COMMON_H
#include <string>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <stdexcept>
#include "Card.h"
#include <unistd.h>
#include <vector>
#include <chrono>
#include <iomanip>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "err.h"
#define TRICK_NUMBER (13)
#define DELIMITER "\r\n"
#define BUFFER_SIZE_READALL (128)
#define DEBUG_COUT(X) if(DEBUG_ON){std::cout<<X;}
#define DEBUG_ON true
//TODO read whole line not just what comes use /r/n as separator

class Card; //class card for functions definitions :)
uint16_t read_port(char const *string);
ssize_t	readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const char *vptr, size_t n, bool report);
std::string readall(int fd);
int send_IAM(int fd, const char *seat,bool report);

int send_BUSY(int fd, const std::string& busy_seats,bool report);

int send_DEAL(int fd, int deal_type, char first_seat, const std::string &cards,bool report);

int send_TRICK(int fd, int trick_number, const char *cards,bool report);

int send_WRONG(int fd, int trick_number,bool report);

int send_TAKEN(int fd, int trick_number, const char *cards, const char *winner_seat,bool report);

int send_SCORE(int fd, int score1, int score2, int score3, int score4,bool report);

int send_TOTAL(int fd, int score1, int score2, int score3, int score4,bool report);

enum PlayersSides {N='N', E='E', S='S', W='W', None = 'X'};

inline std::string readallBlocking(int fd) {
    char buffer[BUFFER_SIZE_READALL];
    std::string results;
    int n = 0;
    n = read(fd, buffer, BUFFER_SIZE_READALL);
    results += std::string(buffer,n);
    return results;
}

// inline bool operator==(PlayersSides &lhs, PlayersSides &rhs) {
//     return static_cast<char>(lhs) == static_cast<char>(rhs);
// }
// inline bool operator!=(PlayersSides &lhs, PlayersSides &rhs) {
//     return static_cast<char>(lhs) != static_cast<char>(rhs);
// }

inline std::ostream& operator<<(std::ostream& os, PlayersSides side) {
    switch (side) {
        case N: os << 'N'; break;
        case E: os << 'E'; break;
        case S: os << 'S'; break;
        case W: os << 'W'; break;
        default: os << 'X'; break; // Handle None or any other invalid value
    }
    return os;
}

inline char toChar(const PlayersSides& side) {
    switch (side) {
        case N: return 'N';
        case E: return 'E';
        case W: return 'W';
        case S: return 'S';
        default: return 'X';
    }
}

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
            c = W;
        break;
        case E:
            c = N;
        break;
        case S:
            c = E;
        break;
        case W:
            c = S;
        break;
        default: return None;
    }
    return c;
}
inline PlayersSides operator+(PlayersSides c, int i) {
    for(int i2=1;i2<=i;i2++)++c;
    return c;
}
inline PlayersSides operator-(PlayersSides c, int i) {
    for(int i2=1;i2<=i;i2++)--c;
    return c;
}

PlayersSides fromChar(char c);

extern PlayersSides me;

inline std::vector<Card> cardStringToArray2(const std::string& cardsString)
{
    //std::cout<<cardsString<<"set\n";
    std::string k;
    std::vector<Card> wyn;
    for(auto & c : cardsString){
        k+=c;
        if(c=='C' || c=='D' || c=='H' || c=='S'){
            //std::cout<<k<<"\n";
            try{wyn.emplace_back(k);}
            catch(std::invalid_argument &e){ return {};}
            k = "";
        }
    }
    return wyn;
}

 inline void makeReport(char* sender_ip, uint16_t sender_port, char* receiver_ip, uint16_t receiver_port, std::string message)  //nadawca - odbiorca
 {
     std::cout<<"["<<sender_ip<<":"<<sender_port<<","<<receiver_ip<<":"<<receiver_port<<","<<"time here"<<"] "<<message;
 }

void split_tricknum_rest(const std::string& content, int &trick_number, std::string &cards);

inline std::string timeReport() {
    auto now = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %1000;

    std::stringstream time;
    time << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H:%M:%S");
    time << "." << std::setw(3) << std::setfill('0') << milliseconds.count();
    return time.str();
}

inline void reportF(int sockfd, const std::string &message, bool is_message_recived) {
    sockaddr_in reciver_addr{}, sender_addr{};
    socklen_t reciver_len = sizeof(reciver_addr);
    socklen_t sender_len = sizeof(sender_addr);

    // Get sender info
    if (getsockname(sockfd, (sockaddr *)&reciver_addr, &reciver_len) == -1) {
        syserr("getsockname");
    }

    // Get receiver info
    if (getpeername(sockfd, (sockaddr *)&sender_addr, &sender_len) == -1) {
        syserr("getpeername");
    }

    char server_ip[INET_ADDRSTRLEN];
    char client_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(reciver_addr.sin_addr), server_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(sender_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    int server_port = ntohs(reciver_addr.sin_port);
    int client_port = ntohs(sender_addr.sin_port);
    if(is_message_recived)
        std::cout << "[" << client_ip << ":" << client_port << "," << server_ip << ":" << server_port << "," << timeReport() << "] " << message;
    else
        std::cout << "[" << server_ip << ":" << server_port << "," << client_ip << ":" << client_port << "," << timeReport() << "] " << message;

}



#endif //ZAD2_SIK_COMMON_H
