#pragma once

//#include <SDL3/SDL_render.h>
//#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../Graphics/GraphicsRenderer.h"
#include "../Core/ECS.h"


// Handles rendering 3D meshes
class Render3DSystem : public System
{

public:
    void Init() override;
    void Update(float deltaTime) override;

    void Render() override;
    void DrawMesh(const Mesh& mesh, const Transform3DComponent& transform);
    void SetRenderCamera(Camera* camera);

private:
    struct MeshRender
    {
        Mesh mesh;
        Transform3DComponent transform;
    };

    GraphicsRenderer mRenderer;
    std::vector<MeshRender> mAdditionalRenders;
};
