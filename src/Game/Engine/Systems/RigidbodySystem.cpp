#include "RigidBodySystem.h"
#include "../Core/GameManager.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/Transform3DComponent.h"
#include "../Components/ColliderComponent.h"
#include <cmath>
#include <string>


// Giving collider if any the correct physics response
void RigidBodySystem::OnEntityAdded(Entity entity)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

	// Just seeing if they have a collidercomponent im guessing I just have to make sure the collider is added first
	// There will also be a public function to set the physics response
	if (coordinator.HasComponent<ColliderComponent>(entity))
	{
		// utility::log("Giving rigidbody physicsresponse");
		auto& col = coordinator.GetComponent<ColliderComponent>(entity);

		col.physicsResponse = [this](Entity self, Entity other, const CollisionManifold& collision) {
			this->physicsResponse(self, other, collision);
		};
	}
}


// If has collider, remove the physics response
void RigidBodySystem::OnEntityRemoved(Entity entity)
{
    mCollisions.erase(entity);
    mDeleted.insert(entity);
	// Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	// if (coordinator.HasComponent<ColliderComponent>(entity))
	// {
	// 	auto& col = coordinator.GetComponent<ColliderComponent>(entity);
	//
	// 	col.physicsResponse = nullptr;
	// }
}


void RigidBodySystem::Update(float deltaTime)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	// Applying forces
	for (const auto& entity : mEntities)
	{
		auto& rigidbody = coordinator.GetComponent<RigidBodyComponent>(entity);

        if (rigidbody.isAwake)
            applyForces(rigidbody);
	}

	// Applying impulses to objects that are colliding
	for (int k = 0; k < mImpulseIteration; k++)
	{
		for (int i = 0; i < mImpulses.size(); i++)
		{
			int jSize = mImpulses[i].collision.contacts.size();
			for (int j = 0; j < jSize; j++)
			{
				applyImpulse(mImpulses[i], j);
			}
		}
	}

    // Performing Linear Projection to fix any sinking issues that might occur
	int size = mImpulses.size();
	for (int i = 0; i < size; i++)
	{
		ImpulseData impData = mImpulses[i];

        if (utility::containsValue(mDeleted, impData.entA) || utility::containsValue(mDeleted, impData.entB))
        {
            continue;
        }

		auto& rbA = coordinator.GetComponent<RigidBodyComponent>(impData.entA);
		auto& rbB = coordinator.GetComponent<RigidBodyComponent>(impData.entB);

		float invMassA = rbA.GetInvMass();
		float invMassB = rbB.GetInvMass();
		float totalMass = invMassA + invMassB;

		if (CMP_FLOAT(totalMass, 0.f))
			continue; // Infinite mass

		// Correction amount is based on penetration depth, slack, and amount of linear projection
		float depth = std::fmaxf(impData.collision.depth - mPenetrationSlack, 0.f);
		float scalar;
		scalar = depth / totalMass;

		Vector3 correction = impData.collision.normal * scalar * mLinearProjectionPercent;

		// Applying correction to position of both rbs
		if (rbA.isStatic == false)
		{
			auto& transformA = coordinator.GetComponent<Transform3DComponent>(impData.entA);
            Vector3 oldPos = transformA.GetPosition();
			transformA.SetPosition(transformA.GetPosition() - (correction * invMassA));

            // // Adjust velocity to match position change
            Vector3 posDelta = transformA.GetPosition() - oldPos;
            rbA.velocity += posDelta * deltaTime;
		}
		if (rbB.isStatic == false)
		{
			auto& transformB = coordinator.GetComponent<Transform3DComponent>(impData.entB);
            Vector3 oldPos = transformB.GetPosition();
			transformB.SetPosition(transformB.GetPosition() + (correction * invMassB));

            // // Adjust velocity to match position change
            Vector3 posDelta = transformB.GetPosition() - oldPos;
            rbB.velocity += posDelta * deltaTime;
		}

		// Solving constraints would go here but I don't have any constraints
	}

    // utility::log("rb works");

	// Integrate position and velocity of every rigidbody
	for (const auto& entity : mEntities)
	{
		auto& rigidbody = coordinator.GetComponent<RigidBodyComponent>(entity);

        if (!rigidbody.isAwake)
        {
            rigidbody.sleepTimer.Increment(deltaTime);
            if (rigidbody.sleepTimer.IsFinished())
            {
                rigidbody.isAwake = true;
                rigidbody.awakeTimer.Reset();
            }
        }
        else
        {
            rigidbody.awakeTimer.Increment(deltaTime);
        }

		if (rigidbody.isStatic)
			continue;


		auto& transform = coordinator.GetComponent<Transform3DComponent>(entity);

		// Integrate Linear forces into Linear velocity and apply dampening to simulate air friction
		Vector3 acceleration = rigidbody.forces * rigidbody.GetInvMass();
		rigidbody.velocity = rigidbody.velocity + acceleration * deltaTime;
		rigidbody.velocity = rigidbody.velocity * mDamping;

		// if (rigidbody.velocity.x > 100.f || rigidbody.velocity.x < -100.f)

		// Some clamps
		if (fabsf(rigidbody.velocity.x) < 0.001f)
			rigidbody.velocity.x = 0.0f;
		if (fabsf(rigidbody.velocity.y) < 0.001f)
			rigidbody.velocity.y = 0.0f;
		if (fabsf(rigidbody.velocity.z) < 0.001f)
			rigidbody.velocity.z = 0.0f;

		// Integrate Linear velocity with respect to time into position
		transform.SetPosition(transform.GetPosition() + (rigidbody.velocity * deltaTime));

		if (coordinator.HasComponent<ColliderComponent>(entity))
		{
			const auto& collider = coordinator.GetComponent<ColliderComponent>(entity);

			if (collider.shape == ShapeType::OBB_3D || collider.shape == ShapeType::SPHERE_3D)
			{
				// Integrate angular forces into Angular Velocity
				Vector3 angAccel = rigidbody.GetInvTensor(collider).GetMultiplyVector3(rigidbody.torques);
				rigidbody.angularVelocity = rigidbody.angularVelocity + angAccel * deltaTime;
				rigidbody.angularVelocity = rigidbody.angularVelocity * mDamping;

				if (fabsf(rigidbody.angularVelocity.x) < 0.001f)
					rigidbody.angularVelocity.x = 0.0f;
				if (fabsf(rigidbody.angularVelocity.y) < 0.001f)
					rigidbody.angularVelocity.y = 0.0f;
				if (fabsf(rigidbody.angularVelocity.z) < 0.001f)
					rigidbody.angularVelocity.z = 0.0f;


				// Integrate angular velocity into orientation
				transform.SetRotationRAD(transform.GetRotationRAD() + rigidbody.angularVelocity * deltaTime);
			}


            // If velocities below threshold, awake timer is done, and its colliding, then we can put to sleep
            if (shouldSleep(rigidbody) && rigidbody.awakeTimer.IsFinished() && utility::containsValue(mCollisions, entity))
            {
                rigidbody.sleepTimer.Reset();
                rigidbody.isAwake = false;
                rigidbody.velocity = Vector3(0, 0, 0);
                rigidbody.angularVelocity = Vector3(0, 0, 0);
                rigidbody.forces = Vector3(0, 0, 0);
            }
		}
	}

	mImpulses.clear();
    mCollisions.clear();
    mDeleted.clear();
}

