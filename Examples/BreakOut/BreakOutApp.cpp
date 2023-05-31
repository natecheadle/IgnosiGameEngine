#include "BreakOutApp.h"

#include <3D/Material.h>
#include <3D/Sprite.h>
#include <Keys.h>
#include <LinearAlgebra/Vector2.hpp>
#include <Renderer/Renderer_OpenGL.h>
#include <Texture/ImageFile.h>
#include <Units/Degree.hpp>

#include <memory>

using namespace nate::Modules;
using namespace std::chrono_literals;

namespace nate::BreakOut
{
    BreakOutApp::BreakOutApp(
        std::unique_ptr<ECS::World<Render::Mesh3D, Render::Sprite>> pWorld,
        const GUI::WindowSize&                                      window_size,
        std::string                                                 window_name)
        : App(pWorld->CreateSystem<Render::Renderer_OpenGL, Render::Mesh3D, Render::Sprite>(),
              window_size,
              std::move(window_name))
        , m_pWorld(std::move(pWorld))
    {
        std::filesystem::path shader_dir(std::string_view(SHADER_DIR));
        m_LevelDir = shader_dir / "Levels";
    }

    void BreakOutApp::Initialize()
    {
        std::filesystem::path shader_dir(std::string_view(SHADER_DIR));
        std::filesystem::path shader_inc_dir = std::filesystem::path(std::string_view(SHADER_DIR));
        shader_dir /= "Shaders";
        shader_inc_dir /= "../../Modules/Render/Shader/GLSL/";
        assert(std::filesystem::is_directory(shader_dir));
        assert(std::filesystem::is_directory(shader_inc_dir));

        auto vertex_shader_path   = shader_dir / "vertex_shader.vert";
        auto fragment_shader_path = shader_dir / "fragment_shader.frag";
        auto background_path      = shader_dir / "background.jpg";
        auto paddle_path          = shader_dir / "paddle.png";
        auto ball_path            = shader_dir / "awesomeface.png";

        auto pBackMat   = std::make_unique<Render::Material>();
        auto pPaddleMat = std::make_shared<Render::Material>();
        auto pBallMat   = std::make_shared<Render::Material>();

        auto* pRenderer = GetRenderer();

        Modules::Render::ImageFile background(background_path);
        pBackMat->Diffuse = pRenderer->CreateTexture(background, nate::Modules::Render::TextureUnit::Texture0);

        Modules::Render::ImageFile paddle(paddle_path);
        pPaddleMat->Diffuse = pRenderer->CreateTexture(paddle, Render::TextureUnit::Texture0);

        Modules::Render::ImageFile ball(ball_path);
        pBallMat->Diffuse = pRenderer->CreateTexture(ball, Render::TextureUnit::Texture0);

        auto pVertexShader   = pRenderer->CreateShader(vertex_shader_path, {shader_inc_dir});
        auto pFragmentShader = pRenderer->CreateShader(fragment_shader_path, {shader_inc_dir});
        m_pShader            = pRenderer->CreateShaderProgram(pFragmentShader.get(), nullptr, pVertexShader.get());

        m_pCamera = std::make_unique<Render::Camera>(GetWindow());
        m_pCamera->Near(-1.0);
        m_pCamera->Far(1.0);

        float winWidth   = static_cast<float>(GetWindow()->GetLastWindowSize().Width());
        float windHeight = static_cast<float>(GetWindow()->GetLastWindowSize().Height());

        m_pBackground = std::make_unique<Background>(std::move(m_pWorld->CreateEntity<Background>(
            Render::Sprite(pRenderer, static_cast<float>(background.AspectRatio())))));
        m_pBackground->Sprite().AttachedMaterial((std::move(pBackMat)));
        m_pBackground->Sprite().Origin({0.0f, 0.0f});
        m_pBackground->Sprite().Size({winWidth, windHeight});

        m_pLevel = std::make_unique<Level>(m_pWorld.get(), pRenderer);
        m_pLevel->Load(
            m_LevelDir / "One.lvl",
            static_cast<unsigned int>(winWidth),
            static_cast<unsigned int>(windHeight) / 2U);

        Vector2<float> paddleSize{100.0f, 20.0f};
        Vector2<float> playerPos{winWidth / 2.0f - paddleSize[0] / 2.0f, windHeight - paddleSize[1]};
        m_pPaddle = std::make_unique<Paddle>(std::move(
            m_pWorld->CreateEntity<Paddle>(Render::Sprite(pRenderer, static_cast<float>(paddle.AspectRatio())))));
        m_pPaddle->Sprite().AttachedMaterial(std::move(pPaddleMat));
        m_pPaddle->Sprite().Size(paddleSize);
        m_pPaddle->Sprite().Origin(playerPos);

        m_pBall = std::make_unique<Ball>(std::move(m_pWorld->CreateEntity<Ball>(Render::Sprite(pRenderer))));
        m_pBall->Sprite().AttachedMaterial(std::move(pBallMat));

        Vector2<float> ballPos =
            playerPos + Vector2<float>({paddleSize[0] / 2.0f - m_pBall->Radius(), -m_pBall->Radius() * 2.0f});
        m_pBall->Sprite().Origin(ballPos);
        m_pBall->WindowWidth(winWidth);
    }

    void BreakOutApp::Shutdown()
    {
        m_pCamera.reset();
        m_pLevel.reset();
        m_pBackground.reset();
        m_pShader.reset();
        m_pPaddle.reset();
        m_pBall.reset();
    }

    void BreakOutApp::UpdateApp(std::chrono::nanoseconds time)
    {
        // TODO this should be handled automatically
        m_pCamera->Update(time);

        GetWindow()->ExecuteWithKeyStates([this](const GUI::KeyStateMap& keyStates) {
            if ((keyStates[GUI::Key::Left].first == GUI::KeyState::Pressed ||
                 keyStates[GUI::Key::Left].first == GUI::KeyState::Repeat))
            {
                if (m_pPaddle->Sprite().Origin()[0] > 0.0)
                {
                    m_pPaddle->Sprite().TranslateX(-PADDLE_SPEED);
                    if (m_pBall->IsStuck())
                    {
                        m_pBall->Sprite().TranslateX(-PADDLE_SPEED);
                    }
                }
            }

            if ((keyStates[GUI::Key::Right].first == GUI::KeyState::Pressed ||
                 keyStates[GUI::Key::Right].first == GUI::KeyState::Repeat))
            {
                if (m_pPaddle->Sprite().Origin()[0] <
                    static_cast<float>(GetWindow()->GetLastWindowSize().Width()) - m_pPaddle->Sprite().Size()[0])
                {
                    m_pPaddle->Sprite().TranslateX(PADDLE_SPEED);
                    if (m_pBall->IsStuck())
                    {
                        m_pBall->Sprite().TranslateX(PADDLE_SPEED);
                    }
                }
            }

            if ((keyStates[GUI::Key::Space].first == GUI::KeyState::Pressed ||
                 keyStates[GUI::Key::Space].first == GUI::KeyState::Repeat))
            {
                m_pBall->Release();
            }
        });

        m_pBall->Update(time);

        auto renderUpdate = [&]() -> void {
            m_pShader->Use();
            m_pShader->SetShaderVar("projection", m_pCamera->ViewOrthographic());

            GetRenderer()->DrawAllSprites(m_pShader.get());
        };

        GetRenderer()->ExecuteFunction(renderUpdate);
    }
} // namespace nate::BreakOut
