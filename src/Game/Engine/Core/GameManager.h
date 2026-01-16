#pragma once

#include "ECS.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>


// TODO: This class handles how the game runs. The engine will have "Scenes" explain what scenes are here
// Handles the order the systems are executed and handles setting up all the systems.
// Handles closing all scenes and systems.
class GameManager
{
public:
    static GameManager& Instance(); 
private:
    GameManager() = default;
    ~GameManager() = default;

public:
    // To be called in main
    void Start();
    void Close();
    void Update(const float deltaTime);
    void Render();

    // Get the ECSCoordinator
    Coordinator& GetCoordinator();

    // System management methods
    void AddSystem(std::shared_ptr<System> system, int stackPos = -1);
    void RemoveSystem(std::shared_ptr<System> system);
    int GetSystemStackIndex(std::shared_ptr<System> system) const;
    void ChangeSystemPosition(std::shared_ptr<System> system, int newStackPos);

    // Scene management methods
    void AddScene(const std::string& name, std::unique_ptr<Scene> scene, int stackPos = -1);
    void RemoveScene(const std::string& name);
    void PauseScene(const std::string& name);
    void UnpauseScene(const std::string& name);
    bool IsSceneActive(const std::string& name);
    bool IsScenePaused(const std::string& name);
    int GetSceneStackIndex(const std::string& name) const;
    void ChangeScenePosition(const std::string& name, int newStackPos);

private:
    bool isSceneInStack(const std::string& name) const;
    void addSceneToStack(std::string name, int stackPos);

    void setUpSystems();
    void initSystems();

private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> mScenes;
    std::vector<std::string> mScenesStack; // String so it can ref the mScenes

    // Going to do similar stack for systems
    std::vector<std::shared_ptr<System>> mSystemsStack;

    Coordinator mCoordinator = Coordinator(); // Coordinates ECS
};
