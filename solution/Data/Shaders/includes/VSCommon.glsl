#ifndef VSCOMMON_INCLUDED
#define VSCOMMON_INCLUDED

#include "UnityShaderVariables.glsl"
#include "Lighting.glsl"

vec3 Mul( mat3 matrix, vec3 pos )
{
	vec3 vResult = pos * matrix;
	return vResult;
}

vec4 Mul( mat4 matrix, vec4 pos )
{
	vec4 vResult = pos * matrix;
	return vResult;
}

vec4 g_ObjectToClipPos(vec3 pos)
{
    // More efficient than computing M*VP matrix product
    return Mul(UNITY_MATRIX_VP, Mul(g_ObjectToWorld, vec4(pos, 1.0)));
}

#endif