//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_CARD_H
#define ZAD2_SIK_CARD_H
#include "common.h"
class Card {
private:
  int value = -1;
  char color = 0;
  bool played = false;

public:
  Card() = default;
  Card(int value, char color);

  explicit Card(std::string code);

  [[nodiscard]] char getColor() const;

  [[nodiscard]] int getValue() const;

  [[nodiscard]] bool isPlayed() const;

  bool operator==(const Card &rhs) const;

  void play();

  void wrong();

  [[nodiscard]] std::string toString() const;
};

#endif // ZAD2_SIK_CARD_H
