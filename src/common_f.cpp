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







void split_tricknum_rest(const std::string& content, int &trick_number, std::string &cards)  //splits trick number and cards
{
    sscanf(content.c_str(), "%d", &trick_number);
    if(trick_number/1000 > 0) {cards=content.substr(2, content.size()-2);trick_number/=100;} //4 digit number
    else if(trick_number/100 > 0 && trick_number%10==10) {cards=content.substr(1,content.size()-1); trick_number/=100;} //one digit trick number and card 10*
    else if(trick_number/100>0) {cards=content.substr(2, content.size()-2); trick_number/=10;} //two digit trick number and one digit card
    //two digit trick ???
    else if(trick_number/10>0 && (content[2]=='C' || content[2]=='H' || content[2]=='D' || content[2]=='S')) {cards=content.substr(1, content.size()-1); trick_number/=10;  } //one digit trick and card with number
    else if(trick_number/10>0) {cards=content.substr(2, content.size()-2);}
    //one digit trick
    else {cards=content.substr(1, content.size()-1);} //
    std::cout<<"from split trick function: trick number - "<< trick_number<< " cards string - "<< cards<<"\n";
}











