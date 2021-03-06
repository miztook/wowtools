#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;
in mediump vec4 Col0;

out mediump vec4 v_Diffuse;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);

	v_Diffuse = Col0.zyxw;
}