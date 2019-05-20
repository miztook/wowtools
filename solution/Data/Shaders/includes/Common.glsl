#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#define CBUFFER_START(name) cbuffer name {
#define CBUFFER_END };

uniform mat4 unity_ObjectToWorld;
uniform mat4 unity_WorldToObject;
uniform mat4 unity_MatrixVP;
uniform mat4 unity_MatrixV;
uniform mat4 unity_MatrixInvV;
uniform mat4 unity_MatrixP;

#define UNITY_MATRIX_P glstate_matrix_projection
#define UNITY_MATRIX_V unity_MatrixV
#define UNITY_MATRIX_I_V unity_MatrixInvV
#define UNITY_MATRIX_VP unity_MatrixVP
#define UNITY_MATRIX_M unity_ObjectToWorld

#define UNITY_MATRIX_MVP mul(unity_MatrixVP, unity_ObjectToWorld)
#define UNITY_MATRIX_MV mul(unity_MatrixV, unity_ObjectToWorld)
#define UNITY_MATRIX_T_MV transpose(UNITY_MATRIX_MV)
#define UNITY_MATRIX_IT_MV transpose(mul(unity_WorldToObject, unity_MatrixInvV))

#define UNITY_LIGHTMODEL_AMBIENT (glstate_lightmodel_ambient * 2)

// Time values from Unity
uniform vec4 _Time;
uniform vec4 _SinTime;
uniform vec4 _CosTime;

// x = 1 or -1 (-1 if projection is flipped)
// y = near plane
// z = far plane
// w = 1/far plane
uniform vec4 _ProjectionParams;

// x = width
// y = height
// z = 1 + 1.0/width
// w = 1 + 1.0/height
uniform vec4 _ScreenParams;

uniform vec3 _WorldSpaceCameraPos;
uniform vec4 _WorldSpaceLightPos0;

uniform vec4 _LightPositionRange; // xyz = pos, w = 1/range

#endif