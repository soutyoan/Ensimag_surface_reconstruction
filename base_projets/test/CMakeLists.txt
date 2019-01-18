# CMake entry point
cmake_minimum_required (VERSION 2.6)
project(Base_Projet)

add_executable(test_no_graphic test_no_graphic.cpp)
target_link_libraries(test_no_graphic lib_reconstruction ${ALL_LIBS})

add_executable(test_export_sphere test_export_sphere.cpp)
target_link_libraries(test_export_sphere lib_reconstruction ${ALL_LIBS})

add_library(plan_f SHARED Implicit_plan_function.cpp)

# add_executable(test_marching_cubes test_marching_cubes.cpp)
# target_link_libraries(test_marching_cubes lib_reconstruction plan_f ${ALL_LIBS})
