#pragma once
#include "Trick.h"
#include "Player.h"
#include <map>
#include <string>
#include <utility>
class Game {
public:
    int gameType;
    bool finished=false;
    PlayersSides firstPlayer;
    std::map<PlayersSides,std::string> players_cards;
    Trick tricks[14];
    int currentTrickNum = 1;
    std::map<PlayersSides, int> playersScore;
    Game( int game_type,PlayersSides first_player) : gameType(game_type) , firstPlayer(first_player){
        tricks[1] = Trick(first_player,1,gameType); //setting first trick
    }
    void addTrick(Trick trick){
        tricks[trick.getTrickNumber()]=trick;
    }
    Trick currentTrick() {
        return tricks[currentTrickNum];
    }
    bool playCard(const PlayersSides side, const Card card, const int trickNumber,   std::map<PlayersSides, int>& playersFd) {
        if(trickNumber != currentTrickNum) return false; // check trick number
        if(!tricks[currentTrickNum].playCard(side, card)) return false;
        if(currentTrick().points()!=-1) { //trick is finished
            playersScore[currentTrick().getWinner()]+=currentTrick().points();
            for(PlayersSides s: {N,E,S,W}) {
                currentTrick().sendTakenMess(s, playersFd[s]); // sending taken to all players
            }
            if(currentTrickNum == 13) {
                finished = true;
                //last trick ended so game is finished
                //TODO send score or maybe not here

            }
            else {
                ++currentTrickNum;
                tricks[currentTrickNum] = Trick(tricks[currentTrickNum-1].getWinner(), currentTrickNum, gameType);
            }
        }

    }


    void sendDealMess(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
        for(int i = 1 ;i<currentTrickNum;i++) { // send trick info to player if needed
            tricks[i].sendTrickMess(fd);
        }
        send_DEAL(fd, gameType, firstPlayer,players_cards[side]);
    }

    void sendTrickMess(std::map<PlayersSides, int>  *playersFd) {
        tricks[currentTrickNum].sendTrickMess((*playersFd)[tricks[currentTrickNum].currentPlayer]);
    }

    void setCards(PlayersSides side, std::string cards){
        players_cards[side]=cards;
    }
    void print(){
        std::cout<<"Game type: "<<gameType<<std::endl;
        std::cout<<"First player: "<<firstPlayer<<std::endl;
        PlayersSides side = N;
        for(int i = 0 ;i<4;i++)
        {
            std::cout<<"Player "<<side<<": "<<players_cards[side]<<std::endl;
            ++side;
        }
    }
};