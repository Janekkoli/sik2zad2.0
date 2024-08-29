//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_TRICK_H
#define ZAD2_SIK_TRICK_H
#include <bits/stdc++.h>
#include "Card.h"
class Trick {
private:
    std::map<PlayersSides,Card> cards;
    PlayersSides firstPlayer;
    int trickNumber = -1;
    PlayersSides winner;
    int trickType;

public:
    PlayersSides currentPlayer;

    Trick(PlayersSides first_player, int trickNumber, int trickType) : firstPlayer(first_player), trickNumber(trickNumber), trickType(trickType) {currentPlayer = first_player;winner=firstPlayer;}
    Trick()= default;

    bool isTrickNew() { //true is no card was played in trick yet
        if(currentPlayer==firstPlayer) return true;
        return false;
    }

    void sendTakenMess(const int fd) {
        char winStr = toChar(winner);
        send_TAKEN(fd, trickNumber, cardsToShortString().c_str(),&winStr );
    }

    void establishWinner() {
        char trickColor = cards[firstPlayer].getColor();
        for(PlayersSides s: {N,E,S,W}){
            if(cards[s].getColor() == trickColor && cards[s].getValue() > cards[winner].getValue())
                winner = s;
        }
    }

    bool playCard(const PlayersSides side, const Card card){
        if(side!=currentPlayer)return false;
        cards[side] = card;
        ++currentPlayer;
        std::cout<<"Current player: "<<currentPlayer<<"\n";
        establishWinner();
        return true;
    }

    [[nodiscard]] std::map<PlayersSides, Card> getCards() const {
        return cards;
    }
    void setWinner(const PlayersSides winner) {
        Trick::winner = winner;
    }

    Card& getFirstCardPlayed(){
        return cards[firstPlayer];
    }

    void sendTrickMess(const int fd) {
        send_TRICK(fd, trickNumber, cardsToShortString().c_str());
    }

    [[nodiscard]] PlayersSides getWinner() const {
        return winner;
    }
    std::string cardsToString(){
        std::string s;
        PlayersSides x = firstPlayer;
        for(int i=0; i<4; i++){
            s+=cards[x].toString() + " ";
            ++x;
        }
        return s;
    }

    std::string cardsToShortString() {
        std::string s;
        PlayersSides x = firstPlayer;
        for(int i=0; i<4; i++){
            s+=cards[x].toString();
            ++x;
        }
        return s;
    }

    [[nodiscard]] int getTrickNumber() const {
        return trickNumber;
    }

    char trickColor() { // 0 if no card was assign
        return cards[firstPlayer].getColor();
    }
    int points() {
        return points(trickType);
    }
    int points(int t){
        if(currentPlayer+1 != firstPlayer) return -1; //trick is not finished
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

    void addCard(const Card card){
        //player has to play the same color as the first card if he has one
        //---
        cards[currentPlayer] = card;
        ++currentPlayer;
    }
    void addCards(const std::vector<Card>& cards) {
        for(const Card c: cards) {
            this->cards[currentPlayer]= c;
            ++currentPlayer;
        }
    }
};


#endif //ZAD2_SIK_TRICK_H
