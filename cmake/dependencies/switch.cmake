# Load target libraries from the devkitPro toolchain, never the host SDK.
include(FetchContent)
find_package(PkgConfig REQUIRED)
option(SWITCH_NXVK_ZINK "Use experimental NXVK/Zink instead of switch-mesa" OFF)
if(SWITCH_NXVK_ZINK)
    pkg_check_modules(SWITCH_SDL2 REQUIRED sdl2)
    pkg_check_modules(NXVK_GL REQUIRED nxvk-gl)
    add_library(lus_switch_nxvk INTERFACE)
    target_include_directories(lus_switch_nxvk INTERFACE ${NXVK_GL_INCLUDE_DIRS})
    target_compile_definitions(lus_switch_nxvk INTERFACE PAPERBOAT_NXVK_ZINK=1)
    # Keep whole-archive/group switches beside their libraries, not in link_options.
    target_link_libraries(lus_switch_nxvk INTERFACE ${NXVK_GL_LDFLAGS} nx m)
    add_library(OpenGL::GL ALIAS lus_switch_nxvk)
    add_library(lus_switch_sdl INTERFACE)
    target_include_directories(lus_switch_sdl INTERFACE ${SWITCH_SDL2_INCLUDE_DIRS})
    target_link_directories(lus_switch_sdl INTERFACE ${SWITCH_SDL2_LIBRARY_DIRS})
    set(SDL_NXVK_LIBS ${SWITCH_SDL2_STATIC_LIBRARIES})
    list(REMOVE_ITEM SDL_NXVK_LIBS EGL GL glapi drm_nouveau)
    target_link_libraries(lus_switch_sdl INTERFACE ${SDL_NXVK_LIBS} OpenGL::GL)
    add_library(SDL2::SDL2 ALIAS lus_switch_sdl)
    add_compile_definitions(PAPERBOAT_NXVK_ZINK=1)
else()
    pkg_check_modules(SWITCH_SDL2 REQUIRED IMPORTED_TARGET sdl2)
    add_library(SDL2::SDL2 ALIAS PkgConfig::SWITCH_SDL2)
    find_package(OpenGL CONFIG REQUIRED)
endif()
find_library(SWITCH_GLAD_LIBRARY glad REQUIRED)
find_path(SWITCH_GLAD_INCLUDE_DIR glad/glad.h REQUIRED)
add_library(lus_switch_glad INTERFACE)
target_include_directories(lus_switch_glad INTERFACE "${SWITCH_GLAD_INCLUDE_DIR}")
target_link_libraries(lus_switch_glad INTERFACE "${SWITCH_GLAD_LIBRARY}" OpenGL::GL)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(tinyxml2_BUILD_TESTING OFF)
set(JSON_BuildTests OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_EXAMPLE OFF)
FetchContent_Declare(tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 11.0.0 GIT_SHALLOW TRUE OVERRIDE_FIND_PACKAGE)
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0 GIT_SHALLOW TRUE OVERRIDE_FIND_PACKAGE)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.16.0 GIT_SHALLOW TRUE OVERRIDE_FIND_PACKAGE)

# O2R needs ZIP/zlib; encryption, command-line tools and optional codecs do not
# belong in the console executable.
set(BUILD_TOOLS OFF)
set(BUILD_REGRESS OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_DOC OFF)
set(BUILD_OSSFUZZ OFF)
set(ENABLE_BZIP2 OFF)
set(ENABLE_LZMA OFF)
set(ENABLE_ZSTD OFF)
set(ENABLE_COMMONCRYPTO OFF)
set(ENABLE_GNUTLS OFF)
set(ENABLE_MBEDTLS OFF)
set(ENABLE_OPENSSL OFF)
FetchContent_Declare(libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip.git
    GIT_TAG v1.11.4 GIT_SHALLOW TRUE OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(tinyxml2 nlohmann_json spdlog libzip)
