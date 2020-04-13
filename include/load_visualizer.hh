#pragma once

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
#include <array>
#include "shader.h"

/*
    coordinate system from this to OpenGL:
    Loading Planner:
      z
      |
      |
      0------y
     /
    x
    OpenGL:
      y
      |
      |
      0------x
     /
    z
    so it's like from (x, y, z) to (y, z, x)

    a box is composed of location and dimension

    OpenGL window coordinate:
    0,H ------------ W,H
     |                |
     |                |
     |                |
    0,0 ------------ W,0
*/

namespace dr
{

class LoadVisualizer
{
public:
  LoadVisualizer();
  LoadVisualizer(const LoadVisualizer &) = default;
  LoadVisualizer(LoadVisualizer &&) = default;
  LoadVisualizer &operator=(const LoadVisualizer &) = default;
  LoadVisualizer &operator=(LoadVisualizer &&) = default;
  ~LoadVisualizer();

  bool is_on() { return static_cast<bool>(is_on_); }

  void add_plane(float x1, float y1, float z1, float x2, float y2, float z2);
  void flush_plans();

  void add_box(const std::array<float, 6> &box);
  void flush_boxes();

  void turn_off();
  void turn_on();

private:
  std::thread thread_;
  void thread_main();
  std::mutex mtx_;
  std::atomic<bool> is_on_{true};

  static unsigned int win_width_;
  static unsigned int win_height_;

  float map_range_ = 10;

  bool show_planes_ = true;

  // data format of planes:
  // [x11, y11, z11, x12, y12, z12, x21, y21, z21, x22, y22, z22,...]
  std::vector<float> planes_;
  // [x11, y11, z11, x12, y12, z12, x21, y21, z21, x22, y22, z22,...]
  std::vector<float> boxes_;

  void render_planes(Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
  void render_boxes(
      Shader &shader, const glm::mat4 &model,
      const glm::mat4 &view, const glm::mat4 &projection);

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
  static void glfw_error_callback(int error, const char *description);
  void process_input(GLFWwindow *window);

  void main_panel();
};

}
