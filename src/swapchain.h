#pragma once

struct SwapChainDesc
{
    uint32_t width = 0u;
    uint32_t height = 0u;
    uint32_t bufferCount = 2u;
};

struct SwapChain
{
    IDXGISwapChain3* pSwapChain = nullptr;
    ID3D12DescriptorHeap* pRenderTargetHeap = nullptr;
    std::vector<ID3D12Resource*> renderTargets{};
    glm::uvec2 extent{};
};

SwapChain createSwapChain(_In_ xwin::Window& _rWindow,
                          _In_ Device& _rDevice,
                          _In_ SwapChainDesc _desc);

void resizeSwapChain(_In_ xwin::Window& _rWindow,
                     _In_ Device& _rDevice,
                     _Inout_ SwapChain& _rOldSwapChain,
                     _In_ SwapChainDesc _desc);

void releaseSwapChain(_Inout_ SwapChain& _rSwapChain);
