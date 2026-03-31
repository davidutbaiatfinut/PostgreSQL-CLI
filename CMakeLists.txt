cmake_minimum_required(VERSION 3.14)
project(PostgreSQLCLI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure warnings are treated seriously
add_compile_options(-Wall -Wextra -pedantic)

# Add Homebrew libpq path for Apple Silicon/Intel macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/libpq")
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/libpq")
endif()

find_package(PostgreSQL REQUIRED)

# Fetch nlohmann_json for configuration parsing
include(FetchContent)
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Fetch GLFW and ImGui for the GUI module
find_package(OpenGL REQUIRED)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.5-docking
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Create an object library for our core code so we can link it easily for testing
set(CORE_SOURCES
    src/utils/logger.cpp
    src/utils/config.cpp
    src/db/pg_connection.cpp
    src/db/pg_query.cpp
    src/core/command_handler.cpp
    src/cli/cli_parser.cpp
    src/cli/formatter.cpp
)

add_library(pgcli_core OBJECT ${CORE_SOURCES})
target_include_directories(pgcli_core PUBLIC src)
if(PostgreSQL_FOUND)
    target_include_directories(pgcli_core PUBLIC ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(pgcli_core PUBLIC ${PostgreSQL_LIBRARIES} nlohmann_json::nlohmann_json)
endif()

# Main executable
add_executable(pgcli src/main.cpp src/ui/gui_app.cpp ${IMGUI_SOURCES})
target_link_libraries(pgcli PRIVATE pgcli_core glfw OpenGL::GL)
target_include_directories(pgcli PRIVATE src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

# Tests (Optional, but included structure if user wants to expand)
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.2
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(pgcli_tests src/tests/test_main.cpp) # Placeholder
    target_link_libraries(pgcli_tests PRIVATE pgcli_core Catch2::Catch2WithMain)
    add_test(NAME pgcli_tests COMMAND pgcli_tests)
endif()
