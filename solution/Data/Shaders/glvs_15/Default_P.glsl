#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);
}