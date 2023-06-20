#include "App.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

namespace nate::Modules::App
{

    App::App(std::unique_ptr<Render::Renderer> pRenderer, const GUI::WindowSize& window_size, std::string window_name)
        : m_pRenderer(std::move(pRenderer))
    {
        m_pWindow = m_pRenderer->Initialize(window_size, std::move(window_name));
    }

    void App::Close()
    {
        Shutdown();
        m_pRenderer.reset();
    }

    int App::Run()
    {
        try
        {

            Initialize();

            double            period{0};
            std::future<void> swap_future;
            while (!m_pWindow->ShouldClose() && m_pRenderer->IsRunning())
            {
                std::chrono::time_point<std::chrono::high_resolution_clock> begin =
                    std::chrono::high_resolution_clock::now();
                m_pRenderer->ClearColorBuffer();
                m_pRenderer->ClearDepthBuffer();

                if (m_pWindow->ShouldClose())
                    break;

                UpdateApp(period);

                if (m_pRenderer->IsErrored())
                {
                    std::cerr << m_pRenderer->Error().what();
                    return 1;
                }
                if (swap_future.valid())
                {
                    if (std::future_status::ready != swap_future.wait_for(std::chrono::seconds(1)))
                    {
                        throw std::runtime_error("Failed to swap buffers within 1 second.");
                    }
                }
                swap_future = std::move(m_pRenderer->SwapBuffers());

                std::chrono::time_point<std::chrono::high_resolution_clock> end =
                    std::chrono::high_resolution_clock::now();
                period = std::chrono::duration<double>(end - begin).count();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "caught exception e " << e.what();
            //TODO there is probably other state managment that needs to happen here.
        }

        Close();

        return 0;
    }

} // namespace nate::Modules::App