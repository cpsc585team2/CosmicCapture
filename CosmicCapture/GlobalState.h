#pragma once

#include <physx/PxPhysicsAPI.h>
#include <optional>
#include <array>
#include <map>
#include "./audio/AudioEngine.h"


using namespace physx;

enum class PowerUpOptions
{
	SPIKE_TRAP, SPEED_BOOST, PROJECTILE
};

struct spikeTrapState
{
	PxRigidStatic* triggerBody = nullptr; // The trigger body for the spike trap
	bool active = false; // Spike trap has been deployed and is ready to catch people
	bool inUse = false; // The spike trap has caught somebody
	int actingUpon; // Which car the spike trap caught
};

struct projectileState
{
	PxRigidDynamic* body = nullptr; // The body of the projectile
	bool active = false; // True when the projectile can blow up cars
};


struct State 
{
	inline static int worldGrid[26][26] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 9, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 9, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 9, 1, 1, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

	};
	inline static std::pair<float, float> worldGridCenterCoords[26][26];


	inline static bool flagPickedUp = false;
	inline static std::array<bool, 4> flagPickedUpBy = { false, false, false, false };
	inline static PxRigidStatic* flagPickupBox = nullptr;
	inline static PxRigidDynamic* flagBody = nullptr;
	inline static PxRigidStatic* flagDropoffBoxes[4] = { nullptr, nullptr, nullptr, nullptr };
	inline static bool resetFlag = false;

	inline static PxVehicleDrive4W* vehicles[4] = { nullptr, nullptr, nullptr, nullptr };

	//Human players
	inline static int numHumanPlayers = 0;

	// Scores and powerups
	inline static std::array<int, 4> scores = {0, 0, 0, 0};
	inline static int winScore = 3;
	inline static std::array<std::optional<PowerUpOptions>, 4> heldPowerUps = {};

	inline static std::map<int, PxRigidStatic*> spikeTrapPickupTriggerBodies;
	inline static std::map<int, PxRigidStatic*> projectilePickupTriggerBodies;
	inline static std::map<int, PxRigidStatic*> speedBoostPickupTriggerBodies;

	inline static std::map<int, spikeTrapState> spike_trap_states;
	inline static std::map<int, projectileState> projectileStates;

	inline static std::array<bool, 4> killCars = { false, false, false, false };

	inline static bool blueArena = false;
	inline static bool redArena = true;

	inline static bool canPickupFlag = true;
	inline static bool startPickupFlagTimer = false;
	inline static float flagTimer = 0.0f;

	inline static std::optional<int> slowCar;
};

struct Audio
{
	inline static AudioEngine soundSystem = AudioEngine();
	inline static AudioInstance music = soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	inline static AudioInstance engine = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	inline static AudioInstance collision = soundSystem.createInstance(audioConstants::SOUND_FILE_COLLISION);
	inline static AudioInstance projectile = soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE);
	inline static AudioInstance flag_pickup = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
	inline static AudioInstance projectile_pickup = soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
	inline static AudioInstance spike_trap_pickup = soundSystem.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
	inline static AudioInstance speed_boost_pickup = soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
	inline static AudioInstance flag_return = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);
	inline static AudioInstance speed_boost = soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST);
	inline static AudioInstance projectile_explosion = soundSystem.createInstance(audioConstants::SOUND_FILE_EXPLOSION);
	inline static AudioInstance car_crash = soundSystem.createInstance(audioConstants::SOUND_FILE_CRASH);
	inline static AudioInstance flag_lost = soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_LOST);
	inline static AudioInstance gate_switch = soundSystem.createInstance(audioConstants::SOUND_FILE_GATE_SWITCH);
	inline static AudioInstance caught = soundSystem.createInstance(audioConstants::SOUND_FILE_CAUGHT);

	inline static AudioInstance engine2 = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE2);
	inline static AudioInstance engine3 = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE3);
	inline static AudioInstance engine4 = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE4);

};
