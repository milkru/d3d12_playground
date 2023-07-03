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

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 texCoord : TEXCOORD;
};

struct VertexOutput
{
    float3 normal : NORMAL;
    float3 texCoord : TEXCOORD;
    float3 worldPosition : WORLD_POS;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;
    output.normal = mul(vertexInput.normal, (float3x3)model);
    output.texCoord = vertexInput.texCoord;
    output.worldPosition = mul(float4(vertexInput.position, 1.0f), model);
    output.position = mul(float4(vertexInput.position, 1.0f),
                          mul(model, mul(view, projection)));

    return output;
}
