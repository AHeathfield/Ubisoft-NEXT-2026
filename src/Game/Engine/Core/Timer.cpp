#include "Timer.h"

Timer::Timer(const bool start_paused, const float max)
    : mPaused(start_paused)
{
    SetMax(max);
}

void Timer::SetMax(const float max)
{
    if (max < 0.f)
        mMax = 0.f;
    else
        mMax = max;
}

void Timer::Increment(const float deltaTime)
{
    if (mPaused || mFinished)
        return;

    mSecondsPassed += deltaTime;

    if (mSecondsPassed > mMax)
    {
        mSecondsPassed = mMax;
        mFinished = true;
    }
}

void Timer::Unpause()
{
    mPaused = false;
}


void Timer::Pause()
{
    mPaused = true;
}


void Timer::Reset(const bool start_paused)
{
    mPaused = start_paused;
    mFinished = false;
    mSecondsPassed = 0.f;
}

float Timer::GetSeconds() const
{
    return mSecondsPassed;
}

bool Timer::IsPaused() const
{
    return mPaused;
}

bool Timer::IsFinished() const
{
    return mFinished;
}

