#include "DeathScene.h"
#include "StartScene.h"
#include "../../Engine/Core/GameManager.h"
#include "../../Engine/Components/Transform2DComponent.h"
#include "../../Engine/Components/TextComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Systems/CollisionSystem.h"

void DeathScene::Enter()
{
    // May need to remove the other scenes here
    GameManager& gm = GameManager::Instance();
    Coordinator& coordinator = gm.GetCoordinator();

    // Removing Player, Enemy, and Start
    // gm.RemoveScene("PlayerScene");
    gm.RemoveScene("EnemiesScene");
    gm.RemoveScene("StartScene");

    // TODO: Add a time display here

    mText = CreateEntity();
	coordinator.AddComponent(
			mText, TextComponent("YOU LOSE", Color(255, 255, 255))
	);
	coordinator.AddComponent(
			mText, Transform2DComponent(
	                   Vector2(500.f, 500.f),
	                   0.f,
	                   Vector2(1.f, 1.f)
	               )
	);
}


void DeathScene::Update(float deltaTime)
{
    // if (GameManager::Instance().IsSceneInStack("PlayerScene"))
    // {
    //     GameManager::Instance().RemoveScene("PlayerScene");
    // }
    if (App::IsKeyPressed(App::KEY_ENTER))
    {
        GameManager::Instance().GetCoordinator().GetSystem<CollisionSystem>()->Init();
        GameManager::Instance().AddScene("StartScene", std::make_unique<StartScene>());
        // GameManager::Instance().RemoveScene("DeathScene");
    }
    utility::log("DEATH");
}

