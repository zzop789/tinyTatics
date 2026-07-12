#include "Engine/Renderer/Texture2D.h"

#include <glad/gl.h>
#include <stb_image.h>

#include <stdexcept>
#include <utility>

namespace TinyTactics
{
    Texture2D::Texture2D(const std::string& path)
    {
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
        if (!data)
        {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        GLenum internalFormat = 0;
        GLenum dataFormat = 0;

        if (m_Channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (m_Channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else
        {
            stbi_image_free(data);
            throw std::runtime_error("Unsupported texture channel count: " + path);
        }

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            static_cast<int>(internalFormat),
            m_Width,
            m_Height,
            0,
            dataFormat,
            GL_UNSIGNED_BYTE,
            data);

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        Release();
    }

    Texture2D::Texture2D(Texture2D&& other) noexcept
        : m_RendererID(std::exchange(other.m_RendererID, 0)),
          m_Width(std::exchange(other.m_Width, 0)),
          m_Height(std::exchange(other.m_Height, 0)),
          m_Channels(std::exchange(other.m_Channels, 0))
    {
    }

    Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
    {
        if (this != &other)
        {
            Release();

            m_RendererID = std::exchange(other.m_RendererID, 0);
            m_Width = std::exchange(other.m_Width, 0);
            m_Height = std::exchange(other.m_Height, 0);
            m_Channels = std::exchange(other.m_Channels, 0);
        }

        return *this;
    }

    void Texture2D::bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture2D::unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    int Texture2D::GetWidth() const
    {
        return m_Width;
    }

    int Texture2D::GetHeight() const
    {
        return m_Height;
    }

    uint32_t Texture2D::GetRendererID() const
    {
        return m_RendererID;
    }

    void Texture2D::Release()
    {
        if (m_RendererID != 0)
        {
            glDeleteTextures(1, &m_RendererID);
            m_RendererID = 0;
        }
    }
}