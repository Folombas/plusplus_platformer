#include "Input.h"
#include "Common.h"

Input& Input::getInstance() {
    static Input instance;
    return instance;
}

void Input::init(GLFWwindow* window) {
    m_window = window;
}

void Input::update() {
    m_keysPrev = m_keys;
    m_prevMouseX = m_mouseX;
    m_prevMouseY = m_mouseY;
    
    // Получаем состояние клавиш
    m_keys[GLFW_KEY_W] = glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS;
    m_keys[GLFW_KEY_A] = glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS;
    m_keys[GLFW_KEY_S] = glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS;
    m_keys[GLFW_KEY_D] = glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS;
    m_keys[GLFW_KEY_SPACE] = glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS;
    m_keys[GLFW_KEY_LEFT] = glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS;
    m_keys[GLFW_KEY_RIGHT] = glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    m_keys[GLFW_KEY_UP] = glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS;
    m_keys[GLFW_KEY_DOWN] = glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS;
    m_keys[GLFW_KEY_ESCAPE] = glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    m_keys[GLFW_KEY_ENTER] = glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS;
    m_keys[GLFW_KEY_Z] = glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS;
    m_keys[GLFW_KEY_X] = glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS;
    
    // Получаем позицию мыши
    glfwGetCursorPos(m_window, &m_mouseX, &m_mouseY);
    
    // Состояние кнопок мыши
    m_mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    m_mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

bool Input::isKeyPressed(int key) const {
    auto it = m_keys.find(key);
    auto itPrev = m_keysPrev.find(key);
    return (it != m_keys.end() && it->second) && (itPrev == m_keysPrev.end() || !itPrev->second);
}

bool Input::isKeyDown(int key) const {
    auto it = m_keys.find(key);
    return it != m_keys.end() && it->second;
}

bool Input::isKeyUp(int key) const {
    auto it = m_keys.find(key);
    return it == m_keys.end() || !it->second;
}

bool Input::isMouseButtonDown(int button) const {
    auto it = m_mouseButtons.find(button);
    return it != m_mouseButtons.end() && it->second;
}

glm::vec2 Input::getMousePosition() const {
    return glm::vec2(static_cast<float>(m_mouseX), static_cast<float>(m_mouseY));
}

glm::vec2 Input::getMouseDelta() const {
    return glm::vec2(
        static_cast<float>(m_mouseX - m_prevMouseX),
        static_cast<float>(m_mouseY - m_prevMouseY)
    );
}
