#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <iostream>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "input.h"

#include "./audio/AudioEngine.h"

#include "./physics/ContactReportCallback.h"

#include "Camera.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "GameUI.h"
#include "DoorManager.h"
#include "PowerUpManager.h"

#include "OpponentInput.h"
#include "InvisibleBarrier.h"

#include "GlobalState.h"

float angle = -0.25f;
glm::vec2 g_scale = { 1.f, 1.f };
glm::vec2 g_pos = { 1.0f, 1.0f };
float scalingFactor = 3.0f;

void initializeGridCenterCoords() {
	float flatOffset = 10.f; //TUNING POINT
	for (int i = 0; i < 26; i++) {
		for (int j = 0; j < 26; j++) {
			State::worldGridCenterCoords[i][j].first = i * 25.f - 325.f + 12.5f;
			State::worldGridCenterCoords[i][j].second = j * 25.f - 325.f + 12.5f;
			if (i == 1 || j == 1 || j == 24 || i == 24) continue;
			if ((i + 1 < 26) && (i - 1 >= 0) && (j + 1 < 26) && (j - 1 >= 0)) {
				if (State::worldGrid[i + 1][j] == 0)
					State::worldGridCenterCoords[i][j].first -= flatOffset;
				if (State::worldGrid[i - 1][j] == 0)
					State::worldGridCenterCoords[i][j].first += flatOffset;
				if (State::worldGrid[i][j + 1] == 0)
					State::worldGridCenterCoords[i][j].second -= flatOffset;
				if (State::worldGrid[i][j - 1] == 0)
					State::worldGridCenterCoords[i][j].second += flatOffset;
			}
		}
	}
}

void updateWorldGridArena2() {
	State::worldGrid[6][1] = 1;
	State::worldGrid[6][2] = 1;
	State::worldGrid[6][23] = 1;
	State::worldGrid[6][24] = 1;
	State::worldGrid[7][8] = 1;
	State::worldGrid[7][9] = 1;
	State::worldGrid[7][10] = 1;
	State::worldGrid[7][15] = 1;
	State::worldGrid[7][16] = 1;
	State::worldGrid[7][17] = 1;
	State::worldGrid[19][1] = 1;
	State::worldGrid[19][2] = 1;
	State::worldGrid[19][23] = 1;
	State::worldGrid[19][24] = 1;
	State::worldGrid[18][8] = 1;
	State::worldGrid[18][9] = 1;
	State::worldGrid[18][10] = 1;
	State::worldGrid[18][15] = 1;
	State::worldGrid[18][16] = 1;
	State::worldGrid[18][17] = 1;
	State::worldGrid[10][12] = 1;
	State::worldGrid[10][13] = 1;
	State::worldGrid[15][12] = 1;
	State::worldGrid[15][13] = 1;

	State::worldGrid[1][6] = 0;
	State::worldGrid[2][6] = 0;
	State::worldGrid[23][6] = 0;
	State::worldGrid[24][6] = 0;
	State::worldGrid[8][7] = 0;
	State::worldGrid[9][7] = 0;
	State::worldGrid[10][7] = 0;
	State::worldGrid[15][7] = 0;
	State::worldGrid[16][7] = 0;
	State::worldGrid[17][7] = 0;
	State::worldGrid[1][19] = 0;
	State::worldGrid[2][19] = 0;
	State::worldGrid[23][19] = 0;
	State::worldGrid[24][19] = 0;
	State::worldGrid[8][18] = 0;
	State::worldGrid[9][18] = 0;
	State::worldGrid[10][18] = 0;
	State::worldGrid[15][18] = 0;
	State::worldGrid[16][18] = 0;
	State::worldGrid[17][18] = 0;
	State::worldGrid[12][10] = 0;
	State::worldGrid[13][10] = 0;
	State::worldGrid[12][15] = 0;
	State::worldGrid[13][15] = 0;

}
void updateWorldGridArena1() {
	State::worldGrid[6][1] = 0;
	State::worldGrid[6][2] = 0;
	State::worldGrid[6][23] = 0;
	State::worldGrid[6][24] = 0;
	State::worldGrid[7][8] = 0;
	State::worldGrid[7][9] = 0;
	State::worldGrid[7][10] = 0;
	State::worldGrid[7][15] = 0;
	State::worldGrid[7][16] = 0;
	State::worldGrid[7][17] = 0;
	State::worldGrid[19][1] = 0;
	State::worldGrid[19][2] = 0;
	State::worldGrid[19][23] = 0;
	State::worldGrid[19][24] = 0;
	State::worldGrid[18][8] = 0;
	State::worldGrid[18][9] = 0;
	State::worldGrid[18][10] = 0;
	State::worldGrid[18][15] = 0;
	State::worldGrid[18][16] = 0;
	State::worldGrid[18][17] = 0;
	State::worldGrid[10][12] = 0;
	State::worldGrid[10][13] = 0;
	State::worldGrid[15][12] = 0;
	State::worldGrid[15][13] = 0;

	State::worldGrid[1][6] = 1;
	State::worldGrid[2][6] = 1;
	State::worldGrid[23][6] = 1;
	State::worldGrid[24][6] = 1;
	State::worldGrid[8][7] = 1;
	State::worldGrid[9][7] = 1;
	State::worldGrid[10][7] = 1;
	State::worldGrid[15][7] = 1;
	State::worldGrid[16][7] = 1;
	State::worldGrid[17][7] = 1;
	State::worldGrid[1][19] = 1;
	State::worldGrid[2][19] = 1;
	State::worldGrid[23][19] = 1;
	State::worldGrid[24][19] = 1;
	State::worldGrid[8][18] = 1;
	State::worldGrid[9][18] = 1;
	State::worldGrid[10][18] = 1;
	State::worldGrid[15][18] = 1;
	State::worldGrid[16][18] = 1;
	State::worldGrid[17][18] = 1;
	State::worldGrid[12][10] = 1;
	State::worldGrid[13][10] = 1;
	State::worldGrid[12][15] = 1;
	State::worldGrid[13][15] = 1;
}

