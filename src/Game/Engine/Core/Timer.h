#pragma once

#include <cfloat>

class Timer
{
public:
    Timer() = default;
    Timer(const bool start_paused, const float max = FLT_MAX);

    void SetMax(const float max);

    void Increment(const float deltaTime);
    void Unpause();
    void Pause();
    void Reset(const bool start_pause = false);

    float GetSeconds() const;
    bool IsPaused() const;
    bool IsFinished() const;

private:
    float mSecondsPassed = 0.f;
    float mMax = FLT_MAX;
    bool mPaused = false;
    bool mFinished = false;
};
