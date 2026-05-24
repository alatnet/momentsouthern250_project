cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_64 ON)
else()
    set(ARCH_64 OFF)
endif()

if(CMAKE_BUILD_TYPE MATCHES "DEBUG" OR CMAKE_BUILD_TYPE MATCHES "Debug")
    set( DEBUG_BUILD ON )
else()
    set( DEBUG_BUILD OFF )
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(MS250Project VERSION 0.0.1 LANGUAGES C CXX)

list(APPEND projSrcs
    # src/Utils.cpp
    src/main.cpp
    ufbx/ufbx.c
    src/MS250_FBX.cpp
    src/MS250_Math.cpp
    src/MS250_Utils.cpp
    # src/Ray_FBX.cpp
    src/GL_FBX.cpp
    src/glad/gl.c
    src/testTri.cpp
    src/glGrid.cpp
)

add_executable(MS250Project
    ${projSrcs}
)

if (DEBUG_BUILD)
    if (WIN32 OR MSVC)
    else()
        add_compile_options(-Wall -g -fsanitize=address,undefined)
    endif()
else()
endif()

target_compile_definitions(
    MS250Project
    PUBLIC
        UNICODE
        _DEBUG
        MS250_USE_GLM
)

set_target_properties(MS250Project PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(MS250Project PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(MS250Project PROPERTIES SOVERSION ${PROJECT_VERSION})

target_include_directories(MS250Project
    PUBLIC
        include/
        ufbx/
        src/glm/
    PRIVATE
        src/
)

#add raylib
add_library(RAYLIB STATIC IMPORTED)
set_target_properties(RAYLIB PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/raylib/build/raylib/libraylib.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/raylib/build/raylib/include"
)

target_link_libraries(MS250Project
    PUBLIC
        # RAYLIB
        GL
        GLU
        #openal
        m
        pthread
        dl
        X11
        Xrandr
        Xinerama
        Xi
        Xxf86vm
        Xcursor
        glfw
        # GLEW
)

#set output directories
set_target_properties(MS250Project
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MS250Project PROPERTY CXX_STANDARD 20)
endif()
