#include <LinearAlgebra/SquareMatrix.hpp>
#include <LinearAlgebra/SquareMatrix4x4.hpp>
#include <LinearAlgebra/Vector.hpp>
#include <LinearAlgebra/Vector3.hpp>
#include <LinearAlgebra/Vector4.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

using namespace Ignosi::Libraries::Math;

namespace Ignosi::Test
{
    class MatrixFixture : public testing::Test
    {
      public:
        SquareMatrix4x4<float> test;
        MatrixFixture()
            : test({{{{{1.0, 2.0, 3.0, 4.0}}, {{5.0, 6.0, 7.0, 8.0}}, {{9.0, 10.0, 11.0, 12.0}}, {{13.0, 14.0, 15.0, 16.0}}}}})
        {
        }

        static SquareMatrix4x4<float> TranslateMatrix(const glm::mat4& in)
        {
            SquareMatrix4x4<float> rslt;
            for (size_t i = 0; i < SquareMatrix4x4<float>::size(); ++i)
            {
                for (size_t j = 0; j < SquareMatrix4x4<float>::size(); ++j)
                {
                    rslt[i][j] = in[int(i)][int(j)];
                }
            }
            return rslt;
        }

        static glm::mat4 TranslateMatrix(const SquareMatrix4x4<float>& in)
        {
            glm::mat4 rslt;
            for (size_t i = 0; i < SquareMatrix4x4<float>::size(); ++i)
            {
                for (size_t j = 0; j < SquareMatrix4x4<float>::size(); ++j)
                {
                    rslt[int(i)][int(j)] = in[i][j];
                }
            }
            return rslt;
        }
    };

    TEST_F(MatrixFixture, ValidateDeterminate)
    {
        ASSERT_EQ(0, test.determinant());
    }

    TEST_F(MatrixFixture, ValidateTo3x3)
    {
        SquareMatrix3x3<float> mat3 = {{{{{1.0, 2.0, 3.0}}, {{5.0, 6.0, 7.0}}, {{9.0, 10.0, 11.0}}}}};
        ASSERT_EQ(mat3, test.to_3x3());
    }

    TEST_F(MatrixFixture, ValidateAddition)
    {
        SquareMatrix4x4<float> rslt{
            {{{{2.0, 4.0, 6.0, 8.0}}, {{10.0, 12.0, 14.0, 16.0}}, {{18.0, 20.0, 22.0, 24.0}}, {{26.0, 28.0, 30.0, 32.0}}}}};

        ASSERT_EQ(rslt, test + test);
    }

    TEST_F(MatrixFixture, ValidateSubtraction)
    {
        SquareMatrix4x4<float> rslt{{{{{0.0, 0.0, 0.0, 0.0}}, {{0.0, 0.0, 0.0, 0.0}}, {{0.0, 0.0, 0.0, 0.0}}, {{0.0, 0.0, 0.0, 0.0}}}}};

        ASSERT_EQ(rslt, test - test);
    }

    TEST_F(MatrixFixture, ValidateMultiplication)
    {
        SquareMatrix4x4<float> rslt{
            {{{{90.0, 100.0, 110.0, 120.0}},
              {{202.0, 228.0, 254.0, 280.0}},
              {{314.0, 356.0, 398.0, 440.0}},
              {{426.0, 484.0, 542.0, 600.0}}}}};

        ASSERT_EQ(rslt, test * test);
        ASSERT_EQ(TranslateMatrix(TranslateMatrix(test) * TranslateMatrix(test)), test * test);

        auto rot_x = SquareMatrix4x4<float>::rotate_zyx_init({M_PI_4, 0.0, 0.0});

        auto rot_y = SquareMatrix4x4<float>::rotate_zyx_init({0.0, M_PI_4, 0.0});

        auto rot_x_glm = glm::rotate<float>(glm::mat4(1.0), M_PI_4, glm::vec3(1, 0, 0));
        auto rot_y_glm = glm::rotate<float>(glm::mat4(1.0), M_PI_4, glm::vec3(0, 1, 0));

        ASSERT_EQ(TranslateMatrix(rot_x_glm * rot_y_glm), rot_x * rot_y);
        ASSERT_EQ(TranslateMatrix(rot_y_glm * rot_x_glm), rot_y * rot_x);
    }

    TEST_F(MatrixFixture, ValidateIdentity)
    {
        SquareMatrix4x4<float> rslt{{{{{1.0, 0.0, 0.0, 0.0}}, {{0.0, 1.0, 0.0, 0.0}}, {{0.0, 0.0, 1.0, 0.0}}, {{0.0, 0.0, 0.0, 1.0}}}}};
        ASSERT_EQ(rslt, SquareMatrix4x4<float>::identity<SquareMatrix4x4<float>>());
    }

    TEST_F(MatrixFixture, ValidateInverse)
    {
        SquareMatrix3x3<float> input{{{{{2.0, 1.0, -1.0}}, {{-3.0, -1.0, 2.0}}, {{-2.0, 1.0, 2.0}}}}};
        SquareMatrix3x3<float> rslt{{{{{4.0, 3.0, -1.0}}, {{-2.0, -2.0, 1.0}}, {{5.0, 4.0, -1.0}}}}};
        ASSERT_EQ(rslt, input.invert());
    }

