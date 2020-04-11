add_executable(load_visualizer_app main.cc)

target_include_directories(load_visualizer_app PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(load_visualizer_app PRIVATE
  -Wall
  -Wextra
  -pedantic
  # -Werror # TODO:
  -pthread
)

target_link_libraries(load_visualizer_app PRIVATE load_visualizer)

set_target_properties(load_visualizer_app
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
