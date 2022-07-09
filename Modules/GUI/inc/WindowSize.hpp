#pragma once

namespace nate::Modules::GUI {
    class WindowSize {
        int m_Width{0};
        int m_Height{0};

      public:
        WindowSize() = default;
        WindowSize(int width, int height)
            : m_Width(width)
            , m_Height(height)
        {
        }
        ~WindowSize() = default;

        WindowSize(const WindowSize& other) = default;
        WindowSize& operator=(const WindowSize& other) = default;

        bool operator==(const WindowSize& other) const = default;

        int Width() const { return m_Width; }
        int Height() const { return m_Height; }

        void Width(int value) { m_Width = value; }
        void Height(int value) { m_Height = value; }
    };
} // namespace nate::Modules::GUI