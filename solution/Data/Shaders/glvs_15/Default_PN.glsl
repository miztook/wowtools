#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;
in vec3 Normal;

out vec3 v_Normal;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);

	v_Normal = Mul3(g_ObjectToWorld, Normal);
}