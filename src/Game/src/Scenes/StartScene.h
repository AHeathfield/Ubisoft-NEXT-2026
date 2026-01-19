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
    Entity mCube = -1;
    Entity mSphere = -1;
    Entity mFloor = -1;
    Entity mWall = -1;
    Entity mZombie = -1;
    Entity mBird = -1;
};
