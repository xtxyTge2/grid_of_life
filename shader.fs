#version 430 core
out vec4 fragment_color;  
in vec4 vertex_position; 
in vec2 texture_coordinate;

uniform sampler2D texture1;

void main()
{
	fragment_color = texture(texture1, texture_coordinate);
}