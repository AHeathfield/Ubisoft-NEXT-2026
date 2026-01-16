# Comments
- Remove not needed ones
- In system header files put a brief explanation of what it does and its signature!

# Camera
- Currently the Render3D has the fov and other values set, I think camera should have this instead

# Clean up Game Manager (DONE)
- Find better way of adding systems
- I don't think the string of a system is needed

# 2D Collision system
- Very simple just have AABB, OBB, and sphere colliders
- Need collider2D (would need to change collidercomponent to Collider3DComponent)

# Mesh primitive shape optimization + Mesh optimization
- Just load the mesh once and put it into a Mesh.cpp files so the engine only needs to load the default shapes once, and use pointers to get those meshses
- MeshSystem should contain the LoadFromObject and should contain all the meshes that have been loaded once before so it doesnt have to read file.
    - MAKE GAME FIRST THEN DO THIS OPTIMIZATION, no point in optimizing if theres no game :)

# Split headers with methods into headers + cpp
- I do know header code runs a bit faster so maybe keep like Math classes like that
- I think more cpp files means faster compile time
- Try to match the API file structure...

# Entity active control
- Not sure if this a good name for this
- Basically it would be like frustrum culling but for entities x amount of distance away
    - Basically, if like 2 entities have a transform and one is set as like ref (the player) if the other is x amount of distance away, somehow set that entity to inactive
    - Could make it based on the camera the Render system is using

# User defined systems (DONE)
- Make it so users can add own systems and they will be called before all engine systems

# Scene CreateEntity() (DONE)
- Currently there's a lot of repetitive manual steps to creating an entity, like CreateEntity() and the mSceneEntities.push_back(entity), just make a method part of the scene class to handle this.

# DEBUG (DONE)
- Add a method to Render3DSystem to add a mesh to the queue (doesn't have to be entity) which will help alot with debugging
    - Then add a if debug for the octree world BVH's

# URGENT, Implement new Ubisoft API (they updated it DEC 10) DONE!!

# Events (DONE, for now lol)
- Using the API's methods for input, create specific types of events (Key Events, Mouse events, Controller(if time)) and add them to a queue in GameManager
- I don't think I need the event dispatcher, The Cherno said that you can just use if statements which I like since they make more sense to me
    - I think having things like the dispatcher create ambiguity
- Main goal is to have events be run through each scene but have it so if a scene marks the event as handled that the scenes below it in the scene stack don't even get the event coming.
- I think I should reimplement the scene stack
- *NOT GOING TO DO EVENTS*,  I think with the way I have scenes set up there's not really a need for that, I would rather have it more similar to how I handle events in Godot with just simple checks which I can do with the API.

# Camera (DONE, for now)
- Camera class should just have things necessary to the camera but not have any like update or event handling
- For example, the player should handle how the inputs effects the camera.
- Add basic camera movement
    - Add mouse motion to rotate camera like in first person shooter
- *NEW*, Try to change rendering system + camera to how the game physics cookbook does it
    - There is a lot better structure and I think it has some better optimizations using octrees.
    - *ONLY DO THIS IF I HAVE TIME, I ALREADY HAVE A SYSTEM THAT WORKS*

# Math Library (DONE, enough)
- So far I just have the essentials and adding them as I need.
- It would be nice if I have time to implement a bunch of other things especially for matrices

# Create RigidBody (DONE, enough)
- Not to sure about this need to find a video or article or Textbook on this
- I can always look at the Godot Source code aswell

# 3D Collision Detection (DONE, enough)
- Same with Rigidbody, might not even need rigidbody and can maybe just stick with simple collisions
- Basic shapes
    - Cubes, rectangles
    - Spheres
- Decided to go with a static and dynamic approach using BVH Octree's for optimizing
- Still need to test all the primitives
- I THINK I SHOULD CREATE EVENTS (Observer pattern) and that way collisions can be handled by different systems...

# 3D Collision Response (DONE, enough)
- I kind of want to do it how Godot does it where you can define things that happen in a collision
- If I make lets say a RigidBodyComponent, if it collides with other physics collider you won't be able to go through them
- Area3D component would be like a trigger

# 3D Physics (see rigidbody)
- I've already done alot at the time of writing this
- If I have extra time adding particle, spring, and cloth physics can be cool

# 3D Rendering (it works...)
- Works decent currently, lots of comments to clean up
- If it's laggy may need to find some new ways
    - Static objects can be done efficiently with binary space partitioning
- Need to add scaling
- Need to add color per mesh
- If time look into multiple light sources
- If time look into 3D Textures

# 2D Rendering (DONE, see Sprite system)
- Should be similar to how I did it in my summer projects
    - Store textures
    - Have a render order (basically painters algorithm). I think a new implementation may be better for this aswell (maybe just use painters algo lol)

# Audio (DONE)
- Similar to summer projects

# Useful Things To Add
## Property Animation Player
- AnimationsPlayer like in Godot where it uses curves to interpolate things like transform
    - Linear, bizer curve

# If Time
## Sprite Animation Player
- Similar to Summer projects
- This would be nice for things like UI

## Skybox
- Would be cool but I have no idea how to do it

## Lighting
- Not to sure about this

## Basic Shaders
- Not sure if I can do it with the limits on using openGL
