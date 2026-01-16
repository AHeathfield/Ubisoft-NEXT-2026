#pragma once

#include "../../Engine/Core/Scene.h"

// TODO: Change this to LevelScene or smth
class StartScene : public Scene
{
public:
    void Enter() override;
    //void Exit() override;
    //void HandleEvent() override;
    void Update(float deltaTime) override;

private:
    Entity mCube;
    Entity mSphere;
    Entity mFloor;
    Entity mZombie;
    Entity mBird;
};
