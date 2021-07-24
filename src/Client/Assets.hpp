#pragma once

#include "Engine/Utils/Cube.hpp"

class Assets {
public:

	void init();
	const Cube& getCube() const;
	void destroy();

private:

	Cube m_cube;

};