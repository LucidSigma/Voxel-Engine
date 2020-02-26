#version 460 core

layout (location = 0) out vec4 out_colour;

layout (location = 0) in vec2 v_textureCoordinates;

uniform sampler2D u_Text;
uniform vec3 u_TextColour;

void main()
{
	float alpha = texture(u_Text, v_textureCoordinates).r;
	alpha = alpha < 0.5 ? 0.0 : 1.0;
	alpha *= smoothstep(0.25, 0.75, alpha);

	out_colour = vec4(u_TextColour, alpha);
}