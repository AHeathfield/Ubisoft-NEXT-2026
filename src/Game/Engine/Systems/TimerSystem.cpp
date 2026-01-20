#include "TimerSystem.h"
#include "../Components/TimerComponent.h"
#include "../Core/GameManager.h"
#include <string>
#include <unordered_set>

void TimerSystem::Update(float deltaTime)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    // utility::log("Timer system worked!");

    std::set<Entity> temp = mEntities;
    for (const auto& entity : temp)
    {
        auto& timer = coordinator.GetComponent<TimerComponent>(entity);

        timer.timer.Increment(deltaTime);

        if (timer.timer.IsFinished())
        {
            if (timer.onTimerEnd)
            {
                timer.onTimerEnd(entity);
            }

            if (coordinator.IsEntityDestroyed(entity))
                continue;

            if (timer.isLooping)
            {
                timer.timer.Reset();
            }
            else
            {
                // Remove timer
                coordinator.RemoveComponent<TimerComponent>(entity);
            }
        }
    }

}
