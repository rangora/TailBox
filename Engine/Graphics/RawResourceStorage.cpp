#include "RawResourceStorage.h"
#include "GpuBuffer.h"
#include "Shader.h"
#include "TextureResource.h"
#include "Resources/BaseResource.h"

namespace tb
{
    RawResourceStorage::RawResourceStorage() = default;
    RawResourceStorage::~RawResourceStorage()
    {
        Release();
    }

    void RawResourceStorage::Initialize()
    {
        _geoemtryBuffers.reserve(100);

        _geoemtryBuffers.emplace("Box", br::CreateBoxBuffer());
        _geoemtryBuffers.emplace("Cube", br::CreateCubeBuffer());

        // tex
        for (TextureResource* texResource : br::CreateDefaultTextureResource())
        {
            _textures.emplace(texResource->_alias, texResource);

        }
    }

    void RawResourceStorage::Release()
    {
        for (auto& [_, resource] : _geoemtryBuffers)
        {
            resource->Destroy();
            delete resource;
        }
        _geoemtryBuffers.clear();

        for (auto& [_, resource] : _shaders)
        {
            delete resource;
        }
        _shaders.clear();

        for (auto& [_, resource] : _textures)
        {
            resource->Destroy();
            delete resource;
        }
        _textures.clear();
    }

    GeometryBuffer* RawResourceStorage::GetGeometryBuffer(const std::string& name)
    {
        GeometryBuffer* aaa = _geoemtryBuffers.find(name)->second;
        return _geoemtryBuffers.find(name)->second;
    }

    Shader* RawResourceStorage::GetShader(const std::string& name)
    {
        return _shaders.find(name)->second;
    }

    TextureResource* RawResourceStorage::GetTexture(const std::string& name)
    {
        return _textures.find(name)->second;
    }

    void RawResourceStorage::AddShader(const std::string& key, Shader* shader)
    {
        _shaders.emplace(std::make_pair(key, shader));
    }

    void RawResourceStorage::AddTexture(const std::string& key, TextureResource* texture)
    {
        _textures.emplace(std::make_pair(key, texture));
    }

} // namespace tb