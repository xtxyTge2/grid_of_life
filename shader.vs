#version 330 core
layout (location = 0) in vec3 a_position;   
layout (location = 1) in vec2 a_texture_coordinate; 

out vec4 vertex_position;
out vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{	
    gl_Position = projection * view * model * vec4(a_position, 1.0f);
	vertex_position = gl_Position;
	texture_coordinate = a_texture_coordinate;
}     