#pragma once
#include <chrono>

using namespace std::chrono_literals;
class DeltaTime {
    std::chrono::steady_clock::time_point previousTime = std::chrono::steady_clock::now();
public:
    std::chrono::nanoseconds GetDeltaTime() {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = currentTime - previousTime;
        previousTime = currentTime;

        return elapsed;
    }

    long float GetDeltaTime_s() {
        long float nanoseconds = GetDeltaTime().count();
        long float sToNanoseconds = std::chrono::nanoseconds(1s).count();
        long float result = nanoseconds / sToNanoseconds;
        return  result;
    }

    void Reset() {
        previousTime = std::chrono::steady_clock::now();
    }

};