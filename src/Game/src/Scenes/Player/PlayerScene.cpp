#include "PlayerScene.h"
#include "../../../Engine/Core/GameManager.h"
#include "../../../Engine/Systems/Render3DSystem.h"
#include "../../../../ContestAPI/app.h"
#include "../../../Engine/Components/Transform3DComponent.h"
#include "../../../Engine/Components/ColliderComponent.h"
#include "../../../Engine/Components/TimerComponent.h"
#include "../../../Engine/Components/MeshComponent.h"
// #include "../../../Engine/Components/CameraComponent.h"
#include "../../../Engine/Components/RigidBodyComponent.h"
#include "../../../Engine/Components/CharacterBodyComponent.h"
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
		Transform3DComponent(
            Vector3(0, 3.f, 5.f),
            Vector3(),
            Vector3(1.f, 1.f, 1.f)
        )
	);
	coordinator.AddComponent(
			mPlayer,
			ColliderComponent(
				ColliderType::DYNAMIC,
				ShapeType::AABB_3D,
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
			mPlayer, CharacterBodyComponent()
	);
	coordinator.AddComponent(
			mPlayer, AudioComponent{}
	);
	auto& audio = coordinator.GetComponent<AudioComponent>(mPlayer);
	audio.AddAudio("Music", "./Data/SFX/ChuckNorris_BG.mp3");
	audio.AddAudio("Shoot", "./Data/SFX/ak47.mp3");
	audio.Play("Music", true);



	// Creating the camera instance
	mCamera = std::make_shared<Camera>();
    // mCamera->UpdateProjection();

	// Initial prev mouse pos to middle (I think the virtual stuff through it off, ideally there is a way to remove the cursor lol)
	// mPrevMousePos.x = APP_VIRTUAL_WIDTH / 2.f;
	// mPrevMousePos.y = APP_VIRTUAL_HEIGHT / 2.f;
	// APP_VIRTUAL_TO_NATIVE_COORDS(mPrevMousePos.x, mPrevMousePos.y)

	// Setting the render camera to the player camera
	coordinator.GetSystem<Render3DSystem>()->SetCamera(mCamera);


    // Setting the timers
    mThrowCooldown = Timer(false, _THROW_COOLDOWN);
}


void PlayerScene::Update(const float deltaTime)
{
    mMovedXZ = false;
    mInputDir = Vector3(0.f, 0.f, 0.f);

    // Updating timers
    mThrowCooldown.Increment(deltaTime);

	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	auto& transform = coordinator.GetComponent<Transform3DComponent>(mPlayer);
	auto& character = coordinator.GetComponent<CharacterBodyComponent>(mPlayer);
	const auto& coll = coordinator.GetComponent<ColliderComponent>(mPlayer);
    // utility::log("Pos: " + transform.GetPosition().GetString());


    // Applying custom gravity
    if (!character.isOnFloor)
        character.velocity = character.upDirection *  -1.f * _GRAVITY;

	// Vector2 newMousePos, mouseMovement;
	// App::GetMousePos(newMousePos.x, newMousePos.y);
	// mouseMovement = newMousePos - mPrevMousePos;

	// If theres no cursor movement the coordinates are 0, 0
	// I want the default coordinates to be the center of the screen this just ensures that
	// if (mCursorHasBeenMoved)
	// {
	// 	mPrevMousePos = newMousePos;
	// }
	// else
	// {
	// 	if (newMousePos.equals(Vector2(0,0)) == false)
	// 	{
	// 		mCursorHasBeenMoved = true;
	// 	}
	// 	mouseMovement.zero();
	// }



    // Camera controls
    Vector3 rotation = transform.GetRotationRAD();

    if (App::IsKeyPressed(App::KEY_LEFT))
        rotation.x += 1.f * _SENS * deltaTime;
	if (App::IsKeyPressed(App::KEY_RIGHT))
        rotation.x += -1.f * _SENS * deltaTime;
	if (App::IsKeyPressed(App::KEY_UP))
        rotation.y += -1.f * _SENS * deltaTime;
	if (App::IsKeyPressed(App::KEY_DOWN))
        rotation.y += 1.f * _SENS * deltaTime;


    // Clamp pitch
    const float maxPitch = utility::degToRad(89.0f);
    rotation.y = std::clamp(rotation.y, -maxPitch, maxPitch);

    transform.SetRotationRAD(rotation);
    mCamera->SetTransform(transform);



	// If I wanted to implement something like like a lean in siege thats when you use z "roll"
	// rotation.x += mouseMovement.x * _SENS * deltaTime;
	// rotation.y += mouseMovement.y * _SENS * deltaTime;
	//    transform.SetRotationRAD(rotation);
    // mCamera->SetTransform(transform);

    // Getting the input direction
	if (App::IsKeyPressed(App::KEY_A))
        moveLeft();
	if (App::IsKeyPressed(App::KEY_D))
        moveRight();
	if (App::IsKeyPressed(App::KEY_W))
        moveForward();
	if (App::IsKeyPressed(App::KEY_S))
        moveBackward();

    if (mMovedXZ && mInputDir.GetMagnitudeSquared() > 0.f)
    {
        Vector3 newVel = mInputDir.GetNormal() * _SPEED;
        character.velocity.x = newVel.x;
        character.velocity.z = newVel.z;
    }
    else
        moveToRest(character);

    // Jump
    if (App::IsKeyPressed(App::KEY_V) && character.isOnFloor)
        jump(character, _JUMP);


	// For testing with flying
	if (App::IsKeyPressed(App::KEY_E))
        moveUp(transform, _SPEED * deltaTime);
	if (App::IsKeyPressed(App::KEY_Q))
        moveDown(transform, _SPEED * deltaTime);
    if (App::IsKeyPressed(App::KEY_O))
        transform.SetPosition(Vector3(0.f, 0.f, 0.f));

	// Sound test
	auto& audio = coordinator.GetComponent<AudioComponent>(mPlayer);
	if (App::IsMousePressed(GLUT_LEFT_BUTTON) || App::IsKeyPressed(App::KEY_SPACE))
    {
        if (mThrowCooldown.IsFinished())
        {
            audio.Restart("Shoot");
            throwBall();
            mThrowCooldown.Reset();
        }
    }


    // Updating the Camera
    // utility::log(transform.GetPosition().GetString());
    mCamera->SetTransform(transform);
    mCamera->UpdateView();
}


