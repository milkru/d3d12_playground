#pragma once

struct Pipeline
{
    ID3D12RootSignature* pRootSignature = nullptr;
    ID3D12PipelineState* pPipelineState = nullptr;
};

Pipeline createPipeline(_In_ Device& _rDevice,
                        _In_ Shader* _pVertexShader,
                        _In_ Shader* _pPixelShader);

void releasePipeline(_Inout_ Pipeline& _rPipeline);
