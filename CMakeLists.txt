cmake_minimum_required(VERSION 3.5.1)
project (lua_vm)

# set release or debug builds
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Release RelWithDebInfo Debug)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()
#SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# set flags for debug and release mode
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-g HAS_DEBUG)
check_cxx_compiler_flag(-Wall HAS_WALL)
check_cxx_compiler_flag(-O0 HAS_OPT0)
check_cxx_compiler_flag(-O3 HAS_OPT3)
check_cxx_compiler_flag(-fmessage-length=0 HAS_MSG)
check_cxx_compiler_flag(-std=c++11 HAS_CNEW)
check_cxx_compiler_flag(-lstdc++fs HAS_FS)
if(HAS_DEBUG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
endif()
if(HAS_WALL)
	if(UNIX)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	else()
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -W4")
	endif()
endif()
if(HAS_OPT0)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()
if(HAS_OPT3)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()
if(HAS_MSG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fmessage-length=0")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fmessage-length=0")
endif()
if(HAS_CNEW)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")
endif()

# include directories for project
if (MSVC)
	find_path(LUA_PATH lua.hpp
		PATHS
		${CMAKE_SOURCE_DIR}/lua53_Win64_lib/include)
	if(NOT LUA_PATH)
		message(FATAL_ERROR "Lua includes not found")
	endif()
endif()

# use the file(GLOB...) or file(GLOB_RECURSE...) to grab wildcard src files
file(GLOB_RECURSE SOURCES 	"${CMAKE_SOURCE_DIR}/source/*.cpp"
							"${CMAKE_SOURCE_DIR}/include/*.hpp"
							"${CMAKE_SOURCE_DIR}/include/*.h"
							"${CMAKE_SOURCE_DIR}/lua53_Win64_lib/include/*.hpp"
							"${CMAKE_SOURCE_DIR}/lua53_Win64_lib/include/*.h")

# add defines for engine resources
add_definitions(
	"-DROOT_DIR=\"${CMAKE_SOURCE_DIR}/\""
)

# use file(GLOB_RECURSE...), add_library(lua_vm...), and
# source_group(<group name> FILES...) to create folder structure in project
# LIST_DIRECTORIES remove directory name from files
# include all necessary headers
file(GLOB_RECURSE PROJECT_SOURCES
	LIST_DIRECTORIES false
	"${CMAKE_SOURCE_DIR}/scripts/*.lua*")

foreach(_proj_source IN ITEMS ${PROJECT_SOURCES})
	get_filename_component(_source_path "${_proj_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}" "${_source_path}")
	source_group("${_source_path_rel}" FILES "${_proj_source}")
endforeach()

include_directories(${CMAKE_SOURCE_DIR}/include ${LUA_PATH})

add_executable(lua_vm ${SOURCES} ${PROJECT_SOURCES})

# set visual studio startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			PROPERTY VS_STARTUP_PROJECT
			lua_vm)

# filesystem goes last
if (UNIX)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -lstdc++fs")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -lstdc++fs")
endif()

if (MSVC)
	find_library(LUA_LIB lua53 ${CMAKE_CURRENT_SOURCE_DIR}/lua53_Win64_lib)
	if(NOT LUA_LIB)
	message(FATAL_ERROR "Lua Release library not found")
	endif()
endif()

target_link_libraries(lua_vm ${LUA_LIB})