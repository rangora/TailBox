#include "Cube.h"
#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/GpuBuffer.h"
#include "Graphics/Renderer.h"
#include "Scene/SceneManager.h"

namespace tb
{
    Cube::Cube()
    {
        // trnasformBuffer
        _cBuffer = new GpuBuffer;
        _cBuffer->CreateConstantBuffer(sizeof(Matrix), 256);

        auto sceneMgr = SceneManager::Get();
        sceneMgr->RegisterMesh(this);
    }

    Cube::~Cube()
    {
        if (_cBuffer)
        {
            delete _cBuffer;
        }
    }

    void Cube::Render(const XMMATRIX& vpMtx)
{
        auto renderer = Renderer::Get();
        auto info = renderer->GetGeometryBuffer("Cube");

        Engine::GetDX12Device()->GetCommmandList()->SetPipelineState(Renderer::Get()->GetShader("Cube")->_pipelineState.Get());

        // b0
        {
            FXMVECTOR posVec{0.f, 0.f, 0.0f};
            XMFLOAT3 rotVec{0.f, 0.f, 0.f};

            XMMATRIX rotMtx = XMMatrixRotationRollPitchYaw(rotVec.x, rotVec.y, rotVec.z);
            XMMATRIX translateMtx = XMMatrixTranslationFromVector(posVec);
            XMMATRIX worldMtx = rotMtx * translateMtx;
            XMMATRIX transposed = XMMatrixTranspose(worldMtx * vpMtx);
            XMStoreFloat4x4(&_matrix._wvpMat, transposed);

            int32 rootParamIndex = 0;

            void* buffer = &_matrix;
            uint32 size = sizeof(_matrix);

            // Get CB handler (PushData)
            ::memcpy(&_cBuffer->_mappedBuffer[_cBuffer->_currentIdx * _cBuffer->_elementSize], buffer, size);
            D3D12_CPU_DESCRIPTOR_HANDLE sourceCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                _cBuffer->_cpuHandleBegin, _cBuffer->_currentIdx * _cBuffer->_handleIncrementSize);
            _cBuffer->_currentIdx++;

            D3D12_CPU_DESCRIPTOR_HANDLE destCPUHandle =
                Engine::GetDX12Device()->GetRootDescriptorHeap()->GetCPUHandle(rootParamIndex);
            Engine::GetDX12Device()->GetRootDescriptorHeap()->SetCBV(sourceCPUHandle, destCPUHandle);
        }

        //// cbv push �� �ϰ�
        //// IASetPrimitiveTopology
        //// IASetVertexBuffers
        //// IASetIndexBuffer
        Engine::GetDX12Device()->GetCommmandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        Engine::GetDX12Device()->GetCommmandList()->IASetVertexBuffers(0, 1, &info->_vertexBufferView);
        Engine::GetDX12Device()->GetCommmandList()->IASetIndexBuffer(&info->_indexBufferView);
        Engine::GetDX12Device()->GetRootDescriptorHeap()->CommitTable(0); // CommitTable
        Engine::GetDX12Device()->GetCommmandList()->DrawIndexedInstanced(info->_indexCount, 1, 0, 0, 0);
    }

    void Cube::Clear()
    {
        _cBuffer->_currentIdx = 0;
    }
} // namespace tb