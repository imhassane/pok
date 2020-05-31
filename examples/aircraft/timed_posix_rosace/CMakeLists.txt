cmake_minimum_required(VERSION 2.8.11)
project(ROSACE_Posix C)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

# Find external libraries
find_library(MATH_LIBRARY m)
find_package(Threads REQUIRED)

add_executable(rosace_posix
  rosace.c
  io.c io.h
  assemblage_includes.c assemblage_includes.h
  assemblage.c assemblage.h
  threads.c threads.h
  types.h)

add_executable(rosace_posix-float
  rosace.c
  io.c io.h
  assemblage_includes.c assemblage_includes.h
  assemblage.c assemblage.h
  threads.c threads.h
  types.h)
set_target_properties(rosace_posix-float
  PROPERTIES COMPILE_FLAGS -DUSE_FLOAT)

if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC rosace_posix "-pthread")
  target_compile_options(PUBLIC rosace_posix-float "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(rosace_posix "${CMAKE_THREAD_LIBS_INIT}")
  target_link_libraries(rosace_posix-float "${CMAKE_THREAD_LIBS_INIT}")
endif()
target_link_libraries(rosace_posix ${MATH_LIBRARY})
target_link_libraries(rosace_posix-float ${MATH_LIBRARY})
