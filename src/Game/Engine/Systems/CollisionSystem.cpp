#include "CollisionSystem.h"
#include "../Core/GameManager.h"
#include "../Components/ColliderComponent.h"
#include "../../config.h"
#include <unordered_set>
#include <vector>
#include "Render3DSystem.h"


// PRIVATE STATIC HELPER METHODS FOR DISPATCH TABLE
bool CollisionSystem::AABB_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsAABBAABBIntersection(col1.collider.aabb, col2.collider.aabb);
}
bool CollisionSystem::AABB_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsAABBOBBIntersection(col1.collider.aabb, col2.collider.obb);
}
bool CollisionSystem::AABB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsAABBPlaneIntersection(col1.collider.aabb, col2.collider.plane);
}
bool CollisionSystem::AABB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSphereAABBIntersection(col2.collider.sphere, col1.collider.aabb);
}
bool CollisionSystem::AABB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleAABBIntersection(col2.collider.triangle, col1.collider.aabb);
}
bool CollisionSystem::AABB_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshAABBIntersection(col2.meshCollider, col1.collider.aabb);
}
bool CollisionSystem::OBB_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsOBBOBBIntersection(col1.collider.obb, col2.collider.obb);
}
bool CollisionSystem::OBB_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsAABBOBBIntersection(col2.collider.aabb, col1.collider.obb);
}
bool CollisionSystem::OBB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsOBBPlaneIntersection(col1.collider.obb, col2.collider.plane);
}
bool CollisionSystem::OBB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSphereOBBIntersection(col2.collider.sphere, col1.collider.obb);
}
bool CollisionSystem::OBB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleOBBIntersection(col2.collider.triangle, col1.collider.obb);
}
bool CollisionSystem::OBB_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshOBBIntersection(col2.meshCollider, col1.collider.obb);
}
bool CollisionSystem::PLANE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsPlanePlaneIntersection(col1.collider.plane, col2.collider.plane);
}
bool CollisionSystem::PLANE_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsAABBPlaneIntersection(col2.collider.aabb, col1.collider.plane);
}
bool CollisionSystem::PLANE_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsOBBPlaneIntersection(col2.collider.obb, col1.collider.plane);
}
bool CollisionSystem::PLANE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSpherePlaneIntersection(col2.collider.sphere, col1.collider.plane);
}
bool CollisionSystem::PLANE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTrianglePlaneIntersection(col2.collider.triangle, col1.collider.plane);
}
bool CollisionSystem::PLANE_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshPlaneIntersection(col2.meshCollider, col1.collider.plane);
}
bool CollisionSystem::SPHERE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSphereSphereIntersection(col1.collider.sphere, col2.collider.sphere);
}
bool CollisionSystem::SPHERE_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSphereAABBIntersection(col1.collider.sphere, col2.collider.aabb);
}
bool CollisionSystem::SPHERE_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSphereOBBIntersection(col1.collider.sphere, col2.collider.obb);
}
bool CollisionSystem::SPHERE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsSpherePlaneIntersection(col1.collider.sphere, col2.collider.plane);
}
bool CollisionSystem::SPHERE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleSphereIntersection(col2.collider.triangle, col1.collider.sphere);
}
bool CollisionSystem::SPHERE_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshSphereIntersection(col2.meshCollider, col1.collider.sphere);
}
bool CollisionSystem::TRIANGLE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleTriangleIntersectionRobust(col1.collider.triangle, col2.collider.triangle);
}
bool CollisionSystem::TRIANGLE_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleAABBIntersection(col1.collider.triangle, col2.collider.aabb);
}
bool CollisionSystem::TRIANGLE_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleOBBIntersection(col1.collider.triangle, col2.collider.obb);
}
bool CollisionSystem::TRIANGLE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTriangleSphereIntersection(col1.collider.triangle, col2.collider.sphere);
}
bool CollisionSystem::TRIANGLE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsTrianglePlaneIntersection(col1.collider.triangle, col2.collider.plane);
}
bool CollisionSystem::TRIANGLE_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshTriangleIntersection(col2.meshCollider, col1.collider.triangle);
}
bool CollisionSystem::MESH_MESH(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshMeshIntersection(col1.meshCollider, col2.meshCollider);
}
bool CollisionSystem::MESH_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshAABBIntersection(col1.meshCollider, col2.collider.aabb);
}
bool CollisionSystem::MESH_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshOBBIntersection(col1.meshCollider, col2.collider.obb);
}
bool CollisionSystem::MESH_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshSphereIntersection(col1.meshCollider, col2.collider.sphere);
}
bool CollisionSystem::MESH_PLANE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshPlaneIntersection(col1.meshCollider, col2.collider.plane);
}
bool CollisionSystem::MESH_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::IsMeshTriangleIntersection(col1.meshCollider, col2.collider.triangle);
}


