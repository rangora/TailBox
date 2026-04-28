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

cbuffer MaterialConstants : register(b2)
{
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
    float4 Emissive;
};
