#pragma once
#include "Input.h"
#include "Pathfinding.h"
#include "GlobalState.h"
#include <algorithm>

class OpponentInput
{
public:
	OpponentInput();
	OpponentInput(int playerNumber);
	std::map<MovementFlags, bool> getInput(PxVec3 playerPos, PxVec3 playerDir);
	//int getOrientation(PxVec3 dirVec);
	void updatePath(PxVec3 playerPos, PxVec3 targetPos);
	PxVec3 getPlayerToTargetDir(PxVec3 playerDirVec, int playerVehicleRDIndex, PxVec3 targetGlobalPos);
	PxVec3 getPlayerToTargetDir(PxVec3 playerDirVec, int playerVehicleRDIndex);
	int dirsToCommand(PxVec3 playerDirVec, PxVec3 targetDirVec);
	void setPlayerNum(int num);
	void attachVehicle(PxVehicleDrive4W* vehicle) { mVehicles.push_back(vehicle); }
private:
	std::vector<PxVehicleDrive4W*> mVehicles;
	std::map<MovementFlags, bool> followPath();
	std::pair<int, int> getGridCoordinates(float globalPosX, float globalPosZ);
	//std::map<MovementFlags, bool> getCommand(int playerDir, int targetDir);
	std::map<MovementFlags, bool> getCommand(int commandNum);
	//int getTargetDirection(std::pair<int, int> playerCoords, std::pair<int, int> targetCoords);
	//int checkDiagonals(std::pair<int, int> currentPos, int targetDir);
	

	int playerNum;
	int counter;
	std::stack<std::pair<int, int>> path;
	std::pair<int, int> target;
	Pathfinding pathfinder;

	bool reversing = false;
	int stuckCounter;
	float reverseThresholdSpeed = -6.f;
	int stuckThreshold = 250; //TUNING POINT
	std::pair<int, int> lastPosition;
};

