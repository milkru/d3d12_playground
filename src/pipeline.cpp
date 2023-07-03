#include "device.h"
#include "shader.h"
#include "pipeline.h"

static ID3D12RootSignature* createRootSignature(_In_ ID3D12Device* _pDevice)
{
    D3D12_DESCRIPTOR_RANGE1 descriptorRange = {
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        .NumDescriptors = 1u,
        .BaseShaderRegister = 0u,
        .RegisterSpace = 0u,
        .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
        .OffsetInDescriptorsFromTableStart = 0u};

    D3D12_ROOT_PARAMETER1 rootParameters[] = {
        {.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
         .DescriptorTable = {.NumDescriptorRanges = 1,
                             .pDescriptorRanges = &descriptorRange},
         .ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX},
        {.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
         .DescriptorTable = {.NumDescriptorRanges = 1,
                             .pDescriptorRanges = &descriptorRange},
         .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL}};

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {
        .Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
        .Desc_1_1 = {
            .NumParameters = _countof(rootParameters),
            .pParameters = rootParameters,
            .NumStaticSamplers = 0u,
            .pStaticSamplers = nullptr,
            .Flags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT}};

    ID3DBlob* signature;
    ID3DBlob* error;
    if (FAILED(D3D12SerializeVersionedRootSignature(
            &rootSignatureDesc, &signature, &error)))
    {
        printf("RootSignature creation failed [%s]",
               (const char*)error->GetBufferPointer());
        DX_SAFE_RELEASE(error);
    }

    ID3D12RootSignature* rootSignature = nullptr;
    DX_CALL(_pDevice->CreateRootSignature(0u,
                                          signature->GetBufferPointer(),
                                          signature->GetBufferSize(),
                                          IID_PPV_ARGS(&rootSignature)));

    DX_SAFE_RELEASE(signature);
    return rootSignature;
}

static ID3D12PipelineState*
    createPipelineState(_In_ ID3D12Device* _pDevice,
                        _In_ ID3D12RootSignature* _pRootSignature,
                        _In_ ID3DBlob* _pVertexShader,
                        _In_ ID3DBlob* _pPixelShader)
{
    D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {
        .BlendEnable = FALSE,
        .LogicOpEnable = FALSE,
        .SrcBlend = D3D12_BLEND_ONE,
        .DestBlend = D3D12_BLEND_ZERO,
        .BlendOp = D3D12_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D12_BLEND_ONE,
        .DestBlendAlpha = D3D12_BLEND_ZERO,
        .BlendOpAlpha = D3D12_BLEND_OP_ADD,
        .LogicOp = D3D12_LOGIC_OP_NOOP,
        .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL};

    D3D12_BLEND_DESC blendDesc = {.AlphaToCoverageEnable = FALSE,
                                  .IndependentBlendEnable = FALSE,
                                  .RenderTarget = {renderTargetBlendDesc}};

    D3D12_RASTERIZER_DESC rasterizerDesc = {
        .FillMode = D3D12_FILL_MODE_SOLID,
        .CullMode = D3D12_CULL_MODE_BACK,
        .FrontCounterClockwise = FALSE,
        .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
        .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
        .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        .DepthClipEnable = TRUE,
        .MultisampleEnable = FALSE,
        .AntialiasedLineEnable = FALSE,
        .ForcedSampleCount = 0u,
        .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF};

    D3D12_INPUT_ELEMENT_DESC vertexInputLayout[] = {
        {.SemanticName = "POSITION",
         .SemanticIndex = 0u,
         .Format = DXGI_FORMAT_R32G32B32_FLOAT,
         .InputSlot = 0u,
         .AlignedByteOffset = 0u,
         .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         .InstanceDataStepRate = 0u},
        {.SemanticName = "NORMAL",
         .SemanticIndex = 0u,
         .Format = DXGI_FORMAT_R32G32B32_FLOAT,
         .InputSlot = 0u,
         .AlignedByteOffset = 12u,
         .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         .InstanceDataStepRate = 0u},
        {.SemanticName = "TEXCOORD",
         .SemanticIndex = 0u,
         .Format = DXGI_FORMAT_R32G32_FLOAT,
         .InputSlot = 0u,
         .AlignedByteOffset = 24u,
         .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         .InstanceDataStepRate = 0u}};

    D3D12_DEPTH_STENCILOP_DESC stencilOp;
    stencilOp.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    stencilOp.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    stencilOp.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    stencilOp.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = _pRootSignature;
    pipelineStateDesc.VS = {.pShaderBytecode =
                                _pVertexShader->GetBufferPointer(),
                            .BytecodeLength = _pVertexShader->GetBufferSize()};
    pipelineStateDesc.PS = {.pShaderBytecode =
                                _pPixelShader->GetBufferPointer(),
                            .BytecodeLength = _pPixelShader->GetBufferSize()};
    pipelineStateDesc.BlendState = blendDesc;
    pipelineStateDesc.SampleMask = UINT_MAX;
    pipelineStateDesc.RasterizerState = rasterizerDesc;
    pipelineStateDesc.DepthStencilState.DepthEnable = TRUE;
    pipelineStateDesc.DepthStencilState.DepthWriteMask =
        D3D12_DEPTH_WRITE_MASK_ALL;
    pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
    pipelineStateDesc.DepthStencilState.StencilReadMask = 0xff;
    pipelineStateDesc.DepthStencilState.StencilWriteMask = 0xff;
    pipelineStateDesc.DepthStencilState.FrontFace = stencilOp;
    pipelineStateDesc.DepthStencilState.BackFace = stencilOp;
    pipelineStateDesc.DepthStencilState.DepthFunc =
        D3D12_COMPARISON_FUNC_LESS_EQUAL;
    pipelineStateDesc.InputLayout = {vertexInputLayout,
                                     _countof(vertexInputLayout)};
    pipelineStateDesc.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.NumRenderTargets = 1u;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineStateDesc.SampleDesc = {.Count = 1u, .Quality = 0u};
    pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ID3D12PipelineState* pPipelineState = nullptr;
    _pDevice->CreateGraphicsPipelineState(&pipelineStateDesc,
                                          IID_PPV_ARGS(&pPipelineState));

    return pPipelineState;
}

Pipeline createPipeline(_In_ Device& _rDevice,
                        _In_ Shader* _pVertexShader,
                        _In_ Shader* _pPixelShader)
{
    ID3D12RootSignature* pRootSignature = createRootSignature(_rDevice.pDevice);
    ID3D12PipelineState* pPipelineState = createPipelineState(
        _rDevice.pDevice, pRootSignature, _pVertexShader, _pPixelShader);

    return {.pRootSignature = pRootSignature, .pPipelineState = pPipelineState};
}

void releasePipeline(_Inout_ Pipeline& _rPipeline)
{
    DX_SAFE_RELEASE(_rPipeline.pPipelineState);
    DX_SAFE_RELEASE(_rPipeline.pRootSignature);

    _rPipeline = {};
}
