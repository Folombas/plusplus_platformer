#pragma once

#include <GL/glew.h>
#include <string>
#include <unordered_map>

class Texture {
public:
    Texture();
    ~Texture();
    
    bool loadFromFile(const std::string& path);
    void bind(int unit = 0) const;
    void unbind() const;
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GLuint getID() const { return m_textureID; }
    
private:
    GLuint m_textureID;
    int m_width;
    int m_height;
};

class TextureManager {
public:
    static TextureManager& getInstance();
    
    int loadTexture(const std::string& name, const std::string& path);
    void bindTexture(int id, int unit = 0);
    Texture* getTexture(int id);
    Texture* getTextureByName(const std::string& name);
    
private:
    TextureManager() = default;
    std::unordered_map<std::string, int> m_nameToId;
    std::unordered_map<int, std::unique_ptr<Texture>> m_textures;
    int m_nextId = 0;
};
