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
find_package(Boost CONFIG REQUIRED COMPONENTS json)

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

add_library(stb_image STATIC
    external/stb_impl.cpp
)
target_include_directories(stb_image PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/external"
)

add_library(physics STATIC
    src/Physics/Velocity.cpp
    src/Physics/constants.cpp
    src/Physics/Collisions.cpp
)
add_library(rendering STATIC
    src/Rendering/Camera.cpp
    src/Rendering/InventoryWindow.cpp
    src/Rendering/Model.cpp
    src/Rendering/Renderer.cpp
)
add_library(gameplay STATIC
    src/Gameplay/Controls.cpp
    src/Gameplay/Objects.cpp
    src/Gameplay/Player.cpp
    src/Gameplay/Scene.cpp
)
add_library(inventory STATIC
    src/Inventory/Inventory.cpp
    src/Inventory/ItemCatalog.cpp
)
add_library(ui STATIC
    src/UI/LoggerWindow.cpp
    src/UI/MainMenu.cpp
    src/UI/MenuBar.cpp
    src/UI/PlayerWindow.cpp
)
target_include_directories(physics PUBLIC
    src/Physics
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(rendering PUBLIC
    src/Rendering
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(gameplay PUBLIC
    src/Gameplay
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(inventory PUBLIC
    src/Inventory
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(ui PUBLIC
    src/UI
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(rendering PUBLIC glad glm::glm glfw OpenGL::GL assimp::assimp imgui stb_image)
target_compile_definitions(rendering PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(gameplay PUBLIC rendering inventory physics stb_image)
target_link_libraries(inventory PUBLIC Boost::json)
target_link_libraries(physics PUBLIC glad glm::glm glfw OpenGL::GL rendering)
target_link_libraries(ui PUBLIC gameplay glm::glm glfw imgui stb_image)
target_compile_definitions(gameplay PUBLIC GLFW_INCLUDE_NONE)

add_executable(ReDox
    src/main.cpp
)

set_target_properties(ReDox PROPERTIES
    SUFFIX ".exe"
)

target_link_libraries(ReDox PRIVATE
    physics rendering gameplay inventory ui imgui Boost::json
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ReDox PRIVATE -Wall -Wextra)
    target_compile_options(physics PRIVATE -Wall -Wextra)
    target_compile_options(rendering PRIVATE -Wall -Wextra)
    target_compile_options(gameplay PRIVATE -Wall -Wextra)
    target_compile_options(inventory PRIVATE -Wall -Wextra)
    target_compile_options(ui PRIVATE -Wall -Wextra)
endif()

# Resolve runtime assets independently of the launch working directory.
target_compile_definitions(ReDox PRIVATE
    ReDox_MODEL_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/models/stylized-character.obj"
    ReDox_ITEMS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/data/items.json"
    ReDox_CRATE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/models/Crate.glb"
    ReDox_ICONS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/icons"
)
target_compile_definitions(ui PRIVATE
    ReDox_ICONS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/icons"
)
