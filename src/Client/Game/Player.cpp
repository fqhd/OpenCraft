#include "Player.hpp"
#include "Utils.hpp"
#include "Converter.hpp"
#include <iostream>

const unsigned int PRECISION = 50;
const unsigned int REACH_DISTANCE = 5;
const float SPEED = 4.0f;
const float PLAYER_WIDTH = 1.0f;
const float PLAYER_HEIGHT = 2.0f;
const float GRAVITY = 36.0f;

math::vec3 position; // We declare the player position as a global variable because we want to use it in the AABB sorting function

void Player::init(Camera* _camera, ParticleHandler* _handler, World* _world, NetworkManager* _nManager) {
	m_camera = _camera;
	m_world = _world;
	m_particleHandler = _handler;
	m_networkManager = _nManager;

	hotbar.init();
	position = math::vec3(36, 32, 32);
	gamemode = SURVIVAL;
	hotbar.items[0].id = ItemID::GRASS;
	hotbar.items[0].count = 22;
	hotbar.items[4].id = ItemID::WOOD;
	hotbar.items[4].count = 32;
	hotbar.items[1].id = ItemID::CACTUS;
	hotbar.items[1].count = 32;
	hotbar.items[2].id = ItemID::DIAMOND;
	hotbar.items[2].count = 32;
	hotbar.items[3].id = ItemID::STONE;
	hotbar.items[3].count = 32;
	hotbar.items[5].id = ItemID::SAND;
	hotbar.items[5].count = 32;
	hotbar.items[6].id = ItemID::LEAVES;
	hotbar.items[6].count = 10;

}

void Player::update(float deltaTime) {
	hotbar.update();

	movement(deltaTime);
	if (gamemode != GameMode::SPECTATOR) collideWithWorld();
	placeAndBreakBlocks();
}

void Player::placeAndBreakBlocks() {
	getVisibleBlocks();

	if (!visibleBlocks.lookingAtBlock) return;

	if (InputManager::isKeyPressed(GLFW_MOUSE_BUTTON_LEFT) && gamemode != GameMode::SPECTATOR) {
		breakBlock();
		m_networkManager->sendBlockUpdatePacket(visibleBlocks.breakableBlock, 0);
	} else if (InputManager::isKeyPressed(GLFW_MOUSE_BUTTON_RIGHT) && gamemode != GameMode::SPECTATOR) {
		if(canPlaceBlock()){
			placeBlock();
			m_networkManager->sendBlockUpdatePacket(visibleBlocks.placeableBlock, Converter::itemIDToBlockID(hotbar.getSelectedItem().id));
		}
	}

	//We get the visible blocks again to update them after a block has been pressed
	getVisibleBlocks();
}


void Player::movement(float deltaTime) {
	math::vec3 camForward = m_camera->getForward();
	math::vec3 forward = math::normalize(math::vec3(camForward.x, 0.0f, camForward.z));
	math::vec3 side = math::normalize(math::cross(camForward, math::vec3(0.0f, 1.0f, 0.0f)));

	if (InputManager::isKeyDown(GLFW_KEY_W)) {
		position += forward * SPEED * deltaTime;
	}

	if (InputManager::isKeyDown(GLFW_KEY_S)) {
		position -= forward * SPEED * deltaTime;
	}

	if (InputManager::isKeyDown(GLFW_KEY_A)) {
		position -= side * SPEED * deltaTime;
	}

	if (InputManager::isKeyDown(GLFW_KEY_D)) {
		position += side * SPEED * deltaTime;
	}

	if(GameModeCanFly(gamemode)){
		if(InputManager::isKeyDown(GLFW_KEY_SPACE)){
			position.y += SPEED * deltaTime;
		}
		if(InputManager::isKeyDown(GLFW_KEY_LEFT_SHIFT)){
			position.y -= SPEED * deltaTime;
		}
	}else{
		if(InputManager::isKeyDown(GLFW_KEY_SPACE) && m_canJump){
			m_yVelocity = 10.5f;
			m_canJump = false;
		}
		m_yVelocity -= GRAVITY * deltaTime;
		// Calculating the maximum velocity
		float maxVelocity = (PLAYER_WIDTH/2 - 0.1) / deltaTime; // subtract 0.1 as a small bias because if we move at PLAYER_WIDTH/2 per frame, it could glitch through the collision detection. The reason why we do PLAYER_WIDTH and not PLAYER_HEIGHT is because if it was indeed PLAYER_HEIGHT the AABB collision detection would think it is a X or Z axis collision and push the player on those sides.
		if(m_yVelocity < -maxVelocity) m_yVelocity = -maxVelocity; // Capping the yvelocity
		position.y += m_yVelocity * deltaTime;
	}
	
}

