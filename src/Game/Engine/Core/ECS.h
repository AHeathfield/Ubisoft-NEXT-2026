// THIS IS FOLLOWING https://austinmorlan.com/posts/entity_component_system/

// An Entity is an ID. A Component is a struct of data. A System is the logic that operates on the components.
#pragma once

//#include <SDL3/SDL_log.h>
#include "Utility.h"
#include <array>
#include <cstdint>
#include <bitset>
#include <queue>
#include <cassert>
#include <set>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <memory>

// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 5000;

// Components are just structs with a small chunk of functionally related data. EX:
// struct Transform
// {
//     Vec3 position;
//     Quat rotation;
//     Vec3 scale;
// }

// A simple type alias
using ComponentType = std::uint8_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

// I chose the very simple approach of using a std::bitset (modern C++ equivalent of a bitfield), called a Signature. Each component type has a unique ID (starting from 0), which is used to represent a bit in the signature.
//
// As an example, if Transform has type 0, RigidBody has type 1, and Gravity has type 2, an entity that “has” those three components would have a signature of 0b111 (bits 0, 1, and 2 are set).

// Systems would have a signature that it would compare with an entities signature to see if that entity contains all the necessary components (doesn't matter if entity has more than the necessary)

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;
// Basically the signature just reps what components an entity owns


// The Entity Manager is in charge of distributing entity IDs and keeping record of which IDs are in use and which are not.
class EntityManager
{
public:
    EntityManager()
    {
        // Initialize the queue with all possible entity IDs
        for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
        {
            mAvailableEntities.push(entity);
        }
    }

    Entity CreateEntity()
    {
        // Assert is a check and if it isn't true it will create an error with the additional msg
        assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

        // Take an ID from the front of the queue
        Entity id = mAvailableEntities.front();
        mAvailableEntities.pop();
        mLivingEntityCount++;

        // std::string log = std::to_string(mLivingEntityCount);
        // SDL_Log(log.c_str());

        // If it's not in set it won't cause an error
        mDestroyedEntities.erase(id);

        return id;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        // Invalidate the destroyed entity's signature
        // This sets the signature (bitset) values to all 0, resets it
        mSignatures[entity].reset();

        // Put the destroyed ID at the back of the queue
        // Im guessing bc you don't want to be checking non existing entities before existing ones
        mAvailableEntities.push(entity);
        mLivingEntityCount--;

        // Resetting it since it every entity
        if (mDestroyedEntities.size() >= MAX_ENTITIES)
        {
            mDestroyedEntities.clear();
        }
        mDestroyedEntities.insert(entity);
    }

    bool IsEntityDestroyed(Entity entity)
    {
        return (mDestroyedEntities.find(entity) != mDestroyedEntities.end());
    }

    // This sets what components the entity has
    void SetSignature(Entity entity, Signature signature)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        // Put this entity's signature into the array
        mSignatures[entity] = signature;
    }

    Signature GetSignature(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        // Get this entity's signature from the array
        return mSignatures[entity];
    }

private:
    // Queue of unused entity IDs
    std::queue<Entity> mAvailableEntities;

    // A Set of Deleted Entities used to check if a entity has been deleted
    std::unordered_set<Entity> mDestroyedEntities;

    // Array of signatures where the index corresponds to the entity ID
    std::array<Signature, MAX_ENTITIES> mSignatures;

    // Total living entities - used to keep limits on how many exist
    uint32_t mLivingEntityCount = 0;
};



// Component Array
// To Avoid checking if the index in the component array is valid (entity ID is corresponds to the index) we map the entity IDs to array indices.
// To access array we use entity ID to look up the actual array index
// When entity is destroyed, you take the last valid element (last item added to array) and move it into the deleted entity's spot and update the map so that the entity ID now points to the correct position

// The map from array index to entity ID is used when moving the last array element, you know which entity was using that index and can update its map.

