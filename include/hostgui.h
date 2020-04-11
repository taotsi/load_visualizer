#ifndef HOSTGUI_H_
#define HOSTGUI_H_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include "shader.h"

//TODO: 3d plane render
//TODO: mouse click response
//TODO: image play, RGB, disparity/pointcloud
//TODO: 2d plot, vectors
//TODO: seperate ogl window

/*
    coordinate system from this to OpenGL:
    this:
    z
    |
    |   y
    | /
    0------x
    OpenGL:
        y
        |
        |
        |
        0------x
      /
    z
    so it's like from (x, y, z) to (x, z, -y)
*/

/*
    a rectangle is composed of two triangles
    p3-----*p2*
    | \     |
    |   \   |
    |     \ |
    *p1*-----p4
*/

/*
    OpenGL window coordinate:
    0,H ------------ W,H
     |                |
     |                |
     |                |
    0,0 ------------ W,0
*/

class HostGui
{
public:
    HostGui();
    HostGui(const HostGui &) = default;
    HostGui(HostGui &&) = default;
    HostGui &operator=(const HostGui &) = default;
    HostGui &operator=(HostGui &&) = default;
    ~HostGui();

    bool is_on() { return static_cast<bool>(is_on_); }

    void AddPlane(float x1, float y1, float z1, float x2, float y2, float z2);
    void FlushPlanes();

    void TurnOff();
    void TurnOn();

private:
    std::thread thread_;
    void ThreadMain();
    std::mutex mtx_;
    std::atomic<bool> is_on_{true};

    static unsigned int win_width_;
    static unsigned int win_height_;

    float map_range_ = 10;

    bool show_planes_ = true;

    // data format of planes:
    // [x11, y11, z11, x12, y12, z12, x21, y21, z21, x22, y22, z22,...]
    std::vector<float> planes_;

    void RenderPlanes(Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void glfw_error_callback(int error, const char *description);
    void processInput(GLFWwindow *window);

    void MainPanel();

    void RenderPoints(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
    void RenderTest(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
};

#endif // HOSTGUI_H_