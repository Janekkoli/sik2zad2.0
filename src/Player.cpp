//
// Created by jan on 6/9/24.
//

#include "Player.h"

Player::Player(PlayersSides seat): seat(seat) {}

void Player::setCards(const std::vector<Card> &cards) {
    //std::cout<<"setting cards: "<<cards.size()<<"\n";
    this->cards=cards;
}

int Player::getPoints() {
    return points;
}

bool Player::canPlayThisCard(const Card card, const char trickColor) {
    //std::cout<<cardsToString()<<" | "<<card.toString()<<" | "<<trickColor<<"\n";
    if(card.getColor() != trickColor && trickColor!=0)
        for(Card c: cards)
            if(c.getColor() == trickColor)
                return false;
    for(Card c: cards)
        if(c==card)
            return true;
    return false;
}

bool Player::playCard(Card card) {
    bool isTheCardInHand = false; //TODO check if card is in deck
    for(Card c: cards)
        if(c==card)
            isTheCardInHand = true;
    if(!isTheCardInHand) return false;
    auto it = std::remove(cards.begin(), cards.end(), card);

    cards.erase(it, cards.end());
    return true; //TODO is it not a problem
}

void Player::wrongCard(Card card) { //puts card back because server  it
    cards.push_back(card);
}

int Player::numberOfCards(char color) {
    int number=0;
    for(int i=0; i<13; i++){
        if(cards[i].getColor()==color && !cards[i].isPlayed())number++;
    }
    return number;
}

void Player::receivePoints(int p) {
    points+=p;
}

std::string Player::cardsToString() {
    std::string s;
    if(cards.empty())return "no cards";
    for(auto card : cards){
        //if(cards[i].isPlayed())continue;
        s+=card.toString() + " ";
    }
    return s;
}

char Player::getSeat() const {
    return seat;
}
