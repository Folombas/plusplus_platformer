#include "Renderer.h"
#include <iostream>

const char* VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec2 TexCoord;
out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Color = aColor;
}
)";

const char* FRAGMENT_SHADER = R"(
#version 330 core
in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = texture(ourTexture, TexCoord) * Color;
    } else {
        FragColor = Color;
    }
}
)";

Shader::Shader() : m_program(0) {}

Shader::~Shader() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

bool Shader::compileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return false;
    }
    return true;
}

bool Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    if (!compileShader(vertexShader, vertexSource)) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    if (!compileShader(fragmentShader, fragmentSource)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);
    
    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_program);
        m_program = 0;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void Shader::use() const {
    glUseProgram(m_program);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

void Renderer::init() {
    m_shader.loadFromSource(VERTEX_SHADER, FRAGMENT_SHADER);
    setupBuffers();
    
    m_viewMatrix = glm::mat4(1.0f);
    m_projectionMatrix = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
                                     static_cast<float>(WINDOW_HEIGHT), 0.0f, -1.0f, 1.0f);
}

void Renderer::setupBuffers() {
    float vertices[] = {
        // pos      // tex
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    
    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // tex coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void Renderer::shutdown() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void Renderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_shader.use();
    m_shader.setMat4("view", m_viewMatrix);
    m_shader.setMat4("projection", m_projectionMatrix);
}

void Renderer::endFrame() {
    // Nothing for now
}

void Renderer::drawSprite(int textureId, float x, float y, float width, float height,
                          float rotation, const glm::vec4& color) {
    drawSpriteRect(textureId, x, y, width, height, 0, 0, 1, 1, rotation, color);
}

void Renderer::drawSpriteRect(int textureId, float x, float y, float width, float height,
                               int srcX, int srcY, int srcW, int srcH,
                               float rotation, const glm::vec4& color) {
    m_shader.use();
    
    // Получаем размеры текстуры
    int texWidth = 1, texHeight = 1;
    if (textureId >= 0) {
        auto tex = TextureManager::getInstance().getTexture(textureId);
        if (tex) {
            texWidth = tex->getWidth();
            texHeight = tex->getHeight();
            tex->bind(0);
            m_shader.setInt("ourTexture", 0);
            m_shader.setInt("useTexture", 1);
        } else {
            m_shader.setInt("useTexture", 0);
        }
    } else {
        m_shader.setInt("useTexture", 0);
    }
    
    // Вычисляем UV координаты
    float u0 = (texWidth > 0) ? static_cast<float>(srcX) / texWidth : 0.0f;
    float v0 = (texHeight > 0) ? static_cast<float>(srcY) / texHeight : 0.0f;
    float u1 = (texWidth > 0) ? static_cast<float>(srcX + srcW) / texWidth : 1.0f;
    float v1 = (texHeight > 0) ? static_cast<float>(srcY + srcH) / texHeight : 1.0f;
    
    float vertices[] = {
        // pos      // tex
        x, y,           u0, v0,
        x + width, y,   u1, v0,
        x + width, y + height, u1, v1,
        x, y + height, u0, v1
    };
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    // Модельная матрица с вращением
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec2 center(x + width / 2.0f, y + height / 2.0f);
    model = glm::translate(model, glm::vec3(center, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-center, 0.0f));
    m_shader.setMat4("model", model);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::drawText(const std::string& text, float x, float y, float size,
                        const glm::vec3& color) {
    // Простая заглушка - в полной версии нужен шрифт
    drawSprite(-1, x, y, static_cast<float>(text.length()) * size * 0.6f, size, 0.0f,
               glm::vec4(color, 1.0f));
}

void Renderer::setCamera(const glm::mat4& viewMatrix) {
    m_viewMatrix = viewMatrix;
    m_shader.setMat4("view", m_viewMatrix);
}

void Renderer::setProjection(const glm::mat4& projMatrix) {
    m_projectionMatrix = projMatrix;
    m_shader.setMat4("projection", m_projectionMatrix);
}