math::vec3 Player::getEyePos() const {
	return math::vec3(position.x + 0.5f, position.y + 1.5f, position.z + 0.5f);
}

math::ivec3 vecToBlock(const math::vec3& vec) {
	return math::ivec3(math::floor(vec.x), math::floor(vec.y), math::floor(vec.z));
}

void Player::getVisibleBlocks() {
	math::ivec3 pos = vecToBlock(m_camera->getPosition());
	visibleBlocks.lookingAtBlock = false;

	math::vec3 rayPosition = m_camera->getPosition();
	for (unsigned int i = 0; i < PRECISION; i++) {
		rayPosition += m_camera->getForward() * REACH_DISTANCE / (float)PRECISION;

		visibleBlocks.breakableBlock = vecToBlock(rayPosition);
		uint8_t blockID = m_world->getBlock(visibleBlocks.breakableBlock.x, visibleBlocks.breakableBlock.y, visibleBlocks.breakableBlock.z);

		if (blockID) {
			visibleBlocks.lookingAtBlock = true;
			rayPosition -= m_camera->getForward() * REACH_DISTANCE / (float)PRECISION;
			visibleBlocks.placeableBlock = vecToBlock(rayPosition);
			break;
		}
	}
}

void Player::placeBlock() {
	m_world->setBlock(visibleBlocks.placeableBlock.x, visibleBlocks.placeableBlock.y, visibleBlocks.placeableBlock.z, Converter::itemIDToBlockID(hotbar.getSelectedItem().id));
}

void Player::breakBlock() {
	math::ivec3 vb = visibleBlocks.breakableBlock;

	uint8_t blockID = m_world->getBlock(vb.x, vb.y, vb.z);
	m_world->setBlock(vb.x, vb.y, vb.z, 0);
	m_particleHandler->placeParticlesAroundBlock(vb.x, vb.y, vb.z, blockID);
}

void Player::collideWithWorld(){
	math::ivec3 playerCenterBlock = vecToBlock(position);
	std::vector<AABB> blocksToCollideWith;
	m_canJump = false;

	for(int x = -1; x < 2; x++){
		for(int y = -1; y < 3; y++){
			for(int z = -1; z < 2; z++){
				math::ivec3 iBlockPos = playerCenterBlock + math::ivec3(x, y, z);

				if(m_world->getBlock(iBlockPos.x, iBlockPos.y, iBlockPos.z)){
					math::vec3 blockPos = math::vec3(iBlockPos.x, iBlockPos.y, iBlockPos.z);
					AABB blockBox(blockPos, math::vec3(1, 1, 1));
					blocksToCollideWith.push_back(blockBox);
				}
			}
		}
	}

	// Sort blocks
	std::stable_sort(blocksToCollideWith.begin(), blocksToCollideWith.end(), compareDistance);

	// Collision detection & response
	for(auto& i : blocksToCollideWith){
		AABB playerBox(position, math::vec3(PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH));
		if(Utils::collideBoxes(playerBox, i) == Y_AXIS){
			if(i.position.y < position.y && m_yVelocity < 0){
				m_canJump = true;
				m_yVelocity = 0.0f;
			}
		}
		position = playerBox.position;
	}
}

bool Player::compareDistance(AABB a, AABB b){
	math::vec3 centerPlayerPos = position + math::vec3(PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2, PLAYER_WIDTH / 2);

	math::vec3 centerBlockPos1 = a.position + math::vec3(0.5, 0.5, 0.5);
	math::vec3 centerBlockPos2 = b.position + math::vec3(0.5, 0.5, 0.5);

	float distance1 = math::length(math::fabs(centerBlockPos1 - centerPlayerPos));
	float distance2 = math::length(math::fabs(centerBlockPos2 - centerPlayerPos));

	return distance1 < distance2;
}

bool Player::canPlaceBlock(){
	if(hotbar.getSelectedItem().count == 0) return false;
	AABB player(position, math::vec3(PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH));
	AABB box(math::vec3(visibleBlocks.placeableBlock.x, visibleBlocks.placeableBlock.y, visibleBlocks.placeableBlock.z), math::vec3(1));
	return !Utils::collideBoxes(player, box) * Converter::itemIDToBlockID(hotbar.getSelectedItem().id);
}