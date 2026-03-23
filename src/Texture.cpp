#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

Texture::Texture() : m_textureID(0), m_width(0), m_height(0) {}

Texture::~Texture() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
    }
}

bool Texture::loadFromFile(const std::string& path) {
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 4);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }
    
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
    return true;
}

void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureManager& TextureManager::getInstance() {
    static TextureManager instance;
    return instance;
}

int TextureManager::loadTexture(const std::string& name, const std::string& path) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return it->second;
    }
    
    auto texture = std::make_unique<Texture>();
    if (!texture->loadFromFile(path)) {
        return -1;
    }
    
    int id = m_nextId++;
    m_nameToId[name] = id;
    m_textures[id] = std::move(texture);
    return id;
}

void TextureManager::bindTexture(int id, int unit) {
    auto it = m_textures.find(id);
    if (it != m_textures.end()) {
        it->second->bind(unit);
    }
}

Texture* TextureManager::getTexture(int id) {
    auto it = m_textures.find(id);
    return (it != m_textures.end()) ? it->second.get() : nullptr;
}

Texture* TextureManager::getTextureByName(const std::string& name) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) {
        return getTexture(it->second);
    }
    return nullptr;
}
