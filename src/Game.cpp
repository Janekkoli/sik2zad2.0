//
// Created by jan on 6/9/24.
//

#include "Game.h"

Game::Game(int game_type, PlayersSides first_player): gameType(game_type) , firstPlayer(first_player) {
    tricks[1] = Trick(first_player,1,gameType); //setting first trick
    tricks[0] = Trick(firstPlayer,0,gameType);
    tricks[0].setWinner(firstPlayer); //this is like first player win trick number 0
}

void Game::addTrick(const Trick &trick) {
    tricks[trick.getTrickNumber()]=trick;
}

bool Game::isGameNew() { //true if no card was played in game yet
    if(currentTrickNum==1 && currentTrick().isTrickNew()) return true;
    return false;
}

Trick & Game::currentTrick() {
    return tricks[currentTrickNum];
}

bool Game::playCard(const PlayersSides side, const Card card, const int trickNumber, std::map<PlayersSides, int> &playersFd) {
    if(trickNumber != currentTrickNum) {return false;} // check trick number
    if(!tricks[currentTrickNum].playCard(side, card)) {return false;}
    if(currentTrick().points()!=-1) { //trick is finished
        //std::cout<<"Trick finished :((";
        playersScore[currentTrick().getWinner()]+=currentTrick().points();
        for(PlayersSides s: {N,E,S,W}) {
            currentTrick().sendTakenMess(playersFd[s]); // sending taken to all players
        }
        if(currentTrickNum == 13) {
            finished = true;
            //last trick ended so game is finished
        }
        else {
            ++currentTrickNum;
            tricks[currentTrickNum] = Trick(tricks[currentTrickNum-1].getWinner(), currentTrickNum, gameType);
        }
    }
    //std::cout<<"Current trick: "<<currentTrickNum<<"Current player: "<<currentTrick().currentPlayer<<"\n";
    return true;
}

void Game::sendDealMess(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
    send_DEAL(fd, gameType, firstPlayer,players_cards[side],true);
    for(int i = 1 ;i<currentTrickNum;i++) { // send trick info to player if needed
        tricks[i].sendTakenMess(fd);
    }
}

void Game::sendTrickMess(std::map<PlayersSides, int> &playersFd) {
    //std::cout<<"Sending trick message: "<<currentTrick().currentPlayer<<" "<<currentTrick().cardsToString()<<"Current trick: "<<currentTrickNum<<"\n";
    currentTrick().sendTrickMess(playersFd[currentTrick().currentPlayer]);
}

void Game::setCards(const PlayersSides side, const std::string &cards) {
    players_cards[side]=cards;
}

void Game::print() {
    std::cout<<"Game type: "<<gameType<<std::endl;
    std::cout<<"First player: "<<firstPlayer<<std::endl;
    PlayersSides side = N;
    for(int i = 0 ;i<4;i++)
    {
        std::cout<<"Player "<<side<<": "<<players_cards[side]<<std::endl;
        ++side;
    }
}
