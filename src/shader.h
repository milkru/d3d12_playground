#pragma once

struct ShaderDesc
{
    const wchar_t* path = L"";
    const char* target = "ps_5_0";
    const char* entry = "main";
};

typedef ID3DBlob Shader;

Shader* createShader(_In_ ShaderDesc _desc);

void releaseShader(_Inout_ Shader*& _pShader);
