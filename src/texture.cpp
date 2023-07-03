#include "device.h"
#include "texture.h"

DepthStencil createDepthStencil(_In_ Device& _rDevice, _In_ glm::uvec2 _size)
{
    D3D12_HEAP_PROPERTIES heapProperties = {
        .Type = D3D12_HEAP_TYPE_DEFAULT,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1u,
        .VisibleNodeMask = 1u};

    D3D12_RESOURCE_DESC resourceDesc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = 0ull,
        .Width = _size.x,
        .Height = _size.y,
        .DepthOrArraySize = 1u,
        .MipLevels = 1u,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .SampleDesc = {.Count = 1u, .Quality = 0u},
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL};

    D3D12_CLEAR_VALUE optimizedClearValue = {
        .Format = DXGI_FORMAT_D32_FLOAT,
        .DepthStencil{.Depth = 1.0f, .Stencil = 0u}};

    ID3D12Resource* pDepthStencil = nullptr;
    DX_CALL(_rDevice.pDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,
        IID_PPV_ARGS(&pDepthStencil)));

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        .NumDescriptors = 1,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0u};

    ID3D12DescriptorHeap* pHeap = nullptr;
    DX_CALL(_rDevice.pDevice->CreateDescriptorHeap(&heapDesc,
                                                   IID_PPV_ARGS(&pHeap)));

    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle(
        pHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {
        .Format = DXGI_FORMAT_D32_FLOAT,
        .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
        .Flags = D3D12_DSV_FLAG_NONE,
        .Texture2D = {.MipSlice = 0u}};

    _rDevice.pDevice->CreateDepthStencilView(
        pDepthStencil, &depthStencilDesc, depthStencilHandle);

    return {.pResource = pDepthStencil, .pHeap = pHeap};
}

void releaseDepthStencil(_Inout_ DepthStencil& _rDepthStencil)
{
    DX_SAFE_RELEASE(_rDepthStencil.pHeap);
    DX_SAFE_RELEASE(_rDepthStencil.pResource);

    _rDepthStencil = {};
}