// TODO: Currently the AABB collisions are done lazy by just turning them into OBB, should have a more optimized method for them...
CollisionManifold CollisionSystem::man_AABB_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	OBB obb1 = Geometry3D::CreateOBBFromAABB(col1.collider.aabb);
	OBB obb2 = Geometry3D::CreateOBBFromAABB(col2.collider.aabb);
	return Geometry3D::FindCollisionManifold(obb1, obb2);
}
CollisionManifold CollisionSystem::man_AABB_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	OBB obb1 = Geometry3D::CreateOBBFromAABB(col1.collider.aabb);
	return Geometry3D::FindCollisionManifold(obb1, col2.collider.obb);
}
// CollisionManifold CollisionSystem::man_AABB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
CollisionManifold CollisionSystem::man_AABB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	OBB obb1 = Geometry3D::CreateOBBFromAABB(col1.collider.aabb);
	return Geometry3D::FindCollisionManifold(obb1, col2.collider.sphere);
}
// CollisionManifold CollisionSystem::man_AABB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_AABB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
CollisionManifold CollisionSystem::man_OBB_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::FindCollisionManifold(col1.collider.obb, col2.collider.obb);
}
CollisionManifold CollisionSystem::man_OBB_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	OBB obb2 = Geometry3D::CreateOBBFromAABB(col2.collider.aabb);
	return Geometry3D::FindCollisionManifold(col1.collider.obb, obb2);
}
// CollisionManifold CollisionSystem::man_OBB_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
CollisionManifold CollisionSystem::man_OBB_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::FindCollisionManifold(col1.collider.obb, col2.collider.sphere);
}
// CollisionManifold CollisionSystem::man_OBB_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_OBB_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_PLANE_PLANE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_PLANE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_PLANE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_PLANE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
CollisionManifold CollisionSystem::man_SPHERE_SPHERE(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::FindCollisionManifold(col1.collider.sphere, col2.collider.sphere);
}
CollisionManifold CollisionSystem::man_SPHERE_AABB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	OBB obb2 = Geometry3D::CreateOBBFromAABB(col2.collider.aabb);
	return Geometry3D::FindCollisionManifold(col1.collider.sphere, obb2);
}
CollisionManifold CollisionSystem::man_SPHERE_OBB(const ColliderComponent& col1, const ColliderComponent& col2)
{
	return Geometry3D::FindCollisionManifold(col1.collider.sphere, col2.collider.obb);
}
// CollisionManifold CollisionSystem::man_SPHERE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_SPHERE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_TRIANGLE_TRIANGLE(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_TRIANGLE_MESH(const ColliderComponent& col1, const ColliderComponent& col2);
// CollisionManifold CollisionSystem::man_MESH_MESH(const ColliderComponent& col1, const ColliderComponent& col2);




