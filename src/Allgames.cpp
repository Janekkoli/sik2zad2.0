//
// Created by jan on 6/9/24.
//

#include "Allgames.h"
All_Games all_games = All_Games();

All_Games::All_Games() {
    players[N] = Player(N);
    players[E] = Player(E);
    players[S] = Player(S);
    players[W] = Player(W);
}

void All_Games::addGame(Game game) { games.push_back(game); }

Player & All_Games::getPlayer(const PlayersSides side) { return players[side]; }

void All_Games::sendDealMess(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
    currentGame().sendDealMess(side, fd);
}

void All_Games::sendTrickMess() {
    for(PlayersSides s: {N,E,S,W})
        playersFd[s] = players[s].fd;
    currentGame().sendTrickMess(playersFd);
}

void All_Games::sendTotalMess() {
    //std::cout<<"SENDING TOTAL TO ALL PLAYERS\n";
    for(PlayersSides s: {N,E,S,W}) {
        send_TOTAL(players[s].fd, players[N].getPoints(), players[E].getPoints(), players[S].getPoints(), players[W].getPoints(),true);
    }
}

void All_Games::startNewGame() {
    games.pop_front(); //go to next game

    for(PlayersSides s: {N,E,S,W}) {
        players[s].setCards(cardStringToArray2(currentGame().players_cards[s])); //setting cards for players
    }
}

bool All_Games::playCard(const PlayersSides side, const Card card, const int trickNumber) {
    if(!players[side].canPlayThisCard(card, currentGame().tricks[trickNumber].trickColor())) {std::cout<<"pplayers.playcard wrong\n";return false;}
    //player has this card and color is alright
    if(!currentGame().playCard(side, card, trickNumber, playersFd)) {std::cout<<"current game playcard error\n";return false;}
    players[side].playCard(card);
    if(currentGame().finished) { //game finished
        for(PlayersSides s: {N,E,S,W}) {
            players[s].receivePoints(currentGame().playersScore[s]);
            //std::cout<<"SENDIONG SCORE MESSAGE\n";
            send_SCORE(players[s].fd, currentGame().playersScore[N], currentGame().playersScore[E], currentGame().playersScore[S], currentGame().playersScore[W],true );
        }
    }
    return true;
}

void All_Games::AddNewGame(Game game) {
    games.push_back(game);
}

Game & All_Games::currentGame() {
    //there must be some current game if no game is finished
    return games.front();
}

Game & All_Games::lastGame() {
    return games.back();
}
