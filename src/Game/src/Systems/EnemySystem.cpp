#include "EnemySystem.h"
#include "../Scenes/Player/PlayerScene.h"
#include "../../Engine/Core/GameManager.h"
#include "../../Engine/Components/RigidBodyComponent.h"
#include "../../Engine/Components/Transform3DComponent.h"

void EnemySystem::Update(float deltaTime)
{
    GameManager& gm = GameManager::Instance();
    Coordinator& coordinator = gm.GetCoordinator();
    auto* playerScene =  static_cast<PlayerScene*>(gm.GetScene("PlayerScene"));
    Vector3 playerPos = playerScene->GetPlayerPosition();
    float speed = 8.f;

    std::set<Entity> ents_to_check = mEntities;
    for (const auto& ent : ents_to_check)
    {
        auto& enemyTransform = coordinator.GetComponent<Transform3DComponent>(ent);
        auto& enemyRB = coordinator.GetComponent<RigidBodyComponent>(ent);
        Vector3 dir = (playerPos - enemyTransform.GetPosition()).GetNormal();

        // Move towards player
        enemyRB.velocity += dir * speed * deltaTime;
        // enemyRB.velocity.y -= 2.f * deltaTime;

        if (enemyTransform.GetPosition().y > _KILLY)
        {
            coordinator.DestroyEntity(ent);
        }
    }
}
