#pragma once

#include "../Core/ECS.h"

// This system just resets the wasChanged flag
class Transform3DSystem : public System
{
public:
    void Update(float deltaTime) override;
};
