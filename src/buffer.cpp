#include "device.h"
#include "buffer.h"

Buffer createBuffer(_In_ Device& _rDevice,
                    _In_ uint32_t _sizeInBytes,
                    _In_ uint32_t _strideInBytes)
{
    // Currently using only host visible buffers
    D3D12_HEAP_PROPERTIES heapProperties = {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
        .CreationNodeMask = 1u,
        .VisibleNodeMask = 1u};

    D3D12_RESOURCE_DESC resourceDesc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0ull,
        .Width = _sizeInBytes,
        .Height = 1u,
        .DepthOrArraySize = 1u,
        .MipLevels = 1u,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {.Count = 1u, .Quality = 0u},
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE};

    ID3D12Resource* pBufferResource = nullptr;
    DX_CALL(_rDevice.pDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pBufferResource)));

    return {.pResource = pBufferResource,
            .gpuAddress = pBufferResource->GetGPUVirtualAddress(),
            .sizeInBytes = _sizeInBytes,
            .strideInBytes = _strideInBytes};
}

void uploadBufferData(_In_ Buffer& _rBuffer, _In_ void* _pData)
{
    // We don't need to read specified memory on CPU
    D3D12_RANGE readRange{.Begin = 0, .End = 0};
    void* pMappedMemory = nullptr;

    DX_CALL(_rBuffer.pResource->Map(0u, &readRange, &pMappedMemory));
    memcpy(pMappedMemory, _pData, _rBuffer.sizeInBytes);
    _rBuffer.pResource->Unmap(0u, nullptr);
}

void releaseBuffer(_Inout_ Buffer& _rBuffer)
{
    DX_SAFE_RELEASE(_rBuffer.pResource);

    _rBuffer = {};
}

void releaseConstantBuffer(_Inout_ ConstantBuffer& _rConstantBuffer)
{
    DX_SAFE_RELEASE(_rConstantBuffer.pHeap);
    DX_SAFE_RELEASE(_rConstantBuffer.pResource);

    _rConstantBuffer = {};
}
