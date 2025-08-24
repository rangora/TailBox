#include "InputLayout.h"

namespace tb
{
    std::vector<std::string> InputLayout::semanticStorage(2);

    void InputLayout::AddElement(const char* semantic, UINT index, DXGI_FORMAT format, UINT offset, UINT slot,
                                 D3D12_INPUT_CLASSIFICATION classification, UINT stepRate)
    {
        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = semantic;
        element.SemanticIndex = index;
        element.Format = format;
        element.InputSlot = slot;
        element.AlignedByteOffset = offset;
        element.InputSlotClass = classification;
        element.InstanceDataStepRate = stepRate;

        _elements.push_back(element);
    }

    UINT InputLayout::GetSize() const
    {
        return static_cast<UINT>(_elements.size());
    }

    D3D12_INPUT_LAYOUT_DESC InputLayout::GetDesc() const
    {
        D3D12_INPUT_LAYOUT_DESC desc = {};
        desc.pInputElementDescs = _elements.data();
        desc.NumElements = static_cast<UINT>(_elements.size());
        return desc;
    }

    InputLayout InputLayoutPreset::BaseInputLayout()
    {
        InputLayout layout;
        layout.AddElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        layout.AddElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 12, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        return layout;
    }

    InputLayout InputLayoutPreset::MaterialInputLayout()
    {
        InputLayout layout;
        layout.AddElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        layout.AddElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 12, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        //layout.AddElement("COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 28, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        //layout.AddElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 20, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        //layout.AddElement("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 32, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        //layout.AddElement("BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 44, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        return layout;
    }

} // namespace tb