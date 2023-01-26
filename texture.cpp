#pragma once

//--------------------------------------------------------------------------------
#include "texture.h"

//--------------------------------------------------------------------------------
Texture::Texture() : id(0), name(""), gl_texture_identifier(0), texture_data({}) {
}

void Texture::bind() {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texture_data.number_of_channels == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_data.width, texture_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data.image_data);
	} else if (texture_data.number_of_channels == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_data.width, texture_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data.image_data);
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(gl_texture_identifier);
	glBindTexture(GL_TEXTURE_2D, id);

	free_image_data(texture_data.image_data);
}

//--------------------------------------------------------------------------------
Texture::~Texture() {

}

void Texture::load_data_from_file(std::string& path) {	
	texture_data = {};

	unsigned char* image_data = load_image_from_file(path, &texture_data.width, &texture_data.height, &texture_data.number_of_channels);
		
	if (image_data) {
		texture_data.image_data = image_data;
	} else {
		std::cout << "Failed to load texture from full path: " << path << std::endl;
	}
}

//--------------------------------------------------------------------------------
Texture_Catalog::Texture_Catalog() {

}

//--------------------------------------------------------------------------------
Texture_Catalog::~Texture_Catalog() {

}


//--------------------------------------------------------------------------------
void Texture_Catalog::load_and_bind_all_textures(std::vector<std::string>& texture_file_paths) {
	for (int i = 0; i < texture_file_paths.size(); i++) {
		Texture* texture = new Texture();
		texture->name = "texture" + std::to_string(i);
		texture->gl_texture_identifier = m_gl_texture_identifier_map[(unsigned int) textures.size()];

		texture->load_data_from_file(texture_file_paths[i]);
		textures.push_back(*texture);

		texture->bind();
	}
}
