#include "Render3DSystem.h"
#include "../Core/GameManager.h"
#include "../Components/MeshComponent.h"
#include "../Components/ColliderComponent.h"
#include "../Components/Transform3DComponent.h"
#include "../../../ContestAPI/AppSettings.h"
#include <string>
#include "../../config.h"


void Render3DSystem::Init()
{
	// float aspect_ratio = (float) APP_VIRTUAL_HEIGHT / (float) APP_VIRTUAL_WIDTH;
}

void Render3DSystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    // Updating renderer
	mRenderer.ClearRenderer();
    if (mCamera != nullptr)
    {
        // utility::log(mCamera->GetProjectionMatrix().GetString());
        mRenderer.SetCameraPosition(mCamera->GetTransform().GetPosition());
        mRenderer.SetProjectionMatrix(mCamera->GetProjectionMatrix());
        mRenderer.SetViewMatrix(mCamera->GetViewMatrix());
    }

	for (const auto& entity : mEntities)
	{
		auto& meshComp = coordinator.GetComponent<MeshComponent>(entity);
		auto& transform3 = coordinator.GetComponent<Transform3DComponent>(entity);
		if (meshComp.isLoaded)
		{
			mRenderer.DrawMesh(meshComp.mesh, transform3.GetWorldMatrix());

#if DEBUG_MODE
			if (coordinator.HasComponent<ColliderComponent>(entity))
			{
				const auto& collider = coordinator.GetComponent<ColliderComponent>(entity);
				mRenderer.DrawMesh(collider.meshRender, transform3.GetWorldMatrix());

				// Will draw the mesh BVH's aabbs
				if (collider.shape == ShapeType::MESH_3D)
				{
					std::vector<AABB> allBounds;
					collider.meshCollider.accelerator->GetAllBounds(allBounds);
					Mesh aabb = Geometry3D::GetDefaultPrimitiveMesh(ShapeType::AABB_3D);
					aabb.isWireframe = true;
					aabb.color = Color(0, 255, 0);

					for (int i = 0; i < allBounds.size(); i++)
					{
						Transform3DComponent transform;
						transform.SetPosition(allBounds[i].position);
						transform.SetScale(allBounds[i].size);
						mRenderer.DrawMesh(aabb, transform.GetWorldMatrix());
					}
				}
			}
#endif

		}
		else
		{
			utility::log("Entity: " + std::to_string(entity) + " mesh is not loaded!");
		}
	}

	for (int i = 0; i < mAdditionalRenders.size(); i++)
	{
		mRenderer.DrawMesh(mAdditionalRenders[i].mesh, mAdditionalRenders[i].transform.GetWorldMatrix());
	}

	mAdditionalRenders.clear();
}


void Render3DSystem::Render()
{
	mRenderer.DrawAll();
}

void Render3DSystem::DrawMesh(const Mesh& mesh, const Transform3DComponent& transform)
{
	mAdditionalRenders.push_back(MeshRender{mesh, transform});
	// mRenderer.DrawMesh(mesh, transform.GetWorldMatrix());
}

void Render3DSystem::SetCamera(Camera* camera)
{
    mCamera = camera;
}