void CollisionSystem::Init()
{
	// ================== Initializing Collision Dispatch Table =============
	mCollisionTable[ShapeType::AABB_3D][ShapeType::AABB_3D] = &CollisionSystem::AABB_AABB;
	mCollisionTable[ShapeType::AABB_3D][ShapeType::OBB_3D] = &CollisionSystem::AABB_OBB;
	mCollisionTable[ShapeType::AABB_3D][ShapeType::PLANE_3D] = &CollisionSystem::AABB_PLANE;
	mCollisionTable[ShapeType::AABB_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::AABB_TRIANGLE;
	mCollisionTable[ShapeType::AABB_3D][ShapeType::MESH_3D] = &CollisionSystem::AABB_MESH;

	mCollisionTable[ShapeType::OBB_3D][ShapeType::OBB_3D] = &CollisionSystem::OBB_OBB;
	mCollisionTable[ShapeType::OBB_3D][ShapeType::AABB_3D] = &CollisionSystem::OBB_AABB;
	mCollisionTable[ShapeType::OBB_3D][ShapeType::PLANE_3D] = &CollisionSystem::OBB_PLANE;
	mCollisionTable[ShapeType::OBB_3D][ShapeType::SPHERE_3D] = &CollisionSystem::OBB_SPHERE;
	mCollisionTable[ShapeType::OBB_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::OBB_TRIANGLE;
	mCollisionTable[ShapeType::OBB_3D][ShapeType::MESH_3D] = &CollisionSystem::OBB_MESH;

	mCollisionTable[ShapeType::PLANE_3D][ShapeType::PLANE_3D] = &CollisionSystem::PLANE_PLANE;
	mCollisionTable[ShapeType::PLANE_3D][ShapeType::AABB_3D] = &CollisionSystem::PLANE_AABB;
	mCollisionTable[ShapeType::PLANE_3D][ShapeType::OBB_3D] = &CollisionSystem::PLANE_OBB;
	mCollisionTable[ShapeType::PLANE_3D][ShapeType::SPHERE_3D] = &CollisionSystem::PLANE_SPHERE;
	mCollisionTable[ShapeType::PLANE_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::PLANE_TRIANGLE;
	mCollisionTable[ShapeType::PLANE_3D][ShapeType::MESH_3D] = &CollisionSystem::PLANE_MESH;

	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::SPHERE_3D] = &CollisionSystem::SPHERE_SPHERE;
	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::AABB_3D] = &CollisionSystem::SPHERE_AABB;
	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::OBB_3D] = &CollisionSystem::SPHERE_OBB;
	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::PLANE_3D] = &CollisionSystem::SPHERE_PLANE;
	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::SPHERE_TRIANGLE;
	mCollisionTable[ShapeType::SPHERE_3D][ShapeType::MESH_3D] = &CollisionSystem::SPHERE_MESH;

	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::TRIANGLE_TRIANGLE;
	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::AABB_3D] = &CollisionSystem::TRIANGLE_AABB;
	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::OBB_3D] = &CollisionSystem::TRIANGLE_OBB;
	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::SPHERE_3D] = &CollisionSystem::TRIANGLE_SPHERE;
	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::PLANE_3D] = &CollisionSystem::TRIANGLE_PLANE;
	mCollisionTable[ShapeType::TRIANGLE_3D][ShapeType::MESH_3D] = &CollisionSystem::TRIANGLE_MESH;

	mCollisionTable[ShapeType::MESH_3D][ShapeType::MESH_3D] = &CollisionSystem::MESH_MESH;
	mCollisionTable[ShapeType::MESH_3D][ShapeType::AABB_3D] = &CollisionSystem::MESH_AABB;
	mCollisionTable[ShapeType::MESH_3D][ShapeType::OBB_3D] = &CollisionSystem::MESH_OBB;
	mCollisionTable[ShapeType::MESH_3D][ShapeType::SPHERE_3D] = &CollisionSystem::MESH_SPHERE;
	mCollisionTable[ShapeType::MESH_3D][ShapeType::PLANE_3D] = &CollisionSystem::MESH_PLANE;
	mCollisionTable[ShapeType::MESH_3D][ShapeType::TRIANGLE_3D] = &CollisionSystem::MESH_TRIANGLE;


	// ================== Initializing Manifold Dispatch Table =============
	mManifoldTable[ShapeType::AABB_3D][ShapeType::AABB_3D] = &CollisionSystem::man_AABB_AABB;
	mManifoldTable[ShapeType::AABB_3D][ShapeType::OBB_3D] = &CollisionSystem::man_AABB_OBB;
	mManifoldTable[ShapeType::AABB_3D][ShapeType::SPHERE_3D] = &CollisionSystem::man_AABB_SPHERE;
	mManifoldTable[ShapeType::AABB_3D][ShapeType::PLANE_3D] = nullptr; // &CollisionSystem::man_AABB_PLANE;
	mManifoldTable[ShapeType::AABB_3D][ShapeType::TRIANGLE_3D] = nullptr; // &CollisionSystem::man_AABB_TRIANGLE;
	mManifoldTable[ShapeType::AABB_3D][ShapeType::MESH_3D] = nullptr; // &CollisionSystem::man_AABB_MESH;

	mManifoldTable[ShapeType::OBB_3D][ShapeType::OBB_3D] = &CollisionSystem::man_OBB_OBB;
	mManifoldTable[ShapeType::OBB_3D][ShapeType::AABB_3D] = &CollisionSystem::man_OBB_AABB;
	mManifoldTable[ShapeType::OBB_3D][ShapeType::SPHERE_3D] = &CollisionSystem::man_OBB_SPHERE;
	mManifoldTable[ShapeType::OBB_3D][ShapeType::PLANE_3D] = nullptr; // &CollisionSystem::man_OBB_PLANE;
	mManifoldTable[ShapeType::OBB_3D][ShapeType::TRIANGLE_3D] = nullptr; // &CollisionSystem::man_OBB_TRIANGLE;
	mManifoldTable[ShapeType::OBB_3D][ShapeType::MESH_3D] = nullptr; //&CollisionSystem::man_OBB_MESH;

	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::SPHERE_3D] = &CollisionSystem::man_SPHERE_SPHERE;
	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::AABB_3D] = &CollisionSystem::man_SPHERE_AABB;
	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::OBB_3D] = &CollisionSystem::man_SPHERE_OBB;
	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::PLANE_3D] = nullptr; // &CollisionSystem::man_PLANE_SPHERE;
	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::TRIANGLE_3D] = nullptr; // &CollisionSystem::man_SPHERE_TRIANGLE;
	mManifoldTable[ShapeType::SPHERE_3D][ShapeType::MESH_3D] = nullptr; // &CollisionSystem::man_SPHERE_TRIANGLE;


	mManifoldTable[ShapeType::PLANE_3D][ShapeType::PLANE_3D] = nullptr; // &CollisionSystem::man_PLANE_PLANE;
	mManifoldTable[ShapeType::PLANE_3D][ShapeType::AABB_3D] = nullptr; // &CollisionSystem::man_PLANE_PLANE;
	mManifoldTable[ShapeType::PLANE_3D][ShapeType::OBB_3D] = nullptr; // &CollisionSystem::man_PLANE_PLANE;
	mManifoldTable[ShapeType::PLANE_3D][ShapeType::SPHERE_3D] = nullptr; // &CollisionSystem::man_PLANE_SPHERE;
	mManifoldTable[ShapeType::PLANE_3D][ShapeType::TRIANGLE_3D] = nullptr; // &CollisionSystem::man_PLANE_TRIANGLE;
	mManifoldTable[ShapeType::PLANE_3D][ShapeType::MESH_3D] = nullptr; //&CollisionSystem::man_PLANE_TRIANGLE;

	mManifoldTable[ShapeType::TRIANGLE_3D][ShapeType::TRIANGLE_3D] = nullptr; // &CollisionSystem::man_TRIANGLE_TRIANGLE;
	mManifoldTable[ShapeType::TRIANGLE_3D][ShapeType::AABB_3D] = nullptr; // &CollisionSystem::man_SPHERE_TRIANGLE;
	mManifoldTable[ShapeType::TRIANGLE_3D][ShapeType::OBB_3D] = nullptr; // &CollisionSystem::man_SPHERE_TRIANGLE;
	mManifoldTable[ShapeType::TRIANGLE_3D][ShapeType::SPHERE_3D] = nullptr; // &CollisionSystem::man_SPHERE_TRIANGLE;
	mManifoldTable[ShapeType::TRIANGLE_3D][ShapeType::MESH_3D] = nullptr; //&CollisionSystem::man_TRIANGLE_TRIANGLE;

	mManifoldTable[ShapeType::MESH_3D][ShapeType::MESH_3D] = nullptr;
	mManifoldTable[ShapeType::MESH_3D][ShapeType::AABB_3D] = nullptr;
	mManifoldTable[ShapeType::MESH_3D][ShapeType::OBB_3D] = nullptr;
	mManifoldTable[ShapeType::MESH_3D][ShapeType::SPHERE_3D] = nullptr;
	mManifoldTable[ShapeType::MESH_3D][ShapeType::PLANE_3D] = nullptr;
	mManifoldTable[ShapeType::MESH_3D][ShapeType::TRIANGLE_3D] = nullptr; //&CollisionSystem::man_TRIANGLE_TRIANGLE;



	// Setting up BVH Octrees SHOULD BE A BETTER WAY TO DO THIS especially when it comes to the size and position
	// Will probably move this method into the start scene or something
	CreateBVHOctree(mStaticOctree, mStaticEntities, Vector3(0, 0, 0), 100.f);
	CreateBVHOctree(mDynamicOctree, mDynamicEntities, Vector3(0, 0, 0), 100.f);
}

