cbuffer goemetryParams : register(b0)
{
    matrix wvpMat;
};

struct VS_IN
{
    float4 _pos : POSITION;
    float _color : COLOR;
};

struct VS_OUT
{
    float4 _pos : SV_Position;
    float4 _color : COLOR;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    output._pos = mul(input._pos, wvpMat);
    output._color = input._color;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return input._color;
}