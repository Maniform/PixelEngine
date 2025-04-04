#include <PixelEngine.h>

#include <fstream>

using namespace std;
using namespace glm;

PixelEngine* PixelEngine::pixelEngine = nullptr;

PixelEngine::PixelEngine(int width, int height, int frameWidth, int frameHeight)
	: window(nullptr)
	, frameSize(frameWidth, frameHeight)
	, size(width, height)
	, overscan(10)
	, pixels(0)
	, dirty(true)
{
	pixelEngine = this;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_REVISION, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(frameSize.x, frameSize.y, "PixelEngine", nullptr, nullptr);
	glfwSetFramebufferSizeCallback(window, onResized);
	glfwMakeContextCurrent(window);
	
	glewInit();
	
	//glfwSwapInterval(1);
	glfwGetFramebufferSize(window, &frameSize.x, &frameSize.y);
	
	string vertexCode = loadShaderFile(shaderFolder + "screen.vert");
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
	string fragmentCode = loadShaderFile(shaderFolder + "screen.frag");
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());
	program = linkShaders(vertexShader, fragmentShader);
	
	glUseProgram(program);
	glUniform2ui(glGetUniformLocation(program, "size"), size.x, size.y);
	glUseProgram(0);
	
	pixels.resize(size.x * size.y);
	fill(pixels.begin(), pixels.end(), vec3(0, 0, 0));
//	bool value = true;
//	unsigned int index = 0;
//	for(uint8_t i = 0 ; i < size.y ; i++)
//	{
//		for(uint8_t j = 0 ; j < size.x ; j++)
//		{
//			if(i == 0 || i == size.y - 1)
//			{
//				pixels[index] = vec3(0.0, 1.0, 0.0);
//			}
//			else if(j == 0 || j == size.x - 1)
//			{
//				pixels[index] = vec3(0.0, 0.0, 1.0);
//			}
//			else
//			{
//				pixels[index] = value ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 0.0, 0.0);
//			}
//			value = !value;
//			index++;
//		}
//		value = !value;
//	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &pixelBuffer);
	
	glBindVertexArray(vao);
	//Color buffer
	glBindBuffer(GL_ARRAY_BUFFER, pixelBuffer);
	glBufferData(GL_ARRAY_BUFFER, pixels.size() * sizeof(vec3), pixels.data(), GL_STATIC_DRAW);
	int location = glGetAttribLocation(program, "color");
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(location);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &fbot);
	glBindTexture(GL_TEXTURE_2D, fbot);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbot, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PixelEngine::~PixelEngine()
{
	glDeleteProgram(program);
	glDeleteBuffers(1, &pixelBuffer);
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &fbot);
	glDeleteTextures(1, &fbot);
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	pixelEngine = nullptr;
}

bool PixelEngine::update()
{
	if(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		if(dirty)
		{
			render();
			dirty = false;
		}
		
		return true;
	}
	
	return false;
}

void PixelEngine::clear()
{
	std::fill(pixels.begin(), pixels.end(), vec3(0.0f, 0.0f, 0.0f));
	dirty = true;
}

void PixelEngine::setPixelColor(unsigned int x, unsigned int y, glm::vec3 color)
{
	pixels[x + y * size.x] = color;
	dirty = true;
}

vec3 PixelEngine::getPixelColor(unsigned int x, unsigned int y) const
{
	return pixels[x + y * size.x];
}

GLFWwindow* PixelEngine::getWindow() const
{
	return window;
}

string PixelEngine::loadShaderFile(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		throw runtime_error("Failed to open shader file: " + filename);
	}
	return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

GLuint PixelEngine::compileShader(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	if (shader == 0)
	{
		throw runtime_error("Erreur lors de la création du shader");
	}

	// Transmettre le code source au shader
	glShaderSource(shader, 1, &source, nullptr);

	// Compiler le shader
	glCompileShader(shader);

	// Vérifier s'il y a des erreurs de compilation
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		glDeleteShader(shader);

		throw runtime_error("Erreur de compilation du shader: " + string(infoLog));
	}

	return shader;
}

GLuint PixelEngine::linkShaders(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram();
	if (program == 0)
	{
		throw runtime_error("Erreur lors de la création du programme de shaders");
	}
	
	// Attacher les shaders au programme
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	
	// Linker le programme
	glLinkProgram(program);
	
	// Vérifier s'il y a des erreurs de linkage
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(program);
		
		throw runtime_error("Erreur de linkage du programme: " + string(infoLog));
	}
	
	// Supprimer les shaders après le linkage
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return program;
}

void PixelEngine::render()
{
	glViewport(0, 0, frameSize.x, frameSize.y);
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Update color buffer
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, pixelBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pixels.size() * sizeof(vec3), pixels.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, size.x, size.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, size.x * size.y);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, size.x, size.y, overscan, overscan, frameSize.x - overscan, frameSize.y - overscan, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	glfwSwapBuffers(window);
}

void PixelEngine::onResized(GLFWwindow *window, int width, int height)
{
	if(pixelEngine)
	{
		pixelEngine->frameSize = ivec2(width, height);
		pixelEngine->dirty = true;
	}
}
