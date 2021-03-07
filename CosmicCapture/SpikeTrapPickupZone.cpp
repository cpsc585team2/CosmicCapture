#include "SpikeTrapPickupZone.h"
#include "GlobalState.h"
#include "physics/VehicleFilterShader.h"

SpikeTrapPickupZone::SpikeTrapPickupZone(const ShaderProgram& shader, const std::shared_ptr<Camera>& camera):
	Entity("models/projectile_sphere.ply", "textures/pink.jpg", shader, camera)
{}

void SpikeTrapPickupZone::attachPhysics(Physics& instance)
{
	PxShape* speedboostPickupMarker = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true);
	speedboostPickupMarker->setSimulationFilterData(PxFilterData(COLLISION_FLAG_SCENERY, COLLISION_FLAG_SCENERY_AGAINST, 0, 0));
	pickupBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(40.f, MIN_HEIGHT, 25.f)));
	pickupBody->attachShape(*speedboostPickupMarker);
	speedboostPickupMarker->release();
	instance.gScene->addActor(*pickupBody);

	PxShape* speedboostPickupTriggerShape = instance.gPhysics->createShape(PxBoxGeometry(1.1f, 2.f, 1.1f), *instance.gMaterial, true);
	//trigger box for picking up the flag
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	speedboostPickupTriggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	State::spikeTrapPickupTriggerBody = instance.gPhysics->createRigidStatic(PxTransform(PxVec3(40.f, 2.f, 25.f)));
	State::spikeTrapPickupTriggerBody->attachShape(*speedboostPickupTriggerShape);
	instance.gScene->addActor(*State::spikeTrapPickupTriggerBody);
}

void SpikeTrapPickupZone::draw(Physics& instance, const ShaderProgram& depthTexture, bool depth, const unsigned& depthMap)
{
	PxTransform transform = pickupBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix, depthTexture, depth, depthMap);
}

void SpikeTrapPickupZone::simulate(Physics& instance)
{
	PxVec3 pos = pickupBody->getGlobalPose().p;
	if (ascending) {
		pickupBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y + 0.01f, pos.z)));
		if (pos.y + 0.1f >= MAX_HEIGHT) {
			ascending = false;
		}
	}
	else {
		pickupBody->setGlobalPose(PxTransform(PxVec3(pos.x, pos.y - 0.01f, pos.z)));
		if (pos.y - 0.1f <= MIN_HEIGHT) {
			ascending = true;
		}
	}
}

void SpikeTrapPickupZone::cleanUpPhysics()
{
	PX_RELEASE(pickupBody);
	PX_RELEASE(State::spikeTrapPickupTriggerBody);
}