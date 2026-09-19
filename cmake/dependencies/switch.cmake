# Load target libraries from the devkitPro toolchain, never the host SDK.
include(FetchContent)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SWITCH_SDL2 REQUIRED IMPORTED_TARGET sdl2)
add_library(SDL2::SDL2 ALIAS PkgConfig::SWITCH_SDL2)
find_package(OpenGL CONFIG REQUIRED)
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
