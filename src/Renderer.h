#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    
    void use() const;
    
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
    GLuint getProgram() const { return m_program; }
    
private:
    GLuint m_program;
    bool compileShader(GLuint shader, const std::string& source);
};

class Renderer {
public:
    static Renderer& getInstance();
    
    void init();
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    // Рендеринг спрайта
    void drawSprite(int textureId, float x, float y, float width, float height,
                    float rotation = 0.0f, const glm::vec4& color = glm::vec4(1.0f));
    
    // Рендеринг части спрайта (для спрайтшитов)
    void drawSpriteRect(int textureId, float x, float y, float width, float height,
                        int srcX, int srcY, int srcW, int srcH,
                        float rotation = 0.0f, const glm::vec4& color = glm::vec4(1.0f));
    
    // Рендеринг текста (простой)
    void drawText(const std::string& text, float x, float y, float size,
                  const glm::vec3& color = glm::vec3(1.0f));
    
    void setCamera(const glm::mat4& viewMatrix);
    void setProjection(const glm::mat4& projMatrix);
    
    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    
private:
    Renderer() = default;
    
    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    
    void setupBuffers();
};
