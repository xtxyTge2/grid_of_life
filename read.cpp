#pragma once

//--------------------------------------------------------------------------------
#include "read.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Tracy.hpp"

//--------------------------------------------------------------------------------
std::string read_from_file_into_std_string(const std::string& full_path) {
	ZoneScoped;
	std::ifstream t(full_path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}

//--------------------------------------------------------------------------------
unsigned char* load_image_from_file(const std::string& full_path, int* width, int* height, int* number_of_channels) {
	ZoneScoped;
	//stbi_set_flip_vertically_on_load(true);
	return stbi_load(full_path.c_str(), width, height, number_of_channels, 0);
}

//--------------------------------------------------------------------------------
void free_image_data(unsigned char* data) {
	ZoneScoped;
	stbi_image_free(data);
}