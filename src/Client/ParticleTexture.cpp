#include "ParticleTexture.hpp"
#include "Image.hpp"

// These values must match the resolution of the particle texture
const unsigned int IMG_WIDTH = 128;

void ParticleTexture::loadFromFile(const std::string& path){
	// Creating the texture
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Loading the image data from the file and loading it to the GPU(generated texture)
	Image image;
	image.loadFromFile(path);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());
	image.free();
	glBindTexture(GL_TEXTURE_2D, 0);
	populateUVQuadsArray();
}

void ParticleTexture::bind(){
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void ParticleTexture::unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ParticleTexture::destroy(){
	glDeleteTextures(1, &m_textureID);
}

math::vec4 indexToQuad(float x, float y){
	math::vec4 v;
	v.x = 1 + x * 4 + (x);
	v.y = 1 + y * 4 + (y);
	v.z = 4;
	v.w = 4;
	return v;
}

math::vec4 ParticleTexture::getUVQuad(ParticleID _particleID) {
	return m_uvQuads[(unsigned int)_particleID];
}

void ParticleTexture::populateUVQuadsArray(){
	// This function fills the m_uvQuads Array with math::vec4s of UV quads in the same order their particle IDs are defined in ParticleID.hpp
	unsigned int i = 0;
	m_uvQuads[i++] = indexToQuad(0, 0) / IMG_WIDTH; // DIRT
	m_uvQuads[i++] = indexToQuad(1, 0) / IMG_WIDTH; // STONE
}
