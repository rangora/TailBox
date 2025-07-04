struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv0 : TEXCOORD0;
};

Texture2D texture0 : register(t0);

SamplerState sample0 : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
    return texture0.Sample(sample0, input.uv0);
    //return float4(1.f, 1.f, 0.f, 1.f);
}