#version 150

#include "../includes/VSCommon.glsl"

in vec3 Pos;
in mediump vec4 Col0;
in mediump vec2 Tex0;

out mediump vec4 v_Diffuse;
out mediump vec2 v_Tex0;

void main(void)
{
	gl_Position = g_ObjectToClipPos(Pos);

	v_Diffuse = Col0.zyxw;

	v_Tex0.xy = Tex0.xy;
}