#ifndef UNITY_LIGHTING_COMMON_INCLUDED
#define UNITY_LIGHTING_COMMON_INCLUDED

#include "UnityShaderVariables.glsl"

fixed4 _LightColor0;
fixed4 _SpecColor;

struct UnityLight
{
    fixed3 color;
    fixed3 dir;
};

struct UnityIndirect
{
    fixed3 diffuse;
    fixed3 specular;
};

struct UnityGI
{
    UnityLight light;
    UnityIndirect indirect;
};

#endif
