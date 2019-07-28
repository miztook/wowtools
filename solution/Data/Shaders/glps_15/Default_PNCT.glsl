#version 150

#include "../includes/PSCommon.glsl"

in mediump vec4 v_Diffuse;
in mediump vec2 v_Tex0;
in vec3 v_Normal;

uniform sampler2D _MainTex;

void main(void)
{
	mediump vec4 color = texture2D(_MainTex, v_Tex0) * v_Diffuse;
	SV_Target0 = color;
}