#include "EnemiesScene.h"
#include "../Player/PlayerScene.h"
#include "../../Components/EnemyComponent.h"
#include "../../Systems/EnemySystem.h"
#include "../../../Engine/Core/GameManager.h"
#include "../../../Engine/Components/MeshComponent.h"
#include "../../../Engine/Components/Transform3DComponent.h"
#include "../../../Engine/Components/ColliderComponent.h"
#include "../../../Engine/Components/RigidBodyComponent.h"
#include "../../../Engine/Components/TimerComponent.h"

void EnemiesScene::Enter()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    // Registering the EnemySystem
    coordinator.RegisterComponent<EnemyComponent>();
	auto enemySys = coordinator.RegisterSystem<EnemySystem>();
{
		Signature signature;
		signature.set(coordinator.GetComponentType<EnemyComponent>());
		signature.set(coordinator.GetComponentType<Transform3DComponent>());
		signature.set(coordinator.GetComponentType<RigidBodyComponent>());
		coordinator.SetSystemSignature<EnemySystem>(signature);
	}
	GameManager::Instance().AddSystem(enemySys);


    mEnemy = CreateEntity();
    coordinator.AddComponent(
    	mEnemy,
    	MeshComponent("./data/Objects/cube.obj", Color(255, 0, 0))
    );
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

    // mKillzone = CreateEntity();
    // coordinator.AddComponent(
    // 	mKillzone,
    // 	Transform3DComponent(
    //         Vector3(0, 20.f, 0),
    //         Vector3(0.f, 0.f, 0.f),
    //         Vector3(100.f, 20.f, 100.f)
    //     )
    // );
    // coordinator.AddComponent(
    //  mKillzone,
    //     ColliderComponent(
    //         ColliderType::DYNAMIC,
    //         ShapeType::AABB_3D,
    //         [this](Entity self, Entity other, const CollisionManifold& collision) {
    //             this->onKillZoneEnter(other);
    //         },
    //         nullptr,
    //         nullptr
    //     )
    // );
    //
    // Creating spawners around the map
    createSpawner(Vector3(0.f, 10.f, 20.f), 1.0f);


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
}



// Private Methods
void EnemiesScene::createSpawner(const Vector3& position, const float spawn_time)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    Entity spawner = CreateEntity();

    coordinator.AddComponent(
    	spawner,
    	Transform3DComponent(
            position,
            Vector3(0, 0, 0.f),
            Vector3(1.f, 1.f, 1.f)
    	)
    );
    coordinator.AddComponent(
        spawner,
	    TimerComponent{
            .timer = Timer(false, spawn_time),
            .isLooping = true,
            .onTimerEnd = [this, &position](Entity self) {
                    this->spawnEnemy(position);
				}
        }
    );
}


// spawns enemy at spawner
void EnemiesScene::spawnEnemy(const Vector3& position)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    Entity enemy = CreateEntity();
    coordinator.AddComponent(
    	enemy,
    	MeshComponent("./data/Objects/cube.obj", Color(255, 0, 0))
    );
    coordinator.AddComponent(
    	enemy,
    	Transform3DComponent(
            position,
            Vector3(0, 0, 0.f),
            Vector3(1.f, 1.f, 1.f)
    	)
    );
    coordinator.AddComponent(
	    enemy,
	    RigidBodyComponent{
	       .mass = 3.f
	    }
    );
    coordinator.AddComponent(
	    enemy, EnemyComponent{}
    );
    coordinator.AddComponent(
	    enemy,
	    ColliderComponent(
            ColliderType::DYNAMIC,
            ShapeType::OBB_3D
	    )
    );
}


void EnemiesScene::onKillZoneEnter(Entity other)
{
    utility::log("killing");
    GameManager::Instance().GetCoordinator().DestroyEntity(other);
}
