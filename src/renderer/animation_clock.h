#ifndef ANIMATION_CLOCK_H
#define ANIMATION_CLOCK_H

#include <chrono>

class animation_clock {
public:
    animation_clock();
    ~animation_clock();

    void start();
    float seconds() const;
    
private:
    std::chrono::steady_clock::time_point start_time;
    
};

#endif
