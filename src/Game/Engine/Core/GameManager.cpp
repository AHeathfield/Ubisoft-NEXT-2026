#include "GameManager.h"
#include <memory>
#include <string>
#include <algorithm>

#include "../Components/MeshComponent.h"
#include "../Components/Transform3DComponent.h"
#include "../Components/Transform2DComponent.h"
#include "../Components/ColliderComponent.h"
#include "../Components/TextComponent.h"
#include "../Components/TimerComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AudioComponent.h"
#include "../Components/CharacterBodyComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Systems/Render3DSystem.h"
#include "../Systems/TimerSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/TextSystem.h"
#include "../Systems/CharacterMovementSystem.h"
// #include "../Systems/CharacterSnapSystem.h"
#include "../Systems/RigidBodySystem.h"
#include "../Systems/Transform3DSystem.h"
#include "../Systems/Transform2DSystem.h"
#include "../Systems/SpriteSystem.h"
#include "Utility.h"

// Singleton
GameManager& GameManager::Instance()
{
    static GameManager s_instance;
    return s_instance;
}

// // For setting up initial things
// GameManager::GameManager()
// {
//     // Registering Components
// }

void GameManager::Start()
{
    mCoordinator.RegisterComponent<TimerComponent>();
    mCoordinator.RegisterComponent<MeshComponent>();
    mCoordinator.RegisterComponent<SpriteComponent>();
    mCoordinator.RegisterComponent<TextComponent>();
    mCoordinator.RegisterComponent<AudioComponent>();
    mCoordinator.RegisterComponent<Transform3DComponent>();
    mCoordinator.RegisterComponent<Transform2DComponent>();
    mCoordinator.RegisterComponent<ColliderComponent>();
    mCoordinator.RegisterComponent<RigidBodyComponent>();
    mCoordinator.RegisterComponent<CharacterBodyComponent>();

    setUpSystems();
}


void GameManager::Close()
{
    // The order shouldn't matter for closing them
    for (auto itr = mScenes.begin(); itr != mScenes.end(); itr++)
    {
        itr->second->Exit();
    }

    for (int i = mSystemsStack.size() - 1; i >= 0; i--)
    {
        mSystemsStack[i]->Close();
    }

    mScenes.clear();
    mScenesStack.clear();
    mSystemsStack.clear();
}



void GameManager::Update(float deltaTime)
{
    utility::log("TEST0");

    // Updating the scenes (reverse order since its a stack)
    for (int i = (int)mScenesStack.size() - 1; i >= 0; i--)
    {
        const std::string& scene_name = mScenesStack[i];

        auto it = mScenes.find(scene_name);                // FIXED
        if (it != mScenes.end() && it->second && it->second->IsActive())
        {
            it->second->Update(deltaTime / 1000.0f);
        }
    }

    utility::log("TEST1");

    // Updating the active Systems in correct order
    for (int i = (int)mSystemsStack.size() - 1; i >= 0; i--)
    {
        utility::log("Testing sys: " + std::to_string(i));
        if (mSystemsStack[i]->IsActive())
        {
            mSystemsStack[i]->Update(deltaTime / 1000.0f);
        }
    }

    utility::log("TEST2");

    // Removing scenes queued for removal
    utility::log(std::to_string(mScenesToBeRemoved.size()));
    for (int i = (int)mScenesToBeRemoved.size() - 1; i >= 0; i--)
    {
        utility::log("BYE");
        const std::string& name = mScenesToBeRemoved[i];

        auto it = mScenes.find(name);                       // FIXED
        if (it != mScenes.end() && it->second)
        {
            utility::log("HII");
            it->second->Exit();
            mScenes.erase(it);
            utility::vectorRemoveElement(mScenesStack, name);
            utility::log("Succesfully Removed: " + name);
        }
    }

    mScenesToBeRemoved.clear();
    utility::log("TEST3");
}


// Should Render 3D then 2D
void GameManager::Render()
{
    // Updating systems
    for (int i = mSystemsStack.size() - 1; i >= 0; i--)
    {
        if (mSystemsStack[i]->IsActive())
        {
            mSystemsStack[i]->Render();
        }
    }
}


Coordinator& GameManager::GetCoordinator()
{
    return mCoordinator;
}


void GameManager::DestroyEntity(Entity entity)
{
    for (int i = (int)mScenesStack.size() - 1; i >= 0; i--)
    {
        const std::string& scene_name = mScenesStack[i];

        auto it = mScenes.find(scene_name);                // FIXED
        if (it != mScenes.end() && it->second && it->second->IsActive())
        {
            it->second->DestroyEntity(entity);
        }
    }
}

