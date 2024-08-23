#pragma once
#include "Trick.h"
#include "Player.h"
#include <map>
#include <string>
class Game {
public:
    int game_type;
    bool finished=false;
    PlayersSides firstPlayer;
    std::map<PlayersSides,std::string> players_cards;
    Trick tricks[14];
    int currentTrick = 1;
    Game( int game_type,PlayersSides first_player) : game_type(game_type) , firstPlayer(first_player){
        tricks[1] = Trick(first_player,1); //setting first trick
    }
    void addTrick(Trick trick){
        tricks[trick.getTrickNumber()]=trick;
    }

    bool playCard(const PlayersSides side, const Card card, const int trickNumber) {
        if(trickNumber != currentTrick) return false; // check trick number
        if(!tricks[currentTrick].playCard(side, card)) return false;
        if(side == W) {
            if(currentTrick == 13) {
                finished = true;
                //TODO send score
            }
            else {
                ++currentTrick;
                tricks[currentTrick] = Trick(tricks[currentTrick-1].getWinner(), currentTrick);
            }
        }

    }


    void sendDealMess(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
        //TODO also send past tricks
        send_DEAL(fd, game_type, firstPlayer,players_cards[side]);
    }

    void sendTrickMess(std::map<PlayersSides, int>  *playersFd) {
        tricks[currentTrick].sendTrickMess((*playersFd)[tricks[currentTrick].currentPlayer]);
    }

    void setCards(PlayersSides side, std::string cards){
        players_cards[side]=cards;
    }
    void print(){
        std::cout<<"Game type: "<<game_type<<std::endl;
        std::cout<<"First player: "<<firstPlayer<<std::endl;
        PlayersSides side = N;
        for(int i = 0 ;i<4;i++)
        {
            std::cout<<"Player "<<side<<": "<<players_cards[side]<<std::endl;
            ++side;
        }
    }
};