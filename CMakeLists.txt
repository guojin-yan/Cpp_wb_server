# 1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.0)
# 2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(ygj_server)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC 
	wb_server/log.cc
	)

add_library(ygj_server SHARED ${LIB_SRC})
#add_library(ygj_sever_static PROPERTIES OUTPUT_NAME "ygj_server")

add_executable(test tests/test.cc)
add_dependencies(test ygj_server)
target_link_libraries(test ygj_server)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)