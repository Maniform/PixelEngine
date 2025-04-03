#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

class PixelEngine
{
public:
	PixelEngine(int width, int height, int frameWidth = 800, int frameHeight = 600);
	~PixelEngine();

	bool update();
	void clear();
	
	void setPixelColor(unsigned int x, unsigned int y, glm::vec3 color);
	glm::vec3 getPixelColor(unsigned int x, unsigned int y) const;
	
private:
	static PixelEngine* pixelEngine;
	
	std::string shaderFolder = SHADER_FOLDER;
	
	GLFWwindow* window;
	GLuint program;
	GLuint pixelBuffer;
	GLuint vao;
	GLuint fbo;
	GLuint fbot; //framebuffer texture
	
	glm::ivec2 frameSize;
	glm::ivec2 size;
	unsigned short overscan;
	std::vector<glm::vec3> pixels;
	bool dirty;
	
	std::string loadShaderFile(const std::string& filename);
	GLuint compileShader(GLenum type, const char *source);
	GLuint linkShaders(GLuint vertexShader, GLuint fragmentShader);
	
	void render();
	static void onResized(GLFWwindow* window, int width, int height);
};
