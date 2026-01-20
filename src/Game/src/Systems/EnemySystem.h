#pragma once

#include "../../Engine/Core/ECS.h"

class EnemySystem : public System
{
public:
    void Update(float deltaTime) override;

private:
    static constexpr float _KILLY = 20.f;

};
