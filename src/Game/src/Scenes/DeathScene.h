#pragma once

#include "../../Engine/Core/Scene.h"

// TODO: Change this to LevelScene or smth
class DeathScene : public Scene
{
public:
    void Enter() override;
    void Update(float deltaTime) override;

private:
    Entity mText = -1;
};
