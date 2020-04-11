add_executable(test_load_visualizer
  test_main.cc
)

target_include_directories(test_load_visualizer PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(test_load_visualizer
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)

target_link_libraries(test_load_visualizer PRIVATE load_visualizer)
