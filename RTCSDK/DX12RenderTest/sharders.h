#pragma once
#define GET_STR(x) #x
const char* PixelSharderStr = GET_STR(
struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_textureY : register(t0);
Texture2D g_textureU : register(t1);
Texture2D g_textureV : register(t2);
SamplerState g_sampler : register(s0);

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 y = g_textureY.Sample(g_sampler, input.uv);
    float c = y[0];
    float4 u = g_textureU.Sample(g_sampler, input.uv);
    float d = u[0] - 0.5;
    float4 v = g_textureV.Sample(g_sampler, input.uv);
    float e = v[0] - 0.5;
    float4 rgba = float4(c + 1.370705 * e, c - 0.337633 * d - 0.698001 * e, c + 1.732446 * d, 0);
    return rgba;
}

);
const char* VertexSharderStr = GET_STR(
struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};
PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;
    result.position = position;
    result.uv = uv;
    return result;
}
);