template <typename Element>
static Buffer createBuffer(_In_ Device& _rDevice,
                           _In_ uint32_t _size,
                           _In_opt_ Element* _pInitialData = nullptr)
{
    uint32_t stride = sizeof(Element);
    Buffer buffer = createBuffer(_rDevice, _size * stride, stride);

    if (_pInitialData)
    {
        uploadBufferData(buffer, _pInitialData);
    }

    return buffer;
}

template <typename Type>
ConstantBuffer createConstantBuffer(_In_ Device& _rDevice)
{
    uint32_t sizeInBytes = (sizeof(Type) + 255) & ~255;
    Buffer buffer = createBuffer(_rDevice, sizeInBytes, sizeInBytes);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = 1u,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0u};

    ID3D12DescriptorHeap* pHeap = nullptr;
    DX_CALL(_rDevice.pDevice->CreateDescriptorHeap(&heapDesc,
                                                   IID_PPV_ARGS(&pHeap)));

    D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {
        viewDesc.BufferLocation = buffer.pResource->GetGPUVirtualAddress(),
        viewDesc.SizeInBytes = sizeInBytes};

    D3D12_CPU_DESCRIPTOR_HANDLE viewHandle =
        pHeap->GetCPUDescriptorHandleForHeapStart();
    _rDevice.pDevice->CreateConstantBufferView(&viewDesc, viewHandle);

    ConstantBuffer constantBuffer = {buffer};
    constantBuffer.pHeap = pHeap;
    return constantBuffer;
}