Vector3 PlayerScene::GetPlayerPosition() const
{
    return GameManager::Instance().GetCoordinator().GetComponent<Transform3DComponent>(mPlayer).GetPosition();
}



void PlayerScene::onCollisionEnter(Entity self, Entity other, const CollisionManifold& collision)
{
	utility::log("Enter");
	mIsColliding = true;
}


void PlayerScene::onCollisionStay(Entity self, Entity other, const CollisionManifold& collision)
{
    // utility::log("Stay");
}


void PlayerScene::onCollisionExit(Entity self, Entity other)
{
	utility::log("Exit");
	mIsColliding = false;
}


void PlayerScene::moveLeft()
{
    Vector3 camera_look = mCamera->GetLookDirection().GetNormal();
    camera_look.y = 0.f;

    // The cross of the direction facing and the world DOWN should be the right dir relative to the facing dir
    Vector3 local_left = camera_look.GetCrossProduct(Vector3::DOWN);
    mInputDir += local_left;
    mMovedXZ = true;
}


void PlayerScene::moveRight()
{
    Vector3 camera_look = mCamera->GetLookDirection().GetNormal();
    camera_look.y = 0.f;

    // The cross of the direction facing and the world UP should be the right dir relative to the facing dir
    Vector3 local_right = camera_look.GetCrossProduct(Vector3::UP);
    mInputDir += local_right;
    mMovedXZ = true;

}


void PlayerScene::moveForward()
{
    // The cross of the direction
    Vector3 camera_look = mCamera->GetLookDirection().GetNormal();
    camera_look.y = 0.f;

    mInputDir += camera_look;
    mMovedXZ = true;
}
void PlayerScene::moveBackward()
{
    Vector3 camera_look = mCamera->GetLookDirection().GetNormal();
    camera_look.y = 0.f;

    mInputDir += camera_look * -1.f;// * speed;
    mMovedXZ = true;
}


void PlayerScene::moveToRest(CharacterBodyComponent& character)
{
    character.velocity.x = utility::moveToward(character.velocity.x, 0, 1.5f);
    character.velocity.z = utility::moveToward(character.velocity.z, 0, 1.5f);
}


void PlayerScene::jump(CharacterBodyComponent& character, const float speed)
{
    character.velocity = character.upDirection * speed;
    character.justJumped = true;
}


// These are flying controls so they will just straight up change pos
void PlayerScene::moveDown(Transform3DComponent& transform, const float speed)
{
    Vector3 pos = transform.GetPosition();
    pos.y += speed;
    transform.SetPosition(pos);
}


void PlayerScene::moveUp(Transform3DComponent& transform, const float speed)
{
    Vector3 pos = transform.GetPosition();
    pos.y -= speed;
    transform.SetPosition(pos);
}


void PlayerScene::throwBall()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    const auto& playerTransform = coordinator.GetComponent<Transform3DComponent>(mPlayer);

    Entity ball = CreateEntity();
    coordinator.AddComponent(
    	ball,
    	MeshComponent("./data/Objects/sphere.obj", Color(0, 255, 0))
    );
    coordinator.AddComponent(
    	ball,
    	Transform3DComponent(
            playerTransform.GetPosition(),
            Vector3(0, 0, 0),
            Vector3(0.5f, 0.5f, 0.5f)
    	)
    );
    coordinator.AddComponent(
	    ball,
	    RigidBodyComponent{.mass = 1.f}
    );
    coordinator.AddComponent(
	    ball,
	    TimerComponent{
            .timer = Timer(false, 1.f),
            .isLooping = false,
            .onTimerEnd = [this](Entity self) {
                    this->deleteBall(self);
				}
        }
    );
    coordinator.AddComponent(
	    ball,
	    ColliderComponent(
            ColliderType::DYNAMIC,
            ShapeType::SPHERE_3D
	    )
    );

    auto& rb = coordinator.GetComponent<RigidBodyComponent>(ball);
    auto& tc = coordinator.GetComponent<TimerComponent>(ball);

    // Just ensuring the timer gets resetted
    // tc.timer = Timer(false, 1.f);
    // tc.onTimerEnd = [this](Entity self) {
    //     this->deleteBall(self);
    // };

    rb.velocity = mCamera->GetLookDirection().GetNormal() * _THROW_STRENGTH;
}


void PlayerScene::deleteBall(Entity self)
{
    GameManager::Instance().GetCoordinator().DestroyEntity(self);
}

