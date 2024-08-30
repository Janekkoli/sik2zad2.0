//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_PLAYER_H
#define ZAD2_SIK_PLAYER_H
#include "Game.h"
class Player {
private:

    PlayersSides seat=N;
    int points=0;
public:
    std::vector<Card> cards;
    int fd = -1;
    Player(PlayersSides seat);

    Player()= default;



    void setCards(const std::vector<Card>& cards);

    int getPoints();

    bool canPlayThisCard(const Card card, const char trickColor);

    //old design
    bool playCard(Card card);

    void wrongCard(Card card);

    int numberOfCards(char color);

    void receivePoints(int p);

    std::string cardsToString();

    char getSeat() const;
};


#endif //ZAD2_SIK_PLAYER_H
