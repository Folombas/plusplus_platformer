#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

class Input {
public:
    static Input& getInstance();
    
    void init(GLFWwindow* window);
    void update();
    
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    bool isKeyUp(int key) const;
    
    bool isMouseButtonDown(int button) const;
    glm::vec2 getMousePosition() const;
    glm::vec2 getMouseDelta() const;
    
private:
    Input() = default;
    
    GLFWwindow* m_window = nullptr;
    std::unordered_map<int, bool> m_keys;
    std::unordered_map<int, bool> m_keysPrev;
    std::unordered_map<int, bool> m_mouseButtons;
    double m_mouseX = 0, m_mouseY = 0;
    double m_prevMouseX = 0, m_prevMouseY = 0;
};
