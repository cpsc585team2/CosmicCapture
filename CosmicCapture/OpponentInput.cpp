#include "OpponentInput.h"

OpponentInput::OpponentInput()
{
	playerNum = 0;
	counter = 0;
	stuckCounter = 0;
}

OpponentInput::OpponentInput(int playerNumber)
{
	playerNum = playerNumber;
	counter = 0;
	stuckCounter = 0;
}


std::map<MovementFlags, bool> OpponentInput::getInput(PxVec3 playerPos, PxVec3 playerDir)
{
	bool sharpTurn = false;
	float vehicleSpeed = mVehicles[0]->computeForwardSpeed();
	std::pair<int, int> current = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<float, float> centre = State::worldGridCenterCoords[target.first][target.second];
	double dist = std::sqrt(std::pow(playerPos.x - centre.first, 2) + std::pow(playerPos.z - centre.second, 2));
	double distToGoal = std::sqrt(std::pow(playerPos.x - goalPos.x, 2) + std::pow(playerPos.z - goalPos.z, 2));
	if (distToGoal < 25.f)
	{
		subTargetting = true;
	}
	else
	{
		subTargetting = false;
	}
	if (subTargetting)
	{
		if (distToGoal < 1.f)
		{
			subTargetting = false;
			if (path.empty())
			{
				path = pathfinder.aStarSearch(State::worldGrid, current, getGridCoordinates(goalPos.x, goalPos.z));
			}
			target = path.top();
			path.pop();
		}
	}
	else
	{
		if (dist < 10.f)
		{
			if (path.empty())
			{
				path = pathfinder.aStarSearch(State::worldGrid, current, getGridCoordinates(goalPos.x, goalPos.z));
			}
			target = path.top();
			path.pop();
		}
	}


	if (vehicleSpeed < 0.5f && !reversing)
	{
		if (stuckCounter >= stuckThreshold)
		{
			reversing = true;
		}
		else
		{
			stuckCounter++;
		}
	}
	else
	{
		stuckCounter = 0;
		lastPosition = current;
	}

	std::map<MovementFlags, bool> command;
	if (reversing)
	{
		command[MovementFlags::LEFT] = true;
		command[MovementFlags::RIGHT] = true;
		command[MovementFlags::DOWN] = false;
		command[MovementFlags::UP] = true;
		framesReversing++;
		if (vehicleSpeed < reverseThresholdSpeed || framesReversing > reversingMax)
		{
			reversing = false;
			framesReversing = 0;
		}
	}
	else
	{
		if (target.first == -1 && target.second == -1)
		{
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = true;
			command[MovementFlags::UP] = false;
		}
		else if (target.first == -2 && target.second == -2)
		{
			command[MovementFlags::LEFT] = true;
			command[MovementFlags::RIGHT] = true;
			command[MovementFlags::DOWN] = true;
			command[MovementFlags::UP] = true;
		}
		else
		{
			PxVec3 targetDir;
			if (subTargetting)
			{
				targetDir = getPlayerToTargetDir(playerNum, goalPos);
			}
			else
			{
				targetDir = getPlayerToTargetDir(playerNum);
			}
			command = getCommand(dirsToCommand(playerDir, targetDir, &sharpTurn));
		}
	}

	command[MovementFlags::ACTION] = true;
	if (!recentlyUsedAction)
	{
		if (State::heldPowerUps[playerNum] == PowerUpOptions::SPIKE_TRAP)
		{
			command[MovementFlags::ACTION] = false;
			recentlyUsedAction = true;
		}
		else if (State::heldPowerUps[playerNum] == PowerUpOptions::SPEED_BOOST)
		{
			command[MovementFlags::ACTION] = false;
			recentlyUsedAction = true;
		}
		else if (State::heldPowerUps[playerNum] == PowerUpOptions::PROJECTILE)
		{
			if (State::flagPickedUp)
			{
				if (State::flagPickedUpBy[playerNum])
				{
				}
				else
				{
					if (subTargetting && pointingAtGoal(playerDir, getPlayerToTargetDir(playerNum, goalPos)))
					{
						command[MovementFlags::ACTION] = false;
						recentlyUsedAction = true;
					}
				}
			}
		}
	}
	else
	{
		actionCounter++;
		if (actionCounter > actionDelay)
		{
			actionCounter = 0;
			recentlyUsedAction = false;
		}
	}
	return command;
}


