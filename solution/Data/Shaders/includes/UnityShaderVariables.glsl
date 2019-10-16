#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#define CBUFFER_START(name) uniform name {
#define CBUFFER_END };

uniform mat4 g_ObjectToWorld;
uniform mat4 g_MatrixVP;
uniform mat4 g_MatrixV;
uniform mat4 g_MatrixInvV;
uniform mat4 g_MatrixP;

#define UNITY_MATRIX_P g_MatrixP
#define UNITY_MATRIX_V g_MatrixV
#define UNITY_MATRIX_I_V g_MatrixInvV
#define UNITY_MATRIX_VP g_MatrixVP
#define UNITY_MATRIX_M g_ObjectToWorld

CBUFFER_START(UnityPerCamera)

	vec3 _WorldSpaceCameraPos;

	// x = 1 or -1 (-1 if projection is flipped)
    // y = near plane
    // z = far plane
    // w = 1/far plane
    vec4 _ProjectionParams;
	
	// x = width
    // y = height
    // z = 1 + 1.0/width
    // w = 1 + 1.0/height
    vec4 _ScreenParams;
	
	vec4 _ZBufferParams;
	
	// x = orthographic camera's width
    // y = orthographic camera's height
    // z = unused
    // w = 1.0 if camera is ortho, 0.0 if perspective
    vec4 unity_OrthoParams;
	
CBUFFER_END

CBUFFER_START(UnityLighting)
	#ifdef USING_DIRECTIONAL_LIGHT
    mediump vec4 _WorldSpaceLightPos0;
    #else
    vec4 _WorldSpaceLightPos0;
    #endif
CBUFFER_END

CBUFFER_START(UnityPerFrame)
	mediump vec4 unity_ShadowColor;
CBUFFER_END

CBUFFER_START(UnityFog)
    mediump vec4 unity_FogColor;
	
	// x = density / sqrt(ln(2)), useful for Exp2 mode
    // y = density / ln(2), useful for Exp mode
    // z = -1/(end-start), useful for Linear mode
    // w = end/(end-start), useful for Linear mode
    vec4 unity_FogParams;
	
CBUFFER_END

//Lightmaps
uniform sampler2D 	unity_Lightmap;
uniform sampler2D	unity_ShadowMask;

CBUFFER_START(UnityLightmaps)
    vec4 unity_LightmapST;
CBUFFER_END
	
#endif