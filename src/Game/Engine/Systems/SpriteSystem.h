#pragma once

#include "../Core/ECS.h"
#include <map>
#include <unordered_set>

// Runs animations that are suppose to run
// Draws animation in correct positions
class SpriteSystem : public System
{
public:
    void Update(float deltaTime) override;
    void Render() override;
    void OnEntityAdded(Entity entity) override;
    void OnEntityRemoved(Entity entity) override;

private:
    // To draw similarily to painters algorithm
    // key represents the layer lower are drawn first
    // value is sprites on that layer
    // Using map because they are sorted from lowest - greatest which is what I want
    std::map<int, std::unordered_set<Entity>> mRenderLayers;
};
