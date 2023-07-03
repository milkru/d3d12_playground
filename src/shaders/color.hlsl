cbuffer FrameConstants : register(b0)
{
    row_major float4x4 projection;
    row_major float4x4 model;
    row_major float4x4 view;
    float3 ambientColor;
    float3 sunColor;
    float3 cameraPosition;
    float3 sunDirection;
    float3 albedoColor;
    float3 specularColor;
    float padding;
    float sunIntensity;
    float smoothness;
};

struct PixelInput
{
    float3 normal : NORMAL;
    float3 texCoord : TEXCOORD;
    float3 worldPosition : WORLD_POS;
};

float4 main(PixelInput pixelInput) : SV_Target0
{
    float3 ambientComponent = 0.15f * ambientColor;

    float3 normal = normalize(pixelInput.normal);
    float3 lightDirection = normalize(sunDirection);
    float3 diffuseColor = sunIntensity * sunColor * albedoColor;
    float diffuseFactor = max(dot(-lightDirection, normal), 0.0);
    float3 diffuseComponent = diffuseColor * diffuseFactor;

    float3 viewDirection = normalize(cameraPosition - pixelInput.worldPosition);
    float3 reflectDirection = reflect(lightDirection, normal);
    float specularity = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    float3 specularComponent = smoothness * specularity * specularColor;

    float3 finalColor = ambientComponent + diffuseComponent + specularComponent;
    return float4(finalColor, 1.0);
}
