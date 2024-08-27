#include <iostream>
#include "common.h"

int main() {
    auto v =  cardStringToArray2("2D3D4D5D6D7D8D9D10DJDQDKDAD");
    std::cout<<"\nsize:"<< v.size()<<"\n";
    for(Card c: v) {
        std::cout<<c.toString()<<" ";
    }
    PlayersSides s = N;
    std::cout<<s<<"\n";
    ++s;
    std::cout<<s;
}
