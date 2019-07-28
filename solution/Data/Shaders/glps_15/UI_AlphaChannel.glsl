#version 150

#include "../includes/PSCommon.glsl"

in mediump vec4 v_Diffuse;
in mediump vec2 v_Tex0;

uniform sampler2D _MainTex;

void main(void)
{
	mediump vec4 color;
	mediump vec4 texCol = texture2D(_MainTex, v_Tex0.xy);
	color.rgb = texCol.rgb * v_Diffuse.rgb;
	color.a = texCol.a;

    // Copy the primary color
    SV_Target0 = color;
}