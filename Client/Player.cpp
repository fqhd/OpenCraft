#include "Player.hpp"
#include "Constants.hpp"
#include "Utils.hpp"


void Player::update(Camera& camera, Settings& settings, std::vector<vec3>& colors, ParticleHandler& handler, World& world, float deltaTime, sf::TcpSocket& socket){

     getVisibleBlocks(camera, world);

     if(!visibleBlocks.lookingAtBlock) return;

     if(InputManager::isButtonPressed(GLFW_MOUSE_BUTTON_LEFT)){
          breakBlock(handler, colors, world);
          sendBlockData(visibleBlocks.breakableBlock, 0, socket);
     } else if(InputManager::isButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)){
          placeBlock(world);
          sendBlockData(visibleBlocks.placeableBlock, selectedBlock, socket);
     }

}

void Player::getVisibleBlocks(Camera& camera, World& world){
     visibleBlocks.lookingAtBlock = false;

     glm::vec3 rayPosition = camera.getPosition();
     for(unsigned int i = 0; i < Constants::getPrecision(); i++){
          rayPosition += camera.getForward() * (Constants::getPlayerReachDistance() / (float)Constants::getPrecision());

		visibleBlocks.breakableBlock = vecToBlock(rayPosition);
          uint8_t blockID = world.getBlock(visibleBlocks.breakableBlock.x, visibleBlocks.breakableBlock.y, visibleBlocks.breakableBlock.z);

          if(blockID){
               visibleBlocks.lookingAtBlock = true;
               rayPosition -= camera.getForward() * (Constants::getPlayerReachDistance() / (float)Constants::getPrecision());
               visibleBlocks.placeableBlock = vecToBlock(rayPosition);
               break;
          }
     }
}

void Player::placeBlock(World& world){
     world.setBlock(visibleBlocks.placeableBlock.x, visibleBlocks.placeableBlock.y, visibleBlocks.placeableBlock.z, selectedBlock);
}

void Player::breakBlock(ParticleHandler& handler, std::vector<vec3>& colors, World& world){
     uint8_t blockID = world.getBlock(visibleBlocks.breakableBlock.x, visibleBlocks.breakableBlock.y, visibleBlocks.breakableBlock.z);
     world.setBlock(visibleBlocks.breakableBlock.x, visibleBlocks.breakableBlock.y, visibleBlocks.breakableBlock.z, 0);
     handler.placeParticlesAroundBlock(visibleBlocks.breakableBlock.x, visibleBlocks.breakableBlock.y, visibleBlocks.breakableBlock.z, colors[blockID]);
}

void Player::sendBlockData(const glm::ivec3& blockUpdate, uint8_t block, sf::TcpSocket& socket){

     sf::Packet packet;
     packet << blockUpdate.x << blockUpdate.y << blockUpdate.z << block;
     socket.send(packet);
	
}

glm::ivec3 Player::vecToBlock(const glm::vec3& vec){
	return glm::ivec3(floor(vec.x), floor(vec.y), floor(vec.z));
}