// Maybe change so they can add the system and it just calls multiple times?? dont really see a use case in that though
void GameManager::AddSystem(std::shared_ptr<System> system, int stackPos)
{
    if (utility::containsValue(mSystemsStack, system) == false)
    {
        if (stackPos >= mSystemsStack.size() || stackPos < 0)
        {
            mSystemsStack.push_back(system);
        }
        else
        {
            mSystemsStack.insert(mSystemsStack.begin() + stackPos, system);
        }
    }
    else
    {
        utility::log("ERROR: System cannot be added since it is already in the stack");
    }
}


// Returns -1 if not in stack
int GameManager::GetSystemStackIndex(std::shared_ptr<System> system) const
{
    return utility::vectorGetIndexOfValue(mSystemsStack, system);
}


void GameManager::ChangeSystemPosition(std::shared_ptr<System> system, int newStackPos)
{
    int currentIndex = GetSystemStackIndex(system);

    if (currentIndex == -1)
    {
        utility::log("ERROR: Cannot change system stack posiiton since it is not in stack.");
        return;
    }

    utility::vectorMoveElement(mSystemsStack, currentIndex,  newStackPos);
}


void GameManager::RemoveSystem(std::shared_ptr<System> system)
{
    utility::vectorRemoveElement(mSystemsStack, system);
}


// Returns -1 if not in stack
int GameManager::GetSceneStackIndex(const std::string& name) const
{
    return utility::vectorGetIndexOfValue(mScenesStack, name);
}

void GameManager::ChangeScenePosition(const std::string& name, int newStackPos)
{
    // Check to see if scene is registered
    if (mScenes.find(name) != mScenes.end())
    {
        int currentIndex = GetSceneStackIndex(name);

        // If the scene is part of the stack already
        if (currentIndex != -1)
        {
            utility::vectorMoveElement(mScenesStack, currentIndex,  newStackPos);
        }
        // Adds it to the correct position
        else
        {
            addSceneToStack(name, newStackPos);
        }
    }
    else
    {
        std::string log = "ERROR: Cannot change scene stack position since the scene " + name + " is not registered."; 
        utility::log(log);
    }
}



Scene* GameManager::GetScene(const std::string& name)
{
    auto it = mScenes.find(name);                          // FIXED
    if (it != mScenes.end() && it->second)
    {
        return it->second.get();
    }
    return nullptr;
}

bool GameManager::IsSceneInStack(const std::string& name) const
{
    if(std::find(mScenesStack.begin(), mScenesStack.end(), name) != mScenesStack.end())
    {
        return true;
    }
    return false;
}


// NOTE: You cannot add a scene thats name already is taken
void GameManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene, int stackPos)
{
    if (utility::hasKey(mScenes, name) == false)
    {
        // This gives the map ownership of the unique ptr
        mScenes[name] = std::move(scene);
        mScenes[name]->Enter();

        auto& sceneReal = mScenes[name];
        addSceneToStack(name, stackPos);
    }
    else
    {
        std::string log = "Can't add " + name + " scene, the name is already taken. Use another name.\n";
        utility::log(log);
    }
}


void GameManager::RemoveScene(const std::string& name)
{
    if (utility::hasKey(mScenes, name))
    {
        mScenesToBeRemoved.push_back(name);
    }
    else
    {
        utility::log("Can't remove " + name + " because it is not in the stack.");
    }

}


void GameManager::PauseScene(const std::string& name)
{
    auto it = mScenes.find(name);                          // FIXED
    if (it != mScenes.end() && it->second)
    {
        it->second->SetActive(false);
    }
    else
    {
        utility::log("Can't pause " + name + " because it is not in the stack.");
    }
}



bool GameManager::IsSceneActive(const std::string& name)
{
    auto it = mScenes.find(name);                          // FIXED
    if (it != mScenes.end() && it->second)
    {
        return it->second->IsActive();
    }
    return false;
}

bool GameManager::IsScenePaused(const std::string& name)
{
    return !IsSceneActive(name);
}


