#include "device.h"
#include "swapchain.h"

static ID3D12DescriptorHeap*
    createRenderTargetDescriptorHeap(_In_ ID3D12Device* _pDevice,
                                     _In_ UINT _renderTargetCount)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = _renderTargetCount,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0u};

    ID3D12DescriptorHeap* pHeap = nullptr;
    DX_CALL(_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&pHeap)));

    return pHeap;
}

static std::vector<ID3D12Resource*>
    createRenderTargets(_In_ ID3D12Device* _pDevice,
                        _In_ IDXGISwapChain3* _pSwapChain,
                        _In_ ID3D12DescriptorHeap* _pHeap,
                        _In_ UINT _renderTargetCount)
{
    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(
        _pHeap->GetCPUDescriptorHandleForHeapStart());
    UINT renderTargetDescriptorSize =
        _pDevice->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    std::vector<ID3D12Resource*> renderTargets(_renderTargetCount);
    for (UINT renderTargetIndex = 0u; renderTargetIndex < _renderTargetCount;
         ++renderTargetIndex)
    {
        DX_CALL(_pSwapChain->GetBuffer(
            renderTargetIndex,
            IID_PPV_ARGS(&renderTargets[renderTargetIndex])));
        _pDevice->CreateRenderTargetView(
            renderTargets[renderTargetIndex], nullptr, renderTargetViewHandle);
        renderTargetViewHandle.ptr += (1 * renderTargetDescriptorSize);
    }

    return renderTargets;
}

static void releaseRenderTargets(_Inout_ SwapChain& _rSwapChain)
{
    for (ID3D12Resource* pRenderTarget : _rSwapChain.renderTargets)
    {
        DX_SAFE_RELEASE(pRenderTarget);
    }

    _rSwapChain.renderTargets.clear();
}

SwapChain createSwapChain(_In_ xwin::Window& _rWindow,
                          _In_ Device& _rDevice,
                          _In_ SwapChainDesc _desc)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        .Width = _desc.width,
        .Height = _desc.height,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = FALSE,
        .SampleDesc = {.Count = 1u, .Quality = 0u},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = _desc.bufferCount,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = 0u};

    IDXGISwapChain1* pNewSwapChain = xgfx::createSwapchain(
        &_rWindow, _rDevice.pFactory, _rDevice.pCommandQueue, &swapChainDesc);
    DX_CALL(pNewSwapChain->QueryInterface(__uuidof(IDXGISwapChain3),
                                          (void**)&pNewSwapChain));

    IDXGISwapChain3* pSwapChain = (IDXGISwapChain3*)pNewSwapChain;

    ID3D12DescriptorHeap* pRenderTargetHeap =
        createRenderTargetDescriptorHeap(_rDevice.pDevice, _desc.bufferCount);

    std::vector<ID3D12Resource*> renderTargets = createRenderTargets(
        _rDevice.pDevice, pSwapChain, pRenderTargetHeap, _desc.bufferCount);

    return {.pSwapChain = pSwapChain,
            .pRenderTargetHeap = pRenderTargetHeap,
            .renderTargets = renderTargets,
            .extent = {_desc.width, _desc.height}};
}

void resizeSwapChain(_In_ xwin::Window& _rWindow,
                     _In_ Device& _rDevice,
                     _Inout_ SwapChain& _rSwapChain,
                     _In_ SwapChainDesc _desc)
{
    releaseRenderTargets(_rSwapChain);
    DX_SAFE_RELEASE(_rSwapChain.pRenderTargetHeap);

    IDXGISwapChain3* pSwapChain = _rSwapChain.pSwapChain;

    DX_CALL(pSwapChain->ResizeBuffers(_desc.bufferCount,
                                      _desc.width,
                                      _desc.height,
                                      DXGI_FORMAT_R8G8B8A8_UNORM,
                                      0u));

    ID3D12DescriptorHeap* pRenderTargetHeap =
        createRenderTargetDescriptorHeap(_rDevice.pDevice, _desc.bufferCount);
    std::vector<ID3D12Resource*> renderTargets = createRenderTargets(
        _rDevice.pDevice, pSwapChain, pRenderTargetHeap, _desc.bufferCount);

    _rSwapChain = {.pSwapChain = pSwapChain,
                   .pRenderTargetHeap = pRenderTargetHeap,
                   .renderTargets = renderTargets,
                   .extent = {_desc.width, _desc.height}};
}

void releaseSwapChain(_Inout_ SwapChain& _rSwapChain)
{
    releaseRenderTargets(_rSwapChain);

    DX_SAFE_RELEASE(_rSwapChain.pRenderTargetHeap);
    DX_SAFE_RELEASE(_rSwapChain.pSwapChain);

    _rSwapChain = {};
}
