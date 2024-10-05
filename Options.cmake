# Check which system we are building for
IF (WIN32)
    message(STATUS "Spice: Building for Windows system.")
    set(SPICE_TARGET_OS "windows" CACHE STRING "Spice target OS")
    add_definitions(-DSPICE_TARGET_OS="windows")
    add_definitions(-DOS_WINDOWS)
ELSEIF (UNIX)
    message(STATUS "Spice: Building for Unix system.")
    set(SPICE_TARGET_OS "unix" CACHE STRING "Spice target OS")
    add_definitions(-DSPICE_TARGET_OS="linux")
    add_definitions(-DOS_UNIX)
ELSE ()
    message(FATAL_ERROR "Unsupported platform. Please compile for Windows or Unix-like systems.")
ENDIF ()

# Check which cpu architecture we are building for
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} TARGET_ARCH_STR)
if (TARGET_ARCH_STR STREQUAL "x86_64" OR TARGET_ARCH_STR STREQUAL "amd64")
    message(STATUS "Spice: Building for x86_64 architecture.")
    set(SPICE_TARGET_ARCH "amd64" CACHE STRING "Spice target arch")
    add_definitions(-DSPICE_TARGET_ARCH="amd64")
    add_definitions(-DARCH_X86_64)
elseif (TARGET_ARCH_STR STREQUAL "arm")
    message(STATUS "Spice: Building for ARM architecture.")
    set(SPICE_TARGET_ARCH "arm" CACHE STRING "Spice target arch")
    add_definitions(-DSPICE_TARGET_ARCH="arm")
    add_definitions(-DARCH_ARM)
elseif (TARGET_ARCH_STR STREQUAL "aarch64")
    message(STATUS "Spice: Building for AArch64 architecture.")
    set(SPICE_TARGET_ARCH "aarch64" CACHE STRING "Spice target arch")
    add_definitions(-DSPICE_TARGET_ARCH="aarch64")
    add_definitions(-DARCH_AARCH64)
else ()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}. Please compile for x86_64, arm or aarch64 architectures.")
endif ()

# Spice version
set(SPICE_VERSION "dev" CACHE STRING "Spice build version")
add_definitions(-DSPICE_VERSION="${SPICE_VERSION}")
message(STATUS "Spice: Build version is set to '${SPICE_VERSION}'")

# Spice built by
set(SPICE_BUILT_BY "$ENV{USERNAME}" CACHE STRING "Spice built by person")
add_definitions(-DSPICE_BUILT_BY="${SPICE_BUILT_BY}")
message(STATUS "Spice: Built by is set to '${SPICE_BUILT_BY}'")

# Unity builds
option(SPICE_UNITY_BUILD "Enable unity builds for the compiler executable" OFF)
if (SPICE_UNITY_BUILD)
    message(STATUS "Spice: Unity builds for Spice are enabled.")
    set(CMAKE_UNITY_BUILD ON)
else ()
    message(STATUS "Spice: Unity builds for Spice are disabled.")
endif ()

# Link time optimization
option(SPICE_LTO "Enable link time optimization (LTO) for the compiler executable" OFF)
if (SPICE_LTO)
    message(STATUS "Spice: LTO for Spice is enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -flto")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
else ()
    message(STATUS "Spice: LTO for Spice is disabled.")
endif ()

# Developer build option (for build speed-ups)
option(SPICE_DEV_COMPILE "Developer-only option" OFF)
if (SPICE_DEV_COMPILE)
    message(STATUS "Spice: Dev build for Spice is enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fuse-ld=mold -ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fuse-ld=mold -ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fuse-ld=mold -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
else ()
    message(STATUS "Spice: Dev build for Spice is disabled.")
endif ()

# Profiler build option (for sampling profilers like perf)
option(SPICE_PROF_COMPILE "Profiler build for Spice" OFF)
if (SPICE_PROF_COMPILE)
    message(STATUS "Spice: Profiler build for Spice is enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fno-omit-frame-pointer -O0")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -fno-omit-frame-pointer -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -g -fno-omit-frame-pointer -O0")
else ()
    message(STATUS "Spice: Profiler build for Spice is disabled.")
endif ()

# Link statically
option(SPICE_LINK_STATIC "Link the compiler executable statically" OFF)
if (SPICE_LINK_STATIC)
    message(STATUS "Spice: Static linking for Spice is enabled.")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
else ()
    message(STATUS "Spice: Static linking for Spice is disabled.")
endif ()

# Coverage
option(SPICE_RUN_COVERAGE "Generate coverage reports for Spice source files" OFF)
if (SPICE_RUN_COVERAGE)
    message(STATUS "Spice: Coverage reports enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
else ()
    message(STATUS "Spice: Coverage reports disabled.")
endif ()

# ASAN
option(SPICE_ASAN "Instrument executable with address sanitizer" OFF)
if (SPICE_ASAN)
    message(STATUS "Spice: Address sanitizer enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address -fsanitize=undefined")
else ()
    message(STATUS "Spice: Address sanitizer disabled.")
endif ()

# TSAN
option(SPICE_TSAN "Instrument executable with thread sanitizer" OFF)
if (SPICE_TSAN)
    message(STATUS "Spice: Thread sanitizer enabled.")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=thread")
else ()
    message(STATUS "Spice: Thread sanitizer disabled.")
endif ()

# Overload of new and delete operators
option(SPICE_OVERLOAD_NEW_DELETE "" OFF)
if (SPICE_OVERLOAD_NEW_DELETE)
    message(STATUS "Spice: New and delete operators are overloaded")
    add_compile_definitions(SPICE_NEW_DELETE_OVERLOADED)
else ()
    message(STATUS "Spice: New and delete operators are not overloaded")
endif ()
