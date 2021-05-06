#pragma once
#include "Entity.h"
#include "GlobalState.h"
#include "physics/Physics.h"


class Projectile : public Entity
{
	public:
		Projectile();
		void attachPhysics(Physics& instance) override;
		void draw(Physics& instance, const ShaderProgram& shader, const Camera& camera, bool depth) override;
		void simulate(Physics& instance) override;
		void cleanUpPhysics() override;

		void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicle = vehicle; }

		bool shouldBeDeleted = false; // Set to true when the projectile should be removed from the world

	private:
		int mID;
		PxVehicleDrive4W* mVehicle;

		float activationTimer = 0.0f;
		constexpr static float ACTIVATION_TIME = 0.03f * 60.0f; // The projectile is not active for ~ second to prevent self collisions
	
		float removalTimer = 0;
		constexpr static float REMOVAL_TIME = 5.0f * 60.0f; // The projectile should only exist in the game world for ~5 seconds before being deleted
};

