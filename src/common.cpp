//
// Created by jan on 6/7/24.
//

#include <common.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "err.h"
#include <iostream>
#define BAD_DESCRIPTOR (-1)

uint16_t read_port(char const *string) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }
    return (uint16_t) port;
}

// Following two functions come from Stevens' "UNIX Network Programming" book.
// Read n bytes from a descriptor. Use in place of read() when fd is a stream socket.
ssize_t readn(int fd, void *vptr, size_t n) {
    ssize_t nleft, nread;
    char *ptr;

    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0)
            return nread;     // When error, return < 0.
        else if (nread == 0)
            break;            // EOF

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft;         // return >= 0
}

std::string readall(int fd) { //TODO maybe should wait until delimiter \n\r
    const size_t read_chunks = 128;
    char buffer[read_chunks];
    std::string result;
    ssize_t nread;
    while ((nread = read(fd, buffer, read_chunks) ) > 0) {
        result+=std::string(buffer, nread);
    }
    if(nread < 0 && errno != EAGAIN){
        syserr("readall"); //TODO samething to do here
    }
    return result;
}

// Write n bytes to a descriptor.
ssize_t writen(int fd, const void *vptr, size_t n) {
    ssize_t nleft, nwritten;
    const char *ptr;

    ptr = (char*)vptr;               // Can't do pointer arithmetic on void*.
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
            return nwritten;  // error

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}
#define MAX_MESSAGE_SIZE 256




int send_IAM(int fd, const char *seat) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[6] = {'I', 'A', 'M', *seat, '\r', '\n'};
    return writen(fd, buffer, 6);
}

int send_TOTAL(int fd, int score1, int score2, int score3, int score4) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[MAX_MESSAGE_SIZE];
    snprintf(buffer, MAX_MESSAGE_SIZE, "TOTALN%dE%dS%dW%d\r\n", score1, score2, score3, score4);
    return writen(fd, buffer, strlen(buffer));
}

int send_SCORE(int fd, int score1, int score2, int score3, int score4) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[MAX_MESSAGE_SIZE];
    snprintf(buffer, MAX_MESSAGE_SIZE, "SCOREN%dE%dS%dW%d\r\n", score1, score2, score3, score4);
    return writen(fd, buffer, strlen(buffer));
}

int send_TAKEN(int fd, int trick_number, const char *cards, const char *winner_seat) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[MAX_MESSAGE_SIZE];
    snprintf(buffer, MAX_MESSAGE_SIZE, "TAKEN%d%s%c\r\n", trick_number, cards, *winner_seat);
    return writen(fd, buffer, strlen(buffer));
}

int send_WRONG(int fd, int trick_number) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[64];
    snprintf(buffer, 64, "WRONG%d\r\n", trick_number);
    return writen(fd, buffer, strlen(buffer));
}

int send_TRICK(int fd, int trick_number, const char *cards) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[MAX_MESSAGE_SIZE];
    snprintf(buffer, MAX_MESSAGE_SIZE, "TRICK%d%s\r\n", trick_number, cards);
    return writen(fd, buffer, strlen(buffer));
}

int send_DEAL(int fd, int deal_type, const char first_seat, const std::string &cards) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[MAX_MESSAGE_SIZE];
    snprintf(buffer, MAX_MESSAGE_SIZE, "DEAL%d%c%s\r\n", deal_type, first_seat, cards.c_str());
    return writen(fd, buffer, strlen(buffer));
}

int send_BUSY(int fd, const char *busy_seats) {
    if(fd<1) return BAD_DESCRIPTOR;
    char buffer[64];
    snprintf(buffer, strlen(busy_seats)+6, "BUSY%s\r\n", busy_seats);
    return writen(fd, buffer, strlen(busy_seats)+6);
}

PlayersSides fromChar(char c) {
    if(c=='N')return N;
    if(c=='E')return E;
    if(c=='S')return S;
    if(c=='W')return W;
    throw std::invalid_argument("Invalid seat");
}


