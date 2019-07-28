#version 150

#include "../includes/PSCommon.glsl"

in vec3 v_Normal;
in mediump vec4 v_Diffuse;

void main(void)
{
	SV_Target0 = v_Diffuse;
}