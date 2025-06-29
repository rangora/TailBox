#include "InputLayout.h"

namespace tb
{
    std::vector<std::string> InputLayout::semanticStorage(2);

    void InputLayout::AddElement(const char* semantic, DXGI_FORMAT format, UINT offset, UINT slot)
    {
        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = semantic;
        element.SemanticIndex = 0;
        element.Format = format;
        element.InputSlot = slot;
        element.AlignedByteOffset = offset;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;

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
        layout.AddElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0);
        layout.AddElement("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT, 12);
        return layout;
    }

} // namespace tb