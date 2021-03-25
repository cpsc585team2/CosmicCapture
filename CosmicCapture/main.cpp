#include <memory>
#include <fmt/format.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

#include "graphics/Window.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Model.h"
#include "imgui/imgui.h"

#include "input.h"

#include "./audio/AudioEngine.h"

#include "Camera.h"
#include "Vehicle.h"
#include "Flag.h"
#include "FlagDropoffZone.h"
#include "Projectile.h"
#include "ProjectilePickupZone.h"
#include "SpeedBoost.h"
#include "SpeedBoostPickupZone.h"
#include "SpikeTrap.h"
#include "SpikeTrapPickupZone.h"
#include "DoorSwitchZone.h"

#include "OpponentInput.h"
#include "GridMarker.h"
#include "InvisibleBarrier.h"

#include "GlobalState.h"


#define M_PI  3.14159265358979323846

float angle = -0.25f;

void initializeGridCenterCoords() {
	float flatOffset = 4.f; //TUNING POINT
	float diagonalOffset = 1.f; //TUNING POINT
	bool shifted = false;
	for (int i = 0; i < 36; i++) {
		for (int j = 0; j < 36; j++) {
			State::worldGridCenterCoords[i][j].first = i * 10.f - 180.f + 5.f;
			State::worldGridCenterCoords[i][j].second = j * 10.f - 180.f + 5.f;
			shifted = false;
			if ((i + 1 < 36) && (i - 1 >= 0) && (j + 1 < 36) && (j - 1 >= 0)) {
				if (State::worldGrid[i + 1][j] == 0) {
					State::worldGridCenterCoords[i][j].first -= flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i - 1][j] == 0) {
					State::worldGridCenterCoords[i][j].first += flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].second -= flatOffset;
					shifted = true;
				}
				if (State::worldGrid[i][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].second += flatOffset;
					shifted = true;
				}

				if (shifted) continue;

				if (State::worldGrid[i - 1][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].first += diagonalOffset;
					State::worldGridCenterCoords[i][j].second += diagonalOffset;
				}
				if (State::worldGrid[i + 1][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].first -= diagonalOffset;
					State::worldGridCenterCoords[i][j].second -= diagonalOffset;
				}
				if (State::worldGrid[i + 1][j - 1] == 0) {
					State::worldGridCenterCoords[i][j].first -= diagonalOffset;
					State::worldGridCenterCoords[i][j].second += diagonalOffset;
				}
				if (State::worldGrid[i - 1][j + 1] == 0) {
					State::worldGridCenterCoords[i][j].first += diagonalOffset;
					State::worldGridCenterCoords[i][j].second -= diagonalOffset;
				}
				
			}

		}
	}
}
	
void updateWorldGridArena1() {
	State::worldGrid[12][17] = 0;
	State::worldGrid[12][18] = 0;
	State::worldGrid[28][17] = 0;
	State::worldGrid[28][18] = 0;
	State::worldGrid[7][1] = 0;
	State::worldGrid[7][2] = 0;
	State::worldGrid[29][1] = 0;
	State::worldGrid[29][2] = 0;
	State::worldGrid[7][33] = 0;
	State::worldGrid[7][34] = 0;
	State::worldGrid[29][33] = 0;
	State::worldGrid[29][34] = 0;

	State::worldGrid[17][12] = 1;
	State::worldGrid[18][12] = 1;
	State::worldGrid[17][28] = 1;
	State::worldGrid[18][28] = 1;
	State::worldGrid[1][7] = 1;
	State::worldGrid[2][7] = 1;
	State::worldGrid[1][29] = 1;
	State::worldGrid[2][29] = 1;
	State::worldGrid[33][7] = 1;
	State::worldGrid[34][7] = 1;
	State::worldGrid[33][29] = 1;
	State::worldGrid[34][29] = 1;

}
void updateWorldGridArena2() {
	State::worldGrid[12][17] = 1;
	State::worldGrid[12][18] = 1;
	State::worldGrid[23][17] = 1;
	State::worldGrid[23][13] = 1;
	State::worldGrid[7][1] = 1;
	State::worldGrid[7][2] = 1;
	State::worldGrid[29][1] = 1;
	State::worldGrid[29][2] = 1;
	State::worldGrid[7][33] = 1;
	State::worldGrid[7][34] = 1;
	State::worldGrid[29][33] = 1;
	State::worldGrid[29][34] = 1;

	State::worldGrid[17][12] = 0;
	State::worldGrid[18][12] = 0;
	State::worldGrid[17][23] = 0;
	State::worldGrid[18][23] = 0;
	State::worldGrid[1][7] = 0;
	State::worldGrid[2][7] = 0;
	State::worldGrid[1][29] = 0;
	State::worldGrid[2][29] = 0;
	State::worldGrid[33][7] = 0;
	State::worldGrid[34][7] = 0;
	State::worldGrid[33][29] = 0;
	State::worldGrid[34][29] = 0;
}

