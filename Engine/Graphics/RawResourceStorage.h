#pragma once

#include "Core.h"

namespace tb
{
    class Shader;
    class GeometryBuffer;
    class TextureResource;

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


    private:
        std::unordered_map<std::string, Shader*> _shaders;
        std::unordered_map<std::string, GeometryBuffer*> _geoemtryBuffers;
        std::unordered_map<std::string, TextureResource*> _textures;
    };
} // namespace tb