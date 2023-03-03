#pragma once
//--------------------------------------------------------------------------------
#include <Tracy/Tracy.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.hpp"
#include "texture.hpp"

#include "read.hpp"

//--------------------------------------------------------------------------------
class Shader {
public:
	Shader(const std::string& shader_source_code_path, GLenum shader_type);

	//--------------------------------------------------------------------------------
	// data
	std::string shader_source_code;
	unsigned int id;
};



//--------------------------------------------------------------------------------
class Shader_Program {
public:
	Shader_Program();

	Shader_Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

	~Shader_Program();

	void link_and_cleanup();

	void use();

	void set_uniform_int(const std::string &name, int value);	

	void set_uniform_mat4(const std::string& name, glm::mat4 value);

	void load_texture_catalog(Texture_Catalog& catalog);
	//--------------------------------------------------------------------------------
	// data
	unsigned int id;
	std::unique_ptr<Shader> vertex_shader;
	std::unique_ptr<Shader> fragment_shader;
};