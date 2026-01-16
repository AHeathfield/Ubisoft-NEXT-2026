#include "PlayerScene.h"
#include "../../../Engine/Core/GameManager.h"
#include "../../../Engine/Systems/Render3DSystem.h"
#include "../../../../ContestAPI/app.h"
#include "../../../Engine/Components/Transform3DComponent.h"
#include "../../../Engine/Components/ColliderComponent.h"
#include "../../../Engine/Components/RigidbodyComponent.h"
#include "../../../Engine/Components/AudioComponent.h"
#include "../../Components/TestComponent.h"
#include "../../Systems/TestSystem.h"
#include <memory>
#include <string>
// #include "../../../Engine/src/Components/MeshComponent.h"


void PlayerScene::Enter()
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	// THIS IS TEMPORARY JUST FOR TESTING
	coordinator.RegisterComponent<TestComponent>();

	auto testSystem = coordinator.RegisterSystem<TestSystem>();
	{
		Signature signature;
		signature.set(coordinator.GetComponentType<TestComponent>());
		coordinator.SetSystemSignature<TestSystem>(signature);
	}
	GameManager::Instance().AddSystem(testSystem);


	mPlayer = CreateEntity();
	coordinator.AddComponent(mPlayer, TestComponent{});
	coordinator.AddComponent(
		mPlayer,
		Transform3DComponent()
	);
	// NEED to adjust camera so it relies on the transform and effects the transform, probably should make a component but maybe not...
	coordinator.AddComponent(
			mPlayer, 
			RigidbodyComponent{}
	);
	coordinator.AddComponent(
			mPlayer,
			ColliderComponent(
				ColliderType::DYNAMIC,
				ShapeType::OBB_3D,
				[this](Entity self, Entity other, const CollisionManifold& collision) {
			        this->onCollisionEnter(self, other, collision);
				},
				[this](Entity self, Entity other, const CollisionManifold& collision) {
			        this->onCollisionStay(self, other, collision);
				},
				[this](Entity self, Entity other) {
			        this->onCollisionExit(self, other);
				}
			)
	);
	coordinator.AddComponent(
			mPlayer, AudioComponent{}
	);

	auto& audio = coordinator.GetComponent<AudioComponent>(mPlayer);
	audio.AddAudio("Music", "./Data/SFX/ChuckNorris_BG.mp3");
	audio.AddAudio("Shoot", "./Data/SFX/ak47.mp3");
	audio.Play("Music", true);



	// Creating the camera instance
	mCamera = std::make_unique<Camera>();

	// Initial prev mouse pos to middle (I think the virtual stuff through it off, ideally there is a way to remove the cursor lol)
	mPrevMousePos.x = APP_VIRTUAL_WIDTH / 2.f;
	mPrevMousePos.y = APP_VIRTUAL_HEIGHT / 2.f;
	// APP_VIRTUAL_TO_NATIVE_COORDS(mPrevMousePos.x, mPrevMousePos.y)

	// Setting the render camera to the player camera
	coordinator.GetSystem<Render3DSystem>()->SetRenderCamera(mCamera.get());


}

void PlayerScene::Update(const float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	Vector2 newMousePos, mouseMovement;
	App::GetMousePos(newMousePos.x, newMousePos.y);
	mouseMovement = newMousePos - mPrevMousePos;

	// If theres no cursor movement the coordinates are 0, 0
	// I want the default coordinates to be the center of the screen this just ensures that
	if (mCursorHasBeenMoved)
	{
		mPrevMousePos = newMousePos;
	}
	else
	{
		if (newMousePos.equals(Vector2(0,0)) == false)
		{
			mCursorHasBeenMoved = true;
		}
		mouseMovement.zero();
	}


	// If I wanted to implement something like like a lean in siege thats when you use z "roll"
	mCamera->rotation.x += mouseMovement.x * _SENS * deltaTime;
	mCamera->rotation.y += mouseMovement.y * _SENS * deltaTime;

	if (App::IsKeyPressed(App::KEY_A))
	    mCamera->MoveLeft(_SPEED * deltaTime);
	if (App::IsKeyPressed(App::KEY_D))
	    mCamera->MoveRight(_SPEED * deltaTime);
	if (App::IsKeyPressed(App::KEY_W))
		mCamera->MoveForward(_SPEED * deltaTime);
	if (App::IsKeyPressed(App::KEY_S))
		mCamera->MoveBackward(_SPEED * deltaTime);

	// For testing with flying
	if (App::IsKeyPressed(App::KEY_SPACE) || App::IsKeyPressed(App::KEY_E))
		mCamera->position.y -= _SPEED * deltaTime;
	if (App::IsKeyPressed(App::KEY_Q))
		mCamera->position.y += _SPEED * deltaTime;

	// Sound test
	auto& audio = coordinator.GetComponent<AudioComponent>(mPlayer);
	if (App::IsMousePressed(GLUT_LEFT_BUTTON))
		audio.Restart("Shoot");


	// Updating the transform
	// TODO: actually check if it was changed
	auto& transform = coordinator.GetComponent<Transform3DComponent>(mPlayer);
	transform.SetPosition(mCamera->position);
	transform.SetRotationRAD(mCamera->rotation);
}


// void PlayerScene::Render()
// {
// 	// const auto& collider = coordinator.GetComponent<ColliderComponent>(mPlayer);
//
// 	if (mIsColliding)
// 	{
// 		App::Print(100, 100, "Player is colliding!");
// 	}
// }


void PlayerScene::onCollisionEnter(Entity self, Entity other, const CollisionManifold& collision)
{
	utility::log("Enter");
	mIsColliding = true;
}


void PlayerScene::onCollisionStay(Entity self, Entity other, const CollisionManifold& collision)
{
	utility::log("Stay");
}


void PlayerScene::onCollisionExit(Entity self, Entity other)
{
	utility::log("Exit");
	mIsColliding = false;
}
