#include "TextSystem.h"
#include "../Core/GameManager.h"
#include "../Components/TextComponent.h"
#include "../Components/Transform2DComponent.h"
#include "../../../ContestAPI/app.h"


void TextSystem::Update(float deltaTime)
{

}

void TextSystem::Render()
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    for (const auto& ent : mEntities)
    {
        const auto& text = coordinator.GetComponent<TextComponent>(ent);
        const auto& transform = coordinator.GetComponent<Transform2DComponent>(ent);

        // Using API, no scaling sadly or rotation
        
        Vector2 pos = transform.GetPosition();
        App::Print(pos.x, pos.y, text.text.c_str(), text.color.GetPercentageRed(), text.color.GetPercentageGreen(), text.color.GetPercentageBlue());
    }
}
