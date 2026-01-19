#include "EnemiesScene.h"
#include "../Player/PlayerScene.h"
#include "../../../Engine/Core/GameManager.h"
#include "../../../Engine/Components/MeshComponent.h"
#include "../../../Engine/Components/Transform3DComponent.h"
#include "../../../Engine/Components/ColliderComponent.h"
#include "../../../Engine/Components/RigidBodyComponent.h"
#include "../../../Engine/Components/Transform2DComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"

void EnemiesScene::Enter()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    mEnemy = CreateEntity();
    coordinator.AddComponent(
    	mEnemy,
    	MeshComponent("./data/Objects/cube.obj", Color(255, 0, 0))
    );
    utility::log("test");
    coordinator.AddComponent(
    	mEnemy,
    	Transform3DComponent(
            Vector3(0, 0, 10.f),
            Vector3(0, 0, 0.f),
            Vector3(1.f, 1.f, 1.f)
    	)
    );
    coordinator.AddComponent(
	    mEnemy,
	    RigidBodyComponent{
	       .mass = 3.f
	    }
    );
    coordinator.AddComponent(
	    mEnemy,
	    ColliderComponent(
            ColliderType::DYNAMIC,
            ShapeType::OBB_3D
	    )
    );


    // coordinator.AddComponent(
    //  mEnemy,
    //  MeshComponent("./data/Objects/zombie.obj", Color(255, 0, 0))
    // );
    // coordinator.AddComponent(
    //  mEnemy,
    //  Transform3DComponent(
    //         Vector3(0.f, 0.f, 20.f),
    //         Vector3(0, 0, 180.f),
    //         Vector3(0.5f, 0.5f, 0.5f)
    //  )
    // );
    // coordinator.AddComponent(
    // 		mEnemy,
    // 		RigidBodyComponent{}
    // );
    // coordinator.AddComponent(
    //  mEnemy,
    //  ColliderComponent(
    //         ColliderType::DYNAMIC,
    //         ShapeType::OBB_3D
    //  )
    //  // ColliderComponent(
    //  //        CollidSPHERE_3DerType::DYNAMIC,
    //  //        coordinator.GetComponent<MeshComponent>(mEnemy).mesh
    //  // )
    // );
    //
    
    // GameManager::Instance().AddScene("EnemiesScene", std::make_unique<EnemiesScene>());

}


void EnemiesScene::Update(float deltaTime)
{
    GameManager& gm = GameManager::Instance();
    Coordinator& coordinator = gm.GetCoordinator();

    auto& enemyTransform = coordinator.GetComponent<Transform3DComponent>(mEnemy);
    auto& enemyRB = coordinator.GetComponent<RigidBodyComponent>(mEnemy);
    auto* playerScene =  static_cast<PlayerScene*>(gm.GetScene("PlayerScene"));

    Vector3 playerPos = playerScene->GetPlayerPosition();
    float speed = 8.f;

    // Move towards player
    Vector3 dir = (playerPos - enemyTransform.GetPosition()).GetNormal();
    enemyRB.velocity += dir * speed * deltaTime;


}
