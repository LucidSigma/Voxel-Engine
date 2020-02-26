#version 460 core

layout (location = 0) in vec2 v_textureCoordinates;

layout (location = 0) out vec4 out_colour;

uniform sampler2D u_SpriteTexture;
uniform vec3 u_Colour;

void main()
{
	out_colour = vec4(u_Colour, 1.0) * texture(u_SpriteTexture, v_textureCoordinates);
}