void CollisionSystem::Update(float deltaTime)
{
	// Updating collider world space transforms
	transformColliders();
	
	// Checking for collisions
	checkCollisions();

	// Collision Response
	handleCollisions();


#if DEBUG_WORLD
	// utility::log("test");
	auto render_sys = GameManager::Instance().GetCoordinator().GetSystem<Render3DSystem>();
	Transform3DComponent transform;
	std::vector<AABB> allBounds;
	Mesh mesh_aabb = Geometry3D::GetDefaultPrimitiveMesh(ShapeType::AABB_3D);
	mesh_aabb.isWireframe = true;

	// Static Octree
	mesh_aabb.color = Color(255, 165, 0);
	mStaticOctree->GetAllBounds(allBounds);
	for (int i = 0; i < allBounds.size(); i++)
	{
		transform.SetPosition(allBounds[i].position);
		transform.SetScale(allBounds[i].size);
		render_sys->DrawMesh(mesh_aabb, transform);
	}
	allBounds.clear();

	// Dynamic Octree
	mesh_aabb.color = Color(0, 0, 255);
	mDynamicOctree->GetAllBounds(allBounds);
	for (int i = 0; i < allBounds.size(); i++)
	{
		transform.SetPosition(allBounds[i].position);
		transform.SetScale(allBounds[i].size);
		render_sys->DrawMesh(mesh_aabb, transform);
	}
#endif
}


void CollisionSystem::OnEntityAdded(Entity entity)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    if (coordinator.HasComponent<ColliderComponent>(entity))
    {
        auto& collider = coordinator.GetComponent<ColliderComponent>(entity);
        const auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);
        collider.UpdateWorldSpace(transform);

        switch (collider.colliderType)
        {
            case ColliderType::STATIC:
                mStaticEntities.insert(entity);
                insertOctreeNode(mStaticOctree, entity);
                break;
            case ColliderType::DYNAMIC:
                mDynamicEntities.insert(entity);
                insertOctreeNode(mDynamicOctree, entity);
                break;
        }
    }

	// // To accelerate mesh collisions
	// if (collider.shape == ShapeType::MESH_3D)
	// {
	// 	utility::log("accelerating mesh");
	// 	Geometry3D::AccelerateMesh(collider.meshCollider);
	// }
}