Vector3 RigidBodySystem::GetGravity() const
{
	return mGravity;
}

void RigidBodySystem::SetGravity(const Vector3& gravity)
{
	mGravity = gravity;
}

float RigidBodySystem::GetDamping() const
{
	return mDamping;
}

void RigidBodySystem::SetDamping(float damping)
{
	mDamping = damping;
}

// Private methods
void RigidBodySystem::applyForces(RigidBodyComponent& rb)
{
	rb.forces = mGravity * rb.mass;
}


// Resolves the penetration of 2 objects
// See https://physics.info/momentum/summary.shtml to understand how to derive magnitude of impulse
void RigidBodySystem::applyImpulse(const ImpulseData& impData, int c)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();

    if (utility::containsValue(mDeleted, impData.entA) || utility::containsValue(mDeleted, impData.entB))
    {
        return;
    }

	auto& rbA = coordinator.GetComponent<RigidBodyComponent>(impData.entA);
	auto& rbB = coordinator.GetComponent<RigidBodyComponent>(impData.entB);
	auto& transformA = coordinator.GetComponent<Transform3DComponent>(impData.entA);
	auto& transformB = coordinator.GetComponent<Transform3DComponent>(impData.entB);
	bool hasCollider = coordinator.HasComponent<ColliderComponent>(impData.entA) && coordinator.HasComponent<ColliderComponent>(impData.entB);

    // If there was an impulse, awake sleeping entity
    // if (!rbA.isAwake)
    // {
    //     rbA.isAwake = true;
    //     rbA.sleepTimer.Reset();
    // }
    // if (!rbB.isAwake)
    // {
    //     rbB.isAwake = true;
    //     rbB.sleepTimer.Reset();
    // }


	// Linear velocity
	float invMass1 = rbA.GetInvMass();
	float invMass2 = rbB.GetInvMass();
	float invMassSum = invMass1 + invMass2;
	if (CMP_FLOAT(invMassSum, 0.f))
		return;

	Vector3 r1, r2, relativeVel;
	Mat4x4 i1, i2;
	// Point of contact relative to center of mass
	r1 = impData.collision.contacts[c] - transformA.GetPosition();
	r2 = impData.collision.contacts[c] - transformB.GetPosition();

	if (hasCollider)
	{
		i1 = rbA.GetInvTensor(coordinator.GetComponent<ColliderComponent>(impData.entA));
		i2 = rbB.GetInvTensor(coordinator.GetComponent<ColliderComponent>(impData.entB));
	}
	else
	{
		i1.SetIdentity();
		i2.SetIdentity();
	}

	// Relative velocity
	relativeVel = (rbB.velocity + rbB.angularVelocity.GetCrossProduct(r2)) - (rbA.velocity + rbA.angularVelocity.GetCrossProduct(r1));
	// relativeVel = rbB.velocity - rbA.velocity;

	// Relative collision normal
	Vector3 relativeNorm = impData.collision.normal;
	relativeNorm.Normalize();
	// Moving away from each other? Do nothing! (since no collision can occur)
	if (relativeVel.GetDotProduct(relativeNorm) > 0.f)
		return;


	// if (rbA.isStatic && !rbB.isStatic)
	// {
	// 	relativeNorm = relativeNorm * -1;
	// }

	// j is the magnitude of the impulse needed to resolve the collision
	// calculating j per contact
	float e = fminf(rbA.restitution, rbB.restitution);
	float numerator = (-(1.f + e) * relativeVel.GetDotProduct(relativeNorm));
	float d1 = invMassSum;
	Vector3 d2, d3;
	float denominator;
	d2 = i1.GetMultiplyVector3(r1.GetCrossProduct(relativeNorm));
	d2 = d2.GetCrossProduct(r1);
	d3 = i2.GetMultiplyVector3(r2.GetCrossProduct(relativeNorm));
	d3 = d3.GetCrossProduct(r2);
	denominator = d1 + relativeNorm.GetDotProduct(d2 + d3);
	// denominator = d1;

	float j;
	if (CMP_FLOAT(denominator, 0.f))
		j = 0.f;
	else
		j = numerator / denominator;

	if (impData.collision.contacts.size() > 0 && j != 0.f)
	{
		j /= (float)impData.collision.contacts.size();
	}

	// utility::log("j: " + std::to_string(j));

	// Multiply the collision normal by magnitude of impulse and apply result to velocity of each colliding bodies
	// This is how we apply Linear Impulse to rigidbodies
	// Updating both linear and angular velocities
	Vector3 impulse = relativeNorm * j;

	if (!rbA.isStatic)
	{
		rbA.velocity = rbA.velocity - (impulse * invMass1);
		rbA.angularVelocity = rbA.angularVelocity - i1.GetMultiplyVector3(r1.GetCrossProduct(impulse));


	}
	if (!rbB.isStatic)
	{
		rbB.velocity = rbB.velocity + (impulse * invMass2);
		rbB.angularVelocity = rbB.angularVelocity + i2.GetMultiplyVector3(r2.GetCrossProduct(impulse));
	}

	// Tangent vector for friction
	Vector3 t = relativeVel - (relativeNorm * relativeVel.GetDotProduct(relativeNorm)); // Tangential vector
	if (CMP_FLOAT(t.GetMagnitudeSquared(), 0.f))
		return;

	t.Normalize(); 

	// Find magnitude of the tangential impulse, same as j process but replace every instance of collision normal with collision tangent t
	numerator = -1 * relativeVel.GetDotProduct(t);
	d1 = invMassSum;
	d2 = i1.GetMultiplyVector3(r1.GetCrossProduct(t));
	d2 = d2.GetCrossProduct(r1);
	d3 = i2.GetMultiplyVector3(r2.GetCrossProduct(t));
	d3 = d3.GetCrossProduct(r2);
	denominator = d1 + t.GetDotProduct(d2 + d3);
	// denominator = d1;
	
	float jt;
	if (CMP_FLOAT(denominator, 0.f))
		jt = 0.f;
	else
		jt = numerator / denominator;

	if (impData.collision.contacts.size() > 0.f && jt != 0.f)
	{
		jt /= (float)impData.collision.contacts.size();
	}
	if (CMP_FLOAT(jt, 0.f))
		return;

	// Clamping magnitude of friction between
	// -j * friction and j * friction
	// Property of friction is called Coulomb's Law
	float friction = std::sqrtf(rbA.friction * rbB.friction);
	if (jt > j * friction)
		jt = j * friction;
	else if (jt < -j * friction)
		jt = -j * friction;


	if (jt > 100.f)
		utility::log("jt: " + std::to_string(jt));

	// Apply tangential impulse (friction) to the linear and angular velocity of each rb
	Vector3 tangentImpulse = t * jt;

	if (!rbA.isStatic)
	{
		rbA.velocity = rbA.velocity - (tangentImpulse * invMass1);
		rbA.angularVelocity = rbA.angularVelocity - i1.GetMultiplyVector3(r1.GetCrossProduct(tangentImpulse));

        if (!shouldSleep(rbA))
        {
            rbA.isAwake = true;
            rbA.awakeTimer.Reset();
        }
	}

	if (!rbB.isStatic)
	{
		rbB.velocity = rbB.velocity + (tangentImpulse * invMass2);
		rbB.angularVelocity = rbB.angularVelocity + i2.GetMultiplyVector3(r2.GetCrossProduct(tangentImpulse));

        if (!shouldSleep(rbB))
        {
            rbB.isAwake = true;
            rbB.awakeTimer.Reset();
        }
	}
}


