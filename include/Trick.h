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
    PlayersSides nextPlayer;
    PlayersSides first_player;
    int trickNumber = -1;
    PlayersSides winner;
public:
    Trick(PlayersSides first_player, int trickNumber) : first_player(first_player), trickNumber(trickNumber) {nextPlayer = first_player;}
    Trick()= default;

    std::map<PlayersSides, Card> getCards() const {
        return cards;
    }
    void setWinner(PlayersSides winner) {
        Trick::winner = winner;
    }

    Card getFirstCardPlayed(){
        return cards[first_player];
    }

    [[nodiscard]] PlayersSides getWinner() const {
        return winner;
    }
    std::string cardsToString(){
        std::string s;
        PlayersSides x = first_player;
        for(int i=0; i<4; i++){
            s+=cards[x].toString() + " ";
            ++x;
        }
        return s;
    }

    [[nodiscard]] int getTrickNumber() const {
        return trickNumber;
    }

    int points(int t){
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

    bool addCard(Card card){
        //player has to play the same color as the first card if he has one
        //---
        cards[nextPlayer] = card;
        ++nextPlayer;
        return true;
    }
};


#endif //ZAD2_SIK_TRICK_H
