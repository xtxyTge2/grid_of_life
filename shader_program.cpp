#pragma once
//--------------------------------------------------------------------------------
#include "shader_program.h"

//--------------------------------------------------------------------------------
Shader_Program::Shader_Program(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
	id = glCreateProgram();
	vertex_shader = std::make_unique<Shader>(vertex_shader_path, GL_VERTEX_SHADER);
	fragment_shader = std::make_unique<Shader>(fragment_shader_path, GL_FRAGMENT_SHADER);
}

//--------------------------------------------------------------------------------
Shader_Program::~Shader_Program() {
	glDeleteProgram(id);
}

//--------------------------------------------------------------------------------
void Shader_Program::link_and_cleanup() {
	glAttachShader(id, vertex_shader->id);
	glAttachShader(id, fragment_shader->id);
	glLinkProgram(id);

	// check shader program linking status
	int success;
	char info_log[512];
	glGetProgramiv(id, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// shaders got linked and are not used anymore
	glDeleteShader(vertex_shader->id);
	glDeleteShader(fragment_shader->id);
}

//--------------------------------------------------------------------------------
void Shader_Program::use() {
	glUseProgram(id);
}

//--------------------------------------------------------------------------------
void Shader_Program::set_uniform_int(const std::string& name, int value) {
	use();
	int location = glGetUniformLocation(id, name.c_str());
	glUniform1i(location, (int)value);
}

//--------------------------------------------------------------------------------
void Shader_Program::set_uniform_mat4(const std::string& name, glm::mat4 value) {
	use();
	unsigned int location = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader_Program::load_texture_catalog(Texture_Catalog& catalog) {
	for (int i = 0; i < catalog.textures.size(); i++) {
		set_uniform_int(catalog.textures[i].name, i);
	}
}