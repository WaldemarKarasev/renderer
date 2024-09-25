cmake_minimum_required(VERSION 3.15)

# setting clang compilers
set(CMAKE_CXX_COMPILER "clang++-18")
set(CMAKE_C_COMPILER "clang-18")


project(vulkan_renderer CXX C)


# settings
include(cmake/logging.cmake)
include(cmake/check_compiler.cmake)
include(cmake/compile_options.cmake)
include(cmake/sanitize.cmake)


include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
#   GIT_TAG        3.4 # release-1.10.0
)
# FetchContent_MakeAvailable(glfw)


# SET(CMAKE_FIND_ROOT_PATH  /home/pingvinus/vulkan/1.3.283.0/x86_64)
SET(CMAKE_FIND_ROOT_PATH  /home/pingvinus/vulkan/1.3.290.0/x86_64)
find_package(Vulkan REQUIRED volk)

# include_directories(${Vulkan_INCLUDE_DIRS})
add_subdirectory(renderer)

add_executable(engine main.cpp)
target_link_libraries(engine app)
