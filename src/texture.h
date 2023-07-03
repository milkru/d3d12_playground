#pragma once

struct DepthStencil
{
    ID3D12Resource* pResource = nullptr;
    ID3D12DescriptorHeap* pHeap = nullptr;
};

DepthStencil createDepthStencil(_In_ Device& _rDevice, _In_ glm::uvec2 _size);

void releaseDepthStencil(_Inout_ DepthStencil& _rDepthStencil);
