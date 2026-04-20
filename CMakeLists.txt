# Shell for moonchild: requires SDL2 + SDL2_mixer (CMake package configs).
# macOS (Homebrew): brew install cmake sdl2 sdl2_mixer
# Then: cmake -S . -B build && cmake --build build

cmake_minimum_required(VERSION 3.16)
project(moonchild_shell LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

# All C/C++ sources under src/ (subfolders included). Re-runs CMake when files are added/removed.
file(GLOB_RECURSE MOONCHILD_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

add_executable(moonchild_shell ${MOONCHILD_SOURCES})

target_include_directories(moonchild_shell PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/moonchild"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/framewrk"
)

target_link_libraries(moonchild_shell PRIVATE
  SDL2::SDL2
  SDL2_mixer::SDL2_mixer
)
