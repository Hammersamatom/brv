cmake_minimum_required (VERSION 3.16)
project (brv VERSION 0.0.1 DESCRIPTION "An attempt at an RV32I interpreter/VM")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")


find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

add_subdirectory(dependencies/json)

add_executable(
  ${CMAKE_PROJECT_NAME}
  src/main.cpp
  src/debug.cpp
)

find_package(fmt REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}
			PRIVATE
			fmt::fmt
      nlohmann_json::nlohmann_json
)