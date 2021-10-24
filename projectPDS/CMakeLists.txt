cmake_minimum_required(VERSION 3.17)
project(projectPDS)

find_package(Boost 1.76.0  )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()



set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(projectPDS main.cpp Node.hpp BayesianNetwork.hpp RealVector.cpp Matrix.cpp ThreadPool.cpp pugiconfig.hpp pugixml.cpp pugixml.hpp)
target_link_libraries (projectPDS ${Boost_LIBRARIES})