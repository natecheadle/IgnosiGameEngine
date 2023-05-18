#pragma once

#include "IWindow.h"
#include "KeyPressedInfo.hpp"
#include "Keys.h"
#include "MouseClickedInfo.hpp"
#include "WindowMessages.hpp"
#include "WindowSize.hpp"

#include <SquareMatrix4x4.hpp>
#include <Units/Radian.hpp>
#include <Vector3.hpp>

#include <chrono>
#include <cmath>
#include <functional>

namespace nate::Modules::Render
{
    class Camera3D
    {
        Vector3<float> m_Position{0.0, 0.0, 5.0};
        Vector3<float> m_Direction{0.0, 0.0, -1.0};
        Vector3<float> m_WorldUp{0.0, 1.0, 0.0};

        float m_FOV{M_PI_4};
        float m_Near{0.1};
        float m_Far{100.0};

        GUI::IWindow* m_pWindow;

      public:
        virtual ~Camera3D();

        virtual SquareMatrix4x4<float> Projection() const;
        virtual SquareMatrix4x4<float> View() const;

        virtual void Update(std::chrono::nanoseconds /* time */) {}

        const Vector3<float>& CameraPosition() const { return m_Position; }
        const Vector3<float>& CameraDirection() const { return m_Direction; }
        const Vector3<float>& WorldUp() const { return m_WorldUp; }

        void CameraPosition(const Vector3<float>& val);
        void CameraDirection(const Vector3<float>& val);
        void CameraTarget(const Vector3<float>& val);

      protected:
        Camera3D(GUI::IWindow* pWindow);
        const GUI::IWindow* Window() const { return m_pWindow; }

        void WorldUp(const Vector3<float>& val);

        float FOV() const { return m_FOV; }
        float Near() const { return m_Near; }
        float Far() const { return m_Far; }

        void FOV(float val) { m_FOV = val; }
        void Near(float val) { m_Near = val; }
        void Far(float val) { m_Far = val; }

        void Translate(const Vector3<float>& value);
    };
} // namespace nate::Modules::Render