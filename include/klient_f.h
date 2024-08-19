#ifndef ZAD2_SIK_KLIENT_F_H
#define ZAD2_SIK_KLIENT_F_H
#pragma once
#include <bits/stdc++.h>

#include "common_f.h"
struct sockaddr_in get_server_address(char const *host, uint16_t port);
void trics_print();
bool dealwith_TRICK(std::string content);
bool play_card(int socket_fd, Card card);
#endif //ZAD2_SIK_KLIENT_F_H
