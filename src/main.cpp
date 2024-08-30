#include <iostream>
#include "common.h"
#include <string>
#include <chrono>
#include <iomanip>

int main() {
    auto now = std::chrono::system_clock::now();
    time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) %1000;

    std::stringstream time;
    time << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H:%M:%S");
    time << "." << std::setw(3) << std::setfill('0') << milliseconds.count();
    std::cout<<time.str();
}
