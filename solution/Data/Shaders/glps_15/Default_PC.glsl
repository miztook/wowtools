#version 150

#include "../includes/PSCommon.glsl"

in mediump vec4 v_Diffuse;

void main(void)
{
	SV_Target0 = v_Diffuse;
}