#include "StartScene.h"
#include "Player/PlayerScene.h"
#include "Enemy/EnemiesScene.h"
#include "../../Engine/Core/GameManager.h"
#include "../../Engine/Components/MeshComponent.h"
#include "../../Engine/Components/Transform3DComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/Components/RigidBodyComponent.h"
#include "../../Engine/Components/Transform2DComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Systems/Render3DSystem.h"
#include <cstdint>
#include <memory>

void StartScene::Enter()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    // mCube = CreateEntity();
    // coordinator.AddComponent(
    // 	mCube,
    // 	MeshComponent("./data/Objects/cube.obj", Color(255, 0, 0))
    // );
    // utility::log("test");
    // coordinator.AddComponent(
    // 	mCube,
    // 	Transform3DComponent(
    //         Vector3(0, 0, 10.f),
    //         Vector3(0, 0, 0.f),
    //         Vector3(1.f, 1.f, 1.f)
    // 	)
    // );
    // coordinator.AddComponent(
    //  mCube,
    //  RigidBodyComponent{
    //     .mass = 3.f
    //  }
    // );
    // coordinator.AddComponent(
    //  mCube,
    //  ColliderComponent(
    //         ColliderType::DYNAMIC,
    //         ShapeType::AABB_3D
    //  )
    // );


    mSphere = CreateEntity();
    coordinator.AddComponent(
    	mSphere,
    	MeshComponent("./data/Objects/sphere.obj", Color(0, 255, 0))
    );
    coordinator.AddComponent(
    	mSphere,
    	Transform3DComponent(
            Vector3(0.5f, 5.f, 10.f),
            Vector3(0, 0, 0),
            Vector3(1.f, 1.f, 1.f)
    	)
    );
    coordinator.AddComponent(
	    mSphere,
	    RigidBodyComponent{}
    );
    coordinator.AddComponent(
	    mSphere,
	    ColliderComponent(
            ColliderType::DYNAMIC,
            ShapeType::SPHERE_3D
	    )
    );

		//   mZombie = CreateEntity();
		//   coordinator.AddComponent(
		//    mZombie,
		//    MeshComponent("./data/Objects/zombie.obj", Color(0, 155, 155))
		//   );
		//   coordinator.AddComponent(
		//    mZombie,
		//    Transform3DComponent(
		// Vector3(0.f, 0.f, 20.f),
		// Vector3(0, 0, 180.f),
		// Vector3(0.5f, 0.5f, 0.5f)
		//    )
		//   );
		//   // coordinator.AddComponent(
		//   // 		mZombie,
		//   // 		RigidBodyComponent{}
		//   // );
		//   coordinator.AddComponent(
		//    mZombie,
		//    ColliderComponent(
		// ColliderType::DYNAMIC,
		// coordinator.GetComponent<MeshComponent>(mZombie).mesh
		//    )
		//   );

    mFloor = CreateEntity();
    coordinator.AddComponent(
    	mFloor,
    	MeshComponent("./data/Objects/cube.obj")
    );
    coordinator.AddComponent(
    	mFloor,
    	Transform3DComponent(
	    Vector3(0.f, 10.f, 10.f),
	    Vector3(0, 0, 0),
	    Vector3(50.f, 1.f, 50.f)
    	)
    );
    coordinator.AddComponent(
	    mFloor,
	    RigidBodyComponent{
            .isStatic = true,
            .mass = 0.f 
	    }
    );
    coordinator.AddComponent(
	    mFloor,
	    ColliderComponent(
            ColliderType::STATIC,
            ShapeType::AABB_3D
	    )
    );

    mWall = CreateEntity();
    coordinator.AddComponent(
    	mWall,
    	MeshComponent("./data/Objects/cube.obj")
    );
    coordinator.AddComponent(
    	mWall,
    	Transform3DComponent(
	    Vector3(10.f, 10.f, 10.f),
	    Vector3(0, 0, 0),
	    Vector3(1.f, 10.f, 10.f)
    	)
    );
    coordinator.AddComponent(
	    mWall,
	    RigidBodyComponent{
            .isStatic = true,
            .mass = 0.f 
	    }
    );
    coordinator.AddComponent(
	    mWall,
	    ColliderComponent(
            ColliderType::STATIC,
            ShapeType::AABB_3D
	    )
    );


		//   Entity test = CreateEntity();
		//   coordinator.AddComponent(
		//    test, Transform2DComponent(
		// Vector2(400.f, 100.f),
		// 0,
		// Vector2(1.f, 1.f)
		//    )
		//   );
		//   coordinator.AddComponent(
		//    test, SpriteComponent(
		// "./data/TestData/Test.bmp", 
		// 8,
		// 4,
		// 0
		//    )
		//   );
		//   mBird = CreateEntity();
		//   coordinator.AddComponent(
		//    mBird, Transform2DComponent(
		// Vector2(600.f, 600.f),
		// 0,
		// Vector2(4.f, 4.f)
		//    )
		//   );
		//   coordinator.AddComponent(
		//    mBird, SpriteComponent(
		// "./data/Sprites/mac_the_marauder_spritesheet.png", 
		// 4,
		// 3,
		// 1
		//    )
		//   );
		//   auto& ubi_sprite = coordinator.GetComponent<SpriteComponent>(test);
		//   ubi_sprite.CreateAnimation("TEST", {24,25,26,27,28,29,30,31});
		//   ubi_sprite.Play("TEST", 15.f);
		//
		//   auto& bird_sprite = coordinator.GetComponent<SpriteComponent>(mBird);
		//   bird_sprite.CreateAnimation("TEST", {0, 1});
		//   bird_sprite.Play("TEST", 5.f);

    GameManager::Instance().AddScene("PlayerScene", std::make_unique<PlayerScene>());
    GameManager::Instance().AddScene("EnemiesScene", std::make_unique<EnemiesScene>());

}


void StartScene::Update(float deltaTime)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    // const auto& transform = coordinator.GetComponent<Transform3DComponent>(mCube);
    // utility::log(transform.GetPosition().GetString());
    // auto& transformTest = coordinator.GetComponent<Transform3DComponent>(mZombie);
    // transformTest.SetPosition(transformTest.GetPosition() + Vector3(0, 0.1f, 0));

    // auto& bird_sprite = coordinator.GetComponent<SpriteComponent>(mBird);
    // if (App::IsKeyPressed(App::KEY_P))
    // {
    //     if (bird_sprite.isPlaying)
    //     {
    //         bird_sprite.Stop();
    //     }
    //     else
    //     {
    //         bird_sprite.Play("TEST", 5.f);
    //     }
    // }
}
