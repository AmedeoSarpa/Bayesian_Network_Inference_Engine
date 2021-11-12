cmake_minimum_required(VERSION 3.17)
project(projectPDS)

find_package(Boost 1.76.0  )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(projectPDS main.cpp Node.cpp BayesianNetwork.cpp RealVector.hpp Matrix.hpp ThreadPool.cpp pugixml_library/pugiconfig.hpp pugixml_library/pugixml.cpp pugixml_library/pugixml.hpp)
target_link_libraries (projectPDS ${Boost_LIBRARIES})