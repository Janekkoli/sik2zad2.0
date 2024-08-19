#pragma once
#include "Trick.h"
#include <bits/stdc++.h>
#include <map>
#include <string>
class Game {
public:
    int game_type;
    bool finished=false;
    PlayersSides first_player;
    std::map<PlayersSides,std::string> players_cards;
    Trick tricks[14];
    Game( int game_type,PlayersSides first_player) : game_type(game_type), first_player(first_player) {}
    void addTrick(Trick trick){
        tricks[trick.getTrickNumber()]=trick;
    }
    void setCards(PlayersSides side, std::string cards){
        players_cards[side]=cards;
    }
    void print(){
        std::cout<<"Game type: "<<game_type<<std::endl;
        std::cout<<"First player: "<<first_player<<std::endl;
        PlayersSides side = N;
        for(int i = 0 ;i<4;i++)
        {
            std::cout<<"Player "<<side<<": "<<players_cards[side]<<std::endl;
            ++side;
        }
    }
};