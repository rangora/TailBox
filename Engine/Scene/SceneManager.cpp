#include "SceneManager.h"
#include "Core/Event.h"
#include "Core/Input.h"
#include "Engine.h"
#include "Graphics/DX12Device.h"
#include "Graphics/Mesh.h"
#include "Scene.h"
#include "Window/Window.h"

namespace tb
{
    SceneManager::SceneManager()
    {
        float rw = 0.0, rh = 0.f;
        Engine::GetDX12Device()->GetRenderTargetViewSize(rw, rh);

        _camera.SetPosition(0.f, 0.f, -4.f);
        _camera.SetRotation(0.f, 0.f, 0.f);
        _camera.SetProjection(90.f, (float)rw / (float)rh, 0.1f, 1000.f);
    }

    void SceneManager::SetLayer(uint32 index, const std::string& name)
    {
        if (_layerIndex.find(name) == _layerIndex.end())
        {
            _activeIndex = _scenes.size();
            _layerIndex.insert({name, _activeIndex});
            _scenes[index] = new Scene;
        }

        _activeIndex = index;

        Scene* newScene = _scenes[_activeIndex];
        newScene->Initialize();
    }

    void SceneManager::RegisterMesh(Mesh* mesh)
    {
        auto scene = _scenes[_activeIndex];
        scene->AddMesh(mesh);
    }

    void SceneManager::RegisterMesh(Cube* cube)
    {
        auto scene = _scenes[_activeIndex];
        scene->AddCube(cube);
    }

    void SceneManager::Update(float deltaTime)
    {
        static int32 preMousePosX = 0;
        static int32 preMousePosY = 0;
        auto window = Engine::Get().GetWindow();

        if (Input::IsMouseButtonPressed(MouseButton::Right))
        {
            preMousePosX = window->GetMousePosX();
            preMousePosY = window->GetMousePosY();
        }

        if (Input::IsMouseButtonDown(MouseButton::Right))
        {
            int32 mousePosX = window->GetMousePosX();
            int32 mousePosY = window->GetMousePosY();

            int32 x_delta = mousePosX - preMousePosX;
            int32 y_delta = mousePosY - preMousePosY;

            static float speed = 0.5f;
            _camera.AddRotation(y_delta * speed * deltaTime, x_delta * speed * deltaTime, 0.f);

            preMousePosX = mousePosX;
            preMousePosY = mousePosY;
        }

        if (Input::IsKeyButtonDown(KeyButton::W) || Input::IsKeyButtonDown(KeyButton::S))
        {
            const float sign = Input::IsKeyButtonDown(KeyButton::W) ? 1.f : -1.f;

            static float speed = 1.f;
            XMFLOAT3 additive = {};
            XMStoreFloat3(&additive, XMVector3Normalize(_camera._forward) * speed * deltaTime);
            _camera.AddPosition(additive.x * sign, additive.y * sign, additive.z * sign);
        }

        if (Input::IsKeyButtonDown(KeyButton::D) || Input::IsKeyButtonDown(KeyButton::A))
        {
            const float sign = Input::IsKeyButtonDown(KeyButton::D) ? 1.f : -1.f;

            static float speed = 1.f;
            XMFLOAT3 additive = {};
            XMStoreFloat3(&additive, XMVector3Normalize(_camera._right) * speed * deltaTime);
            _camera.AddPosition(additive.x * sign, additive.y * sign, additive.z * sign);
        }

        Scene* scene = _scenes[_activeIndex];
        scene->Update(deltaTime);
    }

    void SceneManager::OnEndFrame()
    {
        // CameraInfo event
        {
            CameraInfoDisplayEvent event(_camera._pos, _camera._rot);
            EventDispatcher dispatcher(event);

            dispatcher.Dispatch<CameraInfoDisplayEvent>([](CameraInfoDisplayEvent& e)
                {
                    Engine::Get().GetWindow()->OnUpdateCameraInfo(e.GetPos(), e.GetRot());
                    return true;
                });
        }
    }

    void SceneManager::Render()
    {
        XMMATRIX vpMtx = _camera._viewMtx * _camera._projMtx;
        Scene* scene = _scenes[_activeIndex];
        scene->Render(vpMtx);
    }

    void SceneManager::OnRenderBegin()
    {
        _scenes[_activeIndex]->Clear();
    }
} // namespace tb