void CollisionSystem::OnEntityRemoved(Entity entity)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	if (coordinator.HasComponent<ColliderComponent>(entity))
	{
		const auto& collider = coordinator.GetComponent<ColliderComponent>(entity);

		switch (collider.colliderType)
		{
			case ColliderType::STATIC:
				mStaticEntities.erase(entity);
				removeOctreeNode(mStaticOctree, entity);
				break;
			case ColliderType::DYNAMIC:
				mDynamicEntities.erase(entity);
				removeOctreeNode(mDynamicOctree, entity);
				break;
		}
	}

}


void CollisionSystem::Close()
{
    if (mStaticOctree != nullptr)
    {
        delete mStaticOctree;
        mStaticOctree = nullptr;
    }

    if (mDynamicOctree != nullptr)
    {
        delete mDynamicOctree;
        mDynamicOctree = nullptr;
    }

    mStaticEntities.clear();
    mDynamicEntities.clear();
    mCollisions.clear();
    mExitings.clear();
}


bool CollisionSystem::CreateBVHOctree(OctreeNode*& root, const std::unordered_set<Entity>& entities, const Vector3& pos, float size)
{
	// If there already is a BVHroot
	if (root != nullptr)
		return false;

	// Min and max point to construct bounding box for the world
	Vector3 min = Vector3(pos.x - size, pos.y - size, pos.z - size);
	Vector3 max = Vector3(pos.x + size, pos.y + size, pos.z + size);

	// Construct tree root
	root = new OctreeNode();
	root->bounds = AABB::FromMinMax(min, max);
	root->children = nullptr;

	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		root->entities.push_back(*it);
	}

	// Split octree 5 times, 5 is an arbitrary number that apparently works well
	splitOctree(root, 5);
	return true;
}


CollisionManifold CollisionSystem::GetClosestStaticManifold(const ColliderComponent& collider)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    std::unordered_set<Entity> ents_to_check = queryOctree(mStaticOctree, collider);

    return getClosestManifold(collider, ents_to_check);
}


CollisionManifold CollisionSystem::GetClosestDynamicManifold(const ColliderComponent& collider)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    std::unordered_set<Entity> ents_to_check = queryOctree(mDynamicOctree, collider);

    return getClosestManifold(collider, ents_to_check);
}


std::unordered_set<Entity> CollisionSystem::QueryStaticCollider(const ColliderComponent& collider)
{
    return queryOctree(mStaticOctree, collider);
}


std::unordered_set<Entity> CollisionSystem::QueryDynamicCollider(const ColliderComponent& collider)
{
    return queryOctree(mDynamicOctree, collider);
}


// For now its just a switch, if its too slow I can make it into a tiny dispatch table
bool CollisionSystem::RaycastCollider(const Ray3& ray, RaycastResult& outResult, const ColliderComponent& colliderComp)
{
    switch (colliderComp.shape)
    {
        case ShapeType::AABB_3D:
            return Geometry3D::Raycast(colliderComp.collider.aabb, ray, outResult);
        case ShapeType::OBB_3D:
            return Geometry3D::Raycast(colliderComp.collider.obb, ray, outResult);
        case ShapeType::SPHERE_3D:
            return Geometry3D::Raycast(colliderComp.collider.sphere, ray, outResult);
        case ShapeType::PLANE_3D:
            return Geometry3D::Raycast(colliderComp.collider.plane, ray, outResult);
        case ShapeType::TRIANGLE_3D:
            return Geometry3D::Raycast(colliderComp.collider.triangle, ray, outResult);
        case ShapeType::MESH_3D:
            return Geometry3D::Raycast(colliderComp.meshCollider, ray, outResult);
    }
}


bool CollisionSystem::RaycastStatic(const Ray3& ray, RaycastResult& outResult, const float rayLength)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    std::unordered_set<Entity> ents_to_check = queryRayOctree(mStaticOctree, ray, rayLength);
    outResult = getClosestRaycast(ray, rayLength, ents_to_check);

    return outResult.hit;
}


bool CollisionSystem::RaycastDynamic(const Ray3& ray, RaycastResult& outResult, const float rayLength)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    std::unordered_set<Entity> ents_to_check = queryRayOctree(mDynamicOctree, ray, rayLength);
    outResult = getClosestRaycast(ray, rayLength, ents_to_check);

    return outResult.hit;
}

std::unordered_set<Entity> CollisionSystem::QueryStaticRay(const Ray3&ray, const float rayLength)
{
    return queryRayOctree(mStaticOctree, ray, rayLength);
}


std::unordered_set<Entity> CollisionSystem::QueryDynamicRay(const Ray3&ray, const float rayLength)
{
    return queryRayOctree(mDynamicOctree, ray, rayLength);
}



