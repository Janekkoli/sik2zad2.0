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

public:
  std::list<Game> games = {Game(0, None)};
  std::map<PlayersSides, Player> players;
  std::map<PlayersSides, int> playersFd;
  All_Games();

  void addGame(Game game);

  Player& getPlayer(const PlayersSides side);

  void sendDealMess(const PlayersSides side, const int fd);

  void sendTrickMess();

  void sendTotalMess();

  void startNewGame();

  bool playCard(const PlayersSides side, const Card card, const int trickNumber);

  void AddNewGame(Game game);

  Game& currentGame();

  Game& lastGame();
};
extern All_Games all_games;

#endif // ZAD2_SIK_ALLGAMES_H
