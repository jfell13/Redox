cmake_minimum_required(VERSION 3.20)
project(ReDox VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (ReDox_PRERELEASE "alpha")

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)

# Replace this package's invalid SDK-specific zlib dependency.
find_package(ZLIB REQUIRED)
set_property(TARGET assimp::assimp PROPERTY
    INTERFACE_LINK_LIBRARIES ZLIB::ZLIB
)

# Build the generated loader from source instead of relying on glad.o.
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL ${CMAKE_DL_LIBS})

add_library(physics STATIC
    Physics/Velocity.cpp
    Physics/constants.cpp
)
add_library(rendering STATIC
    Rendering/Camera.cpp
    Rendering/LoggerWindow.cpp
    Rendering/MenuBar.cpp
    Rendering/Objects.cpp
    Rendering/Renderer.cpp
)
target_include_directories(physics PUBLIC
    Physics
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_include_directories(rendering PUBLIC
    Rendering
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(rendering PUBLIC glad glm::glm glfw OpenGL::GL assimp::assimp imgui)
target_compile_definitions(rendering PUBLIC GLFW_INCLUDE_NONE)

add_executable(ReDox
    main.cpp
    Controls.cpp
)

set_target_properties(ReDox PROPERTIES
    SUFFIX ".exe"
)

target_link_libraries(ReDox PRIVATE
    physics rendering imgui
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ReDox PRIVATE -Wall -Wextra)
    target_compile_options(physics PRIVATE -Wall -Wextra)
    target_compile_options(rendering PRIVATE -Wall -Wextra)
endif()

# Use a stable path regardless of the launch working directory.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/stylized-character.obj"
               "${CMAKE_CURRENT_BINARY_DIR}/stylized-character.obj" COPYONLY)
target_compile_definitions(ReDox PRIVATE
    ReDox_MODEL_PATH="${CMAKE_CURRENT_BINARY_DIR}/stylized-character.obj"
)
