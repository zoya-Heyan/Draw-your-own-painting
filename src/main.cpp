#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include <iostream>

struct Point { float x, y; };
struct Line {
    std::vector<Point> points;
    int colorIndex;
    float thickness;
};

std::vector<Line> lines;
bool drawing = false;
int currentColor = 0;
float currentThickness = 2.0f;
int fbWidth, fbHeight;

// 🎨 颜色表
std::array<std::array<float,3>,4> colors = {{
    {1.0f, 0.0f, 0.0f}, // 红
    {0.0f, 1.0f, 0.0f}, // 绿
    {0.0f, 0.0f, 1.0f}, // 蓝
    {0.0f, 0.0f, 0.0f}  // 黑
}};

// 判断点击是否在矩形内
bool isInRect(double x, double y, float rx, float ry, float rw, float rh) {
    return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

// 绘制矩形
void drawRect(float x, float y, float w, float h, std::array<float,3> c) {
    glColor3f(c[0], c[1], c[2]);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
    glEnd();
}

// 📐 窗口大小变化回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    fbWidth = width;
    fbHeight = height;
    glViewport(0, 0, fbWidth, fbHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, fbWidth, fbHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// 🖱 鼠标点击
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    // 转换为 framebuffer 坐标
    double scaleX = (double)fbWidth / winWidth;
    double scaleY = (double)fbHeight / winHeight;
    xpos *= scaleX;
    ypos *= scaleY;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // 点击上方工具栏区域
        if (ypos < 70) {
            if (isInRect(xpos, ypos, 20, 20, 40, 40)) currentColor = 0;
            else if (isInRect(xpos, ypos, 70, 20, 40, 40)) currentColor = 1;
            else if (isInRect(xpos, ypos, 120, 20, 40, 40)) currentColor = 2;
            else if (isInRect(xpos, ypos, 170, 20, 40, 40)) currentColor = 3;
            else if (isInRect(xpos, ypos, 230, 20, 70, 40)) currentThickness += 1.0f;
            else if (isInRect(xpos, ypos, 310, 20, 70, 40)) currentThickness = std::max(1.0f, currentThickness - 1.0f);
            else if (isInRect(xpos, ypos, 390, 20, 80, 40) && !lines.empty()) lines.pop_back();
            else if (isInRect(xpos, ypos, 480, 20, 80, 40)) lines.clear();
            return;
        }

        // 开始画线
        drawing = true;
        lines.push_back(Line());
        lines.back().colorIndex = currentColor;
        lines.back().thickness = currentThickness;
    } 
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        drawing = false;
    }
}

// 🖱 鼠标移动
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!drawing || lines.empty()) return;

    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    float fx = xpos * fbWidth / winWidth;
    float fy = ypos * fbHeight / winHeight;
    lines.back().points.push_back({fx, fy});
}

// ⌨️ 键盘快捷键
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    switch (key) {
        case GLFW_KEY_C:  // 切换颜色
            currentColor = (currentColor + 1) % colors.size();
            break;
        case GLFW_KEY_UP:  // 加粗
        case GLFW_KEY_EQUAL:
        case GLFW_KEY_KP_ADD:
            currentThickness += 1.0f;
            break;
        case GLFW_KEY_DOWN: // 减细
        case GLFW_KEY_MINUS:
        case GLFW_KEY_KP_SUBTRACT:
            currentThickness = std::max(1.0f, currentThickness - 1.0f);
            break;
        case GLFW_KEY_U: // 撤销
            if (!lines.empty()) lines.pop_back();
            break;
        case GLFW_KEY_R: // 清空
            lines.clear();
            break;
    }
}

int main() {
    if (!glfwInit()) return -1;

    int width = 1000, height = 700;
    GLFWwindow* window = glfwCreateWindow(width, height, "🎨Drawer", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    // 初始化视口
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    framebuffer_size_callback(window, fbWidth, fbHeight);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制工具栏背景
        drawRect(0, 0, fbWidth, 70, {0.93f, 0.93f, 0.93f});

        // 绘制颜色按钮
        for (int i = 0; i < 4; ++i) {
            float x = 20 + i * 50;
            drawRect(x, 20, 40, 40, colors[i]);
            if (i == currentColor) {
                glColor3f(0.3f, 0.3f, 0.3f);
                glLineWidth(3);
                glBegin(GL_LINE_LOOP);
                glVertex2f(x, 20);
                glVertex2f(x+40, 20);
                glVertex2f(x+40, 60);
                glVertex2f(x, 60);
                glEnd();
            }
        }

        // 绘制功能按钮
        drawRect(230, 20, 70, 40, {0.8f,0.8f,0.8f}); // +
        drawRect(310, 20, 70, 40, {0.8f,0.8f,0.8f}); // -
        drawRect(390, 20, 80, 40, {0.9f,0.7f,0.7f}); // undo
        drawRect(480, 20, 80, 40, {0.7f,0.9f,0.7f}); // clear

        // 绘制线条
        for (auto &line : lines) {
            glLineWidth(line.thickness);
            auto &c = colors[line.colorIndex];
            glColor3f(c[0], c[1], c[2]);
            glBegin(GL_LINE_STRIP);
            for (auto &p : line.points)
                glVertex2f(p.x, p.y);
            glEnd();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}