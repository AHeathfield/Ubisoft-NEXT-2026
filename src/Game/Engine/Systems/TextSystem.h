#pragma once

#include "../Core/ECS.h"

// This system just resets the wasChanged flag
class TextSystem : public System
{
public:
    void Update(float deltaTime) override;
    void Render() override;
};
