# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(Base_Projet)

add_executable(projet  main.cpp)
target_link_libraries(projet ${ALL_LIBS} lib_reconstruction)
