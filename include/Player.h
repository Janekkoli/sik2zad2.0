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

    PlayersSides seat=N;
    int points=0;
public:
    std::vector<Card> cards;
    int fd = -1;
    Player(PlayersSides seat) : seat(seat){}
    Player()= default;



    void setCards(const std::vector<Card>& cards){
        //std::cout<<"setting cards: "<<cards.size()<<"\n";
        this->cards=cards;
    }

    int getPoints() {
        return points;
    }
    bool canPlayThisCard(const Card card, const char trickColor) {
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

    //old design
    bool playCard(Card card){
        auto it = std::remove(cards.begin(), cards.end(), card);
        cards.erase(it, cards.end());
        return true; //TODO is it not a problem
    }
    void wrongCard(Card card){ //puts card back because server  it
        cards.push_back(card);
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
        for(auto card : cards){
            //if(cards[i].isPlayed())continue;
            s+=card.toString() + " ";
        }
        return s;
    }
    char getSeat() const {
        return seat;
    }
};


#endif //ZAD2_SIK_PLAYER_H
