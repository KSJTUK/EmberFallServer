#pragma once

class Timer {
public:
    using Time_t = float;
    using Clock = std::chrono::high_resolution_clock;
    using Unit = std::chrono::milliseconds;
    using Duration = std::chrono::duration<Time_t, Unit>;
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    using FPS_REP = size_t;

public:
    Timer();
    ~Timer();

private:
    TimePoint mPrevTimePoint;
    TimePoint mStartTimePoint;

    TimePoint mElapsedTime;

};
