cmake_minimum_required(VERSION 3.22.1)
project(multi_realsense_emitter_synchronizer)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(realsense2_camera_msgs REQUIRED)
find_package(message_filters REQUIRED)
find_package(isaac_ros_common REQUIRED)
find_package(nlohmann_json REQUIRED)

# Build the node logic as a shared library (a component)
add_library(
  multi_realsense_emitter_synchronizer_component SHARED
  src/multi_realsense_emitter_synchronizer_node.cpp
  src/emitter_synchronizer.cpp)

target_include_directories(
  multi_realsense_emitter_synchronizer_component PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${isaac_ros_common_INCLUDE_DIRS})

target_link_libraries(
  multi_realsense_emitter_synchronizer_component PUBLIC
  rclcpp::rclcpp
  rclcpp_components::component
  ${realsense2_camera_msgs_TARGETS}
  message_filters::message_filters
  ${isaac_ros_common_LIBRARIES}
  nlohmann_json::nlohmann_json)

# Register the library as a component plugin
rclcpp_components_register_nodes(
  multi_realsense_emitter_synchronizer_component
  PLUGIN "nvblox::MultiRealsenseEmitterSynchronizerNode")

# Create a standalone executable
add_executable(multi_realsense_emitter_synchronizer_node src/multi_realsense_emitter_synchronizer_main.cpp)
target_link_libraries(multi_realsense_emitter_synchronizer_node PRIVATE multi_realsense_emitter_synchronizer_component)

# Install targets
install(
  TARGETS multi_realsense_emitter_synchronizer_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)

install(
  TARGETS multi_realsense_emitter_synchronizer_node
  DESTINATION lib/${PROJECT_NAME})

install(
  DIRECTORY include/
  DESTINATION include)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(test_emitter_synchronizer
    test/test_emitter_synchronizer.cpp)
  if(TARGET test_emitter_synchronizer)
    target_link_libraries(test_emitter_synchronizer
      multi_realsense_emitter_synchronizer_component
      rclcpp::rclcpp
      rclcpp_components::component)
  endif()
endif()

ament_package()
