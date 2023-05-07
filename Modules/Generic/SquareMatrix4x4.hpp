#pragma once

#include "SquareMatrix.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

#include <stdexcept>

namespace nate::Modules
{
    template <typename T = float>
    class SquareMatrix4x4 : public SquareMatrix<4, T>
    {
        using BASE = SquareMatrix<4, T>;

      public:
        SquareMatrix4x4() = default;

        SquareMatrix4x4(std::array<Vector<4, T>, 4> init)
            : BASE(init)
        {
        }

        template <typename IT>
        SquareMatrix4x4(IT begin, IT end)
            : BASE(begin, end)
        {
        }

        SquareMatrix4x4(const SquareMatrix4x4& other) = default;
        SquareMatrix4x4(SquareMatrix4x4&& other)      = default;

        static SquareMatrix4x4 lookat(const Vector3<T>& pos, const Vector3<T>& tar, const Vector3<T>& up)
        {
            Vector3<T> dir = pos - tar;
            dir.normalize_this();
            Vector3<T> right  = up.cross(dir).normalize_this();
            Vector3<T> cam_up = dir.cross(right);

            SquareMatrix4x4 lookat(BASE::template identity<SquareMatrix4x4>());
            lookat[0] = Vector4<T>(right, 0.0);
            lookat[1] = Vector4<T>(cam_up, 0.0);
            lookat[2] = Vector4<T>(dir, 0.0);
            lookat.transpose_this();

            SquareMatrix4x4 posMat(BASE::template identity<SquareMatrix4x4>());
            posMat[3] = Vector4<T>(-1 * pos);

            lookat *= posMat;
            return lookat;
        }

        static SquareMatrix4x4 perspective(T fov, T aspect, T near, T far)
        {
            SquareMatrix4x4 per;
            T               ta = std::tan(fov / T(2.0));

            per[0][0] = 1 / (ta * aspect);
            per[1][1] = 1 / ta;
            per[2][2] = -(far + near) / (far - near);
            per[3][2] = (-2 * far * near) / (far - near);
            per[2][3] = -1;

            return per;
        }

        static SquareMatrix4x4 scale_init(const Vector3<T>& scale)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();
            rslt[0][0]           = scale[0];
            rslt[1][1]           = scale[1];
            rslt[2][2]           = scale[2];

            return rslt;
        }

        static SquareMatrix4x4 translate_init(const Vector3<T>& trans)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();
            rslt[3]              = Vector4<T>(trans);

