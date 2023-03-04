#pragma once

//--------------------------------------------------------------------------------
#include <tracy/Tracy.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//--------------------------------------------------------------------------------
std::string read_from_file_into_std_string(const std::string& full_path);

unsigned char* load_image_from_file(const std::string& full_path, int* width, int* height, int* number_of_channels);

void free_image_data(unsigned char*);