// PRIVATE METHODS
void CollisionSystem::splitOctree(OctreeNode* node, int depth)
{
	if (depth <= 0)
		return;

	depth--;

	// Current node has no children
	if (node->children == nullptr)
	{
		node->children = new OctreeNode[8];

		Vector3 c = node->bounds.position;
		Vector3 e = node->bounds.size * 0.5f;

		// Split the octree bounding box into 8 equal child bounding boxes
		// Each child bounding box shares 1 vertex, the center of the parent bounding box
		node->children[0].bounds = AABB(c + Vector3(-e.x, +e.y, -e.z), e);
		node->children[1].bounds = AABB(c + Vector3(+e.x, +e.y, -e.z), e);
		node->children[2].bounds = AABB(c + Vector3(-e.x, +e.y, +e.z), e);
		node->children[3].bounds = AABB(c + Vector3(+e.x, +e.y, +e.z), e);
		node->children[4].bounds = AABB(c + Vector3(-e.x, -e.y, -e.z), e);
		node->children[5].bounds = AABB(c + Vector3(+e.x, -e.y, -e.z), e);
		node->children[6].bounds = AABB(c + Vector3(-e.x, -e.y, +e.z), e);
		node->children[7].bounds = AABB(c + Vector3(+e.x, -e.y, +e.z), e);

		// If node has children and still contains any entities, assign the entities to the children
		if (node->children != nullptr && node->entities.size() > 0)
		{
			Coordinator& coordinator = GameManager::Instance().GetCoordinator();
			// For each child
			for (int i = 0; i < 8; i++)
			{
				int size = node->entities.size();
				// For each entity the child contains
				for (int j = 0; j < size; j++)
				{
					// Only add models to the child bounding box if the shape of the entity intersects the bounds of the child node
					// In my case I need to make a temporary collider component for the bounds aabb to make this work
					Entity ent = node->entities[j];
					const auto& ent_col = coordinator.GetComponent<ColliderComponent>(ent);

					ColliderComponent child_col;
					child_col.shape = ShapeType::AABB_3D;
					child_col.collider.aabb = node->bounds;


					// Checks to see if entity's collider intersects with the child nodes bounds (aabb)
                    CollisionFunc collisionFunc = mCollisionTable[ent_col.shape][child_col.shape];

					if (collisionFunc != nullptr)
					{
						if (collisionFunc(ent_col, child_col) == true)
						{
							node->children[i].entities.push_back(ent);
						}
					}
				}
			}
			node->entities.clear();

			// Finally, recurse. this ensures that we split as many times as requested by the depth parameter
			for (int i = 0; i < 8; i++)
			{
				splitOctree(&(node->children[i]), depth);
			}
		}
	}
}

void CollisionSystem::insertOctreeNode(OctreeNode* node, Entity entity)
{
	const auto& ent_col = GameManager::Instance().GetCoordinator().GetComponent<ColliderComponent>(entity);

	// Making temporary collider component for the bounds
	ColliderComponent bounds_col;
	bounds_col.shape = ShapeType::AABB_3D;
	bounds_col.collider.aabb = node->bounds;

	// Checks to see if entity's collider intersects with the child nodes bounds (aabb)
	CollisionFunc collisionFunc = mCollisionTable[ent_col.shape][bounds_col.shape];

	if (collisionFunc != nullptr)
	{
		if (collisionFunc(ent_col, bounds_col) == true)
		{
			// Only insert entities into leaf nodes
			if (node->children == nullptr)
			{
				node->entities.push_back(entity);
			}
			else
			{
				// This is not a leaf node, recursively call insert on all the children of this node
				for (int i = 0; i < 8; i++)
				{
					insertOctreeNode(&(node->children[i]), entity);
				}
			}
		}
	}
	
}

void CollisionSystem::removeOctreeNode(OctreeNode* node, Entity entity)
{
	if (node->children == nullptr)
	{
		// If this is leaf node and it contains entity we are trying to delete, delete the entity
		utility::vectorRemoveElement(node->entities, entity);
	}
	else // Not a leaf node, do recursive call on remove on all nodes of the current node
	{
		for (int i = 0; i < 8; i++)
		{
			removeOctreeNode(&(node->children[i]), entity);
		}
	}
}


void CollisionSystem::updateOctreeNode(OctreeNode* node, Entity entity)
{
	removeOctreeNode(node, entity);
	insertOctreeNode(node, entity);
}

std::unordered_set<Entity> CollisionSystem::queryOctree(OctreeNode* node, const ColliderComponent& collider)
{
	std::unordered_set<Entity> result;

	// Making temporary collider component for the bounds
	ColliderComponent bounds_col;
	bounds_col.shape = ShapeType::AABB_3D;
	bounds_col.collider.aabb = node->bounds;

	// Checks to see if collider being queried intersects with the nodes bounds (aabb)
	CollisionFunc collisionFunc = mCollisionTable[collider.shape][bounds_col.shape];

	if (collisionFunc != nullptr)
	{
		// Only do things if the query collider intersects with node bounds (aabb)
		if (collisionFunc(collider, bounds_col) == true)
		{
			// If leaf node return any entities which are intersecting with query box
			if (node->children == nullptr)
			{
				Coordinator& coordinator = GameManager::Instance().GetCoordinator();
				int num_entities = node->entities.size();
				for (int i = 0; i < num_entities; i++)
				{
					Entity ent = node->entities[i];
					const auto& ent_col = coordinator.GetComponent<ColliderComponent>(ent);

					CollisionFunc thisCollisionFunc = mCollisionTable[collider.shape][ent_col.shape];

					if (thisCollisionFunc != nullptr) 
					{
						if (thisCollisionFunc(collider, ent_col))
						{
							result.insert(ent);
						}
					}
				}
			}
			else // Not leaf node, recursively collect all intersecting shapes from the child nodes
			{
				for (int i = 0; i < 8; i++)
				{
					std::unordered_set<Entity> child = queryOctree(&(node->children[i]), collider);

					if (child.size() > 0)
					{
						result.insert(child.begin(), child.end());
					}
				}
			}
		}
	}

	return result;
}



