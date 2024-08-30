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

std::string readallBlocking(int fd);

std::ostream& operator<<(std::ostream& os, PlayersSides side);

char toChar(const PlayersSides& side);

PlayersSides operator--(PlayersSides &c);

PlayersSides operator++(PlayersSides &c);

PlayersSides operator+(PlayersSides c, int i);

PlayersSides operator-(PlayersSides c, int i);

PlayersSides fromChar(char c);

extern PlayersSides me;

std::vector<Card> cardStringToArray2(const std::string& cardsString);

void makeReport(char* sender_ip, uint16_t sender_port, char* receiver_ip, uint16_t receiver_port, std::string message);

void split_tricknum_rest(const std::string& content, int &trick_number, std::string &cards);

std::string timeReport();

void reportF(int sockfd, const std::string &message, bool is_message_recived);


#endif //ZAD2_SIK_COMMON_H
