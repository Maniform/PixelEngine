#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <unordered_map>
#include <vector>

struct GLFWwindow;

class PixelEngine
{
public:
	PixelEngine(int width, int height, int frameWidth = 800, int frameHeight = 600);
	~PixelEngine();

	bool update();
	void clear();
	
	void setPixelColor(unsigned int x, unsigned int y, glm::vec3 color);
	glm::vec3 getPixelColor(unsigned int x, unsigned int y) const;
	
	GLFWwindow* getWindow() const;
	std::unordered_map<unsigned int, int> getKeys() const;
	int getKey(unsigned int key) const;
	double getTime() const;
	
private:
	static PixelEngine* pixelEngine;
	
	std::string shaderFolder = SHADER_FOLDER;
	
	GLFWwindow* window;
	unsigned int program;
	unsigned int pixelBuffer;
	unsigned int vao;
	unsigned int fbo;
	unsigned int fbot; //framebuffer texture
	
	glm::ivec2 frameSize;
	glm::ivec2 size;
	unsigned short overscan;
	std::vector<glm::vec3> pixels;
	bool dirty;

	std::unordered_map<unsigned int, int> keys;
	
	std::string loadShaderFile(const std::string& filename);
	unsigned int compileShader(unsigned int type, const char *source);
	unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader);
	
	void render();
	static void onResized(GLFWwindow* window, int width, int height);
	static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
};
