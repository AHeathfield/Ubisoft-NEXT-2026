#include "Transform3DSystem.h"
#include "../Core/GameManager.h"
#include "../Components/Transform3DComponent.h"
#include <cstdint>
#include <string>

// void Transform3DSystem::OnEntityAdded(Entity entity)
// {
//     // utility::log("Transform: " + std::to_string(entity));
// }

void Transform3DSystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	for (const auto& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);
		transform.SetChanged(false);
	}
}