// DEMO:
// Add D to Entity 3
// ------
// Array: [A, B, C, D]
//
// Entity->Index:
// [0:0] Entity 0's data (A) is at Index 0
// [1:1] Entity 1's data (B) is at Index 1
// [2:2] Entity 2's data (C) is at Index 2
// [3:3] Entity 3's data (D) is at Index 3
//
// Index->Entity:
// [0:0] Index 0 holds Entity 0's data (A)
// [1:1] Index 1 holds Entity 1's data (B)
// [2:2] Index 2 holds Entity 2's data (C)
// [3:3] Index 3 holds Entity 3's data (D)
//
// Size: 4
// So far so good. Everything is packed into memory. But then we delete the value B from Entity 1. To keep it packed, we move the last element D into the spot occupied by B, and update the maps.
//
// Entity 3 maps to Index 1, and Index 1 maps to Entity
//
// Delete B (which was at Index 1 and was the data of Entity 1)
// ------
// Array: [A, D, C]
//
// Entity->Index:
// [0:0] Entity 0's data (A) is at Index 0
// [3:1] Entity 3's data (D) is at Index 1
// [2:2] Entity 2's data (C) is at Index 2
//
// Index->Entity:
// [0:0] Index 0 holds Entity 0's data (A)
// [1:3] Index 1 holds Entity 3's data (D)
// [2:2] Index 2 holds Entity 2's data (C)
//
// Size: 3

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager (seen later)
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
public:
    void InsertData(Entity entity, T component)
    {
        assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

        // Put new entry at end and update the maps
        size_t newIndex = mSize;
        mEntityToIndexMap[entity] = newIndex;
        mIndexToEntityMap[newIndex] = entity;
        mComponentArray[newIndex] = component;
        mSize++;

        // std::string log = std::to_string(mSize);
        // SDL_Log(log.c_str());
    }

    void RemoveData(Entity entity)
    {
        // Bc the end is always an open spot
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
        size_t indexOfLastElement = mSize - 1;
        mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
        mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        mEntityToIndexMap.erase(entity);
        mIndexToEntityMap.erase(indexOfLastElement);

        --mSize;
    }

    T& GetData(Entity entity)
    {
        assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

        // Return a reference to the entity's component
        return mComponentArray[mEntityToIndexMap[entity]];
    }

    bool HasComponent(Entity entity)
    {
        return (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end());
    }

    void EntityDestroyed(Entity entity) override
    {
        if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
        {
            // Remove the entity's component if it existed
            RemoveData(entity);
        }
    }

private:
    // The packed array of components (of generic type T),
    // set to a specified maximum amount, matching the maximum number
    // of entities allowed to exist simultaneously, so that each entity
    // has a unique spot.
    std::array<T, MAX_ENTITIES> mComponentArray;

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, size_t> mEntityToIndexMap;

    // Map from an array index to an entity ID (to help with the moving when deleted)
    std::unordered_map<size_t, Entity> mIndexToEntityMap;

    // Total size of valid entries in the array
    size_t mSize = 0;
};



// The Component Manager
// In charge of talking to all of the diff ComponentArrays (when a component needs to be added or removed)
// Recall ComponentType is just a uint8_t

class ComponentManager
{
public:
    template<typename T>
    void RegisterComponent()
    {
        const char* typeName = typeid(T).name();
        // utility::log(typeName);

        assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

        //Add this component type to the component type map
        mComponentTypes.insert({ typeName, mNextComponentType });

        // Create a ComponentArray pointer and add it to the component arrays map
        mComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

        // Increment the value so that the next component registered will be different
        mNextComponentType++;
    }

    template<typename T>
    ComponentType GetComponentType()
    {
        const char* typeName = typeid(T).name();
        // utility::log(typeName);

        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

        // Return this component's type - used for creating signatures
        return mComponentTypes[typeName];
    }

    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        // Add a component to the array for an entity
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        // Remove a component from the array for an entity
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity)
    {
        // Get a reference to a component from the array for an entity
        return GetComponentArray<T>()->GetData(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        return GetComponentArray<T>()->HasComponent(entity);
    }

    void EntityDestroyed(Entity entity)
    {
        // Notify each component array that an entity has been destroyed
        // If it has a component for that entity, it will remove it
        for (auto const& pair : mComponentArrays)
        {
            auto const& component = pair.second;

            component->EntityDestroyed(entity);
        }
    }


private:
    // Map from type string pointer to a component type
    std::unordered_map<const char*, ComponentType> mComponentTypes;

    // Map from type string pointer to a component array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays;

    // The component type to be assigned to the next registered component - starting at 0
    ComponentType mNextComponentType = 0;

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }
};



// The System
// A system is any functionality that iterates upon a list of entities with a certain signature of components.
//
// the list of entities is a set bc each entity is unique (unique #) and it's slightly faster than a list O(log n) remove compared to O(n)
// It also makes code easier to read and simpler
    // No need for std::find, just insert() and erase()
class System
{
public:
    virtual ~System() = default; // So I can create collections of Systems 
    virtual void Init() {}
    virtual void Update(float deltaTime) = 0; // I think every system should have an update method that does something with the entities
    virtual void Render() {}
    virtual void OnEntityAdded(Entity entity) {}
    virtual void OnEntityRemoved(Entity entity) {}
    virtual void Close() {}

    void SetActive(const bool active) { mActive = active; }
    bool IsActive() const { return mActive; }

    std::set<Entity> mEntities; // Not sure if I want this protected or not

private:
    bool mActive = true;
};

// Each system will inherit from this class

//EX:
// for (auto const& entity : mEntities) // mEntities!!!!!
// {
//     auto& rigidBody = GetComponent<RigidBody>(entity);
//     auto& transform = GetComponent<Transform>(entity);
//     auto const& gravity = GetComponent<Gravity>(entity);
//
//     transform.position += rigidBody.velocity * dt;
//
//     rigidBody.velocity += gravity.force * dt;
// }



