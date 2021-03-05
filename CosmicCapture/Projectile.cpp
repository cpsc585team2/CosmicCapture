#include "Projectile.h"



Projectile::Projectile(const ShaderProgram& shaderProgram, std::shared_ptr<Camera> camera) :
	Entity("models/projectile_sphere.ply", "textures/blue.jpg", shaderProgram, camera)
{}

void Projectile::attachPhysics(Physics& instance) {
	PxShape* projectile = instance.gPhysics->createShape(PxSphereGeometry(1.f), *instance.gMaterial, true); //create projectile shape
	projectile->setSimulationFilterData(PxFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST, 0, 0));
	PxVec3 pos = mVehicles[0]->getRigidDynamicActor()->getGlobalPose().p;
	PxVec3 dir = mVehicles[0]->getRigidDynamicActor()->getLinearVelocity();
	State::projectileBody = instance.gPhysics->createRigidDynamic(PxTransform(PxVec3(pos.x, pos.y + 5.f, pos.z)));
	State::projectileBody->attachShape(*projectile);
	projectile->release();
	State::projectileBody->setAngularDamping(0.0f); //I failed highschool physics idk what this means
	State::projectileBody->setLinearVelocity(PxVec3(dir.x * 10, dir.y * 10, dir.z * 10));
	instance.gScene->addActor(*State::projectileBody);
}

void Projectile::draw(Physics& instance) {
	PxTransform transform = State::projectileBody->getGlobalPose();
	PxMat44 modelMatrix(transform);
	mGeometry->draw(modelMatrix);
}
void Projectile::simulate(Physics& instance) {
}
void Projectile::cleanUpPhysics() {
	PX_RELEASE(State::projectileBody);
}