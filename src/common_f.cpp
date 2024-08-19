//
// Created by jan on 6/9/24.
//
#include <bits/stdc++.h>
#include "common_f.h"

int cardStringToArray(const std::string& karty, Card* wyn, int n)
{
    std::string k;
    int i = 0;
    for(auto & c : karty){
        k+=c;
        if(c=='C' || c=='D' || c=='H' || c=='S'){
            if(i>=n) return 0;
            try{wyn[i]=*new Card(k);}
            catch(std::invalid_argument &e){ return 0;}
            k = "";i++;
        }
    }
    return i;
}







void split_tricknum_rest(std::string content, int &trick_number, std::string &cards)
{
    sscanf(content.c_str(), "%d", &trick_number);
    std::cout<< trick_number <<"- \n";
    if(trick_number/1000 > 0) {cards=content.substr(2, content.size()-2);trick_number/=100;}
    else if(trick_number/100 > 0 && trick_number%100==10) {cards=content.substr(1,content.size()-1); trick_number/=100;}
    else if(trick_number/100>0) {cards=content.substr(2, content.size()-2); trick_number/=10;}
    else {cards=content.substr(1, content.size()-1);trick_number/=10;}
}











