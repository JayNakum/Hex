#pragma once

#include "core/common.h"

#include "maths/maths.h"

namespace engine
{
    // Basic shapes geometry generator, ported from Frank Luna's DX12 book.
    class GeometryGenerator
    {
    public:
        struct Vertex
        {
            Vertex() = default;

            Vertex(const vec3f& p, const vec3f& n, const vec3f& t, const vec2f& uv)
                : position(p), normal(n), tangentU(t), textureCoord(uv)
            {
            }

            Vertex(
                float px, float py, float pz,
                float nx, float ny, float nz,
                float tx, float ty, float tz,
                float u, float v)
                : position(px, py, pz), normal(nx, ny, nz), tangentU(tx, ty, tz), textureCoord(u, v)
            {
            }

            vec3f position;
            vec3f normal;
            vec3f tangentU;
            vec2f textureCoord;
        };

        struct MeshData
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices32;

            inline std::vector<uint16_t>& getIndices16()
            {
                if (mIndices16.empty())
                {
                    mIndices16.resize(indices32.size());
                    for (size_t i = 0; i < indices32.size(); ++i)
                    {
                        mIndices16[i] = static_cast<uint16_t>(indices32[i]);
                    }
                }

                return mIndices16;
            }

            inline size_t getVBByteSize() const
            {
                return vertices.size() * sizeof(GeometryGenerator::Vertex);
            }

            inline size_t getIBByteSize() const
            {
                return indices32.size() * sizeof(uint32_t);
            }

            inline size_t getIB16ByteSize() const
            {
                return indices32.size() * sizeof(uint16_t);
            }

        private:
            std::vector<uint16_t> mIndices16;
        };

        MeshData createBox(float width, float height, float depth, uint32_t numSubdivisions);

        MeshData createSphere(float radius, uint32_t sliceCount, uint32_t stackCount);

        MeshData createGeosphere(float radius, uint32_t numSubdivisions);

        MeshData createCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

        MeshData createGrid(float width, float depth, uint32_t m, uint32_t n);

        MeshData createQuad(float x = -0.5f, float y = 0.5f, float w = 1.0f, float h = 1.0f, float depth = 0.0f);

    private:
        void subdivide(MeshData& meshData);
        Vertex midPoint(const Vertex& v0, const Vertex& v1);
        void buildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
        void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, MeshData& meshData);
    };
}
