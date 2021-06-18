#ifndef GUI_FONT_SHADER_H
#define GUI_FONT_SHADER_H

#include "../Utils/Shader.hpp"
#include <glm/glm.hpp>
#include "../Utils/Vertex.hpp"

class GUIFontShader : public Shader {
public:

	void init();

	//Loading Functions
	void loadMatrix(const glm::mat4& matrix);
	void loadColor(const ColorRGBA8& color);
	void loadPosition(const glm::vec2& position);



private:

	void getUniformLocations();



	//Uniforms
	GLint m_matrixLocation = 0;
	GLint m_colorLocation = 0;
	GLint m_positionLocation = 0;

};

#endif