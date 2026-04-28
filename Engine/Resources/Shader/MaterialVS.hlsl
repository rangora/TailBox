cbuffer MeshConstants : register(b0)
{
    matrix WorldMatrix;
};

cbuffer GlobalConstants : register(b1)
{
    matrix ViewProjMatrix;
    float3 CameraPosition;
    float Time;
};

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
};


VSOutput main(VSInput input)
{
    VSOutput output;

    float4 worldPos = mul(input.position, WorldMatrix);
    output.worldPos = worldPos.xyz;
    output.position = mul(worldPos, ViewProjMatrix);

    output.uv = input.uv;

    output.normal = normalize(mul(input.normal, (float3x3) WorldMatrix));

    return output;
}
