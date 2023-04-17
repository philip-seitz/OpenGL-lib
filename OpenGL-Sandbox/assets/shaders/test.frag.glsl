#version 450 core

layout (location = 0) out vec4 o_Color;
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform vec4 u_Color;
uniform sampler2D u_Textures[2];

void main()
{
	int i_TexIndex = int(v_TexIndex);
	vec4 texColor = texture(u_Textures[i_TexIndex], v_TexCoord);
	o_Color = texColor;
}