// The System Manager
// The System Manager is in charge of maintaining a record of registered systems and their signatures. When a system is registered, it’s added to a map with the same typeid(T).name() trick used for the components. That same key is used for a map of system pointers as well.

// As with components, this approach requires a call to RegisterSystem() for every additional system type added to the game.
//
// Each system needs to have a signature set for it so that the manager can add appropriate entities to each systems’s list of entities. When an entity’s signature has changed (due to components being added or removed), then the system’s list of entities that it’s tracking needs to be updated.

// If entity the system is tracking is destroyed, then the system also needs to update its list
class SystemManager
{
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        const char* typeName = typeid(T).name();

        assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

        // Create a pointer to the system and return it so it can be used externally
        auto system = std::make_shared<T>();
        mSystems.insert({ typeName, system });
        return system;
    }

    template<typename T>
    std::shared_ptr<T> GetSystem()
    {
        const char* typeName = typeid(T).name();
        auto system = mSystems.find(typeName);

        assert(system != mSystems.end() && "System does not exist");

        return std::static_pointer_cast<T>(system->second);
    }

    template<typename T>
    void SetSignature(Signature signature)
    {
        const char* typeName = typeid(T).name();

        assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

        // Set the signature for this system
        mSignatures.insert({ typeName, signature });
    }

    void EntityDestroyed(Entity entity)
    {
        // Erase a destroyed entity from all system lists
        // mEntities is a set so no check needed
        for (auto const& pair : mSystems)
       {
            auto const& system = pair.second;

            system->OnEntityRemoved(entity);
            system->mEntities.erase(entity);
        }
    }

    void EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        // Notify each system that an entity's signature changed
        for (auto const& pair : mSystems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = mSignatures[type];

            // Entity signature matches system signature - insert into set (BITWISE AND)
            if ((entitySignature & systemSignature) == systemSignature)
            {
                // SDL_Log("Added entity to system");
                system->OnEntityAdded(entity);
                system->mEntities.insert(entity);
            }
            // Entity signature does not match system signature - erase from set
            else
            {
                system->OnEntityRemoved(entity);
                // utility::log("SIG CHANGE REMOVED");
                system->mEntities.erase(entity);
            }
        }
    }

private:
    // Map from system type string pointer to a signature
    std::unordered_map<const char*, Signature> mSignatures;

    // Map from system type string pointer to a system pointer
    std::unordered_map<const char*, std::shared_ptr<System>> mSystems;
};



//The Coordinator
// This allows the Entity Manager, Component Manager, and System Manager talk to each other
// Its nice since we can just say coordinator.AddEntity, or .AddComponent, or system since it has pointers to each manager
class Coordinator
{
public:
    // void Init()
    // Constructor
    Coordinator()
    {
        // Create pointers to each manager
        mComponentManager = std::make_unique<ComponentManager>();
        mEntityManager = std::make_unique<EntityManager>();
        mSystemManager = std::make_unique<SystemManager>();
    }

    // Entity methods
    Entity CreateEntity()
    {
        return mEntityManager->CreateEntity();
    }

    void DestroyEntity(Entity entity)
    {
        mSystemManager->EntityDestroyed(entity);
        mEntityManager->DestroyEntity(entity);
        mComponentManager->EntityDestroyed(entity);
    }

    bool IsEntityDestroyed(Entity entity)
    {
        return mEntityManager->IsEntityDestroyed(entity);
    }


    // Component methods
    template<typename T>
    void RegisterComponent()
    {
        mComponentManager->RegisterComponent<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component)
    {
        mComponentManager->AddComponent<T>(entity, component);

        auto signature = mEntityManager->GetSignature(entity);
        signature.set(mComponentManager->GetComponentType<T>(), true);
        mEntityManager->SetSignature(entity, signature);
        mSystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        mComponentManager->RemoveComponent<T>(entity);

        auto signature = mEntityManager->GetSignature(entity);
        signature.set(mComponentManager->GetComponentType<T>(), false);
        mEntityManager->SetSignature(entity, signature);

        mSystemManager->EntitySignatureChanged(entity, signature);
    }


    template<typename T>
    T& GetComponent(Entity entity)
    {
        return mComponentManager->GetComponent<T>(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        return mComponentManager->HasComponent<T>(entity);
    }


    template<typename T>
    ComponentType GetComponentType()
    {
        return mComponentManager->GetComponentType<T>();
    }


    // System methods
    template<typename T>
    std::shared_ptr<T> RegisterSystem()
    {
        return mSystemManager->RegisterSystem<T>();
    }

    template<typename T>
    std::shared_ptr<T> GetSystem()
    {
        return mSystemManager->GetSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature)
    {
        mSystemManager->SetSignature<T>(signature);
    }

private:
    std::unique_ptr<ComponentManager> mComponentManager = nullptr;
    std::unique_ptr<EntityManager> mEntityManager = nullptr;
    std::unique_ptr<SystemManager> mSystemManager = nullptr;
};
