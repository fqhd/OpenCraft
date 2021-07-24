#pragma once

#include <SFML/Network.hpp>
#include "World.hpp"
#include "ParticleHandler.hpp"
#include "EntityHandler.hpp"
#include "Camera.hpp"
#include "Config.hpp"

class World;
class EntityHandler;

class NetworkManager {
public:

	friend class EntityHandler;

	void connectToServer(sf::IpAddress& _ip, Config* _c);
	void receiveGameUpdatePacket(World* _world, ParticleHandler& _pHandler, EntityHandler& _eHandler);
	void sendPositionDataToServer(Camera& _camera);
	void sendBlockUpdatePacket(const math::ivec3& _blockPosition, uint8_t _blockType);
	void downloadWorld(uint8_t* _data);

private:

	sf::Clock m_dataFrequencyTimer;
	sf::IpAddress m_serverIp;
	sf::TcpSocket m_tcpSocket;
	sf::UdpSocket m_udpSocket;
	uint8_t m_id = 0;
	Config* m_config = nullptr;

};
