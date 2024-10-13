cmake_minimum_required(VERSION 3.12)

project(CG_ProjTerreno)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CG_ProjTerreno "src/main.cpp")

add_subdirectory(deps/glfw)

target_include_directories(CG_ProjTerreno PRIVATE
  deps/glm 
  deps/glfw/include
  deps/glew/include
)

target_link_directories(CG_ProjTerreno PRIVATE
  deps/glfw/lib
  deps/glew/lib
)

target_link_libraries(CG_ProjTerreno PRIVATE
  glfw
  GLEW
  GL
)

