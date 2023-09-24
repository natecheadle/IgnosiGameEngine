#pragma once

#include "ImageFile.h"

#include <Resource.h>

#include <filesystem>
#include <string>

namespace Ignosi::Modules::Render
{
    enum class TextureUnit : unsigned int
    {
        Texture0,
        Texture1,
        Texture2,
        Texture3,
        Texture4,
        Texture5,
        Texture6,
        Texture7,
        Texture8,
        Texture9,
        Texture10,
        Texture11,
        Texture12,
        Texture13,
        Texture14,
        Texture15,
        Texture16,
    };

    class Texture : ECS::Resource
    {
      private:
        const TextureUnit m_Unit;

      protected:
        Texture(const std::string& name, TextureUnit unit);

      public:
        virtual ~Texture() = default;

        TextureUnit  TextureUnit() const { return m_Unit; }
        virtual void Activate() const = 0;
        virtual void Bind() const     = 0;
    };
} // namespace Ignosi::Modules::Render
