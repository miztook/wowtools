#version 150

#include "../includes/PSCommon.glsl"

in mediump vec4 v_Diffuse;
in mediump vec2 v_Tex0;

uniform sampler2D _MainTex;

void main(void)
{	
	mediump vec4 color = texture2D(_MainTex, v_Tex0) * v_Diffuse;
	color.a = v_Diffuse.a;
	
	// Copy the primary color
    SV_Target0 = color;
}