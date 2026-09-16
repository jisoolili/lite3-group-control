cmake_minimum_required(VERSION 3.8)
project(group_control)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

add_library(group_control_core
  src/action_manager.cpp
  src/group_controller.cpp
  src/robot.cpp
  src/robot_manager.cpp
)

target_include_directories(group_control_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(test_robot_manager tests/test_robot_manager.cpp)
target_link_libraries(test_robot_manager group_control_core)
enable_testing()
add_test(NAME test_robot_manager COMMAND test_robot_manager)

add_executable(dual_robot_demo src/dual_robot_demo.cpp)
target_link_libraries(dual_robot_demo group_control_core)

install(
  TARGETS group_control_core test_robot_manager dual_robot_demo
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

ament_package()
