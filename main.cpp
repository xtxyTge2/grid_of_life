#include "Tracy.hpp"


// memory leak detection on windows
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type#
#else
#define DBG_NEW new
#endif

//#define EIGEN_NO_DEBUG

// General remark for tracy: Set the /Zi compiler flag in visual studio, or otherwise there will be a vs
// studio bug/feature regarding macro expansions. Otherwise the macros of Tracy, e.g. ZoneScoped, FrameMark wont work.

//--------------------------------------------------------------------------------
#include <iostream>
#include <vector>

#include "renderer.hpp"

#include "opengl.hpp"
#include "state.hpp"
#include "texture.hpp"

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
void GLAPIENTRY GLDebugMessageCallback( GLenum source,
	                GLenum type,
	                GLuint id,
	                GLenum severity,
	                GLsizei length,
	                const GLchar* message,
	                const void* userParam);

GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height);

//--------------------------------------------------------------------------------
std::unique_ptr<State> g_state = std::make_unique<State>();

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

//--------------------------------------------------------------------------------
GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height) {
	ZoneScoped;
	std::cout << "Hello, Sailor!" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef DEBUG_OPENGL
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);	
#else
	
#endif
	

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "GridOfLife3D", NULL, NULL);

	set_input_callbacks(window);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return nullptr;
	}
#ifdef DEBUG_OPENGL
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GLDebugMessageCallback, NULL);
#endif

	// vsync off
	// glfwSwapInterval(0);
	return window;
}

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *msg, const void *data)
{
	std::string _source;
	std::string _type;
	std::string _severity;

	switch (source) {
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW SYSTEM";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER COMPILER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			_source = "UNKNOWN";
			break;

		default:
			_source = "UNKNOWN";
			break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UDEFINED BEHAVIOR";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;

		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;

		default:
			_type = "UNKNOWN";
			break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "HIGH";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "MEDIUM";
			break;

		case GL_DEBUG_SEVERITY_LOW:
			_severity = "LOW";
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			_severity = "NOTIFICATION";
			break;

		default:
			_severity = "UNKNOWN";
			break;
	}

	std::cout << id << " : " << _type << " of " << _severity << " severity, raised from " << _source <<
		": " << msg << "\n";
}

//--------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ZoneScoped;

	if (window == g_state->window) {
		g_state->framebuffer_size_callback(width, height);
	}
}

//--------------------------------------------------------------------------------
int main(int argc, char** argv) {
	
	GLFWwindow* window = init_glfw_glad_and_create_window(1920, 1080);
	if (window == nullptr) {
		std::cout << "Failed to initialize glfw and create a window." << std::endl;
		return -1;
	}
	
	g_state->initialise(window);
	
	// ------------------------------------------------------------------
	while (!g_state->should_quit()) {	
		g_state->update();
		
		FrameMark;
	}
	
	glfwTerminate();

	// delete unique ptr to state, so after this state gets freed. Do this to see memory leaks below.
	g_state.reset();

#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

