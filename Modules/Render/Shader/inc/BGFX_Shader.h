#pragma once

#include "Shader.h"

#include <bgfx/bgfx.h>

#include <filesystem>
#include <vector>

namespace nate::Modules::Render {
    class BGFX_Shader : public Shader {
      public:
        BGFX_Shader(std::string_view name, std::filesystem::path shadersPath);
        ~BGFX_Shader() = default;

        bgfx::ShaderHandle Handle() const { return m_Handle; }

      private:
        bgfx::ShaderHandle m_Handle;

        static bgfx::ShaderHandle    CreateHandle(const std::vector<std::uint8_t>& data);
        static std::filesystem::path GetFullPath(std::string_view name, std::filesystem::path shadersPath);
    };
} // namespace nate::Modules::Render