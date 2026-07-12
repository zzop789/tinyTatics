#pragma once

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/VertexArray.h"
#include <glm/gtc/matrix_transform.hpp>

namespace TinyTactics
{
    namespace {
        struct Renderer2DData
        {
            std::shared_ptr<VertexArray> QuadVertexArray;
            std::unique_ptr<Shader> FlatColorShader;
        };
        Renderer2DData s_Data;
    }
    void Renderer2D::Init(){
        const float vertices[] = {
            -0.5F, -0.5F, 0.0F,
             0.5F, -0.5F, 0.0F,
             0.5F,  0.5F, 0.0F,
            -0.5F,  0.5F, 0.0F,
        };
        const uint32_t indices[] = {
            0, 1, 2,
            2, 3, 0,
        };
        s_Data.QuadVertexArray = std::make_shared<VertexArray>();
        auto vertexBuffer = std::make_shared<VertexBuffer>(
            vertices, 
            static_cast<uint32_t>(sizeof(vertices)));

        vertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
        });

        auto indexBuffer = std::make_shared<IndexBuffer>(indices, 6);
        s_Data.QuadVertexArray->AddVertexBuffer(vertexBuffer);
        s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
        s_Data.FlatColorShader = std::make_unique<Shader>(
            "Assets/Shaders/FlatColor.vert", 
            "Assets/Shaders/FlatColor.frag");
    }
    void Renderer2D::Shutdown(){
        s_Data.QuadVertexArray.reset();
        s_Data.FlatColorShader.reset();
    }
    void Renderer2D::BeginScene(const Camera2D& camera){
        s_Data.FlatColorShader->Bind();
        s_Data.FlatColorShader->SetMat4(
            "u_ViewProjection", 
            camera.GetViewProjectionMatrix());
    }
    void Renderer2D::EndScene(){
        s_Data.FlatColorShader->Unbind();
    }
    void Renderer2D::DrawQuad(
        const glm::vec2& position, 
        const glm::vec2& size, 
        const glm::vec4& color){
            const glm::mat4 transform =
                glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));
            
            s_Data.FlatColorShader->SetMat4("u_Transform", transform);
            s_Data.FlatColorShader->SetFloat4("u_Color", color);
            RenderCommand::DrawIndexed(*s_Data.QuadVertexArray);
        }
}