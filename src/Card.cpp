//
// Created by jan on 6/9/24.
//

#include "Card.h"

Card::Card(int value, char color) {
    if (value < 2 || value > 14) {
        throw std::invalid_argument("Invalid card value");
    }
    if (color != 'C' && color != 'D' && color != 'H' && color != 'S') {
        throw std::invalid_argument("Invalid card color");
    }
}

Card::Card(std::string code) {
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

char Card::getColor() const { return color; }

int Card::getValue() const { return value; }

bool Card::isPlayed() const { return played; }

bool Card::operator==(const Card &rhs) const {
    return color == rhs.color && value == rhs.value; //&& played == rhs.played;
}

void Card::play() { played = true; }

void Card::wrong() { played = false; }

std::string Card::toString() const {
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
