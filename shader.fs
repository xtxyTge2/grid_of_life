#version 330 core
out vec4 fragment_color;  
in vec4 vertex_position; 
in vec2 texture_coordinate;

uniform sampler2D texture;

void main()
{
	fragment_color = texture(texture, texture_coordinate);
}