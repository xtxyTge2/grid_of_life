#pragma once

//--------------------------------------------------------------------------------
#include "shader.hpp"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
Shader::Shader(const std::string& shader_source_code_path, GLenum shader_type) {
	ZoneScoped;
	shader_source_code = read_from_file_into_std_string(shader_source_code_path);
	//shader_source_code = &file_contents;
	const char* shader_code = shader_source_code.c_str();

	id = glCreateShader(shader_type);
	glShaderSource(id, 1, &shader_code, NULL);
	glCompileShader(id);

	// check shader compilation status
	int success;
	char info_log[2048];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(id, 2048, NULL, info_log);
		std::cout << "ERROR::SHADER::" << shader_type << "::COMPILATION_FAILED\n" << info_log << std::endl;
	}
};

