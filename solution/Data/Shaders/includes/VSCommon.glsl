#ifndef VSCOMMON_INCLUDED
#define VSCOMMON_INCLUDED

#include "Common.glsl"

float Saturate(float x)
{
    return max(0.0, min(1.0, x));
}

vec3 Mul3( mat3 matrix, vec3 pos )
{
	vec3 vResult;
    vResult.x = dot( pos, matrix[0].xyz );
    vResult.y = dot( pos, matrix[1].xyz );
    vResult.z = dot( pos, matrix[2].xyz );
	return vResult;
}

vec4 Mul4( mat4 matrix, vec4 pos )
{
	vec4 vResult;
    vResult.x = dot( pos, matrix[0].xyzw );
    vResult.y = dot( pos, matrix[1].xyzw );
    vResult.z = dot( pos, matrix[2].xyzw );
	vResult.w = dot( pos, matrix[3].xyzw );
	return vResult;
}

vec4 UnityObjectToClipPos(vec3 pos)
{
    // More efficient than computing M*VP matrix product
    return Mul4(UNITY_MATRIX_VP, Mul4(unity_ObjectToWorld, vec4(pos, 1.0)));
}

#endif