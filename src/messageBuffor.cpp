//
// Created by jan on 8/30/24.
//

#include "../include/messageBuffor.h"

std::string messageBuffor::addToBuffor(char *buffer, ssize_t messageLen) {
    std::string ret = "";
    restOnBuffor += std::string(buffer, messageLen);
    size_t pos;
    const std::string delimiter = DELIMITER;
    pos = restOnBuffor.rfind(DELIMITER);
    if(pos == std::string::npos) return "";
    ret = restOnBuffor.substr(0, pos+ delimiter.length()); // TODO is this allright
    restOnBuffor.erase(0, pos+delimiter.length());
    return ret;
}
