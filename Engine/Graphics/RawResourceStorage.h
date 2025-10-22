#pragma once

#include "Core.h"

namespace tb
{
    class Shader;
    class GeometryBuffer;
    class TextureResource;
    class Mesh;

    struct RawResourceStorage
    {
        RawResourceStorage();
        ~RawResourceStorage();

        void Initialize();
        void Release();

        GeometryBuffer* GetGeometryBuffer(const std::string& name);
        Shader* GetShader(const std::string& name);
        TextureResource* GetTexture(const std::string& name);

        void AddShader(const std::string& key, Shader* shader);
        void AddTexture(const std::string& key, TextureResource* texture);

        class StaticMesh* _cubeMesh = nullptr;

    private:
        std::unordered_map<std::string, Shader*> _shaders;
        std::unordered_map<std::string, GeometryBuffer*> _geoemtryBuffers;
        std::unordered_map<std::string, TextureResource*> _textures;
        std::unordered_map<std::string, Mesh*> _meshes;
    };
} // namespace tb