//
// Created by jan on 6/9/24.
//
#pragma once
#ifndef ZAD2_SIK_PLAYER_H
#define ZAD2_SIK_PLAYER_H
#include <bits/stdc++.h>
#include "Game.h"
class Player {
private:
    std::vector<Card> cards;
    PlayersSides seat=N;
    int points=0;
public:
    int fd = -1;
    Player(PlayersSides seat) : seat(seat){}
    Player()= default;


    void setCards(const std::vector<Card>& cards){
        this->cards=cards;
    }

    bool canPlayThisCard(Card card, char trickColor) {
        if(card.getColor() != trickColor && trickColor!=0)
            for(Card c: cards)
                if(c.getColor() == trickColor)
                    return false;
        for(Card c: cards)
            if(c==card)
                return true;
        return false;
    }

    //old design
    bool playCard(Card card){
        for(int i=0; i<13; i++){
            if(cards[i]==card){
                cards[i].play();
                break;
            }
            if(i==12)return false;
        }
        return true;
    }
    bool wrongCard(Card card){ //player cant play this card
        for(int i=0; i<13; i++){
            if(cards[i]==card){
                cards[i].wrong();
                break;
            }
            if(i==12)return false;
        }
        return true;
    }

    int numberOfCards(char color){
        int number=0;
        for(int i=0; i<13; i++){
            if(cards[i].getColor()==color && !cards[i].isPlayed())number++;
        }
        return number;
    }

    void receivePoints(int p){
        points+=p;
    }
    std::string cardsToString(){
        std::string s;
        if(cards.empty())return "no cards";
        for(int i=0;i<13;i++){
            if(cards[i].isPlayed())continue;
            s+=cards[i].toString() + " ";
        }
        return s;
    }
    char getSeat() const {
        return seat;
    }
};


#endif //ZAD2_SIK_PLAYER_H
