//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_textureY : register(t0);
Texture2D g_textureUV : register(t1);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = position;
    result.uv = uv;

    return result;
}

float clip(float a)
{
    if (a > 1)
        return 1;
    else if (a < 0)
        return 0;
    else
        return a;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 y = g_textureY.Sample(g_sampler, input.uv);
    float c = y[0];
    float4 uv = g_textureUV.Sample(g_sampler, input.uv);
    float d = uv[0] - 0.5;
    float e = uv[1] - 0.5;
    float4 rgba = float4(c + 1.370705 * e, c - 0.337633 * d - 0.698001 * e, c + 1.732446 * d, 0);
    return rgba;
}
