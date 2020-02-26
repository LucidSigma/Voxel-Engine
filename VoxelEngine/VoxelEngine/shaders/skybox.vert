#version 460 core

layout (location = 0) in vec3 in_position;

layout (location = 0) out vec3 v_textureCoordinates;

uniform mat4 u_ViewProjection;

void main()
{
	v_textureCoordinates = in_position;
	v_textureCoordinates.y *= -1.0;

	gl_Position = (u_ViewProjection * vec4(in_position, 1.0)).xyww;
}