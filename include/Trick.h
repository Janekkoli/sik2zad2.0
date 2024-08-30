//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_TRICK_H
#define ZAD2_SIK_TRICK_H
#include <bits/stdc++.h>
#include "Card.h"
class Trick {
    PlayersSides firstPlayer = None;
    int trickNumber = -1;
    PlayersSides winner = N;
    int trickType = -1;
public:
    std::map<PlayersSides,Card> cards;

    PlayersSides currentPlayer = None;

    Trick(PlayersSides first_player, int trickNumber, int trickType);

    Trick()= default;

    bool isTrickNew();

    void sendTakenMess(const int fd);

    void establishWinner();

    bool playCard(const PlayersSides side, const Card& card);

    void wrongCard();

    std::map<PlayersSides, Card>& getCards();

    void setWinner(const PlayersSides winner);

    Card& getFirstCardPlayed();

    void sendTrickMess(const int fd);

    PlayersSides& getWinner();

    std::string cardsToString();

    std::string cardsToShortString();

    [[nodiscard]] int getTrickNumber() const;

    char trickColor();

    int points();

    int points(int t);

    void addCard(const Card card);

    void addCards(const std::vector<Card>& cards);

    void print();
};


#endif //ZAD2_SIK_TRICK_H