int main(int, char**) {
	initializeGridCenterCoords();
	updateWorldGridArena2();

	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	Physics physics = Physics::Instance();
	physics.Initialize();

	// Cameras for each of the players
	Camera camera(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	Camera camera1(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	Camera camera2(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	Camera camera3(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);

	std::array<Camera*, 4> cameras = { &camera, &camera1, &camera2, &camera3 };

	Input input = Input();
	OpponentInput opponentBrains[3];
	opponentBrains[0].setPlayerNum(1);
	opponentBrains[1].setPlayerNum(2);
	opponentBrains[2].setPlayerNum(3);

	ShaderProgram shaderProgram("shaders/main.vert", "shaders/main.frag");
	shaderProgram.compile();

	ShaderProgram simpleDepthShader("shaders/simple.vert", "shaders/simple.frag");
	simpleDepthShader.compile();

	// The arena model
	Model arenaPlane("models/arena_plane.obj", "textures/arena_plane_texture.jpg", GL_DYNAMIC_DRAW);
	Model centerArea("models/center_area.obj", "textures/center_area_texture.jpg", GL_DYNAMIC_DRAW, true);
	Model dome("models/dome.obj", "textures/dome.jpg", GL_DYNAMIC_DRAW, true);
	Model innerWalls("models/inner_walls.obj", "textures/pillars_texture.jpg", GL_DYNAMIC_DRAW, true);
	Model walls("models/walls.obj", "textures/pillars_texture.jpg", GL_DYNAMIC_DRAW, true);
	Model redGates("models/red_gates.obj", "textures/red_gates.jpg", GL_DYNAMIC_DRAW);
	Model blueGates("models/blue_gates.obj", "textures/blue_gates.jpg", GL_DYNAMIC_DRAW);
	Model skybox("models/skybox.obj", "textures/stars.jpg", GL_DYNAMIC_DRAW, true);

	// Shadow setup start ---------------------------------------------------------------------

	// Configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024 * 12, SHADOW_HEIGHT = 1024 * 12;

	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set sampler aliases
	shaderProgram.use();

	auto shaderID = static_cast<unsigned int>(shaderProgram);
	auto samplerLoc = glGetUniformLocation(shaderID, "textureSampler");
	glUniform1i(samplerLoc, 0);
	samplerLoc = glGetUniformLocation(shaderID, "shadowMap");
	glUniform1i(samplerLoc, 1);

	//main loop flag
	bool quit = false;

	// Entities
	Vehicle car(0, "models/car_body.obj", "textures/car_body_texture.jpg", "textures/green_tire_texture.jpg");

	car.attachPhysics(physics);
	State::vehicles[0] = car.getVehicle();

	Vehicle opponentCar1(1, "models/blueCar.obj", "textures/blue_car_texture.jpg", "textures/blue_tire_texture.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicles[1] = opponentCar1.getVehicle();
	opponentBrains[0].attachVehicle(opponentCar1.getVehicle());

	Vehicle opponentCar2(2, "models/redCar.obj", "textures/red_car_texture.jpg", "textures/red_tire_texture.jpg");
	opponentCar2.attachPhysics(physics);
	opponentBrains[1].attachVehicle(opponentCar2.getVehicle());
	State::vehicles[2] = opponentCar2.getVehicle();

	Vehicle opponentCar3(3, "models/yellowCar.obj", "textures/yellow_car_texture.jpg", "textures/yellow_tire_texture.jpg");
	opponentCar3.attachPhysics(physics);
	State::vehicles[3] = opponentCar3.getVehicle();
	opponentBrains[2].attachVehicle(opponentCar3.getVehicle());

	std::array<Vehicle*, 4> cars = { &car, &opponentCar1, &opponentCar2, &opponentCar3 };

	Flag flag;
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(1);
	flagDropoffZone1.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone2(2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(3);
	flagDropoffZone3.attachPhysics(physics);

	DoorManager doorManager;

	std::vector<Entity*> entities;
	entities.push_back(&car);
	entities.push_back(&flag);
	entities.push_back(&flagDropoffZone0);
	entities.push_back(&flagDropoffZone1);
	entities.push_back(&flagDropoffZone2);
	entities.push_back(&flagDropoffZone3);
	entities.push_back(&opponentCar1);
	entities.push_back(&opponentCar2);
	entities.push_back(&opponentCar3);

	PowerUpManager powerUpManager(physics);

	// setup audio
	Audio::soundSystem.initialize();
	Audio::soundSystem.initializeBuffers();
	Audio::music = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	Audio::music.loop();
	Audio::music.playSound();
	Audio::engine = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	Audio::engine.loop();
	Audio::engine.setVolume(0.1f);
	Audio::engine.playSound();
	Audio::collision = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_COLLISION);
	Audio::projectile = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE);
	Audio::flag_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_PICKUP);
	Audio::projectile_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_PROJECTILE_PICKUP);
	Audio::spike_trap_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPIKE_TRAP_PICKUP);
	Audio::speed_boost_pickup = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST_PICKUP);
	Audio::flag_return = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_RETURN);
	Audio::speed_boost = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_SPEED_BOOST);
	Audio::projectile_explosion = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_EXPLOSION);
	Audio::car_crash = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_CRASH);
	Audio::flag_lost = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_FLAG_LOST);
	Audio::gate_switch = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_GATE_SWITCH);
	Audio::caught = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_CAUGHT);

	Audio::engine2 = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE2);
	Audio::engine2.loop();
	Audio::engine2.setVolume(0.05f);
	Audio::engine3 = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE3);
	Audio::engine3.loop();
	Audio::engine3.setVolume(0.05f);
	Audio::engine4 = Audio::soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE4);
	Audio::engine4.loop();
	Audio::engine4.setVolume(0.05f);

	InvisibleBarrier barriers(0);
	barriers.attachPhysics(physics);
	entities.push_back(&barriers);

	for (int opponentNum = 1; opponentNum < 4; opponentNum++) {
		opponentBrains[opponentNum - 1].updatePath(State::vehicles[opponentNum]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
	}
	int aiStuffCounter = 0;


	GameUI gameUI;
	bool gameStarted = false;
	bool playersSelected = false;
	bool gameFinished = false;

	InputInfo* info;

	auto processVehicleInput = [&input, &info](Vehicle& v)
	{
		if (v.useKeyboard)
			v.processInput(*info);
		else if (v.useController)
		{
			const auto controllerInfo = input.getInfo(v.controllerNumber);
			v.processInput(*controllerInfo);
		}
	};

	auto processPowerupInput = [&](Vehicle& v, int playerNum)
	{
		if (v.useKeyboard)
			powerUpManager.use(physics, *info, playerNum);
		else if (v.useController)
		{
			const auto controllerInfo = input.getInfo(v.controllerNumber);
			powerUpManager.use(physics, *controllerInfo, playerNum);
		}
	};

	auto preLoop = [&]()
	{
		auto controllerInfo= input.getAllControllerInfo();
		bool controllerPressed = false;
		for (auto& [id, c_info] : *controllerInfo)
		{
			if (c_info.inputReleased(MovementFlags::ENTER))
				controllerPressed = true;
		}
		gameUI.renderMenu();
		if (!info->inputState[MovementFlags::ENTER] || controllerPressed)
			gameStarted = true;
	};

	auto render = [&](int x, int y, int width, int height, int playerNum, bool isReversing = false)
	{

		// Engine for non-first players
		if (!Audio::engine2.isSoundPlaying() && State::numHumanPlayers > 1) {
			Audio::engine2.playSound();
		}

		// Engine for non-first players
		if (!Audio::engine3.isSoundPlaying() && State::numHumanPlayers > 2) {
			Audio::engine3.playSound();
		}

		// Engine for non-first players
		if (!Audio::engine4.isSoundPlaying() && State::numHumanPlayers > 3) {
			Audio::engine4.playSound();
		}

		// Update camera
		float velocity = cars[playerNum]->getVelocity();
		cameras[playerNum]->updateCamera(cars[playerNum]->mGeometry->getModelMatrix(), velocity, cars[playerNum]->isReversing(), isReversing);

		//Update sound - lower start, growth and cap of engine volume with multiplayer
		if (State::numHumanPlayers > 1) {
			if(playerNum == 0) Audio::engine.setVolume(0.1f + 0.0001f * abs(velocity));
			if(playerNum == 1) Audio::engine2.setVolume(0.1f + 0.0001f * abs(velocity));
			if(playerNum == 2) Audio::engine3.setVolume(0.1f + 0.0001f * abs(velocity));
			if(playerNum == 3) Audio::engine4.setVolume(0.1f + 0.0001f * abs(velocity));
		}
		else Audio::engine.setVolume(0.1f + 0.0001f * abs(velocity));

		shaderProgram.use();

		// first render to depth map ---------------
		simpleDepthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		doorManager.simulate();

		// First pass
		centerArea.draw(simpleDepthShader, camera, true, 1);
		arenaPlane.draw(simpleDepthShader, camera, true, 1);
		innerWalls.draw(simpleDepthShader, camera, true, 1);
		dome.draw(simpleDepthShader, camera, true, 1);
		walls.draw(simpleDepthShader, camera, true, 1);
		// don't include skybox in depth map

		if (State::redArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateDownOffset);
				blueGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateUpOffset);
			}
			else redGates.draw(simpleDepthShader, camera, true, 2);
		}
		if (State::blueArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateUpOffset);
				blueGates.draw(simpleDepthShader, camera, true, 2, doorManager.gateDownOffset);
			}
			else blueGates.draw(simpleDepthShader, camera, true, 2);
		}

		for (const auto& entity : entities)
			entity->draw(physics, simpleDepthShader, camera, true);

		powerUpManager.draw(physics, simpleDepthShader, camera, true);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(x, y, width, height);

		// Now standard rendering -----------------
		shaderProgram.use();

		float near_plane = 100.f, far_plane = 800.f;

		auto nearLoc = glGetUniformLocation(shaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		auto farLoc = glGetUniformLocation(shaderID, "far_plane");
		glUniform1f(farLoc, far_plane);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);

		// Second pass
		centerArea.draw(shaderProgram, *cameras[playerNum], false, 1);
		arenaPlane.draw(shaderProgram, *cameras[playerNum], false, 1);
		innerWalls.draw(shaderProgram, *cameras[playerNum], false, 1);
		dome.draw(shaderProgram, *cameras[playerNum], false, 1);
		walls.draw(shaderProgram, *cameras[playerNum], false, 1);
		skybox.draw(shaderProgram, *cameras[playerNum], false, 0);
		
		if (State::redArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateDownOffset);
			    blueGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateUpOffset);

			} else redGates.draw(shaderProgram, *cameras[playerNum], false, 2);
		}
		if (State::blueArena) {
			if (doorManager.gateDownOffset < 0) {
				redGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateUpOffset);
				blueGates.draw(shaderProgram, *cameras[playerNum], false, 2, doorManager.gateDownOffset);
			}
			else blueGates.draw(shaderProgram, *cameras[playerNum], false, 2);
		}
		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, *cameras[playerNum], false);

		powerUpManager.draw(physics, shaderProgram, *cameras[playerNum], false);

		if (!State::flagPickedUpBy[playerNum])
			gameUI.setCompassDirection(cars[playerNum]->mGeometry->getModelMatrix(), flag.mGeometry->getModelMatrix());
		else if (State::flagPickedUpBy[playerNum])
			gameUI.setCompassDirection(cars[playerNum]->mGeometry->getModelMatrix(), State::flagDropoffBoxes[playerNum]->getGlobalPose().p);

		gameUI.render(playerNum);
	};

	auto mainLoop = [&]()
	{
		processVehicleInput(car);
		if (State::numHumanPlayers >= 2) processVehicleInput(opponentCar1);
		if (State::numHumanPlayers >= 3) processVehicleInput(opponentCar2);
		if (State::numHumanPlayers >= 4) processVehicleInput(opponentCar3);

		powerUpManager.pickup(physics);

		processPowerupInput(car, 0);
		if (State::numHumanPlayers >= 2) processPowerupInput(opponentCar1, 1);
		if (State::numHumanPlayers >= 3) processPowerupInput(opponentCar2, 2);
		if (State::numHumanPlayers >= 4) processPowerupInput(opponentCar3, 3);

		//arena door switch
		if (doorManager.arenaSwitch) {
			if (State::blueArena) {
				updateWorldGridArena2();
				physics.generateRedDoor(); //switch from blue doors to red
				State::redArena = true;
				State::blueArena = false;
				//draw red arena
				fmt::print("Doors switching\n");
				fmt::print("Red arena loaded\n");
			}
			else if (State::redArena) {
				updateWorldGridArena1();
				physics.generateBlueDoor(); //switch from red doors to blue
				State::blueArena = true;
				State::redArena = false;
				//draw blue arena
				fmt::print("Doors switching\n");
				fmt::print("Blue arena loaded\n");
			}
			doorManager.arenaSwitch = false;
			doorManager.arenaTimer = 0;
		}

		// TODO: Only compute the AI paths that are actually needed
		if (aiStuffCounter % 3 == 0 && State::numHumanPlayers < 2) { //stagger pathfinding on different frames
			if (State::flagPickedUpBy[1]) {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[1]->getGlobalPose().p);
			}
			else {
				opponentBrains[0].updatePath(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (aiStuffCounter % 3 == 1 && State::numHumanPlayers < 3) {
			if (State::flagPickedUpBy[2]) {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[2]->getGlobalPose().p);
			}
			else {
				opponentBrains[1].updatePath(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (State::numHumanPlayers < 4) {
			if (State::flagPickedUpBy[3]) {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagDropoffBoxes[3]->getGlobalPose().p);
			}
			else {
				opponentBrains[2].updatePath(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}


		if (State::numHumanPlayers < 2) {
			std::map<MovementFlags, bool>  command = opponentBrains[0].getInput(State::vehicles[1]->getRigidDynamicActor()->getGlobalPose().p, opponentCar1.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar1.processInput(command);
			powerUpManager.use(physics, command, 1);
		}
		if (State::numHumanPlayers < 3) {
			const std::map<MovementFlags, bool>  command = opponentBrains[1].getInput(State::vehicles[2]->getRigidDynamicActor()->getGlobalPose().p, opponentCar2.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar2.processInput(command);
			powerUpManager.use(physics, command, 2);
		}
		if (State::numHumanPlayers < 4) {
			const std::map<MovementFlags, bool>  command = opponentBrains[2].getInput(State::vehicles[3]->getRigidDynamicActor()->getGlobalPose().p, opponentCar3.mGeometry->getModelMatrix().column2.getXYZ());
			opponentCar3.processInput(command);
			powerUpManager.use(physics, command, 3);
		}

		aiStuffCounter++;

		for (const auto& entity : entities)
			entity->simulate(physics);

		powerUpManager.simulate(physics);

		if (State::slowCar0) {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 800;
			eng1.mPeakTorque = 2000;
			car.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		else {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 2000;
			eng1.mPeakTorque = 2000;
			car.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		if (State::slowCar1) {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 800;
			eng1.mPeakTorque = 2000;
			opponentCar1.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		else {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 2000;
			eng1.mPeakTorque = 2000;
			opponentCar1.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		if (State::slowCar2) {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 800;
			eng1.mPeakTorque = 2000;
			opponentCar2.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		else {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 2000;
			eng1.mPeakTorque = 2000;
			opponentCar2.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		if (State::slowCar3) {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 800;
			eng1.mPeakTorque = 2000;
			opponentCar3.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		else {
			PxVehicleEngineData eng1;
			eng1.mMaxOmega = 2000;
			eng1.mPeakTorque = 2000;
			opponentCar3.getVehicle()->mDriveSimData.setEngineData(eng1);
		}
		

		if (State::killCars[0]) {
			State::flagPickedUpBy[0] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			car.getVehicle()->getRigidDynamicActor()->release();
			car.attachPhysics(physics);
			std::cout << "Respawning player" << std::endl;
			State::killCars[0] = false;
			State::slowCar0 = false;
		}
		if (State::killCars[1]) {
			State::flagPickedUpBy[1] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar1.getVehicle()->getRigidDynamicActor()->release();
			opponentCar1.attachPhysics(physics);
			std::cout << "Respawning opponent 1" << std::endl;
			State::killCars[1] = false;
			State::slowCar1 = false;
		}
		if (State::killCars[2]) {
			State::flagPickedUpBy[2] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar2.getVehicle()->getRigidDynamicActor()->release();
			opponentCar2.attachPhysics(physics);
			std::cout << "Respawning opponent 2" << std::endl;
			State::killCars[2] = false;
			State::slowCar2 = false;
		}
		if (State::killCars[3]) {
			State::flagPickedUpBy[3] = false;
			State::flagPickedUp = false;
			State::canPickupFlag = false;
			State::startPickupFlagTimer = true;
			opponentCar3.getVehicle()->getRigidDynamicActor()->release();
			opponentCar3.attachPhysics(physics);
			std::cout << "Respawning opponent 3" << std::endl;
			State::killCars[3] = false;
			State::slowCar3 = false;
		}
		physics.stepPhysics();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (State::numHumanPlayers == 1)
			render(0, 0, width, height, 0);

		// If there are more then one human players, draw the four player layout so that there are not
		// weird aspect ratios
		if (State::numHumanPlayers > 1) {
			render(0, height / 2, width / 2, height / 2, 0);
			render(width / 2, height / 2, width / 2, height / 2, 1);
			render(0, 0, width / 2, height / 2, 2);
			render(width / 2, 0, width / 2, height / 2, 3);
		}

		// Check to see if any of the players have won
		if (std::any_of(std::begin(State::scores), std::end(State::scores), [](int score){
				return score >= State::winScore;
			}))
		{
			gameFinished = true;
			gameStarted = false;
		}

		/*ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);
		ImGui::SliderFloat("x Scale", &g_scale.x, 0.01f, 3.0f);
		ImGui::SliderFloat("y Scale", &g_scale.y, 0.01f, 3.0f);
		ImGui::SliderFloat("Position x", &g_pos.x, -3.0f, 3.0f);
		ImGui::SliderFloat("Position y", &g_pos.y, -3.0f, 3.0f);
		ImGui::SliderFloat("Projectile speed scaling factor", &scalingFactor, 1.0f, 5.0f);

		ImGui::Text("Car Stuff (Press R after changing values)");
		ImGui::Text("VehicleCreate.cpp (lines 38-41)");
		ImGui::SliderFloat("Engine Peak Torque", &peakTorque, 100.f, 2000.f);
		ImGui::SliderFloat("Engine Max Omega (Speed)", &maxOmega, 500.f, 2000.f);
		ImGui::SliderFloat("Gear Switch Time", &gearSwitchTime, 0.f, 50.f);
		ImGui::SliderFloat("Clutch Strength", &clutchStrength, 0.f, 100.f);
		ImGui::Text("Physics.cpp (lines 14-24)");
		ImGui::SliderFloat("Chassis Mass", &chassMass, 1000.f, 2000.f);
		ImGui::SliderFloat("Chassis Dimension X", &chassDimX, 1.f, 5.f);
		ImGui::SliderFloat("Chassis Dimension Y", &chassDimY, 1.f, 4.f);
		ImGui::SliderFloat("Chassis Dimension Z", &chassDimZ, 1.f, 10.f);
		ImGui::SliderFloat("Wheel Mass", &whMass, 20.f, 60.f);
		ImGui::SliderFloat("Wheel Radius", &whRadius, 0.1f, 2.f);
		ImGui::SliderFloat("Wheel Width", &whWidth, 0.1f, 2.f);
		ImGui::SliderFloat("Spring Max Compression", &springMaxCompression, 0.f, 1.f);
		ImGui::SliderFloat("Spring Max Droop", &springMaxDroop, 0.f, 1.f);
		ImGui::SliderFloat("Spring Strength", &springStrength, 0.f, 50000.f);
		ImGui::SliderFloat("Spring Damper Rate", &springDamperRate, 0.f, 10000.f);
		ImGui::Text("Vehicle.cpp (lines 8-25");
		ImGui::SliderFloat("rise rate accel", &riseRateAccel, 0.1f, 500.f);
		ImGui::SliderFloat("rise rate brake", &riseRateBrake, 0.1f, 500.f);
		ImGui::SliderFloat("rise rate handbrake", &riseRateHandBrake, 0.1f, 50.f);
		ImGui::SliderFloat("rise rate steer left", &riseRateSteerLeft, 0.1f, 50.f);
		ImGui::SliderFloat("rise rate steer right", &riseRateSteerRight, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate accel", &fallRateAccel, 0.1f, 500.f);
		ImGui::SliderFloat("fall rate brake", &fallRateBrake, 0.1f, 500.f);
		ImGui::SliderFloat("fall rate handbrake", &fallRateHandBrake, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate steer left", &fallRateSteerLeft, 0.1f, 50.f);
		ImGui::SliderFloat("fall rate steer right", &fallRateSteerRight, 0.1f, 50.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData1A", &gSteerVsForwardSpeedData1A, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData1B", &gSteerVsForwardSpeedData1B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData2A", &gSteerVsForwardSpeedData2A, 0.f, 20.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData2B", &gSteerVsForwardSpeedData2B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData3A", &gSteerVsForwardSpeedData3A, 0.f, 100.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData3B", &gSteerVsForwardSpeedData3B, 0.f, 10.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData4A", &gSteerVsForwardSpeedData4A, 0.f, 500.f);
		ImGui::SliderFloat("gSteerVsForwardSpeedData4B", &gSteerVsForwardSpeedData4B, 0.f, 10.f); */
	};
	bool keyboardUsed = false;
	std::vector<int> controllerNumbersUsed;

	// Loop until the user closes the window
	while (!quit)
	{
		quit = input.HandleInput();
		info = input.getInfo(); // Get the keyboard info

		// Render
		//window.startImGuiFrame();
		Window::clear();

		//ImGui::Begin("Framerate Counter!");
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (!gameStarted && !gameFinished)
			preLoop();
		else if (gameStarted && !playersSelected)
		{
			auto controllerInfo = input.getAllControllerInfo();

			// First handle the keyboard inputs
			if (info->inputReleased(MovementFlags::ACTION))
			{
				for (auto* const player : cars)
				{
					if (!player->isHuman && !keyboardUsed)
					{
						player->setHuman(true);
						keyboardUsed = true;
						break;
					}
					if (player->isHuman && player->useKeyboard)
					{
						player->ready = true;
						fmt::print("Keyboard player is ready!\n");
					}
				}
			}

			// Then handle the controller inputs
			for (auto& [id, c_info] : *controllerInfo)
			{
				if (c_info.inputReleased(MovementFlags::ACTION))
				{
					for (auto* const player : cars)
					{
						if (!player->isHuman && std::find(controllerNumbersUsed.begin(), controllerNumbersUsed.end(), id) == controllerNumbersUsed.end())
						{
							player->setHuman(false, id);
							controllerNumbersUsed.push_back(id);
							break;
						}
						if (player->isHuman && player->useController && player->controllerNumber == id)
						{
							player->ready = true;
							fmt::print("Controller player is ready!\n");
						}
					}
				}
			}

			// If at least one player is a human, and all the human players are ready then all players are ready
			int humanCount = 0;
			int readyCount = 0;
			for (auto* const player : cars)
			{
				if (player->isHuman)
				{
					humanCount += 1;
					if (player->ready)
						readyCount += 1;
				}
			}

			if (humanCount >= 1 && readyCount == humanCount)
			{
				playersSelected = true;
				State::numHumanPlayers = humanCount;
			}

			// Render all four players
			render(0, height / 2, width / 2, height / 2, 0, true);
			gameUI.renderPlayerSelect(car.isHuman, car.ready);

			render(width / 2, height / 2, width / 2, height / 2, 1, true);
			gameUI.renderPlayerSelect(opponentCar1.isHuman, opponentCar1.ready);

			render(0, 0, width / 2, height / 2, 2, true);
			gameUI.renderPlayerSelect(opponentCar2.isHuman, opponentCar2.ready);

			render(width / 2, 0, width / 2, height / 2, 3, true);
			gameUI.renderPlayerSelect(opponentCar3.isHuman, opponentCar3.ready);

		}
		else if (gameStarted && !gameFinished)
			mainLoop();
		else if (gameFinished)
		{
			if (State::numHumanPlayers == 1)
			{
				render(0, 0, width, height, 0, true);
				gameUI.renderEndScreen(0);
			}

			if (State::numHumanPlayers > 1)
			{
				render(0, height / 2, width / 2, height / 2, 0, true);
				gameUI.renderEndScreen(0);

				render(width / 2, height / 2, width / 2, height / 2, 1, true);
				gameUI.renderEndScreen(1);

				render(0, 0, width / 2, height / 2, 2, true);
				gameUI.renderEndScreen(2);

				render(width / 2, 0, width / 2, height / 2, 3, true);
				gameUI.renderEndScreen(3);
			}

			// If any of the players press enter, reset the game
			if (!info->inputState[MovementFlags::ENTER])
			{
				for (auto* c : cameras)
				{
					c->lagCounter = 0;
				}
				State::scores.fill(0);
				State::flagPickedUp = false;
				State::killCars.fill(true);
				State::flagPickedUpBy.fill(false);

				gameFinished = false;
				gameStarted = false;
				playersSelected = true; // No need to reset who is playing if they want to play again

				if (State::blueArena) {
					updateWorldGridArena2();
					physics.generateRedDoor(); //switch from blue doors to red
					State::redArena = true;
					State::blueArena = false;
					//draw red arena
					fmt::print("Red arena loaded\n");
				}
			}
		}


		//ImGui::End();

		//Window::renderImGuiFrame();
		window.swap();
	}

	//cleanup
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	powerUpManager.cleanUp();
	physics.CleanupPhysics();
	Audio::soundSystem.killSources();

	return 0;
}