int main(int argc, char** args) {
	initializeGridCenterCoords();
	updateWorldGridArena2();
	// Window Initialization
	const GLint width = 1280, height = 720;
	Window window("Cosmic Capture", width, height);
	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	//physics
	Physics physics = Physics::Instance();
	const auto sCamera = std::make_shared<Camera>(PxVec3(0.0f, 7.0f, -13.0f), PxVec3(-0.6f, -0.2f, -0.7f), aspect);
	physics.Initialize();

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
	Model redArena("models/redArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);

	Model blueArena("models/blueArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
	// Shadow setup start ---------------------------------------------------------------------

// Configure depth map FBO
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024 * 4, SHADOW_HEIGHT = 1024 * 4;

	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

	auto mShaderID = static_cast<unsigned int>(shaderProgram);
	auto samplerLoc = glGetUniformLocation(mShaderID, "textureSampler");
	glUniform1i(samplerLoc, 0);
	samplerLoc = glGetUniformLocation(mShaderID, "shadowMap");
	glUniform1i(samplerLoc, 1);


	//main loop flag
	bool quit = false;

	// Entities
	Vehicle car(shaderProgram, sCamera, 0, "textures/blank.jpg");
	car.attachPhysics(physics);
	State::vehicleRDs[0] = car.getVehicle()->getRigidDynamicActor();

	Vehicle opponentCar1(shaderProgram, sCamera, 1, "textures/blue.jpg");
	opponentCar1.attachPhysics(physics);
	State::vehicleRDs[1] = opponentCar1.getVehicle()->getRigidDynamicActor();
	opponentBrains[0].attachVehicle(opponentCar1.getVehicle());

	Vehicle opponentCar2(shaderProgram, sCamera, 2, "textures/pink.jpg");
	opponentCar2.attachPhysics(physics);
	State::vehicleRDs[2] = opponentCar2.getVehicle()->getRigidDynamicActor();
	opponentBrains[1].attachVehicle(opponentCar2.getVehicle());

	Vehicle opponentCar3(shaderProgram, sCamera, 3, "textures/green.jpg");
	opponentCar3.attachPhysics(physics);
	State::vehicleRDs[3] = opponentCar3.getVehicle()->getRigidDynamicActor();
	opponentBrains[2].attachVehicle(opponentCar3.getVehicle());

	//projectile prototype stuff----------------------
	Projectile testProj(shaderProgram, sCamera);
	ProjectilePickupZone projPickupZone(shaderProgram, sCamera);
	projPickupZone.attachPhysics(physics);
	//------------------------------------------------

	//speedboost powerup
	SpeedBoost testSpeedBoost(shaderProgram, sCamera);
	SpeedBoostPickupZone speedboostPickupZone(shaderProgram, sCamera);
	speedboostPickupZone.attachPhysics(physics);

	// Spike trap powerup
	SpikeTrap testSpikeTrap(shaderProgram, sCamera);
	SpikeTrapPickupZone spikeTrapPickupZone(shaderProgram, sCamera);
	spikeTrapPickupZone.attachPhysics(physics);

	Flag flag(shaderProgram, sCamera);
	flag.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone0(shaderProgram, sCamera, 0);
	flagDropoffZone0.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone1(shaderProgram, sCamera, 1);
	flagDropoffZone1.attachPhysics(physics);

	// setup audio
	AudioEngine soundSystem = AudioEngine();
	soundSystem.initialize();
	soundSystem.initializeBuffers();
	AudioInstance music = soundSystem.createInstance(audioConstants::SOUND_FILE_MAIN_TRACK);
	//music.loop();
	//music.playSound();
	//AudioInstance engine = soundSystem.createInstance(audioConstants::SOUND_FILE_ENGINE);
	//engine.loop();
	//engine.playSound();


	FlagDropoffZone flagDropoffZone2(shaderProgram, sCamera, 2);
	flagDropoffZone2.attachPhysics(physics);

	FlagDropoffZone flagDropoffZone3(shaderProgram, sCamera, 3);
	flagDropoffZone3.attachPhysics(physics);


	DoorSwitchZone doorSwitchZone(shaderProgram, sCamera);
	doorSwitchZone.attachPhysics(physics);

	std::vector<Entity*> entities;
	entities.push_back(&car);
	entities.push_back(&flag);
	entities.push_back(&flagDropoffZone0);
	entities.push_back(&flagDropoffZone1);
	entities.push_back(&flagDropoffZone2);
	entities.push_back(&flagDropoffZone3);
	entities.push_back(&projPickupZone);
	entities.push_back(&speedboostPickupZone);
	entities.push_back(&opponentCar1);
	entities.push_back(&opponentCar2);
	entities.push_back(&opponentCar3);
	entities.push_back(&spikeTrapPickupZone);
	entities.push_back(&doorSwitchZone);


	InvisibleBarrier barriers(shaderProgram, sCamera, 0);
	barriers.attachPhysics(physics);
	entities.push_back(&barriers);

	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	//PxVec3 position1(100.f, 2.0f, 100.0f);
	//GridMarker gm1(shaderProgram, sCamera, position1);
	//gm1.attachPhysics(physics);
	//entities.push_back(&gm1);	
	//GRID VISUALS TO HELP ME MAKE AI----------------------------------------
	for (int opponentNum = 1; opponentNum < 4; opponentNum++) {		
			opponentBrains[opponentNum - 1].updatePath(State::vehicleRDs[opponentNum]->getGlobalPose().p, State::flagBody->getGlobalPose().p);	
	}
	//opponentBrains.updatePath(State::vehicleRDs[1]->getGlobalPose().p, State::flagBody->getGlobalPose().p); //get Initial path--------------------------------
	int aiStuffCounter = 0;
	// Loop until the user closes the window
	while (!quit) {
		quit = input.HandleInput();

		// Physics simulation
		auto inputState = input.getInputState();
		
		// Repeat for all vehicles eventually...
		car.processInput(inputState);

		if (State::spikeTrapPickedUp && testSpikeTrap.hasOwningVehicle())
			testSpikeTrap.processInput(inputState, physics);
		
		if (inputState[MovementFlags::ACTION] == false && State::projectilePickedUp) {
			testProj.attachVehicle(car.getVehicle());
			testProj.attachPhysics(physics);			
			entities.push_back(&testProj);
			
			State::projectilePickedUp = false;
		}

		// Use speed boost
		if (inputState[MovementFlags::ACTION] == false && State::speedboostPickedUp) {
			testSpeedBoost.attachVehicle(car.getVehicle());
			testSpeedBoost.attachPhysics(physics);
			entities.push_back(&testSpeedBoost);
			State::speedboostPickedUp = false;
			State::speedBoostFinished = false;
		}

		// Pickup spike trap
		if (State::spikeTrapPickedUp && !testSpikeTrap.hasOwningVehicle()) {
			testSpikeTrap.attachOwningVehicle(car.getVehicle());
			entities.push_back(&testSpikeTrap);
		}

		// In this case, the trap has already been placed, and now is being picked up again
		if (State::spikeTrapPickedUp && State::spikeTrapActive)
		{
			// Reset
			auto loc = std::find(entities.begin(), entities.end(), &testSpikeTrap);
			entities.erase(loc);
			testSpikeTrap.cleanUpPhysics();

			State::spikeTrapActive = false; // The other spike trap is no longer picked up and should be removed
			testSpikeTrap.attachOwningVehicle(car.getVehicle());
		}

		// Run into spike trap
		if (State::spikeTrapInUse && !testSpikeTrap.hasAffectedVehicle())
		{
			switch (State::spikeTrapActingUpon)
			{
			case 0:
				testSpikeTrap.attachAffectedVehicle(car.getVehicle());
				break;
			case 1:
				testSpikeTrap.attachAffectedVehicle(opponentCar1.getVehicle());
				break;
			case 2:
				testSpikeTrap.attachAffectedVehicle(opponentCar2.getVehicle());
				break;
			case 3:
				testSpikeTrap.attachAffectedVehicle(opponentCar3.getVehicle());
				break;
			}
		}
		//arena door switch
		if (State::arenaSwitch && State::arenaSwitchReady) {
			//need undraw code here
			if (State::blueArena) {
				updateWorldGridArena1();
				physics.generateRedDoor(); //switch from blue doors to red
				State::redArena = true;
				State::blueArena = false;
				//draw red arena
				//Model arena("models/redArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
				fmt::print("Button pressed, doors switching\n");
				fmt::print("Red arena loaded\n");
			}
			else if (State::redArena) {
				updateWorldGridArena2();
				physics.generateBlueDoor(); //switch from red doors to blue
				State::blueArena = true;
				State::redArena = false;
				//draw blue arena
				//Model arena("models/blueArena.obj", "textures/blank.jpg", shaderProgram, sCamera, GL_DYNAMIC_DRAW);
				fmt::print("Button pressed, doors switching\n");
				fmt::print("Blue arena loaded\n");
			}
			State::arenaSwitch = false;
			State::arenaSwitchReady = false;
			State::arenaTimer = 0;
		}
		if (State::arenaSwitch && !State::arenaSwitchReady) {
			State::arenaSwitch = false;
		}


		//forgive me--------------------
		if (aiStuffCounter % 3 == 0) { //stagger pathfinding on different frames
			if (State::flagPickedUpBy[1]) {
				opponentBrains[0].updatePath(State::vehicleRDs[1]->getGlobalPose().p, State::flagDropoffBoxes[1]->getGlobalPose().p);
			}
			else {
				opponentBrains[0].updatePath(State::vehicleRDs[1]->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else if (aiStuffCounter % 3 == 1) {
			if (State::flagPickedUpBy[2]) {
				opponentBrains[1].updatePath(State::vehicleRDs[2]->getGlobalPose().p, State::flagDropoffBoxes[2]->getGlobalPose().p);
			}
			else {
				opponentBrains[1].updatePath(State::vehicleRDs[2]->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		else {
			if (State::flagPickedUpBy[3]) {
				opponentBrains[2].updatePath(State::vehicleRDs[3]->getGlobalPose().p, State::flagDropoffBoxes[3]->getGlobalPose().p);
			}
			else {
				opponentBrains[2].updatePath(State::vehicleRDs[3]->getGlobalPose().p, State::flagBody->getGlobalPose().p);
			}
		}
		opponentCar1.processInput(opponentBrains[0].getInput(State::vehicleRDs[1]->getGlobalPose().p, opponentCar1.mGeometry->getModelMatrix().column2.getXYZ()));
		opponentCar2.processInput(opponentBrains[1].getInput(State::vehicleRDs[2]->getGlobalPose().p, opponentCar2.mGeometry->getModelMatrix().column2.getXYZ()));
		opponentCar3.processInput(opponentBrains[2].getInput(State::vehicleRDs[3]->getGlobalPose().p, opponentCar3.mGeometry->getModelMatrix().column2.getXYZ()));

		aiStuffCounter++;
		//------------------------------*/

		for (const auto& entity : entities)
			entity->simulate(physics);
		
		physics.stepPhysics();

		// Render
		window.startImGuiFrame();
		Window::clear();

		// Update camera
		sCamera->updateCamera(car.mGeometry->getModelMatrix());

		shaderProgram.use();

		auto counter = 1;

		// first render to depth map ---------------
		simpleDepthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// First pass
		if(State::redArena) redArena.drawArena(simpleDepthShader, true, depthMapFBO);
		if(State::blueArena) blueArena.drawArena(simpleDepthShader, true, depthMapFBO);

		for (const auto& entity : entities)
			entity->draw(physics, simpleDepthShader, true, depthMapFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Now standard rendering -----------------

		shaderProgram.use();

		float near_plane = 200.f, far_plane = 600.f;

		auto nearLoc = glGetUniformLocation(mShaderID, "near_plane");
		glUniform1f(nearLoc, near_plane);

		auto farLoc = glGetUniformLocation(mShaderID, "far_plane");
		glUniform1f(farLoc, far_plane);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glActiveTexture(GL_TEXTURE0);

		// Second pass
		if (State::redArena) {
			redArena.drawArena(shaderProgram, false, depthMap);
		}
		if (State::blueArena) {
			blueArena.drawArena(shaderProgram, false, depthMap);
		}
		for (const auto& entity : entities)
			entity->draw(physics, shaderProgram, false, depthMap);

		//scott's debugging prints----------------------------------------------------------------------------------------------
		//PxVec3 playerPosition = car.getVehicle()->getRigidDynamicActor()->getGlobalPose().p;
		//PxVec3 playerDir = car.mGeometry->getModelMatrix().column2.getXYZ();
		//PxVec3 playerToTarget = opponentBrains[0].getPlayerToTargetDir(playerDir, 0, State::flagBody->getGlobalPose().p);
		//int xIndex = (int)((playerPosition.x + 180.f) / 10.f);
		//int zIndex = (int)((playerPosition.z + 180.f) / 10.f);;
		//int dir = opponentBrains[1].getOrientation(playerDir);
		//printf("%f, %f, %f (%f) -- %f, %f, %f (%f)\n", playerDir.x, playerDir.y, playerDir.z, atan2(playerDir.z, playerDir.x), playerToTarget.x, playerToTarget.y, playerToTarget.z, atan2(playerToTarget.z, playerToTarget.x));
		//printf("Coordinates: %f, %f, %f -- %d, %d. DirVector: x: %f, z: %f, dir: %d\n", playerPosition.x, playerPosition.y, playerPosition.z, xIndex, zIndex, playerDir.x, playerDir.z, dir);
		//-----------------------------------------------------------------------------------------------------------------------

		ImGui::Begin("Framerate Counter!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Camera Position");
		ImGui::SliderFloat("Camera angle", &angle, -2.0f * M_PI, 2.0f * M_PI);

		// Game State information
		ImGui::Text("Player 1 Score: %d", State::scores[0]);
		ImGui::Text("Opponent 1 Score: %d", State::scores[1]);
		ImGui::Text("Opponent 2 Score: %d", State::scores[2]);
		ImGui::Text("Opponent 3 Score: %d", State::scores[3]);
		if (State::scores[0] == 3) {
			ImGui::Text("You win");
			if (ImGui::Button("Restart?"))
			{
				for (int i = 0; i < 4; ++i)
					State::scores[i] = 0;
			}
		}
		else if (State::scores[1] == 3) {
			ImGui::Text("Opponent 1 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int i = 0; i < 4; ++i)
					State::scores[i] = 0;
			}
		}
		else if (State::scores[2] == 3) {
			ImGui::Text("Opponent 2 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int i = 0; i < 4; ++i)
					State::scores[i] = 0;
			}
		}
		else if (State::scores[3] == 3) {
			ImGui::Text("Opponent 3 wins");
			if (ImGui::Button("Restart?"))
			{
				for (int i = 0; i < 4; ++i)
					State::scores[i] = 0;
			}
		}
		
		ImGui::End();

		Window::renderImGuiFrame();
		window.swap();
	}
	//cleanup
	for (const auto& entity : entities)
		entity->cleanUpPhysics();
	physics.CleanupPhysics();
	soundSystem.killSources();

	return 0;
}
