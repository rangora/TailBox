struct VSOutput
{
    float4 pos : SV_POSITION;
};

float4 main(VSOutput input) : SV_TARGET
{
    return float4(1.f, 1.f, 0.f, 1.f);
}