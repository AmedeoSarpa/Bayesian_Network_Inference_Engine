cmake_minimum_required(VERSION 3.19)
project(projectPDS)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.76.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(projectPDS main.cpp Node.cpp)
target_link_libraries (projectPDS ${Boost_LIBRARIES})