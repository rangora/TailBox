cbuffer MaterialConstants : register(b2)
{
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
    float4 Emissive;
};

Texture2D DiffuseTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D AmbientTexture : register(t2);
Texture2D EmissiveTexture : register(t3);

SamplerState MaterialSampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

// 기존 상수 버퍼들 (World, ViewProj)
cbuffer MeshConstants : register(b0)
{
    matrix WorldMatrix;
};

cbuffer GlobalConstants : register(b1)
{
    float4x4 ViewProjMatrix;
    float3 CameraPosition;
    float Time;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    // 월드 공간 위치 계산
    float4 worldPos = mul(input.position, WorldMatrix);
    output.worldPos = worldPos.xyz;

    // 최종 위치 (ViewProj 변환)
    output.position = mul(worldPos, ViewProjMatrix);

    // UV 좌표
    output.uv = input.uv;

    // 노말, 탄젠트, 바이탄젠트를 월드 공간으로 변환
    output.normal = normalize(mul(input.normal, (float3x3) WorldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) WorldMatrix));
    output.bitangent = normalize(mul(input.bitangent, (float3x3) WorldMatrix));

    return output;
}

// 노말 맵 적용 함수
/*
float3 ApplyNormalMap(float2 uv, float3 normal, float3 tangent, float3 bitangent)
{
    // 노말 맵에서 노말 벡터 샘플링 (0~1 -> -1~1 변환)
    float3 normalMap = NormalTexture.Sample(MaterialSampler, uv).rgb * 2.0 - 1.0;
    normalMap.xy *= NormalStrength;

    // TBN 매트릭스 구성
    float3x3 tbnMatrix = float3x3(tangent, bitangent, normal);

    // 탄젠트 공간에서 월드 공간으로 변환
    return normalize(mul(normalMap, tbnMatrix));
}
*/

// 기본적인 Blinn-Phong 조명 계산 (Light 시스템 구현 전 임시)
float3 CalculateBlinnPhong(float3 worldPos, float3 normal, float3 viewDir, float3 lightDir, float3 lightColor)
{
    // 디퓨즈 라이팅
    float NdotL = max(0.0, dot(normal, lightDir));
    float3 diffuse = Diffuse.rgb * lightColor * NdotL;
    float SpecularPower = 1.f;

    // 스펙큘러 라이팅 (Blinn-Phong)
    float3 halfDir = normalize(lightDir + viewDir);
    float NdotH = max(0.0, dot(normal, halfDir));
    float specular = pow(NdotH, SpecularPower);
    float3 specularColor = Specular.rgb * lightColor * specular;

    // 앰비언트 라이팅
    float3 ambient = Ambient.rgb * lightColor * 0.1; // 임시로 0.1 스케일

    return ambient + diffuse + specularColor;
}

// Pixel Shader
float4 main(VSOutput input) : SV_TARGET
{
    // 텍스처에서 Material 속성 샘플링
    float4 diffuseTexColor = DiffuseTexture.Sample(MaterialSampler, input.uv);
    float4 specularTexColor = SpecularTexture.Sample(MaterialSampler, input.uv);
    float4 emissiveTexColor = EmissiveTexture.Sample(MaterialSampler, input.uv);

    // 최종 Material 색상 계산
    float4 finalDiffuse = Diffuse * diffuseTexColor;
    float4 finalSpecular = Specular * specularTexColor;
    float4 finalEmissive = Emissive * emissiveTexColor;

    // 노말 계산 (노말 맵이 있는 경우 적용)
    float3 normal = normalize(input.normal);
    // TODO: 노말 맵 체크 후 적용
    // if (hasNormalTexture) normal = ApplyNormalMap(input.uv, normal, input.tangent, input.bitangent);

    // 뷰 방향 계산
    float3 viewDir = normalize(CameraPosition - input.worldPos);

    // 임시 조명 (Light 시스템 구현 전)
    float3 lightDir = normalize(float3(1.0, 1.0, -1.0));
    float3 lightColor = float3(1.0, 1.0, 1.0);

    // 조명 계산
    float3 litColor = CalculateBlinnPhong(input.worldPos, normal, viewDir, lightDir, lightColor);

    // 발광 색상 추가
    litColor += finalEmissive.rgb;

    // 최종 색상 출력
    float Opacity = 1.f;
    float4 finalColor = float4(litColor, finalDiffuse.a * Opacity);

    return finalColor;
}

//// 간단한 언릿 셰이더 (텍스처만 출력)
//float4 PSUnlit(VSOutput input) : SV_TARGET
//{
//    float4 diffuseTexColor = DiffuseTexture.Sample(MaterialSampler, input.uv);
//    return Diffuse * diffuseTexColor;
//}