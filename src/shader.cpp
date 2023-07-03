#include "shader.h"

#include <d3dcompiler.h>
#ifdef _MSC_VER
#pragma comment(lib, "d3dcompiler")
#endif

Shader* createShader(_In_ ShaderDesc _desc)
{
    UINT compilationFlags = 0u;

#ifdef DEBUG_
    compilationFlags |= D3DCOMPILE_DEBUG;
    compilationFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // DEBUG_

    ID3DBlob* pShader = nullptr;
    ID3DBlob* pErrors = nullptr;

    if (FAILED(D3DCompileFromFile(_desc.path,
                                  nullptr,
                                  nullptr,
                                  _desc.entry,
                                  _desc.target,
                                  compilationFlags,
                                  0u,
                                  &pShader,
                                  &pErrors)))
    {
        const char* errorMessage = (const char*)pErrors->GetBufferPointer();
        printf("%s\n", errorMessage);

        pErrors->Release();
        pErrors = nullptr;

        assert(!"Shader compilation failed!");
    }

    return (Shader*)pShader;
}

void releaseShader(_Inout_ Shader*& _pShader)
{
    DX_SAFE_RELEASE(_pShader);
}