            return rslt;
        }

        static SquareMatrix4x4 rotate_init(const Vector3<T>& rotAxis, T theta)
        {
            auto createMatrix = [](const Vector3<T>& rotAxis, T theta) -> SquareMatrix4x4 {
                SquareMatrix4x4 rslt;
                rslt[0][0] = std::cos(theta) + rotAxis.X() * rotAxis.X() * (1 - std::cos(theta));
                rslt[1][0] = rotAxis.X() * rotAxis.Y() * (1 - std::cos(theta)) - rotAxis.Z() * std::sin(theta);
                rslt[2][0] = rotAxis.X() * rotAxis.Z() * (1 - std::cos(theta)) + rotAxis.Y() * std::sin(theta);

                rslt[0][1] = rotAxis.Y() * rotAxis.X() * (1 - std::cos(theta)) + rotAxis.Z() * std::sin(theta);
                rslt[1][1] = std::cos(theta) + rotAxis.Y() * rotAxis.Y() * (1 - std::cos(theta));
                rslt[2][1] = rotAxis.Y() * rotAxis.Z() * (1 - std::cos(theta)) - rotAxis.X() * std::sin(theta);

                rslt[0][2] = rotAxis.Z() * rotAxis.X() * (1 - std::cos(theta)) - rotAxis.Y() * std::sin(theta);
                rslt[1][2] = rotAxis.Z() * rotAxis.Y() * (1 - std::cos(theta)) + rotAxis.X() * std::sin(theta);
                rslt[2][2] = std::cos(theta) + rotAxis.Z() * rotAxis.Z() * (1 - std::cos(theta));
                return rslt;
            };

            return std::abs(rotAxis.length() - 1.0) > 1e-9 ? createMatrix(rotAxis.normalize(), theta)
                                                           : createMatrix(rotAxis, theta);
        }

        static SquareMatrix4x4 rotate_x_init(T theta)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();
            rslt[1][1]           = std::cos(theta);
            rslt[2][1]           = -std::sin(theta);
            rslt[1][2]           = std::sin(theta);
            rslt[2][2]           = std::cos(theta);
            return rslt;
        }

        static SquareMatrix4x4 rotate_y_init(T theta)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();
            rslt[0][0]           = std::cos(theta);
            rslt[0][2]           = -std::sin(theta);
            rslt[2][0]           = std::sin(theta);
            rslt[2][2]           = std::cos(theta);
            return rslt;
        }

        static SquareMatrix4x4 rotate_z_init(T theta)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();
            rslt[0][0]           = std::cos(theta);
            rslt[1][0]           = -std::sin(theta);
            rslt[0][1]           = std::sin(theta);
            rslt[1][1]           = std::cos(theta);
            return rslt;
        }

        static SquareMatrix4x4 rotate_xyz_init(const Vector3<T>& rot)
        {
            SquareMatrix4x4 rslt = BASE::template identity<SquareMatrix4x4>();

            T alpha = rot.z();
            T beta  = rot.y();
            T gamma = rot.z();

            rslt[0][0] = std::cos(beta) * std::cos(gamma);
            rslt[0][1] = std::cos(beta) * std::sin(gamma);
            rslt[0][2] = -std::sin(beta);

            rslt[1][0] = std::cos(alpha) * std::sin(beta) * std::sin(gamma) - std::cos(alpha) * std::sin(gamma);
            rslt[1][1] = std::sin(alpha) * std::sin(beta) * std::sin(gamma) + std::cos(alpha) * std::cos(gamma);
            rslt[1][2] = std::sin(alpha) * std::cos(beta);

            rslt[2][0] = std::cos(alpha) * std::sin(beta) * std::cos(gamma) + std::sin(alpha) * std::sin(gamma);
            rslt[2][1] = std::sin(alpha) * std::sin(beta) * std::cos(gamma) - std::cos(alpha) * std::sin(gamma);
            rslt[2][2] = std::cos(alpha) * std::cos(beta);

            return rslt;
        }

        SquareMatrix4x4& operator=(const SquareMatrix4x4& other) = default;
        SquareMatrix4x4& operator=(SquareMatrix4x4&& other)      = default;

        SquareMatrix4x4 scale(const Vector3<T>& scale) const noexcept
        {
            SquareMatrix4x4 rslt = scale_init(scale);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& scale_this(const Vector3<T>& scale) noexcept
        {
            *this *= scale_init(scale);
            return *this;
        }

        SquareMatrix4x4 translate(const Vector3<T>& trans) const noexcept
        {
            SquareMatrix4x4 rslt = translate_init(trans);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& translate_this(const Vector3<T>& trans) noexcept
        {
            *this *= translate_init(trans);
            return *this;
        }

        SquareMatrix4x4 rotate(const Vector3<T>& rotAxis, T theta) const noexcept
        {
            SquareMatrix4x4 rslt = rotate_init(rotAxis, theta);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& rotate_this(const Vector3<T>& rotAxis, T theta) noexcept
        {
            *this *= rotate_init(rotAxis, theta);
            return *this;
        }

        SquareMatrix4x4 rotate_x(T theta) const
        {
            SquareMatrix4x4 rslt = rotate_x_init(theta);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& rotate_x_this(T theta)
        {
            *this *= rotate_x_init(theta);
            return *this;
        }

        SquareMatrix4x4 rotate_y(T theta) const
        {
            SquareMatrix4x4 rslt = rotate_y_init(theta);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& rotate_y_this(T theta)
        {
            *this *= rotate_y_init(theta);
            return *this;
        }

        SquareMatrix4x4 rotate_z(T theta) const
        {
            SquareMatrix4x4 rslt = rotate_z_init(theta);
            rslt                 = *this * rslt;
            return rslt;
        }

        SquareMatrix4x4& rotate_z_this(T theta)
        {
            *this *= rotate_z_init(theta);
            return *this;
        }
    };

    template <class T>
    Vector4<T> operator*(const SquareMatrix4x4<T>& lhs, const Vector4<T>& rhs)
    {
        Vector4<T> rslt;
        for (size_t i = 0; i < Vector4<T>::size(); ++i)
        {
            for (size_t j = 0; j < Vector4<T>::size(); ++j)
            {
                rslt[i] += lhs[j][i] * rhs[i];
            }
        }
        return rslt;
    }

    template <class T>
    Vector4<T> operator*(const SquareMatrix4x4<T>& lhs, const Vector3<T>& rhs)
    {
        return lhs * Vector4<T>(rhs);
    }
} // namespace nate::Modules