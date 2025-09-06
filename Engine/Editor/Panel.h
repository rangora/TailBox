#pragma once

namespace tb
{
    class Panel
    {
    public:
        Panel() = default;
        virtual ~Panel() = default;

        virtual void Draw() = 0;
    };
} // namespace tb