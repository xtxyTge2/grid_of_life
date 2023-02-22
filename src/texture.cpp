//--------------------------------------------------------------------------------
#include "texture.hpp"

//--------------------------------------------------------------------------------
Texture::Texture(std::string texture_name, GLenum identifier, std::string texture_data_path) : 
	id(0), 
	name(texture_name), 
	gl_texture_identifier(identifier), 
	data_path(texture_data_path), 
	texture_data({}) 
{

}

void Texture::bind() {
	ZoneScoped;
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


void Texture::load_data_from_file() {	
	ZoneScoped;

	unsigned char* image_data = load_image_from_file(data_path, &texture_data.width, &texture_data.height, &texture_data.number_of_channels);
		
	if (image_data) {
		texture_data.image_data = image_data;
	} else {
		std::cout << "Failed to load texture from full path: " << data_path << std::endl;
	}
}


//--------------------------------------------------------------------------------
void Texture_Catalog::load_and_bind_all_textures(std::vector<std::string>& texture_file_paths) {
	ZoneScoped;

	textures.clear();
	textures.reserve(texture_file_paths.size());
	for (int i = 0; i < texture_file_paths.size(); i++) {
		std::string texture_name = "texture" + std::to_string(i);
		GLenum gl_texture_identifier = m_gl_texture_identifier_map[(unsigned int) i];
		std::string data_path = texture_file_paths[i];
		textures.emplace_back(texture_name, gl_texture_identifier, data_path);
	}

	for (Texture& texture: textures) {
		texture.load_data_from_file();
		texture.bind();
	}
}
