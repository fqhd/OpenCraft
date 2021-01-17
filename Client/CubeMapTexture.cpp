#include "CubeMapTexture.hpp"
#include "Image.hpp"

void CubeMapTexture::init(const std::vector<std::string>& locations) {

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	for(unsigned int i = 0; i < locations.size(); i++){
		Image image;
		image.loadFromFile(locations[i]);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getData());

		image.free();
	}

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void CubeMapTexture::bind(){
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
}

void CubeMapTexture::unbind(){
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMapTexture::destroy(){
    glDeleteTextures(1, &m_textureID);
}
