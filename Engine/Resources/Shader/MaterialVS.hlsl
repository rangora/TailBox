cbuffer MaterialConstants : register(b2)
{
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
    float4 Emissive;
};

Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D AmbientTexture : register(t2);
Texture2D EmissiveTexture : register(t3);

SamplerState MaterialSampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

// 기존 상수 버퍼들 (World, ViewProj)
cbuffer MeshConstants : register(b0)
{
    matrix WorldMatrix;
};

cbuffer GlobalConstants : register(b1)
{
    float4x4 ViewProjMatrix;
    float3 CameraPosition;
    float Time;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    // 월드 공간 위치 계산
    float4 worldPos = mul(input.position, WorldMatrix);
    output.worldPos = worldPos.xyz;

    // 최종 위치 (ViewProj 변환)
    output.position = mul(worldPos, ViewProjMatrix);

    // UV 좌표
    output.uv = input.uv;

    // 노말, 탄젠트, 바이탄젠트를 월드 공간으로 변환
    output.normal = normalize(mul(input.normal, (float3x3) WorldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) WorldMatrix));
    output.bitangent = normalize(mul(input.bitangent, (float3x3) WorldMatrix));

    return output;
}