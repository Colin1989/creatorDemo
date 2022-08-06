cmake_minimum_required(VERSION 3.8)

option(APP_NAME "Project Name" "SpineTest")
project(${APP_NAME} CXX)
set(CC_LIB_NAME cocos)
set(CC_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CC_PROJ_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes/Game.h
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes/Game.cpp
 	${CMAKE_CURRENT_LIST_DIR}/../common/Classes/jsb_spine_cloth.h
    ${CMAKE_CURRENT_LIST_DIR}/../common/Classes/jsb_spine_cloth.cpp
)
set(CC_COMMON_SOURCES)
set(CC_ALL_SOURCES)

include(${CMAKE_CURRENT_LIST_DIR}/../common/CMakeLists.txt)

cc_android_before_target(${CC_LIB_NAME})
add_library(${CC_LIB_NAME} SHARED ${CC_ALL_SOURCES})
cc_android_after_target(${CC_LIB_NAME})

