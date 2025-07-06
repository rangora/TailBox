cbuffer MeshConstants : register(b0)
{
    matrix WorldMatrix;
};

cbuffer GlobalConstants : register(b1)
{
    float4x4 ViewProjMatrix;
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv0 : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv0 : TEXCOORD0;
};

VSOutput main(VSInput vsInput)
{
    VSOutput output;
    output.pos = mul(vsInput.pos, WorldMatrix);
    output.uv0 = vsInput.uv0;
    return output;
}
