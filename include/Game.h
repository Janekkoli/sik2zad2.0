#pragma once
#include "Trick.h"
#include <map>
#include <string>
class Game {
public:
    int gameType;
    bool finished=false;
    PlayersSides firstPlayer;
    std::map<PlayersSides,std::string> players_cards;
    Trick tricks[14];
    int currentTrickNum = 1;
    std::map<PlayersSides, int> playersScore;
    Game( int game_type,PlayersSides first_player);

    void addTrick(const Trick& trick);

    bool isGameNew();

    Trick& currentTrick();

    bool playCard(const PlayersSides side, const Card card, const int trickNumber,   std::map<PlayersSides, int>& playersFd);


    void sendDealMess(const PlayersSides side, const int fd);

    void sendTrickMess(std::map<PlayersSides, int>  &playersFd);

    void setCards(const PlayersSides side, const std::string &cards);

    void print();
};