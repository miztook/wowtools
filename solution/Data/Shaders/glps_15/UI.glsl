#version 150

#include "../includes/PSCommon.glsl"

in mediump vec4 v_Diffuse;
in mediump vec2 v_Tex0;

uniform sampler2D _MainTex;

void main(void)
{
	mediump vec4 texCol = texture2D(_MainTex, v_Tex0.xy);
	mediump vec4 color = texCol * v_Diffuse;
	
#ifdef _USE_ALPHA_
	color.a = v_Diffuse.a;
#else
	color.a = 1.0;
#endif

#ifdef _USE_ALPHA_CHANNEL_
	color.a = texCol.a * color.a;
#endif
	
	// Copy the primary color
	SV_Target0 = color;
}
