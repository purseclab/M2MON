cmake_minimum_required(VERSION 3.4.3)

# To support both in- and out-of-source builds,
# we check for the presence of the add_llvm_loadable_module command.
# - if this command is not present, we are building out-of-source
if(NOT COMMAND add_llvm_loadable_module)
    if (DEFINED ENV{LLVM_DIR})
        # We need to match the build environment for LLVM:
        # In particular, we need C++11 and the -fno-rtti flag
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_FLAGS "-std=gnu++11 -fno-rtti")

        find_package(LLVM REQUIRED CONFIG)
        
        list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
        include(AddLLVM)

        add_definitions(${LLVM_DEFINITIONS})
        include_directories(${LLVM_INCLUDE_DIRS})

    else()
        message(FATAL_ERROR "\
WARNING: The LLVM_DIR var was not set (required for an out-of-source build)!\n\
Please set this to environment variable to point to the LLVM build directory\
(e.g. on linux: export LLVM_DIR=/path/to/llvm/build/dir)")
    endif()
else()
        set(IN_SOURCE_BUILD 1)
endif()

# Option to disable SVFG optimization for the Minion analysis pass.
option(MINION_SVFG_NOOPT "Disable SVFG optimization" ON)
if(MINION_SVFG_NOOPT)
    add_definitions(-DMINION_SVFG_NOOPT)
endif()

# Option to enable call graph construction for Minion.
option(MINION_CGRAPH "Enable Minion call graph construction" ON)
if(MINION_CGRAPH)
    add_definitions(-DMINION_CGRAPH)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/include)


add_subdirectory(lib)
add_subdirectory(tools)

