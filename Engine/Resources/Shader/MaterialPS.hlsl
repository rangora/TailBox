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

Texture2D BaseTexture : register(t0);
SamplerState MaterialSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
};

float4 main(VSOutput input) : SV_TARGET
{
    float lightDiffuse = 0.7f;
    float lightSpecular = 0.5f;
    float lightShininess = 32.f;
    float lightAmbient = 0.5f;

    // Ambient
    float3 ambient = lightAmbient * BaseTexture.Sample(MaterialSampler, input.uv).rgb;

    // Diffuse
    float3 normal = normalize(input.normal);
    float3 lightDir = normalize(float3(0.f, -1.f, 0.f));
    float diff = max(dot(normal, lightDir), 0.f);
    float3 diffuse = lightDiffuse * diff * BaseTexture.Sample(MaterialSampler, input.uv).rgb;

    // Specular
    float3 viewDir = normalize(CameraPosition - input.worldPos);
    float3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.f), lightShininess);
    float3 specular = lightSpecular * spec * BaseTexture.Sample(MaterialSampler, input.uv).rgb;

    return float4(ambient + diffuse + spec, 1.f);
}