// void GameManager::ActivateSystem(const std::string& name)
// {
//     if (mSystemsStack.find(name) != mSystems.end())
//     {
//         // Adding to the count of states using this system
//         // mSystemsStackUsage[name]++;
//         mSystemsStack[name]->isActive = true;
//     }
//     else
//     {
//         //SDL_Log("The system %s is not registered or setup or exists", name.c_str());
//     }
// }
//
//
// void GameManager::DeactivateSystem(const std::string& name)
// {
//     if (mSystemsStack.find(name) != mSystems.end())
//     {
//         // Adding to the count of states using this system
//         // mSystemsStackUsage[name]--;
//         // if (mSystemsStackUsage[name] < 0)
//         // {
//         //     mSystemsStackUsage[name] = 0;
//         // }
//         mSystemsStack[name]->isActive = false;
//     }
//     else
//     {
//         //SDL_Log("The system %s is not registered or setup or exists", name.c_str());
//     }
// }


// Private Methods ==========================================

void GameManager::addSceneToStack(std::string name, int stackPos)
{
    // This is a private method, not going to put a check here if the scene is a registered scene
    // The check will be in the public method
    if (IsSceneInStack(name) == false)
    {
        if (stackPos >= mScenesStack.size() || stackPos < 0)
        {
            mScenesStack.push_back(name);
        }
        else
        {
            mScenesStack.insert(mScenesStack.begin() + stackPos, name);
        }
    }
}


// This just sets up the engines systems
void GameManager::setUpSystems()
{
    // Registering Systems
    // Render System
    auto render3DSystem = mCoordinator.RegisterSystem<Render3DSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<MeshComponent>());
        signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
        mCoordinator.SetSystemSignature<Render3DSystem>(signature);
    }

    // Sprite System
    auto spriteSystem = mCoordinator.RegisterSystem<SpriteSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<SpriteComponent>());
        signature.set(mCoordinator.GetComponentType<Transform2DComponent>());
        mCoordinator.SetSystemSignature<SpriteSystem>(signature);
    }

    // Collision System
    auto collisionSystem = mCoordinator.RegisterSystem<CollisionSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<ColliderComponent>());
        signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
        mCoordinator.SetSystemSignature<CollisionSystem>(signature);
    }

    // RigidBody System
    auto rigidBodySystem = mCoordinator.RegisterSystem<RigidBodySystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<RigidBodyComponent>());
        signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
        mCoordinator.SetSystemSignature<RigidBodySystem>(signature);
    }


    // Character Movement System
    auto characterMovementSystem = mCoordinator.RegisterSystem<CharacterMovementSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<CharacterBodyComponent>());
        signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
        signature.set(mCoordinator.GetComponentType<ColliderComponent>());
        mCoordinator.SetSystemSignature<CharacterMovementSystem>(signature);
    }

    // Character Snapping System
    // auto characterSnapSystem = mCoordinator.RegisterSystem<CharacterSnapSystem>();
    // {
    //     Signature signature;
    //     signature.set(mCoordinator.GetComponentType<CharacterBodyComponent>());
    //     signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
    //     mCoordinator.SetSystemSignature<CharacterSnapSystem>(signature);
    // }

    // Transform3D System
    auto transform2DSystem = mCoordinator.RegisterSystem<Transform2DSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<Transform2DComponent>());
        mCoordinator.SetSystemSignature<Transform2DSystem>(signature);
    }

    // Transform3D System
    auto transform3DSystem = mCoordinator.RegisterSystem<Transform3DSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<Transform3DComponent>());
        mCoordinator.SetSystemSignature<Transform3DSystem>(signature);
    }

    // Timer System
    auto timerSystem = mCoordinator.RegisterSystem<TimerSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<TimerComponent>());
        mCoordinator.SetSystemSignature<TimerSystem>(signature);
    }

    auto textSystem = mCoordinator.RegisterSystem<TextSystem>();
    {
        Signature signature;
        signature.set(mCoordinator.GetComponentType<TextComponent>());
        signature.set(mCoordinator.GetComponentType<Transform2DComponent>());
        mCoordinator.SetSystemSignature<TextSystem>(signature);
    }

    // Setting order they are called in
    mSystemsStack.push_back(transform2DSystem);
    mSystemsStack.push_back(transform3DSystem);
    mSystemsStack.push_back(textSystem);
    mSystemsStack.push_back(spriteSystem);
    mSystemsStack.push_back(render3DSystem);
    mSystemsStack.push_back(collisionSystem);
    mSystemsStack.push_back(characterMovementSystem);
    mSystemsStack.push_back(rigidBodySystem);
    mSystemsStack.push_back(timerSystem);

    initSystems();
}

void GameManager::initSystems()
{
    // Just calling Init on all systems
    for (int i = mSystemsStack.size() - 1; i >= 0; i--)
    {
        mSystemsStack[i]->Init();
    }
}
