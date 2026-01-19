#pragma once

#include "../../../Engine/Core/Scene.h"

class EnemiesScene : public Scene
{
public:
    void Enter() override;
    //void Exit() override;
    //void HandleEvent() override;
    void Update(float deltaTime) override;

private:
    // std::vector<Vector3> mSpawnerLocations;
    Entity mEnemy = -1;
};
