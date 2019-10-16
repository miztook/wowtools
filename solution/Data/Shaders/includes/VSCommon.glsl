#ifndef VSCOMMON_INCLUDED
#define VSCOMMON_INCLUDED

#include "UnityShaderVariables.glsl"

float Saturate(float x)
{
    return max(0.0, min(1.0, x));
}

vec3 Mul( mat3 matrix, vec3 pos )
{
	vec3 vResult;
    vResult.x = dot( pos, matrix[0].xyz );
    vResult.y = dot( pos, matrix[1].xyz );
    vResult.z = dot( pos, matrix[2].xyz );
	return vResult;
}

vec4 Mul( mat4 matrix, vec4 pos )
{
	vec4 vResult;
    vResult.x = dot( pos, matrix[0].xyzw );
    vResult.y = dot( pos, matrix[1].xyzw );
    vResult.z = dot( pos, matrix[2].xyzw );
	vResult.w = dot( pos, matrix[3].xyzw );
	return vResult;
}

vec4 g_ObjectToClipPos(vec3 pos)
{
    // More efficient than computing M*VP matrix product
    return Mul(UNITY_MATRIX_VP, Mul(g_ObjectToWorld, vec4(pos, 1.0)));
}

#endif