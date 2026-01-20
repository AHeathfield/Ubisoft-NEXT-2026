#pragma once

#include "ECS.h"
#include <string>
#include <vector>
#include <unordered_set>

/*
 * A Scene is like a collection of entities.
 * - It will need to first initialize all entities
 * - It will also need to delete all entities when switching Scenes
 */
class Scene
{
public:
    virtual ~Scene() = default; // So we can delete the reference
    virtual void Enter() = 0;   // = 0 means has to be implemented
    virtual void Exit();

    virtual void Update(const float deltaTime) {}

    Entity CreateEntity();
    void DestroyEntity(Entity ent);

    bool IsActive();
    void SetActive(const bool active);


    std::unordered_set<Entity> sceneEntities;

private:
    bool mActive = true;
};