void RigidBodySystem::addRotationalImpulse(Entity ent, const Vector3& point, const Vector3& impulse)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
	if (coordinator.HasComponent<ColliderComponent>(ent) == false)
		return;

	const auto& transform = coordinator.GetComponent<Transform3DComponent>(ent);
	const auto& collider = coordinator.GetComponent<ColliderComponent>(ent);
	auto& rb = coordinator.GetComponent<RigidBodyComponent>(ent);

	if (rb.mass == 0.f)
		return;

	Vector3 centerOfMass = transform.GetPosition();
	Vector3 torque = (point - centerOfMass).GetCrossProduct(impulse);

	// Immediately change angular vel by some acceleration
	Vector3 angAccel = rb.GetInvTensor(collider).GetMultiplyVector3(torque);
	rb.angularVelocity = rb.angularVelocity + angAccel;
}


// Just creates the Impulse data to be applied during the update phase
// TODO: Add impulse data if colliding with character body (only rb should be effected!)
void RigidBodySystem::physicsResponse(Entity self, Entity other, const CollisionManifold& collision)
{
	Coordinator& coordinator = GameManager::Instance().GetCoordinator();
    
    if (!coordinator.HasComponent<RigidBodyComponent>(self))
        return;

	if (coordinator.HasComponent<RigidBodyComponent>(other))
	{
		if (collision.isColliding)
		{
			// utility::log(std::to_string(collision.depth));
			mImpulses.push_back(ImpulseData{self, other, collision});
            mCollisions.insert(self);
            mCollisions.insert(other);
		}
		else
		{
			utility::log("False alarm!");
		}
	}
}


bool RigidBodySystem::shouldSleep(RigidBodyComponent& rb) const
{
    return (rb.velocity.GetMagnitudeSquared() < rb.sleepVelocityThreshold*rb.sleepVelocityThreshold && rb.angularVelocity.GetMagnitudeSquared() < rb.sleepAngularVelocityThreshold*rb.sleepAngularVelocityThreshold);// && rb.sleepTimer.IsFinished())
}
