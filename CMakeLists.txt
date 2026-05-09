cmake_minimum_required(VERSION 3.10)
project(VideoRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(video_renderer
    src/main.cpp
    src/renderer.cpp
    src/effect_factory.cpp
    src/light_band_effect.cpp
    src/dot_circle_effect.cpp
    src/image_overlay_effect.cpp
    src/box_effect.cpp
)

target_link_libraries(video_renderer ${OpenCV_LIBS})

target_include_directories(video_renderer PRIVATE include)