std::unordered_set<Entity> CollisionSystem::queryRayOctree(OctreeNode* node, const Ray3& ray, const float rayLength)
{
	std::unordered_set<Entity> result;

	// Making temporary collider component for the bounds
	ColliderComponent bounds_col;
	bounds_col.shape = ShapeType::AABB_3D;
	bounds_col.collider.aabb = node->bounds;

    RaycastResult raycastResult;
    RaycastCollider(ray, raycastResult, bounds_col);

    // Only do things if the query collider intersects with node bounds (aabb)
    if (raycastResult.hit)// && raycastResult.t < rayLength)
    {
        // If leaf node return any entities which are intersecting with query box
        if (node->children == nullptr)
        {
            Coordinator& coordinator = GameManager::Instance().GetCoordinator();
            int num_entities = node->entities.size();
            for (int i = 0; i < num_entities; i++)
            {
                Entity ent = node->entities[i];
                const auto& ent_col = coordinator.GetComponent<ColliderComponent>(ent);

                raycastResult.Reset();
                RaycastCollider(ray, raycastResult, ent_col);

                if (raycastResult.hit && raycastResult.t < rayLength)
                {
                    result.insert(ent);
                }
            }
        }
        else // Not leaf node, recursively collect all intersecting shapes from the child nodes
        {
            for (int i = 0; i < 8; i++)
            {
                std::unordered_set<Entity> child = queryRayOctree(&(node->children[i]), ray, rayLength);

                if (child.size() > 0)
                {
                    result.insert(child.begin(), child.end());
                }
            }
        }
    }

	return result;
}


RaycastResult CollisionSystem::getClosestRaycast(const Ray3& ray, const float rayLength, const std::unordered_set<Entity>& ents_to_check)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    RaycastResult result;
    RaycastResult testResult;
    result.t = FLT_MAX;

    for (const auto& entityB : ents_to_check)
    {
        testResult.Reset();

        const auto& collB = coordinator.GetComponent<ColliderComponent>(entityB);

        RaycastCollider(ray, testResult, collB);

        if (testResult.hit && testResult.t < result.t)
            result = testResult;
    }

    return result;
}





void CollisionSystem::transformColliders()
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

	// Static colliders are transformed when they are added
	// Dynamic are transformed every frame
	// Transform world space colliders
	for (const auto& entity : mDynamicEntities)
	{
		auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);

		if (transform.IsChanged())
		{
			// Update world space
			auto& col = coordinator.GetComponent<ColliderComponent>(entity);

			col.UpdateWorldSpace(transform);

			// Update dynamic octree with changes
			updateOctreeNode(mDynamicOctree, entity);
		}
	}
}

// Going to first check dynamic vs static then dynamic vs dynamic
void CollisionSystem::checkCollisions()
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

	mCollisions.clear();
	mExitings.clear();
	std::unordered_set<Entity> dynamic_ents_checked;
    for (const auto& entityA : mDynamicEntities)
    {
		dynamic_ents_checked.insert(entityA); // inserting here so we don't need to check it against self

		auto& colA = coordinator.GetComponent<ColliderComponent>(entityA);

		// Inserting current collisions
		mCollisions[entityA].insert(colA.currentCollisions.begin(), colA.currentCollisions.end());

		if (colA.isActive)
		{
			// Dynamic vs static collision check
			std::unordered_set<Entity> ents_to_check_static = queryOctree(mStaticOctree, colA);
			for (const auto& entityB : ents_to_check_static)
			{
				auto& colB = coordinator.GetComponent<ColliderComponent>(entityB);
				if (colB.isActive)
				{
					// If collision add collision to collider A entities
					if (isCollision(colA, colB))
					{
						mCollisions[entityA].insert(entityB);
					}
					// If its a current collision but no collision that means it has exited
					else if (utility::containsValue(colA.currentCollisions, entityB))
					{
						mCollisions[entityA].erase(entityB);
						mExitings.push_back(ExitingPair{entityA, entityB});
						colA.currentCollisions.erase(entityB);
					}
				}
			}



			// Dynamic vs Dynamic
			// We only want to check collision between 2 colliders once not twice
			std::unordered_set<Entity> ents_to_check_dynamic = queryOctree(mDynamicOctree, colA);
			for (const auto& entityB : ents_to_check_dynamic)
			{
				// If the entityB already checked all its collisions ie was entityA before
				if (utility::containsValue(dynamic_ents_checked, entityB))
					continue;

				auto& colB = coordinator.GetComponent<ColliderComponent>(entityB);
				if (colB.isActive)
				{
					// If collision add collision to collider A entities
					if (isCollision(colA, colB))
					{
						mCollisions[entityA].insert(entityB);
						// mCollisions[entityB].insert(entityA);
					}
					// If its a current collision but no collision remove it from the current collisions
					// Not removing it from mCollisions so it can call its OnCollisionExit
					else if (utility::containsValue(colA.currentCollisions, entityB))
					{
						mCollisions[entityA].erase(entityB);
						mExitings.push_back(ExitingPair{entityA, entityB});
						colA.currentCollisions.erase(entityB);
					}
				}
			}

			
			// If a current entity wasn't in any of the "ents_to_check" so wasn't in the bounding volume hierarchy and its a currentCollision, then the collision has exited
			for (const auto& entityB : colA.currentCollisions)
			{
				if (!utility::containsValue(ents_to_check_static, entityB) && !utility::containsValue(ents_to_check_dynamic, entityB))
				{
					mCollisions[entityA].erase(entityB);
					mExitings.push_back(ExitingPair{entityA, entityB});
				}
			}
		}
    }
}


