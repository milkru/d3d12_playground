#pragma once

struct Vertex
{
    float position[3];
    float normal[3];
    float texCoord[2];
};

struct Mesh
{
    uint32_t indexCount;
    Buffer vertexBuffer;
    Buffer indexBuffer;
};

Mesh createMesh(_In_ Device& _rDevice, _In_ const char* _pPath);

void releaseMesh(_Inout_ Mesh& _rMesh);
