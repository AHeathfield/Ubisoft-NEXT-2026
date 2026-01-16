#include "Transform3DSystem.h"
#include "../Core/GameManager.h"
#include "../Components/Transform3DComponent.h"


void Transform3DSystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	for (const auto& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);
		transform.wasChanged = false;
	}
}