bool CollisionSystem::isCollision(const ColliderComponent& colA, const ColliderComponent& colB)
{
	// Checking if A collides with B
	CollisionFunc collisionFunc = mCollisionTable[colA.shape][colB.shape];

	if (collisionFunc == nullptr)
		return false;

	return collisionFunc(colA, colB);
}


void CollisionSystem::handleCollisions()
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

	// Exiting Collisions
	const int exitSize = mExitings.size();
	for (int i = 0; i < exitSize; i++)
	{
		Entity entA = mExitings[i].entA;
		Entity entB = mExitings[i].entB;
		auto& colA = coordinator.GetComponent<ColliderComponent>(entA);

		if (colA.onCollisionExit)
		{
			colA.onCollisionExit(entA, entB);
		}

		colA.currentCollisions.erase(entB);
	}


	// New and recurring collisions
	for (auto it = mCollisions.begin(); it != mCollisions.end(); it++)
	{
		Entity entA = it->first;
		auto& colA = coordinator.GetComponent<ColliderComponent>(entA);

		if (!colA.isActive)
			continue;
			
		for (const auto& entB : it->second)
		{
			auto& colB = coordinator.GetComponent<ColliderComponent>(entB);

			if (!colB.isActive)
			{
				if (colA.onCollisionExit)
				{
					colA.onCollisionExit(entA, entB);
				}
				colA.currentCollisions.erase(entB);
				continue;
			}

			// Getting the collision manifold
			CollisionManifold collision = getManifold(colA, colB);

			if (collision.isColliding)
			{
				// If STILL colliding
				if (utility::containsValue(colA.currentCollisions, entB))
				{
					if (colA.physicsResponse)
					{
						colA.physicsResponse(entA, entB, collision);
					}
					if (colA.onCollisionStay)
					{
						colA.onCollisionStay(entA, entB, collision);
					}
				}
				else // NEW collision
				{
					if (colA.physicsResponse)
					{
						colA.physicsResponse(entA, entB, collision);
					}
					if (colA.onCollisionEnter)
					{
						colA.onCollisionEnter(entA, entB, collision);
					}
					colA.currentCollisions.insert(entB);
				}

			}
			else // Manifold indicates not a proper collision (this is rough but works lol)
			{
				// If they were colliding
				if (utility::containsValue(colA.currentCollisions, entB))
				{
					if (colA.onCollisionExit)
					{
						colA.onCollisionExit(entA, entB);
					}
					colA.currentCollisions.erase(entB);
				}

			}
		}
	}
}


CollisionManifold CollisionSystem::getManifold(const ColliderComponent& colA, const ColliderComponent& colB)
{
	ManifoldFunc manifoldFunc = mManifoldTable[colA.shape][colB.shape];

	// A default manifold
	if (manifoldFunc == nullptr)
	{
		// It only checks for manifold if shapes are intersecting
		// If there is no manifold defined just going to return a default manifold with isColliding set to true
		CollisionManifold manifold;
		manifold.isColliding = true;
		return manifold;
	}

	return manifoldFunc(colA, colB);
}


CollisionManifold CollisionSystem::getClosestManifold(const ColliderComponent& collider, const std::unordered_set<Entity> ents_to_check)
{
    Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    CollisionManifold closest;
    CollisionManifold test;
    for (const auto& entity : ents_to_check)
    {
        const auto& other_col = coordinator.GetComponent<ColliderComponent>(entity);

        test = getManifold(collider, other_col);
        if (test.isColliding && test.depth < closest.depth)
            closest = test;
    }

    return closest;
}