void OpponentInput::updatePath(PxVec3 playerPos, PxVec3 targetPos)
{
	std::pair<int, int> p = getGridCoordinates(playerPos.x, playerPos.z);
	std::pair<int, int> t = getGridCoordinates(targetPos.x, targetPos.z);
	goalPos = targetPos;
	if (p == t)
	{
	}
	else if (State::worldGrid[p.first][p.second] == 0 || State::worldGrid[t.first][t.second] == 0)
	{
	}
	else
	{
		path = pathfinder.aStarSearch(State::worldGrid, p, t);
		if (path.size() > 1)
		{
			path.pop();
		}
		target = path.top();
		path.pop();
	}
}

std::map<MovementFlags, bool> OpponentInput::followPath()
{
	std::map<MovementFlags, bool> inputMap;
	return inputMap;
}

std::pair<int, int> OpponentInput::getGridCoordinates(float globalPosX, float globalPosZ) const
{
	int xIndex = std::min(static_cast<int>((globalPosX + 325.f) / 25.f), 25);
	int zIndex = std::min(static_cast<int>((globalPosZ + 325.f) / 25.f), 25);
	std::pair p(xIndex, zIndex);
	return p;
}


PxVec3 OpponentInput::getPlayerToTargetDir(int playerVehicleRDIndex, PxVec3 targetGlobalPos)
{
	float targetX = targetGlobalPos.x;
	float targetZ = targetGlobalPos.z;
	PxVec3 playerPos = State::vehicles[playerVehicleRDIndex]->getRigidDynamicActor()->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();
}

PxVec3 OpponentInput::getPlayerToTargetDir(int playerVehicleRDIndex) const
{
	float targetX = State::worldGridCenterCoords[target.first][target.second].first;
	float targetZ = State::worldGridCenterCoords[target.first][target.second].second;
	PxVec3 playerPos = State::vehicles[playerVehicleRDIndex]->getRigidDynamicActor()->getGlobalPose().p;
	float playerX = playerPos.x;
	float playerZ = playerPos.z;

	float shiftedTargetX = targetX - playerX;
	float shiftedTargetZ = targetZ - playerZ;

	return PxVec3(shiftedTargetX, 0.f, shiftedTargetZ).getNormalized();
}

bool OpponentInput::pointingAtGoal(PxVec3 playerDirVec, PxVec3 targetDirVec) const
{
	bool pointingAtGoal = false;
	float playerX = playerDirVec.x;
	float playerZ = playerDirVec.z;
	float targetX = targetDirVec.x;
	float targetZ = targetDirVec.z;
	float playerAngleRads = atan2(playerZ, playerX);
	float targetAngleRads = atan2(targetZ, targetX);
	float diff = abs(playerAngleRads - targetAngleRads);
	if (diff < 0.1f)
	{
		pointingAtGoal = true;
	}
	return pointingAtGoal;
}

int OpponentInput::dirsToCommand(PxVec3 playerDirVec, PxVec3 targetDirVec, bool* sharpTurnFlag) const
{
	float playerX = playerDirVec.x;
	float playerZ = playerDirVec.z;
	float targetX = targetDirVec.x;
	float targetZ = targetDirVec.z;
	float playerAngleRads = atan2(playerZ, playerX);
	float targetAngleRads = atan2(targetZ, targetX);
	int commandNum = 1;
	float diff = abs(playerAngleRads - targetAngleRads);
	if (diff < 0.175f)
	{
		//TUNING POINT -- magic number = how close you have to be pointing to your target to go straight
		return commandNum;
	}
	if (diff > 1.f)
	{
		*sharpTurnFlag = true;
	}

	if (playerAngleRads > targetAngleRads)
	{
		if (static_cast<double>(playerAngleRads) - targetAngleRads > M_PI)
			commandNum = 2;
		else
			commandNum = 3;
	}
	else
	{
		if (static_cast<double>(targetAngleRads) - playerAngleRads > M_PI)
			commandNum = 3;
		else
			commandNum = 2;
	}
	return commandNum;
}

std::map<MovementFlags, bool> OpponentInput::getCommand(int commandNum)
{
	std::map<MovementFlags, bool> inputMap;
	if (commandNum == 1)
	{
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	else if (commandNum == 3)
	{
		inputMap[MovementFlags::LEFT] = false;
		inputMap[MovementFlags::RIGHT] = true;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	else if (commandNum == 2)
	{
		inputMap[MovementFlags::LEFT] = true;
		inputMap[MovementFlags::RIGHT] = false;
		inputMap[MovementFlags::DOWN] = true;
		inputMap[MovementFlags::UP] = false;
	}
	return inputMap;
}

void OpponentInput::setPlayerNum(int num)
{
	playerNum = num;
}

