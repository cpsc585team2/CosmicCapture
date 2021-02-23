#pragma once

#include <map>

#include "Entity.h"
#include "physics/Physics.h"
#include "input.h"

// TODO: Might want to think about a better place to store these definitions (so they are not longer globals)
inline PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f, //rise rate eANALOG_INPUT_ACCEL
		6.0f, //rise rate eANALOG_INPUT_BRAKE		
		6.0f, //rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f, //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, //fall rate eANALOG_INPUT_ACCEL
		10.0f, //fall rate eANALOG_INPUT_BRAKE		
		10.0f, //fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f, //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f, //rise rate eANALOG_INPUT_ACCEL
		6.0f, //rise rate eANALOG_INPUT_BRAKE		
		6.0f, //rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f, //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f, //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f, //fall rate eANALOG_INPUT_ACCEL
		10.0f, //fall rate eANALOG_INPUT_BRAKE		
		10.0f, //fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f, //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

inline PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 0.75f,
	5.0f, 0.75f,
	30.0f, 0.125f,
	120.0f, 0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
inline PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);


class Vehicle : public Entity
{
public:
	Vehicle(std::unique_ptr<Model> model) : Entity(std::move(model)) {}
	void attachPhysics(Physics& instance) override;

	void draw(Physics& instance) override;
	void simulate(Physics& instance) override;
	void processInput(const std::map<MovementFlags, bool>& inputs);

	void cleanUpPhysics() override;

private:
	PxVehicleDrive4W* mVehicle4W;
	bool mIsVehicleInAir = true;
	bool mInReverseMode = false;
};

inline void Vehicle::cleanUpPhysics()
{
	mVehicle4W->getRigidDynamicActor()->release();
	mVehicle4W->free();
}

