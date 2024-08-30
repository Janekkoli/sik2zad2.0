//
// Created by jan on 8/30/24.
//
#pragma once
#include "common.h"
#ifndef MESSAGEBUFFOR_H
#define MESSAGEBUFFOR_H



class messageBuffor {
    std::string restOnBuffor = "";
public:
    std::string addToBuffor(char* buffer, ssize_t messageLen);
};



#endif //MESSAGEBUFFOR_H
