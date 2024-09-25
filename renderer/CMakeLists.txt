set(TARGET app)

get_filename_component(PATH "." ABSOLUTE)
get_filename_component(INCLUDE_PATH ".." ABSOLUTE)

file(GLOB_RECURSE CXX_SOURCES ${PATH}/*.cpp)
file(GLOB_RECURSE CXX_INCLUDES ${PATH}/*.hpp)

add_library(${TARGET} ${CXX_SOURCES} ${CXX_INCLUDS})

target_include_directories(${TARGET} PUBLIC ${INCLUDE_PATH})
message(AAAAAAAAAA ${INCLUDE_PATH})

target_link_libraries(${TARGET} Vulkan::Vulkan glfw)