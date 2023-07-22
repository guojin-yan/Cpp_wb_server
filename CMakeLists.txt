# 1.cmake verson，指定cmake版本 
cmake_minimum_required(VERSION 3.0)
# 2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(ygj_server)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include (cmake/utils.cmake)


set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")


include_directories(.)
link_directories()

find_library(YAMLCPP yaml-cpp)
message("***", ${YAMLCPP})



set(LIB_SRC 
	wb_server/config.cc
	wb_server/log.cc
	wb_server/fiber.cc
	wb_server/mutex.cc
	wb_server/scheduler.cc
	wb_server/thread.cc
	wb_server/util.cc
	)

add_library(ygj_server SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(ygj_server)

set(LIB_LIB
	ygj_server
	pthread
	${YAMLCPP}
	)

#add_library(ygj_sever_static PROPERTIES OUTPUT_NAME "ygj_server")

#add_executable(test tests/test.cc)
#add_dependencies(test ygj_server)
#target_link_libraries(test ygj_server)

#add_executable(test_config tests/test_config.cc)
#add_dependencies(test_config ygj_server)
#force_redefine_file_macro_for_sources(test_config)
#target_link_libraries(test_config ygj_server ${LIB_LIB})


#add_executable(test_thread tests/test_thread.cc)
#add_dependencies(test_thread ygj_server)
#force_redefine_file_macro_for_sources(test_thread)
#target_link_libraries(test_thread ygj_server ${LIB_LIB})

#add_executable(test_util tests/test_util.cc)
#add_dependencies(test_util ygj_server)
#force_redefine_file_macro_for_sources(test_util)
#target_link_libraries(test_util ygj_server ${LIB_LIB})

#add_executable(test_fiber tests/test_fiber.cc)
#add_dependencies(test_fiber ygj_server)
#force_redefine_file_macro_for_sources(test_fiber)
#target_link_libraries(test_fiber ygj_server ${LIB_LIB})

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler ygj_server)
force_redefine_file_macro_for_sources(test_scheduler)
target_link_libraries(test_scheduler ygj_server ${LIB_LIB})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)