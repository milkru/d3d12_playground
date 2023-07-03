#include "device.h"
#include "buffer.h"
#include "mesh.h"

#include <fast_obj.h>
#include <meshoptimizer.h>

Mesh createMesh(_In_ Device& _rDevice, _In_ const char* _pPath)
{
    fastObjMesh* pMesh = fast_obj_read(_pPath);
    assert(pMesh != nullptr);

    std::vector<Vertex> vertices;
    vertices.reserve(pMesh->index_count);

    for (int32_t index = 0; index < pMesh->index_count; ++index)
    {
        Vertex vertex{};
        fastObjIndex vertexIndex = pMesh->indices[index];

        vertex.position[0] = pMesh->positions[3 * vertexIndex.p + 0];
        vertex.position[1] = pMesh->positions[3 * vertexIndex.p + 1];
        vertex.position[2] = pMesh->positions[3 * vertexIndex.p + 2];

        vertex.normal[0] = pMesh->normals[3 * vertexIndex.n + 0];
        vertex.normal[1] = pMesh->normals[3 * vertexIndex.n + 1];
        vertex.normal[2] = pMesh->normals[3 * vertexIndex.n + 2];

        vertex.texCoord[0] = pMesh->texcoords[2 * vertexIndex.t + 0];
        vertex.texCoord[1] = pMesh->texcoords[2 * vertexIndex.t + 1];

        vertices.push_back(vertex);
    }

    std::vector<uint32_t> remapTable(pMesh->index_count);
    size_t vertexCount = meshopt_generateVertexRemap(remapTable.data(),
                                                     nullptr,
                                                     pMesh->index_count,
                                                     vertices.data(),
                                                     vertices.size(),
                                                     sizeof(Vertex));

    vertices.resize(vertexCount);
    std::vector<uint32_t> indices(pMesh->index_count);

    meshopt_remapVertexBuffer(vertices.data(),
                              vertices.data(),
                              indices.size(),
                              sizeof(Vertex),
                              remapTable.data());

    meshopt_remapIndexBuffer(
        indices.data(), nullptr, indices.size(), remapTable.data());

    meshopt_optimizeVertexCache(
        indices.data(), indices.data(), indices.size(), vertices.size());

    meshopt_optimizeOverdraw(indices.data(),
                             indices.data(),
                             indices.size(),
                             &vertices[0].position[0],
                             vertices.size(),
                             sizeof(Vertex),
                             /*threshold*/ 1.01f);

    meshopt_optimizeVertexFetch(vertices.data(),
                                indices.data(),
                                indices.size(),
                                vertices.data(),
                                vertices.size(),
                                sizeof(Vertex));

    return {.indexCount = uint32_t(indices.size()),
            .vertexBuffer =
                createBuffer(_rDevice, vertices.size(), vertices.data()),
            .indexBuffer =
                createBuffer(_rDevice, indices.size(), indices.data())};
}

void releaseMesh(_Inout_ Mesh& _rMesh)
{
    releaseBuffer(_rMesh.indexBuffer);
    releaseBuffer(_rMesh.indexBuffer);

    _rMesh = {};
}