    TEST_F(MatrixFixture, ValidatePerspective)
    {
        SquareMatrix4x4<float> input(SquareMatrix4x4<float>::perspective(M_PI_4, 800.0 / 600.0, 0.1, 100.0));
        glm::mat4              projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

        ASSERT_EQ(TranslateMatrix(projection), input);
    }

    TEST_F(MatrixFixture, ValidateLookAt)
    {
        SquareMatrix4x4<float> input(SquareMatrix4x4<float>::lookat({0.0, 0.0, 3.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}));
        glm::mat4              look_at = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        ASSERT_EQ(TranslateMatrix(look_at), input);
    }

    TEST_F(MatrixFixture, ValidateOrthographic)
    {
        glm::mat4              ortho_mat = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
        SquareMatrix4x4<float> ortho     = SquareMatrix4x4<float>::orthographic(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
        ASSERT_EQ(TranslateMatrix(ortho_mat), ortho);
    }

    TEST_F(MatrixFixture, ValidateRotationMatrix)
    {
        auto rot_zyx = SquareMatrix4x4<float>::rotate_zyx_init({M_PI_4, M_PI_4, M_PI_4});

        auto rot_x = SquareMatrix4x4<float>::rotate_zyx_init({M_PI_4, 0.0, 0.0});

        auto rot_y = SquareMatrix4x4<float>::rotate_zyx_init({0.0, M_PI_4, 0.0});

        auto rot_z = SquareMatrix4x4<float>::rotate_zyx_init({0.0, 0.0, M_PI_4});

        auto rot_x_glm = glm::rotate<float>(glm::mat4(1.0), M_PI_4, glm::vec3(1, 0, 0));
        auto rot_y_glm = glm::rotate<float>(glm::mat4(1.0), M_PI_4, glm::vec3(0, 1, 0));
        auto rot_z_glm = glm::rotate<float>(glm::mat4(1.0), M_PI_4, glm::vec3(0, 0, 1));

        ASSERT_EQ(TranslateMatrix(rot_x_glm), rot_x);
        ASSERT_EQ(TranslateMatrix(rot_y_glm), rot_y);
        ASSERT_EQ(TranslateMatrix(rot_z_glm), rot_z);

        ASSERT_EQ(TranslateMatrix(rot_x_glm * rot_y_glm), rot_x * rot_y);
        ASSERT_EQ(TranslateMatrix(rot_x_glm * rot_z_glm), rot_x * rot_z);
        ASSERT_EQ(TranslateMatrix(rot_y_glm * rot_z_glm), rot_y * rot_z);

        ASSERT_EQ(rot_zyx, rot_z * rot_y * rot_x);
    }

    TEST(VectorFixture, ValidateDotProduct)
    {
        Vector3<float> vec(1.0, 2.0, 3.0);
        float          rslt = vec.dot(vec);
        ASSERT_EQ(14, rslt);
    }

    TEST(VectorFixture, ValidateCrossProduct)
    {
        Vector3<float> vec1(1.0, 2.0, 3.0);
        Vector3<float> vec2(-1.0, -1.0, -1.0);
        Vector3<float> rslt = vec1.cross(vec2);
        ASSERT_EQ(Vector3<float>(1, -2, 1), rslt);
    }

    TEST(VectorFixture, ValidateSubtraction)
    {
        Vector3<float> vec1(1.0, 2.0, 3.0);
        Vector3<float> vec2(-1.0, -1.0, -1.0);
        Vector3<float> rslt = vec1 - vec2;
        ASSERT_EQ(Vector3<float>(2, 3, 4), rslt);
    }

    TEST(LinearAlgebraFixture, ValidateRotateX)
    {
        Vector3<float> init(0, 1, 1);
        Vector4<float> rotated(SquareMatrix4x4<float>::rotate_x_init(M_PI_4) * Vector4<float>(init));
        ASSERT_EQ(Vector3(0.0f, 0.0f, std::sqrt(2.0f)), rotated.ToVector3());
    }

    TEST(LinearAlgebraFixture, ValidateRotateY)
    {
        Vector3<float> init(1, 0, 1);
        Vector4<float> rotated(SquareMatrix4x4<float>::rotate_y_init(M_PI_4) * Vector4<float>(init));
        ASSERT_EQ(Vector3(std::sqrt(2.0f), 0.0f, 0.0f), rotated.ToVector3());
    }

    TEST(LinearAlgebraFixture, ValidateRotateZ)
    {
        Vector3<float> init(1, 1, 0);
        Vector4<float> rotated(SquareMatrix4x4<float>::rotate_z_init(M_PI_4) * Vector4<float>(init));
        ASSERT_EQ(Vector3<float>(0, std::sqrt(2.0f), 0), rotated.ToVector3());
    }

    TEST(LinearAlgebraFixture, ValidateScale)
    {
        Vector3<float> init(1, 1, 1);
        Vector4<float> scaled(SquareMatrix4x4<float>::scale_init({2.0, 2.0, 2.0}) * Vector4<float>(init));
        ASSERT_EQ(Vector3<float>(2.0, 2.0, 2.0), scaled.ToVector3());
    }

    TEST(LinearAlgebraFixture, ValidateTranslate)
    {
        Vector3<float> init(1.0f, 1.0f, 1.0f);
        Vector4<float> translated(SquareMatrix4x4<float>::translate_init({2.0f, 2.0f, 2.0f}) * Vector4<float>(init));
        ASSERT_EQ(Vector3<float>(3.0, 3.0, 3.0), translated.ToVector3());
    }

} // namespace Ignosi::Test