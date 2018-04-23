#
# CMake for clgm2
#

# global
cmake_minimum_required(VERSION 3.0)
set(APP_NAME clgm2)

# directory
set(DIR_UWS ${PROJECT_SOURCE_DIR}/lib/uWebSockets)
set(DIR_MRUBY ${PROJECT_SOURCE_DIR}/lib/mruby/)
set(DIR_PARSON ${PROJECT_SOURCE_DIR}/lib/mruby/build/mrbgems/mruby-json/)

# environment
if(NOT DEFINED ENV{CLGM_ENV})
  set(ENV{CLGM_ENV} "development")
  add_definitions(-g3 -Wall)
else()
  add_definitions(-O3 -Wall)
endif()

# include directory
set(INCLUDE_DIRECTORIES
  ${PROJECT_SOURCE_DIR}/include
  ${DIR_MRUBY}/include
  ${DIR_UWS}/src
  ${DIR_PARSON}/src
)
include_directories(${INCLUDE_DIRECTORIES})

# c_flags and cpp_flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# build settings of each platform
if(APPLE)
  include_directories(/usr/local/opt/openssl/include)
  link_directories(/usr/local/opt/openssl/lib)
endif()

# mruby
add_custom_target(mruby
  ${CMAKE_COMMAND} -E env CLGM_ENV=$ENV{CLGM_ENV} ruby minirake
  WORKING_DIRECTORY ${DIR_MRUBY} 
)

# uWS
file(GLOB SOURCES ${DIR_UWS}/src/*.cpp)
add_library(uWS STATIC ${SOURCES})

# main
file(GLOB SOURCES src/*.cpp src/core/*.cpp src/util/*.cpp src/glue/*.cpp)
add_executable(${APP_NAME} ${SOURCES})
add_dependencies(${APP_NAME} uWS mruby)

# link
target_link_libraries(${APP_NAME} ${CMAKE_SOURCE_DIR}/build/libuWS.a ${DIR_MRUBY}/build/clgm2/lib/libmruby.a ssl crypto z uv pthread msgpackc yaml)