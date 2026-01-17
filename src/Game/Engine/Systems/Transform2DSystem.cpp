#include "Transform2DSystem.h"
#include "../Core/GameManager.h"
#include "../Components/Transform2DComponent.h"


void Transform2DSystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	for (const auto& entity : mEntities)
	{
		auto& transform = coordinator.GetComponent<Transform2DComponent>(entity);
		transform.SetChanged(false);
	}
}
