#pragma once

#include "../../../Engine/Core/Scene.h"
#include "../../../Engine/Math/Vector2.h"
#include "../../../Engine/Physics/CollisionManifold.h"
#include "../../../Engine/Components/CharacterBodyComponent.h"
#include "../../../Engine/Camera/Camera.h"
#include "../../../Engine/Core/Timer.h"
#include <memory>

class PlayerScene : public Scene
{
public:
    void Enter() override;
    // void Exit() override;
    void Update(const float deltaTime) override;
    // void Render() override;
    
    Vector3 GetPlayerPosition() const;

private:
    void onCollisionEnter(Entity self, Entity other, const CollisionManifold& collision);
    void onCollisionStay(Entity self, Entity other, const CollisionManifold& collision);
    void onCollisionExit(Entity self, Entity other);

    void moveLeft();
    void moveRight();
    void moveForward();
    void moveBackward();
    void moveToRest(CharacterBodyComponent& character);
    void jump(CharacterBodyComponent& character, const float speed);

    // Flying
    void moveDown(Transform3DComponent& transform, const float speed);
    void moveUp(Transform3DComponent& transform, const float speed);

    // Attacks
    void throwBall();
    void deleteBall(Entity self);

private:
    static constexpr float _SPEED = 8.0f;
    static constexpr float _JUMP = 200.f;
    static constexpr float _GRAVITY = 15.f;
    static constexpr float _SENS = 3.f;
    static constexpr float _EPS = 0.00000001f;
    static constexpr float _THROW_STRENGTH = 100.f;
    static constexpr float _THROW_COOLDOWN = 0.1f;

private:
    Timer mThrowCooldown;
    Entity mPlayer = -1;
    std::shared_ptr<Camera> mCamera = nullptr;
    // Vector2 mPrevMousePos = Vector2();

    Vector3 mInputDir = Vector3();
    // Vector3 mInputDirCam = Vector3();
    // bool mCursorHasBeenMoved = false;
    bool mMovedXZ = false;
};
