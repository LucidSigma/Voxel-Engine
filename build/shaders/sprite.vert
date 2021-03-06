#version 460 core

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_textureCoordinates;

layout (location = 0) out vec2 v_textureCoordinates;

uniform mat4 u_Projection;
uniform mat4 u_Model;

void main()
{
	v_textureCoordinates = in_position;
	
	gl_Position = u_Projection * u_Model * vec4(in_position, 0.0, 1.0);
}