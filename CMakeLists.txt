cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

project(smartwatch)

option(MENU_TEST OFF "")
option(MENU_EXAMPLE OFF "")
add_subdirectory("./third_party/csmenu")

find_package(PkgConfig REQUIRED)
pkg_search_module(BLEPP REQUIRED libblepp)

add_executable(smartwatch main.cc)
target_include_directories(smartwatch PUBLIC ${BLEPP_INCLUDE_DIRS})
target_link_libraries(smartwatch menu-lib ${BLEPP_LINK_LIBRARIES} bluetooth)