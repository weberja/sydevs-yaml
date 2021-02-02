set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

cmake_minimum_required(VERSION 3.5)
project(sydevs-yaml VERSION 1.0
                    DESCRIPTION "Describe Simulation with a .yaml"
                    LANGUAGES CXX)

# ------------------------------------------------------------------------------
#
#   Yaml-Cpp
#
# ------------------------------------------------------------------------------
add_subdirectory(external/yaml-cpp)
target_compile_options(yaml-cpp PUBLIC -Wno-shadow)

# ------------------------------------------------------------------------------
#
#   Registerer
#
# ------------------------------------------------------------------------------
set(REG_DIR external/registerer)
file(GLOB REG_HDRS "${REG_DIR}/*.h")

# ------------------------------------------------------------------------------
#
#   Lua
#
# ------------------------------------------------------------------------------
add_subdirectory(external/lua-cmake)

set(SOL_DIR external/sol)
file(GLOB SOL_HDRS "${SOL_DIR}/*.hpp")
#include_directories(external)
# ------------------------------------------------------------------------------
#
#   SyDEVS Library
#
# ------------------------------------------------------------------------------
set(SYDEVS_DIR external/sydevs)
#add_subdirectory(external/sydevs)

set(CORE_DIR ${SYDEVS_DIR}/src/sydevs/core)
file(GLOB CORE_HDRS "${CORE_DIR}/*.h")
aux_source_directory(${CORE_DIR} CORE_SRCS)
source_group("core-include" FILES ${CORE_HDRS})
source_group("core-src" FILES ${CORE_SRCS})

set(TIME_DIR ${SYDEVS_DIR}/src/sydevs/time)
file(GLOB TIME_HDRS "${TIME_DIR}/*.h")
aux_source_directory(${TIME_DIR} TIME_SRCS)
source_group("time-include" FILES ${TIME_HDRS})
source_group("time-src" FILES ${TIME_SRCS})

set(SYSTEMS_DIR ${SYDEVS_DIR}/src/sydevs/systems)
file(GLOB SYSTEMS_HDRS "${SYSTEMS_DIR}/*.h")
aux_source_directory(${SYSTEMS_DIR} SYSTEMS_SRCS)
source_group("systems-include" FILES ${SYSTEMS_HDRS})
source_group("systems-src" FILES ${SYSTEMS_SRCS})

add_library(SyDEVS ${CORE_SRCS} ${TIME_SRCS} ${SYSTEMS_SRCS} ${CORE_HDRS} ${TIME_HDRS} ${SYSTEMS_HDRS})
target_include_directories(SyDEVS PUBLIC ${SYDEVS_DIR}/src)

target_compile_features(SyDEVS PUBLIC cxx_std_11)
set_target_properties(SyDEVS PROPERTIES CXX_EXTENSIONS OFF)
# ------------------------------------------------------------------------------
#
#   SyDEVS-YAML
#
# ------------------------------------------------------------------------------
set(CODE_DIR src)

# sydevs::systems
set(GENERICS_SYSTEMS_DIR ${CODE_DIR}/systems)
aux_source_directory(${GENERICS_SYSTEMS_DIR} GENERICS_SYSTEMS_SRCS)
file(GLOB GENERICS_SYSTEMS_HDRS "${GENERICS_SYSTEMS_DIR}/*.h")

# sydevs::generics
set(GENERICS_DIR ${CODE_DIR}/generics)
aux_source_directory(${GENERICS_DIR} GENERICS_SRCS)
file(GLOB GENERICS_HDRS "${GENERICS_DIR}/*.h")

add_library(SyDEVS-YAML ${GENERICS_SYSTEMS_SRCS} ${GENERICS_SRCS} ${GENERICS_SYSTEMS_HDRS} ${GENERICS_HDRS} ${REG_HDRS})
target_link_libraries(SyDEVS-YAML PUBLIC SyDEVS yaml-cpp lua::lib)
target_include_directories(SyDEVS-YAML PUBLIC src/ external/)

target_compile_features(SyDEVS-YAML PUBLIC cxx_std_17)
set_target_properties(SyDEVS-YAML PROPERTIES CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
#
#   Move Example
#
# ------------------------------------------------------------------------------
file( GLOB_RECURSE pattern_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/example/counter" "example/counter/*.*" )
list(FILTER lib_srcs EXCLUDE REGEX ".*cmake-build.*")
foreach( pattern_file ${pattern_files} )
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${pattern_file}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/example/counter/${pattern_file}" "${CMAKE_CURRENT_BINARY_DIR}/example/${pattern_file}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/example/counter/${pattern_file}")
	list( APPEND pattern_files_dest "${pattern_file}" )
endforeach( pattern_file )

add_custom_target(CopyExampleSimulation ALL DEPENDS ${pattern_files_dest} )

file( GLOB_RECURSE lua_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/src/lua/" "src/lua/*.*" )
list(FILTER lib_srcs EXCLUDE REGEX ".*cmake-build.*")
foreach( lua_file ${lua_files} )
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${lua_file}" COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/src/lua/${lua_file}" "${CMAKE_CURRENT_BINARY_DIR}/lua/${lua_file}" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/lua/${lua_file}")
	list( APPEND lua_files_dest "${lua_file}" )
endforeach( lua_file )

add_custom_target(CopyGlobalLuaFiles ALL DEPENDS ${lua_files_dest} )

# ------------------------------------------------------------------------------
#
#   Example
#
# ------------------------------------------------------------------------------
set(EXAMPLE_DIR example)
aux_source_directory(${EXAMPLE_DIR} EXAMPLE_SRCS)
aux_source_directory(${EXAMPLE_DIR}/nodes/ EXAMPLE_NODES_SRCS)
file(GLOB EXAMPLE_HDRS "${EXAMPLE_DIR}/*/*.h")
add_executable(counter example/main.cpp ${EXAMPLE_SRCS} ${EXAMPLE_NODES_SRCS} ${EXAMPLE_HDRS})
target_link_libraries(counter PRIVATE SyDEVS-YAML)
target_include_directories(counter PRIVATE ${EXAMPLE_DIR})
add_dependencies(counter CopyExampleSimulation CopyGlobalLuaFiles)