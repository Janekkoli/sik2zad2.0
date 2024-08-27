//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_ALLGAMES_H
#define ZAD2_SIK_ALLGAMES_H
#include "Player.h"
#include "common.h"
class All_Games {
private:
  std::list<Game> games = {Game(0, None)};

public:
  std::map<PlayersSides, Player> players;
  std::map<PlayersSides, int> playersFd;
  All_Games() {
    players[N] = Player(N);
    players[E] = Player(E);
    players[S] = Player(S);
    players[W] = Player(W);
  }
  void addGame(Game game) { games.push_back(game); }
  Player* getPlayer(const PlayersSides side) { return &players[side]; }
  void sendDealMess(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
      currentGame().sendDealMess(side, fd);
  }

  void sendTrickMess() {
    for(PlayersSides s: {N,E,S,W})
      playersFd[s] = players[s].fd;
    currentGame().sendTrickMess(&playersFd);
  }

  void sendTotalMess() {
    for(PlayersSides s: {N,E,S,W}) {
      send_TOTAL(players[s].fd, players[N].getPoints(), players[E].getPoints(), players[S].getPoints(), players[W].getPoints());
    }
  }

  void startNewGame() {
    for(PlayersSides s: {N,E,S,W}) {
      players[s].setCards(cardStringToArray2(currentGame().players_cards[s])); //setting cards for players
    }
    games.pop_front(); //go to next game
  }

  bool playCard(const PlayersSides side, const Card card, const int trickNumber) {
    if(!players[side].canPlayThisCard(card, currentGame().tricks[trickNumber].trickColor())) return false;
    //player has this card and color is alright
    if(!currentGame().playCard(side, card, trickNumber, playersFd)) return false;
    if(currentGame().finished) { //game finished
      for(PlayersSides s: {N,E,S,W}) {
        players[s].receivePoints(currentGame().playersScore[s]);
        send_SCORE(players[s].fd, currentGame().playersScore[N], currentGame().playersScore[E], currentGame().playersScore[S], currentGame().playersScore[W] );
      }
    }
    return true;
  }

  bool AddNewGame(Game game) {
    if (!(games.empty() || games.back().finished))
      return false;
    games.push_back(game);
    return true;
  }

  Game currentGame() {
    //there must be some current game if no game is finished
    return games.front();
  }
  Game *lastGame() {
    if (games.empty())
      return nullptr;
    return &games.back();
  }
};
extern All_Games all_games;

#endif // ZAD2_SIK_ALLGAMES_H
