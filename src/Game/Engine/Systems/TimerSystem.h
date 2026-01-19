#pragma once

#include "../Core/ECS.h"

class TimerSystem : public System
{
public:
    void Update(float deltaTime) override;
};
