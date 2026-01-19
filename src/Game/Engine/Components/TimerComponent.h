#pragma once

#include "../Core/Timer.h"
#include "../Core/ECS.h"

struct TimerComponent
{
    Timer timer = Timer(true, 1.f);
    bool isLooping = false;

    std::function<void(Entity self)> onTimerEnd = nullptr;
};
