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
  std::list<Game> games;

public:
  std::map<PlayersSides, Player> players;
  All_Games() {
    players[N] = Player();
    players[E] = Player();
    players[S] = Player();
    players[W] = Player();
  }
  void addGame(Game game) { games.push_back(game); }

  void addNewConnectPlayer(const PlayersSides side, const int fd) { //adding newely connected player and sending him his info
    //TODO also send past tricks
    std::cout<<"XDDD"<<side<<" games num"<<games.size()<<std::endl;
    send_DEAL(fd, currentGame().game_type, currentGame().first_player,currentGame().players_cards[side]);
  }

  bool startNewGame(Game game) {
    if (!(games.empty() || games.back().finished))
      return false;
    games.push_back(game);
    return true;
  }

  Game currentGame() {
    //there must be some current game if no game is finished
    return games.back();
  }
  Game *lastGame() {
    if (games.empty())
      return nullptr;
    return &games.back();
  }
};
extern All_Games all_games;

#endif // ZAD2_SIK_ALLGAMES_H
