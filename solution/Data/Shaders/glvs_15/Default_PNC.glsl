#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;
in vec3 Normal;
in mediump vec4 Col0;

out vec3 v_Normal;
out mediump vec4 v_Diffuse;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);

	v_Normal = Mul3(g_ObjectToWorld, Normal);

	v_Diffuse = Col0.zyxw;
}