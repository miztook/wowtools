#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED

#define CBUFFER_START(name) cbuffer name {
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

#endif