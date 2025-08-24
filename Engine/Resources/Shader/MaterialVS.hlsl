//cbuffer MeshConstants : register(b4)
//{
//    matrix WorldMatrix;
//};

//cbuffer GlobalConstants : register(b1)
//{
//    // float4x4
//    matrix ViewProjMatrix;
//    float3 CameraPosition;
//    float Time;
//};

//cbuffer MaterialConstants : register(b2)
//{
//    float4 Diffuse;
//    float4 Specular;
//    float4 Ambient;
//    float4 Emissive;
//};

cbuffer TempConstants : register(b0)
{
    matrix WorldMatrix;

    matrix ViewProjMatrix;
    float3 CameraPosition;
    float Time;

    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
    float4 Emissive;
};

//Texture2D BaseTexture : register(t0);
//SamplerState MaterialSampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    //float4 color : COLOR;
    //float3 normal : NORMAL;
    //float3 tangent : TANGENT;
    //float3 bitangent : BITANGENT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    //float4 color : COLOR;
    //float3 worldPos : TEXCOORD1;
    //float3 normal : NORMAL;
    //float3 tangent : TANGENT;
    //float3 bitangent : BITANGENT;
};


VSOutput main(VSInput input)
{
    VSOutput output;

    // 월드 공간 위치 계산
    float4 worldPos = mul(input.position, WorldMatrix);
    //output.worldPos = worldPos.xyz;

    // UV 좌표
    output.uv = input.uv;

    //// 최종 위치 (ViewProj 변환)
    output.position = mul(worldPos, ViewProjMatrix);

    //// 노말, 탄젠트, 바이탄젠트를 월드 공간으로 변환
    //output.normal = normalize(mul(input.normal, (float3x3) WorldMatrix));
    //output.tangent = normalize(mul(input.tangent, (float3x3) WorldMatrix));
    //output.bitangent = normalize(mul(input.bitangent, (float3x3) WorldMatrix));

    return output;
}