#pragma once
//--------------------------------------------------------------------------------
#include <glad/glad.h>
#include "read.h"

//--------------------------------------------------------------------------------
class Shader {
public:
	Shader(const std::string& shader_source_code_path, GLenum shader_type);

	std::string shader_source_code;
	unsigned int id;
};