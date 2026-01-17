#pragma once

#include "../../../Engine/Core/Scene.h"
#include "../../../Engine/Math/Vector2.h"
#include "../../../Engine/Physics/CollisionManifold.h"
#include "../../../Engine/Camera/Camera.h"
#include <memory>

class PlayerScene : public Scene
{
public:
    void Enter() override;
    //void Exit() override;
    void Update(const float deltaTime) override;
    // void Render() override;

private:
    void onCollisionEnter(Entity self, Entity other, const CollisionManifold& collision);
    void onCollisionStay(Entity self, Entity other, const CollisionManifold& collision);
    void onCollisionExit(Entity self, Entity other);

    void moveLeft(Transform3DComponent& transform, const float speed);
    void moveRight(Transform3DComponent& transform, const float speed);
    void moveForward(Transform3DComponent& transform, const float speed);
    void moveBackward(Transform3DComponent& transform, const float speed);
    void moveDown(Transform3DComponent& transform, const float speed);
    void moveUp(Transform3DComponent& transform, const float speed);

private:
    static constexpr float _SPEED = 8.0f;
    static constexpr float _SENS = 0.2f;

private:
    Entity mPlayer;
    std::unique_ptr<Camera> mCamera;
    Vector2 mPrevMousePos;
    bool mCursorHasBeenMoved = false;
    bool mIsColliding = false;
};
