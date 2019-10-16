#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;
in vec3 Normal;
in mediump vec2 Tex0;

out vec3 v_Normal;
out mediump vec2 v_Tex0;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);

	v_Normal = Mul(mat3(g_ObjectToWorld), Normal);

	v_Tex0.xy = Tex0.xy;
}