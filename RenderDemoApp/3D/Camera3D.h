#pragma once

#include "IWindow.h"
#include "KeyPressedInfo.hpp"
#include "Keys.h"
#include "MouseClickedInfo.hpp"
#include "WindowMessages.hpp"
#include "WindowSize.hpp"

#include <SquareMatrix4x4.hpp>
#include <Vector3.hpp>

#include <chrono>
#include <functional>

namespace nate::Modules::Render
{
    class Camera3D
    {
        Vector3<float> m_Position;
        Vector3<float> m_Target;
        Vector3<float> m_WorldUp{
            {0.0, 1.0, 0.0}
        };

        float m_FOV;
        float m_Near;
        float m_Far;

        // NOTE: The following 3 vectors are calculated values stored for conveniance.
        Vector3<float> m_Direction; // NOTE: This vector points out the back of the camera
        Vector3<float> m_Right;
        Vector3<float> m_CameraUp;

        GUI::IWindow* m_pWindow;

      public:
        virtual ~Camera3D();

        virtual SquareMatrix4x4<float> Projection() const;
        virtual SquareMatrix4x4<float> View() const;

        virtual void Update(std::chrono::nanoseconds /* time */) {}

      protected:
        Camera3D(GUI::IWindow* pWindow);
        const GUI::IWindow* Window() const { return m_pWindow; }

        const Vector3<float>& CameraPosition() const { return m_Position; }
        const Vector3<float>& CameraTarget() const { return m_Target; }
        const Vector3<float>& WorldUp() const { return m_WorldUp; }

        Vector3<float> CameraDirection() const { return m_Direction; }
        Vector3<float> CameraRight() const { return m_Right; }
        Vector3<float> CameraUp() const;

        void CameraPosition(const Vector3<float>& val);
        void CameraTarget(const Vector3<float>& val);
        void WorldUp(const Vector3<float>& val);

        float FOV() const { return m_FOV; }
        float Near() const { return m_Near; }
        float Far() const { return m_Far; }

        void FOV(float val) { m_FOV = val; }
        void Near(float val) { m_Near = val; }
        void Far(float val) { m_Far = val; }

        void Translate(const Vector3<float>& value);
        void RotateX(float val);
        void RotateY(float val);
        void RotateZ(float val);

      private:
        void Recalculate();
    };
} // namespace nate::Modules::Render