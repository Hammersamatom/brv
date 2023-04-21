cmake_minimum_required (VERSION 3.16)
project (brv VERSION 0.0.1 DESCRIPTION "An attempt at an RV32I interpreter/VM")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

# Find SDL2
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
#find_package(SDL2 REQUIRED COMPONENTS main)

# Define executable target
#include_directories(
#  ./
#  ${SDL2_INCLUDE_DIRS}
#  ${SDL2main_INCLUDE_DIRS}
#)

add_executable(
  ${CMAKE_PROJECT_NAME}
  src/main.cpp
    #  ${SOURCES}
)

find_package(fmt REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME}
			PRIVATE
			fmt::fmt
#      ${SDL2_LIBS} ${CMAKE_DL_LIBS}
)

# Print current include dirs
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#  message(STATUS "dir='${dir}'")
#endforeach()

# Print linking libraries
#message("Libraries: ${SDL2_LIBS}")

# Copy SDL2 DLLs to output folder on Windows
#if(WIN32)
#    foreach(DLL ${SDL2_DLLS})
#        add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND
#            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)
#    endforeach()
#endif()