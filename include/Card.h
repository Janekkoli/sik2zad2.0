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
  Card(int value, char color) {
    if (value < 2 || value > 14) {
      throw std::invalid_argument("Invalid card value");
    }
    if (color != 'C' && color != 'D' && color != 'H' && color != 'S') {
      throw std::invalid_argument("Invalid card color");
    }
  }
  explicit Card(std::string code) {
    if (code.size() == 3 && code[0] == '1' && code[1] == '0') {
      value = 10;
      color = code[2];
    } else if (code.size() != 2)
      throw std::invalid_argument("Invalid card code");
    else
      color = code[1];
    if (code[0] >= '2' && code[0] <= '9')
      value = code[0] - '0';
    if (code[0] == 'J')
      value = 11;
    if (code[0] == 'Q')
      value = 12;
    if (code[0] == 'K')
      value = 13;
    if (code[0] == 'A')
      value = 14;
    if (value == -1 ||
        (color != 'H' && color != 'D' && color != 'C' && color != 'S'))
      throw std::invalid_argument("Invalid card code2");
  }
  [[nodiscard]] char getColor() const { return color; }
  [[nodiscard]] int getValue() const { return value; }

  [[nodiscard]] bool isPlayed() const { return played; }

  bool operator==(const Card &rhs) const {
    return color == rhs.color && value == rhs.value; //&& played == rhs.played;
  }
  void play() { played = true; }

  void wrong() { played = false; }

  [[nodiscard]] std::string toString() const {
    if(value == -1) return "";
    std::string s;
    if (value == 10)
      s = "10";
    else if (value == 11)
      s = "J";
    else if (value == 12)
      s = "Q";
    else if (value == 13)
      s = "K";
    else if (value == 14)
      s = "A";
    else
      s = std::to_string(value);
    s += color;
    return s;
  }
};

#endif // ZAD2_SIK_CARD_H
