# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(Base_Projet)


# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_subdirectory (./external)

include_directories(src/)

# include_directories(
#     external/Eigen/
#     external/LBFGS/
# )

include_directories(
        external/glfw-2.7.6/include/
        external/glm-0.9.4.0/
        external/glew-1.9.0/include/
)

set(ALL_LIBS
	  GLFW_276
    GLEW_190
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS

)


## EIGEN ###
include_directories(external) # Include Eigen as <Eigen/file>

file(
    GLOB
    shader_file
    shader/*.glsl
    )

file(
    GLOB
    source_files
    src/*.cpp
    src/*.h
)

add_library(lib_reconstruction SHARED ${shader_file} ${source_files})

add_subdirectory(test)
add_subdirectory(release)
