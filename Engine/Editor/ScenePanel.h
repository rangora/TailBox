#pragma once

#include "Panel.h"

namespace tb
{
    class ScenePanel : public Panel
    {
    public:
        ScenePanel() = default;
        virtual ~ScenePanel() = default;

        void Draw() final;

    private:

    };
}