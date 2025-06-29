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
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};

VSOutput main(VSInput vsInput)
{
    VSInput output;
    output.pos = mul(vsInput.pos, WorldMatrix);
    return output;
}
