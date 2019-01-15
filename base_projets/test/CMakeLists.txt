# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(Base_Projet)

add_executable(projet  main.cpp)
target_link_libraries(projet ${ALL_LIBS} lib_reconstruction)

add_executable(test_no_graphic test_no_graphic.cpp)
target_link_libraries(test_no_graphic lib_reconstruction ${ALL_LIBS})

add_library(plan_f SHARED Implicit_plan_function.cpp)

add_executable(test_marching_cubes test_marching_cubes.cpp)
target_link_libraries(test_no_graphic lib_reconstruction plan_f ${ALL_LIBS})
