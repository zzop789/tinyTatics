#pragma once

#include <cstdint>
#include <string>   

namespace TinyTactics
{
    // Owns the public-facing behavior for Texture2D.
    // Flow: caller -> Texture2D -> next subsystem.
    // Jump: add implementation details in Texture2D.cpp.
    class Texture2D
    {
    public:
        explicit Texture2D(const std::string& path);
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;

        Texture2D(Texture2D&& other) noexcept;
        Texture2D& operator=(Texture2D&& other) noexcept;

        //Call: bind this texture slot before rendering.

        void bind(uint32_t slot = 0) const;
        static void unbind();

        int GetWidth() const;
        int GetHeight() const;
        uint32_t GetRendererID() const;
    private:
        void Release();

        uint32_t m_RendererID;
        int m_Width=0;
        int m_Height=0;
        int m_Channels=0;
    };
}