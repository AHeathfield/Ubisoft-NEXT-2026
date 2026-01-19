#pragma once

#include "../../../Engine/Core/Scene.h"
#include "../../../Engine/Math/Vector2.h"
#include "../../../Engine/Physics/CollisionManifold.h"
#include "../../../Engine/Components/CharacterBodyComponent.h"
#include "../../../Engine/Camera/Camera.h"
#include <memory>

class PlayerScene : public Scene
{
public:
    void Enter() override;
    // void Exit() override;
    void Update(const float deltaTime) override;
    // void Render() override;

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

private:
    static constexpr float _SPEED = 8.0f;
    static constexpr float _JUMP = 200.f;
    static constexpr float _GRAVITY = 10.f;
    static constexpr float _SENS = 0.2f;
    static constexpr float _EPS = 0.00000001f;

private:
    Entity mPlayer = -1;
    std::shared_ptr<Camera> mCamera = nullptr;
    Vector2 mPrevMousePos = Vector2();

    Vector3 mInputDir = Vector3();
    bool mCursorHasBeenMoved = false;
    bool mIsColliding = false;
    bool mMovedXZ = false;
};
