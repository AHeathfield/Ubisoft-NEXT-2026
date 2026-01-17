#include "SpriteSystem.h"
#include "../Core/GameManager.h"
#include "../Components/SpriteComponent.h"
#include "../Components/Transform2DComponent.h"


void SpriteSystem::OnEntityAdded(Entity entity)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    if (coordinator.HasComponent<SpriteComponent>(entity))
    {
        const auto& sprite_comp = coordinator.GetComponent<SpriteComponent>(entity);
        mRenderLayers[sprite_comp.renderLayer].insert(entity);
    }
}


void SpriteSystem::OnEntityRemoved(Entity entity)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    if (coordinator.HasComponent<SpriteComponent>(entity))
    {
        const auto& sprite_comp = coordinator.GetComponent<SpriteComponent>(entity);
        mRenderLayers[sprite_comp.renderLayer].erase(entity);
    }
}

void SpriteSystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	for (const auto& entity : mEntities)
	{
		auto& sprite_comp = coordinator.GetComponent<SpriteComponent>(entity);

		if (sprite_comp.isHidden)
			continue;

		// Updating transform
		const auto& transform = coordinator.GetComponent<Transform2DComponent>(entity);
		if (transform.IsChanged())
		{
			// Just using the x since API only handles uniform scaling
			sprite_comp.sprite->SetScale(transform.GetScale().x);

			Vector2 pos = transform.GetPosition();
			sprite_comp.sprite->SetPosition(pos.x, pos.y);

			// TODO: Check if API uses RAD or DEG
			sprite_comp.sprite->SetAngle(transform.GetRotationRAD());
		}

		// Updating animation
		if (sprite_comp.isPlaying)
		{
			// This is how I prefer to adjust animation speed
			sprite_comp.sprite->Update(deltaTime * 1000 * sprite_comp.animationSpeed);
		}
	}
}


void SpriteSystem::Render()
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	// Draws sprites
	for (auto i = mRenderLayers.begin(); i != mRenderLayers.end(); i++)
	{
		for (auto it = i->second.begin(); it != i->second.end(); it++)
		{
			Entity entity = *it;
			const auto& sprite_comp = coordinator.GetComponent<SpriteComponent>(entity);

			if (sprite_comp.isHidden)
				continue;

			sprite_comp.sprite->Draw();
		}
	}
}

