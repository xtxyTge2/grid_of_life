#include <tracy/Tracy.hpp>
// General remark for tracy: Set the /Zi compiler flag in visual studio, or otherwise there will be a vs
// studio bug/feature regarding macro expansions. Otherwise the macros of Tracy, e.g. ZoneScoped, FrameMark wont work.

//--------------------------------------------------------------------------------
#include <iostream>

#include "opengl.hpp"
#include "state.hpp"

int main(int argc, char** argv);

//--------------------------------------------------------------------------------
void set_input_callbacks(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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
	glfwSetScrollCallback(window, scroll_callback);
}

//--------------------------------------------------------------------------------
GLFWwindow* init_glfw_glad_and_create_window(int window_width, int window_height) {
	ZoneScoped;
	std::cout << "Hello, Sailor!" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "GridOfLife3D", NULL, NULL);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
		return nullptr;
	}

	set_input_callbacks(window);

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return nullptr;
	}

	// vsync off
	// glfwSwapInterval(0);
	return window;
}
/*
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
*/
//--------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ZoneScoped;

	// pass to our state, we check that we are not in debug window
	if (window == g_state->window) {
		g_state->framebuffer_size_callback(width, height);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	ZoneScoped;

	// pass to our state, we check that we are not in debug window
	if (window == g_state->window) {
		g_state->scroll_callback(xoffset, yoffset);
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

	return 0;
}

