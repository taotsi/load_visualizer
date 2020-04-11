# TODO: make imgui a dependent library

add_library(load_visualizer STATIC
  hostgui.cc
  transceiver.cc
  imgui_impl_glfw.cpp
  imgui_impl_opengl3.cpp
  ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_demo.cpp
  ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/deps/imgui/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/deps/imgui/imgui.cpp
  ${PROJECT_SOURCE_DIR}/deps/gl3w/GL/gl3w.c
)

target_include_directories(load_visualizer PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/deps/imgui
  ${PROJECT_SOURCE_DIR}/deps/gl3w/
)

target_compile_options(load_visualizer PRIVATE
  -Wall
  -Wextra
  -pedantic
  # -Werror # TODO:
  -pthread
)

SET(OpenGL_GL_PREFERENCE GLVND)

find_package(glfw3 3.3 REQUIRED)
find_package(Threads)
find_package(OpenGL REQUIRED)
target_link_libraries(load_visualizer ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} glfw OpenGL::GL)

set_target_properties(load_visualizer
  PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
