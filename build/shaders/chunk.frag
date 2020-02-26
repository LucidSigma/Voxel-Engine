#version 460 core

layout (location = 0) in vec3 v_textureCoordinates;

layout (location = 0) out vec4 out_colour;

uniform sampler2DArray u_BlockTexture;

void main()
{
	out_colour = texture(u_BlockTexture, v_textureCoordinates);

	if (out_colour.a < 0.9)
	{
		discard;
	}
}