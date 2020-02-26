#version 460 core

layout (location = 0) in vec3 v_textureCoordinates;

layout (location = 0) out vec4 out_colour;

uniform samplerCube u_SkyboxCubemap;

void main()
{
	out_colour = texture(u_SkyboxCubemap, v_textureCoordinates);
}