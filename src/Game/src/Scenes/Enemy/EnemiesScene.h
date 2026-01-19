#pragma once

#include "../../../Engine/Core/Scene.h"
#include "../../../Engine/Core/Timer.h"
#include "../../../Engine/Math/Vector3.h"

class EnemiesScene : public Scene
{
public:
    void Enter() override;
    //void Exit() override;
    //void HandleEvent() override;
    void Update(float deltaTime) override;

private:
    void createSpawner(const Vector3& position, const float spawn_time);
    void spawnEnemy(const Vector3& position);

private:
    std::vector<Entity> mSpawners;
    Entity mEnemy = -1;
};
