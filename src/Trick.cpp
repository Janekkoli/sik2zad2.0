//
// Created by jan on 6/9/24.
//

#include "Trick.h"

Trick::Trick(PlayersSides first_player, int trickNumber, int trickType): firstPlayer(first_player), trickNumber(trickNumber), trickType(trickType) {currentPlayer = first_player;winner=firstPlayer;}

bool Trick::isTrickNew() { //true is no card was played in trick yet
    if(currentPlayer==firstPlayer) return true;
    return false;
}

void Trick::sendTakenMess(const int fd) {
    char winStr = toChar(winner);
    send_TAKEN(fd, trickNumber, cardsToShortString().c_str(),&winStr, true );
}

void Trick::establishWinner() {
    for(PlayersSides s: {N,E,S,W}){
        if(cards[s].getColor() == trickColor() && cards[s].getValue() > cards[winner].getValue())
            winner = s;
    }
}

bool Trick::playCard(const PlayersSides side, const Card &card) {
    //std::cout<<"Playing card: "<<card.toString()<<" in trick "<<trickNumber<<" by player "<<side<<" "<<cardsToString()<<" current player"<< currentPlayer<<"\n";
    if(side!=currentPlayer)return false;
    cards[side] = card;
    ++currentPlayer;
    //std::cout<<"Current player: "<<currentPlayer<<"\n";
    if(currentPlayer==firstPlayer)
        establishWinner();
    return true;
}

void Trick::wrongCard() {
    --currentPlayer;
    cards[currentPlayer]=Card();
}

std::map<PlayersSides, Card> & Trick::getCards() {
    return cards;
}

void Trick::setWinner(const PlayersSides winner) {
    this->winner = winner;
}

Card & Trick::getFirstCardPlayed() {
    return cards[firstPlayer];
}

void Trick::sendTrickMess(const int fd) {
    send_TRICK(fd, trickNumber, cardsToShortString().c_str(),true);
}

PlayersSides & Trick::getWinner() {
    return winner;
}

std::string Trick::cardsToString() {
    std::string s;
    PlayersSides x = firstPlayer;
    for(int i=0; i<4; i++){
        s+=cards[x].toString() + " ";
        ++x;
    }
    return s;
}

std::string Trick::cardsToShortString() {
    std::string s;
    PlayersSides x = firstPlayer;
    for(int i=0; i<4; i++){
        s+=cards[x].toString();
        ++x;
    }
    return s;
}

int Trick::getTrickNumber() const {
    return trickNumber;
}

char Trick::trickColor() { // 0 if no card was assigned
    return cards[firstPlayer].getColor();
}

int Trick::points() {
    return points(trickType);
}

int Trick::points(int t) {
    if(currentPlayer != firstPlayer) return -1; //trick is not finished
    /*1 - nie brać lew, za każdą wziętą lewę dostaje się 1 punkt;
        2 - nie brać kierów, za każdego wziętego kiera dostaje się 1 punkt;
        3 -nie brać dam, za każdą wziętą damę dostaje się 5 punktów;
        4 -nie brać panów (waletów i króli), za każdego wziętego pana dostaje się 2 punkty;
        5 -nie brać króla kier, za jego wzięcie dostaje się 18 punktów;
        6 -nie brać siódmej i ostatniej lewy, za wzięcie każdej z tych lew dostaje się po 10 punktów;
        7 -rozbójnik, punkty dostaje się za wszystko wymienione powyżej.*/
    if(t == 1) return 1;
    if(t == 2) {
        int points=0;
        for(auto & card : cards){
            if(card.second.getColor()=='H')points++;
        }
        return points;
    }
    if(t == 3) {
        int points=0;
        for(auto & card : cards){
            if(card.second.getValue()==12)points+=5;
        }
        return points;
    }
    if(t == 4) {
        int points=0;
        for(auto & card : cards){
            if(card.second.getValue()==11 || card.second.getValue()==13)points+=2;
        }
        return points;
    }
    if(t == 5) {
        for(auto & card : cards){
            if(card.second.getValue()==13 && card.second.getColor()=='H')return 18;
        }
        return 0;
    }
    if(t == 6) return (trickNumber == 7 || trickNumber == 13) ? 20 : 0;
    if(t == 7) return points(1) + points(2) + points(3) + points(4) + points(5) + points(6);
    return -1;
}

void Trick::addCard(const Card card) {
    //player has to play the same color as the first card if he has one
    //---
    cards[currentPlayer] = card;
    ++currentPlayer;
}

void Trick::addCards(const std::vector<Card> &cards) {
    for(const Card c: cards) {
        this->cards[currentPlayer]= c;
        ++currentPlayer;
    }
}

void Trick::print() {
    std::cout<<"Trick number: "<<trickNumber<<" Trick type: "<<trickType<<" First player: "<<firstPlayer<<" Winner: "<<winner<<" Current player: "<<currentPlayer<<"\n";
    std::cout<<"Cards: "<<cardsToString()<<"\n";
}
