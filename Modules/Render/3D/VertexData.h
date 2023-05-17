#pragma once

#include "Renderer/VertexDataConfig.h"

#include <Vector3.hpp>

namespace nate::Modules::Render
{
    struct VertexData
    {
        Vector3<float> Position;
        Vector3<float> Normal;
        Vector2<float> TextureCoord;

        static VertexDataConfig describe()
        {
            static VertexDataConfig config;
            if (config.Describe().empty())
            {
                config.PushBackConfig<float, 3>();
                config.PushBackConfig<float, 3>();
                config.PushBackConfig<float, 2>();
            }
            return config;
        }
    };

    static_assert(sizeof(VertexData) == sizeof(float) * 8, "VertexData must be tightly packed");
} // namespace nate::Modules::Render