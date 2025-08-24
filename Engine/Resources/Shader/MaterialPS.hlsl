//cbuffer GlobalConstants : register(b1)
//{
//    float4x4 ViewProjMatrix;
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

//cbuffer TempConstants : register(b0)
//{
//    matrix WorldMatrix;

//    matrix ViewProjMatrix;
//    float3 CameraPosition;
//    float Time;

//    float4 Diffuse;
//    float4 Specular;
//    float4 Ambient;
//    float4 Emissive;
//};

//Texture2D DiffuseTexture : register(t1);
//Texture2D SpecularTexture : register(t2);
//Texture2D AmbientTexture : register(t3);
//Texture2D EmissiveTexture : register(t4);

Texture2D BaseTexture : register(t0);
SamplerState MaterialSampler : register(s0);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
    //float3 worldPos : TEXCOORD1;
    //float3 normal : NORMAL;
    //float3 tangent : TANGENT;
    //float3 bitangent : BITANGENT;
};

// 기본적인 Blinn-Phong 조명 계산
//float3 CalculateBlinnPhong(float3 worldPos, float3 normal, float3 viewDir, float3 lightDir, float3 lightColor)
//{
//    // 디퓨즈 라이팅
//    float NdotL = max(0.0, dot(normal, lightDir));
//    float3 diffuse = Diffuse.rgb * lightColor * NdotL;
//    float SpecularPower = 32.0f;

//    // 스펙큘러 라이팅 (Blinn-Phong)
//    float3 halfDir = normalize(lightDir + viewDir);
//    float NdotH = max(0.0, dot(normal, halfDir));
//    float specular = pow(NdotH, SpecularPower);
//    float3 specularColor = Specular.rgb * lightColor * specular;

//    // 앰비언트 라이팅
//    float3 ambient = Ambient.rgb * lightColor * 0.1;

//    return ambient + diffuse + specularColor;
//}

// 표준 Material Pixel Shader
float4 main(VSOutput input) : SV_TARGET
{
    // 텍스처에서 Material 속성 샘플링
    //float4 diffuseTexColor = DiffuseTexture.Sample(MaterialSampler, input.uv);
    //float4 specularTexColor = SpecularTexture.Sample(MaterialSampler, input.uv);
    //float4 emissiveTexColor = EmissiveTexture.Sample(MaterialSampler, input.uv);

    //// 최종 Material 색상 계산
    //float4 finalDiffuse = Diffuse * diffuseTexColor;
    //float4 finalSpecular = Specular * specularTexColor;
    //float4 finalEmissive = Emissive * emissiveTexColor;

    //// 노말 계산
    //float3 normal = normalize(input.normal);

    //// 뷰 방향 계산
    //float3 viewDir = normalize(CameraPosition - input.worldPos);

    //// 임시 조명 (Light 시스템 구현 전)
    //float3 lightDir = normalize(float3(1.0, 1.0, -1.0));
    //float3 lightColor = float3(1.0, 1.0, 1.0);

    //// 조명 계산
    //float3 litColor = CalculateBlinnPhong(input.worldPos, normal, viewDir, lightDir, lightColor);

    //// 발광 색상 추가
    //// litColor += finalEmissive.rgb;


    //// 최종 색상 출력
    //float Opacity = 0.5f;
    //float4 finalColor = float4(litColor, finalDiffuse.a * Opacity);
    return BaseTexture.Sample(MaterialSampler, input.uv);

    //return float4(1.f, 1.f, 1.f, 1.f);
}