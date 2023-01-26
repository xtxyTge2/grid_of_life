#pragma once
//--------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <unordered_map>

#include "opengl.h"
#include "read.h"

struct Texture_Data {
	int width;
	int height;
	int number_of_channels;

	unsigned char* image_data;
};

//--------------------------------------------------------------------------------
class Texture {
public:
	Texture();

	~Texture();

	void bind();

	void load_data_from_file(std::string& path);
	//--------------------------------------------------------------------------------
	// data
	GLuint id;
	std::string name;
	GLenum gl_texture_identifier;
	Texture_Data texture_data;
};

//--------------------------------------------------------------------------------
class Texture_Catalog {
public:
	Texture_Catalog();

	~Texture_Catalog();

	void load_and_bind_all_textures(std::vector<std::string>& texture_file_paths);
	//--------------------------------------------------------------------------------
	// data
	std::vector<Texture> textures;

	std::unordered_map<GLuint, GLenum> m_gl_texture_identifier_map = {
		{0, GL_TEXTURE0},
		{1, GL_TEXTURE1},
		{2, GL_TEXTURE2},
		{3, GL_TEXTURE3},
		{4, GL_TEXTURE4},
		{5, GL_TEXTURE5},
		{6, GL_TEXTURE6},
		{7, GL_TEXTURE7},
		{8, GL_TEXTURE8},
		{9, GL_TEXTURE9},
		{10, GL_TEXTURE10},
		{11, GL_TEXTURE11},
		{12, GL_TEXTURE12},
		{13, GL_TEXTURE13},
		{14, GL_TEXTURE14},
		{15, GL_TEXTURE15},
		{16, GL_TEXTURE16},
		{17, GL_TEXTURE17},
		{18, GL_TEXTURE18},
		{19, GL_TEXTURE19},
		{20, GL_TEXTURE20},
		{21, GL_TEXTURE21},
		{22, GL_TEXTURE22}
	};
};
